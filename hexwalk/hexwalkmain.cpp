/*
 * Copyright (C) 2025 Carmix <carmixdev@gmail.com>
 *
 * This file is part of HexWalk.
 *
 * HexWalk is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HexWalk is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QActionGroup>
#include <QMenuBar>
#include <QToolBar>
#include <QColorDialog>
#include <QFontDialog>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "hexwalkmain.h"
#include "ui_hexwalkmain.h"
#include "theme.h"

HexWalkMain::HexWalkMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HexWalkMain)
{
    ui->setupUi(this);
    setAcceptDrops( true );
    init();
    setCurrentFile("");
    QFont font("Courier",10);
    ui->binTextedit->setFont(font);
    ui->asciiTextEdit->setFont(font);
    ui->asciiTextEdit->setTextFormat(Qt::PlainText);
    ui->decTextedit->setFont(font);
    ui->floatTextedit_le->setFont(font);
    ui->floatTextedit_be->setFont(font);
    ui->hexTextedit->setFont(font);
    ui->intleTextedit->setFont(font);

}

void HexWalkMain::init()
{
    setAttribute(Qt::WA_DeleteOnClose);
    lbTextEncoding = nullptr;                   // only valid after createStatusBar()
    appSettings = new QSettings("hexwalk","hexwalk");
    //appSettings->clear();
    isUntitled = true;

    hexEdit = ui->widget;
    connect(hexEdit, &QHexEdit::overwriteModeChanged, this, &HexWalkMain::setOverwriteMode);
    connect(hexEdit, &QHexEdit::dataChanged, this, &HexWalkMain::dataChanged);
    searchDialog = new SearchDialog(hexEdit, this);
    advancedSearchDialog = new AdvancedSearchDialog(hexEdit,this);
    optionsDialog = new OptionsDialog(appSettings,this);
    connect(optionsDialog, &OptionsDialog::accepted, this, &HexWalkMain::updateOptions);
    entropyDialog = new EntropyDialog(hexEdit,this);
    analysisDialog = new binanalysisdialog(hexEdit,appSettings,this);
    hashDialog = new HashDialog(this);
    diffDialog = new DiffDialog(this);
    tagsDialog = new TagsDialog(hexEdit,this);
    stringsDialog = new StringsDialog(hexEdit,this);
    byteMapDialog = new ByteMapDialog(hexEdit,this);
    //disasmDialog = new DisasmDialog(hexEdit,this);
    disasmWidget = new DisasmWidget(hexEdit,this);
    converterWidget = new ConverterWidget(this);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();


    setUnifiedTitleAndToolBarOnMac(true);
}
HexWalkMain::~HexWalkMain()
{
    delete ui;
}

void HexWalkMain::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(saveReadable);
    fileMenu->addAction(closeAct);

    recentFilesMenu = fileMenu->addMenu(tr("Open Recent"));
    for(auto i = 0; i < 5; ++i)
        recentFilesMenu->addAction(recentFileActionList.at(i));

    updateRecentActionList();
    fileMenu->addMenu(recentFilesMenu);

    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(cutAct);
    editMenu->addAction(saveSelectionReadable);
    editMenu->addSeparator();
    editMenu->addAction(advancedFindAct);
    editMenu->addAction(findAct);
    editMenu->addAction(overwriteAct);

    encodingMenu = editMenu->addMenu(tr("Text &Encoding"));
    QActionGroup *encodingGroup = new QActionGroup(this);
    encodingGroup->setExclusive(true);
    for (QAction *act : encodingActionList)
    {
        encodingGroup->addAction(act);
        encodingMenu->addAction(act);
    }

    editMenu->addAction(optionsAct);





    analysisMenu = menuBar()->addMenu(tr("&Analysis"));
    analysisMenu->addAction(entropyAct);
    analysisMenu->addAction(binaryAct);
    analysisMenu->addAction(diffAct);
    analysisMenu->addAction(tagsAct);
    analysisMenu->addAction(stringsAct);
    analysisMenu->addAction(byteMapAct);
    analysisMenu->addAction(disasmAct);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(converterAct);
    toolsMenu->addAction(hashAct);


    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void HexWalkMain::createStatusBar()
{
    // Selected Label
    lbSelectedName = new QLabel();
    lbSelectedName->setText(tr("Selected:"));
    statusBar()->addPermanentWidget(lbSelectedName);
    lbSelected = new QLabel();
    lbSelected->setFrameShape(QFrame::Panel);
    lbSelected->setFrameShadow(QFrame::Sunken);
    lbSelected->setMinimumWidth(100);
    statusBar()->addPermanentWidget(lbSelected);

    // Address Label
    lbAddressName = new QLabel();
    lbAddressName->setText(tr("Address:"));
    statusBar()->addPermanentWidget(lbAddressName);
    lbAddress = new QLabel();
    lbAddress->setFrameShape(QFrame::Panel);
    lbAddress->setFrameShadow(QFrame::Sunken);
    lbAddress->setMinimumWidth(100);
    statusBar()->addPermanentWidget(lbAddress);
    connect(hexEdit, &QHexEdit::currentAddressChanged, this, &HexWalkMain::setAddress);

    // Size Label
    lbSizeName = new QLabel();
    lbSizeName->setText(tr("Size:"));
    statusBar()->addPermanentWidget(lbSizeName);
    lbSize = new QLabel();
    lbSize->setFrameShape(QFrame::Panel);
    lbSize->setFrameShadow(QFrame::Sunken);
    lbSize->setMinimumWidth(70);
    statusBar()->addPermanentWidget(lbSize);
    connect(hexEdit, &QHexEdit::currentSizeChanged, this, &HexWalkMain::setSize);

    // Overwrite Mode Label
    lbOverwriteModeName = new QLabel();
    lbOverwriteModeName->setText(tr("Mode:"));
    statusBar()->addPermanentWidget(lbOverwriteModeName);
    lbOverwriteMode = new QLabel();
    lbOverwriteMode->setFrameShape(QFrame::Panel);
    lbOverwriteMode->setFrameShadow(QFrame::Sunken);
    lbOverwriteMode->setMinimumWidth(70);
    statusBar()->addPermanentWidget(lbOverwriteMode);
    setOverwriteMode(hexEdit->overwriteMode());

    // Text Encoding Label
    lbTextEncodingName = new QLabel();
    lbTextEncodingName->setText(tr("Encoding:"));
    statusBar()->addPermanentWidget(lbTextEncodingName);
    lbTextEncoding = new QLabel();
    lbTextEncoding->setFrameShape(QFrame::Panel);
    lbTextEncoding->setFrameShadow(QFrame::Sunken);
    lbTextEncoding->setMinimumWidth(80);
    lbTextEncoding->setToolTip(tr("Encoding of the text area, change it under Edit / Text Encoding"));
    statusBar()->addPermanentWidget(lbTextEncoding);

    statusBar()->showMessage(tr("Ready"), 2000);
}

void HexWalkMain::createActions()
{
    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &HexWalkMain::open);

    closeAct = new QAction(tr("&Close..."), this);
    closeAct->setShortcuts(QKeySequence::Close);
    closeAct->setStatusTip(tr("Close current file"));
    connect(closeAct, &QAction::triggered, this, &HexWalkMain::close);

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &HexWalkMain::save);

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, &QAction::triggered, this, &HexWalkMain::saveAs);

    saveReadable = new QAction(tr("Save &Readable..."), this);
    saveReadable->setStatusTip(tr("Save document in readable form"));
    connect(saveReadable, &QAction::triggered, this, &HexWalkMain::saveToReadableFile);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, qApp, &QApplication::closeAllWindows);

    undoAct = new QAction(QIcon(":/images/undo.png"), tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    connect(undoAct, &QAction::triggered, hexEdit, &QHexEdit::undo);

    redoAct = new QAction(QIcon(":/images/redo.png"), tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    connect(redoAct, &QAction::triggered, hexEdit, &QHexEdit::redo);

    copyAct = new QAction( tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    connect(copyAct, &QAction::triggered, hexEdit, &QHexEdit::copyText);
    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    connect(pasteAct, &QAction::triggered, hexEdit, &QHexEdit::pasteText);
    cutAct = new QAction( tr("&Cut"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    connect(cutAct, &QAction::triggered, hexEdit, &QHexEdit::cutText);

    saveSelectionReadable = new QAction(tr("&Save Selection Readable..."), this);
    saveSelectionReadable->setStatusTip(tr("Save selection in readable form"));
    connect(saveSelectionReadable, &QAction::triggered, this, &HexWalkMain::saveSelectionToReadableFile);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, &QAction::triggered, this, &HexWalkMain::about);

    findAct = new QAction(tr("&Find/Replace"), this);
    findAct->setShortcuts(QKeySequence::FindPrevious);
    findAct->setStatusTip(tr("Show the Dialog for finding and replacing"));
    connect(findAct, &QAction::triggered, this, &HexWalkMain::showSearchDialog);

    overwriteAct = new QAction(tr("&Overwrite/Insert mode"), this);
    overwriteAct->setShortcut(QKeySequence(Qt::Key_Insert));
    overwriteAct->setStatusTip(tr("Toggle overwrite/insert mode"));
    connect(overwriteAct, &QAction::triggered, this, &HexWalkMain::toggleOverwriteMode);

    // Text area encodings, kept in sync with the Options dialog. The list is
    // indexed by QHexEdit::CharEncoding, each action carries its value as data.
    struct { QHexEdit::CharEncoding enc; const char *name; const char *tip; } encodings[] = {
        { QHexEdit::EncodingAscii,   QT_TR_NOOP("&ASCII"),     QT_TR_NOOP("Show the text area as ASCII") },
        { QHexEdit::EncodingUtf8,    QT_TR_NOOP("&UTF-8"),     QT_TR_NOOP("Decode the text area as UTF-8") },
        { QHexEdit::EncodingLatin1,  QT_TR_NOOP("&Latin-1"),   QT_TR_NOOP("Decode the text area as Latin-1 (ISO 8859-1)") },
        { QHexEdit::EncodingUtf16LE, QT_TR_NOOP("UTF-16 L&E"), QT_TR_NOOP("Decode the text area as little endian UTF-16") },
        { QHexEdit::EncodingUtf16BE, QT_TR_NOOP("UTF-16 &BE"), QT_TR_NOOP("Decode the text area as big endian UTF-16") },
    };
    for (const auto &e : encodings)
    {
        QAction *act = new QAction(tr(e.name), this);
        act->setCheckable(true);
        act->setStatusTip(tr(e.tip));
        act->setData((int)e.enc);
        connect(act, &QAction::triggered, this, &HexWalkMain::selectCharEncoding);
        encodingActionList.append(act);
    }

    optionsAct = new QAction(tr("&Options"), this);
    optionsAct->setStatusTip(tr("Options"));
    connect(optionsAct, &QAction::triggered, this, &HexWalkMain::showOptionsDialog);

    findNextAct = new QAction(tr("Find &next"), this);
    findNextAct->setShortcuts(QKeySequence::FindNext);
    findNextAct->setStatusTip(tr("Find next occurrence of the searched pattern"));
    connect(findNextAct, &QAction::triggered, this, &HexWalkMain::findNext);

    advancedFindAct = new QAction(QIcon(":/images/find.png"),tr("Advanced Find"), this);
    advancedFindAct->setShortcuts(QKeySequence::Find);
    advancedFindAct->setStatusTip(tr("Advanced find tool"));
    connect(advancedFindAct, &QAction::triggered, this, &HexWalkMain::showAdvancedSearchDialog);

    entropyAct = new QAction(QIcon(":/images/entropy.png"),tr("Entropy"), this);
    entropyAct->setShortcut(Qt::CTRL|Qt::Key_E);
    entropyAct->setStatusTip(tr("Calculate entropy of file"));
    connect(entropyAct, &QAction::triggered, this, &HexWalkMain::showEntropyDialog);

    diffAct = new QAction(QIcon(":/images/diff.png"),tr("Diff Analysis"), this);
    diffAct->setStatusTip(tr("do diff compare byte to byte"));
    connect(diffAct, &QAction::triggered, this, &HexWalkMain::showDiffDialog);

    binaryAct = new QAction(QIcon(":/images/binary.png"),tr("Binary Analysis"), this);
    binaryAct->setStatusTip(tr("make binary analysis with Binwalk"));
    connect(binaryAct, &QAction::triggered, this, &HexWalkMain::showBinaryDialog);

    converterAct = new QAction(tr("Number Converter"), this);
    converterAct->setStatusTip(tr("Useful number converter"));
    connect(converterAct, &QAction::triggered, this, &HexWalkMain::showConverterWidget);

    hashAct = new QAction(tr("Hash Calculator"), this);
    hashAct->setStatusTip(tr("Hash Calculator"));
    connect(hashAct, &QAction::triggered, this, &HexWalkMain::showHashDialog);

    tagsAct = new QAction(QIcon(":/images/tags.png"),tr("Byte Patterns"), this);
    tagsAct->setStatusTip(tr("Byte Patterns"));
    connect(tagsAct, &QAction::triggered, this, &HexWalkMain::showTagsDialog);

    stringsAct = new QAction(QIcon(":/images/strings.png"),tr("Search Strings"), this);
    stringsAct->setStatusTip(tr("Search Strings"));
    connect(stringsAct, &QAction::triggered, this, &HexWalkMain::showStringsDialog);

    byteMapAct = new QAction(QIcon(":/images/bytemap.png"),tr("ByteMap"), this);
    byteMapAct->setStatusTip(tr("Byte Map"));
    connect(byteMapAct, &QAction::triggered, this, &HexWalkMain::showByteMap);

    disasmAct = new QAction(QIcon(":/images/disasm.png"),tr("Disasm"), this);
    disasmAct->setStatusTip(tr("Disassembler"));
    connect(disasmAct, &QAction::triggered, this, &HexWalkMain::showDisasm);

    QAction* recentFileAction = 0;
    for(auto i = 0; i < 5; ++i){
        recentFileAction = new QAction(this);
        recentFileAction->setVisible(false);
        connect(recentFileAction, &QAction::triggered, this, &HexWalkMain::openRecent);
        recentFileActionList.append(recentFileAction);
    }

    applyThemeIcons(appSettings->value("Theme", Theme::kDark).toString());
}

// (Re)load every toolbar/menu icon for the given theme. Monochrome glyphs are
// re-tinted dark on the light theme so they don't look disabled; colourful
// icons (open/save) are left as-is. Called at startup and whenever the theme
// changes via the Options dialog.
void HexWalkMain::applyThemeIcons(const QString &theme)
{
    openAct->setIcon(Theme::themedIcon(":/images/open.png", theme, /*monochrome*/ false));
    saveAct->setIcon(Theme::themedIcon(":/images/save.png", theme, /*monochrome*/ false));
    undoAct->setIcon(Theme::themedIcon(":/images/undo.png", theme));
    redoAct->setIcon(Theme::themedIcon(":/images/redo.png", theme));
    advancedFindAct->setIcon(Theme::themedIcon(":/images/find.png", theme));
    entropyAct->setIcon(Theme::themedIcon(":/images/entropy.png", theme));
    diffAct->setIcon(Theme::themedIcon(":/images/diff.png", theme));
    binaryAct->setIcon(Theme::themedIcon(":/images/binary.png", theme));
    tagsAct->setIcon(Theme::themedIcon(":/images/tags.png", theme));
    stringsAct->setIcon(Theme::themedIcon(":/images/strings.png", theme));
    byteMapAct->setIcon(Theme::themedIcon(":/images/bytemap.png", theme));
    disasmAct->setIcon(Theme::themedIcon(":/images/disasm.png", theme));
}
void HexWalkMain::toggleOverwriteMode(){
    if(hexEdit->overwriteMode() == true)
    {
        hexEdit->setOverwriteMode(false);
    }
    else
    {
        hexEdit->setOverwriteMode(true);
    }
}
// Applies the text area encoding to the editor, keeps the View menu in sync and
// persists it, so the menu and the Options dialog always agree.
// The stored encoding is the default, applied every time a file is opened.
QHexEdit::CharEncoding HexWalkMain::defaultCharEncoding()
{
    return QHexEdit::charEncodingFromInt(
        appSettings->value("CharEncoding", QHexEdit::EncodingAscii).toInt());
}

