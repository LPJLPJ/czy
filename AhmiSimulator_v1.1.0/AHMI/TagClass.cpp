////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     TagClass.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.01 - modify the tagSetValue function by ������ 
// Revision 2.04 - C++, 20160321 by �ڴ�Ӫ
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
// �������� InitTag
// ��ʼ��tag
// �����б�
// @param1	TagClassPtr Ptr,                             tagָ�룬����
// @param2	char* name,                                  tag����
// @param3	u16 type,                                    tag���� TAGTYPE_INT������TAGTYPE_STR�ַ���
// @param4	u32 initValue,								 ��ʼֵ
// @param6	WidgetLinkDataPtr mWidgetLinkDataPtr,		 ���ӵ�widget
// @param7	u16 mNumOfDynamicPageAction,				 ������page��������Ŀ
// @param8	PageLinkDataPtr mPageLinkDataPtr,			 ���ӵ�page
// @param9	u16 mNumOfCanvasAction,						 ������canvas��������Ŀ
// @param10	CanvasLinkDataPtr mCanvasLinkDataPtr,		 ���ӵ�canvas
// @param11	u8 mLinkTimerID,							 ���ӵ�timer�ı��
// @param12	u8 mLinkTimerVariableID,					 ���ӵ�timer�ı�����ID��
// @param13	u16 tagNeedUpdate,							 �Ƿ�ͨ��modbus��д����
// @param14	u16 tagRW,									 ��д��ʾ�� TAGREAD����TAGWRITEд
// @param15	u16 regID									 modbus�ļĴ�����
// ��ע(�����汾֮����޸�):							  
//    �򵥵ض�����tagָ����и�ֵ
//    20160114����tagˢ��ʱ�䡣 by Arthas
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

	if(tagID == SYSTEM_KEYBOARD_TAG) //����tag
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
// �������� TagGetValue
// ��ȡtag����ֵ
// �����б�
//    TagClassPtr Ptr
// ��ע(�����汾֮����޸�):
// ��
//-----------------------------
u32 TagClass::getValue() const
{
	return this->mValue;
}


//-----------------------------
// �������� TagSetValue
// ��ͣ��������
// �����б�
//   ��
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
void TagClass::setValue(u32 v, u16 tagID)				//��д
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
	//	//���´˿ؼ�
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
// �������� TagSetString
// ��tag�󶨵�string
// �����б�
//   @param1 TagClassPtr Ptr  ҳ��ָ��
//   @param2 char*       str  �ַ����׵�ַ
// ��ע(�����汾֮����޸�):
//   ��
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
// �������� TagSetBindingElement
// ��tag�󶨵�ָ����widget
// �����б�
//   @param1 TagClassPtr Ptr  ҳ��ָ��
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
void TagClass::setBindingElement()				//��д
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
	for(id=0;id<this->mNumOfElementLinker;id++)// ָ�����ӵ�Widget
	{
		pElementLinker = &(this->pTagELmementLinker[id]);
		if(pElementLinker->mElementType == ELEMENT_TYPE_WIDGET) //�ؼ�
		{
			pWidgetLinker = (WidgetLinkDataPtr)(pElementLinker->mLinkElementPtr);
			if(pWidgetLinker->LinkWidgetPageID == WorkingPageID) 
			{
				gWidgetTagTrigger.mInputType = ACTION_TAG_SET_VALUE;
				gWidgetTagTrigger.mTagPtr = this;
				//�ж�widget�Ƿ���focus��subcanvas��
				pLinkedWidgetPtr = &(gPagePtr[WorkingPageID].pWidgetList[pWidgetLinker->LinkWidgetID]);   //widget
				pLinkedCanvas = &(gPagePtr[WorkingPageID].pCanvasList[pLinkedWidgetPtr->ATTATCH_CANVAS]); //widgetָ���canvas
				pFocusedSubcanvasPtr = &(gPagePtr[WorkingPageID].pSubCanvasList[(pLinkedCanvas->mStartAddrOfSubCanvas) + (pLinkedCanvas->mFocusedCanvas)]);  //canvasָ���subcanvas
				if(NULL == pFocusedSubcanvasPtr)
					return ;
				if( (pWidgetLinker->LinkWidgetID) >= (pFocusedSubcanvasPtr->StartAddrOfWidget) && ((pWidgetLinker->LinkWidgetID) < (pFocusedSubcanvasPtr->StartAddrOfWidget + pFocusedSubcanvasPtr->NumOfWidget) ) )//��subcanvas��widget��Χ��
				u8_widgetRefresh = 1;//��ô��Ҫˢ��
			else
				u8_widgetRefresh = 0;//������Ҫˢ��
 				gWidgetTagTrigger.widgetActionTrigger(pLinkedWidgetPtr,u8_widgetRefresh);

			}
		} //end of widget linker
		//�ı����ӵ��ӻ���
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
		//�ı����ӵ�page action
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
// �������� PageActionTrigger
// ����ҳ��ָ��
// �����б�
// @param1 DynamicPageClassPtr PagePtr pageָ��
// @param2 u16 TriggerAction ָ���ַ 
// ��ע(�����汾֮����޸�):
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
// �������� CanvasActionTrigger
// ����PagePtr����Ϣ��ʼ��Canvas
// �����б�
// @param1 DynamicPagePtr PagePtr pageָ��
// ��ע(�����汾֮����޸�):
//    ��ʱ����Ҫ��ʼ��
//-----------------------------
funcStatus TagClass::CanvasActionTrigger(
		CanvasClassPtr mCanvas,
		ActionTriggerClassPtr canvasTagTrigger
		)
{
	//canvas��tag�ı����л�subcanvas
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
// �������� bindTimer
// ����PagePtr����Ϣ��ʼ��Canvas
// �����б�
// @param1 u16 timerID //timer���
// ��ע(�����汾֮����޸�):
//    ��ʱ����Ҫ��ʼ��
//-----------------------------
//funcStatus TagClass::bindTimer(
//		u16 timerID
//		)
//{
//	//this->mLinkTimer = (u8)(timerID);
//	return AHMI_FUNC_SUCCESS;
//}

#endif
