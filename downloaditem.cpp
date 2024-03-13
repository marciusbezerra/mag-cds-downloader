#include "downloaditem.h"
#include "ui_downloaditem.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QListWidget>
#include <QDebug>
#include <QDir>

DownloadItem::DownloadItem(QUrl url, QString target, QWidget *parent)
    : manager(new QNetworkAccessManager(this)),
    url(url),
    target(target),
    QWidget(parent)
    , ui(new Ui::DownloadItem)
{
    ui->setupUi(this);
    ui->label_item->setText(url.toString());

}

void DownloadItem::Start() {
    ui->label_item->setText(QString("Downloading: %1").arg(url.toString()));

    QString fileName = url.fileName();
    QString firstPart;
    QString secondPart;

    QStringList parts = fileName.split(" - ");

    if (parts.length() != 2) {
        firstPart = "Outros";
        secondPart = fileName;
    } else {
        firstPart = parts[0];
        secondPart = fileName;
    }

    QDir dir(target + "/" + firstPart);

    if (!dir.exists()) {
        dir.mkpath(".");
    }

    auto filePath = target + "/" + firstPart + "/" + secondPart;

    if (QFile::exists(filePath)) {
        ui->label_item->setText(QString("Baixado: %1").arg(url.toString()));
        ui->label_status_item->setText("Arquivo já baixaxo");
        ui->label_status_item->setStyleSheet("QLabel { color : green; }");
        ui->progressBar_item->setMaximum(100);
        ui->progressBar_item->setValue(100);
        emit downloadFinished();
        return;
    }

    file = new QFile(filePath + ".part", this);

    qDebug() << file->fileName();

    if (file->open(QIODevice::WriteOnly)) {
        reply = manager->get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::downloadProgress, this, &DownloadItem::onDownloadProgress);
        connect(reply, &QNetworkReply::finished, this, &DownloadItem::onDownloadFinished);
        connect(reply, &QNetworkReply::readyRead, this, &DownloadItem::onDownloadReadyRead);
    } else {
        ui->label_status_item->setText("Erro ao abrir arquivo: " + file->errorString());
        ui->label_status_item->setStyleSheet("QLabel { color : red; }");
        emit downloadFinished();
    }
}

void DownloadItem::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar_item->setMaximum(bytesTotal);
    ui->progressBar_item->setValue(bytesReceived);
}

void DownloadItem::onDownloadFinished()
{
    emit downloadFinished();
    file->rename(file->fileName().left(file->fileName().length() - 5));
    file->close();
    if (reply->error()) {
        ui->label_status_item->setText("Erro do Download: " + reply->errorString());
        ui->label_status_item->setStyleSheet("QLabel { color : red; }");
    } else {
        ui->label_item->setText(QString("Baixado: %1").arg(url.toString()));
        ui->label_status_item->setText("Baixado com sucesso!");
        ui->label_status_item->setStyleSheet("QLabel { color : green; }");
    }
    reply->deleteLater();
}

void DownloadItem::onDownloadReadyRead()
{
    if (file->write(reply->readAll()) == -1) {
        ui->label_status_item->setText("Erro ao escrever no arquivo: " + file->errorString());
        ui->label_status_item->setStyleSheet("QLabel { color : red; }");
        // emit downloadFinished();
    }
}

void DownloadItem::cancelDownload()
{
    // qDebug() << "Entrando em cancelDownload()";
    // if (reply) {
    //     qDebug() << "reply existe";
    //     if (!url.isEmpty()) {
    //         qDebug() << "Cancelando download " << url.toString();
    //         try {
    //             if (reply->isRunning()) {
    //                 qDebug() << "reply está rodando";
    //                 qDebug() << "Cancelando download " << url.toString();
    //                 reply->abort();
    //                 qDebug() << "reply abortado";
    //                 reply->deleteLater();
    //                 qDebug() << "reply marcado para exclusão";
    //                 reply = nullptr;  // Adicionado
    //                 qDebug() << "reply definido como nullptr";
    //                 file->remove();
    //                 qDebug() << "file removido";
    //                 file->deleteLater();
    //                 qDebug() << "file marcado para exclusão";
    //                 file = nullptr;  // Adicionado
    //                 qDebug() << "file definido como nullptr";
    //                 ui->label_status_item->setText("Cancelado");
    //                 ui->label_status_item->setStyleSheet("QLabel { color : yellow; }");
    //                 qDebug() << "Cancelado download " << url.toString();
    //                 emit downloadFinished();
    //             } else {
    //                 qDebug() << "reply não está rodando";
    //             }
    //         } catch (...) {
    //             qDebug() << "Exceção lançada ao verificar se reply está rodando";
    //         }
    //     } else {
    //         qDebug() << "URL é inválido";
    //     }
    // } else {
    //     qDebug() << "Tentativa de cancelar um download que não foi iniciado.";
    // }
}

DownloadItem::~DownloadItem()
{
    delete ui;
}


