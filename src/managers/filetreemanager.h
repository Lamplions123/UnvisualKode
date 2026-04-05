#ifndef FILETREEMANAGER_H
#define FILETREEMANAGER_H

#include <QObject>
#include <QFileInfo>
#include <QStandardItem>
#include "tabsmanager.h"
#include "editorsmanager.h"
#include "filesmanager.h"

class UnvisualKode;

class FileTreeManager : public QObject
{
    Q_OBJECT
    
public:
    FileTreeManager(QWidget *parent = nullptr, TabsManager *tabsManager = nullptr, EditorsManager *editorsManager = nullptr, FilesManager *FileManager = nullptr);
    QStandardItem* createStdItem(QString text, bool isDir, QString canonicalPath);
    void buildTree(QStandardItem *parentItem, QDir dir);
    
    void renderTree(QDir dir);
    void openFolder();
    void closeFolder();
public slots:
    void itemClicked(const QModelIndex &index);
    void onItemExpanded();

private:
    QWidget *m_parent;
    TabsManager *m_tabsManager;
    EditorsManager *m_editorsManager;
    FilesManager *m_fileManager;
    UnvisualKode *mainWindow;
};

#endif // FILETREEMANAGER_H
