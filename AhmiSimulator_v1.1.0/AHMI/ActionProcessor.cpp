////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     ActionProcessor.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.01 - Change MORE,LESS,MUL,DIV, from unsigned to int, 20151120 by XT
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include <stdio.h>
#include "animationDefine.h"
extern "C"
{
#include "mbproto.h"
}
#include "user_mb_app.h"

#ifdef AHMI_CORE

extern ConfigInfoClass      ConfigData;
extern TagClassPtr  TagPtr;
extern u16            WorkingPageID;
extern DynamicPageClassPtr  gPagePtr;
extern TagClassPtr     TagPtr;
extern TimerClassPtr   TimerPtr;
extern u8 staticRefresh;


extern xTaskHandle ModBusSlaveTaskHandle;
extern xTaskHandle ModBusMasterReadWeiteHandle;
extern xTaskHandle	ModBusMasterTaskHandle;
extern xTaskHandle MouseTouchTaskHandle;
extern xTaskHandle	WidgetRefreshTaskHandle;
extern xTaskHandle	ActionProcessorTaskHandle;
extern xTaskHandle UartDataReceiveTaskHandle;
extern xTaskHandle TimerProcessTaskHandle;
extern xTaskHandle TagUpdateTaskHandle;

#ifdef PC_SIM
extern "C" extern QueueHandle_t   MouseQueue;
#endif
extern QueueHandle_t   keyboardQueue;
extern QueueHandle_t   TimerCmdQueue;
extern QueueHandle_t   AnimationTimerCmdQueue;
extern QueueHandle_t   UartDataRecvQueue;
extern QueueHandle_t   RefreshQueue;
extern QueueHandle_t   RefreshQueueWithoutDoubleBuffer;
extern QueueHandle_t   ActionInstructionQueue;
extern QueueHandle_t   AnimationTaskQueue;
extern QueueHandle_t   ModbusMasterRWTaskQueue;
#ifdef USE_MODBUS_MASTER
extern QueueHandle_t   ModbusMasterRWTaskQueue;
#endif

#ifdef EMBEDDED
extern uint8_t runningControl;
#endif

extern AnimationClass gAnimationClass;

ActionProcessorClass gAction;

//u8* pPC;
//u8	mStatusRegister;  //表示比较指令结果，为1表示比较为真
//u16  mPageStack[DEPTH];
//u8   mStackPtr;  

//-----------------------------
// 函数名： ActionProcessorTask
// 处理指令队列
// 参数列表：
// @param1 void* pvParameters 指令列表参数
// 备注(各个版本之间的修改):
// 无
//-----------------------------



void ActionProcessorTask(void* pvParameters)
 {
 	u8* InstructionStartAddr;
 	BaseType_t xStatus;
	gAction.mStackPtr = 0;
 	for(;;)
 	{
#ifdef EMBEDDED
		if(runningControl > 0)
			continue;
#endif
 		xStatus=xQueueReceive( ActionInstructionQueue,
 							(void*)&InstructionStartAddr,
 							portMAX_DELAY);
 		if(xStatus!=pdPASS)
 		{
 			return;
 		}
		if(InstructionStartAddr == NULL || (InstructionStartAddr - gPagePtr[WorkingPageID].pActionInstructions) >= gPagePtr[WorkingPageID].mActionInstructionsSize )
		{
			ERROR_PRINT("ERROR in ActionProcessorTask: recive address out of baundary");
			continue;
		}
		else 
		{
			gAction.pPC = InstructionStartAddr;
			gAction.ActionImplement();
		}

  		taskYIELD();
 	}
 }
 
//-----------------------------
// 函数名： SuspendOtherTask
// 暂停其他任务
// 参数列表：
//  无
// 备注(各个版本之间的修改):
// 无
//-----------------------------
void SuspendOtherTask()
{
	vTaskSuspendAll();

	/*vTaskSuspend(MouseTouchTaskHandle);
	vTaskSuspend(WidgetRefreshTaskHandle);
#ifdef USE_UART
	vTaskSuspend(UartDataReceiveTaskHandle);
#endif
	vTaskSuspend(TimerProcessTaskHandle);*/

	//清空队列

#ifdef PC_SIM
	xQueueReset(MouseQueue);
#endif
	xQueueReset(TimerCmdQueue);
#ifdef USE_UART
	xQueueReset(UartDataRecvQueue);
#endif
	xQueueReset(RefreshQueue);
	xQueueReset(RefreshQueueWithoutDoubleBuffer);
	xQueueReset(ActionInstructionQueue);
	xQueueReset(AnimationTaskQueue);
	xQueueReset(AnimationTimerCmdQueue);
#ifdef USE_MODBUS_MASTER
	xQueueReset(ModbusMasterRWTaskQueue);
#endif

	//clear animation queue
	gAnimationClass.animationQueueReset();

}


