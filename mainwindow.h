#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "downloaditem.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_start_clicked();

private:
    Ui::MainWindow *ui;
    QQueue<DownloadItem*> *downloadQueue;
    int activeDownloads;
    int downloadsCount;
    int downloadsTotal;
    void startNextDownload();
    void downloadFinished();
};
#endif // MAINWINDOW_H
