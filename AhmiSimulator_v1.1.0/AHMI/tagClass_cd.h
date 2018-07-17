////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     Tagclass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.04 - C++, 20160321 by 周钰致
// Additional Comments:
//    definition of tag
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef TAGCLASS_CD__H
#define TAGCLASS_CD__H
#include "publicDefine.h"

#define  TAGNAMESIZE 20

#define  TAGTYPE_INT  0
#define  TAGTYPE_STR  1
#define  TAG_READ_REG 0x80
#define  TAG_NO_READ_REG 0

#define TAGREAD 0
#define TAGWRITE 1

//专用的TAG标号
#define NUM_OF_SYSTEM_TAG (9) //专用tag个数
#define SYSTEM_TAG_0 (0)//默认的0号tag
#define SYSTEM_KEYBOARD_TAG (1) //键盘tag
#define SYSTEM_RTC_YEAR_TAG (4)      //默认的RTC的tag
#define SYSTEM_RTC_HOUR_MINUITE_TAG (5) //默认的RTC时分秒tag
#define SYSTEM_PAGE_TAG             (3) //默认跳转页面的tag
#define SYSTEM_VIDEO_TAG            (2) //默认绑定视频控件的tag
#define SYSTEM_BACKLIGHT_TAG        (6) //默认调节背光的tag
#define SYSTEM_ALARM_TAG            (7) //蜂鸣器的tag
#define SYSTEM_FRAME_RATE_TAG       (8) //帧率Tag，用来测试显示帧率
#define NORMAL_TAG     (10)    //其他用户添加的tag

//tag的element type
#define ELEMENT_TYPE_WIDGET 0
#define ELEMENT_TYPE_CANVAS 1
#define ELEMENT_TYPE_PAGE   2
#define ELEMENT_TYPE_KEYBOARD   3
#define ELEMENT_TYPE_PAGE_GOTO  4

//判断tag类型
#define TAG_IS_INT(tagType) ( (tagType & TAGTYPE_STR) == TAGTYPE_INT)
#define TAG_IS_STR(tagType) ( (tagType & TAGTYPE_STR) == TAGTYPE_STR)



#endif
