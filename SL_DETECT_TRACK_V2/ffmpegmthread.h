#ifndef FFMPEGMTHREAD_H
#define FFMPEGMTHREAD_H


#include <QThread>
#include "data.h"
#include "qdebug.h"
#include <QQueue>
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include <unistd.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include <libavformat/avformat.h>
}


class ffmpegMthread : public QThread
{
    Q_OBJECT
public:
    ffmpegMthread();


signals:


protected:
    void run();


private:

    char imagedata[M_IMG_H * M_IMG_W];

     int save_num = 0;

//     QString savefilename;
//     FILE *f =NULL;


};

#endif // FFMPEGMTHREAD_H
