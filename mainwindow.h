#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void sha1_worker(long long start_index, long long end_index, int thread_number);
    void launch_sha1_calc(int threads_number);

private slots:
    void on_pushButton_clicked();

public slots:
    void updateUI();

signals:
    void workDone();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
