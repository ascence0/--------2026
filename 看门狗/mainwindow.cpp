#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fd = 0;
    times = 0;
    s='V';
}

MainWindow::~MainWindow()
{
    delete ui;
    ::close(fd);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    int dummy;
    QString str;

    times++;

    if(ui->checkBox->isChecked())
        ::ioctl(fd,WDIOC_KEEPALIVE,&dummy);

    ui->times->setText(str.sprintf("%d",times));
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->pushButton->text() == QString("open watchdog"))
    {
        ui->pushButton->setText("close watchdog");
        ui->checkBox->setDisabled(true);

        if(ui->checkBox->isChecked())
        {
            ui->label_2->setText("Keep feeding the watchdog, system will not restart.");
        }
        else
        {
            ui->label_2->setText("System will restart in 1 minute without feed dog.");
        }

        fd = ::open("/dev/watchdog",O_WRONLY);
        if(fd < 0)
        {
            QMessageBox::about(this,"error","open watchdog failure");
            exit(-1);
        }


        write(fd,(char *)&s,sizeof(s));

        times = 0;
        ui->times->setText("0");
        timer.start(1000,this);
    }
    else
    {
        ui->pushButton->setText("open watchdog");
        ui->checkBox->setEnabled(true);
        timer.stop();
        ::close(fd);
    }
}

void MainWindow::moveEvent(QMoveEvent *)
{
    this->move(QPoint(0,0));
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    this->showMaximized();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    destroy();
    exit(0);
}

