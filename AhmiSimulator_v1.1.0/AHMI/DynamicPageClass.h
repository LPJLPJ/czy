////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef DYNAMICPAGECLASS__H
#define DYNAMICPAGECLASS__H

#include "publicType.h"
#include "publicDefine.h"
#include "refreshQueueHandler.h"
#include "DynamicPageClass_c1.h"

#ifdef AHMI_CORE

class DynamicPageClass
{
public:
	//变量
	u32 mStoreOffset;//页面位于存储文件的偏移量
	u32 mTotalSize;  // DynamicPage动态数据的总大小
	u16 mPageAttr;  //Page属性，预留 
	u16 mTotalNumOfTexture;//本页的总纹理数量（包括背景纹理）
	u16 mNumOfWidget;//当前页面包含widget数量
	u16 mActionInstructionsSize;//指令队列总长度
	u16 mNumOfPageAction;//PageAction数量
	u16 mNumOfCanvasAction;//canvasAction数量
	u16 mNumofTouchWidget;//可触碰控件数量
	u8  mNumOfCanvas;//当前页面包含canvas数量
	u8  mNumOfSubCanvas;//当前页面包含子画布数量
	u8  mAnimationType   ;//动画类型
	u8  mtotoalFrame     ;//总帧数
	u8  mATag;            //animation tag
	u8  curEnlightedWidgetID; //当前高亮的widgetID
	u8  curEnlightedWidgetLighted; //当前高亮控件是否保持高亮
	u8	mStartOfStack;    //换页时传递的参数在栈中的位置
	u8	mNumOfPara;			//换页时传递的参数个数
	u8  mNumOfAnimationActions; //用户自定义动画数量
	
	WidgetClassPtr pWidgetList;      //WidgetList是一个指向数组的指针，里面放的是WidgetList的地址	
	CanvasClassPtr pCanvasList;      //CanvasList是一个指向数组的指针，里面放的是CanvasList的地址
	SubCanvasClassPtr pSubCanvasList;//SubCanvasList是一个指向数组的指针，里面放的是SubCanvasList的地址
	AnimationActionClassPtr pAnimationActionList; //动画指令数组
	u8* pActionInstructions;         //指令队列
	u8* pActionStartADDR;            //PageActions的地址下标
	u8* pCanvasActionStartADDR;      //canvas指令起始地址
	touchWidgetRange *pTouchWidgetList; //可触碰控件的范围
	TextureClassPtr pBackgroundTexture;//背景纹理信息   BasicTexture* BackgroundTexture;	
	TextureClassPtr pBasicTextureList;//纹理结构体	 BasicTexture** BasicTextureList;
	TextureClassPtr pMouseTexture; //鼠标纹理信息	
	WidgetClassPtr  pMouseWidget; //鼠标控件
	
	matrixClass     mPageMatrix ; //页面矩阵
	

	//函数
	DynamicPageClass(void);
	~DynamicPageClass(void);

	//初始化页面
	funcStatus InitPage(
		void*          BassAddress             , //存放动态数据的基地址
		u32            StoreOffset            , //数据偏移 
		u16            PageAttr               , //page属性
		u16            NumOfWidget            , //widget的数量
		u8 			   NumOfCanvas			   , //Canvas数量
		u8             NumOfSubCanvas         , //subCanvas数量
		u16            ActionInstructionsSize , //总指令长度
		u16            TotalNumOfTexture	   , //纹理数量
		u16            NumofTouchWidget       , //可触碰纹理数量
		u16            NumOfPageAction        , //Action数量
		u16		       NumOfCanvasAction	  ,	 //CanvasAction数量
		u8             animationType          ,  //切换动画类型
		u8             curEnlightedWidgetID   ,  
		u8             animationFrame         ,  //动画帧数
		u8             numOfAnimationActions     //用户自定义动画数目
	);

	//加载页面
	funcStatus loadPage(u8 page, u8 systemInitial = 0);
	//加载页面并显示动画
	funcStatus loadPageWithAnimation(
		u8 curPageID                  , //当前页面
		u8 nextPageID                 , //下一页面
        u8 curFrame                   , //当前帧
		u8 totalFrame                 , //总共帧
		u8 animationType                //动画类型
		);
	//绘制页面
	funcStatus drawPage(
		TileBoxClassPtr tileBox,
		u32 *sourceShift       ,
		u8  pageEnable         ,
		u8 RefreshType         ,//正在绘制动画，取消控件的包围盒
		u8 staticTextureEn      //是否绘制到静态存储空间
		);
	//绘制动画页面
	funcStatus drawPageWithAnimation(
		DynamicPageClass* pCurPage  , //当前页面指针
		TileBoxClassPtr tileBox       ,
		u32* pSourceShift           ,      //sourcebuffer偏移
		u8  staticTextureEn 
		);
	//更新包围盒
	funcStatus refreshWidgetBox(
	    WidgetClassPtr pWidget,
		CanvasClassPtr pCanvas,
		TextureClassPtr pTexture,
		u8 textureEnable,
		u8 widgetEnable,
		u8 canvasEnable,
		u8 doubleBufferEnable,
		TileBoxClassPtr pTileBox
	);
	//更新示波器包围盒
	funcStatus RefreshOSCWidgetBox(
		WidgetClassPtr pWidget,
		TileBoxClassPtr pTileBox
	);
	//获取控件
	WidgetClassPtr GetWidgetList(u8 Value);
	//获取背景纹理
	TextureClassPtr GetBackgroundTexture();
	//初始化刷新队列
	funcStatus initHighlightWidget(u8 systemInitial);
	//动画tag发生改变
	funcStatus setATag(u8 value);
	//computing transition function
	funcStatus inoutTransitioin(
		u16 in,   //1.11.4
		u16 &out  //1.11.4
		);

	//writing static buffer background
	funcStatus writeBackGround(
		u32 *psourceshift, 
		matrixClassPtr addtionalMatrix
		);

	//writing a black background to the page
	funcStatus clearPage(
		u32 *psourceshift
		);

};

typedef DynamicPageClass* DynamicPageClassPtr;

#endif

#endif

