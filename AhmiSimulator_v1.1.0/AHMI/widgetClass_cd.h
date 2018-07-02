////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    definition used only by widget
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef WIDGETCLASS_CD__H
#define WIDGETCLASS_CD__H
#include "publicDefine.h"
#ifdef AHMI_CORE

#define WIDGET_GENERAL_NUM (21) //ͨ�ò������� ����tagchange��ָ�����������һ������
#define WIDGET_UNVAR (WIDGET_GENERAL_NUM + 19)  //widget�������

#define WidgetAttr							WDG_u16_userVariables[(u32)0]  
#define NumOfTex							WDG_u16_userVariables[1]  
#define StartNumofTex						WDG_u16_userVariables[2]  //�ؼ�������������ṹ��ĵ�һ��Ԫ���±�
#define WidgetWidth							WDG_u16_userVariables[3]  //�ؼ�������
#define WidgetHeight						WDG_u16_userVariables[4]  //�ؼ�����߶�
#define WidgetOffsetX						WDG_u16_userVariables[5] //�ؼ�λ�ã����Ͻ�X����,����
#define WidgetOffsetY						WDG_u16_userVariables[6] //�ؼ�λ�ã����Ͻ�Y���꣬����
#define BindTagID							WDG_u16_userVariables[7]//���󶨵�TagID
#define BindMatrix							WDG_u16_userVariables[8]  //�󶨵ľ���


//�����¼�ר�ò���
#define EnterHighAlarmAction				WDG_u16_userVariables[9]
#define EnterLowAlarmAction					WDG_u16_userVariables[10]
#define LeaveHighAlarmAction				WDG_u16_userVariables[11]
#define LeaveLowAlarmAction					WDG_u16_userVariables[12]
#define OnRealeaseAction					WDG_u16_userVariables[13]	
#define TagChangeAction					    WDG_u16_userVariables[20]   //add by Mr.z to realize tag change

//�ؼ�����ר�ò���
#define TOTALFRAME_AND_NOWFRAME             WDG_u16_userVariables[14]   //[7:0] now frame [15:8] total frame. if total frame == 0, indicates that the widget contains no animaiton
#define START_TAG_L                         WDG_u16_userVariables[15]   //start value, used for animation
#define START_TAG_H                         WDG_u16_userVariables[16]	//start value, used for animation
#define STOP_TAG_L                          WDG_u16_userVariables[17]   //stop value, used for animation
#define STOP_TAG_H                          WDG_u16_userVariables[18]   //stop value, used for animation
#define ATTATCH_CANVAS						WDG_u16_userVariables[19]   //���ŵ�canvas

//�Ǳ��̡����顢ʾ���������ֿ�ר�ò���
#define LowAlarmValueL						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0] 
#define LowAlarmValueH						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1] 
#define HighAlarmValueL						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2]
#define HighAlarmValueH						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 3]
#define MinValueL							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 4] 
#define MinValueH							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 5]
#define MaxValueL							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 6]  
#define MaxValueH							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 7]

//������ר��
#define StartColorValueGB					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8]
#define EndColorValueBAndStartColorValueR	WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9]
#define EndColorValueRG						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10]

//��ɫ������ר��
#define COLOR0GB                            WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8]
#define COLOR1B_COLOR0R                     WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9]
#define COLOR1RG							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10]
#define COLOR2GB							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 11]
#define COLOR2R                             WDG_u16_userVariables[WIDGET_GENERAL_NUM + 14]
#define THRESHOLD0_L                        WDG_u16_userVariables[WIDGET_GENERAL_NUM + 15]
#define THRESHOLD0_H                        WDG_u16_userVariables[WIDGET_GENERAL_NUM + 16]
#define THRESHOLD1_L                        WDG_u16_userVariables[WIDGET_GENERAL_NUM + 17]
#define THRESHOLD1_H                        WDG_u16_userVariables[WIDGET_GENERAL_NUM + 18]

//��ťר��
#define OnEnteredAction                     WDG_u16_userVariables[13]                         //������ȷ�ϰ�ť���µ�ָ��
#define KeyboardPreAndNextWidget            WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0]     //{ǰһ����������һ������}
#define NumOfButtonAndCurHighLight          WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1]     //{��ť��������ǰ����ֵ}
#define ReleaseColorValueGB					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2]
#define PressValueBAndReleaseColorValueR	WDG_u16_userVariables[WIDGET_GENERAL_NUM + 3]
#define PressColorValueRG					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 4]

//�Ǳ���ר�ò���
#define MeterMinAngle						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8] 
#define MeterMaxAngle						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9] 	
#define StartAngle							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10]    //��ʼ�Ƕ�
     
//��̬����ר�ò���
#define DynamicTexMinAngle					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8] 
#define DynamicTexMaxAngle					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9] 	 
                                            
//ʾ����ר�ò���       
#define CurPosI								WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8] //������
#define OSCColor							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9] //������ɫ��R5G6B5
#define BLANK_X                             WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10] //X��������                 
#define BLANK_Y                             WDG_u16_userVariables[WIDGET_GENERAL_NUM + 11] //Y��������
	

	
//����������Ƶ����ר�ò���
#define FrameSpeed							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0] //ÿ�����ٺ�����ʾһ֡
#define NowTime								WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1] //���ھ����˶��ٺ���

