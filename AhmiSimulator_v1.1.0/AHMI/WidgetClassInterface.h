////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     WidgetClassInterface.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    definition used only by widget
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef WIDGETCLASSINTERFACE__H
#define WIDGETCLASSINTERFACE__H

#include "publicType.h"
#include "publicDefine.h"
#include "widgetClassInterface_c1.h"

#ifdef AHMI_CORE
class WidgetClassInterface
{
public:
	//����
	WidgetClassInterface(void);
	~WidgetClassInterface(void);
	//��ʼ��tagtrigger
	 //virtual funcStatus initWidget(
		//WidgetClassPtr p_wptr,   //�ؼ�ָ��
		//u32 *u32p_sourceShift,   //sourcebufferָ��
		//u8 u8_pageRefresh,       //ҳ��ˢ��
		//TileBoxClassPtr pTileBox //��Χ��
		// ) = 0;
	 ////�ؼ����ƺ���
	 //virtual funcStatus widgetCtrl(
		// WidgetClassPtr p_wptr,  //�ؼ�ָ��
		// ActionTriggerClassPtr ActionPtr,
		// u8 u8_pageRefresh      //ҳ��ˢ��
		// ) = 0;
	 ////��������
	 funcStatus drawTexture(
		WidgetClassPtr p_wptr,   //????
		u32 *u32p_sourceShift,   //sourcebuffer??
		u8 RefreshType ,       //???????,????????????????
		TileBoxClassPtr pTileBox, //???
		u8 staticTextureEn          //???????????
		);
};
#endif

#endif