// Applies an encoding to the current view and ticks the matching menu entry.
// The stored default is left alone, so picking an encoding from the Edit menu
// only affects the file at hand.
void HexWalkMain::applyCharEncoding(QHexEdit::CharEncoding encoding)
{
    hexEdit->setCharEncoding(encoding);
    for (QAction *act : encodingActionList)
    {
        bool isCurrent = (act->data().toInt() == (int)encoding);
        act->setChecked(isCurrent);
        // the menu entries are the single source of the encoding names
        if (isCurrent && lbTextEncoding)
            lbTextEncoding->setText(act->text().remove('&'));
    }
    updateInfo();                               // re-decode the Text inspector field
}

void HexWalkMain::selectCharEncoding(){
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        applyCharEncoding(QHexEdit::charEncodingFromInt(action->data().toInt()));
}

void HexWalkMain::openRecent(){
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}
void HexWalkMain::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("file");
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("edit");
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);
    editToolBar->addAction(advancedFindAct);
    analysisToolBar = addToolBar(tr("Analysis"));
    analysisToolBar->setObjectName("analysis");
    analysisToolBar->addAction(entropyAct);
    analysisToolBar->addAction(binaryAct);
    analysisToolBar->addAction(diffAct);
    analysisToolBar->addAction(tagsAct);
    analysisToolBar->addAction(stringsAct);
    analysisToolBar->addAction(byteMapAct);
    analysisToolBar->addAction(disasmAct);
    analysisToolBar->addSeparator();
    gotoLbl = new QLabel();
    gotoLbl->setText("Go To: ");
    gotoLbl->setFixedHeight(25);
    analysisToolBar->addWidget(gotoLbl);

    gotoText = new QLineEdit();
    gotoText->setFixedHeight(25);
    gotoText->setFixedWidth(80);
    gotoText->setText(tr("0"));
    connect(gotoText, &QLineEdit::returnPressed, this, &HexWalkMain::gotoAddress);
    analysisToolBar->addWidget(gotoText);

    analysisToolBar->addSeparator();
    widthLbl = new QLabel();
    widthLbl->setText("Width: ");
    widthLbl->setFixedHeight(25);
    analysisToolBar->addWidget(widthLbl);
    widthText = new QLineEdit();
    widthText->setFixedHeight(25);
    widthText->setFixedWidth(40);
    widthText->setText(tr("16"));
    connect(widthText, &QLineEdit::returnPressed, this, &HexWalkMain::setWidth);
    analysisToolBar->addWidget(widthText);
    //infoToolBar = addToolBar(tr("Info"));

}

