////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     ahmiv3.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by ...
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////


// ahmiv2.cpp : 定义控制台应用程序的入口点。
//


#include "stdafx.h"
#include "ahmiv3.h"
#include "publicInclude.h"
#include <windows.h>
#include <iomanip>
#include <string>
#include <sstream>
#include "trace.h"

#ifdef PC_SIM

extern u8		 stm32info[11];
extern u8       VideoTextureBuffer[VIDEO_BUF_SIZE];

#define SCREEN_ROTATE 0

u32 StartAddress=0;

extern u32 TileRenderCounter;
extern u32 FrameRate;
extern u8  sourceBuffer[2048];
extern u8  GlobalBackgroundBuffer[ MAX_SCREEN_SIZE *2 ]; //used for background texture

extern StructFrameBuffer  GlobalFrameBuffer2[ MAX_SCREEN_SIZE *2]; //used for double buffer

#define DXT5_DIV 3 //DXT5缩减的精度

//-----------------------------
// 函数名： SetWriteBuffer
// 设置绘图地址
// 参数列表：
// @param1 u32 BufferID buffer编号
// 备注(各个版本之间的修改):
//   目前没有用上
//-----------------------------
void SetWriteBuffer(u32 BufferID)
{
	StartAddress = BufferID * 2* 1048576;
}

//-----------------------------
// 函数名： ahmi::ahmi
// AHMI构造函数
// 参数列表：
// @param1 int width  屏幕宽度
// @param2 int height 屏幕高度
// 备注(各个版本之间的修改):
//   无
//-----------------------------
ahmi::ahmi(int width, int height)
{
	screen_height = height;
	screen_width = width;
#ifdef TEST_LOG_GEN
	RemoveDirectory(_T("testLog"));
	CreateDirectory(_T("testLog"),NULL);
#ifdef TEX_RAS_LOG_GEN
	texRasFile.open("testLog\\texRas.log",std::ios::out);
#endif
#ifdef COLOR_BLENDING_LOG_GEN
	colorBlendingFile.open("testLog\\colorBlending.log",std::ios::out);
#endif
#ifdef TILE_BUFFER_LOG_GEN
	tileBufferFile.open("testLog\\tileBuffer.log",std::ios::out);
#endif
#ifdef UV_LOG_GEN
	uvLogFile.open("testLog\\uv.log",std::ios::out);
#endif
#endif
}

//-----------------------------
// 函数名： ahmi::ahmi
// AHMI构造函数
// 参数列表：
// @param1 Tile* tile 当前tile指针
// @param2 u16 x      横坐标
// @param3 u16 y      纵坐标
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void ahmi::InitTile(Tile* tile,u16 x,u16 y)
{
	int i=0;
	tile->x=x;
	tile->y=y;
}

//-----------------------------
// 函数名： InitFinalTile
// 初始化finalTile
// 参数列表：
// @param1 FinalTile* final_tile
// @param2 u16 x      横坐标
// @param3 u16 y      纵坐标
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void ahmi::InitFinalTile(FinalTile* final_tile,u16 x,u16 y)
{
	int i=0;
	for (i = 0; i<TilePixelSize; i++)
	{
		final_tile->x=x;
		final_tile->y=y;
		final_tile->TileBuffer[i].r=0;
		final_tile->TileBuffer[i].g=0;
		final_tile->TileBuffer[i].b=0;
	}
}

//-----------------------------
// 函数名： color_blending
// 颜色混合
// 参数列表：
// @param1 FinalTile* final_tile
// @param2 Tile* tile
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void ahmi::color_blending(FinalTile* final_tile,Tile* tile)
{
	int alpha;
	for(int i=0;i<TilePixelSize;i++)
	{
		if(tile->ColorBuffer[i].m)
		{
			if(tile->mask_usage) alpha = (tile->ColorBuffer[i].a*(tile->AlphaBuffer[i]+1))>>8;//纹理自带alpha与外部alpha混合
			else                 alpha = tile->ColorBuffer[i].a;

			final_tile->TileBuffer[i].r=(tile->ColorBuffer[i].r*alpha + final_tile->TileBuffer[i].r*(255-alpha)+128)/256;//四舍五入 modify by xt 20150401
			final_tile->TileBuffer[i].g=(tile->ColorBuffer[i].g*alpha + final_tile->TileBuffer[i].g*(255-alpha)+128)/256;
			final_tile->TileBuffer[i].b=(tile->ColorBuffer[i].b*alpha + final_tile->TileBuffer[i].b*(255-alpha)+128)/256;				
		}
	}

}

