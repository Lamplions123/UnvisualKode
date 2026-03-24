#include "editorsmanager.h"
#include "tabsmanager.h"

#include <QObject>
#include <QPlainTextEdit>
#include <QString>
#include <QTabWidget>
#include <QWidget>
#include <qdebug.h>

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
    editor->installEventFilter(this);
    updateTabStop(editor);

    return editor;
}

bool EditorsManager::eventFilter(QObject *obj, QEvent *event)
{
    // ctrl + shortcut
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(obj);
    if (!editor) {
        return QObject::eventFilter(obj, event);
    }
    
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            
        if (keyEvent->modifiers() & Qt::ControlModifier) {
            if (keyEvent->text() == "+" || 
                keyEvent->key() == Qt::Key_Plus ||
                keyEvent->key() == Qt::Key_Equal) {
                zoomEditor(editor, false);
                return true;
            } else if (keyEvent->text() == "-" || 
                     keyEvent->key() == Qt::Key_Minus) {
                zoomEditor(editor, true);
                return true;
            }
        }
    }
    
    // ctrl + mouse
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);

        if (wheelEvent->modifiers() & Qt::ControlModifier) {
            if (wheelEvent->angleDelta().y() > 0) {
                zoomEditor(editor, false);
            } else {
                zoomEditor(editor, true);
            }
            return true;
        }
    }
    
    return QObject::eventFilter(obj, event);
}

void EditorsManager::zoomEditor(QPlainTextEdit *editor, bool out)
{
    if (!editor) return;
    
    QFont font = editor->font();
    int currentSize = font.pointSize();
    
    const int MAX_FONT_SIZE = 72;
    const int MIN_FONT_SIZE = 6;

    if (out)
    {
        if (currentSize > MIN_FONT_SIZE) {
            font.setPointSize(currentSize - 1);
        }
    }
    else
    {
        if (currentSize < MAX_FONT_SIZE) {
            font.setPointSize(currentSize + 1);
        }
    }
    editor->setFont(font);

    updateTabStop(editor);
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

void EditorsManager::updateTabStop(QPlainTextEdit *editor)
{
    if (!editor) return;
    
    QFontMetrics fm(editor->font());
    int tabWidth = fm.horizontalAdvance(' ') * 4;  // 4 spaces
    editor->setTabStopDistance(tabWidth);
}