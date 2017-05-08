#ifndef ADDNEWDIALOG_H
#define ADDNEWDIALOG_H

#include <QDialog>

#include "scanmodel.h"
#include "fpropsdialog.h"
#include "qpefile.h"
#include "qpefilemenu.h"

namespace Ui {
class addNewDialog;
}

class addNewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addNewDialog(QWidget *parent = 0);
    ~addNewDialog();
    QString getFamily();
    int getIndex();
public slots:
    void tvPositiveContextMenuRequested(const QPoint &pos);
    void trySampleSlot(bool checked);

private:
    Ui::addNewDialog *ui;
    QPeFileMenu *menu_PE;
};

#endif // ADDNEWDIALOG_H
