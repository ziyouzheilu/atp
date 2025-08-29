#include "ffmpegmthread.h"
#include "detector.h"
#include "PacketFramePool.h"
#include <opencv4/opencv2/opencv.hpp>
#include "QDateTime"
#include"mainwindow.h"
using namespace std;
using namespace cv;


extern QMutex mutexMFFPMEG;
extern struct FrameLinkedList datalistMFFMPEG;

extern int M_target_x;
extern int M_target_y ;
extern int M_target_w ;
extern int M_target_h ;

extern QQueue<target_info> track_target_queue;
extern QMutex track_target_mutex;

extern QQueue<target_info_list> detect_target_queue;
extern QMutex detect_target_mutex;

void set_send_info(uchar* buf, int x,int y,int w,int h,int byte_start)
{
    buf[byte_start]=((x >> 8) & 0xFF);
    buf[byte_start+1]=(x & 0xFF);

    buf[byte_start+2]=((y >> 8) & 0xFF);
    buf[byte_start+3]=(y & 0xFF);

    buf[byte_start+4]=((w >> 8) & 0xFF);
    buf[byte_start+5]=(w & 0xFF);

    buf[byte_start+6]=((h >> 8) & 0xFF);
    buf[byte_start+7]=(h & 0xFF);

}
cv::Mat resize_image(cv::Mat image)
{
    cv::Mat rsImage = cv::Mat::zeros(cv::Size(M_IMG_W_UP, M_IMG_H_UP), image.type());
    int rsCol = 0, rsRow = 0;
    if (image.cols > image.rows)
    {
        rsCol = M_IMG_W_UP;
        rsRow = image.rows * 1.0 / image.cols * M_IMG_H_UP;
    }
    else
    {
        rsRow = M_IMG_H_UP;
        rsCol = image.cols * 1.0 / image.rows * M_IMG_W_UP;
    }
    cv::Mat tempImage;
    cv::resize(image, tempImage, cv::Size(rsCol, rsRow));
    int sx = (rsImage.cols - tempImage.cols) / 2;
    int sy= (rsImage.rows - tempImage.rows) / 2;
    if (sx < 0)sx = 0;
    if (sy < 0)sy = 0;
    tempImage.copyTo(rsImage(cv::Rect(sx, sy, tempImage.cols, tempImage.rows)));
    cv::imwrite("rsImage.jpg",rsImage);
    return rsImage;
}

ffmpegMthread::ffmpegMthread()
{


}


void ffmpegMthread::run()
{
    struct vb_frame_node *mM = NULL;
    Mat ffmpegImage1(M_IMG_H,M_IMG_W,CV_8UC1,imagedata);
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    // 设置服务器地址
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(10800); // 端口号
    //const char* send_ip_str=ip_re;
    inet_pton(AF_INET, "192.168.1.80", &serverAddress.sin_addr);
    int count=1;
    double duration=0;
    int sum=8;
    int fId=0;
    while(1)
    {
        //encode();
        mM = get_frame_from_Node(&mutexMFFPMEG,&datalistMFFMPEG);

        if (mM != NULL)
        {
               //std::printf("MFFMPEG图像有数据。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。");
               memcpy(imagedata, mM->data ,M_IMG_H * M_IMG_W);
               free(mM->data);  //释放内存
               fId = mM->frame_id;
               free(mM); //释放内存
               cv::Mat ffmpegImage;
               if(!ffmpegImage1.empty())
                   ffmpegImage = resize_image(ffmpegImage1);
//                   cv::resize(ffmpegImage1,ffmpegImage,cv::Size(M_IMG_W_UP,M_IMG_W_UP));

               else
                   continue;
               ffmpegImage.at<uchar>(512,0)=((fId >> 8) & 0xFF);
               ffmpegImage.at<uchar>(512,1)=(fId & 0xFF);

//               cv::imwrite("/home/nvidia/zht/XG_DETECT_TRACK0925/ffmpeg.jpg",ffmpegImage);
               target_info_list detect_values;
               memset(&detect_values,0,sizeof(target_info_list));
               target_info track_values;
               memset(&track_values,0,sizeof(target_info));

               int queue_num=detect_target_queue.size();
               if(queue_num>0)
               {
                   detect_target_mutex.lock();
                   detect_values=detect_target_queue.at(queue_num-1);
                   detect_target_mutex.unlock();
               }


               queue_num=track_target_queue.size();
               if(queue_num>0)
               {
                   track_target_mutex.lock();
                   track_values=track_target_queue.at(queue_num-1);
                   track_target_mutex.unlock();
               }


               float fx=0.5625;

               for(int t=0;t<6;t++)
               {
                   int x=0,y=0,w=0,h=0;


                   if(t<5)
                   {
                       x=int(detect_values.T_info[t].info[0]*fx);
                       y = int(detect_values.T_info[t].info[1]*fx+224);

                       w = int(detect_values.T_info[t].info[2]*fx);
                       h = int(detect_values.T_info[t].info[3]*fx);


                       set_send_info(ffmpegImage.data, x,y,w, h, t*sum);
//                       std::cout<<"x,y,w,h:"<<x<<","<<y<<","<<w<<","<<h<<std::endl;

                   }
                   else
                   {
//                       x=int(track_values.info[0]*M_IMG_W_UP/M_IMG_W);
//                       y=int(track_values.info[1]*M_IMG_H_UP/M_IMG_H);

//                       w=int(track_values.info[2]*M_IMG_W_UP/M_IMG_W);
//                       h=int(track_values.info[3]*M_IMG_H_UP/M_IMG_H);
                       x=int(track_values.info[0]*fx);
                       y=int(track_values.info[1]*fx+224);

                       w=int(track_values.info[2]*fx);
                       h=int(track_values.info[3]*fx);

                       set_send_info(ffmpegImage.data, x,y,w, h, t*sum);
                       std::cout<<"x,y,w,h:"<<x<<","<<y<<","<<w<<","<<h<<std::endl;

                   }
               }
//               cv::imwrite("/home/nvidia/zht/send_res/"+std::to_string(count)+".jpg",bgrImage);
               double start=(double)cv::getTickCount();
               send_UDP_Image(ffmpegImage,sock,serverAddress);
               duration += ((double)cv::getTickCount() - start)*1000 / cv::getTickFrequency();
//               qDebug()<<"send time count:"<<duration/count;

//               target_mutex.unlock();
//               cv::imshow("test",bgrImage);
//               cv::waitKey(3);
               count++;
//               msleep(1);
        }

    }
}

