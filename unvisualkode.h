#ifndef UNVISUALKODE_H
#define UNVISUALKODE_H

#include <QMainWindow>
#include <QMap>
#include <QPlainTextEdit>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class UnvisualKode; }
QT_END_NAMESPACE

struct TabInfo {
    QPlainTextEdit *editor;
    QString filePath;
    bool isModified;
};

class UnvisualKode : public QMainWindow
{
    Q_OBJECT

public:
    UnvisualKode(QWidget *parent = nullptr);
    ~UnvisualKode();

private slots:
    void openSite();
    void openAbout();
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void closeTab(int index);
    void onTextChanged();

private:
    Ui::UnvisualKode *ui;
    QMap<QPlainTextEdit*, TabInfo> tabs;

    TabInfo* currentTab();
    void updateTabTitle(int index);
    QPlainTextEdit* createEditor(const QString &content = "");
    QString generateTabName(const QString &baseName);
    void addNewTab(QPlainTextEdit *editor, const QString &tabName, const QString &filePath);
    bool saveToFile(const QString &filePath, const QString &content);
    QString getFileNameFromPath(const QString &filePath);
};

#endif // UNVISUALKODE_H