void HexWalkMain::setFileActionsEnabled(bool enabled)
{
    QAction* all[] = {diffAct, entropyAct, binaryAct, hashAct, tagsAct,stringsAct,byteMapAct};
    for (auto act: all)
    {
        act->setEnabled(enabled);
    }
}

void HexWalkMain::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = fileName.isEmpty();
    setWindowModified(false);
    if (fileName.isEmpty())
    {
        setWindowFilePath("HexWalk");
        this->setWindowTitle("HexWalk");
        setFileActionsEnabled(false);
    }
    else
    {
        setWindowFilePath(curFile + " - HexWalk");
        this->setWindowTitle("HexWalk [" + curFile + "]");
        setFileActionsEnabled(true);
    }

}

void HexWalkMain::loadFile(const QString &fileName)
{
    hexfile.setFileName(fileName);
    if (!hexEdit->setData(hexfile)) {
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("Cannot read file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
        return;
    }
    // a freshly opened file always starts from the configured default encoding,
    // discarding any per-view choice made for the previous file
    applyCharEncoding(defaultCharEncoding());
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
    emit fileLoaded();
}

/*****************************************************************************/
/* Private Slots */
/*****************************************************************************/
void HexWalkMain::about()
{
    QString title = tr("About HexWalk");
    QString text = tr("HexWalk %1 is an HEX editor/viewer/analyzer.<br>"
                      "It is open source and it is based on QT, qhexedit2, binwalk.<br>"
                      "Sources at <a href='https://github.com/gcarmix/HexWalk'>Github</a>.<br>")
    .arg(APP_VERSION);

    QMessageBox msgBox(title, text, QMessageBox::Information, 0, 0, 0, this);
    msgBox.setTextFormat(Qt::RichText);
    QIcon icon = msgBox.windowIcon();
    QSize size = icon.actualSize(QSize(64, 64));
    msgBox.setIconPixmap(icon.pixmap(size));
    msgBox.exec();
}

void HexWalkMain::dataChanged()
{
    setWindowModified(hexEdit->isModified());
}

void HexWalkMain::updateOptions()
{
    readSettings();
}

void HexWalkMain::adjustForCurrentFile(const QString &filePath){
    currentFilePath = filePath;
    setWindowFilePath(currentFilePath);

    QStringList recentFilePaths =
        appSettings->value("recentFiles").toStringList();
    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);
    while (recentFilePaths.size() > 5)
        recentFilePaths.removeLast();
    appSettings->setValue("recentFiles", recentFilePaths);

    // see note
    updateRecentActionList();
}
void HexWalkMain::updateRecentActionList(){

    QStringList recentFilePaths =
        appSettings->value("recentFiles").toStringList();

    auto itEnd = 0u;
    if(recentFilePaths.size() <= 5)
        itEnd = recentFilePaths.size();
    else
        itEnd = 5;

    for (auto i = 0u; i < itEnd; ++i) {
        QString strippedName = QFileInfo(recentFilePaths.at(i)).fileName();
        recentFileActionList.at(i)->setText(strippedName);
        recentFileActionList.at(i)->setData(recentFilePaths.at(i));
        recentFileActionList.at(i)->setVisible(true);
    }

    for (auto i = itEnd; i < 5; ++i)
        recentFileActionList.at(i)->setVisible(false);
}
void HexWalkMain::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        adjustForCurrentFile(fileName);
        loadFile(fileName);
    }
}

