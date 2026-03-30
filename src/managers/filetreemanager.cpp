#include "src/managers/filetreemanager.h"
#include "src/unvisualkode.h"
#include <QMessageBox>
#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <QStandardItemModel>
#include <qhashfunctions.h>
#include <qobject.h>
#include <qstandarditemmodel.h>
#include <qtreeview.h>

FileTreeManager::FileTreeManager(QWidget *parent, TabsManager *tabsManager, EditorsManager *editorsManager, FilesManager *FileManager)
    : m_parent(parent)
    , m_tabsManager(tabsManager)
    , m_editorsManager(editorsManager)
    , m_fileManager(FileManager)
{
}

void FileTreeManager::itemClicked(const QModelIndex &index){
    QString filePath = index.data(Qt::UserRole).toString();

    m_fileManager->openFile(filePath);
}

QStandardItem* FileTreeManager::createStdItem(QString text, bool isDir, QString path){
    QStandardItem* stdItem = new QStandardItem(text);
    stdItem->setEditable(false);
    stdItem->setSelectable(!isDir);
    stdItem->setData(path, Qt::UserRole); // Store the file path in the item

    return stdItem;
}

void FileTreeManager::buildTree(QStandardItem *parentItem, QDir dir) {
    QList<QFileInfo> children = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for (const QFileInfo &child : children) {
        QStandardItem *item = createStdItem(child.fileName(), child.isDir(), child.absoluteFilePath());
        parentItem->appendRow(item);
        
        if (child.isDir()) {
            buildTree(item, child.absoluteFilePath());
        }
    }
}

void FileTreeManager::renderTree(QDir dir)
{   
    UnvisualKode *mainWindow = qobject_cast<UnvisualKode*>(m_parent);

    if (!mainWindow) return;

    QTreeView *treeView = mainWindow->getTreeView();

    QStandardItemModel *model = new QStandardItemModel(0, 1); // creating root elemenet
    model->setHorizontalHeaderLabels({dir.dirName()}); // giving root element our current opened dir name
    QStandardItem *rootItem = model->invisibleRootItem(); // getting unknown root element (?)

    buildTree(rootItem, dir.path()); // calling recursion function

    treeView->setModel(model); // applying root to the treeView
    
    connect(treeView, &QTreeView::clicked,
        this, &FileTreeManager::itemClicked);
}
