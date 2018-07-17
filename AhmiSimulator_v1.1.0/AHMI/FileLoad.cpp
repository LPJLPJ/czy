////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     FileLoad.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.01 - modify the LoadPage function by Arthas（周钰致）
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include <stdio.h>
#include "trace.h"

#ifdef EMBEDDED
#include "spi_if.h"
#endif

#define   BUFFERSIZE  1024

extern ConfigInfoClass		ConfigData;
extern char* cfgString;


#ifdef PC_SIM

char  AHMIFileName[200];
FILE *fp;
FILE *TexOdd;
FILE *TexEven;
FILE *TexVideo;
extern u8       VideoTextureBuffer[VIDEO_BUF_SIZE];
//DynamicPageClassPtr newPage;


//-----------------------------
// 函数名： OpenACFFile
// 读取acf文件
// 参数列表：
//    void
// 备注(各个版本之间的修改):
//   2.02版本： 增加读文件为两个，分别奇偶对齐
//-----------------------------
funcStatus OpenACFFile()
{
	errno_t err;
	err = fopen_s(&fp,AHMI_CFG_FILE_NAME,"rb");
	if(err != 0)
	{
		ERROR_PRINT("Cannot AHMI Source Open File.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	err = fopen_s(&TexOdd,AHMI_TEX_ODD_NAME,"rb");
	if(err != 0)
	{
		ERROR_PRINT("Cannot Open TexOdd File.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	err = fopen_s(&TexEven,AHMI_TEX_EVEN_NAME,"rb");
	if(err != 0)
	{
		ERROR_PRINT("Cannot TextureEven Open File.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	err = fopen_s(&TexVideo,AHMI_TEX_VIDEO_NAME,"rb");
	if(err != 0)
	{
		ERROR_PRINT("Cannot TexVideo Open File.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： LoadConfigData
// 读取配置信息
// 参数列表：
//    ConfigInfo* mConfigInfo    输出的配置信息
// 备注(各个版本之间的修改):
//   直接从文件读取sizeof(ConfigInfo)长度的数据即可
//   2.02版本： 增加了版本号，acf文件和工程的版本号会进行比对
//-----------------------------
int LoadConfigData(ConfigInfoClass* mConfigInfo)
{
	size_t count;
	char text[100];
	count = fread_s(mConfigInfo,sizeof(ConfigInfoClass),sizeof(ConfigInfoClass),1,fp);
	if(count != 1)
	{
		ERROR_PRINT("This is not a AHMI Source File.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	if(mConfigInfo->MagicNumber!= 0x494d4841)
	{
		ERROR_PRINT("This is not a AHMI Source File.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	WriteText(text,"The version of current file is %04x. The version of current project is %04x.\r\n",mConfigInfo->AHMIInfo.Version,VERSION);
	ERROR_PRINT(text);
	if(mConfigInfo->AHMIInfo.Version != (u16)VERSION)
	{
		ERROR_PRINT("The version of the file connot match the version of the project!!!\r\n");
		return AHMI_FUNC_FAILURE;
	}
	WriteText(text,"%d Pages in this File.\r\n%d Timers in this File.\r\n%d Tags in this File.\r\n",mConfigInfo->NumofPages,mConfigInfo->NumofTimers,mConfigInfo->NumofTags);
	ERROR_PRINT(text);
	WriteText(text,"Total Texture Size is %d.\r\nDynamic Page Space Size is %d.\r\n",mConfigInfo->TotalTextureSize,mConfigInfo->DynamicPageSize);
	ERROR_PRINT(text);


	//version information
	cfgString = (char*)pvPortMalloc(sizeof(char)* ConfigData.CfgStringSize);
	//load config string
	count = fread_s(cfgString,sizeof(char)*ConfigData.CfgStringSize,sizeof(char)*ConfigData.CfgStringSize,1,fp);
	if(count != 1)
	{
		ERROR_PRINT("connot read config string.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	ERROR_PRINT(cfgString);
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// 函数名： ReadPageDataFromFile
// 从文件读取页面信息
// 参数列表：
//    返回DynamicPageCfg* 返回值是页面信息
// 备注(各个版本之间的修改):
//-----------------------------
funcStatus ReadPageDataFromFile(DynamicPageCfgClass* curPage)
{
	size_t count;
	//just for debug
	//char debug;
	//for(int i = 0; i < sizeof(DynamicPageCfgClass) * 4; i++)
	//{
	//	fread_s(&debug,sizeof(char),sizeof(char),1,fp);
	//}
	count = fread_s(curPage,sizeof(DynamicPageCfgClass),sizeof(DynamicPageCfgClass),1,fp);
	if(count != 1)
	{
		ERROR_PRINT("There is no enough data to load dynamic page data.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// 函数名： ReadTagDataFromFile
// 从文件读取tag信息
// 参数列表：
//    @return TagClassPtr 返回tag信息
// 备注(各个版本之间的修改):
//-----------------------------
funcStatus ReadTagDataFromFile(TagClassPtr pTag, u8 ID)
{
	size_t count;
	TagClassCfg newTagCfg;
	WidgetLinkDataPtr pWidgetLinkData;
	PageLinkDataPtr pPageLinkData;
	CanvasLinkDataPtr pCanvasLinkData;
	u32 size;

	//read config
	count = fread_s(&newTagCfg,sizeof(TagClassCfg),sizeof(TagClassCfg),1,fp);
	if(count != 1)
	{
		ERROR_PRINT("There is no enough data to load tag Configuration data.\r\n");
		return AHMI_FUNC_FAILURE;
	}

	size = sizeof(struct WidgetLinkData) * newTagCfg.NumOfWidgetLinker;
	pWidgetLinkData =(WidgetLinkDataPtr) pvPortMalloc( size );
	
	count = fread_s(pWidgetLinkData,size,sizeof(struct WidgetLinkData),newTagCfg.NumOfWidgetLinker,fp);
	if(count != newTagCfg.NumOfWidgetLinker)
	{
		ERROR_PRINT("There is no enough data to load tag data of widgetlinkdata.\r\n");
		return AHMI_FUNC_FAILURE;
	}

	size = sizeof(struct PageLinkData) * newTagCfg.NumOfDynamicPageAction;
	pPageLinkData =(PageLinkDataPtr) pvPortMalloc( size );
	
	count = fread_s(pPageLinkData,size,sizeof(struct PageLinkData),newTagCfg.NumOfDynamicPageAction,fp);
	if(count != newTagCfg.NumOfDynamicPageAction)
	{
		ERROR_PRINT("There is no enough data to load tag data of PageLinkData.\r\n");
		return AHMI_FUNC_FAILURE;
	}

	size = sizeof(struct CanvasLinkData) * newTagCfg.NumOfCanvasAction;
	pCanvasLinkData =(CanvasLinkDataPtr) pvPortMalloc( size );
	
	count = fread_s(pCanvasLinkData,size,sizeof(struct CanvasLinkData),newTagCfg.NumOfCanvasAction,fp);
	if(count != newTagCfg.NumOfCanvasAction)
	{
		ERROR_PRINT("There is no enough data to load tag data of PageLinkData.\r\n");
		return AHMI_FUNC_FAILURE;
	}

	pTag->initTag(
		newTagCfg.NumOfWidgetLinker,
		newTagCfg.NumOfCanvasAction,
		newTagCfg.NumOfDynamicPageAction,
		ID,
		newTagCfg.mTagType,
		newTagCfg.mRegID,
		newTagCfg.mValue,
		pWidgetLinkData,
		pPageLinkData,
		pCanvasLinkData
	);

	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： ReadTagStringList
// 从文件读取支持字符串的tag的链表信息
// 参数列表：
//    @return ptr 读取后写入的地址空间
//	  @return number 读取的数据数量
// 备注(各个版本之间的修改):
//-----------------------------
void ReadTagStringList(u8* ptr, u16 number)
{
	size_t count;
	if((ptr == NULL) || (number == NULL))
		return;
	count = fread_s(ptr,sizeof(u8) * number,sizeof(u8),number,fp);
	if(count != 1)
	{
		ERROR_PRINT("There is no enough data to load tag Configuration data.\r\n");
		return ;
	}
}
//-----------------------------
// 函数名： ReadInitString
// 读取初始化字符串
// 参数列表：
//    @return ptr 读取后写入的地址空间
//	  @return number 读取的数据数量
// 备注(各个版本之间的修改):
//-----------------------------
void ReadInitString(u8* ptr, u16 number)
{
	size_t count;
	if((ptr == NULL) || (number == NULL))
		return;
	count = fread_s(ptr,sizeof(u8)*number,sizeof(u8),number,fp);
	if(count != 1)
	{
		ERROR_PRINT("There is no enough data to load tag Configuration data.\r\n");
		return ;
	}
}



//-----------------------------
// 函数名： LoadTextureToSDRAM
// 从文件读取纹理信息
// 参数列表：
//    @param1 void* INbuffer 输入的buffer地址
//    @param2 u32   bufsize  buffer长度
//    @param3 int   size     读取的长度
// 备注(各个版本之间的修改):
//-----------------------------
void LoadTextureToSDRAM	(void* INbuffer,u32 bufsize, int size)
{
	size_t count = 0;
	int i;
	int videoSize = 720*576*2;
//	int i;
	//int offset = ConfigData.TotalSize - ConfigData.TotalTextureSize;
	//fseek(fp,offset,SEEK_SET);
	//count = fread_s(INbuffer,bufsize,1,size,fp);
	for(i = 0; i < size; i++)
	{
		if(i%2 == 0)
			count += fread_s((char *)INbuffer + i,bufsize,sizeof(unsigned char),1,TexEven);
		else if(i % 2 == 1)
			count += fread_s((char *)INbuffer + i,bufsize,sizeof(unsigned char),1,TexOdd);
	}

	//read the video texture from file
	fread_s((char *)VideoTextureBuffer, videoSize,1,videoSize,TexVideo);


#ifdef TEST_VECTOR_GEN
	FILE *fp1,*fp2,*fp3;
	if(count!=size)
	{
		ERROR_PRINT("There is no enough data to load texture.\r\n");
	}

		fp1 = fopen("spidata.hex","w");
	fp2 = fopen("spidata_even.hex", "w");
	fp3 = fopen("spidata_odd.hex", "w");
	for(i=0;i<size;i++)
	{
		fprintf(fp1,"%02X\n",((u8*)INbuffer)[i]);
		if(i %2 == 0)
			fprintf(fp2,"%02X\n",((u8*)INbuffer)[i]);
		else if(i % 2 == 1)
			fprintf(fp3,"%02X\n",((u8*)INbuffer)[i]);
	}
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
#endif

}
#endif


#ifdef EMBEDDED


funcStatus LoadConfigData(ConfigInfoClass* mConfigInfo)
{
	uint32_t Address;
	ReadDataFromSTMFlash(mConfigInfo,sizeof(ConfigInfoClass));
	
	if(mConfigInfo->MagicNumber!= 0x494d4841)
	{
		//printf("This is not a AHMI Source File.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	//printf("The version of current file is %04x. The version of current project is %04x.\r\n",mConfigInfo->AHMIInfo.Version,VERSION);

	if(mConfigInfo->AHMIInfo.Version != (u16)VERSION)
	{
	/*	printf("The version of the file connot match the version of the project!!!\r\n");*/
		return AHMI_FUNC_FAILURE;
	}

	//version information
	cfgString = (char*)pvPortMalloc(sizeof(char)* ConfigData.CfgStringSize);
	//load config string
	ReadDataFromSTMFlash(cfgString, sizeof(char)* ConfigData.CfgStringSize);
	if(cfgString == NULL)
	{
		ERROR_PRINT("connot read config string.\r\n");
		return AHMI_FUNC_FAILURE;
	}
//	/*******************Send texture check sum data*****************/
//	Address = BANK_WRITE_START_ADDR + mConfigInfo->TotalSize;
//	
//	SPI_FPGA_Burst_Send_CheckSum_data(0, (u8*)Address, 4096);  //4096 bytes
//	/**************************************************************/
	
	return AHMI_FUNC_SUCCESS;
}



funcStatus ReadPageData(DynamicPageCfgClass* curPage)
{
	ReadDataFromSTMFlash(curPage,sizeof(DynamicPageCfgClass));
	return AHMI_FUNC_SUCCESS;
}



funcStatus ReadTagData(TagClassPtr pTag,u8 ID)
{

	TagClassCfg			newTagCfg;
	WidgetLinkDataPtr	pWidgetLinkData;
	PageLinkDataPtr		pPageLinkData;
	CanvasLinkDataPtr	pCanvasLinkData;
	u32					size;

	ReadDataFromSTMFlash(&newTagCfg,sizeof(TagClassCfg));

	size = sizeof(struct WidgetLinkData) * newTagCfg.NumOfWidgetLinker;
	pWidgetLinkData =(WidgetLinkDataPtr) pvPortMalloc( size );
	ReadDataFromSTMFlash(pWidgetLinkData,size);

	size = sizeof(struct PageLinkData) * newTagCfg.NumOfDynamicPageAction;
	pPageLinkData =(PageLinkDataPtr) pvPortMalloc( size );
	ReadDataFromSTMFlash(pPageLinkData,size);

	size = sizeof(struct CanvasLinkData) * newTagCfg.NumOfCanvasAction;
	pCanvasLinkData =(CanvasLinkDataPtr) pvPortMalloc( size );
	ReadDataFromSTMFlash(pCanvasLinkData,size);


	pTag->initTag(
		newTagCfg.NumOfWidgetLinker,
		newTagCfg.NumOfCanvasAction,
		newTagCfg.NumOfDynamicPageAction,
		ID,
		newTagCfg.mTagType,
		newTagCfg.mRegID,
		newTagCfg.mValue,
		pWidgetLinkData,
		pPageLinkData,
		pCanvasLinkData
	);

	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： ReadTagStringList
// 从文件读取支持字符串的tag的链表信息
// 参数列表：
//    @return ptr 读取后写入的地址空间
//	  @return number 读取的数据数量
// 备注(各个版本之间的修改):
//-----------------------------
void ReadTagStringList(u8* ptr, u16 number)
{
	if((ptr == NULL) || (number == NULL))
		return;
	ReadDataFromSTMFlash(ptr,number);
}
//-----------------------------
// 函数名： ReadInitString
// 读取初始化字符串
// 参数列表：
//    @return ptr 读取后写入的地址空间
//	  @return number 读取的数据数量
// 备注(各个版本之间的修改):
//-----------------------------
void ReadInitString(u8* ptr, u16 number)
{
	if((ptr == NULL) || (number == NULL))
		return;
	ReadDataFromSTMFlash(ptr,number);
}

#endif