//-----------------------------
// 函数名： ahmi::cache
// 读取纹理数据
// 参数列表：
// @param1 U64 block_addr 块地址
// @param2 u16 in_block_addr 块内地址
// @param3 unsigned int * texel 纹理块
// @param4 u8 TexType 纹理类型
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void ahmi::cache(U64 block_addr,u16 in_block_addr,u32 addr,unsigned int * texel,u8 TexType)
{
	U64 tex_out;
	U64 alpha_out;
	Color color0,color1,color2,color3;
	u16 BColor1;
	u16 BColor2;
	u8 alpha;
	u8 alpha_temp;
	u8 alpha0;
	u8 alpha1;
	U64 index_temp;
	u8 index;

	if((addr & START_ADDR_OF_RAM) == 0) //指向spi flash地址
		tex_out = *(U64*)((u32)TextureBuffer +addr + block_addr*8);
	else //指向 sdram地址
	{
		if(addr >= START_ADDR_OF_RAM + 0x1a80000)
			tex_out = *(U64*)(GlobalBackgroundBuffer +(addr- (START_ADDR_OF_RAM + 0x1a80000)) + block_addr*8);
		else 
			tex_out =  *(U64*)(VideoTextureBuffer +(addr-START_ADDR_OF_RAM) + block_addr*8);;
	}

	switch (TexType)
	{
	case(SIM_DXT1):
		BColor1=(tex_out&0xFFFF);
		BColor2=(tex_out&0xFFFF0000)>>16;
		index_temp=(tex_out&0xFFFFFFFF00000000)>>32;
		index=(index_temp>>(2*in_block_addr))%4;
		color0.r=(BColor1&0xF800)>>8;
		color0.g=(BColor1&0x07E0)>>3;
		color0.b=(BColor1&0x001F)<<3;
		color0.m=1;
		color1.r=(BColor2&0xF800)>>8;
		color1.g=(BColor2&0x07E0)>>3;
		color1.b=(BColor2&0x001F)<<3;
		color1.m=1;
		color2.r = ( color0.r * 3 + color1.r * 1 ) / 4;
        color2.g = ( color0.g * 3 + color1.g * 1 ) / 4;
        color2.b = ( color0.b * 3 + color1.b * 1 ) / 4;
		color2.m = 1;
        color3.r = ( color0.r*1 + color1.r * 3 ) / 4;
        color3.g = ( color0.g*1 + color1.g * 3 ) / 4;
        color3.b = ( color0.b*1 + color1.b * 3 ) / 4;
		color3.m = 1;
		switch (index)
			{
			case 0:
				*texel=((color0.r)<<16)+((color0.g)<<8)+color0.b;
				break;					
			case 1:						
				*texel=((color1.r)<<16)+((color1.g)<<8)+color1.b;
				break;					
			case 2:						
				*texel=((color2.r)<<16)+((color2.g)<<8)+color2.b;
				break;	
			case 3:
				*texel=((color3.r)<<16)+((color3.g)<<8)+color3.b;
				break;
			}
	break;
	case(SIM_DXT3):
			alpha_out=tex_out;
			tex_out = *(U64*)((u32)TextureBuffer +addr + block_addr*8 + 8);
			BColor1=(tex_out&0xFFFF);
			BColor2=(tex_out&0xFFFF0000)>>16;
			index_temp=(tex_out&0xFFFFFFFF00000000)>>32;
			index=(index_temp>>(2*in_block_addr))%4;
			color0.r=(BColor1&0xF800)>>8;
			color0.g=(BColor1&0x07E0)>>3;
			color0.b=(BColor1&0x001F)<<3;
			color0.m=1;
			color1.r=(BColor2&0xF800)>>8;
			color1.g=(BColor2&0x07E0)>>3;
			color1.b=(BColor2&0x001F)<<3;
			color1.m=1;
			color2.r = ( color0.r * 3 + color1.r * 1 ) / 4;
			color2.g = ( color0.g * 3 + color1.g * 1 ) / 4;
			color2.b = ( color0.b * 3 + color1.b * 1 ) / 4;
			color2.m = 1;
			color3.r = ( color0.r*1 + color1.r * 3 ) / 4;
			color3.g = ( color0.g*1 + color1.g * 3 ) / 4;
			color3.b = ( color0.b*1 + color1.b * 3 ) / 4;
			color3.m = 1;
			alpha = (alpha_out>>(4*in_block_addr))%16;
			alpha = alpha*16 + alpha;
			switch (index)
				{
				case 0:
					*texel=((color0.r)<<24)+((color0.g)<<16)+(color0.b<<8)+alpha;
					break;					
				case 1:						
					*texel=((color1.r)<<24)+((color1.g)<<16)+(color1.b<<8)+alpha;
					break;					
				case 2:						
					*texel=((color2.r)<<24)+((color2.g)<<16)+(color2.b<<8)+alpha;
					break;				
				case 3:					
					*texel=((color3.r)<<24)+((color3.g)<<16)+(color3.b<<8)+alpha;

				}

	break;

	case(SIM_DXT5):
		alpha_out=tex_out;
		alpha0 = alpha_out & 0xFF;          //第一字节，alpha0
		alpha1 = (alpha_out & 0xFF00) >> 8; //第二字节，alpha1
		index_temp=(alpha_out&0xFFFFFFFFFFFF0000)>>16; //3-8 index
		index=(index_temp>>(3*in_block_addr))%8;
		if(alpha0 > alpha1) //模拟用256精度来计算
		{
			switch(index)
			{
			case(0):
				alpha = alpha0;
				alpha_temp = alpha0;
				break;
			case(1):
				alpha = alpha1;
				alpha_temp = alpha1;
				break;
			case(2):
				alpha = (( (219 >> DXT5_DIV) * (alpha0 >> DXT5_DIV) ) + ( (37 >> DXT5_DIV) * (alpha1 >> DXT5_DIV) )) >> (8 - 2*DXT5_DIV); //alpha_2 = 6/7*alpha_0 + 1/7*alpha_1;
				alpha_temp = ((6 * alpha0) /7) + ( (1*alpha1) / 7 );
				break;
			case(3):
				alpha = (( (183 >> DXT5_DIV) * (alpha0>>DXT5_DIV) ) + ( (73>>DXT5_DIV) * (alpha1>>DXT5_DIV) )) >> (8 - 2*DXT5_DIV) ; //alpha_3 = 5/7*alpha_0 + 2/7*alpha_1;
				alpha_temp = ((5 * alpha0) /7) + ( (2*alpha1) / 7 );
				break;
			case(4):
				alpha = (( (146>>DXT5_DIV) * (alpha0>>DXT5_DIV) ) + ( (110>>DXT5_DIV) * (alpha1>>DXT5_DIV) )) >> (8 - 2*DXT5_DIV) ; //alpha_4 = 4/7*alpha_0 + 3/7*alpha_1;
				alpha_temp = ((4 * alpha0) /7) + ( (3*alpha1) / 7 );
				break;
			case(5):
				alpha = (( (110 >>DXT5_DIV) * (alpha0>>DXT5_DIV) ) + ( (146>>DXT5_DIV)*(alpha1>>DXT5_DIV) )) >> (8 - 2*DXT5_DIV); //alpha_5 = 3/7*alpha_0 + 4/7*alpha_1;
				alpha_temp = ((3 * alpha0) /7) + ( (4*alpha1) / 7 );
				break;
			case(6):
				alpha = (( (73>>DXT5_DIV) * (alpha0>>DXT5_DIV)) + ((183>>DXT5_DIV)*(alpha1>>DXT5_DIV) )) >> (8 - 2*DXT5_DIV) ; //alpha_6 = 2/7*alpha_0 + 5/7*alpha_1;
				alpha_temp = ((2 * alpha0) /7) + ( (5*alpha1) / 7 );
				break;
			case(7):
				alpha = (( (37>>DXT5_DIV) * (alpha0>>DXT5_DIV) ) + ( (219>>DXT5_DIV)*(alpha1>>DXT5_DIV) ) ) >> (8 - 2*DXT5_DIV) ; //alpha_6 = 2/7*alpha_0 + 5/7*alpha_1;
				alpha_temp = ((1 * alpha0) /7) + ( (6*alpha1) / 7 );
				break;
			default:
				alpha = 0xff;
				alpha_temp = 0xff;
				break;
			}
		}
		else 
		{
			switch(index)
			{
			case(0):
				alpha = alpha0;
				alpha_temp = alpha0;
				break;
			case(1):
				alpha = alpha1;
				alpha_temp = alpha1;
				break;
			case(2):
				alpha = ( (( (205>>DXT5_DIV) * (alpha0>>DXT5_DIV) ) + ((51>>DXT5_DIV)*(alpha1>>DXT5_DIV) ) )>> (8 - 2*DXT5_DIV) ); //alpha_2 = 4/5*alpha_0 + 1/5*alpha_1;
				alpha_temp = ((4 * alpha0) /5) + ( (1*alpha1) / 5 );
				break;
			case(3):
				alpha = ( (( (154>>DXT5_DIV) * (alpha0>>DXT5_DIV)) + ((102>>DXT5_DIV)*(alpha1>>DXT5_DIV)) ) >> (8 - 2*DXT5_DIV) ); //alpha_3 = 3/5*alpha_0 + 2/5*alpha_1;
				alpha_temp = ((3 * alpha0) /5) + ( (2*alpha1) / 5 );
				break;
			case(4):
				alpha = ( (( (102>>DXT5_DIV) * (alpha0>>DXT5_DIV) ) + ( (154>>DXT5_DIV) * (alpha1>>DXT5_DIV) )) >> (8 - 2*DXT5_DIV) ); //alpha_4 = 2/5*alpha_0 + 3/5*alpha_1;
				alpha_temp = ((2 * alpha0) /5) + ( (3*alpha1) / 5 );
				break;
			case(5):
				alpha = ( (( (51>>DXT5_DIV) * (alpha0>>DXT5_DIV) ) +  ( (205>>DXT5_DIV) * (alpha1>>DXT5_DIV) )) >> (8 - 2*DXT5_DIV) ); //alpha_5 = 1/5*alpha_0 + 4/5*alpha_1;
				alpha_temp = ((1 * alpha0) /5) + ( (4*alpha1) / 5 );
				break;
			case(6):
				alpha = 0; 
				alpha_temp = 0;
				break;
			case(7):
				alpha = 255; 
				alpha_temp = 255;
				break;
			default:
				alpha = 0xff;
				alpha_temp = 255;
			}
		}

		tex_out = *(U64*)((u32)TextureBuffer +addr + block_addr*8 + 8);
		BColor1=(tex_out&0xFFFF);
		BColor2=(tex_out&0xFFFF0000)>>16;
		index_temp=(tex_out&0xFFFFFFFF00000000)>>32;
		index=(index_temp>>(2*in_block_addr))%4;
		color0.r= ((BColor1&0xF800)>>8) + ((BColor1 & 0xE000) >> 13);
		color0.g= ((BColor1&0x07E0)>>3) + ((BColor1 & 0x0600)>>9);
		color0.b= ((BColor1&0x001F)<<3) + ((BColor1&0x001C)>>2) ;
		color0.m=1;
		color1.r=((BColor2&0xF800)>>8) + ((BColor2 & 0xE000) >> 13);
		color1.g=((BColor2&0x07E0)>>3) + ((BColor2 & 0x0600) >> 9) ;
		color1.b=((BColor2&0x001F)<<3) + ((BColor2 & 0x001C )>> 2) ;
		color1.m=1;
		color2.r = ( color0.r * 3 + color1.r * 1 ) / 4;
		color2.g = ( color0.g * 3 + color1.g * 1 ) / 4;
		color2.b = ( color0.b * 3 + color1.b * 1 ) / 4;
		color2.m = 1;
		color3.r = ( color0.r*1 + color1.r * 3 ) / 4;
		color3.g = ( color0.g*1 + color1.g * 3 ) / 4;
		color3.b = ( color0.b*1 + color1.b * 3 ) / 4;
		color3.m = 1;
		switch (index)
		{
		case 0:
			*texel=((color0.r)<<24)+((color0.g)<<16)+(color0.b<<8)+alpha;
			break;					
		case 1:						
			*texel=((color1.r)<<24)+((color1.g)<<16)+(color1.b<<8)+alpha;
			break;					
		case 2:						
			*texel=((color2.r)<<24)+((color2.g)<<16)+(color2.b<<8)+alpha;
			break;				
		case 3:					
			*texel=((color3.r)<<24)+((color3.g)<<16)+(color3.b<<8)+alpha;

		}
	break;

	case(SIM_ALPHA8):
			*texel=(tex_out>>(in_block_addr*8))%256;

	break;

	case(SIM_RGB565):
		*texel = (tex_out >> ( in_block_addr * 16))%65536;
	break;

	case(SIM_RGBA8888):
		*texel = (tex_out >> (/*32 - */in_block_addr * 32)) & 0xffffffff;
	break;

	case(SIM_Y8):
		*texel=(tex_out>>(in_block_addr*8))%256;
	break;

	case(SIM_ALPHA1):
		*texel=(tex_out>>(in_block_addr))%2 ? 255 : 0;
	break;

	case(SIM_ALPHA2):
		*texel= ((tex_out>>(in_block_addr*2))%4) * 255 / 3;
		break;
	
	case(SIM_ALPHA4):
		*texel= ((tex_out>>(in_block_addr*4))%16) * 255 / 15;
		break;

	default:
		break;
	}
}