void HexWalkMain::close()
{
    QFile empty;
    hexEdit->setData(empty);
    setCurrentFile("");
}

void HexWalkMain::writeSettings()
{
    appSettings->setValue("pos", pos());
    appSettings->setValue("size", size());
    appSettings->setValue("mainWindowGeometry", saveGeometry());
    appSettings->setValue("mainWindowState", saveState());
}

void HexWalkMain::findNext()
{
    searchDialog->findNext();
}

bool HexWalkMain::saveFile(const QString &fileName)
{
    QString tmpFileName = fileName + ".~tmp";

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QFile file(tmpFileName);
    bool ok = hexEdit->write(file);
    if (QFile::exists(fileName))
        ok = QFile::remove(fileName);
    if (ok)
    {
        file.setFileName(tmpFileName);
        ok = file.copy(fileName);
        if (ok)
            ok = QFile::remove(tmpFileName);
    }
    QApplication::restoreOverrideCursor();

    if (!ok) {
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("Cannot write file %1.")
                                 .arg(fileName));
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}
QString HexWalkMain::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool HexWalkMain::save()
{

    if (isUntitled) {
        return saveAs();
    } else {

        QMessageBox msgBox;
        msgBox.setText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Save:
            // Save was clicked
            return saveFile(curFile);
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
        }



    }
    return 0;
}

