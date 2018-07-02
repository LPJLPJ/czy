////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIÐ¡×é³ÉÔ±
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/08/11 by Zhou Yuzhi(ÖÜîÚÖÂ)
// Additional Comments:
//    handle the interruption
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "animationDefine.h"
#include "semphr.h"
#include "AHMICfgDefinition.h"
#include "drawImmediately_cd.h"
#include "videoClass.h"
#ifdef EMBEDDED
#include "ahmi3_function.h"
#include "spi_if.h"
#endif


extern u8		stm32info[];
extern SemaphoreHandle_t interuptSemaphore;
extern TagClassPtr     TagPtr;

extern  u8 video_cur_r ; 
extern  u8 video_cur_w ;
extern  u8 video_next_r;

u8 interupt_count;

#define SAMPLE_FRAME 0

#ifdef VIDEO_EN
void interuptTask(void* pvParameters)
{
	static BaseType_t xHigherPriorityTaskWoken; 
	u8 *stm32ptraddr;
	stm32ptraddr = (u8 *)Stm32_interface_Addr;
	interupt_count = 0;
	for(;;)
	{
		if(xSemaphoreTakeFromISR(interuptSemaphore, &xHigherPriorityTaskWoken) == pdTRUE)
		{
			//TagPtr[SYSTEM_VIDEO_TAG].setValue(0,SYSTEM_VIDEO_TAG);
			if(interupt_count == SAMPLE_FRAME)
			{
				taskENTER_CRITICAL();
			if(video_cur_w == (video_cur_r + 1) % 3)
				video_cur_w = (video_cur_w + 1) % 3;
			*( (u32*)(stm32ptraddr + addr_video_0) ) = (START_ADDR_OF_CVBS) + video_cur_w * (SIZE_OF_CVBS_BUFFER);
			SPI_FPGA_Burst_Send_REG_data(AHMICFG_VIDEOADDRBASE0,(u16*)&stm32info[addr_video_0],4/2);
			taskEXIT_CRITICAL();
		  }
			interupt_count = (interupt_count + 1) % (SAMPLE_FRAME + 1);
			
		}
			vTaskDelay(10 / portTICK_RATE_MS);
	}
}

#endif


