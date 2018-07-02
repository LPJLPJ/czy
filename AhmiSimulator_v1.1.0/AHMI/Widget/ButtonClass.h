////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//   
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef BUTTONCLASS__H
#define BUTTONCLASS__H
#include "widgetClassInterface.h"

#ifdef AHMI_CORE
#ifdef __cplusplus
class ActionTriggerClass;

class ButtonClass 
{
public:
	ButtonClass(void);
	~ButtonClass(void);
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
};

typedef ButtonClass* ButtonClassPtr;
typedef ActionTriggerClass* ActionTriggerClassPtr;
#endif
#endif

#endif
