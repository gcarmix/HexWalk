#include <QApplication>
#include <QClipboard>
#include <QKeyEvent>
#include <QPainter>
#include <QScrollBar>
#include <QMenu>

#include "qhexedit.h"
#include <algorithm>

// ********************************************************************** Text area decoding

// Decodes the UTF-8 sequence starting at in (avail bytes readable). Returns the
// length of the sequence and sets cp to the decoded codepoint. On a malformed or
// truncated sequence 1 is returned and cp is set to 0, so decoding resyncs on
// the next byte instead of swallowing valid data behind a bad byte.
static int decodeUtf8(const uchar *in, int avail, uint &cp)
{
    uchar b0 = in[0];
    int len;
    uint min;

    if (b0 < 0x80)                  { cp = b0; return 1; }
    else if ((b0 & 0xE0) == 0xC0)   { len = 2; cp = b0 & 0x1F; min = 0x80; }
    else if ((b0 & 0xF0) == 0xE0)   { len = 3; cp = b0 & 0x0F; min = 0x800; }
    else if ((b0 & 0xF8) == 0xF0)   { len = 4; cp = b0 & 0x07; min = 0x10000; }
    else                            { cp = 0; return 1; }   // continuation or invalid lead

    if (avail < len)                { cp = 0; return 1; }
    for (int i = 1; i < len; i++)
    {
        if ((in[i] & 0xC0) != 0x80) { cp = 0; return 1; }   // missing continuation
        cp = (cp << 6) | (in[i] & 0x3F);
    }
    // reject overlong encodings, surrogates and out of range values
    if (cp < min || cp > 0x10FFFF || QChar::isSurrogate(cp))
                                    { cp = 0; return 1; }
    return len;
}

// Decodes the UTF-16 sequence starting at in (avail bytes readable). Works like
// decodeUtf8(): returns the length in bytes and sets cp, or sets cp to 0 for an
// unpaired surrogate or a code unit truncated by the end of the data. Unlike
// UTF-8 this encoding does not resynchronize by itself, the caller has to keep
// the decoding anchored on even file offsets.
static int decodeUtf16(const uchar *in, int avail, bool bigEndian, uint &cp)
{
    if (avail < 2)                  { cp = 0; return 1; }
    uint u1 = bigEndian ? (in[0] << 8 | in[1]) : (in[1] << 8 | in[0]);

    if (u1 < 0xD800 || u1 > 0xDFFF) { cp = u1; return 2; }   // plain BMP code unit
    if (u1 > 0xDBFF)                { cp = 0; return 2; }    // low surrogate without a high one
    if (avail < 4)                  { cp = 0; return 2; }    // high surrogate, pair truncated

    uint u2 = bigEndian ? (in[2] << 8 | in[3]) : (in[3] << 8 | in[2]);
    if (u2 < 0xDC00 || u2 > 0xDFFF) { cp = 0; return 2; }    // high surrogate not followed by a low one

    cp = 0x10000 + ((u1 - 0xD800) << 10) + (u2 - 0xDC00);
    return 4;
}

// A codepoint is only drawn when it occupies its own cell. Control characters,
// unassigned and format codepoints have no glyph, combining marks would be
// painted onto the neighbouring cell - all of them are shown as a dot instead.
static bool isRenderable(uint cp)
{
    if (cp < 0x20 || cp == 0x7f)
        return false;                                       // C0 and DEL
    if (cp >= 0x80 && cp <= 0x9f)
        return false;                                       // C1
    switch (QChar::category(cp))
    {
        case QChar::Other_Control:
        case QChar::Other_Format:
        case QChar::Other_Surrogate:
        case QChar::Other_PrivateUse:
        case QChar::Other_NotAssigned:
        case QChar::Mark_NonSpacing:
        case QChar::Mark_SpacingCombining:
        case QChar::Mark_Enclosing:
            return false;
        default:
            return true;
    }
}

// QString::fromUcs4() takes a const uint* on Qt5 and a const char32_t* on Qt6,
// so build the string by hand to stay portable between both.
static QString cpToString(uint cp)
{
    if (QChar::requiresSurrogates(cp))
        return QString(QChar(QChar::highSurrogate(cp))) + QChar(QChar::lowSurrogate(cp));
    return QString(QChar((ushort)cp));
}

// ********************************************************************** Constructor, destructor

QHexEdit::QHexEdit(QWidget *parent) : QAbstractScrollArea(parent)
    , _addressArea(true)
    , _addressWidth(4)
    , _asciiArea(true)
    , _charEncoding(EncodingAscii)
    , _bytesPerLine(16)
    , _hexCharsInLine(47)
    , _highlighting(true)
    , _overwriteMode(true)
    , _readOnly(false)
    , _hexCaps(false)
    , _dynamicBytesPerLine(false)
    , _editAreaIsAscii(false)
    , _chunks(new Chunks(this))
    , _cursorPosition(0)
    , _lastEventSize(0)
    , _undoStack(new UndoStack(_chunks, this))
{
#ifdef Q_OS_WIN32
    setFont(QFont("Courier", 10));
#else
    setFont(QFont("Monospace", 10));
#endif
    setAddressAreaColor(this->palette().alternateBase().color());
    setHighlightingColor(QColor(0xff, 0xff, 0x99, 0xff));
    setSelectionColor(this->palette().highlight().color());
    setAddressFontColor(QPalette::WindowText);
    setAsciiAreaColor(this->palette().alternateBase().color());
    setAsciiFontColor(QPalette::WindowText);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QAbstractScrollArea::customContextMenuRequested, this, &QHexEdit::showContextMenu);
    connect(&_cursorTimer, &QTimer::timeout, this, &QHexEdit::updateCursor);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &QHexEdit::adjust);
    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &QHexEdit::adjust);
    connect(_undoStack, &QUndoStack::indexChanged, this, &QHexEdit::dataChangedPrivate);

    _cursorTimer.setInterval(500);
    _cursorTimer.start();
    _scrollMult = 1;
    setAddressWidth(4);
    setAddressArea(true);
    setAsciiArea(true);
    setOverwriteMode(true);
    setHighlighting(true);
    setReadOnly(false);

    init();

}

QHexEdit::~QHexEdit()
{
}

// ********************************************************************** Properties

void QHexEdit::setAddressArea(bool addressArea)
{
    _addressArea = addressArea;
    adjust();
    setCursorPosition(_cursorPosition);
    viewport()->update();
}

bool QHexEdit::addressArea()
{
    return _addressArea;
}

void QHexEdit::setAddressAreaColor(const QColor &color)
{
    _addressAreaColor = color;
    viewport()->update();
}

QColor QHexEdit::addressAreaColor()
{
    return _addressAreaColor;
}

