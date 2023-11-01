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
int ui_update_frequency = 10000000;

void MainWindow::sha1_worker(long long start_val, long long end_val, int thread_number = thr_num++)
{
    QString true_hash = ui->lineEdit->text();
    QString phone_prefix = "+";

    QElapsedTimer timer;
    timer.start();

    long long val = start_val;
    while (val < end_val)
    {
        QString input = phone_prefix + QString::number(val);
        QString result_hash = SHA1(input);

        if(result_hash == true_hash)
        {
            ui->textEdit->append("Found possible number: " + input + " with SHA1 = " + result_hash);
        }

        if(val % ui_update_frequency == 0)
        {
            emit workDone();
        }
        val++;
    }
    ui->textEdit->append("Exiting thread # " + QString::number(thread_number) + " after " + QString::number(int(timer.nsecsElapsed() / 1000000000.0)) + " seconds");
}

vector<QThread*> worker_threads {};

void MainWindow::launch_sha1_calc(int threads_number = QThread::idealThreadCount())
{
    long long starting_phone = 380000000000;
    long long final_phone =    381000000000;
    long long operations_amount = final_phone - starting_phone;
    long long operations_amount_per_thread = operations_amount / threads_number + 1;

    ui->progressBar->setMaximum(operations_amount);

    connect(this, &MainWindow::workDone, this, &MainWindow::updateUI);

    for(int i = 0; i < threads_number; i++)
    {
        long long start_val = starting_phone + i * operations_amount_per_thread;
        long long end_val = start_val + operations_amount_per_thread;

        QThread* thread = new QThread;
        QObject::connect(thread, &QThread::started, [=] {
            sha1_worker(start_val, end_val);
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
    launch_sha1_calc();
}

void MainWindow::updateUI()
{
    ui->progressBar->setValue(ui->progressBar->value() + ui_update_frequency);
}





