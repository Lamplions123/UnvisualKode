#include "tabsmanager.h"
#include "filesmanager.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QApplication>

TabsManager::TabsManager(QTabWidget *tabWidget, QMap<QPlainTextEdit*, TabInfo> &tabs, FilesManager *filesManager)
    : m_tabWidget(tabWidget)
    , m_tabs(tabs)
    , m_filesManager(filesManager)
{
}

TabInfo* TabsManager::currentTab()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(
        m_tabWidget->currentWidget()
        );

    if (!editor || !m_tabs.contains(editor)) {
        return nullptr;
    }

    return &m_tabs[editor];
}

QString TabsManager::generateTabName(const QString &baseName)
{
    int count = 0;
    for (int i = 0; i < m_tabWidget->count(); i++) {
        if (m_tabWidget->tabText(i).startsWith(baseName)) {
            count++;
        }
    }
    return count == 0 ? baseName : QString("%1 %2").arg(baseName).arg(count + 1);
}

void TabsManager::addNewTab(QPlainTextEdit *editor, const QString &tabName, const QString &filePath)
{
    int index = m_tabWidget->addTab(editor, tabName);
    m_tabWidget->setCurrentIndex(index);

    TabInfo info;
    info.editor = editor;
    info.filePath = filePath;
    info.isModified = false;
    m_tabs[editor] = info;
}

void TabsManager::closeTab(int index)
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(
        m_tabWidget->widget(index)
        );

    if (!editor || !m_tabs.contains(editor)) return;

    m_tabs.remove(editor);
    m_tabWidget->removeTab(index);
    editor->deleteLater();

    if (m_tabWidget->count() == 0) {
        QApplication::exit();
    }
}

void TabsManager::closeTabWithSaveCheck(int index)
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(
        m_tabWidget->widget(index)
        );

    if (!editor || !m_tabs.contains(editor)) return;

    TabInfo &tab = m_tabs[editor];

    if (tab.isModified) {
        QString fileName = tab.filePath.isEmpty() ?
                               "Untilted" :
                               QFileInfo(tab.filePath).fileName();

        QMessageBox::StandardButton reply = QMessageBox::question(
            m_tabWidget, "Save changes",
            QString("Save changes to '%1'?").arg(fileName),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

        if (reply == QMessageBox::Cancel) return;

        if (reply == QMessageBox::Yes) {
            if (m_filesManager) {
                if (tab.filePath.isEmpty()) {
                    m_filesManager->saveFileAs();
                } else {
                    m_filesManager->saveFile();
                }
            }
        }
    }

    closeTab(index);
}

void TabsManager::updateTabTitle(int index)
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(
        m_tabWidget->widget(index)
        );

    if (!editor || !m_tabs.contains(editor)) return;

    TabInfo &tab = m_tabs[editor];
    QString title;

    if (tab.filePath.isEmpty()) {
        title = "Untilted";
    } else {
        title = QFileInfo(tab.filePath).fileName();
    }

    if (tab.isModified) {
        title += " *";
    }

    m_tabWidget->setTabText(index, title);
}