#include "src/managers/filetreemanager.h"
#include "src/unvisualkode.h"
#include "ui_unvisualkode.h"
#include <QMessageBox>
#include <qabstractitemmodel.h>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <QStandardItemModel>
#include <qhashfunctions.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsizegrip.h>
#include <qstandarditemmodel.h>
#include <qtreeview.h>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>

FileTreeManager::FileTreeManager(QWidget *parent, TabsManager *tabsManager, EditorsManager *editorsManager, FilesManager *FileManager)
    : m_parent(parent)
    , m_tabsManager(tabsManager)
    , m_editorsManager(editorsManager)
    , m_fileManager(FileManager)
    , mainWindow(qobject_cast<UnvisualKode*>(m_parent))
{
}

void FileTreeManager::itemClicked(const QModelIndex &index)
{
    QString filePath = index.data(Qt::UserRole).toString();
    m_fileManager->openFile(filePath);
}

QStandardItem* FileTreeManager::createStdItem(QString text, bool isDir, QString path)
{
    QStandardItem* stdItem = new QStandardItem(text);
    //stdItem->setEditable(false);
    stdItem->setSelectable(!isDir);
    stdItem->setData(path, Qt::UserRole);

    return stdItem;
}

void FileTreeManager::renderChildren(QStandardItem *parentItem, QDir dir)
{
    QList<QFileInfo> children = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for (const QFileInfo &child : children) {
        QStandardItem *item = createStdItem(child.fileName(), child.isDir(), child.absoluteFilePath());
        parentItem->appendRow(item);

        if (child.isDir()) {
            QDir subDir(child.absoluteFilePath());
            bool isEmpty = subDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty();
            
            if (!isEmpty) {
                createDummy(item);
            }
        }
    }
}

void FileTreeManager::renderTree(QDir dir)
{   
    mainWindow->ui->treeView->setVisible(true);
    mainWindow->ui->treeView->setHeaderHidden(true);
    mainWindow->ui->actionClose_folder->setEnabled(true);
    mainWindow->ui->treeView->installEventFilter(this);
    
    QStandardItemModel *model = new QStandardItemModel(0, 1);
    QStandardItem *invisibleRoot = model->invisibleRootItem();
    
    QStandardItem *rootItem = createStdItem(dir.dirName(), true, dir.path());
    invisibleRoot->appendRow(rootItem);
    
    mainWindow->ui->treeView->setModel(model);
    mainWindow->ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    mainWindow->ui->treeView->expand(model->indexFromItem(rootItem));

    renderChildren(rootItem, dir.path());

    connect(mainWindow->ui->treeView, &QTreeView::clicked,
        this, &FileTreeManager::itemClicked);
    connect(mainWindow->ui->treeView, &QTreeView::expanded, 
        this, &FileTreeManager::itemExpanded);
    connect(mainWindow->ui->treeView, &QTreeView::customContextMenuRequested, 
        this, &FileTreeManager::showContextMenu);
    
    connect(model, &QStandardItemModel::itemChanged,
        this, &FileTreeManager::itemRenamed);
}
void FileTreeManager::closeFolder()
{
    QTreeView *treeView = mainWindow->ui->treeView;
    treeView->setModel(nullptr);
    mainWindow->ui->actionClose_folder->setEnabled(false);
    mainWindow->ui->treeView->setVisible(false);
    mainWindow->syncTreeViewAction();
}

void FileTreeManager::openFolder()
{
    QString dir = QFileDialog::getExistingDirectory(
        m_parent,
        "Open a folder",
        QDir::homePath()
    );

    if (dir.isEmpty()) return;
    renderTree(QDir(dir));
}

void FileTreeManager::itemExpanded(const QModelIndex &index)
{  
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(
        const_cast<QAbstractItemModel*>(index.model())
    );
    
    QStandardItem *item = model->itemFromIndex(index);
    QString path = item->data(Qt::UserRole).toString();
    QFileInfo info(path);
    
    QStandardItem *firstChild = item->child(0);
    if (firstChild && firstChild->data(Qt::UserRole + 1).toBool()) {
        item->removeRow(0);
        renderChildren(item, QDir(path));
    }
}

void FileTreeManager::createDummy(QStandardItem *parent)
{
    QStandardItem *dummy = new QStandardItem("if you're seeing this one, your computer is probably slow lmao");
    dummy->setData(true, Qt::UserRole + 1);
    parent->appendRow(dummy);
}




// -- context menu -- //

void FileTreeManager::showContextMenu(const QPoint &pos)
{
    // block duplicate context menus (sometimes happpens)
    static QDateTime lastCall;
    if (lastCall.isValid() && lastCall.msecsTo(QDateTime::currentDateTime()) < 100) {
        return;
    }
    lastCall = QDateTime::currentDateTime();
    
    QModelIndex index = mainWindow->ui->treeView->indexAt(pos);
    QMenu menu(mainWindow);
    
    auto addNewItemActions = [&menu](bool withSeparator = false) {
        if (withSeparator) menu.addSeparator();
        menu.addAction("New file");
        menu.addAction("New folder");
    };
    
    auto addModifyActions = [&menu](bool withSeparator = false) {
        if (withSeparator) menu.addSeparator();
        menu.addAction("Rename");
        menu.addAction("Delete");
    };
    
    auto handleSelection = [this](QAction *selected, const QModelIndex &targetIndex) {
        if (!selected) return;
        
        QString actionText = selected->text();
        
        if (actionText == "New file") {
            createNewFile(targetIndex);
        } else if (actionText == "New folder") {
            createNewFolder(targetIndex);
        } else if (actionText == "Rename") {
            renameItem(targetIndex);
        } else if (actionText == "Delete") {
            deleteItem(targetIndex);
        }
    };
    
    if (index.isValid()) { // clicked on a file/folder
        QStandardItem *item = static_cast<QStandardItemModel*>(mainWindow->ui->treeView->model())->itemFromIndex(index);
        if (item) {
            QString path = item->data(Qt::UserRole).toString();
            bool isDir = QFileInfo(path).isDir();
            
            if (isDir) {
                addNewItemActions(false);
                addModifyActions(true);
            } else {
                addModifyActions(false);
            }
        }
        
        QAction *selected = menu.exec(mainWindow->ui->treeView->viewport()->mapToGlobal(pos));
        handleSelection(selected, index);
        
    } else { // clicked on empty space
        addNewItemActions(false);
        
        QAction *selected = menu.exec(mainWindow->ui->treeView->viewport()->mapToGlobal(pos));
        handleSelection(selected, QModelIndex());
    }
}

