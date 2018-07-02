////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     TextureClass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
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
	u8	mTagType;//tag�����ͣ�0��ʾ������1��ʾ�ַ����ĵ�ַ,����type[5:1]��ʾ�ַ�����
	u8 NumOfWidgetLinker;//���ӵ�Tag��Widget����
	u8 NumOfDynamicPageAction;  //���ӵ�Tag��DynamicPage�¼�����
	u8 NumOfCanvasAction; //linked canvas actions num
	u32 mRegID; //tag�󶨵�reg,���λΪ0��ʾ����, 20160904
	u32 mValue;
};
#endif

#endif
