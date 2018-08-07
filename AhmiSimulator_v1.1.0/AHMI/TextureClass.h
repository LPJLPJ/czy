////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     TextureClass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURECLASS__H
#define TEXTURECLASS__H

#include "publicType.h"
#include "publicDefine.h"
#include "TextureClass_c1.h"
#include "textureClass_cd.h"
#include "publicDefine.h"

#ifdef AHMI_CORE

/////////////////////////////////
//
//    ������
//    ��������:
//    15   : drawing,�Ƿ���ƣ�ֻ�������
//    14   : using widget box ʹ��widget�İ�Χ��
//    13   : animationMoving �ڻ���������ʱ�Ƿ��ƶ���ֻ�������
//    12   : using_phsical_pixel_ratio ������Ϊ��Ļ���ص㲻�������ε��µĻ������⣬1��ʾ��Ҫ������ֻ�������, by zuz 20180802
//    11   : reserved, by zuz 20180802
//    10-9 : maskType mask����
//    8    : addrType ��ַ����
//    7-4  : TexType ��������
//    3:     mask    �Ƿ�ʹ��mask
//    2-1  : matrix  ��������
//    0    : end     ���һ������ 
//
//////////////////////////////////////

class PointClass;

class TextureClass
{
public:
	//����
	u16 mTexAttr;    //��������
	u16 TexWidth;   //�����,����
	u16 TexHeight;	//����ߣ�����
	u16 FocusedSlice;//��ǰָ�������Ƭ
	s16 OffsetX;//1.11.4������ƽ������X
	s16 OffsetY;//1.11.4������ƽ������Y
	s16 RotateAngle;//1.6.9��������ת�Ƕȣ��ȱ�ʾ
	s16 ShearAngleX;//1.6.9��X�����������нǶȣ�tan0��ʾû�з������У�tan90��ʾ����Ϊֱ��
	s16 ShearAngleY;//1.6.9��Y�����������нǶ�
	s16 ScalerX;    //1.6.9�������������X
	s16 ScalerY;    //1.6.9�������������Y
	//����İ�Χ�У�����İ�Χ��ָ������ܸ��ǵ����Χ������ʱ���
	s8  TexLeftTileBox      ;  //��Χ��
	s8  TexTopTileBox     	;  //��Χ��
	s8  TexRightTileBox   	;  //��Χ��
	s8  TexButtomTileBox  	;  //��Χ��
	u32 SingleSliceSize;
	u32 TexAddr;//�����ַ�б��׵�ַ
	u8  staticTexture; //�Ƿ񽫱�������뾲̬�洢�ռ�
	//����
	TextureClass(void);
	~TextureClass(void);

	funcStatus writeSourceBuffer(
		u32 *sourceShift,
		matrixClassPtr addtionalMatrix,
		WidgetClassPtr p_wptr,
		TileBoxClassPtr pTileBox
		 );

	//�Ż���Χ��
	funcStatus renewTextureSourceBox(
		TileBoxClassPtr pTileBox,
		matrixClassPtr additionalMatrix,
		WidgetClassPtr p_wptr
		);
	//������ת�ؼ��İ�Χ��
	funcStatus RenewRotateTextureSourceBox();
	//���¸�ֵ��Χ��
	funcStatus copyBox(
		TileBoxClassPtr pTileBox
		);


	//����ƽ����
	funcStatus adjustMoving(
		u16 centralLength,
		u16 centralPosX,
		u16 centralPosY
		);

	//������Χ��
	funcStatus adjustSourceBox(

		);

	//�������ź�İ�Χ��
	funcStatus adjustSclaring(
		matrixClassPtr addtionalMatrix,
		PointClass*  pointAfterScaler,
		WidgetClassPtr p_wptr
	);

};

typedef TextureClass* TextureClassPtr;

#endif

#endif


