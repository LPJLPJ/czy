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
#ifndef NUMBERCLASS__H
#define NUMBERCLASS__H
#include "widgetClassInterface.h"

#ifdef AHMI_CORE
class ActionTriggerClass;

class NumberClass
{
public:
	NumberClass();
	~NumberClass();

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

	//trigger animation
	funcStatus setATag(
		WidgetClassPtr p_wptr,   //�ؼ�ָ��
		u8 value
		);
};

#endif

#endif
