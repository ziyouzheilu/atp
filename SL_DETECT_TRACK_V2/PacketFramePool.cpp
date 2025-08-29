#include "PacketFramePool.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include"pciemthread.h"
extern int detect_queue_num;
extern int track_queue_num;
// �������ݰ����ڵ���
struct vb_packet_node *put_packet_to_Node(QMutex *mutex,struct PacketLinkedList *list, char *data,int len)
{
	struct vb_packet_node *m = (struct vb_packet_node*) malloc(sizeof(struct vb_packet_node)); // �����ڵ�
    //mutex->lock();//�ȴ�������
    mutex->lock();
	m->data = (char*) malloc(sizeof(char)*len); // ��������
	memcpy(m->data, data, len);
	m->payload_len = len;
	m->pNext = NULL; // �Ѹýڵ�����һ���ڵ���ָ��ָ����
	if (list->pTail != NULL)
		(list->pTail)->pNext = m; // ��������һ��Ԫ�أ��ǾͰ���һԪ�ص���һ�ڵ�ָ��ָ�򱾽ڵ�
	else
		list->pHead = m; // û�еĻ��ͳ䵱���ڵ�
	list->pTail = m; // ������ô˵���������ĵ�ǰ�ڵ���Ϊm
	list->packet_num++;
    //mutex->unlock();;
    mutex->unlock();
	return m;
}
// ��ȡ��һ�����ݰ�����ɾ��
struct vb_packet_node *get_packet_from_Node(QMutex *mutex,struct PacketLinkedList *list)
{
	struct vb_packet_node *m = NULL;
    mutex->lock();//�ȴ�������
	if (list->pHead != NULL)
	{
		m = (struct vb_packet_node*) malloc(sizeof(struct vb_packet_node)); // �����ڵ�
		memcpy(m, (list->pHead),sizeof(struct vb_packet_node));
		//remove_packet_from_Node(list, 0);
		//list->packet_num--;
	}
	else
	{
		m = NULL;
	}
	
	if (list->pHead != NULL)
	{
        remove_packet_from_Node(mutex,list, 0);
		list->packet_num--;
	}
    mutex->unlock();;
	return m;
}
//ɾ��ָ�����ݰ�
void  remove_packet_from_Node(QMutex *mutex,struct PacketLinkedList *list, int index)
{
	struct vb_packet_node *last = NULL; // ��һ���ڵ㣬0x0��Ҳ����NULL���ǿսڵ�
    //mutex->lock();//�ȴ�������
	struct vb_packet_node *now = list->pHead; // ��ǰ�ֵ��Ľڵ㣬һ��ʼΪpHead�ڵ�
	if (now == NULL)
	{
        //mutex->unlock();;
		return;
	}
	for (int i = 1; i <= index; i++)
	{
		if (now->pNext != NULL) { // ����������һ���ڵ��Ļ�
			last = now; // �����֣���һ���ڵ�����now�ڵ�
			now = now->pNext; // ������
		}
		else break; // ��β���˾��˳�
	}
	if (last == NULL) // ����Ҫɾ��pHeadԪ��
	{
		if ((list->pHead)->pNext != NULL) // ����pHeadԪ�غ��滹��
		{
			list->pHead = (list->pHead)->pNext; // ����
		}
		else {
			list->pHead = NULL; // û�еĻ��Ͱ�pHeadԪ��ҲŪ��
			list->pTail = NULL; // û�еĻ��Ͱ�pTailԪ��ҲŪ��
		}
	}
	else {
		last->pNext = NULL; // �Ͽ���һ��������
		if (now->pNext != NULL) // �������滹��Ԫ�صĻ�
		{
			last->pNext = now->pNext; // ����
		}
	}
    //mutex->unlock();;
	free(now); // �ͷ�ɾ������Ԫ�ص��ڴ�
}


// ����ͼ��֡���ڵ���
struct vb_frame_node *put_frame_to_Node(QMutex *mutex,struct FrameLinkedList *list, char *data, int len,int frame_id)
{
	struct vb_frame_node *m = (struct vb_frame_node*) malloc(sizeof(struct vb_frame_node)); // �����ڵ�
    mutex->lock();//�ȴ�������
	m->data = (char*)malloc(sizeof(char)*len); // ��������
	memcpy(m->data, data, len);
	m->payload_len = len;
    m->frame_id = frame_id;
	m->pNext = NULL; // �Ѹýڵ�����һ���ڵ���ָ��ָ����
	if (list->pTail != NULL)
		(list->pTail)->pNext = m; // ��������һ��Ԫ�أ��ǾͰ���һԪ�ص���һ�ڵ�ָ��ָ�򱾽ڵ�
	else
		list->pHead = m; // û�еĻ��ͳ䵱���ڵ�
	list->pTail = m; // ������ô˵���������ĵ�ǰ�ڵ���Ϊm
	list->Frame_num++;
    mutex->unlock();;
	return m;
}

