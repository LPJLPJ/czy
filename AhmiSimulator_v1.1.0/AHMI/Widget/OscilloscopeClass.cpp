////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 3.00 - File Created 2016/04/13 by 于春营
// Additional Comments:
//   
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "AHMIBasicDefine.h"
#include "aniamtion.h"
#include "OscilloscopeClass.h"
#include "drawImmediately_cd.h"
#include "myMathClass.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;
extern u8					sourceBuffer[2048];

//-----------------------------
// 函数名： OSCClass
// 构造函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
OSCClass::OSCClass()
{

}

//-----------------------------
// 函数名： ~OSCClass
// 析构函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
OSCClass::~OSCClass()
{

}

//****************************
// 函数名： OSCClass::initWidget
// 初始化并绘制时间框，如果是全屏刷新，先根据所绑定的tag刷新该控件，在绘制该控件
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //页面刷新
//  @param   TileBoxClassPtr pTileBox //包围盒
// ***************************
funcStatus OSCClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
	)

{
	TextureClass newTexture;
	//s16 A,B,E,F;
//	u16 texAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE;
	//s16 Height,Width;
	s32 height0,width0;
	s16 COS,SIN;
	s32 COS0,SIN0;
	u8   lineWidth;
	u8   stepX;
	u16* samplePointBuf;
	u16  SampleBufSize;
	u16 curposi;//当前点序号
	u16 curposX; //当前点X坐标
	
	//u16 EndPosX;//结束点X坐标
	u16 endPosi;//结束点序号

//	u16 startPosX;//起始点X坐标
	s16 startPosi;//起始点序号

	u8 p1,p2;
	int i;
//	u8 flag=0;
	u8 *sourcebufferaddr;
	u8 breakLine;
	u8 crossTile;//对于处在Tile分界面上的线，需要把两个Tile都画了
	matrixClass matrixTemp;
	myMathClass mathTemp;
	float f_tany;
	s16   s_tany;

	if(NULL == p_wptr)
		return AHMI_FUNC_FAILURE;

	//写入背景文件
	matrixTemp.matrixInit();
	newTexture = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + p_wptr->NumOfTex - 1];
	newTexture.writeSourceBuffer(u32p_sourceShift,&matrixTemp,p_wptr,NULL);


	lineWidth = (p_wptr->WidgetAttr >>12) & 0x7;
	stepX = (p_wptr->WidgetAttr>>5);
	stepX = stepX  & 0x3f;
	samplePointBuf=(u16*)&(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
	SampleBufSize = ( (p_wptr->NumOfTex - 2) * sizeof(TextureClass)) >> 1;
	curposi = p_wptr->CurPosI - 1;
	curposX = curposi * stepX  + p_wptr->WidgetOffsetX ; 


	matrixTemp.matrixInit();								//初始化为单位阵
	sourcebufferaddr = (u8 *)SoureBufferAddr;
	breakLine = (u8)0;//波前的点

	if(curposi!=0 && (curposX>>5) != ((curposX - stepX)>>5))//需要画的线跨越了2个tile
	{
		crossTile = 1;
	}
	else crossTile = 0;

//	startPosX = curposX & (~0x1f) ;// 取Tile边界，Tile大小32
	if(crossTile) 
		startPosi = curposi - ((curposX & 0x1f) + 32 + stepX -1)/stepX;
	else	
		startPosi = curposi - ((curposX & 0x1f) + stepX -1)/stepX;//Curposi - ((CurposX - StartPosX + StepX-1)/StepX )

	if(startPosi<0) 
		startPosi = 0;



	if(p_wptr->WidgetAttr & 0x800)
	{
		/*if(crossTile) 
			endPosi = startPosi + (32+31)/stepX + 1;
		else 
			endPosi = startPosi + 31/stepX + 1;	
		breakLine = 1;*/
		endPosi = startPosi = 0;
		p_wptr->WidgetAttr = p_wptr->WidgetAttr & (~0x800);
		p_wptr->CurPosI = 0;
		return AHMI_FUNC_SUCCESS;
	}
	else
	{
		endPosi = curposi;
		breakLine = 0;
	}
	if(endPosi>=SampleBufSize || endPosi >= (p_wptr->WidgetWidth - p_wptr->BLANK_X)/stepX) 
		endPosi = ( (p_wptr->WidgetWidth) - (p_wptr->BLANK_X) )/ stepX - 1;

	//清空alpha buffer
	newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE | MASKTYPE_OVERLAP ;
	newTexture.TexWidth = 0;
	newTexture.TexHeight = 0;
	newTexture.OffsetX = 0;
	newTexture.OffsetY = 0;
	/*newTexture.TexLeftTileBox = curposX>>5;
	if(crossTile) 
		newTexture.TexLeftTileBox --;
	newTexture.TexTopTileBox = (p_wptr->WidgetOffsetY) >>5;
	newTexture.TexRightTileBox = (curposX)>>5;
	newTexture.TexButtomTileBox = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight + lineWidth)>>5;*/
	newTexture.TexLeftTileBox = pTileBox->LeftBox;
	newTexture.TexRightTileBox = pTileBox->RightBox;
	newTexture.TexTopTileBox = pTileBox->TopBox;
	newTexture.TexButtomTileBox = pTileBox->ButtomBox;

	newTexture.writeSourceBuffer(u32p_sourceShift,  &matrixTemp,p_wptr,NULL);

	//写入直线的alpha+纯颜色
	if(startPosi == endPosi)//第一个画点
	{
		newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE | MASKTYPE_OVERLAP;
		newTexture.TexWidth = lineWidth;
		newTexture.TexHeight = lineWidth;
		newTexture.OffsetX = (curposX <<4);
		newTexture.OffsetY = (samplePointBuf[startPosi] <<4);	
		newTexture.TexLeftTileBox = (curposX)>>5;
		newTexture.TexTopTileBox =  (samplePointBuf[startPosi])>>5;
		newTexture.TexRightTileBox = (curposX)>>5;
		newTexture.TexButtomTileBox = (samplePointBuf[startPosi] + lineWidth)>>5;
		newTexture.RotateAngle = 0;


		newTexture.writeSourceBuffer( u32p_sourceShift,  &matrixTemp,p_wptr,NULL);

	}
	else
	{

		for(i=startPosi+1;i <= endPosi;i++)
		{
			if(breakLine && i-1 == curposi) continue;
			p1 = i-1;
			p2 = i;
			newTexture.OffsetX = ((p_wptr->WidgetOffsetX + p1*stepX ) <<4 );
			newTexture.OffsetY = (samplePointBuf[p1] <<4 );
			
			//两点的连线
			width0 = (1 + stepX)<<6;//1.25.6
			height0 = (samplePointBuf[p2] - samplePointBuf[p1])<<6;//1.25.6
			

			//计算包围盒
			if(height0>=0)//连线方向向下
			{

				newTexture.TexLeftTileBox =   (curposX)>>5;
				if(crossTile) 
					newTexture.TexLeftTileBox --;
				newTexture.TexTopTileBox =    (samplePointBuf[p1])>>5;
				newTexture.TexRightTileBox =  (curposX)>>5;
				newTexture.TexButtomTileBox = (samplePointBuf[p2]+lineWidth)>>5;
			}
			else//连线方向向上
			{

				newTexture.TexLeftTileBox =   (curposX)>>5;
				if(crossTile) 
					newTexture.TexLeftTileBox --;
				newTexture.TexTopTileBox =    (samplePointBuf[p2])>>5;
				newTexture.TexRightTileBox =  (curposX)>>5;
				newTexture.TexButtomTileBox = (samplePointBuf[p1]+lineWidth)>>5;
			}

			COS0 = width0<<9;  //1.16.15
			SIN0 = height0<<9; //1.16.15
			mathTemp.FindAngle(&width0,&height0);
			COS0 /=width0;	//1.22.9
			SIN0 /=width0;	//1.22.9
			COS = (s16)COS0; //1.6.9
			SIN = -(s16)SIN0; //1.6.9
			newTexture.TexWidth = (s16)((width0)>>6);
			newTexture.TexHeight =(s16)(lineWidth);

			//写入直线的alpha纹理
			if(i == startPosi+1)
				newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE | MASKTYPE_OVERLAP ;
			else 
				newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE | MASKTYPE_OR ;

			//writing headers
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.mTexAttr & 0xff);//texfisrtheader
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((newTexture.mTexAttr >> 8) & 0xff);//texsecondheader

			//writing boxsize
			*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexLeftTileBox;//startx
			*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexTopTileBox; //starty
			*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexRightTileBox; //stopx
			*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexButtomTileBox;//stopy
								   
			//writing texsize	  
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth >> 8 & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight >> 8 & 0xff);
							
			//writing matrix
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(COS & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(COS >> 8 & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(SIN & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(SIN >> 8 & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) >> 8 & 0xff); 
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) >> 8 & 0xff);

		}
	}

	newTexture.mTexAttr = PURECOLOR | MASKTEX | EFMATRIX | ADDRTEXTURE;
	newTexture.TexWidth = p_wptr->WidgetWidth;
	newTexture.TexHeight = p_wptr->WidgetHeight;
	newTexture.OffsetX = (p_wptr->WidgetOffsetX)<<4;
	newTexture.OffsetY = (p_wptr->WidgetOffsetY)<<4;
	newTexture.TexLeftTileBox =  (p_wptr->WidgetOffsetX  )>>5;
	newTexture.TexTopTileBox =  (p_wptr->WidgetOffsetY   )>>5;
	newTexture.TexRightTileBox = (p_wptr->WidgetOffsetX + p_wptr->WidgetWidth )>>5 ;
	newTexture.TexButtomTileBox = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight)>>5 ;
	newTexture.TexAddr = ((p_wptr->OSCColor & 0xf800) << 8) //r
						+((p_wptr->OSCColor & 0x07e0) << 5) //g
						+((p_wptr->OSCColor & 0x001f) << 3) //b
						+0xff000000;
	newTexture.SingleSliceSize = 0;
	newTexture.FocusedSlice = 0;

	newTexture.writeSourceBuffer( u32p_sourceShift,&matrixTemp,p_wptr,NULL);

	//写入错切的alpha纹理以显示背光
	if(startPosi == endPosi)//第一个画点
	{
	}
	else
	{
		for(i=startPosi+1;i <= endPosi;i++)
		{
			if(breakLine && i-1 == curposi) continue;
			p1 = i-1;
			p2 = i;
			
			//两点的连线
			width0 = (1 + stepX)<<6;//1.25.6
			height0 = (samplePointBuf[p2] - samplePointBuf[p1])<<6;//1.25.6
			f_tany = (float)(-height0 * 1.0) / width0;
			mathTemp.transferFloatToS16(f_tany, &s_tany);

			if(i == startPosi+1)
				newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABCDEFMATRIX | NONADDRTEXTURE | MASKTYPE_OVERLAP ;
			else 
				newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABCDEFMATRIX | NONADDRTEXTURE | MASKTYPE_OR ;

			

			newTexture.TexWidth = stepX * (p2 - p1);
			newTexture.TexHeight = (p_wptr->WidgetHeight) + height0 + (p_wptr->BLANK_Y);	

			//计算包围盒
			if(height0>=0)//连线方向向下
			{

				newTexture.TexLeftTileBox =   (curposX)>>5;
				if(crossTile) 
					newTexture.TexLeftTileBox --;
				newTexture.TexTopTileBox =    (samplePointBuf[p1])>>5;
				newTexture.TexRightTileBox =  (curposX)>>5;
				newTexture.TexButtomTileBox = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight)>>5;
				newTexture.OffsetX = ((p_wptr->WidgetOffsetX + p1*stepX) <<4 );
				newTexture.OffsetY = (samplePointBuf[p1] <<4 );
				

				//writing headers
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.mTexAttr & 0xff);//texfisrtheader
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((newTexture.mTexAttr >> 8) & 0xff);//texsecondheader

				//writing boxsize
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexLeftTileBox;//startx
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexTopTileBox; //starty
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexRightTileBox; //stopx
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexButtomTileBox;//stopy
									   
				//writing texsize	  
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth >> 8 & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight >> 8 & 0xff);
								
				//writing matrix(错切)
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(512 & 0xff);      //A
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( (512 >> 8) & 0xff); //A
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( s_tany & 0xff) ;       //B
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(( (s_tany)>> 8) & 0xff);        //B
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)0;//(s_tany & 0xff);        //C
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)0;//( (s_tany >> 8) & 0xff);        //C
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(512 & 0xff);      //D
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( (512 >> 8) & 0xff); //D
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) >> 8 & 0xff); 
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) >> 8 & 0xff);

			}
			else//连线方向向上
			{

				newTexture.TexLeftTileBox =   (curposX)>>5;
				if(crossTile) 
					newTexture.TexLeftTileBox --;
				newTexture.TexTopTileBox =    (samplePointBuf[p2])>>5;
				newTexture.TexRightTileBox =  (curposX)>>5;
				newTexture.TexButtomTileBox = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight)>>5;
				newTexture.OffsetX = ((p_wptr->WidgetOffsetX + p1*stepX) <<4 );
				newTexture.OffsetY = (samplePointBuf[p1] <<4);

				//writing headers
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.mTexAttr & 0xff);//texfisrtheader
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((newTexture.mTexAttr >> 8) & 0xff);//texsecondheader

				//writing boxsize
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexLeftTileBox;//startx
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexTopTileBox; //starty
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexRightTileBox; //stopx
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexButtomTileBox;//stopy
									   
				//writing texsize	  
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth >> 8 & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight >> 8 & 0xff);
								
				//writing matrix(错切)
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(512 & 0xff);      //A
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( (512 >> 8) & 0xff); //A
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( s_tany & 0xff) ;       //B
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(( (s_tany)>> 8) & 0xff);        //B
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( 0* 512 & 0xff) ;//(s_tany & 0xff);        //C
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(( (0 * 512)>> 8) & 0xff);//( (s_tany >> 8) & 0xff);        //C
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(512 & 0xff);      //D
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( (512 >> 8) & 0xff); //D
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) >> 8 & 0xff); 
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) >> 8 & 0xff);

			}

		}
	}

	//写入背光文件
	newTexture = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + p_wptr->NumOfTex - 2];
	newTexture.writeSourceBuffer(u32p_sourceShift,&matrixTemp,p_wptr,NULL);

	
	//if(p_wptr->CurPosI > p_wptr->WidgetWidth/stepX)
	//{
	//	p_wptr->CurPosI = 0;
	//}

	return AHMI_FUNC_SUCCESS;
}

