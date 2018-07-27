#ifndef DYNAMICPAGECFGCLASS__H
#define DYNAMICPAGECFGCLASS__H
#include "publicType.h"
#include "publicDefine.h"
#ifdef AHMI_CORE

class DynamicPageCfgClass
{
public:
	//����
	u32      StoreOffset            ;//ҳ��λ�ڴ洢�ļ���ƫ����
	u16      PageAttr               ;//ҳ������ 
	u16      totalNumOfTexture	    ;//��������
	u16      ActionInstructionsSize ;//��ָ���
	u16      NumOfPageAction        ;//Action����
	u16		 NumOfCanvasAction		;//CanvasAction����
	u16      NumOfWidget            ;//widget������
	u16      NumofTouchWidget       ;//�ɴ����ؼ�����
	u8		 NumOfCanvas			;//Canvas����
	u8       NumOfSubCanvas         ;//subCanvas����
	u8       AnimationType          ;//��������
	u8       enlightedWidgetID      ;//��һ��������widget��ID
	u8		mStartOfStack;    //��ҳʱ���ݵĲ�����ջ�е�λ��
	u8		mNumOfPara;			//��ҳʱ���ݵĲ�������
	u8       animationFrame         ;//����֡��
	u8      mNunOfAnimationActions  ;//�û��Զ��嶯����Ŀ 
	//??
	DynamicPageCfgClass(void);
	~DynamicPageCfgClass(void);
};

class  AHMIInfoClass
{
public:
	u16 Version;
	u16 ScreenWidth;
	u16 ScreenHeight;
	u16 ColorSpace;
	u16 TileSize;
};

class  ConfigInfoClass
{
public:
	u8  md5_value[32];
	u32 MagicNumber;//0x494d4841
	u32 TotalSize;//�����ļ��ܴ�С
	u32 TotalTextureSize;//������ߴ磬��λΪ�ֽ�
	u32 DynamicPageSize; //ҳ���С��Ϊ����ҳ���ֵ
	u16 NumofTimers;//timer������
	u16 NumofUartBuf;//uart���ջ�����������
	u16 UartBufSizeforEach;//ÿ��uart���ջ������ĳߴ�
	u16 NumofTags;//TAG������������timer��ռ�õ�TAG
	u16 NumofTagUpdateQueue;//��Ҫ��MCUѰ����µ�Tag����
	
	u16 tagUpdateTime;//��ʾÿ������ٺ��룬����һ��tag��������Ϊ0����ʾ�����¡�
	u16 StringSize;   //��ʾ�����ַ�������tag�ڴ�ռ�
	u16 CfgStringSize; //��ʾ�����ַ����ĳ���
	u8  NumofPages;    //page������
	u8  InterfaceType;//��ʾͨѶЭ������,0ΪAHMI�Զ�Э�飬1ΪModbus Master��2ΪModbus Slave��
	u8  TagStringLength;//��ʾ�ַ�������tag����󳤶ȡ�
	u8  innerClockEn;   //�ڲ�ʱ��ʹ��
	s16 screenratio;    //��Ļ�������
	AHMIInfoClass AHMIInfo;
};

#endif

#endif

