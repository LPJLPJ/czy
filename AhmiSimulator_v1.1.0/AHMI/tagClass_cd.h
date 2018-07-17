////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     Tagclass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.04 - C++, 20160321 by ������
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

//ר�õ�TAG���
#define NUM_OF_SYSTEM_TAG (9) //ר��tag����
#define SYSTEM_TAG_0 (0)//Ĭ�ϵ�0��tag
#define SYSTEM_KEYBOARD_TAG (1) //����tag
#define SYSTEM_RTC_YEAR_TAG (4)      //Ĭ�ϵ�RTC��tag
#define SYSTEM_RTC_HOUR_MINUITE_TAG (5) //Ĭ�ϵ�RTCʱ����tag
#define SYSTEM_PAGE_TAG             (3) //Ĭ����תҳ���tag
#define SYSTEM_VIDEO_TAG            (2) //Ĭ�ϰ���Ƶ�ؼ���tag
#define SYSTEM_BACKLIGHT_TAG        (6) //Ĭ�ϵ��ڱ����tag
#define SYSTEM_ALARM_TAG            (7) //��������tag
#define SYSTEM_FRAME_RATE_TAG       (8) //֡��Tag������������ʾ֡��
#define NORMAL_TAG     (10)    //�����û���ӵ�tag

//tag��element type
#define ELEMENT_TYPE_WIDGET 0
#define ELEMENT_TYPE_CANVAS 1
#define ELEMENT_TYPE_PAGE   2
#define ELEMENT_TYPE_KEYBOARD   3
#define ELEMENT_TYPE_PAGE_GOTO  4

//�ж�tag����
#define TAG_IS_INT(tagType) ( (tagType & TAGTYPE_STR) == TAGTYPE_INT)
#define TAG_IS_STR(tagType) ( (tagType & TAGTYPE_STR) == TAGTYPE_STR)



#endif