void QHexEdit::setAddressFontColor(const QColor &color)
{
    _addressFontColor = color;
    viewport()->update();
}

QColor QHexEdit::addressFontColor()
{
    return _addressFontColor;
}

void QHexEdit::setAsciiAreaColor(const QColor &color)
{
    _asciiAreaColor = color;
    viewport()->update();
}

QColor QHexEdit::asciiAreaColor()
{
    return _asciiAreaColor;
}

void QHexEdit::setAsciiFontColor(const QColor &color)
{
    _asciiFontColor = color;
    viewport()->update();
}

QColor QHexEdit::asciiFontColor()
{
    return _asciiFontColor;
}

void QHexEdit::setHexFontColor(const QColor &color)
{
    _hexFontColor = color;
    viewport()->update();
}

QColor QHexEdit::hexFontColor()
{
    return _hexFontColor;
}

void QHexEdit::setAddressOffset(qint64 addressOffset)
{
    _addressOffset = addressOffset;
    adjust();
    setCursorPosition(_cursorPosition);
    viewport()->update();
}

qint64 QHexEdit::addressOffset()
{
    return _addressOffset;
}

void QHexEdit::setAddressWidth(int addressWidth)
{
    _addressWidth = addressWidth;
    adjust();
    setCursorPosition(_cursorPosition);
    viewport()->update();
}

qint64 QHexEdit::getSize()
{
    return _chunks->size();
}

int QHexEdit::contentWidth()
{
    // same width adjust() feeds to the horizontal scrollbar
    int width = _pxPosAsciiX;
    if (_asciiArea)
        width += _bytesPerLine * _pxCharWidth;
    return width;
}
int QHexEdit::addressWidth()
{
    qint64 size = _chunks->size();
    int n = 1;
    if (size > Q_INT64_C(0x100000000)){ n += 8; size /= Q_INT64_C(0x100000000);}
    if (size > 0x10000){ n += 4; size /= 0x10000;}
    if (size > 0x100){ n += 2; size /= 0x100;}
    if (size > 0x10){ n += 1;}

    if (n > _addressWidth)
        return n;
    else
        return _addressWidth;
}

void QHexEdit::setAsciiArea(bool asciiArea)
{
    if (!asciiArea)
        _editAreaIsAscii = false;
    _asciiArea = asciiArea;
    adjust();
    setCursorPosition(_cursorPosition);
    viewport()->update();
}

bool QHexEdit::asciiArea()
{
    return _asciiArea;
}

void QHexEdit::setCharEncoding(CharEncoding encoding)
{
    if (_charEncoding == encoding)
        return;
    _charEncoding = encoding;
    buildCharMap();
    viewport()->update();
}

QHexEdit::CharEncoding QHexEdit::charEncoding()
{
    return _charEncoding;
}

QHexEdit::CharEncoding QHexEdit::charEncodingFromInt(int value)
{
    switch (value)
    {
        case EncodingUtf8:      return EncodingUtf8;
        case EncodingLatin1:    return EncodingLatin1;
        case EncodingUtf16LE:   return EncodingUtf16LE;
        case EncodingUtf16BE:   return EncodingUtf16BE;
        default:                return EncodingAscii;
    }
}

void QHexEdit::setBytesPerLine(int count)
{
    _bytesPerLine = count;
    _hexCharsInLine = count * 3 - 1;

    adjust();
    setCursorPosition(_cursorPosition);
    viewport()->update();
}

int QHexEdit::bytesPerLine()
{
    return _bytesPerLine;
}

qint64 QHexEdit::firstByteAddress()
{
    return _bPosFirst;
}

void QHexEdit::setCursorPosition(qint64 position)
{
    // 1. delete old cursor
    _blink = false;
    viewport()->update(_cursorRect);
    viewport()->update(_cursorRect2);

    // 2. Check, if cursor in range?
    if (position > (_chunks->size() * 2 - 1))
        position = _chunks->size() * 2  - (_overwriteMode ? 1 : 0);

    if (position < 0)
        position = 0;

    // 3. Calc new position of cursor
    _bPosCurrent = position / 2;
    _pxCursorY = ((position / 2 - _bPosFirst) / _bytesPerLine + 1) * _pxCharHeight;
    int x = (position % (2 * _bytesPerLine));
    if (_editAreaIsAscii)
    {
        _pxCursorX = x / 2 * _pxCharWidth + _pxPosAsciiX;
        _pxCursorX2 = (((x / 2) * 3) + (x % 2)) * _pxCharWidth + _pxPosHexX;
        _cursorPosition = position & 0xFFFFFFFFFFFFFFFE;
    } else {
        _pxCursorX = (((x / 2) * 3) + (x % 2)) * _pxCharWidth + _pxPosHexX;
        _pxCursorX2 = x / 2 * _pxCharWidth + _pxPosAsciiX;
        _cursorPosition = position;
    }

    if (_overwriteMode)
    {
        _cursorRect = QRect(_pxCursorX - horizontalScrollBar()->value(), _pxCursorY + _pxCursorWidth, _pxCharWidth, _pxCursorWidth);
        _cursorRect2 = QRect(_pxCursorX2 - horizontalScrollBar()->value(), _pxCursorY + _pxCursorWidth, _pxCharWidth, _pxCursorWidth);
    }
    else
    {
        _cursorRect = QRect(_pxCursorX - horizontalScrollBar()->value(), _pxCursorY - _pxCharHeight + 4, _pxCursorWidth, _pxCharHeight);
        _cursorRect2 = QRect(_pxCursorX2 - horizontalScrollBar()->value(), _pxCursorY + _pxCursorWidth, _pxCharWidth, _pxCursorWidth);
    }

    // 4. Immediately draw new cursor
    _blink = true;
    viewport()->update(_cursorRect);
    viewport()->update(_cursorRect2);
    emit currentAddressChanged(_bPosCurrent);
}

qint64 QHexEdit::cursorPosition(QPoint pos)
{
    // Calc cursor position depending on a graphical position
    qint64 result = -1;
    int posX = pos.x() + horizontalScrollBar()->value();
    int posY = pos.y() - 3;
    if ((posX >= _pxPosHexX) && (posX < (_pxPosHexX + (1 + _hexCharsInLine) * _pxCharWidth)))
    {
        _editAreaIsAscii = false;
        int x = (posX - _pxPosHexX) / _pxCharWidth;
        x = (x / 3) * 2 + x % 3;
        int y = (posY / _pxCharHeight) * 2 * _bytesPerLine;
        result = _bPosFirst * 2 + x + y;
    }
    else
        if (_asciiArea && (posX >= _pxPosAsciiX) && (posX < (_pxPosAsciiX + (1 + _bytesPerLine) * _pxCharWidth)))
        {
            _editAreaIsAscii = true;
            int x = 2 * (posX - _pxPosAsciiX) / _pxCharWidth;
            int y = (posY / _pxCharHeight) * 2 * _bytesPerLine;
            result = _bPosFirst * 2 + x + y;
        }
    return result;
}