//-----------------------------
// 函数名： ResumeOtherTask
// 恢复其他任务
// 参数列表：
//  无
// 备注(各个版本之间的修改):
// 无
//-----------------------------
void ResumeOtherTask()
{
	xTaskResumeAll();

//	vTaskResume(MouseTouchTaskHandle);
//	vTaskResume(WidgetRefreshTaskHandle);
//#ifdef USE_UART
//	vTaskResume(UartDataReceiveTaskHandle);
//#endif
//	vTaskResume(TimerProcessTaskHandle);
}


//-----------------------------
// 函数名： ActionProcessorClass
// 构造函数
// 参数列表：
// 无
// 备注(各个版本之间的修改):
// 无
//-----------------------------
ActionProcessorClass::ActionProcessorClass(void)
{
	int i = 0;
	mStackPtr = 0;
	mPosOfPageParaStack = 0;
	for(i = 0; i < DEPTH; i++)
	{
		mPageStack[i] = 0;
	}
	for(i = 0; i < PARANUMBER; i++)
	{
		mPageParaStack[i] = 0;
	}
}

//-----------------------------
// 函数名： ~ActionProcessorClass
// 析构函数
// 参数列表：
// 无
// 备注(各个版本之间的修改):
// 无
//-----------------------------
ActionProcessorClass::~ActionProcessorClass(void)
{
	
}
//-----------------------------
// 函数名： ActionImplement
// 顺序执行各条指令
// 参数列表：
// @param1 u8* StartAddr 起始地址
// 备注(各个版本之间的修改):
// 无
//-----------------------------
void ActionProcessorClass::ActionImplement()
 {
	int err;
 	mStatusRegister = 0;
	
 	while(pPC < (gPagePtr[WorkingPageID].pActionInstructions + gPagePtr[WorkingPageID].mActionInstructionsSize) && *pPC != 0)
 	{
 		err = gAction.ActionDecoder();
		if(err!=0) return;
 		pPC++;
 	}
	if( pPC >= (gPagePtr[WorkingPageID].pActionInstructions + gPagePtr[WorkingPageID].mActionInstructionsSize) ) //no ending of actions
	{
		ERROR_PRINT("ERROR in ActionImplement: no ending instructions detected, the pPC is out of baundary");
		return;
	}
 }

