////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     ahmiv3.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/22 by Arthas(������)
// Additional Comments:
//    the definition to draw animation
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATIONDEFINE__H
#define ANIMATIONDEFINE__H
#include "publicDefine.h"
#ifdef AHMI_CORE

#define NO_ANIMATION                       (0)      //�޶���
//page animation
#define PAGE_ANIMATION_SHIFTING_L          (1)      //shift from left
#define PAGE_ANIMATION_SHIFTING_R          (2)      //shift from right
#define PAGE_ANIMATION_SHIFTING_UP         (3)      //shift from up
#define PAGE_ANIMATION_SHIFTING_DOWN       (4)      //shift from down
#define PAGE_ANIMATION_ZOOM                (5)      //zoom from center

//subcanvas animation
#define SUBCANVAS_ANIMATION_SHIFTING_L          (1)      //shift from left
#define SUBCANVAS_ANIMATION_SHIFTING_R          (2)      //shift from right
#define SUBCANVAS_ANIMATION_SHIFTING_UP         (3)      //shift from up
#define SUBCANVAS_ANIMATION_SHIFTING_DOWN       (4)      //shift from down
#define SUBCANVAS_ANIMATION_ZOOM                (5)      //zoom from center
 
#define ANIMATION_HERIZONTAL_SHIFTING_L    (1)      //����ˮƽƽ�ƶ���
#define ANIMATION_HERIZONTAL_SHIFTING_R    (2)      //����ˮƽƽ�ƶ���
#define ANIMATION_VERTICAL_SHIFTING_UP     (3)      //��ֱ����ƽ�ƶ���
#define ANIMATION_VERTICAL_SHIFTING_DOWN   (4)      //��ֱ����ƽ�ƶ���
#define ANIMATION_ZOOM_IN                  (5)      //��С����
#define ANIMATION_ZOOM_OUT                 (6)      //�Ŵ󶯻�

#define ANIMATION_REFRESH_NULL      0     //��Ԫ����Ҫ����
#define ANIMATION_REFRESH_PAGE      1     //��Ҫ����ҳ��
#define ANIMATION_REFRESH_SUBCANVAS 2     //ϵͳsubcanvas�л�����
#define ANIMATION_REFRESH_CANVAS    3     //�û��Զ���canvas����
#define ANIMATION_REFRESH_WIDGET    4     //widget��ϵͳ����
#define ANIMATION_REFRESH_DOUBLE_BUFFER 5 //���»����е����ݵ���Ļ
#define ANIMAITON_REFRESH_STATIC_BUFFER 6 //���¾�̬�����е�����

#define AnimationMsg_Maxnum   16  //��Ϣ�б�����
#define NULL_Animation        4  //Ԫ�����ͣ��б���Ϣ��Ч
#define Animation_Frame       5 //�л�֡��
#ifdef PC_SIM
#define Animation_Time        3 //ÿ֡�л�ʱ�䡪��ms
#endif
#ifdef EMBEDDED
#define Animation_Time        3 //ÿ֡�л�ʱ�䡪��ms
#endif

#define PAGE_NEED_REFRESH    1  //��ʾ����page����canvas��������ʾ����
#define PAGE_NOT_REFRESH     0  //��ʾֻ����Ԫ��

#define ZOOM_SCALE       3     //�Ŵ����С���� 

//action
#define ANIMATION_DIRECTION_ADD 0 //indicating that the animation is from start to stop
#define ANIMATION_DIRECTION_MINUS 1 //indicating that the animation is from stop to start


#endif

#endif