bool HexWalkMain::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void HexWalkMain::saveSelectionToReadableFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save To Readable File"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("HexWalk"),
                                 tr("Cannot write file %1:\n%2.")
                                     .arg(fileName)
                                     .arg(file.errorString()));
            return;
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        file.write(hexEdit->selectionToReadableString().toLatin1());
        QApplication::restoreOverrideCursor();

        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void HexWalkMain::saveToReadableFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save To Readable File"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("HexWalk"),
                                 tr("Cannot write file %1:\n%2.")
                                     .arg(fileName)
                                     .arg(file.errorString()));
            return;
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        file.write(hexEdit->toReadableString().toLatin1());
        QApplication::restoreOverrideCursor();

        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void HexWalkMain::updateInfo()
{
    int selSize = hexEdit->selectedDataBa().size();
    lbSelected->setText(QString::number(selSize,10));

    if(selSize > 0)
    {
        // hex and text stay meaningful for a selection of any length, unlike the
        // numeric fields below, but a huge one is capped: the labels are small
        const int previewMax = 256;
        QByteArray preview = hexEdit->selectedDataBa().left(previewMax);
        QString ellipsis = selSize > previewMax ? QStringLiteral("...") : QString();
        ui->asciiTextEdit->setText(hexEdit->toEncodedString(preview) + ellipsis);
        ui->hexTextedit->setText(QString(preview.toHex()).toUpper() + ellipsis);

        if(selSize <= 8)
        {
            converterWidget->update(hexEdit->selectedData().toUpper());
            if(selSize == 4 || selSize == 8)
            {

                qint64 num = hexEdit->selectedData().toULongLong(NULL,16);
                QByteArray baValue = hexEdit->selectedDataBa();
                if(selSize == 4)
                {
                    float *numf;
                    numf = (float *)&num;
                    ui->floatTextedit_be->setText(QString::number(*numf));
                    std::reverse(baValue.begin(),baValue.end());
                    num = baValue.toHex().toULongLong(NULL,16);
                    numf = (float *)&num;
                    ui->floatTextedit_le->setText(QString::number(*numf));
                }
                else
                {
                    double *numf;
                    numf = (double *)&num;
                    ui->floatTextedit_be->setText(QString::number(*numf));
                    std::reverse(baValue.begin(),baValue.end());
                    num = baValue.toHex().toULongLong(NULL,16);
                    numf = (double *)&num;
                    ui->floatTextedit_le->setText(QString::number(*numf));
                }
            }
            else
            {
                ui->floatTextedit_le->setText("-");
                ui->floatTextedit_be->setText("-");
            }
            QByteArray baValue = hexEdit->selectedDataBa();
            if(ui->signedcb->isChecked())
            {
                if(selSize < 5)
                {
                    ui->decTextedit->setText(QString("%1").arg((signed int)hexEdit->selectedData().toUInt(NULL,16)));
                    std::reverse(baValue.begin(),baValue.end());
                    ui->intleTextedit->setText(QString("%1").arg((signed int)baValue.toHex().toUInt(NULL,16)));
                }
                else
                {
                    ui->decTextedit->setText(QString("%1").arg((signed long long)hexEdit->selectedData().toULongLong(NULL,16)));
                    std::reverse(baValue.begin(),baValue.end());
                    ui->intleTextedit->setText(QString("%1").arg((signed long long)baValue.toHex().toULongLong(NULL,16)));
                }
            }
            else
            {
                ui->decTextedit->setText(QString("%1").arg(hexEdit->selectedData().toULongLong(NULL,16)));
                std::reverse(baValue.begin(),baValue.end());
                ui->intleTextedit->setText(QString("%1").arg(baValue.toHex().toULongLong(NULL,16)));
            }
            ui->binTextedit->setText(QString("%1").arg(hexEdit->selectedData().toULongLong(NULL,16),8,2,QLatin1Char('0')));
        }
        else
        {
            // only the numeric interpretations are meaningless here,
            // hex and text keep the values set above
            ui->decTextedit->setText("-");
            ui->floatTextedit_le->setText("-");
            ui->floatTextedit_be->setText("-");
            ui->intleTextedit->setText("-");
            ui->binTextedit->setText("-");
        }
    }
}
void HexWalkMain::setAddress(qint64 address)
{
    lbAddress->setText(QString("%1(%2)").arg(address, 1, 16).arg(address,1,10));
    if(hexEdit->getSize() > 0)
    {
        if(address < hexEdit->getSize())
        {
            if(hexEdit->selectedDataBa().size() > 0)
            {

                updateInfo();
            }
            else
            {
                // no selection: report the single byte under the cursor
                uchar byte = uchar(hexEdit->dataAt(address,1).at(0));
                ui->hexTextedit->setText(QString("%1").arg(byte,2,16,QLatin1Char('0')).toUpper());
                ui->decTextedit->setText(QString("%1").arg(byte,3,10));
                ui->intleTextedit->setText(QString("%1").arg(byte,3,10));
                ui->binTextedit->setText(QString("%1").arg(byte,8,2,QLatin1Char('0')));
                // the character this byte belongs to, which in a multi byte
                // encoding may start a few bytes earlier
                ui->asciiTextEdit->setText(hexEdit->charAt(address));
                // a single byte has no float reading, do not leave a stale one
                ui->floatTextedit_le->setText("-");
                ui->floatTextedit_be->setText("-");
            }

        }
    }

}

