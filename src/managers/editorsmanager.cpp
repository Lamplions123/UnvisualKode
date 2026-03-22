#include "editorsmanager.h"
#include "tabsmanager.h"

#include <QObject>
#include <QPlainTextEdit>
#include <QString>
#include <QTabWidget>
#include <QWidget>

EditorsManager::EditorsManager(QWidget *parent, QMap<QPlainTextEdit*, TabInfo> *tabs, TabsManager *tabsManager)
    : m_parent(parent)
    , m_tabs(tabs)
    , m_tabsManager(tabsManager)
{
}

QPlainTextEdit* EditorsManager::createEditor(const QString &content)
{
    QPlainTextEdit *editor = new QPlainTextEdit(m_parent);
    editor->setPlainText(content);
    return editor;
}

void EditorsManager::onTextChanged()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(sender());
    if (!editor || !m_tabs || !m_tabs->contains(editor)) return;

    TabInfo &tab = (*m_tabs)[editor];

    if (!tab.isModified) {
        tab.isModified = true;
        if (m_tabsManager) {

            QTabWidget *tabWidget = m_tabsManager->getTabWidget();
            if (tabWidget) {
                int index = tabWidget->indexOf(editor);
                if (index != -1) {
                    m_tabsManager->updateTabTitle(index);
                }
            }
        }
    }
}
