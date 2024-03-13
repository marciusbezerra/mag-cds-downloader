#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "downloaditem.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QQueue>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    downloadQueue = new QQueue<DownloadItem*>();
    activeDownloads = 0;
    downloadsCount = 0;
    downloadsTotal = 0;
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0);
    ui->lcdNumber->display(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startNextDownload()
{
    if (activeDownloads < 5 && !downloadQueue->isEmpty()) {
        qDebug() << "Start download " << activeDownloads;
        DownloadItem *item = downloadQueue->dequeue();
        connect(item, &DownloadItem::downloadFinished, this, &MainWindow::downloadFinished);
        item->Start();
        activeDownloads++;

        // Encontre o QListWidgetItem correspondente ao item de download
        for (int i = 0; i < ui->listWidget_downloads->count(); ++i) {
            QListWidgetItem* witem = ui->listWidget_downloads->item(i);
            if (ui->listWidget_downloads->itemWidget(witem) == item) {
                // Faz o scroll para o item que está sendo baixado
                ui->listWidget_downloads->scrollToItem(witem);
                break;
            }
        }
    }
}

void MainWindow::downloadFinished()
{
    qDebug() << "Download finished";
    activeDownloads--;
    downloadsCount++;
    ui->progressBar->setValue(downloadsCount);
    startNextDownload();
}

void MainWindow::on_pushButton_start_clicked()
{
    if (ui->pushButton_start->text() == "Parar") {
        // while (!downloadQueue->isEmpty()) {
        //     auto item = downloadQueue->dequeue();
        //     item->cancelDownload();
        // }
        ui->pushButton_start->setText("Iniciar");
        return;
    }
    if (ui->lineEdit_url->text().isEmpty() || ui->lineEdit_target->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "Preencha todos os campos");
        return;
    }

    auto url = ui->lineEdit_url->text();
    auto target = ui->lineEdit_target->text();
    // fazer um request para url, ler o conteúdo e listar todos os hrefs que cotenhando a palavra CD...
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request((QUrl(url)));
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, [=](){
        if (reply->error()) {
            QMessageBox::critical(this, "Error", reply->errorString());
            return;
        } else {
            ui->listWidget_downloads->clear();
            activeDownloads = 0;
            auto data = reply->readAll();
            std::string str = data.toStdString();
            std::string href = "href=\"";
            std::string cd = "CD";
            std::string::size_type pos = 0;
            while ((pos = str.find(href, pos)) != std::string::npos) {
                if ((pos = str.find(cd, pos)) != std::string::npos) {
                    std::string::size_type end = str.find("\"", pos);
                    std::string link = str.substr(pos, end - pos);
                    std::string download_link = url.toStdString() + "/" + link;
                    if (download_link[download_link.size() - 1] == '/') {
                        pos = end;
                        continue;
                    }
                    DownloadItem *item = new DownloadItem(QString::fromStdString(download_link), target, this);
                    auto *witem = new QListWidgetItem();
                    witem->setSizeHint(item->sizeHint());
                    // witem->setSizeHint(QSize(891, 90));
                    ui->listWidget_downloads->addItem(witem);
                    ui->listWidget_downloads->setItemWidget(witem, item);
                    downloadQueue->enqueue(item);
                    downloadsTotal++;
                    startNextDownload();
                    pos = end;
                }
            }
            ui->lcdNumber->display(downloadsTotal);
            ui->progressBar->setMaximum(downloadsTotal);
        }
    });
    ui->pushButton_start->setText("Parar");
}

