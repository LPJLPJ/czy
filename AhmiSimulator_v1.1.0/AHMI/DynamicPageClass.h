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
	//����
	u32 mStoreOffset;//ҳ��λ�ڴ洢�ļ���ƫ����
	u32 mTotalSize;  // DynamicPage��̬���ݵ��ܴ�С
	u16 mPageAttr;  //Page���ԣ�Ԥ�� 
	u16 mTotalNumOfTexture;//��ҳ��������������������������
	u16 mNumOfWidget;//��ǰҳ�����widget����
	u16 mActionInstructionsSize;//ָ������ܳ���
	u16 mNumOfPageAction;//PageAction����
	u16 mNumOfCanvasAction;//canvasAction����
	u16 mNumofTouchWidget;//�ɴ����ؼ�����
	u8  mNumOfCanvas;//��ǰҳ�����canvas����
	u8  mNumOfSubCanvas;//��ǰҳ������ӻ�������
	u8  mAnimationType   ;//��������
	u8  mtotoalFrame     ;//��֡��
	u8  mATag;            //animation tag
	u8  curEnlightedWidgetID; //��ǰ������widgetID
	u8  curEnlightedWidgetLighted; //��ǰ�����ؼ��Ƿ񱣳ָ���
	u8	mStartOfStack;    //��ҳʱ���ݵĲ�����ջ�е�λ��
	u8	mNumOfPara;			//��ҳʱ���ݵĲ�������
	u8  mNumOfAnimationActions; //�û��Զ��嶯������
	
	WidgetClassPtr pWidgetList;      //WidgetList��һ��ָ�������ָ�룬����ŵ���WidgetList�ĵ�ַ	
	CanvasClassPtr pCanvasList;      //CanvasList��һ��ָ�������ָ�룬����ŵ���CanvasList�ĵ�ַ
	SubCanvasClassPtr pSubCanvasList;//SubCanvasList��һ��ָ�������ָ�룬����ŵ���SubCanvasList�ĵ�ַ
	AnimationActionClassPtr pAnimationActionList; //����ָ������
	u8* pActionInstructions;         //ָ�����
	u8* pActionStartADDR;            //PageActions�ĵ�ַ�±�
	u8* pCanvasActionStartADDR;      //canvasָ����ʼ��ַ
	touchWidgetRange *pTouchWidgetList; //�ɴ����ؼ��ķ�Χ
	TextureClassPtr pBackgroundTexture;//����������Ϣ   BasicTexture* BackgroundTexture;	
	TextureClassPtr pBasicTextureList;//����ṹ��	 BasicTexture** BasicTextureList;
	TextureClassPtr pMouseTexture; //���������Ϣ	
	WidgetClassPtr  pMouseWidget; //���ؼ�
	
	matrixClass     mPageMatrix ; //ҳ�����
	

	//����
	DynamicPageClass(void);
	~DynamicPageClass(void);

	//��ʼ��ҳ��
	funcStatus InitPage(
		void*          BassAddress             , //��Ŷ�̬���ݵĻ���ַ
		u32            StoreOffset            , //����ƫ�� 
		u16            PageAttr               , //page����
		u16            NumOfWidget            , //widget������
		u8 			   NumOfCanvas			   , //Canvas����
		u8             NumOfSubCanvas         , //subCanvas����
		u16            ActionInstructionsSize , //��ָ���
		u16            TotalNumOfTexture	   , //��������
		u16            NumofTouchWidget       , //�ɴ�����������
		u16            NumOfPageAction        , //Action����
		u16		       NumOfCanvasAction	  ,	 //CanvasAction����
		u8             animationType          ,  //�л���������
		u8             curEnlightedWidgetID   ,  
		u8             animationFrame         ,  //����֡��
		u8             numOfAnimationActions     //�û��Զ��嶯����Ŀ
	);

	//����ҳ��
	funcStatus loadPage(u8 page, u8 systemInitial = 0);
	//����ҳ�沢��ʾ����
	funcStatus loadPageWithAnimation(
		u8 curPageID                  , //��ǰҳ��
		u8 nextPageID                 , //��һҳ��
        u8 curFrame                   , //��ǰ֡
		u8 totalFrame                 , //�ܹ�֡
		u8 animationType                //��������
		);
	//����ҳ��
	funcStatus drawPage(
		TileBoxClassPtr tileBox,
		u32 *sourceShift       ,
		u8  pageEnable         ,
		u8 RefreshType         ,//���ڻ��ƶ�����ȡ���ؼ��İ�Χ��
		u8 staticTextureEn      //�Ƿ���Ƶ���̬�洢�ռ�
		);
	//���ƶ���ҳ��
	funcStatus drawPageWithAnimation(
		DynamicPageClass* pCurPage  , //��ǰҳ��ָ��
		TileBoxClassPtr tileBox       ,
		u32* pSourceShift           ,      //sourcebufferƫ��
		u8  staticTextureEn 
		);
	//���°�Χ��
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
	//����ʾ������Χ��
	funcStatus RefreshOSCWidgetBox(
		WidgetClassPtr pWidget,
		TileBoxClassPtr pTileBox
	);
	//��ȡ�ؼ�
	WidgetClassPtr GetWidgetList(u8 Value);
	//��ȡ��������
	TextureClassPtr GetBackgroundTexture();
	//��ʼ��ˢ�¶���
	funcStatus initHighlightWidget(u8 systemInitial);
	//����tag�����ı�
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

