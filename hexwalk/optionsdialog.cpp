
#include <QColorDialog>
#include <QFontDialog>

#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    //readSettings();
    //writeSettings();
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::show()
{
    readSettings();
    QWidget::show();
}

void OptionsDialog::accept()
{
    writeSettings();
    emit accepted();
    QDialog::hide();
}

void OptionsDialog::readSettings()
{
    QSettings settings;

    ui->cbAddressArea->setChecked(settings.value("AddressArea", true).toBool());
    ui->cbAsciiArea->setChecked(settings.value("AsciiArea", true).toBool());
    ui->cbHighlighting->setChecked(settings.value("Highlighting", true).toBool());
    ui->cbOverwriteMode->setChecked(settings.value("OverwriteMode", true).toBool());
    ui->cbReadOnly->setChecked(settings.value("ReadOnly").toBool());

    setColor(ui->lbHighlightingColor, settings.value("HighlightingColor", QColor(0xff, 0xff, 0x99, 0xff)).value<QColor>());
    setColor(ui->lbAddressAreaColor, settings.value("AddressAreaColor", this->palette().alternateBase().color()).value<QColor>());
    setColor(ui->lbSelectionColor, settings.value("SelectionColor", this->palette().highlight().color()).value<QColor>());
    setColor(ui->lbAddressFontColor, settings.value("AddressFontColor", QPalette::WindowText).value<QColor>());
    setColor(ui->lbAsciiAreaColor, settings.value("AsciiAreaColor", this->palette().alternateBase().color()).value<QColor>());
    setColor(ui->lbAsciiFontColor, settings.value("AsciiFontColor", QPalette::WindowText).value<QColor>());
    setColor(ui->lbHexFontColor, settings.value("HexFontColor", QPalette::WindowText).value<QColor>());
#ifdef Q_OS_WIN32
    ui->leWidgetFont->setFont(settings.value("WidgetFont", QFont("Courier", 10)).value<QFont>());
#else
    ui->leWidgetFont->setFont(settings.value("WidgetFont", QFont("Monospace", 10)).value<QFont>());
#endif

    ui->sbAddressAreaWidth->setValue(settings.value("AddressAreaWidth", 4).toInt());
    ui->sbBytesPerLine->setValue(settings.value("BytesPerLine", 16).toInt());
}

void OptionsDialog::writeSettings()
{
    QSettings settings;
    settings.setValue("AddressArea", ui->cbAddressArea->isChecked());
    settings.setValue("AsciiArea", ui->cbAsciiArea->isChecked());
    settings.setValue("Highlighting", ui->cbHighlighting->isChecked());
    settings.setValue("OverwriteMode", ui->cbOverwriteMode->isChecked());
    settings.setValue("ReadOnly", ui->cbReadOnly->isChecked());

    settings.setValue("HighlightingColor", ui->lbHighlightingColor->palette().color(QPalette::Background));
    settings.setValue("AddressAreaColor", ui->lbAddressAreaColor->palette().color(QPalette::Background));
    settings.setValue("SelectionColor", ui->lbSelectionColor->palette().color(QPalette::Background));
    settings.setValue("AddressFontColor", ui->lbAddressFontColor->palette().color(QPalette::Background));
    settings.setValue("AsciiAreaColor", ui->lbAsciiAreaColor->palette().color(QPalette::Background));
    settings.setValue("AsciiFontColor", ui->lbAsciiFontColor->palette().color(QPalette::Background));
    settings.setValue("HexFontColor", ui->lbHexFontColor->palette().color(QPalette::Background));
    settings.setValue("WidgetFont",ui->leWidgetFont->font());

    settings.setValue("AddressAreaWidth", ui->sbAddressAreaWidth->value());
    settings.setValue("BytesPerLine", ui->sbBytesPerLine->value());
}

void OptionsDialog::setColor(QWidget *widget, QColor color)
{
    QPalette palette = widget->palette();
    palette.setColor(QPalette::Background, color);
    widget->setPalette(palette);
    widget->setAutoFillBackground(true);
}

void OptionsDialog::on_pbHighlightingColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->lbHighlightingColor->palette().color(QPalette::Background), this);
    if (color.isValid())
        setColor(ui->lbHighlightingColor, color);
}

void OptionsDialog::on_pbAddressAreaColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->lbAddressAreaColor->palette().color(QPalette::Background), this);
    if (color.isValid())
        setColor(ui->lbAddressAreaColor, color);
}

void OptionsDialog::on_pbAddressFontColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->lbAddressFontColor->palette().color(QPalette::WindowText), this);
    if (color.isValid())
        setColor(ui->lbAddressFontColor, color);
}

void OptionsDialog::on_pbAsciiAreaColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->lbAsciiAreaColor->palette().color(QPalette::Background), this);
    if (color.isValid())
        setColor(ui->lbAsciiAreaColor, color);
}

void OptionsDialog::on_pbAsciiFontColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->lbAsciiFontColor->palette().color(QPalette::WindowText), this);
    if (color.isValid())
        setColor(ui->lbAsciiFontColor, color);
}

void OptionsDialog::on_pbHexFontColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->lbHexFontColor->palette().color(QPalette::WindowText), this);
    if (color.isValid())
        setColor(ui->lbHexFontColor, color);
}

void OptionsDialog::on_pbSelectionColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->lbSelectionColor->palette().color(QPalette::Background), this);
    if (color.isValid())
        setColor(ui->lbSelectionColor, color);
}

void OptionsDialog::on_pbWidgetFont_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, ui->leWidgetFont->font(), this);
    if (ok)
        ui->leWidgetFont->setFont(font);
}
