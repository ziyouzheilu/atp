#ifndef DETECTOR_H
#define DETECTOR_H
#define OPENCV
#define GPU
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "qdebug.h"
#include "data.h"
#include <yolo_v2_class.hpp>
#include <darknet.h>
#include"fdssttracker.hpp"
#include <QQueue>
using namespace cv;


void delete_iou_overlap(QVector < struct target_info> *stats_new, int over_scale);

#endif // DETECTOR_H
