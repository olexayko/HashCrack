#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QCryptographicHash"

#include <QElapsedTimer>
#include <QMutex>
#include <QThread>
#include <iostream>
#include <thread>
#include <ui_mainwindow.h>

using namespace std;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //QObject::connect(this, &MainWindow::stopWorkerSignal, this, &MainWindow::stop_worker);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString SHA1(QString input)
{
    QByteArray sha1Hash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha1);
    return sha1Hash.toHex();
}

int thr_num = 0;
void MainWindow::launch_worker(long long start_val, long long end_val, int operations_amount, int thread_number = thr_num++)
{
    QString true_hash = ui->lineEdit->text();
    QString phone_prefix = "+";

    QElapsedTimer timer;
    timer.start();
    //qDebug() << "Start seconds: " + QString::number(int(timer.nsecsElapsed() / 1000000000.0));

    long long val = start_val;
    while (val < end_val)
    {
        QString input = phone_prefix + QString::number(val);
        QString result_hash = SHA1(input);
        val++;

        if(result_hash == true_hash)
        {
            ui->textEdit->append("FOUND TRUE NUMBER: " + input + " SHA1 = " + result_hash);
            ui->textEdit->append("Exiting thread # " + QString::number(thread_number) + " after " + QString::number(int(timer.nsecsElapsed() / 1000000000.0)) + " seconds");
            return;
        }

        if(val % 10000000 == 0)
        {
            qDebug() << "Number: " + input + " SHA1 = " + result_hash;
        }
    }

    //qDebug() << "End seconds: " + QString::number(int(timer.nsecsElapsed() / 1000000000.0));
    //ui->textEdit->append("Elapsed time (s): " + QString::number(int(timer.nsecsElapsed() / 1000000000.0)));
    ui->textEdit->append("Exiting thread # " + QString::number(thread_number) + " after " + QString::number(int(timer.nsecsElapsed() / 1000000000.0)) + " seconds");
}


QMutex mutex;
vector<QThread*> worker_threads {};


void MainWindow::launch_sha_calc(int threads_number = QThread::idealThreadCount())
{
    long long starting_phone = 380000000000;
    long long final_phone =    381000000000;
    long long operations_amount = final_phone - starting_phone;
    long long operations_amount_per_thread = operations_amount / threads_number + 1;
    for(int i = 0; i < threads_number; i++)
    {
        long long start_val = starting_phone + i * operations_amount_per_thread;
        long long end_val = start_val + operations_amount_per_thread;
        //qDebug() << "vals: " << start_val << " " << end_val << "thr num: " << threads_number;
        QThread* thread = new QThread;
        QObject::connect(thread, &QThread::started, [=] {
            launch_worker(start_val, end_val, operations_amount_per_thread);
        });
        worker_threads.push_back(thread);
        for (int i = 0; i < worker_threads.size(); i++)
        {
            thread->start();
        }
    }
    ui->textEdit->append("Started hash cracking with " + QString::number(threads_number) + " parallel threads...");
}

void MainWindow::on_pushButton_clicked()
{
    launch_sha_calc();
}





