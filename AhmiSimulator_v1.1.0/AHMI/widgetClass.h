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
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef WIDGETCLASS__H
#define WIDGETCLASS__H

#include "publicType.h"
#include "publicDefine.h"
#include "widgetClass_c1.h"
#include "widgetClass_cd.h"
#include "publicDefine.h"

#ifdef AHMI_CORE


class WidgetClass
{
public:
	//����
	u16 WDG_u16_userVariables[WIDGET_UNVAR];
	matrixClass mWidgetMatrix;
	//����
	WidgetClass(void);
	~WidgetClass(void);
	//���ƿؼ�
	funcStatus drawWidget(
		TileBoxClassPtr pTileBox,
		u32 *sourceShift,
		u8  pageEnable,
		matrixClassPtr pCanvasMatrix,
		u8 RefreshType,        //���ƵĶ������ͣ����ݶ������͸ı���ƿؼ��İ�Χ��
		u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
		);
	//���ƿؼ���ʱ����ƶ���
	funcStatus drawWidgetWithAnimation(
		u8 curFrame,     //��ǰ֡
		u8 totalFrame,   //�ܹ�֡
		TileBoxClassPtr pSourceBox,
		u32 *sourceshift    //sourcebufferƫ��
		);
	//��ʼ���ؼ�����
	funcStatus initWidget(void);
	//�����ؼ����������б�
	funcStatus loadWidgetWithAnimation(
		);
	//�Զ����ķ�ʽ����widget�ڵ�����
	funcStatus drawTextureWithAnimation(
		u8 curFrame,     //��ǰ֡
		u8 totalFrame,   //�ܹ�֡
		TileBoxClassPtr pSourceBox,
		u32 *sourceshift    //sourcebufferƫ��

		);
	//���������������б�
	funcStatus loadTextureWithAnimation(
		);
	//�жϿؼ��Ƿ���ʾ
	funcStatus displayEn(
		u8 widgetID
		);
	//set widget animation tag
	funcStatus setATag(
		u8 value
		);
};
#endif

#endif

