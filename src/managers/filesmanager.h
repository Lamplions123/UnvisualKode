#ifndef FILESMANAGER_H
#define FILESMANAGER_H

#include <QString>
#include <QWidget>
#include <QPlainTextEdit>
#include <QMap>
#include <QObject>
#include <QFileInfo>

struct TabInfo;
class TabsManager;
class EditorsManager;

class FilesManager : public QObject
{
    Q_OBJECT

public:
    FilesManager(QWidget *parent = nullptr, QMap<QPlainTextEdit*, TabInfo> *tabs = nullptr, 
                 TabsManager *tabsManager = nullptr, EditorsManager *editorsManager = nullptr);
    
    void newFile();
    void newFile(QFileInfo fileInfo);
    void openFile(QString fileName);
    void openFileGui();
    void saveFile();
    void saveFileAs();
    bool saveToFile(const QString &filePath, const QString &content);
    QString getFileNameFromPath(const QString &filePath);
    QString readFileContent(const QString &filePath);

private:
    QWidget *m_parent;
    QMap<QPlainTextEdit*, TabInfo> *m_tabs;
    TabsManager *m_tabsManager;
    EditorsManager *m_editorsManager;
};

#endif // FILESMANAGER_H