int put_frame_to_cache(QMutex *mutex,std::vector<struct image_info>&list, struct image_info image)
{

    mutex->lock();//�ȴ�������

    list.push_back(image);
    mutex->unlock();;
    return 0;
}
int remove_frame_to_cache(QMutex *mutex,std::vector<struct image_info>&list,struct image_info image)
{

    mutex->lock();//�ȴ�������

    list.erase(list.begin());
    list.push_back(image);
    mutex->unlock();;
    return 0;
}

// ��ȡ��һ��ͼ��֡����ɾ��
struct vb_frame_node *get_frame_from_Node(QMutex *mutex,struct FrameLinkedList *list)
{
	struct vb_frame_node *m = NULL;
    mutex->lock();//�ȴ�������
	if (list->pHead != NULL)
	{
		m = (struct vb_frame_node*) malloc(sizeof(struct vb_frame_node)); // �����ڵ�
		memcpy(m, (list->pHead), sizeof(struct vb_frame_node));
		//remove_frame_from_Node(list, 0);
	}
	else
	{
		m = NULL;
	}
	
	if (list->pHead != NULL)
	{
        remove_frame_from_Node(mutex,list, 0);
		list->Frame_num--;
	}
    mutex->unlock();;
	return m;
}

struct vb_frame_node *get_frame_from_Node2(QMutex *mutex,struct FrameLinkedList *list, int frame_id)
{
    struct vb_frame_node *last = NULL; // ��һ���ڵ㣬0x0��Ҳ����NULL���ǿսڵ�
    struct vb_frame_node *m = NULL;
    //mutex->unlock();;;//�ȴ�������
    struct vb_frame_node *now = list->pHead; // ��ǰ�ֵ��Ľڵ㣬һ��ʼΪpHead�ڵ�
    if (now == NULL)
    {
        //ReleaseMutex(Frame_Mutex);
        return m;
    }
    for (int i = 1; i <= list->Frame_num; i++)
    {
        if (now->pNext != NULL) { // ����������һ���ڵ��Ļ�
            last = now; // �����֣���һ���ڵ�����now�ڵ�
 //           if(now->frame_id==frame_id)
            {
                m = (struct vb_frame_node*) malloc(sizeof(struct vb_frame_node)); // �����ڵ�
                memcpy(m, now, sizeof(struct vb_frame_node));
                break;
            }
            now = now->pNext; // ������
        }
        else break; // ��β���˾��˳�
    }

    mutex->unlock();
    return m;
}

struct image_info get_frame_from_vector(QMutex *mutex,std::vector<struct image_info>list)
{
    struct image_info node;
    mutex->lock();//�ȴ�������
    node=list.at(list.size()-1);
    mutex->unlock();
    return node;
}

//ɾ��ָ��ͼ��֡
void  remove_frame_from_Node(QMutex *mutex,struct FrameLinkedList *list, int index)
{
	struct vb_frame_node *last = NULL; // ��һ���ڵ㣬0x0��Ҳ����NULL���ǿսڵ�
    //mutex->unlock();;;//�ȴ�������
	struct vb_frame_node *now = list->pHead; // ��ǰ�ֵ��Ľڵ㣬һ��ʼΪpHead�ڵ�
	if (now == NULL)
	{
		//ReleaseMutex(Frame_Mutex);
		return;
	}
	for (int i = 1; i <= index; i++)
	{
		if (now->pNext != NULL) { // ����������һ���ڵ��Ļ�
			last = now; // �����֣���һ���ڵ�����now�ڵ�
			now = now->pNext; // ������
		}
		else break; // ��β���˾��˳�
	}
	if (last == NULL) // ����Ҫɾ��pHeadԪ��
	{
		if ((list->pHead)->pNext != NULL) // ����pHeadԪ�غ��滹��
		{
			list->pHead = (list->pHead)->pNext; // ����
		}
		else {
			list->pHead = NULL; // û�еĻ��Ͱ�pHeadԪ��ҲŪ��
			list->pTail = NULL; // û�еĻ��Ͱ�pTailԪ��ҲŪ��
		}
	}
	else {
		last->pNext = NULL; // �Ͽ���һ��������
		if (now->pNext != NULL) // �������滹��Ԫ�صĻ�
		{
			last->pNext = now->pNext; // ����
		}
	}
	//ReleaseMutex(Frame_Mutex);
//    free(now->data);
	free(now); // �ͷ�ɾ������Ԫ�ص��ڴ�
}

