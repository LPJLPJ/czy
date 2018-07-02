////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2017/06/14
// File Name:     animationTest.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2017/06/14 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    the class of animatonTest
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_TEST__H
#define ANIMATION_TEST__H

#include <fstream>
#include <iostream>


class animationTestClass{
public:
	void createAnimationTest(std::ofstream* pAnimationTestLogFile);
	void animationTaskQueueHandleTest(std::ofstream* pAnimationTestLogFile);
	void animationMainTainTest(std::ofstream* pAnimationTestLogFile);
	void animationActionTest(std::ofstream* pAnimationTestLogFile);
};


#endif