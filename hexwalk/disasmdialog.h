#ifndef DISASMDIALOG_H
#define DISASMDIALOG_H

#include <QDialog>
#include "../src/qhexedit.h"
namespace Ui {
class DisasmDialog;
}

class DisasmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisasmDialog(QHexEdit * hexedit,QWidget *parent = nullptr);
    ~DisasmDialog();
    void capst(void);
    void refresh(void);

private slots:
    void on_btnClose_clicked();

    void on_btnDisasm_clicked();

private:
    Ui::DisasmDialog *ui;
    QHexEdit * _hexedit;
};

#endif // DISASMDIALOG_H
