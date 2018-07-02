////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     FileLoad.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef  FILELOAD__H
#define  FILELOAD__H

//#include "private.h"

#ifdef PC_SIM
funcStatus  OpenACFFile(void);
int   LoadConfigData(ConfigInfoClass*);
funcStatus ReadPageDataFromFile(DynamicPageCfgClass*);
funcStatus ReadTagDataFromFile(TagClassPtr pTag, u8 ID);// 从文件读取tag信息
void ReadTagStringList(u8* ptr, u16 number);
void ReadInitString(u8* ptr, u16 number);
void   LoadTextureToSDRAM(void*,u32,int size);
#endif

#ifdef EMBEDDED
funcStatus LoadConfigData(ConfigInfoClass* mConfigInfo);
funcStatus ReadPageData(DynamicPageCfgClass* curPage);
funcStatus ReadTagData(TagClassPtr pTag, u8 ID);
void ReadTagStringList(u8* ptr, u16 number);
void ReadInitString(u8* ptr, u16 number);

#endif

#endif 