void HexWalkMain::setOverwriteMode(bool mode)
{
    appSettings->setValue("OverwriteMode", mode);
    if (mode)
        lbOverwriteMode->setText(tr("Overwrite"));
    else
        lbOverwriteMode->setText(tr("Insert"));
}

void HexWalkMain::setSize(qint64 size)
{
    lbSize->setText(QString("%1").arg(size));
}

void HexWalkMain::showSearchDialog()
{
    searchDialog->show();
}

void HexWalkMain::showAdvancedSearchDialog()
{
    advancedSearchDialog->show();
}

void HexWalkMain::showOptionsDialog()
{
    optionsDialog->show();
}

void HexWalkMain::showDiffDialog()
{
    if(curFile.length() == 0)
    {
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("You must select a file first.")
                             );
    }
    else
    {
        diffFile = QFileDialog::getOpenFileName(this);
        if (!diffFile.isEmpty()) {
        diffDialog->setFiles(curFile,diffFile);
        diffDialog->show();

        }

    }


}

void HexWalkMain::showEntropyDialog()
{
    if(curFile.length() == 0)
    {
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("You must select a file first.")
                             );
    }
    else
    {
        entropyDialog->show();
        entropyDialog->calculate();
    }

}

void HexWalkMain::showBinaryDialog()
{
    if(curFile.length() == 0)
    {
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("You must select a file first.")
                                );
    }
    else
    {
        analysisDialog->show();
        analysisDialog->analyze(curFile);
    }

}

