////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     animation.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION__H
#define ANIMATION__H
#include "publicType.h"
#include "publicDefine.h"
#include "animationDefine.h"
#include "refreshQueueHandler.h"

#ifdef AHMI_CORE

union ElementPtr;
struct Animation;
typedef void * TimerHandle_t;

class AnimationClass
{
	//����
private:
	AnimationMsgPtr pAnimationMsgPtr;
	TimerHandle_t   aTimer;   //����ʱ��

	//����
public:
	//-----------------------------
	// �������� initAnimationMsg
	// ��ʼ������
	// �����б�
	// ��ע(�����汾֮����޸�):
	//   ��
	//-----------------------------
	funcStatus initAnimationMsg();


	//-----------------------------
	// �������� initAnimationTimer
	// ��ʼ������
	// �����б�
	// ��ע(�����汾֮����޸�):
	//   ��
	//-----------------------------
	funcStatus initAnimationTimer(u16 TimerID);

	//-----------------------------
	// �������� Create_AnimatianData
	// ���������б�
	// �����б�
	// @param1 u8 A_RefreshType      Ԫ�����ͣ��궨���"animationDefine.h"
	// @prarm2 ElementPtr A_New_ElementPtr �»���Ԫ��ָ��
	// @parma3  ElementPtr   A_Old_ElementPtr  ��Ԫ��ָ��
	// ��ע(�����汾֮����޸�):
	//   ��
	//-----------------------------
	funcStatus Create_AnimatianData(	
		u8           A_RefreshType,    //Ԫ������   
		ElementPtr   A_New_ElementPtr, //��Ԫ��ָ��
		ElementPtr   A_Old_ElementPtr  //��Ԫ��ָ��
		); 

	//-----------------------------
	// �������� Animation_maintain
	// ά�������б�
	// �����б�
	// @param1 u8 A_RefreshType      Ԫ�����ͣ��궨���"animationDefine.h"
	// @prarm2 ElementPtr A_New_ElementPtr �»���Ԫ��ָ��
	// @parma3  ElementPtr   A_Old_ElementPtr  ��Ԫ��ָ��
	// ��ע(�����汾֮����޸�):
	//   ��
	//-----------------------------
	void Animation_maintain(u8 refreshTime);

	//-----------------------------
	// �������� animationTaskQueueHandle
	// �������д�������
	// �����б�
	//     @param1 animationTaskQueueMsg i ������Ϣ
	// ��ע(�����汾֮����޸�):
	//   
	//-----------------------------
	funcStatus animationTaskQueueHandle(animationTaskQueueMsg i);

	//-----------------------------
	// �������� animationQueueReset
	// �����������
	// �����б�
	//     void
	// ��ע(�����汾֮����޸�):
	//   
	//-----------------------------
	funcStatus animationQueueReset(void);

};



#endif

#endif