qint64 QHexEdit::cursorPosition()
{
    return _cursorPosition;
}

void QHexEdit::setData(const QByteArray &ba)
{
    _data = ba;
    _bData.setData(_data);
    setData(_bData);
}

QByteArray QHexEdit::data()
{
    return _chunks->data(0, -1);
}

void QHexEdit::setHighlighting(bool highlighting)
{
    _highlighting = highlighting;
    viewport()->update();
}

bool QHexEdit::highlighting()
{
    return _highlighting;
}

void QHexEdit::setHighlightingColor(const QColor &color)
{
    _brushHighlighted = QBrush(color);
    _penHighlighted = QPen(viewport()->palette().color(QPalette::WindowText));
    viewport()->update();
}

QColor QHexEdit::highlightingColor()
{
    return _brushHighlighted.color();
}

void QHexEdit::setOverwriteMode(bool overwriteMode)
{
    _overwriteMode = overwriteMode;
    emit overwriteModeChanged(overwriteMode);
}

bool QHexEdit::overwriteMode()
{
    return _overwriteMode;
}

void QHexEdit::setSelectionColor(const QColor &color)
{
    _brushSelection = QBrush(color);
    _penSelection = QPen(Qt::white);
    viewport()->update();
}

QColor QHexEdit::selectionColor()
{
    return _brushSelection.color();
}

bool QHexEdit::isReadOnly()
{
    return _readOnly;
}

void QHexEdit::setReadOnly(bool readOnly)
{
    _readOnly = readOnly;
}

void QHexEdit::setHexCaps(const bool isCaps)
{
    if (_hexCaps != isCaps)
    {
        _hexCaps = isCaps;
        viewport()->update();
    }
}

bool QHexEdit::hexCaps()
{
    return _hexCaps;
}

void QHexEdit::setDynamicBytesPerLine(const bool isDynamic)
{
    _dynamicBytesPerLine = isDynamic;
    resizeEvent(NULL);
}

bool QHexEdit::dynamicBytesPerLine()
{
    return _dynamicBytesPerLine;
}

// ********************************************************************** Access to data of qhexedit
bool QHexEdit::setData(QIODevice &iODevice)
{
    bool ok = _chunks->setIODevice(iODevice);
    init();
    dataChangedPrivate();
    return ok;
}

QByteArray QHexEdit::dataAt(qint64 pos, qint64 count)
{
    return _chunks->data(pos, count);
}

bool QHexEdit::write(QIODevice &iODevice, qint64 pos, qint64 count)
{
    return _chunks->write(iODevice, pos, count);
}

// ********************************************************************** Char handling
void QHexEdit::insert(qint64 index, char ch)
{
    _undoStack->insert(index, ch);
    refresh();
}

void QHexEdit::remove(qint64 index, qint64 len)
{
    _undoStack->removeAt(index, len);
    refresh();
}

void QHexEdit::replace(qint64 index, char ch)
{
    _undoStack->overwrite(index, ch);
    refresh();
}

// ********************************************************************** ByteArray handling
void QHexEdit::insert(qint64 pos, const QByteArray &ba)
{
    _undoStack->insert(pos, ba);
    refresh();
}

void QHexEdit::replace(qint64 pos, qint64 len, const QByteArray &ba)
{
    _undoStack->overwrite(pos, len, ba);
    refresh();
}

// ********************************************************************** Utility functions
void QHexEdit::ensureVisible()
{
    if (_cursorPosition < (_bPosFirst * 2))
        verticalScrollBar()->setValue((int)(_cursorPosition / 2 / _bytesPerLine/_scrollMult));
    if (_cursorPosition > ((_bPosFirst + (qint64)((_rowsShown - 1)*_bytesPerLine) * 2)))
        verticalScrollBar()->setValue((int)(((_cursorPosition / 2 / _bytesPerLine) - _rowsShown + 1)/_scrollMult));
    if (_pxCursorX < horizontalScrollBar()->value())
        horizontalScrollBar()->setValue(_pxCursorX);
    if ((_pxCursorX + _pxCharWidth) > (horizontalScrollBar()->value() + viewport()->width()))
        horizontalScrollBar()->setValue(_pxCursorX + _pxCharWidth - viewport()->width());
    viewport()->update();
}

qint64 QHexEdit::indexOf(const QByteArray &ba, qint64 from, bool isRegex,bool isCaseInsensitive, bool invertMatch)
{
    qint64 pos = _chunks->indexOf(ba, from,isRegex,isCaseInsensitive, invertMatch);    if (pos > -1)
    {
        qint64 curPos = pos*2;
        setCursorPosition(curPos + _chunks->matchSize*2);
        resetSelection(curPos);
        setSelection(curPos + _chunks->matchSize*2);
        ensureVisible();
    }
    return pos;
}

bool QHexEdit::isModified()
{
    return _modified;
}

qint64 QHexEdit::lastIndexOf(const QByteArray &ba, qint64 from)
{
    qint64 pos = _chunks->lastIndexOf(ba, from);
    if (pos > -1)
    {
        qint64 curPos = pos*2;
        setCursorPosition(curPos - 1);
        resetSelection(curPos);
        setSelection(curPos + ba.length()*2);
        ensureVisible();
    }
    return pos;
}

void QHexEdit::redo()
{
    _undoStack->redo();
    setCursorPosition(_chunks->pos()*2);
    refresh();
}

QString QHexEdit::selectionToReadableString()
{
    QByteArray ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin());
    return toReadable(ba);
}

QString QHexEdit::selectedData()
{
    QByteArray ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin()).toHex();
    return ba;
}
QByteArray QHexEdit::selectedDataBa()
{
    QByteArray ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin());
    return ba;
}


void QHexEdit::setFont(const QFont &font)
{
    QFont theFont(font);
    theFont.setStyleHint(QFont::Monospace);
    QWidget::setFont(theFont);
    QFontMetrics metrics = fontMetrics();
    _pxCharWidth = metrics.horizontalAdvance(QLatin1Char('A'))+1;
    _pxCharHeight = metrics.height();
    _pxGapAdr = _pxCharWidth / 2;
    _pxGapAdrHex = _pxCharWidth;
    _pxGapHexAscii = 2 * _pxCharWidth;
    _pxCursorWidth = _pxCharHeight / 7;
    _pxSelectionSub = _pxCharHeight / 5;
    viewport()->update();
}

QString QHexEdit::toReadableString()
{
    QByteArray ba = _chunks->data();
    return toReadable(ba);
}

