#ifndef UNVISUALKODE_H
#define UNVISUALKODE_H

#include <QMainWindow>
#include <QMap>
#include <QPlainTextEdit>
#include <QTabWidget>

#include "managers/tabsmanager.h"
#include "managers/filesmanager.h"
#include "managers/editorsmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UnvisualKode; }
QT_END_NAMESPACE

class UnvisualKode : public QMainWindow
{
    Q_OBJECT

public:
    UnvisualKode(QWidget *parent = nullptr);
    ~UnvisualKode();

private slots:
    void openSite();
    void openAbout();
    void newWindow();

private:
    Ui::UnvisualKode *ui;
    QMap<QPlainTextEdit*, TabInfo> tabs;
    
    TabsManager *m_tabsManager;
    FilesManager *m_filesManager;
    EditorsManager *m_editorsManager;

    void setupConnections();
    void handleArguments();
};

#endif // UNVISUALKODE_H