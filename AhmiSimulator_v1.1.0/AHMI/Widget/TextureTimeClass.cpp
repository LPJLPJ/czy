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
#include "TextureTimeClass.h"
#include "publicType.h"
#include "drawImmediately_cd.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;

extern u32 startOfDynamicPage;
extern u32 endOfDynamicPage;
extern u8 animationDuration;
extern u8 animationExist;

//-----------------------------
// 函数名： TextureTimeClass
// 构造函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
TextureTimeClass::TextureTimeClass()
{

}

//-----------------------------
// 函数名： ~TextureTimeClass
// 析构函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
TextureTimeClass::~TextureTimeClass()
{

}


//-----------------------------
// 函数名： TextureTimeClass::initWidget
// 初始化并绘制图层数字框，如果是全屏刷新，先根据所绑定的tag刷新该控件，在绘制该控件
// 参数列表：
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //页面刷新
//  @param   TileBoxClassPtr pTileBox //包围盒
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus TextureTimeClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
	)
{
	TagClassPtr bindTag;
	ActionTriggerClass tagtrigger;
	WidgetClassInterface myWidgetClassInterface;

	if((NULL == p_wptr) || (NULL == u32p_sourceShift) || (NULL == pTileBox))
		return AHMI_FUNC_FAILURE;

	bindTag = &TagPtr[p_wptr->BindTagID];

	if(u8_pageRefresh)
	{
		tagtrigger.mTagPtr = bindTag;
		tagtrigger.mInputType = ACTION_TAG_SET_VALUE;
		if(widgetCtrl(p_wptr,&tagtrigger,1) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}

	if(myWidgetClassInterface.drawTexture(p_wptr,u32p_sourceShift,RefreshType,pTileBox,staticTextureEn) == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;
	return AHMI_FUNC_SUCCESS;
}

//***********************************
//图层时间控件
//显示N个字符，需要N张纹理。
//--------------------------------------------------------
//|数字 |0	1	2	3	4	5	6	7	8	9	-	/	:
//|序号	|0	1	2	3	4	5	6	7	8	9	10	11	12
//---------------------------------------------------------
//**********************************
funcStatus TextureTimeClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//页面刷新
	)
{
	u32 value;
	u32 divValue;
	u32 actionAddr;
	u16 code = 0;
	u8 displayMode;
	//	u8 number[NUMOFNUMBER];
	u8 modifyEn; //1表示正在修改
	u8 i;
	u8 numOfChar;
	//	u8 numOfNum;
	u8 hour, minute, second;
	u16 year;
	u16 highlightWidth, highlightHeight;
	u8 month, day;
	TextureClassPtr texturePtr;
	//	AnimationMsg animationMsg;
	//	RefreshMsg refreshMsg;
	if(NULL == p_wptr || NULL == ActionPtr || NULL == ActionPtr->mTagPtr){
		ERROR_PRINT("ERROR: widgetPtr or ActionPtr ActionPtr->mTagPtris NULL"); 
		return AHMI_FUNC_FAILURE;
	}
	u8 curHighlightNum = (u8)(p_wptr->NumOfButtonAndCurHighLight); 

	if( NULL == gPagePtr->pBasicTextureList){
		ERROR_PRINT("ERROR: when drawing clock widght, the texture list corrupt");
		return AHMI_FUNC_FAILURE;
	}

	if( (s16)(p_wptr->WidgetOffsetX) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetX) < -MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) < -MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetWidth > MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetHeight > MAX_WIDTH_AND_HEIGHT || 
		p_wptr->WidgetWidth == 0 ||  
		p_wptr->WidgetHeight == 0)
	{
		ERROR_PRINT("ERROR: when drawing clock widght, the offset\\width\\height exceeds the boundary");
		return AHMI_FUNC_FAILURE;
	}

	displayMode = (p_wptr-> WidgetAttr >> 5) & 0x3;
	modifyEn = ( p_wptr-> WidgetAttr >> 7) & 0x01;
	texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
	gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].mTexAttr |= TEXTURE_USING_WIDGET_BOX;

	if(displayMode == CLOCK_HOUR_MINUTE_MODE) //时分
	{
		numOfChar = 5;
		//		numOfNum = 4;
	}
	else if(displayMode == CLOCK_HOUR_MINUTE_SECOND_MODE) //时分秒
	{
		//		numOfNum = 6;
		numOfChar = 8;
	}
	else //年月日
	{
		//		numOfNum = 8;
		numOfChar = 10;
	}

	if((NULL == p_wptr) || (NULL == ActionPtr))
		return AHMI_FUNC_FAILURE;

	if((texturePtr < (void*)startOfDynamicPage) || (texturePtr > (void*)endOfDynamicPage))
		return AHMI_FUNC_FAILURE;


	if(ActionPtr->mInputType == ACTION_TAG_SET_VALUE || u8_pageRefresh == 1)
	{
		value = ActionPtr->mTagPtr->mValue;

		if(displayMode == CLOCK_HOUR_MINUTE_MODE) //时分模式
		{
			for(i = 0; i < 4; i++)
			{
				if(i == 2) //需要显示':'
				{
					texturePtr->FocusedSlice = COLONSIGN;
					texturePtr++;
				}
				divValue = value >> (( (3 - i) << 2) ); 
				code = (divValue & 0xf)/*基础偏移量*/;
				texturePtr->FocusedSlice = code;
				texturePtr++;
			}
		}
		else if(displayMode == CLOCK_HOUR_MINUTE_SECOND_MODE)//时间显示模式
		{
			for(i = 0; i < 6; i++)
			{
				//共6个数字
				if(i == 2 || i == 4 )//需要显示'：'
				{
					texturePtr->FocusedSlice = COLONSIGN;
					texturePtr++;
				}
				divValue = value >> ( (5 - i) << 2); 
				code = (divValue & 0xf) /*基础偏移量*/;
				texturePtr->FocusedSlice = code;
				texturePtr++;
			}
		}
		else if(displayMode == CLOCK_MINUS_YEAR_MODE || displayMode == CLOCK_GANG_YEAR_MODE)//日期显示模式
		{
			for(i = 0; i < 8; i++)//一共要显示8个数字
			{
				if(i == 4 || i == 6 || i == 8)
				{
					if(displayMode == CLOCK_GANG_YEAR_MODE) //斜杠方式
						code = SLAHSIGN;
					else if(displayMode == CLOCK_MINUS_YEAR_MODE)
						code = DASHSIGN;
					texturePtr->FocusedSlice = code;
					texturePtr++;
				}
				divValue = value >> ( (7 - i) << 2); 
				code = (divValue & 0xf) /*基础偏移量*/;
				texturePtr->FocusedSlice = code;
				texturePtr++;
			}
		}
	}
	else if(ActionPtr->mInputType == ACTION_KEYBOARD_OK)
	{
		if(modifyEn == 0)//进入修改模式
		{
			modifyEn = 1;
			p_wptr-> WidgetAttr |= 0x80;
			return AHMI_FUNC_SUCCESS;
		}
		else if(modifyEn == 1) //发送修改数据,并进入非修改状态
		{
			modifyEn = 0;
			p_wptr-> WidgetAttr &= ~(0x80);
			if( (p_wptr->OnEnteredAction) < 65535)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->OnEnteredAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}
		}
	}
	else if(ActionPtr->mInputType == ACTION_KEYBOARD_PRE) //向左按钮
	{
		if(modifyEn == 0) //非修改模式
		{
			if(displayMode == CLOCK_HOUR_MINUTE_MODE) //时分模式
			{
				if(curHighlightNum ==0)
					curHighlightNum = 2;
				else
					curHighlightNum -= 1;
				if(curHighlightNum != 0) //光标没有挪走
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX = texturePtr[(curHighlightNum - 1) * 3].OffsetX;
					texturePtr[numOfChar].OffsetY = texturePtr[(curHighlightNum - 1) * 3].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[(curHighlightNum - 1) * 3 ].TexWidth) + ((texturePtr[(curHighlightNum - 1) * 3  + 1].OffsetX - texturePtr[(curHighlightNum - 1) * 3 ].OffsetX)>>4); // the width need to be 2 * fontwidth
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[(curHighlightNum - 1) * 3].TexHeight);
					highlightWidth = (texturePtr[(curHighlightNum - 1) * 3 ].TexWidth) + ((texturePtr[(curHighlightNum - 1) * 3  + 1].OffsetX - texturePtr[(curHighlightNum - 1) * 3 ].OffsetX)>>4);
					highlightHeight = (texturePtr[(curHighlightNum - 1) * 3].TexHeight);
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else //光标挪走
				{
					texturePtr[numOfChar + 1].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
					curHighlightNum = 0; //当前高亮变为0
					(p_wptr->NumOfButtonAndCurHighLight) = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
			}
			else if(displayMode == CLOCK_HOUR_MINUTE_SECOND_MODE)//时间显示模式
			{
				if(curHighlightNum == 0)
					curHighlightNum = 3;
				else 
					curHighlightNum -= 1;
				if(curHighlightNum != 0) //光标没有挪走
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX =    texturePtr[(curHighlightNum - 1) * 3 ].OffsetX;
					texturePtr[numOfChar ].OffsetY =    texturePtr[(curHighlightNum - 1) * 3 ].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[(curHighlightNum - 1) * 3 ].TexWidth) + ((texturePtr[(curHighlightNum - 1) * 3  + 1].OffsetX - texturePtr[(curHighlightNum - 1) * 3 ].OffsetX)>>4);	// the width need to be 2 * fontwidth																							  
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[(curHighlightNum - 1) * 3 ].TexHeight);
					highlightWidth = (texturePtr[(curHighlightNum - 1) * 3 ].TexWidth) + ((texturePtr[(curHighlightNum - 1) * 3  + 1].OffsetX - texturePtr[(curHighlightNum - 1) * 3 ].OffsetX)>>4);
					highlightHeight = (texturePtr[(curHighlightNum - 1) * 3 ].TexHeight);
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else //光标挪走
				{
					texturePtr[numOfChar].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
					curHighlightNum = 0; //当前高亮变为0
					(p_wptr->NumOfButtonAndCurHighLight) = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
			}
			else //日期模式
			{
				if(curHighlightNum == 0)
					curHighlightNum = 3;
				else 
					curHighlightNum -= 1;
				if(curHighlightNum == 1) //光标在年上面
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX =    texturePtr[0].OffsetX;
					texturePtr[numOfChar].OffsetY =    texturePtr[0].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[0].TexWidth) +  ((texturePtr[3].OffsetX -  texturePtr[0].OffsetX)>>4);
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[0].TexHeight);
					highlightWidth = (texturePtr[0].TexWidth) +  ((texturePtr[3].OffsetX -  texturePtr[0].OffsetX)>>4);
					highlightHeight = (texturePtr[0].TexHeight);
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[ numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else if(curHighlightNum == 2)//光标在月上面
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX =    texturePtr[5].OffsetX;
					texturePtr[numOfChar].OffsetY =    texturePtr[5].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[5].TexWidth) +  ((texturePtr[6].OffsetX -  texturePtr[5].OffsetX)>>4);
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[5].TexHeight);
					highlightWidth = (texturePtr[5].TexWidth) +  ((texturePtr[6].OffsetX -  texturePtr[5].OffsetX)>>4);
					highlightHeight = (texturePtr[5].TexHeight);
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else if(curHighlightNum == 3) //光标在日上面
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX =    texturePtr[8].OffsetX;
					texturePtr[numOfChar].OffsetY =    texturePtr[8].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[8].TexWidth) +  ((texturePtr[9].OffsetX -  texturePtr[8].OffsetX)>>4);
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[8].TexHeight);
					highlightWidth = (texturePtr[5].TexWidth) +  ((texturePtr[6].OffsetX -  texturePtr[5].OffsetX)>>4);
					highlightHeight = (texturePtr[5].TexHeight);
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else //光标挪走
				{
					texturePtr[numOfChar + 1].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
					curHighlightNum = 0; //当前高亮变为0
					(p_wptr->NumOfButtonAndCurHighLight) = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
			}
		}
		else if(modifyEn) //修改模式
		{
			if(displayMode == CLOCK_HOUR_MINUTE_MODE || displayMode == CLOCK_HOUR_MINUTE_SECOND_MODE) //时分模式
			{
				//获取时分秒
				value = TagPtr[p_wptr->BindTagID].mValue;
				hour = (u8)(value >> 16);
				minute = (u8)(value >> 8);
				second = (u8)(value);
				hour = (hour >> 4) * 10 + (hour & 0xf);
				minute = (minute >> 4) * 10 + (minute & 0xf);
				second = (second >> 4) * 10 + (second & 0xf);
				//修改时分秒
				if(curHighlightNum == 1)//减少当前小时
				{
					if(hour > 0)
					{
						hour--;
					}
					else if(hour == 0)
					{
						hour = 23;
					}
				}
				else if(curHighlightNum == 2) //减少当前分钟
				{
					if(minute > 0)
						minute --;
					else if(minute == 0)
					{
						minute = 59;
						if(hour > 0)
							hour--;
						else if(hour == 0)
							hour = 23;
					}
				}
				else if(curHighlightNum == 3) //减少秒钟
				{
					if(second > 0)
						second --;
					else if(second == 0)
					{
						second = 59;
						if(minute == 0 && hour == 0)
						{
							minute = 59;
							hour = 23;
						}
						else if(minute == 0)
						{
							minute = 59;
							hour --;
						}
						else
							minute --;
					}
				}
				//重新计算tag值
				hour = ((hour/10) << 4)  + (hour % 10);
				minute = ((minute/10) << 4)  + (minute % 10);
				second = ((second/10) << 4)  + (second % 10);
				value = (hour << 16) + (minute << 8) + second;
				if(displayMode == CLOCK_HOUR_MINUTE_MODE)
				{
					for(i = 0; i < 4; i++)
					{
						if(i == 2) //需要显示':'
						{
							code = COLONSIGN; 
							texturePtr->FocusedSlice = code;
							texturePtr++;
						}
						divValue = value >> (( (5 - i) << 2)); 
						code = (divValue & 0xf);
						texturePtr->FocusedSlice = code;
						texturePtr++;
					}
				}
				else if(displayMode == CLOCK_HOUR_MINUTE_SECOND_MODE)
				{
					for(i = 0; i < 6; i++)
					{
						//共6个数字
						if(i == 2 || i == 4 )//需要显示'：'
						{
							code = COLONSIGN; 
							texturePtr->FocusedSlice = code;
							texturePtr++;
						}
						divValue = value >> ( (5 - i) << 2); 
						code = (divValue & 0xf);
						texturePtr->FocusedSlice = code;
						texturePtr++;
					}
				}
			}
			else //年月日模式
			{
				//获取年月日
				value = TagPtr[p_wptr->BindTagID].mValue;
				year = (u16)(value >> 16);
				month = (u8)(value >> 8);
				day = (u8)(value);
				year = (year >> 12) * 1000 + ( (year >> 8) & 0xf) * 100 + (((year >> 4) & 0xf) * 10) + (year & 0xf);
				month = (month >> 4) * 10 + (month & 0xf);
				day = (day >> 4) * 10 + (day & 0xf);
				//修改年月日
				if(curHighlightNum == 1)//减少当年
				{
					if(year > 0)
					{
						//闰年需要额外判断
						if( (year % 4 == 0 && year % 100 != 0) || ( year % 400 == 0) )
						{
							if(month == 2 && day == 29)
							{
								month = 3;
								day = 1;
							}
						}
						year--;
					}
					else if(year == 0)
					{
						year = 9999;
					}
				}
				else if(curHighlightNum == 2) //减少当前月
				{
					if(month > 1)
					{
						switch (month)
						{
						case(5):
						case(7):
						case(8):
						case(10):
							if(day == 31)
							{
								//month += 2;
								day = 1;
							}
							else 
								month--;
							break;
						case(3): //从三月份变成二月份
							if( (year % 4 == 0 && year % 100 != 0) || ( year % 400 == 0) )
							{
								hour = 29;
							}
							else 
								hour = 28;
							if(day > hour)
							{
								month = 3;
								day = day - hour;
							}
							else
								month--;
							break;
						default:
							month --;
							break;
						}
					}
					else if(month == 1)
					{
						month = 12;
						if(year > 0)
						{
							year--;
						}
						else if(year == 0)
						{
							year = 9999;
						}
					}
				}
				else if(curHighlightNum == 3) //减少当前日
				{
					if(day > 1)
						day --;
					else if(day == 1)
					{
						if(month == 1 && year == 0)
						{
							year = 9999;
							month = 12;
							day = 31;
						}
						else if(month == 1)
						{
							year --;
							month = 12;
							day = 31;
						}
						else 
						{
							month --;
							switch( month )
							{
							case(1):
							case(3):
							case(5):
							case(7):
							case(8):
							case(10):
							case(12):
								hour = 31; //天数上限
								break;
							case(2):
								//先判断闰年
								if( (year % 4 == 0 && year % 100 != 0) || (year % 100 == 0 && year % 400 == 0) )
								{
									hour = 29;
								}
								else 
									hour = 28;
								break;
							default:
								hour = 30;
							}
							day = hour;
						}
					}
				}

				//重新计算tag值
				year = ((year/1000) << 12) + ( ((year/100) % 10) << 8) + ( ((year/10) % 10) << 4) + (year % 10);
				month = ((month/10) << 4)  + (month % 10);
				day = ((day/10) << 4)  + (day % 10);
				value = (year << 16) + (month << 8) + day;

				//重新计算纹理值
				for(i = 0; i < 8; i++)//一共要显示8个数字
				{
					if(i == 4 || i == 6 || i == 8)
					{
						if(displayMode == CLOCK_GANG_YEAR_MODE) //斜杠方式
							code = SLAHSIGN;
						else if(displayMode == CLOCK_MINUS_YEAR_MODE)
							code = DASHSIGN;
						texturePtr->FocusedSlice = code;
						texturePtr++;
					}
					divValue = value >> ( (7 - i) << 2); 
					code = (divValue & 0xf);
					texturePtr->FocusedSlice = code;
					texturePtr++;
				}
			}
#ifdef AHMI_DEBUG
			char text[100];
			sprintf(text,"setting clock to value %x", value);
			ERROR_PRINT(text);
#endif
			ActionPtr->mTagPtr->mValue = value;
		}
	}
	else if(ActionPtr->mInputType == ACTION_KEYBOARD_NEXT) //向右按钮
	{
		if(modifyEn == 0) //非修改模式
		{
			if(displayMode == CLOCK_HOUR_MINUTE_MODE)
			{
				if(curHighlightNum == 2)
					curHighlightNum = 0;
				else 
					curHighlightNum += 1;
				if(curHighlightNum != 0) //光标没有挪走
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX = texturePtr[(curHighlightNum - 1) * 3].OffsetX;
					texturePtr[numOfChar].OffsetY = texturePtr[(curHighlightNum - 1) * 3].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[(curHighlightNum - 1) * 3 ].TexWidth) + ((texturePtr[(curHighlightNum - 1) * 3  + 1].OffsetX - texturePtr[(curHighlightNum - 1) * 3 ].OffsetX)>>4);
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[(curHighlightNum - 1) * 3].TexHeight) ;
					highlightWidth = (texturePtr[(curHighlightNum - 1) * 3 ].TexWidth) + ((texturePtr[(curHighlightNum - 1) * 3  + 1].OffsetX - texturePtr[(curHighlightNum - 1) * 3 ].OffsetX)>>4);
					highlightHeight = (texturePtr[(curHighlightNum - 1) * 3].TexHeight);
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else //光标挪走
				{
					texturePtr[numOfChar + 1].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
					curHighlightNum = 0; //当前高亮变为0
					(p_wptr->NumOfButtonAndCurHighLight) = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
			}
			else if(displayMode == CLOCK_HOUR_MINUTE_SECOND_MODE)//时间显示模式
			{
				if(curHighlightNum == 3)
					curHighlightNum = 0;
				else 
					curHighlightNum += 1;
				if(curHighlightNum != 0) //光标没有挪走
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX =    texturePtr[(curHighlightNum - 1) * 3 ].OffsetX;
					texturePtr[numOfChar].OffsetY =    texturePtr[(curHighlightNum - 1) * 3 ].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[(curHighlightNum - 1) * 3 ].TexWidth) + ((texturePtr[(curHighlightNum - 1) * 3  + 1].OffsetX - texturePtr[(curHighlightNum - 1) * 3 ].OffsetX)>>4);																								  
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[(curHighlightNum - 1) * 3 ].TexHeight) ;
					highlightWidth = (texturePtr[(curHighlightNum - 1) * 3 ].TexWidth) + ((texturePtr[(curHighlightNum - 1) * 3  + 1].OffsetX - texturePtr[(curHighlightNum - 1) * 3 ].OffsetX)>>4);	
					highlightHeight = (texturePtr[(curHighlightNum - 1) * 3 ].TexHeight) ;
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else //光标挪走
				{
					texturePtr[numOfChar + 1].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
					curHighlightNum = 0; //当前高亮变为0
					(p_wptr->NumOfButtonAndCurHighLight) = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
			}
			else//日期模式
			{
				if(curHighlightNum == 3)
					curHighlightNum = 0;
				else 
					curHighlightNum += 1;
				if(curHighlightNum == 1) //光标在年上面
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX =    texturePtr[0].OffsetX;
					texturePtr[numOfChar].OffsetY =    texturePtr[0].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[0].TexWidth) +  ((texturePtr[3].OffsetX -  texturePtr[0].OffsetX)>>4);
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[0].TexHeight) ;
					highlightWidth = (texturePtr[0].TexWidth) +  ((texturePtr[3].OffsetX -  texturePtr[0].OffsetX)>>4);	
					highlightHeight = (texturePtr[0].TexHeight) ;
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else if(curHighlightNum == 2)//光标在月上面
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX =    texturePtr[5].OffsetX;
					texturePtr[numOfChar].OffsetY =    texturePtr[5].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[5].TexWidth) +  ((texturePtr[6].OffsetX -  texturePtr[5].OffsetX)>>4);
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[5].TexHeight) ;
					highlightWidth = (texturePtr[5].TexWidth) +  ((texturePtr[6].OffsetX -  texturePtr[5].OffsetX)>>4);
					highlightHeight = (texturePtr[5].TexHeight) ;
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else if(curHighlightNum == 3) //光标在日上面
				{
					texturePtr[numOfChar].mTexAttr |= (DRAWING); //颜色按钮显示
					texturePtr[numOfChar].OffsetX =    texturePtr[8].OffsetX;
					texturePtr[numOfChar].OffsetY =    texturePtr[8].OffsetY;
					//texturePtr[numOfChar + 1].TexWidth =  (texturePtr[8].TexWidth) +  ((texturePtr[9].OffsetX -  texturePtr[8].OffsetX)>>4);
					//texturePtr[numOfChar + 1].TexHeight = (texturePtr[8].TexHeight) ;
					highlightWidth = (texturePtr[8].TexWidth) +  ((texturePtr[9].OffsetX -  texturePtr[8].OffsetX)>>4);
					highlightHeight = (texturePtr[8].TexHeight) ;
					texturePtr[numOfChar].ScalerX = highlightWidth * 512 / texturePtr[numOfChar].TexWidth;
					texturePtr[numOfChar].ScalerY = highlightHeight * 512 / texturePtr[numOfChar].TexHeight;
					texturePtr[numOfChar].renewTextureSourceBox(NULL,NULL,NULL);
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
				else //光标挪走
				{
					texturePtr[numOfChar].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
					curHighlightNum = 0; //当前高亮变为0
					(p_wptr->NumOfButtonAndCurHighLight) = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
				}
			}
		}
		else if(modifyEn) //修改模式
		{
			//修改tag数值
			if(displayMode == CLOCK_HOUR_MINUTE_MODE || displayMode == CLOCK_HOUR_MINUTE_SECOND_MODE) //时分模式
			{
				//获取时分秒
				value = TagPtr[p_wptr->BindTagID].mValue;
				hour = (u8)(value >> 16);
				minute = (u8)(value >> 8);
				second = (u8)(value);
				hour = (hour >> 4) * 10 + (hour & 0xf);
				minute = (minute >> 4) * 10 + (minute & 0xf);
				second = (second >> 4) * 10 + (second & 0xf);
				//修改时分秒
				if(curHighlightNum == 1)//增加当前小时
				{
					if(hour < 23)
					{
						hour++;
					}
					else if(hour == 23)
					{
						hour = 0;
					}
				}
				else if(curHighlightNum == 2) //增加当前分钟
				{
					if(minute < 59)
						minute ++;
					else if(minute == 59)
					{
						minute = 0;
						if(hour < 23)
						{
							hour++;
						}
						else if(hour == 23)
						{
							hour = 0;
						}
					}
				}
				else if(curHighlightNum == 3) //增加秒钟
				{
					if(second < 59)
						second ++;
					else if(second == 59)
					{
						second = 0;
						if(minute == 59 && hour == 23)
						{
							minute = 0;
							hour = 0;
						}
						else if(minute == 59)
						{
							hour++;
							minute = 0;
						}
						else
							minute ++;
					}
				}
				//重新计算tag值
				hour = ((hour/10) << 4)  + (hour % 10);
				minute = ((minute/10) << 4)  + (minute % 10);
				second = ((second/10) << 4)  + (second % 10);
				value = (hour << 16) + (minute << 8) + second;
				if(displayMode == CLOCK_HOUR_MINUTE_MODE)
				{
					for(i = 0; i < 4; i++)
					{
						if(i == 2) //需要显示':'
						{
							code = COLONSIGN; 
							texturePtr->FocusedSlice = code;
							texturePtr++;
						}
#if 0
						divValue = value >> (( (3 - i) << 2) + 2); 
#else
						divValue = value >> ( (5 - i) << 2); 
#endif
						code = (divValue & 0xf) ;
						texturePtr->FocusedSlice = code;
						texturePtr++;
					}
				}
				else if(displayMode == CLOCK_HOUR_MINUTE_SECOND_MODE)
				{
					for(i = 0; i < 6; i++)
					{
						//共6个数字
						if(i == 2 || i == 4 )//需要显示'：'
						{
							code = COLONSIGN; 
							texturePtr->FocusedSlice = code;
							texturePtr++;
						}
						divValue = value >> ( (5 - i) << 2); 
						code = (divValue & 0xf) ;
						texturePtr->FocusedSlice = code;
						texturePtr++;
					}
				}
			}
			else //年月日模式
			{
				//获取年月日
				value = TagPtr[p_wptr->BindTagID].mValue;
				year = (u16)(value >> 16);
				month = (u8)(value >> 8);
				day = (u8)(value);
				year = (year >> 12) * 1000 + ((year >> 8) & 0xf) * 100 + (((year >> 4) & 0xf) * 10) + (year & 0xf);
				month = (month >> 4) * 10 + (month & 0xf);
				day = (day >> 4) * 10 + (day & 0xf);
				//修改年月日
				if(curHighlightNum == 1)//增加当年
				{
					if(year < 9999)
					{
						//闰年需要额外判断
						if( (year % 4 == 0 && year % 100 != 0) || ( year % 400 == 0) )
						{
							if(month == 2 && day == 29)
							{
								month = 3;
								day = 1;
							}
						}
						year++;
					}
					else if(year == 9999)
					{
						year = 0;
					}
				}
				else if(curHighlightNum == 2) //增加当前月
				{
					if(month < 12)
					{
						switch (month)
						{
						case(3):
						case(5):
						case(7):
						case(8):
						case(10):
							if(day == 31)
							{
								month += 2;
								day = 1;
							}
							else 
								month++;
							break;
						case(1): //从一月份变成二月份
							if( (year % 4 == 0 && year % 100 != 0) || ( year % 400 == 0) )
							{
								hour = 29;
							}
							else 
								hour = 28;
							if(day > hour)
							{
								month = 3;
								day = day - hour;
							}
							else 
								month ++;
							break;
						default:
							month ++;
							break;
						}

					}
					else if(month == 12)
					{
						month = 1;
						if(year < 9999)
						{
							year++;
						}
						else if(year == 9999)
						{
							year = 0;
						}
					}
				}
				else if(curHighlightNum == 3) //增加当前日
				{
					switch(month)
					{
					case(1):
					case(3):
					case(5):
					case(7):
					case(8):
					case(10):
					case(12):
						hour = 31; //天数上限
						break;
					case(2):
						//先判断闰年
						if( (year % 4 == 0 && year % 100 != 0) || (year % 100 == 0 && year % 400 == 0) )
						{
							hour = 29;
						}
						else 
							hour = 28;
						break;
					default:
						hour = 30;
					}
					if(day < hour)
						day ++;
					else if(day == hour)
					{
						day = 1;
						if(month == 12)
						{
							month = 1;
							if(year == 9999)
								year = 0;
							else 
								year ++;
						}
						else
							month ++;
					}
				}

				//重新计算tag值
				year = ((year/1000) << 12) + ( ((year/100) % 10) << 8) + ( ((year/10) % 10) << 4) + (year % 10);
				month = ((month/10) << 4)  + (month % 10);
				day = ((day/10) << 4)  + (day % 10);
				value = (year << 16) + (month << 8) + day;

				//重新计算纹理值
				for(i = 0; i < 8; i++)//一共要显示8个数字
				{
					if(i == 4 || i == 6 || i == 8)
					{
						if(displayMode == CLOCK_GANG_YEAR_MODE) //斜杠方式
							code = SLAHSIGN;
						else if(displayMode == CLOCK_MINUS_YEAR_MODE)
							code = DASHSIGN;
						texturePtr->FocusedSlice = code;
						texturePtr++;
					}
					divValue = value >> ( (7 - i) << 2); 
					code = (divValue & 0xf) ;
					texturePtr->FocusedSlice = code;
					texturePtr++;
				}
			}
#ifdef AHMI_DEBUG
			char text[100];
			sprintf(text,"setting clock to value %x", value);
			ERROR_PRINT(text);
#endif
			ActionPtr->mTagPtr->mValue = value;
		}
	}//end of keyboard next
	else if(ActionPtr->mInputType == ACTION_KEYBOARD_CLEAR)
	{
		texturePtr[numOfChar].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
		curHighlightNum = 0; //当前高亮变为0
		p_wptr-> WidgetAttr &= 0xff7f; //exit the modify mode
		(p_wptr->NumOfButtonAndCurHighLight) = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighlightNum;
	}

	if(u8_pageRefresh == 0)
	{
		//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
		refreshMsg.mElementPtr.wptr = p_wptr;
		sendToRefreshQueue(&refreshMsg);
#endif
		return AHMI_FUNC_SUCCESS;
	}
	return AHMI_FUNC_SUCCESS;
}


#endif