void QHexEdit::undo()
{
    _undoStack->undo();
    setCursorPosition(_chunks->pos()*2);
    refresh();
}

// ********************************************************************** Handle events
void QHexEdit::keyPressEvent(QKeyEvent *event)
{
    // Cursor movements
    if (event->matches(QKeySequence::MoveToNextChar))
    {
        qint64 pos = _cursorPosition + 1;
        if (_editAreaIsAscii)
            pos += 1;
        setCursorPosition(pos);
        resetSelection(pos);
    }
    if (event->matches(QKeySequence::MoveToPreviousChar))
    {
        qint64 pos = _cursorPosition - 1;
        if (_editAreaIsAscii)
            pos -= 1;
        setCursorPosition(pos);
        resetSelection(pos);
    }
    if (event->matches(QKeySequence::MoveToEndOfLine))
    {
        qint64 pos = _cursorPosition - (_cursorPosition % (2 * _bytesPerLine)) + (2 * _bytesPerLine) - 1;
        setCursorPosition(pos);
        resetSelection(_cursorPosition);
    }
    if (event->matches(QKeySequence::MoveToStartOfLine))
    {
        qint64 pos = _cursorPosition - (_cursorPosition % (2 * _bytesPerLine));
        setCursorPosition(pos);
        resetSelection(_cursorPosition);
    }
    if (event->matches(QKeySequence::MoveToPreviousLine))
    {
        setCursorPosition(_cursorPosition - (2 * _bytesPerLine));
        resetSelection(_cursorPosition);
    }
    if (event->matches(QKeySequence::MoveToNextLine))
    {
        setCursorPosition(_cursorPosition + (2 * _bytesPerLine));
        resetSelection(_cursorPosition);
    }
    if (event->matches(QKeySequence::MoveToNextPage))
    {
        setCursorPosition(_cursorPosition + (((_rowsShown - 1) * 2 * _bytesPerLine)));
        resetSelection(_cursorPosition);
    }
    if (event->matches(QKeySequence::MoveToPreviousPage))
    {
        setCursorPosition(_cursorPosition - (((_rowsShown - 1) * 2 * _bytesPerLine)));
        resetSelection(_cursorPosition);
    }
    if (event->matches(QKeySequence::MoveToEndOfDocument))
    {
        setCursorPosition(_chunks->size() * 2 );
        resetSelection(_cursorPosition);
    }
    if (event->matches(QKeySequence::MoveToStartOfDocument))
    {
        setCursorPosition(0);
        resetSelection(_cursorPosition);
    }

    // Select commands
    if (event->matches(QKeySequence::SelectAll))
    {
        resetSelection(0);
        setSelection(2 * _chunks->size() + 1);
    }
    if (event->matches(QKeySequence::SelectNextChar))
    {
        qint64 pos = _cursorPosition + 1;
        if (_editAreaIsAscii)
            pos += 1;
        setCursorPosition(pos);
        setSelection(pos);
    }
    if (event->matches(QKeySequence::SelectPreviousChar))
    {
        qint64 pos = _cursorPosition - 1;
        if (_editAreaIsAscii)
            pos -= 1;
        setSelection(pos);
        setCursorPosition(pos);
    }
    if (event->matches(QKeySequence::SelectEndOfLine))
    {
        qint64 pos = _cursorPosition - (_cursorPosition % (2 * _bytesPerLine)) + (2 * _bytesPerLine) - 1;
        setCursorPosition(pos);
        setSelection(pos);
    }
    if (event->matches(QKeySequence::SelectStartOfLine))
    {
        qint64 pos = _cursorPosition - (_cursorPosition % (2 * _bytesPerLine));
        setCursorPosition(pos);
        setSelection(pos);
    }
    if (event->matches(QKeySequence::SelectPreviousLine))
    {
        qint64 pos = _cursorPosition - (2 * _bytesPerLine);
        setCursorPosition(pos);
        setSelection(pos);
    }
    if (event->matches(QKeySequence::SelectNextLine))
    {
        qint64 pos = _cursorPosition + (2 * _bytesPerLine);
        setCursorPosition(pos);
        setSelection(pos);
    }
    if (event->matches(QKeySequence::SelectNextPage))
    {
        qint64 pos = _cursorPosition + (((viewport()->height() / _pxCharHeight) - 1) * 2 * _bytesPerLine);
        setCursorPosition(pos);
        setSelection(pos);
    }
    if (event->matches(QKeySequence::SelectPreviousPage))
    {
        qint64 pos = _cursorPosition - (((viewport()->height() / _pxCharHeight) - 1) * 2 * _bytesPerLine);
        setCursorPosition(pos);
        setSelection(pos);
    }
    if (event->matches(QKeySequence::SelectEndOfDocument))
    {
        qint64 pos = _chunks->size() * 2;
        setCursorPosition(pos);
        setSelection(pos);
    }
    if (event->matches(QKeySequence::SelectStartOfDocument))
    {
        qint64 pos = 0;
        setCursorPosition(pos);
        setSelection(pos);
    }

    // Edit Commands
    if (!_readOnly)
    {
        /* Cut */
        if (event->matches(QKeySequence::Cut))
        {
            QByteArray ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin()).toHex();
            for (qint64 idx = 32; idx < ba.size(); idx +=33)
                ba.insert(idx, "\n");
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(ba);
            if (_overwriteMode)
            {
                qint64 len = getSelectionEnd() - getSelectionBegin();
                replace(getSelectionBegin(), (int)len, QByteArray((int)len, char(0)));
            }
            else
            {
                remove(getSelectionBegin(), getSelectionEnd() - getSelectionBegin());
            }
            setCursorPosition(2 * getSelectionBegin());
            resetSelection(2 * getSelectionBegin());
        } else

        /* Paste */
        if (event->matches(QKeySequence::Paste))
        {
            QClipboard *clipboard = QApplication::clipboard();
            QByteArray ba = QByteArray().fromHex(clipboard->text().toLatin1());
            if (_overwriteMode)
            {
                ba = ba.left(std::min<qint64>(ba.size(), (_chunks->size() - _bPosCurrent)));
                replace(_bPosCurrent, ba.size(), ba);
            }
            else
                insert(_bPosCurrent, ba);
            setCursorPosition(_cursorPosition + 2 * ba.size());
            resetSelection(getSelectionBegin());
        } else

        /* Delete char */
        if (event->matches(QKeySequence::Delete))
        {
            if (getSelectionBegin() != getSelectionEnd())
            {
                _bPosCurrent = getSelectionBegin();
                if (_overwriteMode)
                {
                    QByteArray ba = QByteArray(getSelectionEnd() - getSelectionBegin(), char(0));
                    replace(_bPosCurrent, ba.size(), ba);
                }
                else
                {
                    remove(_bPosCurrent, getSelectionEnd() - getSelectionBegin());
                }
            }
            else
            {
                if (_overwriteMode)
                    replace(_bPosCurrent, char(0));
                else
                    remove(_bPosCurrent, 1);
            }
            setCursorPosition(2 * _bPosCurrent);
            resetSelection(2 * _bPosCurrent);
        } else

        /* Backspace */
        if ((event->key() == Qt::Key_Backspace) && (event->modifiers() == Qt::NoModifier))
        {
            if (getSelectionBegin() != getSelectionEnd())
            {
                _bPosCurrent = getSelectionBegin();
                setCursorPosition(2 * _bPosCurrent);
                if (_overwriteMode)
                {
                    QByteArray ba = QByteArray(getSelectionEnd() - getSelectionBegin(), char(0));
                    replace(_bPosCurrent, ba.size(), ba);
                }
                else
                {
                    remove(_bPosCurrent, getSelectionEnd() - getSelectionBegin());
                }
                resetSelection(2 * _bPosCurrent);
            }
            else
            {
                bool behindLastByte = false;
                if ((_cursorPosition / 2) == _chunks->size())
                    behindLastByte = true;

                _bPosCurrent -= 1;
                if (_overwriteMode)
                    replace(_bPosCurrent, char(0));
                else
                    remove(_bPosCurrent, 1);

                if (!behindLastByte)
                    _bPosCurrent -= 1;

                setCursorPosition(2 * _bPosCurrent);
                resetSelection(2 * _bPosCurrent);
            }
        } else

        /* undo */
        if (event->matches(QKeySequence::Undo))
        {
            undo();
        } else

        /* redo */
        if (event->matches(QKeySequence::Redo))
        {
            redo();
        } else

        if ((QApplication::keyboardModifiers() == Qt::NoModifier) ||
            (QApplication::keyboardModifiers() == Qt::KeypadModifier) ||
            (QApplication::keyboardModifiers() == Qt::ShiftModifier) ||
            (QApplication::keyboardModifiers() == (Qt::AltModifier | Qt::ControlModifier)) ||
            (QApplication::keyboardModifiers() == Qt::GroupSwitchModifier))
        {
            /* Hex and ascii input */
            int key = 0;
            QString text = event->text();
            if (!text.isEmpty())
            {
                if (_editAreaIsAscii)
                    key = (uchar)text.at(0).toLatin1();
                else
                    key = int(text.at(0).toLower().toLatin1());
            }

            if ((((key >= '0' && key <= '9') || (key >= 'a' && key <= 'f')) && _editAreaIsAscii == false)
                || (key >= ' ' && _editAreaIsAscii))
            {
                if (getSelectionBegin() != getSelectionEnd())
                {
                    if (_overwriteMode)
                    {
                        qint64 len = getSelectionEnd() - getSelectionBegin();
                        replace(getSelectionBegin(), (int)len, QByteArray((int)len, char(0)));
                    } else
                    {
                        remove(getSelectionBegin(), getSelectionEnd() - getSelectionBegin());
                        _bPosCurrent = getSelectionBegin();
                    }
                    setCursorPosition(2 * _bPosCurrent);
                    resetSelection(2 * _bPosCurrent);
                }

                // If insert mode, then insert a byte
                if (_overwriteMode == false)
                    if ((_cursorPosition % 2) == 0)
                        insert(_bPosCurrent, char(0));

                // Change content
                if (_chunks->size() > 0)
                {
                    char ch = key;
                    if (!_editAreaIsAscii){
                        QByteArray hexValue = _chunks->data(_bPosCurrent, 1).toHex();
                        if ((_cursorPosition % 2) == 0)
                            hexValue[0] = key;
                        else
                            hexValue[1] = key;
                        ch = QByteArray().fromHex(hexValue)[0];
                    }
                    replace(_bPosCurrent, ch);
                    if (_editAreaIsAscii)
                        setCursorPosition(_cursorPosition + 2);
                    else
                        setCursorPosition(_cursorPosition + 1);
                    resetSelection(_cursorPosition);
                }
            }
        }


    }

    /* Copy */
    if (event->matches(QKeySequence::Copy))
    {
        QByteArray ba;
        if(!_editAreaIsAscii)
        {
        ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin()).toHex();
        for (qint64 idx = 32; idx < ba.size(); idx +=33)
        ba.insert(idx, "\n");
        }
        else
        {
            ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin());
            for (int i = 0; i < ba.length(); i++) {
                if(ba.at(i) < 32 || ba.at(i) > 126)
                {
                    ba[i] = '.';
                }
            }
        }


        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(ba);
    }

    // Switch between insert/overwrite mode
    if ((event->key() == Qt::Key_Insert) && (event->modifiers() == Qt::NoModifier))
    {
        setOverwriteMode(!overwriteMode());
        setCursorPosition(_cursorPosition);
    }

    // switch from hex to ascii edit
    if (event->key() == Qt::Key_Tab && !_editAreaIsAscii){
        _editAreaIsAscii = true;
        setCursorPosition(_cursorPosition);
    }

    // switch from ascii to hex edit
    if (event->key() == Qt::Key_Backtab  && _editAreaIsAscii){
        _editAreaIsAscii = false;
        setCursorPosition(_cursorPosition);
    }

    refresh();
    QAbstractScrollArea::keyPressEvent(event);
}

