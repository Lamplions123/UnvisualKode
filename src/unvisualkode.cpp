#include "unvisualkode.h"
#include "src/managers/filetreemanager.h"
#include "src/managers/tabsmanager.h"
#include "ui_unvisualkode.h"
#include "aboutwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <qaction.h>
#include <qcoreevent.h>
#include <qdir.h>
#include <QSizeGrip>
#include <QVBoxLayout>
#include <qtreeview.h>
#include <qwidget.h>


UnvisualKode::UnvisualKode(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UnvisualKode)
{
    ui->setupUi(this);
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setMovable(true);

    m_tabsManager = new TabsManager(ui->tabWidget, tabs, nullptr);
    m_editorsManager = new EditorsManager(this, &tabs, m_tabsManager);
    m_filesManager = new FilesManager(this, &tabs, m_tabsManager, m_editorsManager);
    m_fileTreeManager = new FileTreeManager(this, m_tabsManager, m_editorsManager, m_filesManager);
    ui->treeView->setVisible(false);

    setupConnections();
    handleArguments();
    initFileTree();
}

UnvisualKode::~UnvisualKode()
{
    delete ui;
}

void UnvisualKode::setupConnections()
{
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, m_tabsManager, &TabsManager::closeTabWithSaveCheck);
    connect(ui->actionSite, &QAction::triggered, this, &UnvisualKode::openSite);
    connect(ui->actionAbout, &QAction::triggered, this, &UnvisualKode::openAbout);
    connect(ui->actionNew_file, &QAction::triggered, m_filesManager, [this]() { m_filesManager->newFile(); }); // idk how this works
    connect(ui->actionOpen_file, &QAction::triggered, m_filesManager, &FilesManager::openFileGui);
    connect(ui->actionSave_file, &QAction::triggered, m_filesManager, &FilesManager::saveFile);
    connect(ui->actionSave_file_as, &QAction::triggered, m_filesManager, &FilesManager::saveFileAs);
    connect(ui->actionNew_window, &QAction::triggered, this, &UnvisualKode::newWindow);
    connect(ui->actionFile_Tree, &QAction::triggered, this, &UnvisualKode::toggleFileTree);
    connect(ui->actionOpen_folder, &QAction::triggered, m_fileTreeManager, &FileTreeManager::openFolder);
    connect(ui->actionClose_folder, &QAction::triggered, m_fileTreeManager, &FileTreeManager::closeFolder);
    
    ui->treeView->viewport()->installEventFilter(this);
}

void UnvisualKode::syncTreeViewAction()
{
    ui->actionFile_Tree->setChecked(ui->treeView->isVisible());
}

void UnvisualKode::handleArguments()
{
    QStringList arguments = QCoreApplication::arguments();
    // argument at index 1 is program's path

    if (arguments.size() == 1) {
        // we've 0 arguments to work with
        m_filesManager->newFile();
    }
    else if (arguments.size() == 2) {
        // we've 1 arguments to work with
        // and it's probably a file to edit, or a file to create
        QFileInfo fileInfo = QFileInfo(arguments.at(1));

        if (!fileInfo.exists())
        {
            m_filesManager->newFile(fileInfo);
            return;
        }

        if (fileInfo.isDir())
        {
            m_fileTreeManager->renderTree(fileInfo.dir());
            ui->actionFile_Tree->setChecked(true);
            ui->treeView->setVisible(true);
            return;
        }

        m_filesManager->openFile(arguments.at(1));
    }
    else {
        // Looks like we can't work with it
        qDebug() << "You shouldn't send more than 1 argument (path)";
    }
}

void UnvisualKode::initFileTree()
{
    // some resize grip code...
}

void UnvisualKode::toggleFileTree()
{   
    ui->treeView->setVisible(!ui->treeView->isVisible());
}

void UnvisualKode::openSite()
{
    QUrl url("https://github.com/Lamplions123/UnvisualKode");
    QDesktopServices::openUrl(url);
}

void UnvisualKode::openAbout()
{
    aboutWindow *aw = new aboutWindow(this);
    aw->show();
}

void UnvisualKode::newWindow()
{
    QString program = QApplication::applicationFilePath();

    QProcess *process = new QProcess(nullptr);
    process->start(program);
}

bool UnvisualKode::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->treeView->viewport()) {
        if (event->type() == QEvent::Show || event->type() == QEvent::Hide) {
            qDebug() << "qweqwe";
            syncTreeViewAction();
        }
    }
    
    return QMainWindow::eventFilter(watched, event);
}

QTreeView* UnvisualKode::getTreeView() const
{
    return ui->treeView;
}
