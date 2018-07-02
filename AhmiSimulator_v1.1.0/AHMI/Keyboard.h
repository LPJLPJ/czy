////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     Keyboard.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/09/01 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    definition of keyboard trigger
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef KEYBOARD__H
#define KEYBOARD__H

#include "keyboard_cd.h"
#include "publicType.h"

void keyboardLeft( void );
void keyboardRight( void );
void keyboardEntered( void );
void keyboardClear( void );     //clear the highlight 
void keyboardReveiveTask(void*); //接受键盘的键值
#ifdef __cplusplus
extern "C"{
#endif
void sentToKeyboardQueue(u8 key);
#ifdef __cplusplus
}
#endif

#endif