//���������ò���
#define TotalSlice							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1] //�ܹ���Slice����

//���пؼ�ר�ò���
#define NumOfLine							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8]        //8~11λ��ʾ���ֿ�Ķ��뷽ʽ����12~15λ��ʾ���ݳ������ֵ��Ĵ���ʽ [7:0] �������ֿؼ��Ķ������ͣ�Ŀǰ����0��ʾ�޶�����1��ʾ��������������л�����
#define PreviousTexturePtrFlag				WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9]        //���ڵ���ģʽ,0��ʾ�������ƣ�1��ʾ��������
#define CurValueL							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10]        //�м����ֿ���ʾ��ֵ
#define CurValueH							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 11]        //�м����ֿ���ʾ��ֵ


//����������̬�����Ǳ��̡����ֿ�ʾ����ר��
#define OldValueL							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 12]       //��16λ
#define OldValueH							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 13]       //��16λ
#define SPACINGX							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 14]       //���ּ�� SpacingX                                                
#define ANIMATIONCURVALUE_L					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 15]       //�Ǳ��̻��߽������ж���ִ�еĵ�ǰֵ by Mr.z
#define ANIMATIONCURVALUE_H					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 16]       //�Ǳ��̻��߽������ж���ִ�еĵ�ǰֵ by Mr.z 
//�ı���ר�ò��� ���ֿ�ר�ò�    ��                            
                                               


//IOר��
#define IOX									WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0]      //ѡ����IO��
#define PinMusk								WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1]      //��Ч��Pin
#define OpenPin								WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2]      //�Ѿ��򿪵�Pin


//��������

#define   DYNAMIC_TEX	 1                 //�������������ַ�ɱ�
#define   BUTTON_TEX	 2                 //��ť�����������������Ӧ�ã�����action
#define   METER_TEX		 3                 //�Ǳ���
#define   PROGBAR_TEX	 4                 //������
#define   SLIDER_TEX	 5                 //����
#define   OSCSCP_TEX	 6                 //ʾ����
#define   TEXTBOX_TEX	 7                 //�ı���
#define   NUMBOX_TEX	 8                 //���ֿ��ı��������Ӧ��
#define   ANIME_TEX      9                 //���������������������Ӧ�ã������Զ��ı���������
#define   VIDEO_TEX     10                 //��Ƶ������������ͷ
#define   KNOB_TEX      11                 //��ť
#define   CLOCK_TEX     12                 //ʱ���
#define   ON_OFF_TEX    13                 //����
#define   TEXTURE_NUM    14                 //ͼ�����ֿؼ�
#define   TEXTURE_TIME    15                 //ͼ��ʱ��ؼ�


//�������õ��ĺ�
//dynamicTex
#define  DYNAMIC_TYPE_BIT 0x00E0 //��̬�������͵�λ

//button
#define  SINGLEBUTTON (0) //�򵥰�ť
#define  SWITCHBUTTON (1) //����
#define  COMPLEXBUTTON (2) //���Ӱ�ť
#define  SINGLE_COLOR_BUTTON_BIT (0x800) //�����Ƿ�Ϊ��ɫ��ť��widget���Ե�11λ

//clock
#define CLOCK_HOUR_MINUTE_SECOND_MODE (0) //ʱ����
#define CLOCK_HOUR_MINUTE_MODE        (1) //ʱ��
#define CLOCK_GANG_YEAR_MODE          (2) //б��������
#define CLOCK_MINUS_YEAR_MODE         (3) //����������  
//rotating dynamic texture
#define ROTATING_DYNAMIC_TEX 0x20
#define DIM_TEX              0x60



//Progbar
#define  CHANGECOLOR (0x100) //�������Ƿ�ı���ɫ
#define  CHANGECOLOR_MODE (0x200) //0��ʾ���䣬1��ʾͻ��
#define  BAR_IS_COLOR   (0x80)  //�������ǲ�����ɫ
#define	 LINE_FILE_EN (0x40)  //�Ƿ���ָ������
#define  LINE_CHANGE_COLOR (0x0400) //ָ���Ƿ�ı���ɫ
#define  PROBARDISPLAY		(0x0800)//�Ƿ���ʾ
#define  THRESHOLD_NUM      (0x1000) //��ֵ������0��ʾ1����1��ʾ2��
#define  PROGBAR_DIR        (0x20)  //����

//���ֿ���뷽ʽ
#define		RIGHTALIGN	0
#define     CENTERALIGN	1
#define		LEFTALIGN	2

//���ݳ������ֵ��Ĵ���ʽ 0����ʾ���ֵ�� 1������ʾ
#define    OVERFLOW_DISPLAY     0
#define    OVERFLOW_NON_DISPLAY 1


//��ת��ʽ
#define CENTRAL_ROTATE (1) //������ת
#define TRANSLATION    (2) //ƽ��
#define DIM            (3) //����

//meter
#define  METER_SIMPLIFY 0x80

//video
#define VIDEO_TYPE_HDMI 0x20

#define  NUMOFNUMBER 32/4//�����ʾ���ٸ�����

//test
#define MAX_WIDTH_AND_HEIGHT 1920
#define MAX_ANGLE            360

#endif

#endif
