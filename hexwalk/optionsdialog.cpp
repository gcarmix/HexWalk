
#include <QColorDialog>
#include <QFontDialog>

#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QSettings * appSettings,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    this->appSettings = appSettings;

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

    ui->cbAddressArea->setChecked(appSettings->value("AddressArea").toBool());
    ui->cbAsciiArea->setChecked(appSettings->value("AsciiArea").toBool());
    ui->cbHighlighting->setChecked(appSettings->value("Highlighting").toBool());
    ui->cbOverwriteMode->setChecked(appSettings->value("OverwriteMode").toBool());
    ui->cbReadOnly->setChecked(appSettings->value("ReadOnly").toBool());

    setColor(ui->lbHighlightingColor, appSettings->value("HighlightingColor").value<QColor>());
    setColor(ui->lbAddressAreaColor, appSettings->value("AddressAreaColor").value<QColor>());
    setColor(ui->lbSelectionColor, appSettings->value("SelectionColor").value<QColor>());
    setColor(ui->lbAddressFontColor, appSettings->value("AddressFontColor").value<QColor>());
    setColor(ui->lbAsciiAreaColor, appSettings->value("AsciiAreaColor").value<QColor>());
    setColor(ui->lbAsciiFontColor, appSettings->value("AsciiFontColor").value<QColor>());
    setColor(ui->lbHexFontColor, appSettings->value("HexFontColor").value<QColor>());
#ifdef Q_OS_WIN32
    ui->leWidgetFont->setFont(appSettings->value("WidgetFont", QFont("Courier", 10)).value<QFont>());
#else
    ui->leWidgetFont->setFont(appSettings->value("WidgetFont", QFont("Monospace", 10)).value<QFont>());
#endif

    ui->sbAddressAreaWidth->setValue(appSettings->value("AddressAreaWidth").toInt());
    ui->sbBytesPerLine->setValue(appSettings->value("BytesPerLine").toInt());
}

void OptionsDialog::writeSettings()
{
    appSettings->setValue("AddressArea", ui->cbAddressArea->isChecked());
    appSettings->setValue("AsciiArea", ui->cbAsciiArea->isChecked());
    appSettings->setValue("Highlighting", ui->cbHighlighting->isChecked());
    appSettings->setValue("OverwriteMode", ui->cbOverwriteMode->isChecked());
    appSettings->setValue("ReadOnly", ui->cbReadOnly->isChecked());

    appSettings->setValue("HighlightingColor", ui->lbHighlightingColor->palette().color(QPalette::Background));
    appSettings->setValue("AddressAreaColor", ui->lbAddressAreaColor->palette().color(QPalette::Background));
    appSettings->setValue("SelectionColor", ui->lbSelectionColor->palette().color(QPalette::Background));
    appSettings->setValue("AddressFontColor", ui->lbAddressFontColor->palette().color(QPalette::Background));
    appSettings->setValue("AsciiAreaColor", ui->lbAsciiAreaColor->palette().color(QPalette::Background));
    appSettings->setValue("AsciiFontColor", ui->lbAsciiFontColor->palette().color(QPalette::Background));
    appSettings->setValue("HexFontColor", ui->lbHexFontColor->palette().color(QPalette::Background));
    appSettings->setValue("WidgetFont",ui->leWidgetFont->font());

    appSettings->setValue("AddressAreaWidth", ui->sbAddressAreaWidth->value());
    appSettings->setValue("BytesPerLine", ui->sbBytesPerLine->value());
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
    QColor color = QColorDialog::getColor(ui->lbAddressFontColor->palette().color(QPalette::Background), this);
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
    QColor color = QColorDialog::getColor(ui->lbAsciiFontColor->palette().color(QPalette::Background), this);
    if (color.isValid())
        setColor(ui->lbAsciiFontColor, color);
}

void OptionsDialog::on_pbHexFontColor_clicked()
{
    QColor color = QColorDialog::getColor(ui->lbHexFontColor->palette().color(QPalette::Background), this);
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

void OptionsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults))
    {
        ui->cbAddressArea->setChecked(true);
        ui->cbAsciiArea->setChecked(true);
        ui->cbHighlighting->setChecked(true);
        ui->cbOverwriteMode->setChecked(true);
        ui->cbReadOnly->setChecked(false);

        setColor(ui->lbHighlightingColor, QColor("#540c00"));
        setColor(ui->lbAddressAreaColor, QColor("#545454"));
        setColor(ui->lbSelectionColor, QColor("#0998c7"));
        setColor(ui->lbAddressFontColor, QColor("#f0f0f0"));
        setColor(ui->lbAsciiAreaColor, QColor("#424242"));
        setColor(ui->lbAsciiFontColor, QColor("#00ff5e"));
        setColor(ui->lbHexFontColor, QColor("#00ff5e"));
    #ifdef Q_OS_WIN32
        ui->leWidgetFont->setFont(QFont("Courier",12));
    #else
        ui->leWidgetFont->setFont(QFont("Monospace",12));
    #endif

        ui->sbAddressAreaWidth->setValue(6);
        ui->sbBytesPerLine->setValue(16);
        }
}