//-----------------------------
// 函数名： ahmi::TexRaster(
// 纹理光栅化
// 参数列表：
// @param1 Tile* tile      当前tile
// @param2 Matrix* matrix  纹理矩阵
// @param3 u32 addr        纹理地址
// @param4 u16 tex_width   纹理宽
// @param5 u16 tex_height  纹理高
// @param6 u8 texture_ctrl {4:1}纹理类型,{0}是否使用alpha
// @param7 u32 InitColor   颜色
// @param8 u8 MaskType     mask类型,0覆盖，1与，2或，3非
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void ahmi::TexRaster(Tile* tile,Matrix* matrix,u32 addr,u16 tex_width,u16 tex_height,u8 texture_ctrl,u32 InitColor,u8 MaskType)
{
	u8 TexType;
	int u0;
	int v0;
	//bool mask_usage;
	bool mask_alpha;
	int i = 0;
	U64 block_addr;
	u16 in_block_addr;
	int block_row;
	TexType = (texture_ctrl & 0x1e) >> 1;
	mask_alpha = (texture_ctrl&0x1);
	tile->mask_usage = mask_alpha;
	//u（x，y）=A(x+E) + C(y+F)；v（x，y）= B(x+E) + D(y+F)
	u0 = ((matrix->A/*1.6.9*/ * (tile->x+matrix->E)/*1.11.4*/ + matrix->C/*1.6.9*/ * (tile->y+matrix->F)/*1.11.4*/)>>5);//1.17.8
	v0 = ((matrix->B/*1.6.9*/ * (tile->x+matrix->E)/*1.11.4*/ + matrix->D/*1.6.9*/ * (tile->y+matrix->F)/*1.11.4*/)>>5);//1.17.8
	TileRenderCounter++;
	for(i=0;i<TilePixelSize;i++)
	{
		int u,v;
		u8 x,y;
		unsigned int alpha;
		unsigned int color;
		unsigned int Y;
		x=i%32;
		y=i/32;
		u=(u0+((matrix->A/*1.6.9*/ * x/*1.5.0*/ + matrix->C * y)>>1))/*1.17.8*/ >>8;//1.17.0
		v=(v0+((matrix->B/*1.6.9*/ * x/*1.5.0*/ + matrix->D * y)>>1))/*1.17.8*/ >>8;//1.17.0
#ifdef UV_LOG_GEN
		uvLogFile.fill('0');
		uvLogFile<< std::setw(8) << std::hex << (int)(u << 8) << " " << std::setw(8) << std::hex << (int)(v << 8) << std::endl;
#endif
		
		switch(TexType)
		{
		case(SIM_DXT1):
			block_row = tex_width/4+(((tex_width&0x3)==0)?0:1);
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				tile->ColorBuffer[i].r=0;
				tile->ColorBuffer[i].g=0;
				tile->ColorBuffer[i].b=0;
				tile->ColorBuffer[i].a=0;
				tile->ColorBuffer[i].m=0;
			}
			else
			{
				block_addr=u/4+v/4 * block_row;
				in_block_addr=(u&0x3)+((v&0x3)<<2);
				ahmi::cache(block_addr,in_block_addr,addr,&color,TexType);						
				tile->ColorBuffer[i].r=(color&0xFF0000) >> 16;
				tile->ColorBuffer[i].g=(color&0xFF00) >> 8;
				tile->ColorBuffer[i].b=(color&0xFF);
				tile->ColorBuffer[i].a=255;
				tile->ColorBuffer[i].m=1;
			 }	
			tile->color_done=1;
			break;

		case(SIM_DXT3):
		case(SIM_DXT5):
			block_row = tex_width/4+(((tex_width&0x3)==0)?0:1);
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				tile->ColorBuffer[i].r=0;
				tile->ColorBuffer[i].g=0;
				tile->ColorBuffer[i].b=0;
				tile->ColorBuffer[i].a=0;
				tile->ColorBuffer[i].m=0;
			}
			else
			{
				block_addr=(u/4+v/4 * block_row)*2;
				in_block_addr=(u&0x3)+((v&0x3)<<2);
				ahmi::cache(block_addr,in_block_addr,addr,&color,TexType);	
				tile->ColorBuffer[i].r=(color&0xFF000000) >> 24;
				tile->ColorBuffer[i].g=(color&0xFF0000) >> 16;
				tile->ColorBuffer[i].b=(color&0xFF00) >>8;
				tile->ColorBuffer[i].a=(color&0xFF);
				tile->ColorBuffer[i].m=1;
			}
			tile->color_done=1;
			break;

		case(SIM_ALPHA8):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=0;
					break;
				case(1):
					tile->AlphaBuffer[i]=0;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=0xff;
					break;
				}
			}
			else
			{
				block_addr=(u+v*tex_width)/8;
				in_block_addr=(u+v*tex_width)%8;
				ahmi::cache(block_addr,in_block_addr,addr,&alpha,TexType);
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=alpha;
					break;
				case(1)://与显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] & alpha;
					break;
				case(2)://或显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] | alpha;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=~alpha;
					break;
				}
				tile->color_done=0;
			}
			break;
		case(SIM_ALPHA1):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=0;
					break;
				case(1):
					tile->AlphaBuffer[i]=0;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=0xff;
					break;
				}
			}
			else
			{
				block_addr=(u+v*tex_width)/64;
				in_block_addr=(u+v*tex_width)%64;
				ahmi::cache(block_addr,in_block_addr,addr,&alpha,TexType);						
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=alpha;
					break;
				case(1)://与显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] & alpha;
					break;
				case(2)://或显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] | alpha;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=~alpha;
					break;
				}
				tile->color_done=0;
			 }
			break;
		case(SIM_ALPHA2):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=0;
					break;
				case(1):
					tile->AlphaBuffer[i]=0;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=0xff;
					break;
				}
			}
			else
			{
				block_addr=(u+v*tex_width)/32;
				in_block_addr=(u+v*tex_width)%32;
				ahmi::cache(block_addr,in_block_addr,addr,&alpha,TexType);						
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=alpha;
					break;
				case(1)://与显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] & alpha;
					break;
				case(2)://或显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] | alpha;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=~alpha;
					break;
				}
				tile->color_done=0;
			 }
			break;
		case(SIM_ALPHA4):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=0;
					break;
				case(1):
					tile->AlphaBuffer[i]=0;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=0xff;
					break;
				}
			}
			else
			{
				block_addr=(u+v*tex_width)/16;
				in_block_addr=(u+v*tex_width)%16;
				ahmi::cache(block_addr,in_block_addr,addr,&alpha,TexType);						
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=alpha;
					break;
				case(1)://与显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] & alpha;
					break;
				case(2)://或显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] | alpha;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=~alpha;
					break;
				}
				tile->color_done=0;
			 }
			break;
		case(SIM_PURECOLOR):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				tile->ColorBuffer[i].r=0;
				tile->ColorBuffer[i].g=0;
				tile->ColorBuffer[i].b=0;
				tile->ColorBuffer[i].a=0;
				tile->ColorBuffer[i].m=0;
			}
			else
			{
				tile->ColorBuffer[i].a=(InitColor&0xFF000000) >> 24;
				tile->ColorBuffer[i].r=(InitColor&0xFF0000) >> 16;
				tile->ColorBuffer[i].g=(InitColor&0xFF00) >> 8;
				tile->ColorBuffer[i].b=InitColor&0xFF;
				tile->ColorBuffer[i].m=1;
			}
			tile->color_done=1;
			break;
		case(SIM_RGB565):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				tile->ColorBuffer[i].r=0;
				tile->ColorBuffer[i].g=0;
				tile->ColorBuffer[i].b=0;
				tile->ColorBuffer[i].a=0;
				tile->ColorBuffer[i].m=0;
			}
			else
			{
				block_addr=(u+v*tex_width)/4;
				in_block_addr=(u+v*tex_width)%4;
				ahmi::cache(block_addr,in_block_addr,addr,&color,TexType);
				tile->ColorBuffer[i].r=(color&0xF800) >> 8;
				tile->ColorBuffer[i].g=(color&0x7E0) >> 3;
				tile->ColorBuffer[i].b=(color&0x1F) << 3;
				tile->ColorBuffer[i].a=255;
				tile->ColorBuffer[i].m=1;
				
			 }
			tile->color_done=1;
			break;
		case(SIM_RGBA8888):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				tile->ColorBuffer[i].r=0;
				tile->ColorBuffer[i].g=0;
				tile->ColorBuffer[i].b=0;
				tile->ColorBuffer[i].a=0;
				tile->ColorBuffer[i].m=0;
			}
			else
			{
				block_addr=(u+v*tex_width)/2;
				in_block_addr=(u+v*tex_width)%2;
				ahmi::cache(block_addr,in_block_addr,addr,&color,TexType);
				tile->ColorBuffer[i].a=(color&0xFF000000) >> 24;
				tile->ColorBuffer[i].r=(color&0xFF0000) >> 16;
				tile->ColorBuffer[i].g=(color&0xFF00) >> 8;
				tile->ColorBuffer[i].b=(color&0xFF);
				tile->ColorBuffer[i].m=1;					
			}
			tile->color_done=1;
			break;
		case(SIM_Y8):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				tile->ColorBuffer[i].r=0;
				tile->ColorBuffer[i].g=0;
				tile->ColorBuffer[i].b=0;
				tile->ColorBuffer[i].a=0;
				tile->ColorBuffer[i].m=0;
			}
			else
			{
				block_addr=(u+v*tex_width)/8;
				in_block_addr=(u+v*tex_width)%8;
				ahmi::cache(block_addr,in_block_addr,addr,&Y,TexType);
				tile->ColorBuffer[i].r=Y;
				tile->ColorBuffer[i].g=Y;
				tile->ColorBuffer[i].b=Y;
				tile->ColorBuffer[i].a=255;
				tile->ColorBuffer[i].m=1;
			}
			tile->color_done=1;
			break;
		case(SIM_PUREMASK):
			if((u<0)||(u>tex_width-1)||(v<0)||(v>tex_height-1))
			{
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=0;
					break;
				case(1):
					tile->AlphaBuffer[i]=0;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i]=0xff;
					break;
				}
			}
			else
			{
				switch(MaskType)
				{
				case(0)://覆盖显示
					tile->AlphaBuffer[i]=255;
					break;
				case(1)://与显示
					tile->AlphaBuffer[i] = tile->AlphaBuffer[i] & 255;
					break;
				case(2)://或显示
					tile->AlphaBuffer[i] = 255;
					break;
				case(3)://取非显示
					tile->AlphaBuffer[i] = 0;
					break;
				}
			 }
			tile->color_done=0;
		}
	}

}


