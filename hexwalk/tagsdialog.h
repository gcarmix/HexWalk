#ifndef TAGSDIALOG_H
#define TAGSDIALOG_H

#include <QDialog>

namespace Ui {
class TagsDialog;
}

class TagsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagsDialog(QWidget *parent = nullptr);
    ~TagsDialog();

private:
    Ui::TagsDialog *ui;
};

#endif // TAGSDIALOG_H