void QHexEdit::mouseMoveEvent(QMouseEvent * event)
{
    _blink = false;
    viewport()->update();
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qint64 actPos = cursorPosition(event->position().toPoint());
#else
    qint64 actPos = cursorPosition(event->pos());
#endif
    if (actPos >= 0)
    {
        setCursorPosition(actPos);
        setSelection(actPos);
    }
}

void QHexEdit::mousePressEvent(QMouseEvent * event)
{
    _blink = false;
    viewport()->update();
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qint64 cPos = cursorPosition(event->position().toPoint());
#else
    qint64 cPos = cursorPosition(event->pos());
#endif
    if (cPos >= 0)
    {
        if (event->button() != Qt::RightButton)
        {
            if (event->modifiers() & Qt::ShiftModifier)
                setSelection(cPos);
            else
                resetSelection(cPos);
        }
        setCursorPosition(cPos);
    }
}

void QHexEdit::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());
    int pxOfsX = horizontalScrollBar()->value();

    if (event->rect() != _cursorRect)
    {
        int pxPosStartY = _pxCharHeight;

        // draw some patterns if needed
        painter.fillRect(event->rect(), viewport()->palette().color(QPalette::Base));
        if (_addressArea)
        {
            painter.fillRect(QRect(-pxOfsX, event->rect().top(), _pxPosHexX - _pxGapAdrHex/2, height()), _addressAreaColor);
        }
        if (_asciiArea)
        {
            int linePos = _pxPosAsciiX - (_pxGapHexAscii / 2);
            painter.setPen(Qt::gray);
            painter.drawLine(linePos - pxOfsX, event->rect().top(), linePos - pxOfsX, height());
        }

        painter.setPen(viewport()->palette().color(QPalette::WindowText));

        // paint address area
        if (_addressArea)
        {
            QString address;
            for (int row=0, pxPosY = _pxCharHeight; row <= (_dataShown.size()/_bytesPerLine); row++, pxPosY +=_pxCharHeight)
            {
                address = QString("%1").arg(_bPosFirst + row*_bytesPerLine + _addressOffset, _addrDigits, 16, QChar('0'));
                painter.setPen(QPen(_addressFontColor));
                painter.drawText(_pxPosAdrX - pxOfsX, pxPosY, hexCaps() ? address.toUpper() : address);
            }
        }

        // paint hex and ascii area
        QPen colStandard = QPen(viewport()->palette().color(QPalette::WindowText));

        painter.setBackgroundMode(Qt::TransparentMode);

        for (int row = 0, pxPosY = pxPosStartY; row <= _rowsShown; row++, pxPosY +=_pxCharHeight)
        {
            QByteArray hex;
            int pxPosX = _pxPosHexX  - pxOfsX;
            int pxPosAsciiX2 = _pxPosAsciiX  - pxOfsX;
            qint64 bPosLine = row * _bytesPerLine;
            for (int colIdx = 0; ((bPosLine + colIdx) < _dataShown.size() && (colIdx < _bytesPerLine)); colIdx++)
            {
                QColor c = viewport()->palette().color(QPalette::Base);
                painter.setPen(QPen(_hexFontColor));

                qint64 posBa = _bPosFirst + bPosLine + colIdx;
                if ((getSelectionBegin() <= posBa) && (getSelectionEnd() > posBa))
                {
                    c = _brushSelection.color();
                    painter.setPen(_penSelection);
                }
                else
                {
                    if (_highlighting)
                        if (_markedShown.at((int)(posBa - _bPosFirst)))
                        {
                            c = _brushHighlighted.color();
                            painter.setPen(_penHighlighted);
                        }
                }

                // render hex value
                QRect r;
                if (colIdx == 0)
                    r.setRect(pxPosX, pxPosY - _pxCharHeight + _pxSelectionSub, 2*_pxCharWidth, _pxCharHeight);
                else
                    r.setRect(pxPosX - _pxCharWidth, pxPosY - _pxCharHeight + _pxSelectionSub, 3*_pxCharWidth, _pxCharHeight);
                painter.fillRect(r,c);

                QRect tagrect;
                if(colorTag)
                {
                    for(int i=0;i<colorTag->size();i++)
                    {
                        ColorTag  tag0 = colorTag->at(i);
                        if(posBa >= tag0.pos && posBa < (tag0.pos + tag0.size))
                        {
                            if (colIdx == 0)
                                tagrect.setRect(pxPosX, pxPosY - _pxCharHeight + _pxSelectionSub, 2*_pxCharWidth, _pxCharHeight);
                            else
                                tagrect.setRect(pxPosX - _pxCharWidth, pxPosY - _pxCharHeight + _pxSelectionSub, 3*_pxCharWidth, _pxCharHeight);
                            QColor tempColor = QColor(QString::fromStdString(tag0.color));
                            tempColor.setAlpha(80);
                            painter.fillRect(tagrect, QBrush(tempColor));
                        }
                    }
                }

                hex = _hexDataShown.mid((bPosLine + colIdx) * 2, 2);
                painter.drawText(pxPosX, pxPosY, hexCaps()?hex.toUpper():hex);
                pxPosX += 3*_pxCharWidth;

                // render ascii value
                if (_asciiArea)
                {
                    if (c == viewport()->palette().color(QPalette::Base))
                        c = _asciiAreaColor;
                    r.setRect(pxPosAsciiX2, pxPosY - _pxCharHeight + _pxSelectionSub, _pxCharWidth, _pxCharHeight);
                    painter.fillRect(r, c);     // one fill per byte, selection stays byte exact
                    uint cp = _cpShown.at(bPosLine + colIdx);
                    if (cp)
                    {
                        // a multi byte character may use the cells of its
                        // continuation bytes, but never leave the current row
                        int span = qMin((int)_spanShown.at(bPosLine + colIdx), _bytesPerLine - colIdx);
                        painter.save();
                        painter.setClipRect(pxPosAsciiX2, pxPosY - _pxCharHeight + _pxSelectionSub,
                                            span * _pxCharWidth, _pxCharHeight);
                        painter.setPen(QPen(_asciiFontColor));
                        painter.drawText(pxPosAsciiX2, pxPosY, cpToString(cp));
                        painter.restore();
                    }
                    pxPosAsciiX2 += _pxCharWidth;
                }
            }
        }
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setPen(viewport()->palette().color(QPalette::WindowText));
    }

    // _cursorPosition counts in 2, _bPosFirst counts in 1
    int hexPositionInShowData = _cursorPosition - 2 * _bPosFirst;

    // due to scrolling the cursor can go out of the currently displayed data
    if ((hexPositionInShowData >= 0) && (hexPositionInShowData <= _hexDataShown.size()) )
    {
        // paint cursor
        if (_readOnly)
        {
            QColor color = viewport()->palette().dark().color();
            painter.fillRect(QRect(_pxCursorX - pxOfsX, _pxCursorY - _pxCharHeight + _pxSelectionSub, _pxCharWidth, _pxCharHeight), color);
        }
        else
        {
            if (_blink && hasFocus())
            {

                painter.fillRect(_cursorRect, this->palette().color(QPalette::WindowText));
                painter.fillRect(_cursorRect2, this->palette().color(QPalette::WindowText));
            }
        }
            if (_editAreaIsAscii)
            {
                // every 2 hex there is 1 ascii
                int asciiPositionInShowData = hexPositionInShowData / 2;
                if(asciiPositionInShowData < _cpShown.size())
                {
                    // no glyph on the continuation byte of a multi byte character
                    uint cp = _cpShown.at(asciiPositionInShowData);
                    if (cp)
                        painter.drawText(_pxCursorX - pxOfsX, _pxCursorY, cpToString(cp));
                }
            }
            else
            {
                painter.drawText(_pxCursorX - pxOfsX, _pxCursorY, hexCaps() ? _hexDataShown.mid(hexPositionInShowData, 1).toUpper() : _hexDataShown.mid(hexPositionInShowData, 1));
            }
    }

    // emit event, if size has changed
    if (_lastEventSize != _chunks->size())
    {
        _lastEventSize = _chunks->size();
        emit currentSizeChanged(_lastEventSize);
    }
}