/*
void ahmi::Write_Frame(FinalTile *finaltile,FramBufferPtr FrameBuffer)
{
   u32 i=0,j=0;
   u32 address;
   u32  x=(*finaltile).x>>4;
   u32  y=(*finaltile).y>>4;
   u16 color;
   	for (j=0;j<TILESIZE;j++)
   		for (i=0;i<TILESIZE;i++)
		{
			//if( (y+j >= globalArgs.ScreenHeigth) || (x+i >= globalArgs.ScreenWidth)) continue;
			//address=2*((globalArgs.ScreenHeigth-1-(y+j))*globalArgs.ScreenWidth+x+i) + StartAddress;
			address = 2*((y+j)*globalArgs.ScreenWidth+x+i)+ StartAddress;

			color = ((*finaltile).TileBuffer[j*32+i].r & 0xf8) << 8;
			color += ((*finaltile).TileBuffer[j*32+i].g & 0xfc) << 3;
			color += ((*finaltile).TileBuffer[j*32+i].b & 0xf8)>> 3;
			*(FrameBuffer+address) = color & 0xff;
			*(FrameBuffer+address+1) = color >> 8;

			//*(FrameBuffer+address) = (*finaltile).TileBuffer[j*32+i].r & 0xf8;
			//*(FrameBuffer+address+1) = (*finaltile).TileBuffer[j*32+i].g & 0xfc;
			//*(FrameBuffer+address+2) = (*finaltile).TileBuffer[j*32+i].b & 0xf8;
			//(FrameBuffer+address)->r=(*finaltile).TileBuffer[j*32+i].r;
			//(FrameBuffer+address)->g=(*finaltile).TileBuffer[j*32+i].g;
			//(FrameBuffer+address)->b=(*finaltile).TileBuffer[j*32+i].b;
		}
}*/

