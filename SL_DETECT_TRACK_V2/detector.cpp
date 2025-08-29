
#include "detector.h"
extern float seg_thres;
double sumElem(Mat img)
{
    double sum = 0;

//    for (int i = 0; i < img.rows; i++)
//    {
//        for (int j = 0; j < img.cols; j++)
//        {
//            sum += img.ptr<uchar>(i)[j];
//        }
//    }
    cv::Scalar summm = cv::sum(img);

    sum = summm[0];

    return sum;
}

void get_target_cons(Mat gray,Mat st,Mat labels,int sql,
                     int label,
                     float *const0,
                     float *target_mean,
                     float *img_mean)
{

    // 统计图像的特性,统计5w*5h大小的目标区域内，目标和背景的对比度
    int im_w = gray.cols;
    int im_h = gray.rows;
    cv::Rect rect(st.at<int>(sql,CC_STAT_LEFT ),
                  st.at<int>(sql,CC_STAT_TOP),
                  st.at<int>(sql,CC_STAT_WIDTH ),
                  st.at<int>(sql,CC_STAT_HEIGHT));//创建一个Rect对象

    Mat tar_img =gray(rect);



    int sx_b = st.at<int>(sql,CC_STAT_LEFT) - st.at<int>(sql,CC_STAT_WIDTH)*2;
    int sy_b = st.at<int>(sql,CC_STAT_TOP) - st.at<int>(sql,CC_STAT_HEIGHT)*2;
    int ex_b = st.at<int>(sql,CC_STAT_LEFT) + st.at<int>(sql,CC_STAT_WIDTH)*3;
    int ey_b = st.at<int>(sql,CC_STAT_TOP) + st.at<int>(sql,CC_STAT_HEIGHT)*3;
    if (sx_b < 0 || sy_b < 0 || ex_b > (im_w - 1) || ey_b > (im_h - 1))
    {
        return;
    }

    cv::Rect rect2(sx_b,sy_b,ex_b-sx_b,ey_b- sy_b);

    Mat back_img =gray(rect2);

//    for(int i = 0; i < 15;i++)
//    {
//      qDebug()<<int(back_img.at<char>(i,0))
//          <<int(back_img.at<char>(i,1))
//          <<int(back_img.at<char>(i,2));
//    }

    //计算目标的对比度

    *target_mean = 0.0;

    for(int i = 0; i < st.at<int>(sql,3);i++)
    {
        for(int j = 0; j < st.at<int>(sql,2);j++)
        {
            if(labels.at<int16_t>(i + st.at<int>(sql,1), j +  st.at<int>(sql,0)) == label)
            {
               // *target_mean = *target_mean + gray.at<uchar>(i,j);

            }
            //temp-zrx
            *target_mean = *target_mean + gray.at<uchar>(i + st.at<int>(sql,1),j +  st.at<int>(sql,0));
        }

    }
      //temp-zrx
   //  *target_mean = *target_mean/(st.at<int>(sql,2)*st.at<int>(sql,3));
    *target_mean = *target_mean/st.at<int>(sql,4);

    // *target_mean =  sumElem(tar_img) * 1.0 / st.at<int>(sql,CC_STAT_WIDTH) / st.at<int>(sql,CC_STAT_HEIGHT);
    float back_mean = ( sumElem(back_img) - *target_mean) * 1.0 / ((ey_b - sy_b + 1) * (ex_b - sx_b + 1) - st.at<int>(sql,CC_STAT_AREA));
    *const0 = (*target_mean - back_mean) / (back_mean+1e-5);
    //*img_mean =  sumElem(gray)*1.0/im_w/im_h;
   // return const,target_mean,img_mean
}
int serch_Tg(int *seq,int len,int Tg)
{
    for(int i = 0;i < len;i++)
    {
        if(seq[i] == Tg)
        {
           return 1;
        }
    }
    return 0;
}