//-----------------------------
// 函数名： ActionDecoder
// 解析各个指令
// 参数列表：
//  无
// 备注(各个版本之间的修改):
// 无
//-----------------------------
int ActionProcessorClass::ActionDecoder()
{
	u32 u32_imm;
	//u16 startAddr;
	//u16 numberOfReg;
	u8 u8_CdRun;
	u8 u8_CdRCd;
	u16 widgetID;
	
	
	u8_CdRun =  mStatusRegister ;//禁用条件执行，但是利用条件执行的框架实现指令跳转，即比较为真是下一条指令跳过
	u8_CdRCd =  1 ;
	mStatusRegister = 0;
	switch( 0x3f & *pPC)
	{
	case(END_CODE):
		{
			void End();
			break;
		}
	case(GOTO_CODE):
		{
			widgetID = *(pPC+3) + (*(pPC+4) << 8);
			GotoPage(u8_CdRun, u8_CdRCd, *(pPC+1), *(pPC+2),widgetID);//
			return -1;

		}
	case(RET_CODE):
		{
			ReturnPage(u8_CdRun, u8_CdRCd);
			return -1;
		}
	case(SET_CODE):
		{
			SetTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;		
			break;			
		}					
	case(INC_CODE):			
		{					
			IncTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;	
			break;		
		}					
	case(DEC_CODE):			
		{					
			DecTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;		
			break;			
		}					
	case(MUL_CODE):			
		{					
			MulTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(DIV_CODE):
		{
			DivTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(MOD_CODE):
		{
			ModTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(AND_CODE):
		{
			AndTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(OR_CODE):
		{
			OrTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(XOR_CODE):
		{
			XorTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(NOT_CODE):
		{
			NotTag(u8_CdRun,u8_CdRCd,*(pPC+1));
			pPC++;
			break;
		}
	case(SL_CODE):
		{
			SLTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(SR_CODE):
		{
			SRTag(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
		
	case(SETIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			SetTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
		
	case(INCIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			IncTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}

	case(DECIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			DecTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(MULIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			MulTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(DIVIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			DivTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(MODIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			ModTagImm(u8_CdRun,u8_CdRCd,*(pPC+1),u32_imm);
			pPC=pPC + 5;
			break;
		}
	case(ANDIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			AndTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(ORIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			OrTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(XORIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			XorTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(SLIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			SLTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(SRIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			SRTagImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(BRCHEQU_CODE):
		{
			BranchEQU(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(BRCHEQUIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			BranchEQUImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(BRCHMORE_CODE):
		{
			BranchMore(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(BRCHMOREIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			BranchMoreImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(BRCHLESS_CODE):
		{
			BranchLess(u8_CdRun,u8_CdRCd,*(pPC+1),*(pPC+2));
			pPC=pPC+2;
			break;
		}
	case(BRCHLESSIMM_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			BranchLessImm(u8_CdRun, u8_CdRCd, *(pPC+1), u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(JUMP_CODE):
		{
			Jump(u8_CdRun,u8_CdRCd,*(pPC+1));
			pPC=pPC+1;
			break;
		}
	case(RWDATA_CODE):
		{
		
			u32_imm = (*(pPC+1)) + ((*(pPC+2))<<8) + ((*(pPC+3))<<16) + ((*(pPC+4))<<24);
			RW_DATA(u8_CdRun,u8_CdRCd,u32_imm);
			pPC = pPC + 4;
			break;
		}
		//timer
	case(SETTIMER_CODE):
		{
			u32_imm = (*(pPC+2)) + ((*(pPC+3))<<8) + ((*(pPC+4))<<16) + ((*(pPC+5))<<24);
			set_timer(u8_CdRun,u8_CdRCd, *(pPC + 1),u32_imm);
			pPC = pPC + 5;
			break;
		}
	case(ANIMATION_CODE):
		{
			u32_imm = *(pPC+2);
			triggerAnimation(u8_CdRun, u8_CdRCd,  *(pPC + 1), (u8)u32_imm);
			pPC = pPC + 2;
			break;
		}
	case(ANIMATION_TAG_CODE):
		{
			u32_imm = *(pPC+2);
			triggerAnimationTag(u8_CdRun, u8_CdRCd,  *(pPC + 1), (u8)u32_imm);
			pPC = pPC + 2;
			break;
		}
	}

	return 0;
}


//-----------------------------
// 函数名： GotoPage
// 跳转指令
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// @param3 u8 page  跳转的页面
// 备注(各个版本之间的修改):
//   会终止指令的顺序执行
//-----------------------------
void ActionProcessorClass::GotoPage  (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_page,u8 u8_nPara,u16 u16_WidgetID)
 {
	// portBASE_TYPE xReturn;
	// char text[200];
	u8* actionAddr;
	DynamicPageClassPtr pageTempPtr;
	u16  pointPosRight;
	u16  numofNumber;
	u16	 signFlag;
	//u8 systemInitial = 1; //默认加载页面时清空高亮情况
	
	//test the input 
	if(u8_page >= ConfigData.NumofPages)
	{
		ERROR_PRINT("ERROR in gotoPage Function: page out of baundary");
		return;
	}
	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		mPageStack[mStackPtr] = WorkingPageID;
 		if(mStackPtr == DEPTH) mStackPtr = 0;
		else mStackPtr++;
 		SuspendOtherTask();
		staticRefresh = 0;
		//WriteText(text,"Goto Page %d.\r\n",page);
		//ERROR_PRINT(text);
		if(u8_nPara > 0)
		{
			if((mPosOfPageParaStack + u8_nPara) >= PARANUMBER)
				mPosOfPageParaStack = 0;
			if(u8_nPara == 10)
			{
				mPageParaStack[mPosOfPageParaStack++] = WorkingPageID;
				mPageParaStack[mPosOfPageParaStack++] = gPagePtr[WorkingPageID].pWidgetList[u16_WidgetID].ATTATCH_CANVAS;
				mPageParaStack[mPosOfPageParaStack++] = gPagePtr[WorkingPageID].pCanvasList[gPagePtr[WorkingPageID].pWidgetList[u16_WidgetID].ATTATCH_CANVAS].mFocusedCanvas;
				mPageParaStack[mPosOfPageParaStack++] = u16_WidgetID;
				mPageParaStack[mPosOfPageParaStack++] = gPagePtr[WorkingPageID].pWidgetList[u16_WidgetID].WidgetAttr;
				mPageParaStack[mPosOfPageParaStack++] = gPagePtr[WorkingPageID].pWidgetList[u16_WidgetID].BindTagID;
				mPageParaStack[mPosOfPageParaStack++] = gPagePtr[WorkingPageID].pWidgetList[u16_WidgetID].MinValueL;
				mPageParaStack[mPosOfPageParaStack++] = gPagePtr[WorkingPageID].pWidgetList[u16_WidgetID].MinValueH;
				mPageParaStack[mPosOfPageParaStack++] = gPagePtr[WorkingPageID].pWidgetList[u16_WidgetID].MaxValueL;
				mPageParaStack[mPosOfPageParaStack++] = gPagePtr[WorkingPageID].pWidgetList[u16_WidgetID].MaxValueH;
				//systemInitial = 0;//加载小键盘页面，不清空高亮
			}
		}

		pageTempPtr = gPagePtr + u8_page;
 		pageTempPtr->loadPage(u8_page);
		if(u8_nPara > 0)
		{
			if((gPagePtr[u8_page].mPageAttr == 1) && (u8_nPara == 10))
			{
				gPagePtr[u8_page].mNumOfPara = u8_nPara;
				gPagePtr[u8_page].mStartOfStack = mPosOfPageParaStack - u8_nPara;
				if((mPageParaStack[gPagePtr[u8_page].mStartOfStack + 4] & 0x001f) == NUMBOX_TEX)
				{
					pointPosRight = (gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 1].WidgetAttr >> 9) & 0xf;
					signFlag = (gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 1].WidgetAttr >> 14) & 0x1;
					numofNumber = (gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 1].WidgetAttr >>5) & 0xf;
					if(pointPosRight > 0)
						numofNumber++;
					if(signFlag > 0)
						numofNumber++;
					pointPosRight = (mPageParaStack[gPagePtr[u8_page].mStartOfStack + 4] >> 9) & 0xf;
					signFlag = (mPageParaStack[gPagePtr[u8_page].mStartOfStack + 4] >> 14) & 0x1;
					if(pointPosRight > 0)
						numofNumber--;
					if(signFlag > 0)
						numofNumber--;

					numofNumber <<= 5;
					mPageParaStack[gPagePtr[u8_page].mStartOfStack + 4] = mPageParaStack[gPagePtr[u8_page].mStartOfStack + 4] & 0xfe1f;
					mPageParaStack[gPagePtr[u8_page].mStartOfStack + 4] = mPageParaStack[gPagePtr[u8_page].mStartOfStack + 4] | numofNumber;

					gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 1].WidgetAttr = mPageParaStack[gPagePtr[u8_page].mStartOfStack + 4];
					gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 1].MinValueL = mPageParaStack[gPagePtr[u8_page].mStartOfStack + 6];
					gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 1].MinValueH = mPageParaStack[gPagePtr[u8_page].mStartOfStack + 7];
					gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 1].MaxValueL = mPageParaStack[gPagePtr[u8_page].mStartOfStack + 8];
					gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 1].MaxValueH = mPageParaStack[gPagePtr[u8_page].mStartOfStack + 9];
				}
					
				actionAddr = (gPagePtr[u8_page].pActionInstructions + gPagePtr[u8_page].pWidgetList[gPagePtr[u8_page].mNumOfWidget - 2].OnRealeaseAction);
				*(actionAddr + 1) = (mPageParaStack[gPagePtr[u8_page].mStartOfStack + 5] & 0xff);
				actionAddr = (gPagePtr[u8_page].pActionStartADDR[0] + gPagePtr[u8_page].pActionInstructions);
				*(actionAddr + 2) = (mPageParaStack[gPagePtr[u8_page].mStartOfStack + 5] & 0xff);
			}
		}
		
		ResumeOtherTask();
	}
 }

 //-----------------------------
// 函数名： ReturnPage
// 返回指令
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//  会终止指令的顺序执行
//-----------------------------
  void ActionProcessorClass::ReturnPage(u8 u8_CdRun, u8 u8_CdRCd)
 {
	// portBASE_TYPE xReturn;
	// char text[200];
	 int i;
	DynamicPageClassPtr pageTempPtr;
	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		if(gPagePtr[WorkingPageID].mPageAttr != 1)
		{
			if(mStackPtr == 0) mStackPtr = DEPTH;
			else mStackPtr--;
 			SuspendOtherTask();
			pageTempPtr = gPagePtr + (u8)mPageStack[mStackPtr];
			//WriteText(text,"Return Page %d.\r\n",mPageStack[mStackPtr]);
			//ERROR_PRINT(text);
 			pageTempPtr->loadPage((u8)mPageStack[mStackPtr]);
			ResumeOtherTask();
		}
		else
		{
			SuspendOtherTask();
			//WriteText(text,"Return Page %d.\r\n",mPageStack[mStackPtr]);
			//ERROR_PRINT(text);
			pageTempPtr = gPagePtr + (mPageParaStack[gPagePtr[WorkingPageID].mStartOfStack] & 0xff);
 			pageTempPtr->loadPage(mPageParaStack[gPagePtr[WorkingPageID].mStartOfStack] & 0xff);
			ResumeOtherTask();
			for(i = 0;i < gPagePtr[WorkingPageID].mNumOfPara;i++)
			{
				mPageParaStack[mPosOfPageParaStack--] = 0;
			}
		}
 		
	}

 }
 //-----------------------------
// 函数名： ReturnPage
// 将tag与目标tag设为同一个值
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::SetTag(u8 u8_CdRun,u8 u8_CdRCd,u8 u8_DestTagClassListTag,u8 u8_SourceTagClassListTag)
{
	//char text[200];
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function SetTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function SetTag: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		if(TAG_IS_INT(TagPtr[u8_DestTagClassListTag].mTagType))  TagPtr[u8_DestTagClassListTag].setValue(TagPtr[u8_SourceTagClassListTag].getValue(), u8_DestTagClassListTag);
		else if(TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType)) TagPtr[u8_DestTagClassListTag].setString((char*)TagPtr[u8_SourceTagClassListTag].getValue());
	}
	//WriteText(text,"Set Tag %d value as Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	//ERROR_PRINT(text);
}


 //-----------------------------
// 函数名： ReturnPage
// 将tag与目标tag相加
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::IncTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function IncTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function IncTag: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue+TagPtr[u8_SourceTagClassListTag].mValue), u8_DestTagClassListTag);

	
	// WriteText(text,"Inc Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);
}

 //-----------------------------
// 函数名： ReturnPage
// 将tag与目标tag相减
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::DecTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];

	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function DecTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function DecTag: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue-TagPtr[u8_SourceTagClassListTag].mValue), u8_DestTagClassListTag);
	
	// WriteText(text,"Dec Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);
}

 //-----------------------------
// 函数名： ReturnPage
// 将tag与目标tag相乘
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::MulTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];

	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function MulTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function MulTag: tag type is not number");
		return;
	}


    if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue(((s32)TagPtr[u8_DestTagClassListTag].mValue*(s32)TagPtr[u8_SourceTagClassListTag].mValue), u8_DestTagClassListTag);
	
	// WriteText(text,"Mul Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);
}

 //-----------------------------
// 函数名： ReturnPage
// 将tag与目标tag相除
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::DivTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function DivTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function DivTag: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue(((s32)TagPtr[u8_DestTagClassListTag].mValue/(s32)TagPtr[u8_SourceTagClassListTag].mValue), u8_DestTagClassListTag);
	
	// WriteText(text,"Div Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);
}

 //-----------------------------
// 函数名： ReturnPage
// 将tag与目标tag取模
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::ModTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function ModTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function ModTag: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue%TagPtr[u8_SourceTagClassListTag].mValue), u8_DestTagClassListTag);
	
	// WriteText(text,"Mod Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);
}

 //-----------------------------
