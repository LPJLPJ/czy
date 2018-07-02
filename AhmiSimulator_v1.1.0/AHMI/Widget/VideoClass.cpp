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
#include "VideoClass.h"
#include "drawImmediately_cd.h"
#ifdef EMBEDDED
#include "ahmi3_function.h"
#endif

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
extern QueueHandle_t   RefreshQueueWithoutDoubleBuffer;
extern  u8 video_cur_w;           
extern  u8 video_cur_r ;   
extern  u8 video_next_r;


//-----------------------------
// 函数名： MeterClass
// 构造函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
VideoClass::VideoClass()
{

}

//-----------------------------
// 函数名： ~MeterClass
// 析构函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
VideoClass::~VideoClass()
{

}

//-----------------------------
// 函数名： VideoClass::initWidget
// 初始化并绘制该控件，如果是全屏刷新，先根据所绑定的tag刷新该控件，在绘制该控件
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //页面刷新
//  @param   TileBoxClassPtr pTileBox //包围盒
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus VideoClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
	)
{
	WidgetClassInterface myWidgetClassInterface;
	ActionTriggerClass tagtrigger;
	TagClassPtr bindTag;

	if((NULL == p_wptr) || (NULL == u32p_sourceShift) || (NULL == pTileBox))
		return AHMI_FUNC_FAILURE;

	//if(RefreshType == ANIMATION_REFRESH_TEXTURE)
	//	RefreshType = ANIMATION_REFRESH_NULL;
	bindTag = &TagPtr[p_wptr->BindTagID];
	//if(u8_pageRefresh)
	//{
		tagtrigger.mTagPtr = bindTag;
		tagtrigger.mInputType = ACTION_TAG_SET_VALUE;
		if(widgetCtrl(p_wptr,0,1) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	//}
	if(myWidgetClassInterface.drawTexture(p_wptr,u32p_sourceShift,RefreshType,pTileBox,staticTextureEn) == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;
	return AHMI_FUNC_SUCCESS;
}

//***********************************
//视频纹理控制函数
//显示采集到的视频
//WidgetAttr标识：
//15-5:保留
//4-0:控件类型，视频纹理为0xA
//**********************************
funcStatus VideoClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//控件指针
	u8 video_base_addr,             //视频控件所绘制的地址
	u8 u8_pageRefresh				//页面刷新
	)
{
	TextureClassPtr videoTexturePtr;// = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
	u8 videoType = (p_wptr->WidgetAttr & VIDEO_TYPE_HDMI) > 0 ? 1 : 0;
	u16 hdmi_width;
	u16 hdmi_height;

	if(NULL == p_wptr)
		return AHMI_FUNC_FAILURE;

	
	if(videoType) //hdmi
	//if(0) //hdmi
	{
		//get the video texture
		videoTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
		//read the video width and height from spi
#ifdef EMBEDDED 
		//readVideoWidthAndHeight(&hdmi_width, &hdmi_height);
		hdmi_width = 1280;
		hdmi_height = 480;
	
	  videoTexturePtr->TexWidth = hdmi_width;
	  videoTexturePtr->TexHeight = hdmi_height;
	  videoTexturePtr->ScalerX = p_wptr->WidgetWidth * 512 / videoTexturePtr->TexWidth;  //缩放量应该是widget的大小除以texture的大小
	  videoTexturePtr->ScalerY = p_wptr->WidgetHeight * 512 / videoTexturePtr->TexHeight; //缩放量应该是widget的大小除以texture的大小
	  videoTexturePtr->TexAddr = START_ADDR_OF_RAM;		
#endif
#ifdef PC_SIM
		hdmi_width = 720;
		hdmi_height = 576;
		videoTexturePtr->TexWidth = hdmi_width;
	    videoTexturePtr->TexHeight = hdmi_height;
		videoTexturePtr->ScalerX = p_wptr->WidgetWidth * 512 / videoTexturePtr->TexWidth;  //缩放量应该是widget的大小除以texture的大小
	    videoTexturePtr->ScalerY = p_wptr->WidgetHeight * 512 / videoTexturePtr->TexHeight; //缩放量应该是widget的大小除以texture的大小
		videoTexturePtr->TexAddr = START_ADDR_OF_RAM;	
#endif
	}
	else //cvbs
	{
		//get the video texture 
		videoTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 2]);
		gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].mTexAttr |= TEXTURE_USING_WIDGET_BOX;

		hdmi_width = 720;
		hdmi_height = 576;
		
		///videoTexturePtr->OffsetX = 200 * 16;//debug
		//videoTexturePtr->OffsetY = -200 * 16;//debug
		//videoTexturePtr->TexLeftTileBox = 0; //debug
		//videoTexturePtr->TexTopTileBox = 0; //debug
		//videoTexturePtr->TexRightTileBox = 40; //debug
		//videoTexturePtr->TexButtomTileBox = 15; //debug
		#ifdef EMBEDDED
		readVideoWidthAndHeight(&hdmi_width, &hdmi_height);
		
		if(hdmi_width == 0 || hdmi_height == 0)
		{
			hdmi_width = 720;
			hdmi_height = 576;
		}
		//hdmi_height = 550;
		videoTexturePtr->TexWidth = hdmi_width;
		videoTexturePtr->TexHeight = hdmi_height / 2 - 2;
		
		videoTexturePtr->ScalerX = p_wptr->WidgetWidth * 512 / hdmi_width;  //缩放量应该是widget的大小除以texture的大小
		videoTexturePtr->ScalerY = 2 * p_wptr->WidgetHeight * 512 / hdmi_height; //缩放量应该是widget的大小除以texture的大小
		if(video_cur_w == (video_cur_r + 2) % 3)
			video_next_r = (video_cur_r + 1) % 3;
		else 
			video_next_r = video_cur_r;
		videoTexturePtr->TexAddr = (START_ADDR_OF_RAM) + (START_ADDR_OF_CVBS) + video_next_r * (SIZE_OF_CVBS_BUFFER) + 2 * hdmi_width;
		#endif
#ifdef PC_SIM
		videoTexturePtr->TexWidth = hdmi_width;
		videoTexturePtr->TexHeight = hdmi_height;
		videoTexturePtr->TexAddr = (START_ADDR_OF_RAM);
#endif
	}



	
	//videoTexturePtr->TexAddr = (video_base_addr << 20);

	if(u8_pageRefresh == 0)
	{
		//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
		refreshMsg.mElementPtr.wptr = p_wptr;
		xQueueSendToBack(RefreshQueueWithoutDoubleBuffer,&refreshMsg,portMAX_DELAY);
#endif
	}
	return AHMI_FUNC_SUCCESS;
}

#endif
