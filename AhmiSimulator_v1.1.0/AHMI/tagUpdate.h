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
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////
#include "FreeRTOS.h"
#include "publicType.h"
#include "tagUpdateClass_c1.h"
#include "publicDefine.h"

#ifndef  TAGUPDATE__H
#define  TAGUPDATE__H

#ifdef AHMI_CORE
class TagUpdateClass
{
public:
	u32 mRegID;         //reg标号
	u16 mTagID;         //tag标号
	TagUpdateClass();
	~TagUpdateClass();
};

typedef class TagUpdateClass* TagUpdateClassPtr;

void initTagUpdateQueue(u16 NumofTagUpdateQueue,u16 NumofTags, TagClassPtr tagPtr, TagUpdateClassPtr TagUpdatePtr);
void TagUpdateTask(void* pvParameters);
void sortTagUpdateClass(u16 NumofTagUpdateQueue,TagUpdateClassPtr TagUpdatePtr);
//for a certain time, this task will trigger the set binding element function
void TagSetBindingElementTask(void* pvParameters);
void UpdateAllTags(void);

#endif

#endif