// 函数名： ReturnPage
// 将tag与目标tag取与
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::AndTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function AndTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function AndTag: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue)&(TagPtr[u8_SourceTagClassListTag].mValue), u8_DestTagClassListTag);
	
	// WriteText(text,"And Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

 //-----------------------------
// 函数名： OrTag
// 将tag与目标tag取或
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::OrTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function OrTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function OrTag: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue)|(TagPtr[u8_SourceTagClassListTag].mValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Or Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

 //-----------------------------
// 函数名：XorTag
// 将tag与目标tag取异或
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::XorTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function XorTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function XorTag: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue)^(TagPtr[u8_SourceTagClassListTag].mValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Xor Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

 //-----------------------------
// 函数名： NotTag
// 将tag取非
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::NotTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function NotTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function NotTag: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue(~(TagPtr[u8_DestTagClassListTag].mValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Not Tag %d value, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

 //-----------------------------
// 函数名： ReturnPage
// 将tag左移目标tag
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::SLTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function SLTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function SLTag: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue)<<(TagPtr[u8_SourceTagClassListTag].mValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Shift Left Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

 //-----------------------------
// 函数名： SRTag
// 将tag右移目标tag
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   貌似没有用上
//-----------------------------
void ActionProcessorClass::SRTag  (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function SRTag: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function SRTag: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue)>>(TagPtr[u8_SourceTagClassListTag].mValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Shift Right Tag %d value by Tag %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

 //-----------------------------
// 函数名： SetTagImm
// 将tag设为目标32位数
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   32位
//-----------------------------
void ActionProcessorClass::SetTagImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function SetTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function SetTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue(u32_ImmValue,u8_DestTagClassListTag);//&?
	
	// WriteText(text,"Set Tag %d value as Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

 //-----------------------------
// 函数名： IncTagImm
// 将tag增加目标32位数
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   32位
//-----------------------------
void ActionProcessorClass::IncTagImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];	

	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function IncTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function IncTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue+u32_ImmValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Inc Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}


//-----------------------------
// 函数名： DecTagImm
// 将tag减少目标32位数
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   32位
//-----------------------------
void ActionProcessorClass::DecTagImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];
		
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function DecTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function DecTagImm: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue-u32_ImmValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Dec Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

//-----------------------------
// 函数名： MulTagImm
// 将tag减少乘以32位数
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   32位
//-----------------------------
void ActionProcessorClass::MulTagImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];
		
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function MulTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function MulTagImm: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue(((s32)TagPtr[u8_DestTagClassListTag].mValue*(s32)u32_ImmValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Mul Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}

//-----------------------------
// 函数名： DivTagImm
// 将tag除以目标32位数
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// 备注(各个版本之间的修改):
//   32位
//-----------------------------
void ActionProcessorClass::DivTagImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];
		
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function DivTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function DivTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue(((s32)TagPtr[u8_DestTagClassListTag].mValue/(s32)u32_ImmValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Div Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}
void ActionProcessorClass::ModTagImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function ModTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function ModTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue%u32_ImmValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Mod Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}
void ActionProcessorClass::AndTagImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function AndTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function AndTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue&u32_ImmValue),u8_DestTagClassListTag);
	
	// WriteText(text,"And Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}
