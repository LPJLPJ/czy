////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     TextureClass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
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
//    纹理类
//    纹理属性:
//    15   : drawing,是否绘制，只用于软件
//    14   : using widget box 使用widget的包围盒
//    13   : animationMoving 在绘制纹理动画时是否移动，只用于软件
//    12   : using_phsical_pixel_ratio 修正因为屏幕像素点不是正方形导致的畸变问题，1表示需要修正，只用于软件, by zuz 20180802
//    11   : reserved, by zuz 20180802
//    10-9 : maskType mask与或非
//    8    : addrType 地址类型
//    7-4  : TexType 纹理类型
//    3:     mask    是否使用mask
//    2-1  : matrix  矩阵类型
//    0    : end     最后一张纹理 
//
//////////////////////////////////////

class PointClass;

class TextureClass
{
public:
	//函数
	u16 mTexAttr;    //纹理属性
	u16 TexWidth;   //纹理宽,整数
	u16 TexHeight;	//纹理高，整数
	u16 FocusedSlice;//当前指向的纹理片
	s16 OffsetX;//1.11.4，纹理平移向量X
	s16 OffsetY;//1.11.4，纹理平移向量Y
	s16 RotateAngle;//1.6.9，纹理旋转角度，度表示
	s16 ShearAngleX;//1.6.9，X方向的纹理错切角度，tan0表示没有发生错切，tan90表示错切为直线
	s16 ShearAngleY;//1.6.9，Y方向的纹理错切角度
	s16 ScalerX;    //1.6.9，纹理放缩向量X
	s16 ScalerY;    //1.6.9，纹理放缩向量Y
	//纹理的包围盒，这里的包围盒指纹理可能覆盖的最大范围，不是时变的
	s8  TexLeftTileBox      ;  //包围盒
	s8  TexTopTileBox     	;  //包围盒
	s8  TexRightTileBox   	;  //包围盒
	s8  TexButtomTileBox  	;  //包围盒
	u32 SingleSliceSize;
	u32 TexAddr;//纹理地址列表首地址
	u8  staticTexture; //是否将本纹理加入静态存储空间
	//函数
	TextureClass(void);
	~TextureClass(void);

	funcStatus writeSourceBuffer(
		u32 *sourceShift,
		matrixClassPtr addtionalMatrix,
		WidgetClassPtr p_wptr,
		TileBoxClassPtr pTileBox
		 );

	//优化包围盒
	funcStatus renewTextureSourceBox(
		TileBoxClassPtr pTileBox,
		matrixClassPtr additionalMatrix,
		WidgetClassPtr p_wptr
		);
	//计算旋转控件的包围盒
	funcStatus RenewRotateTextureSourceBox();
	//重新赋值包围盒
	funcStatus copyBox(
		TileBoxClassPtr pTileBox
		);


	//调整平移量
	funcStatus adjustMoving(
		u16 centralLength,
		u16 centralPosX,
		u16 centralPosY
		);

	//调整包围盒
	funcStatus adjustSourceBox(

		);

	//调整缩放后的包围盒
	funcStatus adjustSclaring(
		matrixClassPtr addtionalMatrix,
		PointClass*  pointAfterScaler,
		WidgetClassPtr p_wptr
	);

};

typedef TextureClass* TextureClassPtr;

#endif

#endif


