#include "filesmanager.h"
#include "tabsmanager.h"
#include "editorsmanager.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QTabWidget>
#include <QObject>
#include <qdebug.h>
#include <qfileinfo.h>

FilesManager::FilesManager(QWidget *parent, QMap<QPlainTextEdit*, TabInfo> *tabs, 
                           TabsManager *tabsManager, EditorsManager *editorsManager)
    : m_parent(parent)
    , m_tabs(tabs)
    , m_tabsManager(tabsManager)
    , m_editorsManager(editorsManager)
{
}

void FilesManager::newFile(QFileInfo fileInfo)
{   

    QPlainTextEdit *editor = m_editorsManager->createEditor();
    connect(editor, &QPlainTextEdit::textChanged, m_editorsManager, &EditorsManager::onTextChanged);
    m_tabsManager->addNewTab(editor, fileInfo.fileName(), fileInfo.filePath());
}

void FilesManager::newFile()
{
    QPlainTextEdit *editor = m_editorsManager->createEditor();
    connect(editor, &QPlainTextEdit::textChanged, m_editorsManager, &EditorsManager::onTextChanged);
    QString tabName = m_tabsManager->generateTabName("Untilted");
    m_tabsManager->addNewTab(editor, tabName, "");
}

void FilesManager::openFile(const QString &filePath)
{   
    
    QFileInfo fileInfo(filePath);
    if (fileInfo.isDir()) return;

    auto it = m_tabs->begin();
    while (it != m_tabs->end()) {
        if (it.value().filePath == filePath) {
            QTabWidget *tabWidget = m_tabsManager->getTabWidget();
            tabWidget->setCurrentIndex(tabWidget->indexOf(it.key()));
            return;  // File already open
        }
        ++it;
    }


    QFile file(filePath);

    QString content = readFileContent(filePath);

    QPlainTextEdit *editor = m_editorsManager->createEditor(content);
    connect(editor, &QPlainTextEdit::textChanged, m_editorsManager, &EditorsManager::onTextChanged);
    QString tabName = getFileNameFromPath(filePath);
    m_tabsManager->addNewTab(editor, tabName, filePath);
}

void FilesManager::openFileGui()
{
    QString fileName = QFileDialog::getOpenFileName(
        m_parent,
        "Open file",
        QDir::homePath(),
        "Text files (*.txt);;All files (*.*)"
        );

    if (fileName.isEmpty()) return;

    openFile(fileName);
}

// Will try to save to a "path". If path doesn't exist, will call saveFileAs
void FilesManager::saveFile()
{
    TabInfo *tab = m_tabsManager->currentTab();

    if (!tab) {
        QMessageBox::warning(m_parent, "Error", "No active tabs!");
        return;
    }

    QString text = tab->editor->toPlainText();

    if (tab->filePath.isEmpty()) {
        saveFileAs();
        return;
    }

    if (saveToFile(tab->filePath, text)) {
        tab->isModified = false;
        QTabWidget *tabWidget = m_tabsManager->getTabWidget();
        m_tabsManager->updateTabTitle(tabWidget->currentIndex());
    }
}

// Will overwrite filepath for an active tab
void FilesManager::saveFileAs()
{
    TabInfo *tab = m_tabsManager->currentTab();

    if (!tab) {
        QMessageBox::warning(m_parent, "Error", "No active tab!");
        return;
    }        qDebug() << "Save cancelled";

    QString fileName = QFileDialog::getSaveFileName(
        m_parent,
        "Save file as",
        QDir::homePath(),
        "Text files (*.txt);;All files (*.*)"
        );

    if (fileName.isEmpty()) {
        qDebug() << "Save cancelled";
        return;
    }

    if (saveToFile(fileName, tab->editor->toPlainText())) {
        tab->filePath = fileName;
        tab->isModified = false;
        QTabWidget *tabWidget = m_tabsManager->getTabWidget();
        int index = tabWidget->currentIndex();
        tabWidget->setTabText(index, getFileNameFromPath(fileName));
        qDebug() << "File saved as:" << fileName;
    }
}

// low-level function for saving
bool FilesManager::saveToFile(const QString &filePath, const QString &content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(m_parent, "Error", "Couldn't save the file:\n" + file.errorString());
        return false;
    }
    QTextStream stream(&file);
    stream << content;
    file.close();
    return true;
}

QString FilesManager::getFileNameFromPath(const QString &filePath)
{
    return QFileInfo(filePath).fileName();
}

QString FilesManager::readFileContent(const QString &filePath)
{   
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(m_parent, "Error", "Cannot open file:\n" + file.errorString());
        return QString();
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();
    return content;
}