void ActionProcessorClass::OrTagImm (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function OrTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function OrTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue|u32_ImmValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Or Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}
void ActionProcessorClass::XorTagImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function XorTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function XorTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue^u32_ImmValue),u8_DestTagClassListTag);
	
	// WriteText(text,"Xor Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}
void ActionProcessorClass::SLTagImm (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{	
	//char text[200];	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function SLTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function SLTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue<<u32_ImmValue), u8_DestTagClassListTag );
	
	// WriteText(text,"Shift Left Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}
void ActionProcessorClass::SRTagImm (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags )
	{
		ERROR_PRINT("ERROR in function SRTagImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function SRTagImm: tag type is not number");
		return;
	}


	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
		TagPtr[u8_DestTagClassListTag].setValue((TagPtr[u8_DestTagClassListTag].mValue>>u32_ImmValue), u8_DestTagClassListTag);
	
	// WriteText(text,"Shift Right Tag %d value by Imm %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
}
void ActionProcessorClass::BranchEQU    (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function BranchEQU: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function BranchEQU: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		if((s32)TagPtr[u8_DestTagClassListTag].getValue() == (s32)TagPtr[u8_SourceTagClassListTag].getValue())
			{
				mStatusRegister = 1;
			}
		else mStatusRegister = 0;
	}

    //WriteText(text,"BranchEQU Tag %d value between Tag %d, u8_CdRun %d, this instruction will %s Run.\r\nNow mStatusRegister equals %d.\r\n",
  	//  u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT",mStatusRegister);
    //ERROR_PRINT(text);	
}
void ActionProcessorClass::BranchEQUImm (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function BranchEQUImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function BranchEQUImm: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		if((s32)TagPtr[u8_DestTagClassListTag].getValue() == (s32)u32_ImmValue)
			mStatusRegister = 1;
		else mStatusRegister = 0;
	}
	
	// WriteText(text,"BranchEQU Tag %d value between Imm %d, u8_CdRun %d, this instruction will %s Run.\r\nNow mStatusRegister equals %d.\r\n",
	//	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT",mStatusRegister);
	// ERROR_PRINT(text);	
}
void ActionProcessorClass::BranchMore   (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function BranchMore: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function BranchMore: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		if((s32)TagPtr[u8_DestTagClassListTag].getValue() > (s32)TagPtr[u8_SourceTagClassListTag].getValue())
			mStatusRegister = 1;
		else mStatusRegister = 0;
	}

     //WriteText(text,"BranchMore Tag %d value between Tag %d, u8_CdRun %d, this instruction will %s Run.\r\nNow mStatusRegister equals %d.\r\n",
  	 //  u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT",mStatusRegister);
     //ERROR_PRINT(text);	
}
void ActionProcessorClass::BranchMoreImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];	
	
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function BranchMoreImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function BranchMoreImm: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		if((s32)TagPtr[u8_DestTagClassListTag].getValue() > (s32)u32_ImmValue)
			mStatusRegister = 1;
		else mStatusRegister = 0;
	}

     //WriteText(text,"BranchMore Tag %d value between Imm %d, u8_CdRun %d, this instruction will %s Run.\r\nNow mStatusRegister equals %d.\r\n",
  	 //  u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT",mStatusRegister);
     //ERROR_PRINT(text);	
}
void ActionProcessorClass::BranchLess   (u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag)
{
	//char text[200];
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags || u8_SourceTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function BranchLess: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) || TAG_IS_STR(TagPtr[u8_SourceTagClassListTag].mTagType))
	{
		ERROR_PRINT("ERROR in function BranchLess: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		if((s32)TagPtr[u8_DestTagClassListTag].getValue() < (s32)TagPtr[u8_SourceTagClassListTag].getValue())
			mStatusRegister = 1;
		else mStatusRegister = 0;
	}

    //WriteText(text,"BranchLess Tag %d value between Tag %d, u8_CdRun %d, this instruction will %s Run.\r\nNow mStatusRegister equals %d.\r\n",
    //	 u8_DestTagClassListTag,u8_SourceTagClassListTag,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT",mStatusRegister);
    //ERROR_PRINT(text);	
}
void ActionProcessorClass::BranchLessImm(u8 u8_CdRun, u8 u8_CdRCd, u8 u8_DestTagClassListTag, u32 u32_ImmValue)
{
	//char text[200];
	// check mTagType of tag
	if(u8_DestTagClassListTag >= ConfigData.NumofTags)
	{
		ERROR_PRINT("ERROR in function BranchLessImm: tag index out of baundary");
		return ;
	}
	if( TAG_IS_STR(TagPtr[u8_DestTagClassListTag].mTagType ) )
	{
		ERROR_PRINT("ERROR in function BranchLessImm: tag type is not number");
		return;
	}

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		if((s32)TagPtr[u8_DestTagClassListTag].getValue() < (s32)u32_ImmValue)
			mStatusRegister = 1;
		else mStatusRegister = 0;
	}

    // WriteText(text,"BranchLess Tag %d value between Imm %d, u8_CdRun %d, this instruction will %s Run.\r\nNow mStatusRegister equals %d.\r\n",
    //	 u8_DestTagClassListTag,Value,u8_CdRun?"Enabled":"Disabled",(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT",mStatusRegister);
    // ERROR_PRINT(text);	
}

