////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     TagClass.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.01 - modify the tagSetValue function by 程泽雨 
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include "publicDefine.h"
#include "AHMIBasicStructure.h"
#include "ButtonClass.h"
#include "ClockClass.h"
#include "KnobClass.h"
#include "MeterClass.h"
#include "NumberClass.h"
#include "OscilloscopeClass.h"
#include "ProgBarClass.h"
#include "SliderClass.h"
#include "TextClass.h"
#include "VideoClass.h"
#include "OnOffTexture.h"
#include "DynamicTexClass.h"
#include "Keyboard.h"
#include "mbconfig.h"
#include "user_mb_app.h"
#ifdef AHMI_DEBUG
#include "trace.h"
#include <iostream>
#endif

#ifdef AHMI_CORE 

extern ConfigInfoClass  ConfigData;
extern TimerClassPtr  TimerPtr;
extern u16             WorkingPageID;
extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u8*     TagChangedListPtr;

extern QueueHandle_t   ModbusMasterRWTaskQueue;
extern QueueHandle_t   ActionInstructionQueue;
extern ActionProcessorClass gAction;
#ifdef USE_MODBUS_MASTER
extern unsigned int    usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];
#endif
TagClass::TagClass(void)
{}

TagClass::~TagClass(void)
{}

//-----------------------------
// 函数名： InitTag
// 初始化tag
// 参数列表：
// @param1	TagClassPtr Ptr,                             tag指针，输入
// @param2	char* name,                                  tag名字
// @param3	u16 type,                                    tag类型 TAGTYPE_INT整数，TAGTYPE_STR字符串
// @param4	u32 initValue,								 初始值
// @param6	WidgetLinkDataPtr mWidgetLinkDataPtr,		 链接的widget
// @param7	u16 mNumOfDynamicPageAction,				 包含的page的命令数目
// @param8	PageLinkDataPtr mPageLinkDataPtr,			 链接的page
// @param9	u16 mNumOfCanvasAction,						 包含的canvas的命令数目
// @param10	CanvasLinkDataPtr mCanvasLinkDataPtr,		 链接的canvas
// @param11	u8 mLinkTimerID,							 链接的timer的编号
// @param12	u8 mLinkTimerVariableID,					 链接的timer的变量的ID号
// @param13	u16 tagNeedUpdate,							 是否通过modbus读写数据
// @param14	u16 tagRW,									 读写标示， TAGREAD读，TAGWRITE写
// @param15	u16 regID									 modbus的寄存器号
// 备注(各个版本之间的修改):							  
//    简单地对输入tag指针进行赋值
//    20160114加入tag刷新时间。 by Arthas
//-----------------------------
void TagClass::initTag(
	u8 NumOfWidgetLinker,
	u8 NumOfCanvasAction,
	u8 NumOfDynamicPageAction,
	u8  tagID,
	u16 Type,
	u32 RegID,
	u32 InitValue,
	
	WidgetLinkDataPtr WidgetLinkDataPtr,
	PageLinkDataPtr PageLinkDataPtr,
	CanvasLinkDataPtr CanvasLinkDataPtr
)
{
	u8 i;
	u8 linkElementCount = 0;
	u32 size;
	
	this->mTagType					= (u8)Type                           ;
	this->mRegID                    = RegID;
	this->mValue                    = InitValue                      ;
	this->mNumOfElementLinker       = (u8)(NumOfWidgetLinker + NumOfCanvasAction + NumOfDynamicPageAction);
//	this->mLinkTimer                = (u8)(-1); // no timer

	if(tagID == SYSTEM_KEYBOARD_TAG) //键盘tag
	{
		this->mNumOfElementLinker = 1;
		size = sizeof(struct TagElementLinkData) * (this->mNumOfElementLinker);
		this->pTagELmementLinker = (ElemenLinkDataPtr) pvPortMalloc( size );
		this->pTagELmementLinker[0].mElementType = ELEMENT_TYPE_KEYBOARD;
		this->pTagELmementLinker[0].mLinkElementPtr = NULL;
		return;
	}
	else if(tagID == SYSTEM_PAGE_TAG)
	{
		this->mNumOfElementLinker = 1;
		size = sizeof(struct TagElementLinkData) * (this->mNumOfElementLinker);
		this->pTagELmementLinker = (ElemenLinkDataPtr) pvPortMalloc( size );
		this->pTagELmementLinker[0].mElementType = ELEMENT_TYPE_PAGE_GOTO;
		this->pTagELmementLinker[0].mLinkElementPtr = NULL;
		return;
	}

	if(this->mNumOfElementLinker != 0)
	{
		size = sizeof(struct TagElementLinkData) * (this->mNumOfElementLinker);
		this->pTagELmementLinker = (ElemenLinkDataPtr) pvPortMalloc( size );
		if(this->pTagELmementLinker == NULL) 
			{
	#ifdef AHMI_DEBUG
				ERROR_PRINT("error in initializing tag.\r\n");
	#endif
				return;

			}
	
		//widget link
		for(i = 0; i < NumOfWidgetLinker; i++)
		{
			this->pTagELmementLinker[linkElementCount].mElementType = ELEMENT_TYPE_WIDGET;
			this->pTagELmementLinker[linkElementCount].mLinkElementPtr = (void *)&(WidgetLinkDataPtr[i]);
			linkElementCount++;
		}
	
		//canvas link
		for(i = 0; i < NumOfCanvasAction; i++)
		{
			this->pTagELmementLinker[linkElementCount].mElementType = ELEMENT_TYPE_CANVAS;
			this->pTagELmementLinker[linkElementCount].mLinkElementPtr = (void *)&(CanvasLinkDataPtr[i]);
			linkElementCount++;
		}
	
		//page link
		for(i = 0; i < NumOfDynamicPageAction; i++)
		{
			this->pTagELmementLinker[linkElementCount].mElementType = ELEMENT_TYPE_PAGE;
			this->pTagELmementLinker[linkElementCount].mLinkElementPtr = (void *)&(PageLinkDataPtr[i]);
			linkElementCount++;
		}
	}
	else 
		this->pTagELmementLinker = NULL;
}

