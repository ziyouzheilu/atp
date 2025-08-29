#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtSerialPort/QSerialPort"
#include "data.h"

#include "detecttrackmthread.h"

#include "ffmpegmthread.h"

#include "pciemthread.h"
#include "QTimer"
#include "QSettings"
#include <QUdpSocket>

#include "trackmthread.h"
#include <opencv2/opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void auto_focus();
    void auto_exp();
    void set_zoom(int zoom);
    void set_focus(int focus);
    ~MainWindow();
signals:
    void signals_send_handle(int,int,int,int,int);


private:
    Ui::MainWindow *ui;

    QSerialPort *serialPort0;
    QSerialPort *serialPort1;


    DetectTrackMthread *detectTrackMthread;


    ffmpegMthread  *FfmpegMthread;


//    pcieLthread *PcieLthread;
    pciemthread *Pciemthread;


    trackmthread *Trackmthread;
//    imgUploadThread *imguploadthread;

//    QTimer *telemeteringTimer;


    QTimer *SendTimer;

    char telemeteringBuf[200];


    QString m_configFileName;
    QSettings *m_settings;

    QUdpSocket* udpsocket_pc =NULL;

    int ct=0;
    float tx=0;
    float ty=0;


public slots:

    void receiveInfo0();
//    void receiveInfo1();
    void receiveInfo2();
    void response();
    unsigned char binaryToChar(const std::string& binary);
     void signals_recv_track(float,float,int);

};
void send_UDP_Image(cv::Mat image,int sock,struct sockaddr_in serverAddress);

#endif // MAINWINDOW_H