//-----------------------------
// 函数名： Jump
// 跳转目标32位数宽的指令
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// @param3 u8 value
// 备注(各个版本之间的修改):
//   不是跳转多少个，每个指令不同宽度
//-----------------------------
void ActionProcessorClass::Jump (u8 u8_CdRun, u8 u8_CdRCd, u8 u32_ImmValue)
{
	//char text[200];
	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		pPC = pPC + (s8)u32_ImmValue;
	// WriteText(text,"Jump pPC at value of %d for %d, u8_CdRun %d, this instruction will %s Run.\r\n",
	//	 pPC,(s8)Value,(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRun ))?"":"NOT");
	// ERROR_PRINT(text);	
	}
}

//-----------------------------
// 函数名： WriteReadData
// 写从指定位置开始的指令数量的16位寄存器值
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行

// 备注(各个版本之间的修改):
//   不是跳转多少个，每个指令不同宽度
//-----------------------------
void ActionProcessorClass::RW_DATA(u8 u8_CdRun , u8 u8_CdRCd ,u32 u32_ImmValue)
{
	ModBusMasterRWMsg mModBusRWMsg;
	u8 u8_Num = (u8)(u32_ImmValue & 0xff);////读写数目
	u16 u16_StartAddr = (u16)((u32_ImmValue>>8) & 0xffff);  /////起始地址
	u8 u8_type = (u8)((u32_ImmValue>>24) & 0x0f);////读写  1表示写 2表示读 待扩展
//	u8 bus_type = (u8)((u32_ImmValue>>28) & 0x0f);///协议类型

	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))  
	{
		//输出
		if(u8_type == 1)  //////Master 写多个寄存器
		{
			if(u8_Num > M_REG_INPUT_NREGS || u16_StartAddr > M_REG_INPUT_NREGS)
				return;
			else if((u16_StartAddr + u8_Num)  > M_REG_INPUT_NREGS)
			     u8_Num = M_REG_INPUT_NREGS - u16_StartAddr;

			mModBusRWMsg.opCode = AHMI_WRITE_CODE;
			mModBusRWMsg.regID = u16_StartAddr;
			mModBusRWMsg.numofReg = u8_Num;
			xQueueSendToBack(ModbusMasterRWTaskQueue,&mModBusRWMsg,portMAX_DELAY);
		}
		else if(u8_type == 2)
		{
			if(u8_Num > M_REG_INPUT_NREGS || u16_StartAddr > M_REG_INPUT_NREGS)
				return;
			else if((u16_StartAddr + u8_Num) > M_REG_INPUT_NREGS)
			     u8_Num = M_REG_INPUT_NREGS - u16_StartAddr;

			mModBusRWMsg.opCode = AHMI_READ_CODE;
			mModBusRWMsg.regID = u16_StartAddr;
			mModBusRWMsg.numofReg = u8_Num;
			xQueueSendToBack(ModbusMasterRWTaskQueue,&mModBusRWMsg,portMAX_DELAY);

		}
	}
}

