////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     drawImmediately_cd.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef DRAWIMMEDIATELY_CD__H
#define DRAWIMMEDIATELY_CD__H
#include "publicDefine.h"
#ifdef AHMI_CORE

//*****************************drawimmediately**********************************
#define  Stm32_interface_Addr  stm32info
#define  Tile_start_x 0
#define  Tile_start_y 1
#define  Tile_end_x   2
#define  Tile_end_y   3
#define  addr_ahmi_0  4 //part of ahmi addr 
#define  addr_ahmi_1  5 //part of ahmi addr 
#define  addr_ahmi_2  6 //part of ahmi addr 
#define  addr_ahmi_3  7 //part of ahmi addr 
#define  addr_disp_0  8 //part of disp addr 
#define  addr_disp_1  9 //part of disp addr 
#define  addr_disp_2  10 //part of disp addr 
#define  addr_disp_3  11 //part of disp addr 
#define  addr_video_0  12 //part of video addr 
#define  addr_video_1  13 //part of video addr 
#define  addr_video_2  14 //part of video addr 
#define  addr_video_3  15 //part of video addr 
#define  clk_disp_mul_0 16 //display mul
#define  clk_disp_mul_1 17 //display mul
#define  clk_disp_div_0 18 //display div
#define  clk_disp_div_1 19 //display div
#define  hpixels_0      20 //horizontal live pixels        
#define  hpixels_1      21 //horizontal live pixels  
#define  vlines_0       22 //vertical live lines        
#define  vlines_1       23 //vertical live lines 
#define  hsyncpw_0      24 //HSYNC pulse width
#define  hsyncpw_1      25 //HSYNC pulse width
#define  vsyncpw_0      26 //VSYNC pulse width
#define  vsyncpw_1      27 //VSYNC pulse width
#define  hfnprch_0      28 //herizontal front portch
#define  hfnprch_1      29 //herizontal front portch
#define  vfnprch_0      30 //vertical front portch
#define  vfnprch_1      31 //vertical front portch
#define  hbkprch_0      32 //herizontal back portch
#define  hbkprch_1      33 //herizontal back portch
#define  vbkprch_0      34 //vertical back portch
#define  vbkprch_1      35 //vertical back portch
#define  logical_addr_0 36 //logical address of nand flash
#define  logical_addr_1 37 //logical address of nand flash
#define  physical_addr_0 38 //physical address of nand flash
#define  physical_addr_1 39 //physical address of nand flash
#define  BBMctrl_0       40 //bbm control of nand flash
#define  BBMctrl_1       41 //bbm control of nand flash
#define  screen_rotate_0 42 //screen rotate degree
#define  screen_rotate_1 43 //screen rotate degree

#define  SIZE_OF_DISPLAY_BUFFER 0x580000// the size of display buffer, used to change buffer
#define  SIZE_OF_CVBS_BUFFER    0x200000 // the size of cvbs buffer, used to change cvbs
#define  START_ADDR_OF_DISPLAY  0xA00000 // the start address of display buufer
#define  START_ADDR_OF_CVBS     0x400000 //the start address of cvbs
#define  START_ADDR_OF_RAM      0x80000000 //the start address of ram


#define NUM_OF_STM32_REGS 42

#define  SoureBufferAddr     sourceBuffer

#define  COLOR_BITS    16

#ifdef PC_SIM
#define  SoureBufferSize  0x10000
#endif
#ifdef EMBEDDED
#define  SoureBufferSize  2048
#endif

#define  TexMinSize       14
#define  TexMaxSize       26



//该定义参考文档纹理映射表
#define  TEXTYPEMASK          0xf0
#define  DXT1                 0x00
#define  DXT3                 0x10
#define  ALPHA8               0xa0
#define  ALPHA1               0x20
#define  ALPHA2               0x90  //20160613
#define  ALPHA4               0xb0  //reserved
#define  PURECOLOR            0x30
#define  RGB565               0x40
#define  RGBA8888             0x50
#define  Y8                   0x60
#define  YUV422               0x70
#define  PUREMASK             0x80
#define  ARGB1555             0xc0  //20160613
#define  ARGB3454             0xd0  //20160613
#define  DXT5                 0xe0  

					          
#define  MASKBIT              0x8
#define  MASKTEX              0x8
#define  NONMASKTEX           0x0
					          

#define  EFMATRIX             0x0
#define  ABEFMATRIX           0x2
#define  ADEFMATRIX           0x4
#define  ABCDEFMATRIX         0x6


#define  NONADDRTEXTURE         0
#define  ADDRTEXTURE          0x100

#define  MASKTYPE_OVERLAP     0
#define  MASKTYPE_AND         0x200
#define  MASKTYPE_OR          0x400
#define  MASKTYPE_NOT         0x600

#define		DRAWING                 0x8000 //是否绘制

#define     ANIMATION_MOVING_BIT    0x4000 //是否绘制动画时移动
#define     ANIMATION_MOVING        0x4000 //有动画时纹理需要移动
#define     ANIMATION_NO_MOVING     0      //有动画时纹理不需要移动

#define  ENDFLAG              0x1



//*****************************drawimmediately**********************************

#endif

#endif
