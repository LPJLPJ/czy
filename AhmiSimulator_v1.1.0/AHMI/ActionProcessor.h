////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     ActionProcessor.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef  ACTIONPROCESSOR__H
#define  ACTIONPROCESSOR__H

#include "publicDefine.h"
#include  "publicType.h"
#ifdef AHMI_CORE
//**********************************
// Instruction Format
//
// {u8_CdRun ,u8_CdRCd ,Code}
// u8_CdRun : 1bit 是否条件执行
// u8_CdRCd : 1bit 条件执行时的状态
// Code: 6bit  指令编码
//**********************************


#define   END_CODE              0	//End
#define   GOTO_CODE				1	//Gotou8_page  
#define   RET_CODE				2	//Returnu8_page
#define   SET_CODE				3	//SetTag
#define   INC_CODE   			4	//IncTag   
#define   DEC_CODE   			5	//DecTag   
#define   MUL_CODE   			6	//MulTag   
#define   DIV_CODE   			7	//DivTag   
#define   MOD_CODE   			8   //ModTag   
#define   AND_CODE   			9	//AndTag   
#define    OR_CODE  			10	//OrTag    
#define   XOR_CODE   			11	//XorTag   
#define   NOT_CODE   			12	//NotTag   
#define    SL_CODE     			13	//SLTag    
#define    SR_CODE     			14	//SRTag    
#define   SETIMM_CODE			15	//SetTagImm
#define   INCIMM_CODE			16	//IncTagImm
#define   DECIMM_CODE			17	//DecTagImm
#define   MULIMM_CODE			18	//MulTagImm
#define   DIVIMM_CODE			19	//DivTagImm
#define   MODIMM_CODE			20   //ModTagImm
#define   ANDIMM_CODE			21	//AndTagImm
#define    ORIMM_CODE 			22	//OrTagImm 
#define   XORIMM_CODE			23	//XorTagImm
#define    SLIMM_CODE 			24	//SLTagImm 
#define    SRIMM_CODE 			25	//SRTagImm 
#define       BRCHEQU_CODE    	26	//BranchEQU		 ,满足则跳过
#define    BRCHEQUIMM_CODE 		27	//BranchEQUImm 	 ,满足则跳过
#define      BRCHMORE_CODE   	28	//BranchMore   	 ,满足则跳过
#define   BRCHMOREIMM_CODE		29	//BranchMoreImm	 ,满足则跳过
#define      BRCHLESS_CODE   	30	//BranchLess   	 ,满足则跳过
#define   BRCHLESSIMM_CODE		31	//BranchLessImm	 ,满足则跳过
#define   JUMP_CODE             32  //jump to instruction
#define   RWDATA_CODE           33  //read and write external data
#define   SETTIMER_CODE         34  //set timer
#define   ANIMATION_CODE        35  //trigger animation
#define   ANIMATION_TAG_CODE    36  //trigger animation of tag

#define   DEPTH   10
#define   PARANUMBER   32

class ActionProcessorClass{
public:
	u8* pPC;
	u8	mStatusRegister;  //表示比较指令结果，为1表示比较为真
	u8	mStackPtr; 
	u8	mPosOfPageParaStack;
	u16	mPageStack[DEPTH];
	u16 mPageParaStack[PARANUMBER];

	ActionProcessorClass();
	~ActionProcessorClass();

	//action指令函数: 
//页面操作函数
	void GotoPage   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_page,u8 u8_nPara,u16 u16_WidgetID); //
	void ReturnPage (u8 u8_CdRun , u8 u8_CdRCd ); //2
//Tag操作函数	
	void SetTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);//TagPtr是指向TagClass结构体的指针
	void IncTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void DecTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void MulTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void DivTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void ModTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void AndTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void OrTag    (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void XorTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void NotTag   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag);
	void SLTag    (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void SRTag    (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);	
	void SetTagImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void IncTagImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void DecTagImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void MulTagImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void DivTagImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void ModTagImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void AndTagImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void OrTagImm (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void XorTagImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void SLTagImm (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void SRTagImm (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
//分支函数
	void BranchEQU    (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void BranchEQUImm (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void BranchMore   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
	void BranchMoreImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void BranchLess   (u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u8 u8_SourceTagClassListTag);
    void BranchLessImm(u8 u8_CdRun , u8 u8_CdRCd , u8 u8_DestTagClassListTag, u32 u32_ImmValue);
	void Jump         (u8 u8_CdRun , u8 u8_CdRCd , u8 u32_ImmValue);

	void RW_DATA(u8 u8_CdRun , u8 u8_CdRCd ,u32 u32_ImmValue);
	void set_timer(u8 u8_CdRun , u8 u8_CdRCd , u8 source, u32 u32_ImmValue);
	void triggerAnimation(u8 u8_CdRun , u8 u8_CdRCd , u8 tagID, u8 u8_animationID);
	void triggerAnimationTag(u8 u8_CdRun , u8 u8_CdRCd , u8 tagID, u8 u8_animationTagID);


#ifdef KEYBOARD_DEBUG//键盘调试函数
	static void keyboardLeft();
	static void keyboardRight();
	static void keyboardEntered();
#endif

	void ActionImplement(void);//actionSpace的地址
	int ActionDecoder(void);

};

	void ActionProcessorTask(void* pvParameters);
	
	

	void SuspendOtherTask(void);
	void ResumeOtherTask(void);

//终止指令
	void End(void);//remained...




#endif //end of iddef AHMI_CORE
#endif //

