////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/30
// File Name:     tagUpdate.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/30
// Revision 2.02 - add the initTagUpdateQueue function by Arthas 20151207
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include "AHMIBasicDefine.h"

#define TAG_UPDATE_TIME 20
#ifdef AHMI_CORE
extern  ConfigInfoClass      ConfigData;
extern  TagUpdateClassPtr    TagUpdatePtr;
extern  xTaskHandle TagUpdateTaskHandle;
extern u8*                 TagChangedListPtr;
extern TagClassPtr     TagPtr;

#ifdef EMBEDDED
extern uint8_t runningControl;
#endif

TagUpdateClass::TagUpdateClass()
{}

TagUpdateClass::~TagUpdateClass()
{}

//-----------------------------
// 函数名： initTagUpdateQueue
// 初始化tagtrigger
// 参数列表：
//   @param1 u16 NumofTagUpdateQueue          tag更新的数组长度
//   @param2 u16 NumofTags                    tag数目
//   @param3 TagClassPtr tagPtr               tag指针
//   @param4 TagUpdateStructPtr TagUpdatePtr  输出的tag更新结构体指针
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void initTagUpdateQueue(u16 NumofTagUpdateQueue,u16 NumofTags, TagClassPtr tagPtr, TagUpdateClassPtr TagUpdatePtr)
{
	int ID;
	int curTag = 0;
	u8 mTagNeedUpdate;
	if((NULL == NumofTagUpdateQueue) || (NULL == NumofTags) || (NULL == tagPtr) || (NULL == TagUpdatePtr))
		return;
	for(ID = 0; ID < NumofTagUpdateQueue; ID++)
		for(;curTag < NumofTags; curTag++)
		{
			mTagNeedUpdate = tagPtr[curTag].mTagType & TAG_READ_REG;
			if(mTagNeedUpdate)
			{
				if(tagPtr)
				TagUpdatePtr[ID].mRegID = tagPtr[curTag].mRegID;
				TagUpdatePtr[ID].mTagID = curTag;
				curTag++;
				break;
			}
		}
	//排序
	sortTagUpdateClass(NumofTagUpdateQueue,TagUpdatePtr);
}


//-----------------------------
// 函数名： TagUpdateTask
// 初始化tagtrigger
// 参数列表：
//   @param1 void* pvParameters  tag更新任务的
// 备注(各个版本之间的修改):
//   无
//-----------------------------
//void TagUpdateTask(void* pvParameters)
//{
//	portTickType xLastWakeTime;
//	int i;
//
//	if(ConfigData.tagUpdateTime == 0) 
//	{
//		vTaskDelete(TagUpdateTaskHandle);
//		return;
//	}
//	if(ConfigData.InterfaceType != INTERFACETYPE_MODMST)
//	{
//		vTaskDelete(TagUpdateTaskHandle);
//		return;
//	}
//
//	xLastWakeTime = xTaskGetTickCount();
//
//
// 	for(;;)
// 	{
//#ifdef EMBEDDED
//		if(runningControl > 0)
//			continue;
//#endif
//		for(i=0;i!=ConfigData.NumofTagUpdateQueue;i++)
//		{
//			//send request via modbus
//			//TagUpdatePtr[i].RegID
//		}
//		//delay for specific time
//		vTaskDelayUntil(&xLastWakeTime,ConfigData.tagUpdateTime/portTICK_RATE_MS);
// 	}
//}

//-----------------------------
// 函数名： TagSetBindingElementTask
// for a certain time, this task will trigger the set binding element function
// 每隔一段时间刷新一次改动的tag所绑定的组件
// 参数列表：
//   @param1 void* pvParameters  tag更新任务的
// 备注(各个版本之间的修改):
//   无
//-----------------------------
//void TagSetBindingElementTask(void* pvParameters)
//{
//	u8 u8_listID;
//	u8 u8_bindingID = 0;
//	u8 temp;
//	u16 tagID = 0;
//	//for(;;)
//	//{
//#ifdef EMBEDDED
//		if(runningControl > 0)
//			continue;
//#endif
//		for(i=0;i!=ConfigData.NumofTagUpdateQueue;i++)
//		{
//			//send request via modbus
//			//TagUpdatePtr[i].RegID
//		}
//		//delay for specific time
//		vTaskDelayUntil(&xLastWakeTime,ConfigData.tagUpdateTime/portTICK_RATE_MS);
// 	}
//}

