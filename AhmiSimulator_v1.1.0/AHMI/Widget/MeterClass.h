////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 3.00 - File Created 2016/04/13 by �ڴ�Ӫ
// Additional Comments:
//   
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef METERCLASS__H
#define METERCLASS__H
#include "widgetClassInterface.h"

#ifdef AHMI_CORE
class ActionTriggerClass;

class MeterClass
{
public:
	MeterClass();
	~MeterClass();

	//�ؼ����ƺ���
	funcStatus widgetCtrl(
		 WidgetClassPtr p_wptr,  //�ؼ�ָ��
		 ActionTriggerClassPtr ActionPtr,
		 u8 u8_pageRefresh      //ҳ��ˢ��
		 );
	//��ʼ��
	 funcStatus initWidget(
		WidgetClassPtr p_wptr,   //�ؼ�ָ��
		u32 *u32p_sourceShift,   //sourcebufferָ��
		u8 u8_pageRefresh,       //ҳ��ˢ��
		u8 RefreshType ,       //���ƵĶ������ͣ����ݶ������͸ı���ƿؼ��İ�Χ��
		TileBoxClassPtr pTileBox, //��Χ��
		u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
		 );

	 //compute the state of current texture
	 funcStatus renderTexture
		 (
		 WidgetClassPtr p_wptr,  //�ؼ�ָ��
		 s32 value
		 );

	 //compute the texture's angle
	 funcStatus renderTextureAngle
		 (
		 WidgetClassPtr p_wptr,  //�ؼ�ָ��
		 s16 Angle
		 );

	 //set animation tag
	funcStatus setATag(
		WidgetClassPtr p_wptr,   //�ؼ�ָ��
		u8 value                 //value of animation tag
		);
};

#endif

#endif
