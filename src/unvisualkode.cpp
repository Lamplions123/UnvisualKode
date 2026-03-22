#include "unvisualkode.h"
#include "ui_unvisualkode.h"
#include "aboutwindow.h"

#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QProcess>


UnvisualKode::UnvisualKode(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UnvisualKode)
{
    ui->setupUi(this);
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setMovable(true);

    // Initialize managers
    m_tabsManager = new TabsManager(ui->tabWidget, tabs, nullptr);
    m_editorsManager = new EditorsManager(this, &tabs, m_tabsManager);
    m_filesManager = new FilesManager(this, &tabs, m_tabsManager, m_editorsManager);
    m_tabsManager->setFilesManager(m_filesManager);

    setupConnections();
    handleArguments();
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
    connect(ui->actionNew_file, &QAction::triggered, m_filesManager, &FilesManager::newFile);
    connect(ui->actionOpen_file, &QAction::triggered, m_filesManager, &FilesManager::openFileGui);
    connect(ui->actionSave_file, &QAction::triggered, m_filesManager, &FilesManager::saveFile);
    connect(ui->actionSave_file_as, &QAction::triggered, m_filesManager, &FilesManager::saveFileAs);
    connect(ui->actionNew_window, &QAction::triggered, this, &UnvisualKode::newWindow);
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
        // and it's probably a file to edit
        m_filesManager->openFile(arguments.at(1));
    }
    else {
        // Looks like we can't work with it
        qDebug() << "You shouldn't send more than 1 argument (path)";
    }
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