//-----------------------------
// 函数名： Write_Frame
// 写framebuffer
// 参数列表：
// @param1 FinalTile *finaltile       写回的tile
// @param2 FramBufferPtr FrameBuffer  framebuffer数组
// 备注(各个版本之间的修改):
// ...
//-----------------------------

void ahmi::Write_Frame(FinalTile *finaltile,FramBufferPtr FrameBuffer, u8 frameType, u8 bufferWidth)
{
   u32 i=0,j=0;
   u32 address;
   u32  x=(*finaltile).x>>4;
   u32  y=(*finaltile).y>>4;
   int  screen_rotate = SCREEN_ROTATE;
   int  screen_height;
   int  screen_width;
   int  x_temp;
   int  y_temp;
   u32  addr_write;
   addr_write = *( (u32*)(stm32info + addr_ahmi_0) );
#ifdef TILE_BUFFER_LOG_GEN
   tileBufferFile << " tile_x " << (int)x << " tile_y " << (int)y << std::endl;
#endif
if(addr_write == 0x1a80000) //write to static texture buffer
   {
		for (j=0;j<TILESIZE;j++)
			for (i=0;i<TILESIZE;i++)
			{
				if( (y+j >= globalArgs.ScreenHeigth) || (x+i >= globalArgs.ScreenWidth )) continue;
				address= 4 * ( (y+j)*globalArgs.ScreenWidth+x+i);
				//address=3*( (y+j) * (globalArgs.ScreenWidth)+ x + i);
				//*(FrameBuffer+address  ) = (*finaltile).TileBuffer[j*32+i].r & 0xf8;
				//*(FrameBuffer+address+1) = (*finaltile).TileBuffer[j*32+i].g & 0xfc;
				//*(FrameBuffer+address+2) = (*finaltile).TileBuffer[j*32+i].b & 0xf8;
				*(GlobalBackgroundBuffer+address+3) = 0xff;   //a
				*(GlobalBackgroundBuffer+address+2) = (*finaltile).TileBuffer[j*32+i].r;
				*(GlobalBackgroundBuffer+address+1) = (*finaltile).TileBuffer[j*32+i].g;
				*(GlobalBackgroundBuffer+address) = (*finaltile).TileBuffer[j*32+i].b;
			}
   }
   else if(frameType == 0)
   {
		for (j=0;j<TILESIZE;j++)
			for (i=0;i<TILESIZE;i++)
			{
				if(screen_rotate == 0)
				{
					screen_width = globalArgs.ScreenWidth;
					screen_height = globalArgs.ScreenHeigth;
					x_temp = x+i;
					y_temp = y + j;
				}
				else if(screen_rotate == 90)
				{
					screen_height = globalArgs.ScreenWidth;
					screen_width = globalArgs.ScreenHeigth;
					x_temp = y + j;
					y_temp =screen_height - (x + i) - 1;
				}
				if( (y_temp >= screen_height) || (x_temp >= screen_width)) continue;
				address=3*(( screen_height-1-(y_temp))*screen_width+x_temp);    //更改地址为左下角为坐标原点
				//address=3*( (y+j) * (globalArgs.ScreenWidth)+ x + i);
				*(FrameBuffer+address  ) = (*finaltile).TileBuffer[j*32+i].r & 0xf8;
				*(FrameBuffer+address+1) = (*finaltile).TileBuffer[j*32+i].g & 0xfc;
				*(FrameBuffer+address+2) = (*finaltile).TileBuffer[j*32+i].b & 0xf8;

#ifdef TILE_BUFFER_LOG_GEN
				u32 finalColor;	
				finalColor = (((*finaltile).TileBuffer[j*32+i].r & 0xf8) << 16) + ( (((*finaltile).TileBuffer[j*32+i].g & 0xfc)) << 8 ) + (((*finaltile).TileBuffer[j*32+i].b & 0xf8) );
				tileBufferFile <<  std::setw(6) << std::hex << finalColor << std::endl;
#endif
				//(FrameBuffer+address)->r=(*finaltile).TileBuffer[j*32+i].r;
				//(FrameBuffer+address)->g=(*finaltile).TileBuffer[j*32+i].g;
				//(FrameBuffer+address)->b=(*finaltile).TileBuffer[j*32+i].b;
			}
   }
   else 
   {
	   for (j=0;j<TILESIZE;j++)
			for (i=0;i<TILESIZE;i++)
			{
				if( (y+j >= globalArgs.ScreenHeigth) || (x+i >= globalArgs.ScreenWidth )) continue;
				address=2 * ( (y+j)*globalArgs.ScreenWidth+x+i);
				//address=3*( (y+j) * (globalArgs.ScreenWidth)+ x + i);
				*(u16*)( FrameBuffer + address) = (u16)(((*finaltile).TileBuffer[j*32+i].r & 0xf8) << 8);
				*(u16*)( FrameBuffer + address) += (u16)(((*finaltile).TileBuffer[j*32+i].g & 0xfc) << 3);
				*(u16*)( FrameBuffer + address) += (u16)(((*finaltile).TileBuffer[j*32+i].b & 0xfc) >> 3);
			}
   }
}

