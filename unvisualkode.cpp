#include "unvisualkode.h"
#include "ui_unvisualkode.h"
#include <QDesktopServices>
#include <QUrl>
#include <aboutwindow.h>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QPlainTextEdit>
#include <QTabWidget>

UnvisualKode::UnvisualKode(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UnvisualKode)
{
    ui->setupUi(this);
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setMovable(true);

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested,
            this, &UnvisualKode::closeTab);

    newFile();

    connect(ui->actionSite, &QAction::triggered, this, &UnvisualKode::openSite);
    connect(ui->actionAbout, &QAction::triggered, this, &UnvisualKode::openAbout);
    connect(ui->actionNew_file, &QAction::triggered, this, &UnvisualKode::newFile);
    connect(ui->actionOpen_file, &QAction::triggered, this, &UnvisualKode::openFile);
    connect(ui->actionSave_file, &QAction::triggered, this, &UnvisualKode::saveFile);
    connect(ui->actionSave_file_as, &QAction::triggered, this, &UnvisualKode::saveFileAs);
}

UnvisualKode::~UnvisualKode()
{
    delete ui;
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

TabInfo* UnvisualKode::currentTab()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(
        ui->tabWidget->currentWidget()
        );

    if (!editor || !tabs.contains(editor)) {
        return nullptr;
    }

    return &tabs[editor];
}

QPlainTextEdit* UnvisualKode::createEditor(const QString &content)
{
    QPlainTextEdit *editor = new QPlainTextEdit(this);
    editor->setPlainText(content);
    editor->setFont(QFont("Courier New", 10));
    editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    connect(editor, &QPlainTextEdit::textChanged, this, &UnvisualKode::onTextChanged);
    return editor;
}

QString UnvisualKode::generateTabName(const QString &baseName)
{
    int count = 0;
    for (int i = 0; i < ui->tabWidget->count(); i++) {
        if (ui->tabWidget->tabText(i).startsWith(baseName)) {
            count++;
        }
    }
    return count == 0 ? baseName : QString("%1 %2").arg(baseName).arg(count + 1);
}

void UnvisualKode::addNewTab(QPlainTextEdit *editor, const QString &tabName, const QString &filePath)
{
    int index = ui->tabWidget->addTab(editor, tabName);
    ui->tabWidget->setCurrentIndex(index);

    TabInfo info;
    info.editor = editor;
    info.filePath = filePath;
    info.isModified = false;
    tabs[editor] = info;
}

bool UnvisualKode::saveToFile(const QString &filePath, const QString &content)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Couldn't save the file:\n" + file.errorString());
        return false;
    }
    QTextStream stream(&file);
    stream << content;
    file.close();
    return true;
}

QString UnvisualKode::getFileNameFromPath(const QString &filePath)
{
    return QFileInfo(filePath).fileName();
}

void UnvisualKode::newFile()
{
    QPlainTextEdit *editor = createEditor();
    QString tabName = generateTabName("Untilted");
    addNewTab(editor, tabName, "");
}

void UnvisualKode::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open file",
        QDir::homePath(),
        "Text files (*.txt);;All files (*.*)"
        );

    if (fileName.isEmpty()) return;

    for (auto it = tabs.begin(); it != tabs.end(); ++it) {
        if (it.value().filePath == fileName) {
            ui->tabWidget->setCurrentWidget(it.key());
            QMessageBox::information(this, "Info", "This file is already open!");
            return;
        }
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot open file:\n" + file.errorString());
        return;
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();

    QPlainTextEdit *editor = createEditor(content);
    QString tabName = getFileNameFromPath(fileName);
    addNewTab(editor, tabName, fileName);
}

void UnvisualKode::saveFile()
{
    TabInfo *tab = currentTab();

    if (!tab) {
        QMessageBox::warning(this, "Error", "No active tab!");
        return;
    }

    QString text = tab->editor->toPlainText();

    if (text.isEmpty()) {
        QMessageBox::information(this, "Information", "The file is empty, nothing to save!");
        return;
    }

    if (tab->filePath.isEmpty()) {
        saveFileAs();
        return;
    }

    if (saveToFile(tab->filePath, text)) {
        tab->isModified = false;
        updateTabTitle(ui->tabWidget->currentIndex());
    }
}

void UnvisualKode::saveFileAs()
{
    TabInfo *tab = currentTab();

    if (!tab) {
        QMessageBox::warning(this, "Error", "No active tab!");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this,
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
        int index = ui->tabWidget->currentIndex();
        ui->tabWidget->setTabText(index, getFileNameFromPath(fileName));
        qDebug() << "File saved as:" << fileName;
    }
}

void UnvisualKode::closeTab(int index)
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(
        ui->tabWidget->widget(index)
        );

    if (!editor || !tabs.contains(editor)) return;

    TabInfo &tab = tabs[editor];

    if (tab.isModified) {
        QString fileName = tab.filePath.isEmpty() ?
                               "Untilted" :
                               getFileNameFromPath(tab.filePath);

        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Save changes",
            QString("Save changes to '%1'?").arg(fileName),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

        if (reply == QMessageBox::Cancel) return;

        if (reply == QMessageBox::Yes) {
            int oldCurrent = ui->tabWidget->currentIndex();
            ui->tabWidget->setCurrentIndex(index);

            if (tab.filePath.isEmpty()) {
                saveFileAs();
            } else {
                saveFile();
            }

            ui->tabWidget->setCurrentIndex(oldCurrent);
        }
    }

    tabs.remove(editor);
    ui->tabWidget->removeTab(index);
    editor->deleteLater();

    if (ui->tabWidget->count() == 0) {
        newFile();
    }
}

void UnvisualKode::onTextChanged()
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(sender());
    if (!editor || !tabs.contains(editor)) return;

    TabInfo &tab = tabs[editor];

    if (!tab.isModified) {
        tab.isModified = true;
        int index = ui->tabWidget->indexOf(editor);
        if (index != -1) {
            updateTabTitle(index);
        }
    }
}

void UnvisualKode::updateTabTitle(int index)
{
    QPlainTextEdit *editor = qobject_cast<QPlainTextEdit*>(
        ui->tabWidget->widget(index)
        );

    if (!editor || !tabs.contains(editor)) return;

    TabInfo &tab = tabs[editor];
    QString title;

    if (tab.filePath.isEmpty()) {
        title = "Untilted";
    } else {
        title = getFileNameFromPath(tab.filePath);
    }

    if (tab.isModified) {
        title += " *";
    }

    ui->tabWidget->setTabText(index, title);
}