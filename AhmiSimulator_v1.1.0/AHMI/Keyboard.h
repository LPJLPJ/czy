////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     Keyboard.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/09/01 by Zhou Yuzhi(������)
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
void keyboardReveiveTask(void*); //���ܼ��̵ļ�ֵ
#ifdef __cplusplus
extern "C"{
#endif
void sentToKeyboardQueue(u8 key);
#ifdef __cplusplus
}
#endif

#endif
