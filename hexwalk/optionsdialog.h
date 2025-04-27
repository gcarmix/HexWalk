#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QtCore>
#include <QDialog>
#include <QAbstractButton>
namespace Ui {
    class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QSettings * appSettings,QWidget *parent = 0);
    ~OptionsDialog();
    Ui::OptionsDialog *ui;
    void show();

public slots:
    virtual void accept();

private slots:
    void on_pbHighlightingColor_clicked();
    void on_pbAddressAreaColor_clicked();
    void on_pbAddressFontColor_clicked();
    void on_pbAsciiAreaColor_clicked();
    void on_pbAsciiFontColor_clicked();
    void on_pbHexFontColor_clicked();
    void on_pbSelectionColor_clicked();
    void on_pbWidgetFont_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    void readSettings();
    void writeSettings();
    void setColor(QWidget *widget, QColor color);
    QSettings * appSettings;
};

#endif // OPTIONSDIALOG_H