void QHexEdit::resizeEvent(QResizeEvent *)
{
    if (_dynamicBytesPerLine)
    {
        int pxFixGaps = 0;
        if (_addressArea)
            pxFixGaps = addressWidth() * _pxCharWidth + _pxGapAdr;
        pxFixGaps += _pxGapAdrHex;
        if (_asciiArea)
            pxFixGaps += _pxGapHexAscii;

        // +1 because the last hex value do not have space. so it is effective one char more
        int charWidth = (viewport()->width() - pxFixGaps ) / _pxCharWidth + 1;

        // 2 hex alfa-digits 1 space 1 ascii per byte = 4; if ascii is disabled then 3
        // to prevent devision by zero use the min value 1
        setBytesPerLine(std::max(charWidth / (_asciiArea ? 4 : 3),1));
    }
    adjust();
}

bool QHexEdit::focusNextPrevChild(bool next)
{
    if (_addressArea)
    {
        if ( (next && _editAreaIsAscii) || (!next && !_editAreaIsAscii ))
            return QWidget::focusNextPrevChild(next);
        else
            return false;
    }
    else
    {
        return QWidget::focusNextPrevChild(next);
    }
}

// ********************************************************************** Handle selections
void QHexEdit::resetSelection()
{
    _bSelectionBegin = _bSelectionInit;
    _bSelectionEnd = _bSelectionInit;
}