target_info_list match_frame_id_info(QMutex *detect_target_mutex,QQueue<target_info_list>* detect_target_queue, int frame_id)
{
    detect_target_mutex->lock();
    target_info_list returnInfo;
    memset(&returnInfo,0,sizeof(target_info_list));
    QQueue<target_info_list>::iterator it;

    int min_dist=1000;
    int id=-1;
    int count=0;
    for(it=detect_target_queue->begin();it!=detect_target_queue->end();it++)
    {
        target_info_list item=*it;
        if(item.target_num>0)
        {
            int dist=(item.T_info[0].info[4]-frame_id);
            if(dist<min_dist)
            {
                min_dist=dist;
                id = count;
                if(min_dist==0)break;

            }
        }
        else continue;

    }
    if(min_dist!=0)id=-1;
    if(id>-1)
    {
//        for(int )
        memcpy(&returnInfo,&detect_target_queue->at(id),sizeof(target_info_list));
    }
    detect_target_mutex->unlock();
    return returnInfo;
}


// �������ݰ����ڵ���
struct vb_packet_targetinfo_node *put_packet_targetinfo_to_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list, float *data,int len)
{
    struct vb_packet_targetinfo_node *m = (struct vb_packet_targetinfo_node*) malloc(sizeof(struct vb_packet_targetinfo_node)); // �����ڵ�
    //mutex->lock();//�ȴ�������
    mutex->lock();
    m->data = (float*) malloc(sizeof(float)*len); // ��������
    memcpy(m->data, data, sizeof(float) * len);
    m->payload_len = len;
    m->pNext = NULL; // �Ѹýڵ�����һ���ڵ���ָ��ָ����
    if (list->pTail != NULL)
        (list->pTail)->pNext = m; // ��������һ��Ԫ�أ��ǾͰ���һԪ�ص���һ�ڵ�ָ��ָ�򱾽ڵ�
    else
        list->pHead = m; // û�еĻ��ͳ䵱���ڵ�
    list->pTail = m; // ������ô˵���������ĵ�ǰ�ڵ���Ϊm
    list->packet_num++;
    //mutex->unlock();;
    mutex->unlock();
    return m;
}
// ��ȡ��һ�����ݰ�����ɾ��
struct vb_packet_targetinfo_node *get_packet_targetinfo_from_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list)
{
    struct vb_packet_targetinfo_node *m = NULL;
    mutex->lock();//�ȴ�������
    if (list->pHead != NULL)
    {
        m = (struct vb_packet_targetinfo_node*) malloc(sizeof(struct vb_packet_targetinfo_node)); // �����ڵ�
        memcpy(m, (list->pHead),sizeof(struct vb_packet_targetinfo_node));
        //remove_packet_from_Node(list, 0);
        //list->packet_num--;
    }
    else
    {
        m = NULL;
    }

    if (list->pHead != NULL)
    {
        remove_packet_targetinfo_from_Node(mutex,list, 0);
        list->packet_num--;
    }
    mutex->unlock();;
    return m;
}
//ɾ��ָ�����ݰ�
void  remove_packet_targetinfo_from_Node(QMutex *mutex,struct PackettargetinfoLinkedList *list, int index)
{
    struct vb_packet_targetinfo_node *last = NULL; // ��һ���ڵ㣬0x0��Ҳ����NULL���ǿսڵ�
    //mutex->lock();//�ȴ�������
    struct vb_packet_targetinfo_node *now = list->pHead; // ��ǰ�ֵ��Ľڵ㣬һ��ʼΪpHead�ڵ�
    if (now == NULL)
    {
        //mutex->unlock();;
        return;
    }
    for (int i = 1; i <= index; i++)
    {
        if (now->pNext != NULL) { // ����������һ���ڵ��Ļ�
            last = now; // �����֣���һ���ڵ�����now�ڵ�
            now = now->pNext; // ������
        }
        else break; // ��β���˾��˳�
    }
    if (last == NULL) // ����Ҫɾ��pHeadԪ��
    {
        if ((list->pHead)->pNext != NULL) // ����pHeadԪ�غ��滹��
        {
            list->pHead = (list->pHead)->pNext; // ����
        }
        else {
            list->pHead = NULL; // û�еĻ��Ͱ�pHeadԪ��ҲŪ��
            list->pTail = NULL; // û�еĻ��Ͱ�pTailԪ��ҲŪ��
        }
    }
    else {
        last->pNext = NULL; // �Ͽ���һ��������
        if (now->pNext != NULL) // �������滹��Ԫ�صĻ�
        {
            last->pNext = now->pNext; // ����
        }
    }
    //mutex->unlock();;
    free(now); // �ͷ�ɾ������Ԫ�ص��ڴ�
}


