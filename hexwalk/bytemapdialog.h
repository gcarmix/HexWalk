#ifndef BYTEMAPDIALOG_H
#define BYTEMAPDIALOG_H

#include <QDialog>
#include "../src/qhexedit.h"
#include <QGraphicsScene>
namespace Ui {
class ByteMapDialog;
}

class ByteMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ByteMapDialog(QHexEdit * hexedit,QWidget *parent = nullptr);
    ~ByteMapDialog();
    void showByteMapDialog();

private slots:
    void on_buttonBox_accepted();
    void updatePos();
    void gotoAddress();
    void refresh();
    void on_comboCols_currentIndexChanged(int index);

private:
    int imageWidth;
    int imageHeight;
    Ui::ByteMapDialog *ui;
    QHexEdit * _hexedit;
    QGraphicsScene *scene;
};

#endif // BYTEMAPDIALOG_H
