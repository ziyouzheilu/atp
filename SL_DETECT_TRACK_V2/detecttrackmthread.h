#ifndef DETECTTRACKM_H
#define DETECTTRACKM_H

#include "data.h"

#include <QThread>

class DetectTrackMthread : public QThread
{
    Q_OBJECT
public:
    DetectTrackMthread();


signals:
    void send_target_info(float *);

    void signals_send_detect(int,int,int,int,int);


protected:
    void run();


private:

    char imagedata[M_IMG_H * M_IMG_W];


};

#endif // DETECTTRACKM_H
