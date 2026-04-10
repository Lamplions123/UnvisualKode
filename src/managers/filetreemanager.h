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
    void renderChildren(QStandardItem *parentItem, QDir dir);
    
    void renderTree(QDir dir);
    void openFolder();
    void closeFolder();
    void createDummy(QStandardItem *parent);
    QString getParentPath(const QModelIndex &index);
    QStandardItem* getItemFromIndex(const QModelIndex &index);
    void createNewItem(const QModelIndex &parentIndex, bool isFile);
public slots:
    void itemClicked(const QModelIndex &index);
    void itemExpanded(const QModelIndex &index);
    void showContextMenu(const QPoint &pos);
    void createNewFile(const QModelIndex &parentIndex);
    void createNewFolder(const QModelIndex &index);
    void deleteItem(const QModelIndex &index);
    void renameItem(const QModelIndex &index);
    void refreshDirectory(const QModelIndex &index);
    void itemRenamed(QStandardItem *item);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QWidget *m_parent;
    TabsManager *m_tabsManager;
    EditorsManager *m_editorsManager;
    FilesManager *m_fileManager;
    UnvisualKode *mainWindow;
};

#endif // FILETREEMANAGER_H
