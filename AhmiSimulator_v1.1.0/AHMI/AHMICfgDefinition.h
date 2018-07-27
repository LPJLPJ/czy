////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    definition for debug
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef AHMI_CFG_DEFINITION
#define AHMI_CFG_DEFINITION

//#define DEBUG        //������DEBUG�������еĵ�����Ϣ

//**************ͨ���޸�������������ѡ��汾********************
#define PC_SIM         //PCƽ̨����ר��
//#define EMBEDDED     //Ƕ��ʽר�ð汾
//***************************************************************


#ifdef PC_SIM

//#define AHMI_DEBUG            //������DEBUG�������еĵ�����Ϣ
#define AHMI_CORE        //�ںˣ�����ƽ̨�����õ��Ĵ���
#define USE_TOUCHSCREEN
#define TEST_VECTOR_GEN  //�򿪻�����FPGAר�õķ����ļ�
//#define USE_UART
//#define IMPLEMENTATION   //ר���ڼ��ɰ汾�����
#define KEYBOARD_DEBUG  //�����ڵ��Լ��̵�ָ��
//#define TEST_LOG_GEN
//#define TILE_BUFFER_LOG_GEN
//#define PARTIAL_DOUBLE_BUFFER //����˫����
#define WHOLE_TRIBLE_BUFFER  //��ȫ������
#define STATIC_BUFFER_EN     //�Ƿ�����̬����
//#define VISUAL_SOURCE_BOX_EN //�Ƿ�����Χ�п��ӻ�

#endif

#ifdef EMBEDDED

//#define DEBUG          //������DEBUG�������еĵ�����Ϣ
#define AHMI_CORE      //�ںˣ�����ƽ̨�����õ��Ĵ���
#define USE_TOUCHSCREEN
//#define KEYBOARD_DEBUG  //�����ڵ��Լ��̵�ָ��

//#define USING_XILINX
#define USING_LATTICE
//#define USING_CHANGE_BRIGHTNESS
#define USING_NO_CHANGE_BRIGHTNESS
//#define USE_UART
#define KEYBOARD_DEBUG  //�����ڵ��Լ��̵�ָ��
#define PARTIAL_DOUBLE_BUFFER //����˫����
//#define WHOLE_TRIBLE_BUFFER  //��ȫ������
//#define PARTIAL_TRIBLE_BUFFER //����������

#endif

//all version, refresh strategy
//#define SOURCE_BOX_COMBINE  //������Χ�кϲ�����

//all version, enable the module test
//#define MODULE_TEST_EN


#endif //end of header
