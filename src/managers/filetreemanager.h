#ifndef FILETREEMANAGER_H
#define FILETREEMANAGER_H

#include <QObject>
#include <QFileInfo>
#include <QStandardItem>
#include "src/managers/tabsmanager.h"
#include "src/managers/editorsmanager.h"
#include "src/managers/filesmanager.h"

class FileTreeManager;

class FileTreeManager : public QObject
{
    Q_OBJECT

public:
    FileTreeManager(QWidget *parent = nullptr, TabsManager *tabsManager = nullptr, EditorsManager *editorsManager = nullptr, FilesManager *FileManager = nullptr);
    QStandardItem* createStdItem(QString text, bool isDir, QString canonicalPath);
    void buildTree(QStandardItem *parentItem, QDir dir);
    
    void renderTree(QDir dir);
public slots:
    void itemClicked(const QModelIndex &index);

private:
    QWidget *m_parent;
    TabsManager *m_tabsManager;
    EditorsManager *m_editorsManager;
    FilesManager *m_fileManager;
};

#endif // FILETREEMANAGER_H
