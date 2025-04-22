#ifndef DISASMWIDGET_H
#define DISASMWIDGET_H

#include <QDockWidget>
#include "../src/qhexedit.h"
namespace Ui {
class DisasmWidget;
}

class DisasmWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DisasmWidget(QHexEdit * hexedit,QWidget *parent = nullptr);
    ~DisasmWidget();
    void capst(void);
    void refresh(void);
private slots:
    void on_btnDisasm_clicked();
private:
    Ui::DisasmWidget *ui;
    QHexEdit * _hexedit;
};

#endif // DISASMWIDGET_H
