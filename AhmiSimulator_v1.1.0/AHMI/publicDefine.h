////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    this file only contains the definition that will used inlcuded by all core files.
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef PUBLICDEFINE__H
#define PUBLICDEFINE__H

#include "publicType.h"
#include "AHMICfgDefinition.h"
#ifdef EMBEDDED
#include "AHMIUserDefine.h"
#endif

#ifdef PC_SIM
#define VERSION (0x0104) //current version in 16.16
#define MINOR_VERSION (0x03) //current minor version 
#endif

#ifdef IMPLEMENTATION
#define AHMI_CFG_FILE_NAME ("ConfigData.acf")
#define AHMI_TEX_EVEN_NAME ("TextureEven.acf")
#define AHMI_TEX_ODD_NAME ("TextureOdd.acf")
#define AHMI_TEX_VIDEO_NAME ("TextureVideo.acf")
#else
#define AHMI_CFG_FILE_NAME ("..\\..\\testData_0304\\ConfigData.acf")
#define AHMI_TEX_EVEN_NAME ("..\\..\\testData_0304\\TextureEven.acf")
#define AHMI_TEX_ODD_NAME ("..\\..\\testData_0304\\TextureOdd.acf")
#define AHMI_TEX_VIDEO_NAME ("..\\..\\testData_0304\\TextureVideo.acf")
#endif

#define TILESIZE 32

#define AHMI_FUNC_SUCCESS 1 //函数执行成功
#define AHMI_FUNC_FAILURE 0 //函数执行失败

#define     SIM_DXT1      0
#define     SIM_DXT3      1
#define     SIM_ALPHA8    10
#define     SIM_ALPHA1    2
#define     SIM_ALPHA2    9   //20160613
#define     SIM_ALPHA4    11  //20160613
#define     SIM_PURECOLOR 3
#define     SIM_RGB565    4
#define     SIM_RGBA8888  5
#define     SIM_Y8        6
#define     SIM_YUV422    7
#define     SIM_PUREMASK  8
#define     TilePixelSize  (TILESIZE*TILESIZE)
#define     SIM_DXT5      14

#define     U64 unsigned long long
#define 	Alpha 	u8
#define 	Mask 	u8
#define 	BitMask 	u8
#define     Color16  u16


//screen 
extern u16 gScreenWidth;
extern u16 gScreenHeight;
//#ifdef PC_SIM
#define     TILE_NUM_X  ((gScreenWidth + TILESIZE-1)/TILESIZE)//((globalArgs.ScreenWidth + TILESIZE-1)/TILESIZE)
#define     TILE_NUM_Y  ((gScreenHeight + TILESIZE-1)/TILESIZE)//((globalArgs.ScreenHeigth + TILESIZE-1)/TILESIZE)
//#endif
#ifdef EMBEDDED
#define SCREEN_WIDTH_CFG 800
#define SCREEN_HEIGHT_CFG 480
#define TILE_NUM_X_CFG  ((SCREEN_WIDTH_CFG + TILESIZE-1)/TILESIZE)
#define TILE_NUM_Y_CFG  ((SCREEN_HEIGHT_CFG + TILESIZE-1)/TILESIZE)
#endif

#define 	SCREEN_WIDTH  gScreenWidth
#define 	SCREEN_HEIGHT gScreenHeight
//#define     TILE_NUM_X  //((SCREEN_WIDTH + TILESIZE-1)/TILESIZE)
//#define     TILE_NUM_Y  //((SCREEN_HEIGHT + TILESIZE-1)/TILESIZE)
#ifdef EMBEDDED
#define		STARTOFMEMORY	0x20000000
#define		ENDOFMEMORY		0x20010000      //内存大小为64K
#endif

#define     SCREEN_SIZE ((TILE_NUM_X) * (TILE_NUM_Y) * TilePixelSize )
#define     MAX_SCREEN_SIZE (1920) * (1080) * 3

static  U64 Mul_count = 0;
#define MUL(a,b) (Mul_count++,(U64)((U64)(a)*(U64)(b))) 

#define TRUE 1
#define FALSE 0

#define  SRCBUFSIZE  2048
#ifdef PC_SIM
#define  TEXBUFSIZE  1024*1024*512
#define  VIDEO_BUF_SIZE 720*576*2
#endif

typedef u8 funcStatus;

#endif