void HexWalkMain::showConverterWidget()
{
    converterWidget->show();
}

void HexWalkMain::gotoAddress()
{
    qint64 destPos = 2*(gotoText->text().toLongLong(NULL,16));
    hexEdit->setCursorPosition(destPos);
    hexEdit->ensureVisible();
}

void HexWalkMain::setWidth()
{
    int value =widthText->text().toInt();
    if (value <= 0)
    {
        value = 8;
        widthText->setText(QString("%1").arg(value));
    }
    else if(value > 64)
    {
        value = 64;
        widthText->setText(QString("%1").arg(value));
    }

    appSettings->setValue("BytesPerLine",value);
    hexEdit->setBytesPerLine(value);
}
void HexWalkMain::showHashDialog()
{
    if(curFile.length() == 0)
    {
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("You must select a file first.")
                             );
    }
    else
    {
        hashDialog->show();
        hashDialog->calculate(curFile);
    }

}

void HexWalkMain::showTagsDialog()
{
    if(curFile.length() == 0)
    {
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("You must select a file first.")
                             );
    }
    else
    {
        tagsDialog->show();

    }

}

void HexWalkMain::showStringsDialog()
{
    if(curFile.length() == 0)
    {
        QMessageBox::warning(this, tr("HexWalk"),
                             tr("You must select a file first.")
                             );
    }
    else
    {
        stringsDialog->show();

    }

}