void QHexEdit::resetSelection(qint64 pos)
{
    pos = pos / 2 ;
    if (pos < 0)
        pos = 0;
    if (pos > _chunks->size())
        pos = _chunks->size();

    _bSelectionInit = pos;
    _bSelectionBegin = pos;
    _bSelectionEnd = pos;
}

void QHexEdit::setSelection(qint64 pos)
{
    pos = pos / 2;
    if (pos < 0)
        pos = 0;
    if (pos > _chunks->size())
        pos = _chunks->size();

    if (pos >= _bSelectionInit)
    {
        _bSelectionEnd = pos;
        _bSelectionBegin = _bSelectionInit;
    }
    else
    {
        _bSelectionBegin = pos;
        _bSelectionEnd = _bSelectionInit;
    }
}

qint64 QHexEdit::getSelectionBegin()
{
    return _bSelectionBegin;
}

qint64 QHexEdit::getSelectionEnd()
{
    return _bSelectionEnd;
}

// ********************************************************************** Private utility functions
void QHexEdit::init()
{
    _undoStack->clear();
    setAddressOffset(0);
    resetSelection(0);
    setCursorPosition(0);
    verticalScrollBar()->setValue(0);

    _modified = false;
}

void QHexEdit::adjust()
{
    // recalc Graphics
    if (_addressArea)
    {
        _addrDigits = addressWidth();
        _pxPosHexX = _pxGapAdr + _addrDigits*_pxCharWidth + _pxGapAdrHex;
    }
    else
        _pxPosHexX = _pxGapAdrHex;
    _pxPosAdrX = _pxGapAdr;
    _pxPosAsciiX = _pxPosHexX + _hexCharsInLine * _pxCharWidth + _pxGapHexAscii;

    // set horizontalScrollBar()
    int pxWidth = _pxPosAsciiX;
    if (_asciiArea)
        pxWidth += _bytesPerLine*_pxCharWidth;
    horizontalScrollBar()->setRange(0, pxWidth - viewport()->width());
    horizontalScrollBar()->setPageStep(viewport()->width());

    // set verticalScrollbar()
    _rowsShown = ((viewport()->height()-4)/_pxCharHeight);
    qint64 lineCount = _chunks->size() / (qint64)_bytesPerLine + 1;
    if(lineCount >= 1024*1024*1024){
        _scrollMult = ceil(lineCount/(1024*1024*1024));
    }
    else
    {
        _scrollMult = 1;
    }
    verticalScrollBar()->setRange(0, (lineCount - _rowsShown)/_scrollMult);
    verticalScrollBar()->setPageStep(_rowsShown);

    qint64 value = (qint64)verticalScrollBar()->value()*_scrollMult;
    _bPosFirst = value * _bytesPerLine;
    _bPosLast = _bPosFirst + (qint64)(_rowsShown * _bytesPerLine) - 1;
    if (_bPosLast >= _chunks->size())
        _bPosLast = _chunks->size() - 1;
    readBuffers();
    setCursorPosition(_cursorPosition);
}

void QHexEdit::dataChangedPrivate(int)
{
    _modified = _undoStack->index() != 0;
    adjust();
    emit dataChanged();
}

void QHexEdit::refresh()
{
    ensureVisible();
    readBuffers();
}

void QHexEdit::buildCharMap()
{
    const int size = _dataShown.size();
    _cpShown.resize(size);
    _spanShown.resize(size);

    // Single byte encodings: every byte is a character on its own.
    if (_charEncoding == EncodingAscii || _charEncoding == EncodingLatin1)
    {
        const bool latin1 = (_charEncoding == EncodingLatin1);
        for (int i = 0; i < size; i++)
        {
            uchar ch = (uchar)_dataShown.at(i);
            // Latin-1 maps every byte onto the codepoint of the same value
            bool ok = latin1 ? isRenderable(ch) : (ch >= ' ' && ch <= '~');
            _cpShown[i] = ok ? (uint)ch : (uint)'.';
            _spanShown[i] = 1;
        }
        return;
    }

    const bool isUtf16 = (_charEncoding == EncodingUtf16LE || _charEncoding == EncodingUtf16BE);

    // A sequence may start just above the first visible byte, so read back a few
    // bytes and decode from there: the top row then decodes like all others.
    // UTF-8 resynchronizes on its own, three bytes are enough. UTF-16 does not,
    // so the decoding is anchored on even file offsets and has to start at least
    // one code unit earlier to catch a surrogate pair crossing the top edge.
    int lookBehind;
    if (isUtf16)
        lookBehind = 2 + (int)(_bPosFirst % 2);
    else
        lookBehind = 3;
    lookBehind = (int)qMin<qint64>(lookBehind, _bPosFirst);

    QByteArray buf;
    if (lookBehind > 0)
        buf = _chunks->data(_bPosFirst - lookBehind, lookBehind);
    buf.append(_dataShown);

    const uchar *p = (const uchar *)buf.constData();
    int i = 0;
    while (i < buf.size())
    {
        uint cp = 0;
        int len;
        if (isUtf16)
            len = decodeUtf16(p + i, buf.size() - i, _charEncoding == EncodingUtf16BE, cp);
        else
            len = decodeUtf8(p + i, buf.size() - i, cp);
        int idx = i - lookBehind;                           // index into _dataShown

        // the glyph is drawn in the cell of the first byte of the sequence
        if (idx >= 0 && idx < size)
        {
            _cpShown[idx] = isRenderable(cp) ? cp : (uint)'.';
            _spanShown[idx] = (quint8)len;
        }
        // continuation bytes keep their hex cell but show no glyph
        for (int j = 1; j < len; j++)
        {
            int c = idx + j;
            if (c >= 0 && c < size)
            {
                _cpShown[c] = 0;
                _spanShown[c] = 0;
            }
        }
        i += len;
    }
}

