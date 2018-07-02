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


class RefreshTestClass{
public:
	void widgetRefreshTaskTest(std::ofstream* RefreshTestLogFile);
	void drawImmediatelyTest(std::ofstream* RefreshTestLogFile);
	void drawPageTest(std::ofstream* RefreshTestLogFile);
	void drawCanvasTest(std::ofstream* RefreshTestLogFile);
	void drawWidgetTest(std::ofstream* RefreshTestLogFile);

	void drawTextureTest(std::ofstream* RefreshTestLogFile);

	void sourceBoxRecomputeTest(std::ofstream* RefreshTestLogFile);

	void writeSourceBufferTest(std::ofstream* RefreshTestLogFile);

	void renewTextureSourceBoxTest(std::ofstream* RefreshTestLogFile);

};


#endif