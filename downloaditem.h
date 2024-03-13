#ifndef DOWNLOADITEM_H
#define DOWNLOADITEM_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QListWidgetItem>
#include <QListWidget>

namespace Ui {
class DownloadItem;
}

class DownloadItem : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadItem(QUrl url, QString target, QWidget *parent = nullptr);
    void Start();
    void cancelDownload();
    ~DownloadItem();

private slots:
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    void onDownloadReadyRead();

private:
    Ui::DownloadItem *ui;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile *file;
    QUrl url;
    QString target;

signals:
    void downloadFinished();
};

#endif // DOWNLOADITEM_H