void sqort(QVector < struct target_info> data ,int *seq ,int datalen)
{
    int len = data.count();
//    if(len <= 10)
//    {
//        return;
//    }
#if 0
    float max = 0.0;
    int max_dex = 0;
    struct target_info info;
    //qDebug() <<"*************************************";
    for(int i = 0;i < len ;i++)
    {
        qDebug()<<data.at(i).info[0]
                  <<data.at(i).info[1]
                    <<data.at(i).info[2]
                      <<data.at(i).info[3]
                        <<data.at(i).info[4];

        if(data.at(i).info[4] > max)
        {
            max = data.at(i).info[4];
            max_dex = i;
        }
    }
#else
    //float max_l = 0.0;
    //int max_dex_l = 0;
    for(int i = 0;i < datalen ;i++)
    {
        float max = 0.0;
        int max_dex = 0;
        for(int j = 0 ;j < len ;j++)
        {
            if(data.at(j).info[4] > max &&
//               data.at(j).info[4] != max_l &&
               serch_Tg(seq,i,j) == 0)
            {
                max = data.at(j).info[4];
                max_dex = j;
            }
        }
        //max_l = max;
       // max_dex_l = max_dex;
        seq[i] = max_dex;
    }

#endif
    //qDebug() <<"*************************************";
   // memcpy(info.info,data.at(max_dex).info,sizeof (struct target_info));
   // return info;
}
void sqort_AREA(QVector < struct target_info> data ,int *seq,int datalen)
{
    int len = data.count();
//    if(len <= 10)
//    {
//        return;
//    }

    //float max_l = 0.0;
    //int max_dex_l = 0;
    for(int i = 0;i < datalen ;i++)
    {
        float max = 0.0;
        int max_dex = 0;
        for(int j = 0 ;j < len ;j++)
        {
            if(data.at(j).info[10] > max &&
//               data.at(j).info[4] != max_l &&
               serch_Tg(seq,i,j) == 0)
            {
                max = data.at(j).info[10];
                max_dex = j;
            }
        }
        //max_l = max;
       // max_dex_l = max_dex;
        seq[i] = max_dex;
    }

    //qDebug() <<"*************************************";
   // memcpy(info.info,data.at(max_dex).info,sizeof (struct target_info));
   // return info;
}

struct target_info sqort_max(QVector < struct target_info> data)
{
    int len = data.count();
    float max = 0.0;
    int max_dex = 0;
    struct target_info info;
//    qDebug() <<"*************************************";
    for(int i = 0;i < len ;i++)
    {
//        qDebug()<<data.at(i).info[0]
//                  <<data.at(i).info[1]
//                    <<data.at(i).info[2]
//                      <<data.at(i).info[3]
//                        <<data.at(i).info[4];

        if(data.at(i).info[4] > max)
        {
            max = data.at(i).info[4];
            max_dex = i;
        }
    }
//    qDebug() <<"*************************************";
    memcpy(info.info,data.at(max_dex).info,sizeof (struct target_info));
    return info;
}
void gausskernel(float *templ,int r,float xgm)
{
    for(int i=0;i<2*r+1;i++)
    {
        templ[i] = exp(-((i*1.0-r)*(i*1.0-r)/2*xgm*xgm))/(xgm*sqrt(2*3.1415926));
    }
    double z = 0;
    for(int i=0;i<2*r+1;i++)
    {
        z+=templ[i];
    }
    for(int i=0;i<2*r+1;i++)
    {
        templ[i]/=z;
    }
}

int gauss_Filtering(int *sumtotal,float *templ,int img_h,int r,float res)
{
    int ave_sum = 0;
    for(int i=r;i<img_h-r;i++)
    {
        double temp=0;
        for(int j=0;j<2*r+1;j++)
        {
             temp+=(sumtotal[i-(r)+j]*templ[j]);
        }
        sumtotal[i]=temp;
      //  qDebug() << i << "= "<< sumtotal[i];
        ave_sum += temp;
    }

    ave_sum = ave_sum * 1.0 /(img_h - 2*r);

    int ave_num = 0;
    int max_num = 0;
    for(int i = img_h * 0.1; i < img_h - r;i++)
    {
        if(i - ave_num > 1)
        {
           max_num = 0;
        }
        if(sumtotal[i] > ave_sum * res)
        {
            ave_num = i;
            max_num++;
            if(max_num > 3)
            {
                break;
            }
        }
    }
    return ave_num;
}

void clc_Gradient(Mat im,int *sum)
{
    for(int i = 0;i < im.rows;i++)
    {
        // 获取第i行的首地址
       uchar* data = im.ptr<uchar>(i);
       int tempsum = 0;
       for (int j = 0; j < im.cols; j++)  // 列循环
       {
           // 开始处理每一个像素值，每一个像素值都减去div
           tempsum += data[j];

       }
       sum[i] = tempsum;
    }
}


int gauss_Filtering_x(int *sumtotal,float *templ,int img_w,int r,float res)
{
    int ave_sum = 0;
    for(int i=r;i<img_w-r;i++)
    {
        double temp=0;
        for(int j=0;j<2*r+1;j++)
        {
             temp+=(sumtotal[i-(r)+j]*templ[j]);
        }
        sumtotal[i]=temp;
      //  qDebug() << i << "= "<< sumtotal[i];
        ave_sum += temp;
    }

    ave_sum = ave_sum * 1.0 /(img_w - 2*r);
    int large_num_num = 0;
    for(int i = img_w * 0.1; i < img_w - r;i++)
    {
        if(sumtotal[i] > ave_sum)
        {
            large_num_num++;
        }
    }
    if(large_num_num * 1.0 / img_w  < 0.1)
    {
        return 1;
    }
    int ave_num = 0;
    int max_num = 0;
    for(int i = img_w * 0.1; i < img_w - r;i++)
    {
        if(i - ave_num > 1)
        {
           max_num = 0;
        }
        if(sumtotal[i] < ave_sum * res)
        {
            ave_num = i;
            max_num++;
            if(max_num > 3)
            {
                break;
            }
        }
    }
    return ave_num;
}

