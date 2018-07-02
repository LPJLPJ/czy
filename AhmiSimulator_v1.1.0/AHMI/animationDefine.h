////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     ahmiv3.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/22 by Arthas(周钰致)
// Additional Comments:
//    the definition to draw animation
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATIONDEFINE__H
#define ANIMATIONDEFINE__H
#include "publicDefine.h"
#ifdef AHMI_CORE

#define NO_ANIMATION                       (0)      //无动画
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
 
#define ANIMATION_HERIZONTAL_SHIFTING_L    (1)      //向左水平平移动画
#define ANIMATION_HERIZONTAL_SHIFTING_R    (2)      //向右水平平移动画
#define ANIMATION_VERTICAL_SHIFTING_UP     (3)      //垂直向上平移动画
#define ANIMATION_VERTICAL_SHIFTING_DOWN   (4)      //垂直向下平移动画
#define ANIMATION_ZOOM_IN                  (5)      //缩小动画
#define ANIMATION_ZOOM_OUT                 (6)      //放大动画

#define ANIMATION_REFRESH_NULL      0     //无元件需要更新
#define ANIMATION_REFRESH_PAGE      1     //需要更新页面
#define ANIMATION_REFRESH_SUBCANVAS 2     //系统subcanvas切换动画
#define ANIMATION_REFRESH_CANVAS    3     //用户自定义canvas动画
#define ANIMATION_REFRESH_WIDGET    4     //widget内系统动画
#define ANIMATION_REFRESH_DOUBLE_BUFFER 5 //更新缓存中的内容到屏幕
#define ANIMAITON_REFRESH_STATIC_BUFFER 6 //更新静态缓存中的内容

#define AnimationMsg_Maxnum   16  //信息列表容量
#define NULL_Animation        4  //元件类型：列表信息无效
#define Animation_Frame       5 //切换帧数
#ifdef PC_SIM
#define Animation_Time        3 //每帧切换时间——ms
#endif
#ifdef EMBEDDED
#define Animation_Time        3 //每帧切换时间——ms
#endif

#define PAGE_NEED_REFRESH    1  //表示更新page或者canvas，并且显示动画
#define PAGE_NOT_REFRESH     0  //表示只更新元件

#define ZOOM_SCALE       3     //放大和缩小倍数 

//action
#define ANIMATION_DIRECTION_ADD 0 //indicating that the animation is from start to stop
#define ANIMATION_DIRECTION_MINUS 1 //indicating that the animation is from stop to start


#endif

#endif
