#include "unvisualkode.h"
#include "ui_unvisualkode.h"
#include <QDesktopServices>
#include <QUrl>

UnvisualKode::UnvisualKode(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UnvisualKode)
{
    ui->setupUi(this);

    connect(ui->actionSite, &QAction::triggered,
            this, &UnvisualKode::openSite);
}

UnvisualKode::~UnvisualKode()
{
    delete ui;
}

void UnvisualKode::openSite()
{
    QUrl url("https://github.com/Lamplions123/UnvisualKode");
    QDesktopServices::openUrl(url);
}
