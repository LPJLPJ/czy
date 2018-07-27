#ifndef DYNAMICPAGECFGCLASS__H
#define DYNAMICPAGECFGCLASS__H
#include "publicType.h"
#include "publicDefine.h"
#ifdef AHMI_CORE

class DynamicPageCfgClass
{
public:
	//变量
	u32      StoreOffset            ;//页面位于存储文件的偏移量
	u16      PageAttr               ;//页面属性 
	u16      totalNumOfTexture	    ;//纹理数量
	u16      ActionInstructionsSize ;//总指令长度
	u16      NumOfPageAction        ;//Action数量
	u16		 NumOfCanvasAction		;//CanvasAction数量
	u16      NumOfWidget            ;//widget的数量
	u16      NumofTouchWidget       ;//可触碰控件数量
	u8		 NumOfCanvas			;//Canvas数量
	u8       NumOfSubCanvas         ;//subCanvas数量
	u8       AnimationType          ;//动画类型
	u8       enlightedWidgetID      ;//第一个高亮的widget的ID
	u8		mStartOfStack;    //换页时传递的参数在栈中的位置
	u8		mNumOfPara;			//换页时传递的参数个数
	u8       animationFrame         ;//动画帧数
	u8      mNunOfAnimationActions  ;//用户自定义动画数目 
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
	u32 TotalSize;//配置文件总大小
	u32 TotalTextureSize;//总纹理尺寸，单位为字节
	u32 DynamicPageSize; //页面大小，为最大的页面的值
	u16 NumofTimers;//timer的数量
	u16 NumofUartBuf;//uart接收缓冲区的数量
	u16 UartBufSizeforEach;//每个uart接收缓冲区的尺寸
	u16 NumofTags;//TAG的数量，包含timer所占用的TAG
	u16 NumofTagUpdateQueue;//需要向MCU寻求更新的Tag数量
	
	u16 tagUpdateTime;//表示每间隔多少毫秒，更新一次tag。若该项为0，表示不更新。
	u16 StringSize;   //表示所有字符串类型tag内存空间
	u16 CfgStringSize; //表示配置字符串的长度
	u8  NumofPages;    //page的数量
	u8  InterfaceType;//表示通讯协议类型,0为AHMI自订协议，1为Modbus Master，2为Modbus Slave。
	u8  TagStringLength;//表示字符串类型tag的最大长度。
	u8  innerClockEn;   //内部时钟使能
	s16 screenratio;    //屏幕拉伸比例
	AHMIInfoClass AHMIInfo;
};

#endif

#endif