void HexWalkMain::readSettings()
{
    if(appSettings->value("BytesPerLine").toInt()<=0)
    {
        appSettings->setValue("Theme",Theme::kDark);
        appSettings->setValue("AddressArea",true);
        appSettings->setValue("AsciiArea",true);
        appSettings->setValue("Highlighting",true);
        appSettings->setValue("OverwriteMode",true);
        appSettings->setValue("ReadOnly",false);
        appSettings->setValue("HighlightingColor",QColor("#540c00"));
        appSettings->setValue("AddressAreaColor",QColor("#545454"));
        appSettings->setValue("SelectionColor",QColor("#0998c7"));
        appSettings->setValue("WidgetFont",QFont("Courier",12));
        appSettings->setValue("AddressFontColor",QColor("#f0f0f0"));
        appSettings->setValue("AsciiAreaColor",QColor("#424242"));
        appSettings->setValue("AsciiFontColor",QColor("#00ff5e"));
        appSettings->setValue("HexFontColor",QColor("#00ff5e"));
        appSettings->setValue("AddressAreaWidth",6);
        appSettings->setValue("BytesPerLine",16);
        appSettings->setValue("HexCaps",true);
        appSettings->setValue("CharEncoding",(int)QHexEdit::EncodingAscii);
        appSettings->setValue("DefaultAnalyzer","hexdig");



        appSettings->setValue("HexFontColor",QColor("#00ff5e"));


        appSettings->setValue("pos", pos());
        appSettings->setValue("size", size());
        appSettings->setValue("mainWindowGeometry", saveGeometry());
        appSettings->setValue("mainWindowState", saveState());
        appSettings->sync();


    }
    else
    {

        restoreGeometry(appSettings->value("mainWindowGeometry").toByteArray());
        restoreState(appSettings->value("mainWindowState").toByteArray());
        int bytesperline = appSettings->value("BytesPerLine").toInt();
        if( bytesperline > 0 && bytesperline < 64 )
        {
            hexEdit->setBytesPerLine(bytesperline);
            widthText->setText(QString("%1").arg(bytesperline));
        }
        else{
            bytesperline = 16;
            hexEdit->setBytesPerLine(bytesperline);
            widthText->setText(QString("%1").arg(bytesperline));
        }

        QPoint pos = appSettings->value("pos", QPoint(200, 200)).toPoint();
        QSize size = appSettings->value("size", QSize(610, 460)).toSize();
        move(pos);
        resize(size);
    }
    hexEdit->setAddressArea(appSettings->value("AddressArea").toBool());
    hexEdit->setAsciiArea(appSettings->value("AsciiArea").toBool());
    hexEdit->setHighlighting(appSettings->value("Highlighting").toBool());
    hexEdit->setOverwriteMode(appSettings->value("OverwriteMode").toBool());
    hexEdit->setReadOnly(appSettings->value("ReadOnly").toBool());

    hexEdit->setHighlightingColor(appSettings->value("HighlightingColor").value<QColor>());
    hexEdit->setAddressAreaColor(appSettings->value("AddressAreaColor").value<QColor>());
    hexEdit->setSelectionColor(appSettings->value("SelectionColor").value<QColor>());
    hexEdit->setFont(appSettings->value("WidgetFont").value<QFont>());
    hexEdit->setAddressFontColor(appSettings->value("AddressFontColor").value<QColor>());
    hexEdit->setAsciiAreaColor(appSettings->value("AsciiAreaColor").value<QColor>());
    hexEdit->setAsciiFontColor(appSettings->value("AsciiFontColor").value<QColor>());
    hexEdit->setHexFontColor(appSettings->value("HexFontColor").value<QColor>());

    hexEdit->setAddressWidth(appSettings->value("AddressAreaWidth").toInt());
    hexEdit->setBytesPerLine(appSettings->value("BytesPerLine").toInt());
    hexEdit->setHexCaps(appSettings->value("HexCaps", true).toBool());
    applyCharEncoding(defaultCharEncoding());

    // Apply the application-wide palette for the selected theme so the whole
    // UI (menus, dialogs, toolbars) re-themes live when Options are accepted.
    QString theme = appSettings->value("Theme", Theme::kDark).toString();
    qApp->setPalette(Theme::paletteFor(theme));
    applyThemeIcons(theme);

}


/*****************************************************************************/
/* Protected methods */
/*****************************************************************************/
void HexWalkMain::closeEvent(QCloseEvent *)
{
    writeSettings();
}


void HexWalkMain::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->accept();
}


void HexWalkMain::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> urls = event->mimeData()->urls();
        QString filePath = urls.at(0).toLocalFile();
        adjustForCurrentFile(filePath);
        loadFile(filePath);
        event->accept();
    }
}

void HexWalkMain::on_signedcb_clicked()
{
    updateInfo();
}

void HexWalkMain::showByteMap()
{
    byteMapDialog->showByteMapDialog();
}

void HexWalkMain::showDisasm()
{
    disasmWidget->show();
}
