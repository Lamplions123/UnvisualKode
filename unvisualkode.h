#ifndef UNVISUALKODE_H
#define UNVISUALKODE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class UnvisualKode;
}
QT_END_NAMESPACE

class UnvisualKode : public QMainWindow
{
    Q_OBJECT

public:
    UnvisualKode(QWidget *parent = nullptr);
    ~UnvisualKode();

private slots:
    void openSite();

private:
    Ui::UnvisualKode *ui;
};
#endif // UNVISUALKODE_H
