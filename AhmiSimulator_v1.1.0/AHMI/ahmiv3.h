////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     ahmiv3.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by Arthas(周钰致)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////
#include "AHMICfgDefinition.h"
#ifdef PC_SIM
#include "stdafx.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#endif
#include "publicType.h"
#include "publicDefine.h"
#include "ahmiv3_vd.h"
#define __PRINTDEBUG__ 0
#ifdef PC_SIM
extern"C"
{
extern struct globalArgs_t globalArgs;
}
extern u16 gScreenWidth;
extern u16 gScreenHeight;
extern u8           TextureBuffer[TEXBUFSIZE];

typedef struct _Matrix{
	short A;
	short B;
	short C;
	short D;
	short E;
	short F;
} Matrix;

typedef u8 StructFrameBuffer;

typedef StructFrameBuffer *FramBufferPtr;

typedef struct AHMI_Color{
		u8 r;
		u8 g;
		u8 b;
		u8 a;
		Mask m;
		} Color;


typedef struct AHMI_Tile{
	u16 x;  //coordinate of left-bottom of the tile 
	u16 y;  //coordinate of left-bottom of the tile
	Color ColorBuffer[TILESIZE * TILESIZE];//for color
	Alpha AlphaBuffer[TILESIZE * TILESIZE];//for alpha
	bool mask_usage;//是否使用mask和alpha
	bool color_done;
	}Tile;

typedef struct _FinalColor{
		u8 r;
		u8 g;
		u8 b;
		} FinalColor;

typedef struct _FinalTile {
	u16 x;
	u16 y;
	FinalColor TileBuffer[TILESIZE* TILESIZE];
	} FinalTile;

typedef struct _SourceBuffer
{
	u16 header;
	U64 Matrix;
	u32 BoxSize;
	u32 TexSize;
	u32 AddrType;
} SourceBuffer;
//const int ScreenWidth;
//const int ScreenHeight;

//Color16 FrameBuffer[ScreenWidth* ScreenHeight];

class ahmi{
	public:	ahmi(int screen_width,int screen_height);
			//void DrawFrame(U64* TextureIndex ,ROMInfo* ,Color16* FrameBuffer);//绘制帧
			void DrawFrame(FramBufferPtr FrameBuffer,u8 flag,u32 StartAddr);
	private:
		// void TileInfoRead(U64* TextureIndex);   //读取tileinfo并计算tile的x，y	
			void TexRaster(Tile* ,Matrix* ,u32 addr ,u16 tex_width,u16 tex_heigth,u8 texture_ctrl,u32 InitColor,u8 MaskType);//纹理光栅化
			void InitTile(Tile*,u16 x,u16 y);
			void cache(U64 block_addr, u16 in_block_addr, u32 addr, unsigned int* texel, u8 texture_ctrl);//纹理读取 modified by darydou
			void InitFinalTile(FinalTile*,u16 x,u16 y);
			void color_blending(FinalTile* ,Tile*);//颜色混合
			void Write_Frame(FinalTile* ,FramBufferPtr FrameBuffer, u8 frameType = 0, u8 bufferWidth = 0);//将tile最终颜色写入frame buffer	
			void Write_back_finaltile(FinalTile *finaltile, FramBufferPtr FrameBuffer);//add by darydou writebackframe to finaltile
			int screen_height;//屏幕高
			int screen_width;//屏幕宽
			int tile_x;
			int tile_y;
			FinalTile theFinalTile;//tile最终结果
			//u32 StartAddress;
			FramBufferPtr pFrameBuffer;
#ifdef TEX_RAS_LOG_GEN
			std::ofstream texRasFile;
#endif
#ifdef COLOR_BLENDING_LOG_GEN
			std::ofstream colorBlendingFile;
#endif
#ifdef TILE_BUFFER_LOG_GEN
			std::ofstream tileBufferFile;
#endif
#ifdef UV_LOG_GEN
			std::ofstream uvLogFile;
#endif
};

int start();
int HWSimulatorThread(u8 flag);
#endif