//-----------------------------
// 函数名： set_timer
// 写从指定位置开始的指令数量的16位寄存器值
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// @param3 u8 source [7:3] timer ID [2:0] tiemr variable ID
// @param4 u32 imm    tag的值
// 备注(各个版本之间的修改):
//   设置timer的变量值
//-----------------------------
void ActionProcessorClass::set_timer(u8 u8_CdRun , u8 u8_CdRCd , u8 source, u32 u32_ImmValue)
{
	u8 timerID;
	u8 timerVariableID;
	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd )) 
	{
		timerID = source >> 3;
		timerVariableID = source & 0x07;
		TimerPtr[timerID].TimerSetValue(timerVariableID, u32_ImmValue);
	}
}

//-----------------------------
// 函数名： set_timer
// 写从指定位置开始的指令数量的16位寄存器值
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// @param3 u8 source [7:3] timer ID [2:0] tiemr variable ID
// @param4 u32 imm    tag的值
// 备注(各个版本之间的修改):
//   设置timer的值
//-----------------------------
void ActionProcessorClass::triggerAnimation(u8 u8_CdRun , u8 u8_CdRCd , u8 tagID, u8 u8_animationID)
{
	TagClassPtr curTag;
	CanvasClassPtr curCanvas = NULL;
	CanvasLinkDataPtr curCanvasLinkDataPtr;
	int i;
	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))
	{
		curTag = &TagPtr[tagID];
		if(curTag->mNumOfElementLinker != (u8)-1)
		{
			for(i = 0; i < curTag->mNumOfElementLinker; i++ )
			{
				if(curTag->pTagELmementLinker[i].mElementType == ELEMENT_TYPE_CANVAS)
				{
					curCanvasLinkDataPtr = (CanvasLinkDataPtr)curTag->pTagELmementLinker[i].mLinkElementPtr;
					curCanvas = &gPagePtr[curCanvasLinkDataPtr->LinkCanvasPageID].pCanvasList[curCanvasLinkDataPtr->LinkCanvasID];
					curCanvas->triggerAnimation(u8_animationID);
				}
			}
		}
		if(curCanvas == NULL)
			return;
		
	}
}

