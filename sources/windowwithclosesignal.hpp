#ifndef WINDOWWITHCLOSESIGNAL_HPP
#define WINDOWWITHCLOSESIGNAL_HPP

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

#endif // WINDOWWITHCLOSESIGNAL_HPP
