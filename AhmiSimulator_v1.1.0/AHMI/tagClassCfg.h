////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     TextureClass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef TAGCLASSCFG__H
#define TAGCLASSCFG__H

#include "publicType.h"
#include "publicDefine.h"
#include "TagClass.h"

#ifdef AHMI_CORE
class TagClassCfg
{
public:
	u8	mTagType;//tag的类型，0表示整数，1表示字符串的地址,并且type[5:1]表示字符数量
	u8 NumOfWidgetLinker;//链接到Tag的Widget个数
	u8 NumOfDynamicPageAction;  //链接到Tag的DynamicPage事件个数
	u8 NumOfCanvasAction; //linked canvas actions num
	u32 mRegID; //tag绑定的reg,最高位为0表示不绑定, 20160904
	u32 mValue;
};
#endif

#endif
