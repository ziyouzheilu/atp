#ifndef  __USER_PACKET_POOL_H__
#define __USER_PACKET_POOL_H__
#include "QMutex"
#include "memory.h"
#include"detector.h"
 //接收到的数据包
 struct vb_packet_node {
	 //struct list_head head;
     char *data;
	 int payload_len;
	 struct vb_packet_node *pNext;
 };

 struct PacketLinkedList // 记录链表的初始节点和末节点
 {
	 int packet_num;
	 struct vb_packet_node *pHead; // 初始节点
	 struct vb_packet_node *pTail; // 末节点
	
 };

 //解码后帧数据
 struct vb_frame_node {
	 //struct list_head head;
	 char *data;
	 int payload_len;
     int frame_id;
	 struct vb_frame_node *pNext;
 };

 struct FrameLinkedList // 记录链表的初始节点和末节点
 {
	 int Frame_num;
	 struct vb_frame_node *pHead; // 初始节点
	 struct vb_frame_node *pTail; // 末节点
 };


 //接收到的数据包
 struct vb_packet_targetinfo_node {
     //struct list_head head;
     float *data;
     int payload_len;
     struct vb_packet_targetinfo_node *pNext;
 };

 struct PackettargetinfoLinkedList // 记录链表的初始节点和末节点
 {
     int packet_num;
     struct vb_packet_targetinfo_node *pHead; // 初始节点
     struct vb_packet_targetinfo_node *pTail; // 末节点

 };


 //数据包处理函数
 struct vb_packet_node *put_packet_to_Node(QMutex *mutex,struct PacketLinkedList *list, char *data, int len);
 struct vb_packet_node *get_packet_from_Node(QMutex *mutex,struct PacketLinkedList *list);

 void  remove_packet_from_Node(QMutex *mutex,struct PacketLinkedList *list, int index);
 //图像帧处理函数
 struct vb_frame_node *put_frame_to_Node(QMutex *mutex,struct FrameLinkedList *list, char *data, int len,int frame_id);
 struct vb_frame_node *get_frame_from_Node(QMutex *mutex,struct FrameLinkedList *list);
 struct vb_frame_node *get_frame_from_Node2(QMutex *mutex,struct FrameLinkedList *list, int frame_id);
 void  remove_frame_from_Node(QMutex *mutex,struct FrameLinkedList *list, int index);

 // 添加数据包到节点中
 struct vb_packet_targetinfo_node *put_packet_targetinfo_to_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list, float *data,int len);
 // 获取第一个数据包并且删除
 struct vb_packet_targetinfo_node *get_packet_targetinfo_from_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list);
 //删除指定数据包
 void  remove_packet_targetinfo_from_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list, int index);
 target_info_list match_frame_id_info(QMutex *detect_target_mutex,QQueue<target_info_list>* detect_target_queue, int frame_id);

 int putDetectTargettoQueue(QMutex *mutex,QQueue<target_info_list> *target_queue, target_info_list info);
 int putTrackTargettoQueue(QMutex *mutex,QQueue<target_info> *target_queue, target_info info);
 int select_target( struct target_info_list info, int state);
 int match_track_detect_id_info(QMutex *detect_target_mutex,QQueue<target_info_list>* detect_target_queue, QMutex *track_target_mutex, QQueue<target_info> *track_target_queue);
#endif 
