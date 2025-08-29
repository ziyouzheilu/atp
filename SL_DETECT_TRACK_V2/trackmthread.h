#ifndef TRACKMTHREAD_H
#define TRACKMTHREAD_H

#include "data.h"
#include <QThread>

class trackmthread : public QThread
{
    Q_OBJECT
public:
    trackmthread();
    void convertToAngle(double x, double y, double *angle_x,double *angle_y);

signals:

     void send_target_info(float *);
     void send_target_data(float, float, int);

protected:

    void run();

private slots:

    void slots_recv_detect(int x,int y,int w,int h, int id);
    void slots_recv_handle(int x,int y,int w,int h, int id);

private:

    char imagedata[M_IMG_H * M_IMG_W];

    int targetx = 0;
    int targety = 0;
    int targetw = 1;
    int targeth = 1;
    int frame_id=0;
    bool initFlag=true;
    bool is_open_MTrack = false;

    bool is_open_MTrack_2 = false;
    int targetx_2 = 0;
    int targety_2 = 0;
    
    int fix_num=30;

    double opt_cx=512;
    double opt_cy=512;
    double fov_x=58.1;
    double fov_y=34.8;

};
#endif // TRACKMTHREAD_H
