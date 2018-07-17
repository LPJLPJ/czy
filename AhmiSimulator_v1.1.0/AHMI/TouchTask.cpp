////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     Mouse.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
//#include "task.h"
#include "TouchTask.h"

#ifdef EMBEDDED
#include "i2c_aw2083.h"
#endif


extern DynamicPageClassPtr  gPagePtr;
extern u16             WorkingPageID;


//鼠标
extern PIDPoint pressPoint;
extern PIDPoint releasePoint;
//鼠标目前状态
extern u8 mouseStatus;

#ifdef PC_SIM
extern "C" QueueHandle_t   MouseQueue;
#endif

#ifdef EMBEDDED
extern Position ScreenCurPosition;
extern Position ScreenLeavePosition;
extern uint32_t flag_Calibration;
static Position PressPosition;
#endif


//Touch screen or mouse
PIDPoint pressPoint;
PIDPoint releasePoint;
//Statuus of touch screen or mouse
u8 mouseStatus = MOUSE_RELEASE;
//-----------------------------
// 函数名： MouseTouchTask
// 触屏消息队列处理函数
// 参数列表：
//    @param1 void* pvParameters     消息队列中的参数
// 备注(各个版本之间的修改):
//-----------------------------
void MouseTouchTask(void* pvParameters)
{
	PIDState MousePtr;
	ActionTriggerClass tagtrigger;
#ifdef PC_SIM
 	BaseType_t xStatus;
#endif

#ifdef EMBEDDED
	 static	u16 TouchFlag = TFNOTHING;
	 static Position tempPosition;

//	if((flag_Calibration == 0) || (flag_Calibration == 0xFFFFFFFF))
//	{
//		while(AW2083_Calibration() == 0);
//	}
	
 	for(;;)
	{
		GetScreenPositionXY();
		
		switch(ScreenCurPosition.event)
		{
			case CLICK:
				if(TouchFlag == TFNOTHING)
					TouchFlag = TFTRYCLICKONE;
				else if((TouchFlag == TFTRYRELEAVEONE) || (TouchFlag == TFTRYRELEAVETWO))
					TouchFlag = TFHOLD;
				break;
			case HOLD:
				if((TouchFlag == TFHOLD) || (TouchFlag == TFCLICK))
					TouchFlag = TFHOLD;
				else if(TouchFlag == TFTRYCLICKONE)
					TouchFlag = TFTRYCLICKTWO;
				else if(TouchFlag == TFTRYCLICKTWO)
					TouchFlag = TFCLICK;
				break;
			case RELEASE:
				if((TouchFlag == TFHOLD) || (TouchFlag == TFCLICK))
				{
					TouchFlag = TFTRYRELEAVEONE;
					tempPosition.pos_x = ScreenLeavePosition.pos_x;
					tempPosition.pos_y = ScreenLeavePosition.pos_y;
				}
				else if((TouchFlag == TFTRYCLICKONE) || (TouchFlag == TFTRYCLICKTWO))
					TouchFlag = TFTRYRELEAVEONE;					
				break;
			case NOTHING:
				if((TouchFlag == TFNOTHING) || (TouchFlag == TFRELEAVE))
					TouchFlag = TFNOTHING;
				else if(TouchFlag == TFTRYRELEAVEONE)
					TouchFlag = TFTRYRELEAVETWO;
				else if(TouchFlag == TFTRYRELEAVETWO)
					TouchFlag = TFRELEAVE;
				break;
			default:
				TouchFlag = TFNOTHING;
				 break;
		}
	if(TouchFlag == TFNOTHING)
		{
			tempPosition.pos_x = 0;
			tempPosition.pos_y = 0;
		}
		else
		{
			switch(TouchFlag)
			{
				case TFTRYCLICKONE:
				case TFTRYCLICKTWO:
					tempPosition.pos_x = 0;
					tempPosition.pos_y = 0;
					tempPosition.event = 0;
					break;
				case TFCLICK:
					tempPosition.pos_x = ScreenCurPosition.pos_x;
					tempPosition.pos_y = ScreenCurPosition.pos_y;
					tempPosition.event = MOUSE_PRESS;
					PressPosition.pos_x = ScreenCurPosition.pos_x;
					PressPosition.pos_y = ScreenCurPosition.pos_y;
					break;
				case TFHOLD:
					tempPosition.pos_x = ScreenCurPosition.pos_x;
					tempPosition.pos_y = ScreenCurPosition.pos_y;
					tempPosition.event = MOUSE_HOLD;
					break;
				case TFTRYRELEAVEONE:
				case TFTRYRELEAVETWO:
					tempPosition.event = MOUSE_HOLD;
					break;
				case TFRELEAVE:
					tempPosition.pos_x = PressPosition.pos_x;
					tempPosition.pos_y = PressPosition.pos_y;
					tempPosition.event = MOUSE_RELEASE;
					PressPosition.pos_x = 0;
					PressPosition.pos_y = 0;
					break;
				default:
					break;
			}

			//printf("temp_X:%d\n",tempPosition.pos_x);
			//printf("temp_Y:%d\n",tempPosition.pos_y); 
			if(tempPosition.event)
			{
				MousePtr.x = tempPosition.pos_x;
				MousePtr.y = tempPosition.pos_y;
				MousePtr.press = tempPosition.event;

//				tagtrigger->mMousePID = MousePtr;
//				tagtrigger->mInputType= MOUSETOUCH;
//				gPagePtr[WorkingPageID].MouseTouch(tagtrigger);
				if(MousePtr.press == MOUSE_PRESS)
				{
					if(mouseStatus == MOUSE_RELEASE) //mouse release
					{
						pressPoint.x = MousePtr.x;
						pressPoint.y = MousePtr.y;
						tagtrigger.mInputType= ACTION_MOUSE_PRESS;
						mouseStatus = MOUSE_PRESS;
					}
					else 
					{
						taskYIELD();
						continue;
					}
				}
				else if(MousePtr.press == MOUSE_RELEASE)
				{
					if(mouseStatus == MOUSE_PRESS) //????????PRESS????????????
					{
						releasePoint.x = MousePtr.x;
						releasePoint.y = MousePtr.y;
						tagtrigger.mInputType= ACTION_MOUSE_RELEASE;
						mouseStatus = MOUSE_RELEASE;
					}
				}
				else if(MousePtr.press == MOUSE_HOLD)
				{
					releasePoint.x = MousePtr.x;
					releasePoint.y = MousePtr.y;
					tagtrigger.mInputType= ACTION_MOUSE_HOLDING;
				}
				else 
				{
					taskYIELD();
					continue;
				}
				tagtrigger.MouseTouch(&pressPoint, &releasePoint);
				taskYIELD();
			}
			
		}	
		
		vTaskDelay(20 / portTICK_RATE_MS);
	}
#endif

#ifdef PC_SIM
 	for(;;)
 	{
 		xStatus=xQueueReceive(MouseQueue,
			                  (void*)&MousePtr,
							  portMAX_DELAY
			                   );
 		if(xStatus!=pdPASS)
 		{
 			return;
 		}
		
		if(MousePtr.press == MOUSE_PRESS)
		{
			//记录按下坐标
			if(mouseStatus == MOUSE_RELEASE) //只有当前状态是抬起的时候才允许更改按下的坐标
			{
				pressPoint.x = MousePtr.x;
				pressPoint.y = MousePtr.y;
				tagtrigger.mInputType= ACTION_MOUSE_PRESS;
				mouseStatus = MOUSE_PRESS;
			}
			else 
			{
				taskYIELD();
				continue;
			}
		}
		else if(MousePtr.press == MOUSE_RELEASE)
		{
			if(mouseStatus == MOUSE_PRESS) //只有当前的状态是PRESS的时候才允许抬起鼠标触发
			{
				releasePoint.x = MousePtr.x;
				releasePoint.y = MousePtr.y;
				tagtrigger.mInputType= ACTION_MOUSE_RELEASE;
				mouseStatus = MOUSE_RELEASE;
			}
		}
		else if(MousePtr.press == MOUSE_HOLD)
		{
			releasePoint.x = MousePtr.x;
			releasePoint.y = MousePtr.y;
			tagtrigger.mInputType= ACTION_MOUSE_HOLDING;
		}
		else 
		{
			taskYIELD();
			continue;
		}
 		tagtrigger.MouseTouch(&pressPoint, &releasePoint);
  		taskYIELD();
 	}
#endif

}