QString QHexEdit::toEncodedString(const QByteArray &ba)
{
    QString result;
    const uchar *p = (const uchar *)ba.constData();
    const int size = ba.size();

    if (_charEncoding == EncodingAscii || _charEncoding == EncodingLatin1)
    {
        const bool latin1 = (_charEncoding == EncodingLatin1);
        for (int i = 0; i < size; i++)
        {
            uchar ch = p[i];
            bool ok = latin1 ? isRenderable(ch) : (ch >= ' ' && ch <= '~');
            result += ok ? cpToString(ch) : QString(QChar('.'));
        }
        return result;
    }

    const bool isUtf16 = (_charEncoding == EncodingUtf16LE || _charEncoding == EncodingUtf16BE);
    int i = 0;
    while (i < size)
    {
        uint cp = 0;
        int len = isUtf16
            ? decodeUtf16(p + i, size - i, _charEncoding == EncodingUtf16BE, cp)
            : decodeUtf8(p + i, size - i, cp);
        result += isRenderable(cp) ? cpToString(cp) : QString(QChar('.'));
        i += len;
    }
    return result;
}

QString QHexEdit::charAt(qint64 pos)
{
    if (pos < 0 || pos >= _chunks->size())
        return QString();

    if (_charEncoding == EncodingAscii || _charEncoding == EncodingLatin1)
    {
        uchar ch = (uchar)_chunks->data(pos, 1).at(0);
        bool ok = (_charEncoding == EncodingLatin1) ? isRenderable(ch)
                                                    : (ch >= ' ' && ch <= '~');
        return ok ? cpToString(ch) : QString(QChar('.'));
    }

    // Start early enough to pick up a sequence that begins before pos. UTF-16
    // has to stay anchored on even file offsets, exactly as buildCharMap() does.
    const bool isUtf16 = (_charEncoding == EncodingUtf16LE || _charEncoding == EncodingUtf16BE);
    int back = isUtf16 ? (int)qMin<qint64>(2 + (pos % 2), pos)
                       : (int)qMin<qint64>(3, pos);
    qint64 start = pos - back;
    QByteArray buf = _chunks->data(start, back + 4);
    const uchar *p = (const uchar *)buf.constData();

    int i = 0;
    while (i < buf.size())
    {
        uint cp = 0;
        int len = isUtf16
            ? decodeUtf16(p + i, buf.size() - i, _charEncoding == EncodingUtf16BE, cp)
            : decodeUtf8(p + i, buf.size() - i, cp);
        if (start + i + len > pos)              // this sequence is the one covering pos
            return isRenderable(cp) ? cpToString(cp) : QString(QChar('.'));
        i += len;
    }
    return QString(QChar('.'));
}

void QHexEdit::readBuffers()
{
    _dataShown = _chunks->data(_bPosFirst, _bPosLast - _bPosFirst + _bytesPerLine + 1, &_markedShown);
    _hexDataShown = QByteArray(_dataShown.toHex());
    buildCharMap();
}

QString QHexEdit::toReadable(const QByteArray &ba)
{
    QString result;

    for (int i=0; i < ba.size(); i += 16)
    {
        QString addrStr = QString("%1").arg(_addressOffset + i, addressWidth(), 16, QChar('0'));
        QString hexStr;
        QString ascStr;
        for (int j=0; j<16; j++)
        {
            if ((i + j) < ba.size())
            {
                hexStr.append(" ").append(ba.mid(i+j, 1).toHex());
                char ch = ba[i + j];
                if ((ch < 0x20) || (ch > 0x7e))
                        ch = '.';
                ascStr.append(QChar(ch));
            }
        }
        result += addrStr + " " + QString("%1").arg(hexStr, -48) + "  " + QString("%1").arg(ascStr, -17) + "\n";
    }
    return result;
}

void QHexEdit::updateCursor()
{
    if (_blink)
        _blink = false;
    else
        _blink = true;
    viewport()->update(_cursorRect);
}
void QHexEdit::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu;
    if(!_editAreaIsAscii)
    {
        if(getSelectionEnd() - getSelectionBegin()> 0)
        {


            QAction *copyAction = contextMenu.addAction("Copy");
            connect(copyAction, &QAction::triggered, this, &QHexEdit::copyText);

            QAction *cutAction = contextMenu.addAction("Cut");
            connect(cutAction, &QAction::triggered, this, &QHexEdit::cutText);


        }
        QAction *pasteAction = contextMenu.addAction("Paste");
        connect(pasteAction, &QAction::triggered, this, &QHexEdit::pasteText);

        contextMenu.exec(mapToGlobal(pos));
    }


}
void QHexEdit::copyText(){
    QByteArray ba;
    if(!_editAreaIsAscii)
    {
        ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin()).toHex();
        for (qint64 idx = 32; idx < ba.size(); idx +=33)
            ba.insert(idx, "\n");
    }
    else
    {
        ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin());
        for (int i = 0; i < ba.length(); i++) {
            if(ba.at(i) < 32 || ba.at(i) > 126)
            {
                ba[i] = '.';
            }
        }
    }


    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ba);
}
void QHexEdit::pasteText(){
    QClipboard *clipboard = QApplication::clipboard();
    QByteArray ba = QByteArray().fromHex(clipboard->text().toLatin1());
    if (_overwriteMode)
    {
        ba = ba.left(std::min<qint64>(ba.size(), (_chunks->size() - _bPosCurrent)));
        replace(_bPosCurrent, ba.size(), ba);
    }
    else
        insert(_bPosCurrent, ba);
    setCursorPosition(_cursorPosition + 2 * ba.size());
    resetSelection(getSelectionBegin());
}
void QHexEdit::cutText(){
    QByteArray ba = _chunks->data(getSelectionBegin(), getSelectionEnd() - getSelectionBegin()).toHex();
    for (qint64 idx = 32; idx < ba.size(); idx +=33)
        ba.insert(idx, "\n");
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ba);
    if (_overwriteMode)
    {
        qint64 len = getSelectionEnd() - getSelectionBegin();
        replace(getSelectionBegin(), (int)len, QByteArray((int)len, char(0)));
    }
    else
    {
        remove(getSelectionBegin(), getSelectionEnd() - getSelectionBegin());
    }
    setCursorPosition(2 * getSelectionBegin());
    resetSelection(2 * getSelectionBegin());
}
