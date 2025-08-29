#ifndef PCIEMTHREAD_H
#define PCIEMTHREAD_H

#include "data.h"
#include "QMutex"
#include <QThread>
#include <opencv2/opencv.hpp>
class pciemthread : public QThread
{
    Q_OBJECT
public:
    pciemthread();


protected:
    void run();



private:

    char imagedata0[M_IMG_H * M_IMG_W * 3];
    char imagedata[M_IMG_H * (M_IMG_W)];


};
int get_image(unsigned char* buffer);
int get_video_image(cv::Mat image,unsigned char* buffer);

struct image_info
{
    cv::Mat image;
    int frame_id;
};
int put_frame_to_cache(QMutex *mutex,std::vector<struct image_info> &list, struct image_info image);
int remove_frame_to_cache(QMutex *mutex,std::vector<struct image_info>&list, struct image_info image);
struct image_info get_frame_from_vector(QMutex *mutex,std::vector<struct image_info>list);
#endif // PCIEMTHREAD_H
