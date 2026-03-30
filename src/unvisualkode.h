#ifndef UNVISUALKODE_H
#define UNVISUALKODE_H

#include <QMainWindow>
#include <QMap>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QTreeView>

#include "managers/tabsmanager.h"
#include "managers/filesmanager.h"
#include "managers/editorsmanager.h"
#include "managers/filetreemanager.h"

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
    void handleArguments();
    void toggleFileTree();

private:
    Ui::UnvisualKode *ui;
    QMap<QPlainTextEdit*, TabInfo> tabs;
    void initFileTree();
    
    TabsManager *m_tabsManager;
    FilesManager *m_filesManager;
    EditorsManager *m_editorsManager;
    FileTreeManager *m_fileTreeManager;

    void setupConnections();

public:
    QTreeView* getTreeView() const;
};

#endif // UNVISUALKODE_H