QString FileTreeManager::getParentPath(const QModelIndex &index)
{
    if (index.isValid()) {
        return index.data(Qt::UserRole).toString();
    }
    
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(
        mainWindow->ui->treeView->model()
    );
    if (!model) return QString();
    
    QStandardItem *rootItem = model->invisibleRootItem()->child(0);
    if (rootItem) {
        return rootItem->data(Qt::UserRole).toString();
    }
    
    return QString();
}

QStandardItem* FileTreeManager::getItemFromIndex(const QModelIndex &index)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(
        const_cast<QAbstractItemModel*>(mainWindow->ui->treeView->model())
    );
    if (!model) return nullptr;
    
    if (index.isValid()) {
        return model->itemFromIndex(index);
    }
    
    return model->invisibleRootItem()->child(0);
}

void FileTreeManager::refreshDirectory(const QModelIndex &index)
{
    QStandardItem *item = getItemFromIndex(index);
    if (!item) return;
    
    item->removeRows(0, item->rowCount());
    QString path = item->data(Qt::UserRole).toString();
    renderChildren(item, QDir(path));
}

void FileTreeManager::createNewItem(const QModelIndex &parentIndex, bool isFile)
{
    QString parentPath = getParentPath(parentIndex);
    if (parentPath.isEmpty()) return;
    
    QString dialogTitle = isFile ? "New file" : "New folder";
    QString defaultName = isFile ? "new_file.txt" : "new_folder";
    QString errorMsg = isFile ? "Couldn't create file, (admin rules?)" : "Couldn't create folder, (admin rules?)";
    
    bool ok;
    QString name = QInputDialog::getText(m_parent, dialogTitle, 
                                         "Name:", QLineEdit::Normal, 
                                         defaultName, &ok);
    if (!ok || name.isEmpty()) return;
    
    QString fullPath = parentPath + "/" + name;
    
    bool success = false;
    if (isFile) {
        QFile file(fullPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            success = true;
        }
    } else {
        QDir dir;
        success = dir.mkdir(fullPath);
    }
    
    if (success) {
        refreshDirectory(parentIndex);
    } else {
        QMessageBox::warning(m_parent, "Error", errorMsg);
    }
}

void FileTreeManager::createNewFile(const QModelIndex &parentIndex)
{
    createNewItem(parentIndex, true);
}

void FileTreeManager::createNewFolder(const QModelIndex &parentIndex)
{
    createNewItem(parentIndex, false);
}

void FileTreeManager::deleteItem(const QModelIndex &index)
{
    QStandardItem *item = getItemFromIndex(index);
    
    QString path = item->data(Qt::UserRole).toString();
    QFileInfo info(path);
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        m_parent,
        "Confirm Delete",
        QString("Are you sure you want to delete '%1'?").arg(info.fileName()),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply != QMessageBox::Yes) return;
    
    bool success = info.isDir() ? QDir(path).removeRecursively() : QFile::remove(path);
    
    if (success) {
        QStandardItem *parentItem = item->parent();
        if (!parentItem) {
            parentItem = static_cast<QStandardItemModel*>(mainWindow->ui->treeView->model())->invisibleRootItem();
        }
        parentItem->removeRow(item->row());
    } else {
        QMessageBox::warning(m_parent, "Error", QString("Couldn't delete '%1'").arg(info.fileName()));
    }
}

void FileTreeManager::renameItem(const QModelIndex &index)
{
    mainWindow->ui->treeView->edit(index);
}

void FileTreeManager::itemRenamed(QStandardItem *item)
{
    QString oldPath = item->data(Qt::UserRole).toString();
    QString newName = item->text();
    
    QFileInfo oldFileInfo(oldPath);
    QString newPath = oldFileInfo.absolutePath() + "/" + newName;
    
    if (oldPath == newPath) return;
    
    if (QFileInfo::exists(newPath)) {
        QMessageBox::warning(m_parent, "Error", 
                             QString("'%1' already exists!").arg(newName));
        item->setText(oldFileInfo.fileName());
        return;
    }
    
    if (QFile::rename(oldPath, newPath)) {
        item->setData(newPath, Qt::UserRole);
    } else {
        QMessageBox::warning(m_parent, "Error", "Couldn't rename file!");
        item->setText(oldFileInfo.fileName());
    }
}

// -- keyboard handling -- //

bool FileTreeManager::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == mainWindow->ui->treeView && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        
        if (keyEvent->key() == Qt::Key_Delete) {
            QModelIndex currentIndex = mainWindow->ui->treeView->currentIndex();
            if (currentIndex.isValid()) {
                deleteItem(currentIndex);
                return true; // event handled
            }
        }
    }
    
    return QObject::eventFilter(obj, event);
}