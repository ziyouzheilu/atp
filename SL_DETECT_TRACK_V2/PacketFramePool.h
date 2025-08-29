#ifndef  __USER_PACKET_POOL_H__
#define __USER_PACKET_POOL_H__
#include "QMutex"
#include "memory.h"
#include"detector.h"
 //���յ������ݰ�
 struct vb_packet_node {
	 //struct list_head head;
     char *data;
	 int payload_len;
	 struct vb_packet_node *pNext;
 };

 struct PacketLinkedList // ��¼����ĳ�ʼ�ڵ��ĩ�ڵ�
 {
	 int packet_num;
	 struct vb_packet_node *pHead; // ��ʼ�ڵ�
	 struct vb_packet_node *pTail; // ĩ�ڵ�
	
 };

 //�����֡����
 struct vb_frame_node {
	 //struct list_head head;
	 char *data;
	 int payload_len;
     int frame_id;
	 struct vb_frame_node *pNext;
 };

 struct FrameLinkedList // ��¼����ĳ�ʼ�ڵ��ĩ�ڵ�
 {
	 int Frame_num;
	 struct vb_frame_node *pHead; // ��ʼ�ڵ�
	 struct vb_frame_node *pTail; // ĩ�ڵ�
 };


 //���յ������ݰ�
 struct vb_packet_targetinfo_node {
     //struct list_head head;
     float *data;
     int payload_len;
     struct vb_packet_targetinfo_node *pNext;
 };

 struct PackettargetinfoLinkedList // ��¼����ĳ�ʼ�ڵ��ĩ�ڵ�
 {
     int packet_num;
     struct vb_packet_targetinfo_node *pHead; // ��ʼ�ڵ�
     struct vb_packet_targetinfo_node *pTail; // ĩ�ڵ�

 };


 //���ݰ�������
 struct vb_packet_node *put_packet_to_Node(QMutex *mutex,struct PacketLinkedList *list, char *data, int len);
 struct vb_packet_node *get_packet_from_Node(QMutex *mutex,struct PacketLinkedList *list);

 void  remove_packet_from_Node(QMutex *mutex,struct PacketLinkedList *list, int index);
 //ͼ��֡������
 struct vb_frame_node *put_frame_to_Node(QMutex *mutex,struct FrameLinkedList *list, char *data, int len,int frame_id);
 struct vb_frame_node *get_frame_from_Node(QMutex *mutex,struct FrameLinkedList *list);
 struct vb_frame_node *get_frame_from_Node2(QMutex *mutex,struct FrameLinkedList *list, int frame_id);
 void  remove_frame_from_Node(QMutex *mutex,struct FrameLinkedList *list, int index);

 // ������ݰ����ڵ���
 struct vb_packet_targetinfo_node *put_packet_targetinfo_to_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list, float *data,int len);
 // ��ȡ��һ�����ݰ�����ɾ��
 struct vb_packet_targetinfo_node *get_packet_targetinfo_from_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list);
 //ɾ��ָ�����ݰ�
 void  remove_packet_targetinfo_from_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list, int index);
 target_info_list match_frame_id_info(QMutex *detect_target_mutex,QQueue<target_info_list>* detect_target_queue, int frame_id);

 int putDetectTargettoQueue(QMutex *mutex,QQueue<target_info_list> *target_queue, target_info_list info);
 int putTrackTargettoQueue(QMutex *mutex,QQueue<target_info> *target_queue, target_info info);
 int select_target( struct target_info_list info, int state);
 int match_track_detect_id_info(QMutex *detect_target_mutex,QQueue<target_info_list>* detect_target_queue, QMutex *track_target_mutex, QQueue<target_info> *track_target_queue);
#endif 
