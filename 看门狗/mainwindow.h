#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBasicTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void timerEvent(QTimerEvent *);
    void moveEvent(QMoveEvent *);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *);
private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QBasicTimer timer;
    int fd;
    int times;
    char s;
};

#endif // MAINWINDOW_H