//-----------------------------
// 函数名： TagGetValue
// 获取tag的数值
// 参数列表：
//    TagClassPtr Ptr
// 备注(各个版本之间的修改):
// 无
//-----------------------------
u32 TagClass::getValue() const
{
	return this->mValue;
}


//-----------------------------
// 函数名： TagSetValue
// 暂停其他任务
// 参数列表：
//   无
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void TagClass::setValue(u32 v, u16 tagID)				//待写
{
	//ModBusRefreshMsg mModBusRefresh;
	//ModBusMasterRWMsg mModBusRWMsg;
	//u8 tagNeedUpdate = this->mTagType & TAG_READ_REG;
#ifdef AHMI_DEBUG
	char text[100];
#endif
	if(tagID >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in tagSetValue: tagID out of baundary");
		return;
	}

	u8 u8_listID = tagID / 8;
	u8 u8_bindingID = tagID % 8;

	if(tagID != 0)
		mValue = v;
	else 
	{
		mValue = 0;
		return;
	}
	//if(this->mLinkTimer != (u8)(-1)) //links to timer
	//{
	//	TimerPtr[this->mLinkTimer].TimerSetValue(5,this->mValue);
	//}
#ifdef USE_MODBUS_MASTER
	usMRegHoldBuf[0][this->mRegID]=v;
#endif
#ifdef AHMI_DEBUG
	WriteText(text,"Set %d tag value %d.\r\n",  tagID, v);
	ERROR_PRINT(text);
#endif

	//setBindingElement();
	//set the tag changed list
	if(TagChangedListPtr != NULL)
		TagChangedListPtr[u8_listID] |= (1 << u8_bindingID);
	else 
	{
		ERROR_PRINT("ERROR in tagSetValue: TagChangedListPtr corruption");
		return;
	}

	//set timer
	//leave it to action

	//set modbus
#ifdef USE_MODBUS_MASTER
	//if(tagNeedUpdate)
	//{
	//	//更新此控件
	//	mModBusRWMsg.opCode = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
	//	mModBusRWMsg.numofReg = 1;
	//	mModBusRWMsg.regID = mRegID;
	//	mModBusRWMsg.value = mValue;
	//	xQueueSendToBack(ModbusMasterRWTaskQueue,&mModBusRWMsg,portMAX_DELAY);
	//}
	//leave it to action
#endif

}

