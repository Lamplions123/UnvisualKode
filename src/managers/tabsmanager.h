#ifndef TABSMANAGER_H
#define TABSMANAGER_H

#include <QString>
#include <QTabWidget>
#include <QMap>
#include <QPlainTextEdit>
#include <QObject>

struct TabInfo {
    QPlainTextEdit *editor;
    QString filePath;
    bool isModified;
};

class FilesManager;

class TabsManager : public QObject
{
    Q_OBJECT

public:
    TabsManager(QTabWidget *tabWidget, QMap<QPlainTextEdit*, TabInfo> &tabs, FilesManager *filesManager = nullptr);
    
    TabInfo* currentTab();
    QString generateTabName(const QString &baseName);
    void addNewTab(QPlainTextEdit *editor, const QString &tabName, const QString &filePath);
    void closeTab(int index);
    void closeTabWithSaveCheck(int index);
    void updateTabTitle(int index);
    QTabWidget* getTabWidget() const { return m_tabWidget; }
    void setFilesManager(FilesManager *filesManager) { m_filesManager = filesManager; }

private:
    QTabWidget *m_tabWidget;
    QMap<QPlainTextEdit*, TabInfo> &m_tabs;
    FilesManager *m_filesManager;
};

#endif // TABSMANAGER_H
