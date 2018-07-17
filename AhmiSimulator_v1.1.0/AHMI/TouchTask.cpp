////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
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


//���
extern PIDPoint pressPoint;
extern PIDPoint releasePoint;
//���Ŀǰ״̬
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
// �������� MouseTouchTask
// ������Ϣ���д�����
// �����б�
//    @param1 void* pvParameters     ��Ϣ�����еĲ���
// ��ע(�����汾֮����޸�):
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
			//��¼��������
			if(mouseStatus == MOUSE_RELEASE) //ֻ�е�ǰ״̬��̧���ʱ���������İ��µ�����
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
			if(mouseStatus == MOUSE_PRESS) //ֻ�е�ǰ��״̬��PRESS��ʱ�������̧����괥��
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