//****************************************
//示波器控制函数
//-----------------------
//示波器函数非常特殊！！！
//-----------------------
//示波器控制函数并不包括背景，仅有采样点和它们的连线！
//
//示波器控制函数使用了分配给它的纹理结构体的空间，但并非使用此结构，
//而是利用该空间存储波形采样点的坐标。
//每个BasicTextureTag结构体拥有32字节空间，因此可以存储16个采样点信息。
//示波器控制函数在绘制时有专用Draw函数，可以自主产生绘制信息而不利用BasicTextureTag提供的信息。
//
//WidgetAttr标识：
//15:old_value_init,用来判断old值是否有效
//14-12:线宽
//11：是否是为第一屏，该参数会在运行时被修改，在刷屏时复位。默认值为0
//10-5:StepX，示波器横向坐标间隔
//4-0:控件类型，示波器为0x6
//****************************************
funcStatus OSCClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//页面刷新
	)
{
	
	u32  actionAddr;
	u32	maxValue;
	u32	minValue;
	u32 lowAlarmValue ;
	u32 highAlarmValue;
	u32 value;
	u32 oldValue=0;
//	RefreshMsg refreshMsg;
	u16* samplePointBuf;
	u16  sampleBufSize;
	u16 oldValueinit=0;
	u8   lineWidth;
	u8   stepX;
	

	if((NULL == p_wptr) || (NULL == ActionPtr))
		return AHMI_FUNC_FAILURE;

	samplePointBuf=(u16*)&(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
	sampleBufSize = ( (p_wptr->NumOfTex - 2) * sizeof(TextureClass)) >> 1;
//	refreshOldPoint = (p_wptr->WidgetAttr & 0x800)?1:0;
	lineWidth = (p_wptr->WidgetAttr >>12) & 0x7;
	stepX = (p_wptr->WidgetAttr>>5) & 0x3f;
	oldValueinit = (p_wptr->WidgetAttr & 0x8000);

	maxValue = (p_wptr->MaxValueH << 16) + p_wptr->MaxValueL;
	minValue = (p_wptr->MinValueH << 16) + p_wptr->MinValueL;
	lowAlarmValue  = (p_wptr->LowAlarmValueH << 16) + p_wptr->LowAlarmValueL;
	highAlarmValue = (p_wptr->HighAlarmValueH << 16) + p_wptr->HighAlarmValueL;

	value = ActionPtr->mTagPtr->mValue;
	value += p_wptr->BLANK_Y;

	if(p_wptr->CurPosI>=sampleBufSize) 
		return AHMI_FUNC_FAILURE;//数组溢出


	samplePointBuf[p_wptr->CurPosI++] = p_wptr->WidgetOffsetY + p_wptr->WidgetHeight - lineWidth
								- ((value - minValue) * (p_wptr->WidgetHeight - lineWidth) /(maxValue - minValue));
	if(p_wptr->CurPosI > (p_wptr->WidgetWidth - p_wptr->BLANK_X)/stepX)
	{
		//Ptr->CurPosX = 0;把该句话放到绘图函数中
		p_wptr->WidgetAttr = p_wptr->WidgetAttr | 0x800;
	}

	//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
	refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
	refreshMsg.mElementPtr.wptr = p_wptr;
	sendToRefreshQueue(&refreshMsg);
#endif

	if(oldValueinit)
	{
		oldValue = (p_wptr->OldValueH  << 16) + p_wptr->OldValueL;
		if(p_wptr->EnterLowAlarmAction && oldValue > lowAlarmValue && value <= lowAlarmValue)
		{
			//进入低值预警
			if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}		
		}
		else if(p_wptr->LeaveLowAlarmAction && oldValue <= lowAlarmValue && value > lowAlarmValue)
		{
			//离开低值预警
			if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}		
		}
		if(p_wptr->EnterHighAlarmAction && oldValue < highAlarmValue && value >= highAlarmValue)
		{
			//进入高值预警
			if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
			}
		}
		else if(p_wptr->LeaveHighAlarmAction && oldValue >= highAlarmValue && value < highAlarmValue)
		{
			//离开高值预警
			if(p_wptr->LeaveHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveHighAlarmAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}	
		}
		//对tagchange的支持 by Mr.z
		if(oldValue != value){
			actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->TagChangeAction);
			xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
		}
	}
	p_wptr->WidgetAttr |= 0x8000;
	p_wptr->OldValueL = (u16) value;
	p_wptr->OldValueH = (u16)(value >> 16); 

	return AHMI_FUNC_SUCCESS;

}

#endif
