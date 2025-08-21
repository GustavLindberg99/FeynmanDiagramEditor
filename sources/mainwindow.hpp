#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

class MainWindow: public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags()): QMainWindow(parent, flags){}

signals:
    void aboutToClose(QCloseEvent *event);

protected:
    void closeEvent(QCloseEvent *event) override{
        emit aboutToClose(event);
    }
};

#endif // MAINWINDOW_H