//-----------------------------
// 函数名： TagSetString
// 将tag绑定到string
// 参数列表：
//   @param1 TagClassPtr Ptr  页面指针
//   @param2 char*       str  字符串首地址
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void TagClass::setString(char* str)
{
	int i;
	char *tagstr = (char*)mValue;

	if(( !TAG_IS_STR(mTagType) ) || (NULL == str)) 
		return;

	for(i=0;i!= ConfigData.TagStringLength ;i++)
	{
		if(str[i] != (char)NULL) tagstr[i] = str[i];
		else break;
	}

	this->setBindingElement();
}

//-----------------------------
// 函数名： TagSetBindingElement
// 将tag绑定到指定的widget
// 参数列表：
//   @param1 TagClassPtr Ptr  页面指针
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void TagClass::setBindingElement()				//待写
{
	u16 id;
	WidgetClassPtr pLinkedWidgetPtr;
	CanvasClassPtr pLinkedCanvas;
	SubCanvasClassPtr pFocusedSubcanvasPtr;
	ElemenLinkDataPtr pElementLinker;
	WidgetLinkDataPtr pWidgetLinker;
	CanvasLinkDataPtr pCanvasLinker;
	PageLinkDataPtr   pPageLinker  ;
	ActionTriggerClass gWidgetTagTrigger;
	u8 u8_widgetRefresh;
#ifdef AHMI_DEBUG
	char text[100];
#endif
	for(id=0;id<this->mNumOfElementLinker;id++)// 指向连接的Widget
	{
		pElementLinker = &(this->pTagELmementLinker[id]);
		if(pElementLinker->mElementType == ELEMENT_TYPE_WIDGET) //控件
		{
			pWidgetLinker = (WidgetLinkDataPtr)(pElementLinker->mLinkElementPtr);
			if(pWidgetLinker->LinkWidgetPageID == WorkingPageID) 
			{
				gWidgetTagTrigger.mInputType = ACTION_TAG_SET_VALUE;
				gWidgetTagTrigger.mTagPtr = this;
				//判断widget是否在focus的subcanvas上
				pLinkedWidgetPtr = &(gPagePtr[WorkingPageID].pWidgetList[pWidgetLinker->LinkWidgetID]);   //widget
				pLinkedCanvas = &(gPagePtr[WorkingPageID].pCanvasList[pLinkedWidgetPtr->ATTATCH_CANVAS]); //widget指向的canvas
				pFocusedSubcanvasPtr = &(gPagePtr[WorkingPageID].pSubCanvasList[(pLinkedCanvas->mStartAddrOfSubCanvas) + (pLinkedCanvas->mFocusedCanvas)]);  //canvas指向的subcanvas
				if(NULL == pFocusedSubcanvasPtr)
					return ;
				if( (pWidgetLinker->LinkWidgetID) >= (pFocusedSubcanvasPtr->StartAddrOfWidget) && ((pWidgetLinker->LinkWidgetID) < (pFocusedSubcanvasPtr->StartAddrOfWidget + pFocusedSubcanvasPtr->NumOfWidget) ) )//在subcanvas的widget范围内
				u8_widgetRefresh = 1;//那么需要刷新
			else
				u8_widgetRefresh = 0;//否则不需要刷新
 				gWidgetTagTrigger.widgetActionTrigger(pLinkedWidgetPtr,u8_widgetRefresh);

			}
		} //end of widget linker
		//改变链接的子画布
		else if(pElementLinker->mElementType == ELEMENT_TYPE_CANVAS) //CANVAS
		{
			pCanvasLinker = (CanvasLinkDataPtr)(pElementLinker->mLinkElementPtr);
			if(pCanvasLinker->LinkCanvasPageID == WorkingPageID)
			{
				gWidgetTagTrigger.mInputType = ACTION_TAG_SET_VALUE;
				gWidgetTagTrigger.mTagPtr = this;
				pLinkedCanvas = &gPagePtr[WorkingPageID].pCanvasList[pCanvasLinker->LinkCanvasID];
				CanvasActionTrigger(pLinkedCanvas, &gWidgetTagTrigger);
			}
		}//end of canvas linker
		//改变链接的page action
		else if(pElementLinker->mElementType == ELEMENT_TYPE_PAGE) //page
		{
			pPageLinker = (PageLinkDataPtr)(pElementLinker->mLinkElementPtr);
			if(pPageLinker->LinkDynamicPageID == WorkingPageID)
			{
				PageActionTrigger(&gPagePtr[WorkingPageID],pPageLinker->LinkActionIDInDynamicPage);
			}
		}// end of page linker
		else if(pElementLinker->mElementType == ELEMENT_TYPE_KEYBOARD)
		{
			if(mValue == AHMI_KEYBOARD_LEFT || mValue == AHMI_KEYBOARD_UP)
			{
#ifdef AHMI_DEBUG
				WriteText(text,"triggering keyboard left.\r\n");
				ERROR_PRINT(text);
#endif
				keyboardLeft();
			}
			else if(mValue == AHMI_KEYBOARD_RIGHT || mValue == AHMI_KEYBOARD_DOWN)
			{
#ifdef AHMI_DEBUG
				WriteText(text,"triggering keyboard right.\r\n");
				ERROR_PRINT(text);
#endif
				keyboardRight();
			}	
			else if(mValue == AHMI_KEYBOARD_OK)
			{
#ifdef AHMI_DEBUG
				WriteText(text,"triggering keyboard OK.\r\n");
				ERROR_PRINT(text);
#endif
				keyboardEntered();
			}
			else if(mValue == AHMI_KEYBOARD_CLEAR)
			{
#ifdef AHMI_DEBUG
				WriteText(text,"triggering keyboard Clear.\r\n");
				ERROR_PRINT(text);
#endif
				keyboardClear();
			}
			else 
			{
				ERROR_PRINT("ERROR in setting keyboard tag: connot find the keyboard value");
				return;
			}
			return;
		}// end of keyboard linker
		else if(pElementLinker->mElementType == ELEMENT_TYPE_PAGE_GOTO)// GO TO page tag
		{
			if(mValue >= 1 && mValue <= ConfigData.NumofPages)
			{
				gAction.GotoPage(0,0,this->mValue - 1, 0, 0);
			}
			else 
				ERROR_PRINT("ERROR when setting page tag binding element: connot goto this page");
			return;
		}
	} // end of linker
	

}