//-----------------------------
// 函数名： triggerAnimationTag
// trigger the animation of animation tag
// 参数列表：
// @param1 u8 u8_CdRun 是否进行条件执行判断，0表示直接执行
// @param2 u8 u8_CdRun 条件执行，mStatusRegister和它相等表示不执行
// @param3 u8 source [7:3] timer ID [2:0] tiemr variable ID
// @param4 u32 imm    tag的值
// 备注(各个版本之间的修改):
//   设置timer的值
//-----------------------------
void ActionProcessorClass::triggerAnimationTag(u8 u8_CdRun , u8 u8_CdRCd , u8 tagID, u8 u8_animationTagID)
{
	TagClassPtr curTag;
	TagClassPtr animationTag;
	CanvasClassPtr curCanvas = NULL;
	u8 u8_animationID;
	CanvasLinkDataPtr curCanvasLinkDataPtr;
	int i;
	if(!u8_CdRun || (u8_CdRun && mStatusRegister == u8_CdRCd ))
	{
		curTag = &TagPtr[tagID];
		animationTag = &TagPtr[u8_animationTagID];
		if(animationTag != NULL)
			u8_animationID = animationTag->getValue();
		else 
			return;
		if(curTag->mNumOfElementLinker != (u8)-1)
		{
			for(i = 0; i < curTag->mNumOfElementLinker; i++ )
			{
				if(curTag->pTagELmementLinker[i].mElementType == ELEMENT_TYPE_CANVAS)
				{
					curCanvasLinkDataPtr = (CanvasLinkDataPtr)curTag->pTagELmementLinker[i].mLinkElementPtr;
					curCanvas = &gPagePtr[curCanvasLinkDataPtr->LinkCanvasPageID].pCanvasList[curCanvasLinkDataPtr->LinkCanvasID];
					curCanvas->triggerAnimation(u8_animationID);
				}
			}
		}
		if(curCanvas == NULL)
			return;
		
	}
}

#endif 