//-----------------------------
// 函数名： ahmi::DrawFrame
// 绘制一帧
// 参数列表：
// @param1 FramBufferPtr FrameBuffer  framebuffer数组
// 备注(各个版本之间的修改):
// ...
//-----------------------------
void ahmi::DrawFrame(FramBufferPtr FrameBuffer,u8 flag,u32 StartAddr = 0)//modified by darydou 
{
	pFrameBuffer = FrameBuffer;
	u16 tile_x = 0, tile_y = 0;
	u16 tile_x_num = 0, tile_y_num = 0;
	//for the sourcebuffer
	u8 headerfirst,headersecond;
	u8  SB_texType, SB_Mask, SB_Matrix, SB_AddrType,SB_End,SB_MaskType;
	u8  texCtrl;
	u8  start_x, start_y, stop_x, stop_y;
	u16 width, height;
	u32 addr = 0;
	u32 color = 0;
	u16 suffershift;
	//u32 addr_ahmi = ( (stm32info[Addr_combine]) * (ADDR_MULTI_BASE) );//Addr_combine[2:0] * 0x200000
	//u8 buffer_width = stm32info[BUFFER_WIDTH];
 	Tile uutTile;
	Matrix uutMatrix;
	int PRINTFLAG = 1;
	#if  __PRINTDEBUG__//add by darydou 3/4
	    int printcount = 0;
	    char textureformat[10];
	    char  buffer[200];
		ERROR_PRINT("The flash boxsize ,as follow:\r\n");
		WriteText(buffer,"tile_start_x is %d \r\n tile_end_x is %d \r\n tile_start_y is %d \r\n tile_end_y is %d \r\n",\
			stm32info[Tile_start_x],stm32info[Tile_end_x],stm32info[Tile_start_y],stm32info[Tile_end_y]);
		ERROR_PRINT(buffer);
	#endif
	for (tile_y_num = stm32info[Tile_start_y]; tile_y_num <= stm32info[Tile_end_y]; tile_y_num++)
		for (tile_x_num = stm32info[Tile_start_x]; tile_x_num <= stm32info[Tile_end_x]; tile_x_num++)
			if(tile_x_num >= 0 && tile_x_num <TILE_NUM_X && tile_y_num >= 0 && tile_y_num < TILE_NUM_Y)
			{

				//tile_x = tile_x_num << 4;//12.4定点数
				//tile_y = tile_y_num << 4;//12.4定点数
				tile_x = tile_x_num * TILESIZE * 16;//12.4定点数
				tile_y = tile_y_num * TILESIZE * 16;//12.4定点数
				InitTile(&uutTile, tile_x, tile_y);
				InitFinalTile(&theFinalTile, tile_x, tile_y);
				suffershift = 0;
				//if (flag == 1)
				//{//将已经刷新的图片作为新的纹理
				//	//由于最终存储的纹理为BMP888格式的，但AHMI没有处理BMP888格式
				//	SB_texType = 8; //1000
				//	SB_Mask = 0;
				//	SB_MaskType = 0;
				//	texCtrl = (SB_texType << 1) + SB_Mask;
				//	//modefied by xt 20150506
				//	//ABCD为1.6.9定点数 EF为12.4定点数，默认未做变化为单位矩阵
				//	uutMatrix.A = 1<<9; 
				//	uutMatrix.B = 0;
				//	uutMatrix.C = 0;
				//	uutMatrix.D = uutMatrix.A;
				//	uutMatrix.E = 0;
				//	uutMatrix.F = 0;
				//	width = screen_width;
				//	height = screen_height;
				//	color = 0;
				//	addr = (u32)FrameBuffer;
				//	TexRaster(&uutTile, &uutMatrix, addr, width, height, texCtrl, color,SB_MaskType);
				//	if (uutTile.color_done)
				//		color_blending(&theFinalTile, &uutTile);
				//}
			
				while(suffershift < SoureBufferSize)
				{
					headerfirst = sourceBuffer[suffershift++];
					headersecond = sourceBuffer[suffershift++];
					//headerfirst为8bits，各比特位包含纹理的相关信息，对应关系如下
					//headerfirst： SB_texType(7:4) SB_Mask(3) SB_Matrix(2:1) SB_End(0)
					SB_texType = (headerfirst & TEXTYPEMASK) >> 4;
					SB_Mask = (headerfirst & MASKBIT) >> 3;   
					SB_Matrix = (headerfirst & ABCDEFMATRIX) >> 1;
					SB_End = headerfirst & 0x1;
					//headersecond为8bits，各比特位包含纹理的相关信息，对应关系如下
					//headersecond： Resaved(7:3) SB_MaskType(2:1) SB_AddrType(0)
					SB_AddrType = headersecond & 0x1;
					SB_MaskType = (headersecond>>1) & 0x3;
					texCtrl = (SB_texType << 1) + SB_Mask;
					
					if (SB_End == 1 || suffershift>=SoureBufferSize)
						break;
					//analyze the boxSize
					start_x = sourceBuffer[suffershift++];
					start_y = sourceBuffer[suffershift++];
					stop_x = sourceBuffer[suffershift++];
					stop_y = sourceBuffer[suffershift++];

					//analyze the texSize
					width = (s16)*((s16 *)(sourceBuffer + suffershift));
					suffershift += 2;
					height = (s16)*((s16 *)(sourceBuffer + suffershift));
					suffershift += 2;

					//analyze the matrix
					if (SB_Matrix == 0)
					{//add by xt 15/3/31
						//right
						uutMatrix.E = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.F = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.A = uutMatrix.D = 512;//1.6.9
						uutMatrix.B = uutMatrix.C = 0;
					}
					else if (SB_Matrix == 1)
					{//add by xt 15/3/31
						//right
						uutMatrix.A = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.B = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.C = -uutMatrix.B;
						uutMatrix.D = uutMatrix.A ;
						uutMatrix.E = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.F = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
					}
					else if(SB_Matrix == 2)
					{//add by xt 15/3/31
						uutMatrix.A = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.B =  0;
						uutMatrix.C =  0;
						uutMatrix.D = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.E = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.F = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;					
					}
					else
					{//add by xt 15/3/31
						//right
						uutMatrix.A = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.B = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.C = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.D = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.E = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
						uutMatrix.F = (s16)*((s16 *)(sourceBuffer + suffershift));
						suffershift += 2;
					}

					//analyze the addr_type
					if (SB_AddrType == 1)
					{//modified by xt 15/5/19
						addr = (u32)*((u32 *)(sourceBuffer + suffershift));
						suffershift += 4;
						color = addr;
					}

					/**************************************************************/
					//打印输入数据 add by darydou 3/4
					#if __PRINTDEBUG__
					if (PRINTFLAG){
						//打印纹理的个数
						printcount++;
						WriteText(buffer, "This is the %d texture!\r\n", printcount);
						ERROR_PRINT(buffer);
						//打印纹理的格式
						switch (SB_texType)
						{
						case SIM_DXT1:
							strcpy(textureformat, "DXT1");
							break;
						case SIM_DXT3: 
							strcpy(textureformat, "DXT3");
							break;
						case SIM_ALPHA8:
							strcpy(textureformat, "ALPHA8");
							break;
						case SIM_PURECOLOR:
							strcpy(textureformat, "COLOR");
							break;
						case SIM_RGB565:
							strcpy(textureformat, "BMP565");
							break;
						case SIM_RGBA8888:
							strcpy(textureformat, "BMP8888");
							break;
						case SIM_Y8:
							strcpy(textureformat, "Y8");
							break;
						case SIM_YUV422:
							strcpy(textureformat, "YUV422");
							break;
						case SIM_ALPHA1:
							strcpy(textureformat, "ALPHA1");
							break;
						case SIM_PUREMASK:
							strcpy(textureformat, "PUREMASK");
							break;
						default:
							strcpy(textureformat, "erro!!!");
							break;
						}
						WriteText(buffer, "The texture format is %s.\r\n", textureformat);
						ERROR_PRINT(buffer);
						//打印是否做了掩膜
						if (SB_Mask == 0)
							ERROR_PRINT("The texture have no mask.\r\n");
						else
							ERROR_PRINT("The texture have mask.\r\n");
						//打印包围盒
						ERROR_PRINT("The texture boxsize have any tile,but not one.As follows:\r\n");
						WriteText(buffer, "tile_start_x is %d,\r\n", start_x);
						ERROR_PRINT(buffer);
						WriteText(buffer, "tile_stop_x  is %d,\r\n", stop_x);
						ERROR_PRINT(buffer);
						WriteText(buffer,"tile_start_y  is %d,\r\n", start_y);
						ERROR_PRINT(buffer);
						WriteText(buffer,"tile_stop_y   is %d.\r\n", stop_y);
						ERROR_PRINT(buffer);
						//打印矩阵信息
						if (SB_Matrix == 0){
							ERROR_PRINT("The matrix only have E and F.\r\nMatrix as follows:\r\n");
						}
						else if (SB_Matrix == 1){
							ERROR_PRINT("The matrix only have A and B, E and F(.\r\nMatrix as follows:\r\n");
						}
						else if (SB_Matrix == 2){
							ERROR_PRINT("The matrix only have A and D, E and F(.\r\nMatrix as follows:\r\n");
						}
						else if (SB_Matrix == 3){
							ERROR_PRINT("The matrix have all A,B,C,D,E and F.\r\nMatrix as follows:\r\n");
						}
						//else{
						//	ERROR_PRINT("The matrix is erro !!!\r\n");
						//}
						WriteText(buffer, " A is %d \r\n B is %d\r\n C is %d\r\n D is %d\r\n E is %d \r\n F is %d .\r\n",
							uutMatrix.A, uutMatrix.B, uutMatrix.C, uutMatrix.D, uutMatrix.E, uutMatrix.F);
						ERROR_PRINT(buffer);
						////打印纹理的大小宽和高
						//if (SB_TexSize == 0){
						//	ERROR_PRINT("The texsize is lower than 256.\r\n");
						//}
						//else{
						//	ERROR_PRINT("The texsize is more than 256.\r\n");
						//}
						WriteText(buffer, "The texsize width is %d,\r\n height is %d\r\n", width, height);
						ERROR_PRINT(buffer);
						//打印是地址还是颜色纹理
						if (SB_AddrType == 0){
							ERROR_PRINT("The texture has no addresss!\r\n");
						}
						else{
							ERROR_PRINT("The texture have a addr!\r\n");
							WriteText(buffer, "The texture addr is 0x%.8x!\r\n", addr);
							ERROR_PRINT(buffer);
						}
						////简单的错误判断
						////1.一个纹理的纹理格式标记是颜色纹理，但地址标记显示它具有地址而不是具有三颜色，纹理文件头有误
						////2.一个纹理的纹理格式标记是非颜色纹理，但地址标记显示它具有三颜色而不是纹理的存储地址，纹理文件头有误
						//if (SB_AddrType&&SB_texType == 3){
						//	ERROR_PRINT("ERROR: The texture textype is  color ,but its textype is addr!!!\r\nThe textureheader may be wrong!!!\r\n	Please check it!!!");
						//}
						//if (!SB_AddrType&&SB_texType != 3){
						//	ERROR_PRINT("ERROR: The texture textype is not  color ,but its textype is color!!!\r\nThe textureheader may be wrong!!!\r\n Please check it!!!");
						//}
						//ERROR_PRINT("\r\n"); 
						//ERROR_PRINT("\r\n");
					}
					#endif
					/**************************************************************/
					//modified by xt 20150506
					if ((tile_x_num < start_x || tile_x_num > stop_x) || (tile_y_num < start_y || tile_y_num > stop_y))
						continue;

					TexRaster(&uutTile, &uutMatrix, addr, width, height, texCtrl, color,SB_MaskType);
#ifdef TEX_RAS_LOG_GEN
					texRasFile << "tile_x " << tile_x_num << " tile_y " << tile_y_num << " texCtrl " << std::setw(4) << std::hex << (int)(texCtrl) << std::endl;
					for(int i = 0; i < TilePixelSize; i++)
					{
						texRasFile << std::setw(2) << std::hex << (int)(uutTile.ColorBuffer[i].a) << " ";
						texRasFile << std::setw(2) << std::hex << (int)(uutTile.ColorBuffer[i].r) << " ";
						texRasFile << std::setw(2) << std::hex << (int)(uutTile.ColorBuffer[i].g) << " ";
						texRasFile << std::setw(2) << std::hex << (int)(uutTile.ColorBuffer[i].b) << std::endl;
					}
#endif
					if (uutTile.color_done)
						color_blending(&theFinalTile, &uutTile);
#ifdef COLOR_BLENDING_LOG_GEN
					colorBlendingFile << "tile_x " << tile_x_num << " tile_y " << tile_y_num << " texCtrl " << std::setw(4) << std::hex << (int)(texCtrl) << std::endl;
					for(int i = 0; i < TilePixelSize; i++)
					{
						colorBlendingFile << std::setw(2) << std::hex << (int)(theFinalTile.TileBuffer[i].r) << " ";
						colorBlendingFile << std::setw(2) << std::hex << (int)(theFinalTile.TileBuffer[i].g) << " ";
						colorBlendingFile << std::setw(2) << std::hex << (int)(theFinalTile.TileBuffer[i].b) << std::endl;
					}
#endif
				}//end of searching every source buffer element
				PRINTFLAG = 0;
				Write_Frame(&theFinalTile, FrameBuffer,flag, 0);

				int m=0;
			}
	FrameRate++;
#ifdef TEST_LOG_GEN
#ifdef TEX_RAS_LOG_GEN
	texRasFile.close();
#endif
#ifdef COLOR_BLENDING_LOG_GEN
	colorBlendingFile.close();
#endif
#ifdef TILE_BUFFER_LOG_GEN
	tileBufferFile.close();
#endif
#ifdef UV_LOG_GEN
	uvLogFile.close();
#endif
#endif
	//for(tile_num = 0; tile_num < (SCREEN_SIZE / 32 / 32); tile_num++ )//searching every tile
	//{
	//	//initial every tile
	//	InitTile(&uutTile, tile_x, tile_y);
	//	InitFinalTile(&theFinalTile, tile_x, tile_y);
	//	//searching every uutSourceBuffer
	//	for(int i = 0; i < SOURCE_BUFFER_SIZE; i++)
	//	{
	//		//analyze the header
	//		header = uutSourceBuffer[i].header;
	//		SB_texType = header % 16;
	//		SB_Mask = (header >> 4) % 2;
	//		SB_Matrix = (header >> 6) % 4;
	//		SB_BoxSize = (header >> 5) % 2;
	//		SB_TexSize = (header >> 8) % 2;
	//		SB_AddrType = (header >> 9) % 2;
	//		texCtrl = (SB_texType << 1) + SB_Mask;
	//
	//		//analyze the matrix
	//		if(SB_Matrix == 0)
	//		{
	//			E = uutSourceBuffer[i].Matrix & 0xffff;
	//			F = (uutSourceBuffer[i].Matrix >> 16) & 0xffff;
	//			A = D = 16;//1.7.8
	//			B = C = 0;
	//		}
	//		else if(SB_Matrix == 1)
	//		{
	//			A = uutSourceBuffer[i].Matrix & 0xffff;
	//			B = (uutSourceBuffer[i].Matrix >> 16) & 0xffff;
	//			C = -B;
	//			D = A;
	//			E = F = 0;
	//		}
	//		else
	//		{
	//			A = uutSourceBuffer[i].Matrix & 0xffff;
	//			B = (uutSourceBuffer[i].Matrix >> 16) & 0xffff;
	//			C = -B;
	//			D = A;
	//			E = (uutSourceBuffer[i].Matrix >> 32) & 0xffff;
	//			F = (uutSourceBuffer[i].Matrix >> 48) & 0xffff;
	//		}
	//
	//		//analyze the boxSize
	//		if(SB_BoxSize == 0)
	//		{
	//			start_x = uutSourceBuffer[i].BoxSize & 0x1f;
	//			start_y = (uutSourceBuffer[i].BoxSize >> 5) & 0x1f;
	//			stop_x = start_x;
	//			stop_y = start_y;
	//		}
	//		else
	//		{
	//			start_x = uutSourceBuffer[i].BoxSize & 0x1f;
	//			start_y = (uutSourceBuffer[i].BoxSize >> 5) & 0x1f;
	//			stop_x =  (uutSourceBuffer[i].BoxSize >> 10) & 0x1f;
	//			stop_y =  (uutSourceBuffer[i].BoxSize >> 15) & 0x1f;
	//		}
	//
	//		//analyze the texSize
	//		if(SB_TexSize == 0)
	//		{
	//			width = uutSourceBuffer[i].TexSize & 0xff;
	//			height = (uutSourceBuffer[i].TexSize >> 8) & 0xff;
	//		}
	//		else
	//		{
	//			width = uutSourceBuffer[i].TexSize & 0xffff;
	//			height = (uutSourceBuffer[i].TexSize >> 16) & 0xffff;
	//		}
	//
	//		//analyze the addr_type
	//		if(SB_AddrType == 0)
	//		{
	//			color = uutSourceBuffer[i].AddrType;
	//			addr = 0;
	//		}
	//		else
	//		{
	//			addr = uutSourceBuffer[i].AddrType;
	//			color = 0;
	//		}
	//
	//		if( (tile_x_num < start_x || tile_x_num > stop_x) && (tile_y_num < start_y || tile_y_num > stop_y) )
	//			continue;
	//
	//		uutMatrix.A = A;
	//		uutMatrix.B = B;
	//		uutMatrix.C = C;
	//		uutMatrix.D = D;
	//		uutMatrix.E = E;
	//		uutMatrix.F = F;
	//		TexRaster(&uutTile, &uutMatrix, &(uutRomInfo->tex[addr]), width, height, texCtrl, color);
	//		if(uutTile.color_done)
	//			color_blending(&theFinalTile, &uutTile);
	//		else
	//			continue;
	//	}//end of searching every source buffer element
	//	Write_Frame(&theFinalTile, FrameBuffer);
	//}
}


#endif
