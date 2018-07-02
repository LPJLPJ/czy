////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI~{P!Wi3IT1~}
//
// Create Date:   2015/11/17
// File Name:     Tagclass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.04 - C++, 20160321 by ~{SZ4:S*~}
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef   TAGCLASS__H
#define   TAGCLASS__H

#include "AHMIBasicStructure.h"
#include "publicType.h"
#include "tagClass_cd.h"
#include "tagClass_c1.h"
#include "publicDefine.h"
#ifdef AHMI_CORE
class TagClass
{
public:
	u8	mTagType;//tag~{5D@`PM#,~}0~{1mJ>U{J}#,~}1~{1mJ>WV7{4.5D5XV7~},~{2"GR~}type[5:1]~{1mJ>WV7{J}A?~}
	//u8  mLinkTimer; //-1 links no timer, else it refers to the timer id
	u8  mNumOfElementLinker; //~{A4=S5DJ}A?~}
	ElemenLinkDataPtr pTagELmementLinker; //~{A4=S5DT*<~V8Uk=a99Le~}
	u32 mRegID; //tag~{0s6(5D~}reg,~{Wn8_N;N*~}0~{1mJ>2;0s6(~}, 20160904
	u32 mValue;

	TagClass();
	~TagClass();

	u32		getValue() const;
	void	setValue(u32 v, u16 tagID);
	void	setString(char* str);
	void	setBindingElement();
	void	initTag(
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
			);

	

	funcStatus PageActionTrigger(
		DynamicPageClassPtr Ptr,
		u16 TriggerAction
		);

	funcStatus CanvasActionTrigger(
		CanvasClassPtr mCanvas,
		ActionTriggerClassPtr canvasTagTrigger
		);

	//funcStatus bindTimer(
	//	u16 timerID
	//	);

};

typedef TagClass* TagClassPtr;

#endif

#endif