//-----------------------------
// ???: TagSetBindingElementTask
// for a certain time, this task will trigger the set binding element function
// ?????????????tag??????
// ????:
//   @param1 void* pvParameters  tag?????
// ??(?????????):
//   ?
//-----------------------------
//void TagSetBindingElementTask(void* pvParameters)
//{
//	u8 u8_listID;
//	u8 u8_bindingID = 0;
//	u8 temp;
//	u16 tagID = 0;
//	//for(;;)
//	//{
//#ifdef EMBEDDED
//		if(runningControl > 0)
//			return;
//#endif
//		//scan each tag
//		for(u8_listID=0; u8_listID < (ConfigData.NumofTags + 7) / 8; u8_listID++)
//		{
//			if(TagChangedListPtr[u8_listID] != 0)
//			{
//				for(u8_bindingID = 0; u8_bindingID != 8; u8_bindingID ++)
//				{
//					if(u8_bindingID == 0)
//					{
//						if(TagChangedListPtr[u8_listID] == 1)
//						{
//							tagID = u8_listID * 8;
//							TagPtr[tagID].setBindingElement();
//							temp = 1;
//							TagChangedListPtr[u8_listID] &= (~temp);
//						}
//					}
//					else if( (TagChangedListPtr[u8_listID] & (1 << u8_bindingID) ) != 0) //this tag need to be changed
//					{
//						tagID = u8_listID * 8 + u8_bindingID;
//						TagPtr[tagID].setBindingElement();
//						temp = 1 << u8_bindingID;
//						TagChangedListPtr[u8_listID] &= (~temp);
//					}
//				}
//			}
//			TagChangedListPtr[u8_listID] = 0;
//		}
//		//delay for specific time
//	//	vTaskDelay( TAG_UPDATE_TIME / portTICK_RATE_MS );
//	//}
//}

//-----------------------------
// 函数名： UpdateAllTags
// update all the tags' binding elements
// 刷新一次改动的tag所绑定的组件
// 参数列表：
//   void
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void UpdateAllTags(void)
{
	u8 u8_listID;
	u8 u8_bindingID = 0;
	u8 temp;
	u16 tagID = 0;
#ifdef EMBEDDED
		if(runningControl > 0)
			return;
#endif
	//scan each tag
	for(u8_listID=0; u8_listID < (ConfigData.NumofTags + 7) / 8; u8_listID++)
	{
		if(TagChangedListPtr[u8_listID] != 0)
		{
			for(u8_bindingID = 0; u8_bindingID != 8; u8_bindingID ++)
			{
				if(u8_bindingID == 0)
				{
					if( (TagChangedListPtr[u8_listID] & 0x01) == 1)
					{
						tagID = u8_listID * 8;
						TagPtr[tagID].setBindingElement();
						temp = 1;
						TagChangedListPtr[u8_listID] &= (~temp);
					}
				}
				else if( (TagChangedListPtr[u8_listID] & (1 << u8_bindingID) ) != 0) //this tag need to be changed
				{
					tagID = u8_listID * 8 + u8_bindingID;
					TagPtr[tagID].setBindingElement();
					temp = 1 << u8_bindingID;
					TagChangedListPtr[u8_listID] &= (~temp);
				}
			}
		}
		TagChangedListPtr[u8_listID] = 0;
	}
}

//-----------------------------
// 函数名： sortTagUpdateStruct
// 以reg的标号从小到大进行排序
// 参数列表：
//   @param1 u16 NumofTagUpdateQueue          数组长度
//   @param2 TagUpdateStructPtr TagUpdatePtr  tag更新链表数组
// 备注(各个版本之间的修改):
//   created by arthas 20160114
//-----------------------------
void sortTagUpdateClass(u16 NumofTagUpdateQueue, TagUpdateClassPtr TagUpdatePtr)
{
	int i,j;
	u16 min;
	if((NULL == NumofTagUpdateQueue) || (NULL == TagUpdatePtr))
		return;
	TagUpdateClassPtr pTagUpdateClassTemp;
	TagUpdateClass tagUpdateClassTemp;
	pTagUpdateClassTemp = &tagUpdateClassTemp;
	for(i = 0; i < NumofTagUpdateQueue - 1; i ++)
	{
		min = i;
		for(j = i + 1; j < NumofTagUpdateQueue; j ++)
		{
			if(TagUpdatePtr[j].mRegID < TagUpdatePtr[min].mRegID)
			{
				min = j;
			}
		}
		if(min != i)
		{
			//交换i,min
			pTagUpdateClassTemp->mRegID = TagUpdatePtr[i].mRegID;
			TagUpdatePtr[i].mRegID = TagUpdatePtr[min].mRegID;
			TagUpdatePtr[min].mRegID = pTagUpdateClassTemp->mRegID;
			pTagUpdateClassTemp->mTagID = TagUpdatePtr[i].mTagID;
			TagUpdatePtr[i].mTagID = TagUpdatePtr[min].mTagID;
			TagUpdatePtr[min].mTagID = pTagUpdateClassTemp->mTagID;
		}
	}
}

#endif