void clc_Gradient_X(Mat im,int *sum)
{
    for(int j = 0; j < im.cols;j++)
    {
        int tempsum = 0;
        for(int i = 0;i < im.rows;i++)
        {
            // 获取第i行的首地址
           uchar* data = im.ptr<uchar>(i);
            // 开始处理每一个像素值，每一个像素值都减去div
           tempsum += data[j];

        }
        sum[j] = tempsum;
    }
}
bool is_sky_ground(Mat Im_in)
{
    bool sky_ground_flag = false;
    float ratio_thr = 5;
    int im_w = Im_in.cols;
    int im_h = Im_in.rows;
    int im_w_t = int(im_w / 4);

    cv::Rect m_select = Rect(0,0,im_w_t ,im_h);
    Mat ROI0 = Im_in(m_select);
  //  imshow("ROI0",ROI0);
    m_select = Rect(im_w - im_w_t ,0,im_w_t ,im_h);
    Mat ROI1 = Im_in(m_select);

  //  imshow("ROI1",ROI1);

    int img_mean_xy_Left = sumElem(ROI0) / im_w / im_h / 4;
    int img_mean_xy_right = sumElem(ROI1)  * 1.0 / im_w / im_h / 4;

       if (img_mean_xy_Left > img_mean_xy_right * ratio_thr)
           sky_ground_flag = 1;
       else
           sky_ground_flag = 0;

       return sky_ground_flag;
}

int get_target_iou(float *box1, float *box2)
{
    int iou;
#if 1

    float x11 = box1[0];
    float x12 = box1[0] + box1[2];
    float y11 = box1[1];
    float y12 = box1[1] + box1[3];

    float x21 = box2[0];
    float x22 = box2[0] + box2[2];
    float y21 = box2[1];
    float y22 = box2[1] + box2[3];

    float xA = max(x11, x21);
    float yA = max(y11, y21);
    float xB = min(x12, x22);
    float yB = min(y12, y22);

    // 两个框各自的面积
    float boxAArea = (x12 - x11) * (y12 - y11);
    float boxBArea = (x22 - x21) * (y22 - y21);

    // 重叠面积
    double interArea0 = xB - xA;
    double interArea1 = yB - yA;
    float interArea = max(interArea0, 0.0) * max(interArea1, 0.0);

    // 计算IOU
    iou = interArea / (boxAArea + boxBArea - interArea);
#endif
    return iou;
}

void delete_iou_overlap(QVector < struct target_info> *stats_new, int over_scale)
{
    over_scale = 5;
    int box_num = stats_new->count();
    if (box_num < 2)
           return;
    int flag[10000];
    if(box_num > 10000)
    {
        box_num = 10000;
    }
    int over_scale_left = int(over_scale/2);

   for (int i = 0;i < box_num; i++)
   {
             if (flag[i])
                 continue;
             int j = i + 1;
             struct target_info box1_info = stats_new->at(i);

             float bbox1[4];
             bbox1[0]= box1_info.info[0]-box1_info.info[2]*over_scale_left;
             bbox1[1]= box1_info.info[1]-box1_info.info[3]*over_scale_left;
             bbox1[2]= box1_info.info[2] * over_scale;
             bbox1[3]= box1_info.info[3] * over_scale;

             //for j in range(box_num):
             while(j<box_num)
             {
                 if (flag[j] == 0)
                 {
                     struct target_info box2_info = stats_new->at(j);
                     float bbox2[4];
                     bbox2[0]= box2_info.info[0]-box2_info.info[2]*over_scale_left;
                     bbox2[1]= box2_info.info[1]-box2_info.info[3]*over_scale_left;
                     bbox2[2]= box2_info.info[2] * over_scale;
                     bbox2[3]= box2_info.info[3] * over_scale;

                     int box_iou = get_target_iou(bbox1, bbox2);
                     if (box_iou > 0)
                     {
                         flag[j] = 1;
                      }
                 }
                 j = j+1;
             }
     }
   QVector < struct target_info> stats_out;
   for (int i = 0;i < box_num ;i++)
   {
           if (flag[i] == 0)
           {
               stats_out.append(stats_new->at(i));
           }
   }
   stats_new->clear();
   for (int i = 0;i < stats_out.count() ;i++)
   {
         stats_new->append(stats_out.at(i));
   }
}