int putDetectTargettoQueue(QMutex *mutex,QQueue<target_info_list> *target_queue, target_info_list info)
{
    if(target_queue->empty())
    {
        mutex->lock();

        target_queue->enqueue(info);
        mutex->unlock();
    }
    else
    {
        mutex->lock();
        target_queue->dequeue();
        target_queue->enqueue(info);
        mutex->unlock();
    }
    return 0;
}

int putTrackTargettoQueue(QMutex *mutex,QQueue<target_info> *target_queue, target_info info)
{
    if(target_queue->empty())
    {
        mutex->lock();
        target_queue->enqueue(info);
        mutex->unlock();
    }
    else
    {
        mutex->lock();
        target_queue->dequeue();
        target_queue->enqueue(info);
        mutex->unlock();
    }
    return 0;
}

int select_target( struct target_info_list info, int state)
{
    int W,H;
    W=1024;
    H = 1024;
    float max_dist=W*H;
    int id=-1;
    if(state == 0)
    {
        for(int i=0;i<info.target_num;i++)
        {
            float dx = (info.T_info[i].info[0]+info.T_info[i].info[2]/2-W/2);
            float dy = (info.T_info[i].info[1]+info.T_info[i].info[3]/2-H/2);
            float dist=std::sqrt(dx*dx+dy*dy);
            if(dist<max_dist)
            {
                max_dist=dist;
                id = i;
            }
        }
    }
    else
    {
        max_dist=0;
        for(int i=0;i<info.target_num;i++)
        {
            float area = info.T_info[i].info[2]*info.T_info[i].info[3];

            if(area>max_dist)
            {
                max_dist=area;
                id = i;
            }
        }
    }
    return id;
}

float comput_iou(Rect p1,Rect p2)
{
    float iou=0;
    int max_x=max(p1.x, p2.x);
    int max_y=max(p1.y,p2.y);
    int min_x=min(p1.x+p1.width,p2.x+p2.width);
    int min_y=min(p1.y+p1.height,p2.y+p2.height);
    if(min_x<max_x&&min_y<max_y)
        return iou;
    float area1 =(min_x-max_x)*(min_y-max_y);
    float area2 = (p1.width*p1.height+p2.width*p2.height-area1);

    if(fabs(area2)>0.000001)
        iou= area1*1.0/area2;
    return iou;
}

// 判断两个矩形框是否重合
bool isOverlap(const Rect& rect1, const Rect& rect2) {
    // 计算两个矩形框在水平方向上的最大和最小坐标
    int left1 = rect1.x, right1 = rect1.x+rect1.width;
    int left2 = rect2.x, right2 = rect2.x+rect2.width;

    // 计算两个矩形框在垂直方向上的最大和最小坐标
    int top1 = rect1.y, bottom1 = rect1.y+rect1.height;
    int top2 = rect2.y, bottom2 = rect2.y+rect2.height;

    // 检查水平方向是否有重叠
    bool horizontalOverlap = !((right1 < left2) || (right2 < left1));

    // 检查垂直方向是否有重叠
    bool verticalOverlap = !((bottom1 < top2) || (bottom2 < top1));

    // 如果在两个方向上都有重叠，则矩形框重合
    return horizontalOverlap && verticalOverlap;
}

int match_track_detect_id_info(QMutex *detect_target_mutex,QQueue<target_info_list>* detect_target_queue, QMutex *track_target_mutex, QQueue<target_info> *track_target_queue)
{
    int result=0;
    detect_target_mutex->lock();
    track_target_mutex->lock();
    

    int dNum=detect_target_queue->size();
    int tNum=track_target_queue->size();
    
    for(int i=tNum-1;i>=0;i--)
    {
        if(result==1){
            break;
        }
        for(int j=dNum-1;j>=0;j--)
        {
            target_info_list detect_data=detect_target_queue->at(j);
            target_info  track_data = track_target_queue->at(i);
            int tId=track_data.info[4];
            int dId = detect_data.T_info[0].info[4];

            if(tId!=dId)
            {
                continue;
            }
            else
            {
                cv::Rect dRect,tRect;
                tRect.x = track_data.info[0];
                tRect.y = track_data.info[1];
                tRect.width = track_data.info[2];
                tRect.height = track_data.info[3];

                for(int t=0;t<detect_data.target_num;t++)
                {

                    dRect.x = detect_data.T_info[t].info[0];
                    dRect.y = detect_data.T_info[t].info[1];
                    dRect.width = detect_data.T_info[t].info[2];
                    dRect.height = detect_data.T_info[t].info[3];

                    float iouV=comput_iou(dRect,tRect);
                    if(iouV>0.6)
                    {
                        result=1;
                        std::cout <<tId<<"帧检测和跟踪匹配成功"<<"\n";
                        break;
                    }
                }
            }
        }
    }
    track_target_mutex->unlock();
    detect_target_mutex->unlock();
    return result;
}
