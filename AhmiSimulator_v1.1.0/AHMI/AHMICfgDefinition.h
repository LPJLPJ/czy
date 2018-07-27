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
//    definition for debug
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef AHMI_CFG_DEFINITION
#define AHMI_CFG_DEFINITION

//#define DEBUG        //定义了DEBUG则会打开所有的调试信息

//**************通过修改以下两个宏来选择版本********************
#define PC_SIM         //PC平台仿真专用
//#define EMBEDDED     //嵌入式专用版本
//***************************************************************


#ifdef PC_SIM

//#define AHMI_DEBUG            //定义了DEBUG则会打开所有的调试信息
#define AHMI_CORE        //内核，所有平台都会用到的代码
#define USE_TOUCHSCREEN
#define TEST_VECTOR_GEN  //打开会生成FPGA专用的仿真文件
//#define USE_UART
//#define IMPLEMENTATION   //专用于集成版本的软件
#define KEYBOARD_DEBUG  //打开用于调试键盘的指令
//#define TEST_LOG_GEN
//#define TILE_BUFFER_LOG_GEN
//#define PARTIAL_DOUBLE_BUFFER //部分双缓存
#define WHOLE_TRIBLE_BUFFER  //完全三缓存
#define STATIC_BUFFER_EN     //是否开启静态缓存
//#define VISUAL_SOURCE_BOX_EN //是否开启包围盒可视化

#endif

#ifdef EMBEDDED

//#define DEBUG          //定义了DEBUG则会打开所有的调试信息
#define AHMI_CORE      //内核，所有平台都会用到的代码
#define USE_TOUCHSCREEN
//#define KEYBOARD_DEBUG  //打开用于调试键盘的指令

//#define USING_XILINX
#define USING_LATTICE
//#define USING_CHANGE_BRIGHTNESS
#define USING_NO_CHANGE_BRIGHTNESS
//#define USE_UART
#define KEYBOARD_DEBUG  //打开用于调试键盘的指令
#define PARTIAL_DOUBLE_BUFFER //部分双缓存
//#define WHOLE_TRIBLE_BUFFER  //完全三缓存
//#define PARTIAL_TRIBLE_BUFFER //部分三缓存

#endif

//all version, refresh strategy
//#define SOURCE_BOX_COMBINE  //开启包围盒合并策略

//all version, enable the module test
//#define MODULE_TEST_EN


#endif //end of header
