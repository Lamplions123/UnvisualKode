#ifndef EDITORSMANAGER_H
#define EDITORSMANAGER_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QMap>
#include <QObject>

struct TabInfo;
class TabsManager;

class EditorsManager : public QObject
{
    Q_OBJECT

public:
    EditorsManager(QWidget *parent = nullptr, QMap<QPlainTextEdit*, TabInfo> *tabs = nullptr, TabsManager *tabsManager = nullptr);
    
    QPlainTextEdit* createEditor(const QString &content = "");
    void onTextChanged();
    void setTabsManager(TabsManager *tabsManager) { m_tabsManager = tabsManager; }

private:
    QWidget *m_parent;
    QMap<QPlainTextEdit*, TabInfo> *m_tabs;
    TabsManager *m_tabsManager;
};

#endif // EDITORSMANAGER_H