//-----------------------------
// 函数名： PageActionTrigger
// 触发页面指令
// 参数列表：
// @param1 DynamicPageClassPtr PagePtr page指针
// @param2 u16 TriggerAction 指令地址 
// 备注(各个版本之间的修改):
//    
//-----------------------------
funcStatus TagClass::PageActionTrigger(
		DynamicPageClassPtr Ptr,
		u16 TriggerAction
		)
{
	u32 addr;
	addr = (u32)(Ptr->pActionInstructions + Ptr->pActionStartADDR[TriggerAction]);

	xQueueSendToBack(ActionInstructionQueue,&addr,portMAX_DELAY);
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： CanvasActionTrigger
// 根据PagePtr的信息初始化Canvas
// 参数列表：
// @param1 DynamicPagePtr PagePtr page指针
// 备注(各个版本之间的修改):
//    暂时不需要初始化
//-----------------------------
funcStatus TagClass::CanvasActionTrigger(
		CanvasClassPtr mCanvas,
		ActionTriggerClassPtr canvasTagTrigger
		)
{
	//canvas的tag改变则切换subcanvas
	u32 value;
	if(canvasTagTrigger->mInputType == ACTION_TAG_SET_VALUE)
	{
		value = canvasTagTrigger->mTagPtr->mValue;
		if(value < mCanvas->mNumOfSubCanvas)
			return mCanvas->focusNewSubCanvas((u8)value);
		else 
			ERROR_PRINT("ERROR in CanvasActionTrigger: connot goto current canvas");
	}
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： bindTimer
// 根据PagePtr的信息初始化Canvas
// 参数列表：
// @param1 u16 timerID //timer标号
// 备注(各个版本之间的修改):
//    暂时不需要初始化
//-----------------------------
//funcStatus TagClass::bindTimer(
//		u16 timerID
//		)
//{
//	//this->mLinkTimer = (u8)(timerID);
//	return AHMI_FUNC_SUCCESS;
//}

#endif
