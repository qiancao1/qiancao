#ifndef PLUGINDEPDIALOG_H
#define PLUGINDEPDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QProcess>
#include <QTextBrowser>

class PluginDepDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PluginDepDialog(const QStringList& requires, const QString& pluginName, QWidget *parent = nullptr);

private slots:
    void onInstallClicked();
    void onProcessOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError();

private:
    QListWidget *m_checkList;
    QPushButton *m_installBtn;
    QPushButton *m_cancelBtn;
    QLabel      *m_statusLabel;
    QTextBrowser *m_outputBrowser;   // 显示安装日志
    QProcess    *m_process;
    QStringList  m_pendingLibs;      // 待安装的库列表
    QString      m_currentLib;
    QString      m_pluginName;
    QString      m_pythonCmd;         // python 命令路径

    void startNextInstall();
    void appendOutput(const QString &text);
};

#endif // PLUGINDEPDIALOG_H