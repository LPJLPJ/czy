////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 3.00 - File Created 2016/04/13 by 于春营
// Additional Comments:
//   
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "AHMIBasicDefine.h"
#include "aniamtion.h"
#include "NumberClass.h"
#include "publicType.h"
#include "drawImmediately_cd.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;

extern u32 startOfDynamicPage;
extern u32 endOfDynamicPage;
extern u8 animationDuration;
extern u8 animationExist;

//-----------------------------
// 函数名： NumberClass
// 构造函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
NumberClass::NumberClass()
{

}

//-----------------------------
// 函数名： ~NumberClass
// 析构函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
NumberClass::~NumberClass()
{

}


//-----------------------------
// 函数名： NumberClass::initWidget
// 初始化并绘制数字框，如果是全屏刷新，先根据所绑定的tag刷新该控件，在绘制该控件
// 参数列表：
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //页面刷新
//  @param   TileBoxClassPtr pTileBox //包围盒
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus NumberClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
	)
{
//	u32 value;
	TagClassPtr bindTag;
	ActionTriggerClass tagtrigger;
	WidgetClassInterface myWidgetClassInterface;

	if((NULL == p_wptr) || (NULL == u32p_sourceShift) || (NULL == pTileBox))
		return AHMI_FUNC_FAILURE;

	bindTag = &TagPtr[p_wptr->BindTagID];
//	value = bindTag->getValue();
	

	if(u8_pageRefresh)
	{
		tagtrigger.mTagPtr = bindTag;
		tagtrigger.mInputType = ACTION_TAG_SET_VALUE;
		if(widgetCtrl(p_wptr,&tagtrigger,1) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}

	if((p_wptr->NumOfLine & 0xff) != 0)
	{
		if(myWidgetClassInterface.drawTexture(p_wptr,u32p_sourceShift,RefreshType,pTileBox,staticTextureEn) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}
	else 
	{
		if(myWidgetClassInterface.drawTexture(p_wptr,u32p_sourceShift,RefreshType,pTileBox,staticTextureEn) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}
	return AHMI_FUNC_SUCCESS;
}

//***********************************
//数字框为0x8数字框控制函数，表函数
//显示N个数字，需要N+2张纹理。
//支持小数（一个小数点）,N+3张纹理。
//只支持tag输入类型。
//第一张纹理为取非的纯掩膜纹理，用于清空alpha buffer
//若为有符号数，需要一张纹理表示符号，之后N（N+1)张纹理对应N个数字（及1个小数点），指向字库中的位置，纹理类型为ALPHA1，顺序自左至右。
//最后一张纹理为纯色纹理或者其他。
//WidgetAttr标识：
//14:符号显示模式：0为无符号数，范围4294967295~0,1为有符号数，范围2147483647~-2147483648
//13:前导零显示模式：0为不显示前导零，1为显示前导零
//12-9:有几位小数，及小数点出现在最小数字的左边几位，比如2表示有两位小数
//8-5:显示的字符数，不包括符号位及小数点
//4-0:控件类型，数字框为0x8
//**********************************
funcStatus NumberClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//页面刷新
	)
{
	u32 actionAddr;
	u32 value;
	u32 uValue[5] = {0};
	s32 sValue[5] = {0};
	s32	maxValue;
	s32	minValue;
	s32 curValue;
	s32 lowAlarmValue;
	s32 highAlarmValue;

	u8  leadZero;
	u8  sign;
	u8  pointPosRight;
	u8	pointPosLeft;
	u8  numofNumber;
	u8	i;
	s32 j;
	u8	showingZero; //表示是否需要显示数字0，在不显示前导零的情况下，需要用此变量表示后续0是否显示.
	u8	signFlag; //是否已经显示符号
	                  // 0表示不显示0，1表示显示0
	u8	dataMinus; // 表示待显示数字的正负性，1表示负数，0表示正数及负号已经打印
	u8  numOfDisTexture;
	u8  nextNumOfDisTexture;
	u8  difOfNumber;
	u16 widthOfFont;
	u16 shiftSize;
	u16 code;

	u8 overflow;
	u8 align;
	u8 overflowStyle;    //数据超出最大值后的处理方式 0：显示最大值，模式方式 1：不显示数据
	u8 numRadix;    // 数字进制 0-十进制  1-十六进制
	u8 markingMode; // 十六进制是否显示0x   0-显示  1-不显示
	u8 transformMode; // 字母是否大写显示  0-小写  1-大写
	u8 radixBase = 10;
	u8 upperLetterOffset = 7;   // 大写字母与数字的偏移量   
	u8 lowerLetterOffset = 39;  // 小写字母的偏移量

	u64 tens;
	TextureClassPtr nextTexturePtr, texturePtr;
	u32 oldValue=0;
	u16 oldValueinit=0;
	u8 movingDir;  //0表示下移，1表示上移

	u8 offsetpoint=0;
//	RefreshMsg refreshMsg;
	s16 SpacingX = (s16)(p_wptr->SPACINGX);

	if((NULL == p_wptr) || (NULL == ActionPtr) || NULL == ActionPtr->mTagPtr || NULL == gPagePtr->pBasicTextureList){
		ERROR_PRINT("ERROR: NULL pointer");
		return AHMI_FUNC_FAILURE;
	}

	if( (s16)(p_wptr->WidgetOffsetX) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetX) < -MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) < -MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetWidth > MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetHeight > MAX_WIDTH_AND_HEIGHT || 
		p_wptr->WidgetWidth == 0 ||  
		p_wptr->WidgetHeight == 0)
	{
		ERROR_PRINT("ERROR: when drawing number widght, the offset\\width\\height exceeds the boundary");
		return AHMI_FUNC_FAILURE;
	}

	numOfDisTexture = 0;
	nextNumOfDisTexture = 0;
	difOfNumber = 0;
	widthOfFont = 0;
	shiftSize = 0;
	overflow = 0;

	value = ActionPtr->mTagPtr->mValue;

	leadZero = (p_wptr->WidgetAttr & 0x2000 )?1:0;
	sign = (p_wptr->WidgetAttr & 0x4000 )?1:0;
	pointPosRight = (p_wptr->WidgetAttr >>9) & 0xf;
	numofNumber = (p_wptr->WidgetAttr >>5) & 0xf;
	oldValueinit = (p_wptr->WidgetAttr & 0x8000);

	align = (p_wptr->NumOfLine >> 8) & 0xf;
	overflowStyle = (p_wptr->NumOfLine >> 12) & 0xf;

	//目前限制只能有1-10位数字，小数位数(0,数字位数减1)
	if(numofNumber > 10 || numofNumber < 1){
		ERROR_PRINT("ERROR: don't support such length of number");
		return AHMI_FUNC_FAILURE;
	}
	//小数位数不能超过数字的位数，且不小于0
	if(pointPosRight < 0 || pointPosRight > numofNumber - 1){
		ERROR_PRINT("ERROR: the numbers of decimal is wrong");
		return AHMI_FUNC_FAILURE;
	}
	
	maxValue = (s32)((p_wptr->MaxValueH << 16) + p_wptr->MaxValueL);
	minValue = (s32)((p_wptr->MinValueH << 16) + p_wptr->MinValueL);
	lowAlarmValue  = (s32)((p_wptr->LowAlarmValueH  << 16) + p_wptr->LowAlarmValueL );
	highAlarmValue = (s32)((p_wptr->HighAlarmValueH << 16) + p_wptr->HighAlarmValueL);


	numRadix =  (u8)(p_wptr->HexControl& 0x000f);
	markingMode = (u8)((p_wptr->HexControl & 0x00f0)>>4);
	transformMode = (u8)((p_wptr->HexControl & 0x0f00)>>8);

	if(numRadix){
		radixBase = 16;
	}

	
	if(sign)
	{
		if((s32)value > (s32)maxValue)
		{
			value = (s32)maxValue;
			overflow = 1;
		}
		if((s32)value < (s32)minValue)
		{
			value = (s32)minValue;
			overflow = 1;
		}
	}
	else
	{
		if((u32)value > (u32)maxValue)
		{
			value = (u32)maxValue;
			overflow = 1;
		}
		if((u32)value < (u32)minValue)
		{
			value = (u32)minValue;
			overflow = 1;
		}
	}

	//modify currnet value
	if(animationDuration) //page animation
	{
		p_wptr->CurValueL = (u16)value;
		p_wptr->CurValueH = (u16)(value >> 16);
	}

	if((p_wptr->NumOfLine & 0xff) == 0 || (p_wptr->NumOfLine & 0xff) == 1) //no animation
		curValue = value;
	else
		curValue = (s32)((p_wptr->CurValueH << 16) + p_wptr->CurValueL);

	if(leadZero == 0)
	{
		if(curValue == 0)
			numOfDisTexture = 1;
		else
		{
			numOfDisTexture = 0;
			for(j = (s32)curValue > 0 ? curValue : -curValue; j >0; j /= 10)
			{
				numOfDisTexture++;
			}
		}
		if(pointPosRight != 0) //至少显示小数加1个数
			if(numOfDisTexture < pointPosRight+1)
				numOfDisTexture = pointPosRight+1;
	}
	else 
		numOfDisTexture = numofNumber;

	if(leadZero == 0)
	{
		if(value == 0)
			nextNumOfDisTexture = 1;
		else
		{
			nextNumOfDisTexture = 0;
			for(j = (s32)value > 0 ? (s32)value : (-(s32)value); j >0; j /= 10)
			{
				nextNumOfDisTexture++;
			}
		}
		if(pointPosRight != 0) //至少显示小数加1个数
			if(nextNumOfDisTexture < pointPosRight+1)
				nextNumOfDisTexture = pointPosRight+1;
	}
	else 
		nextNumOfDisTexture = numofNumber;

	//数据超出最大值后的处理方式 0：显示最大值，模式方式 1：不显示数据
	if(overflowStyle == OVERFLOW_DISPLAY)
	{
		overflow = 0;
	}

	tens=1;
	showingZero=0;
	signFlag = 1 - sign; 
	dataMinus=0;


	for(i=0;i<numofNumber;i++) 
		//tens *=10;
		tens *= radixBase;

	pointPosLeft = numofNumber - pointPosRight;//小数点左边的字符数，包括符号

	if(pointPosLeft <1 ) pointPosLeft = 1;

	if(sign)
	{
		numofNumber++; //表示纹理数量，不是数字数量
		pointPosLeft++;//小数点左边的字符数，包括符号
		if((s32)curValue < 0) //sign of '-'
		{
			numOfDisTexture++;
		}
		if((s32)value < 0)
			nextNumOfDisTexture++;
	}

	if(pointPosRight != 0 )
	{
		numofNumber++;
#if 0
		//no need to judge if the data is zero
		if(curValue != 0)
			numOfDisTexture++; //sign of '.'
		if(value != 0)
			nextNumOfDisTexture++;
#endif	
		numOfDisTexture++; //sign of '.'
		nextNumOfDisTexture++;
	}

	gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].mTexAttr |= TEXTURE_USING_WIDGET_BOX; //puremask第一层必须使用控件的包围盒
	if((p_wptr->NumOfLine & 0xff) == 0 || (p_wptr->NumOfLine & 0xff) == 1)   //无动画的方式切换数字
	{
		texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex+1]);
		#ifdef EMBEDDED
		if((texturePtr < (void*)startOfDynamicPage) || (texturePtr > (void*)endOfDynamicPage))
			return AHMI_FUNC_FAILURE;
		#endif

	}
	else if((p_wptr->NumOfLine & 0xff) == 2) //以动画方式切换数字
	{
		texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1]);
		nextTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + numofNumber + 1]);
	}
	else 
		return AHMI_FUNC_FAILURE;


	//判断平移方向
	uValue[0] = (u32)value;
	sValue[0] = (s32)value;
	uValue[1] = (u32)curValue;
	sValue[1] = (s32)curValue;

    //bug 动画
	if((p_wptr->NumOfLine & 0xff) == 2) {
		if(sign)
		{
			if(sValue[0] > (s32)curValue)
			{
				//下移
				movingDir = 0;
			}
			else if(sValue[0] < (s32)curValue)
			{
				 //上移
				movingDir = 1;
			}
			else
			{
				//不移动
				if(u8_pageRefresh == 0) 
					return AHMI_FUNC_SUCCESS;
			}
		}
		else
		{
			if(uValue[0] > (u32)curValue)
			{
				//下移
				movingDir = 0;
			}
			else if(uValue[0] < (u32)curValue)
			{
				 //上移
				movingDir = 1;
			}
			else
			{
				//不移动
				if(u8_pageRefresh == 0)
					return AHMI_FUNC_SUCCESS;
			}
		}
	}
	else{
		movingDir = 0;
	}
	if(movingDir == 0) //上移
	{
		p_wptr->PreviousTexturePtrFlag = 0;
	}
	else if(movingDir == 1)//下移
	{
		p_wptr->PreviousTexturePtrFlag = 1;
	}
	else if(u8_pageRefresh == 0 && (overflowStyle == 0))
	{
		//ERROR_PRINT("error in parsing number");
		return AHMI_FUNC_SUCCESS;
	}

	if(sValue[1] < 0)
	{
		dataMinus = 1;
		sValue[1] = - sValue[1];
		
	}

	// 截断超过显示范围的数字
	uValue[1] = uValue[1] % tens;
	sValue[1] = sValue[1] % tens;
	// tens /= 10;
	tens /= radixBase;
	

	//操作纹理的值
	//pre
	for(i=0;i<numofNumber;i++)
	{
		// 用于测试字符绘制是否准确
		//texturePtr->FocusedSlice = '0' - 0x20;
		//// tens /= radixBase;
		//texturePtr++;
		//continue;

		if(sign)
		{
			code = (u16)(sValue[1] / tens);		
			sValue[1] %= tens;
		}
		else
		{
			code = (u16)(uValue[1] / tens);
			uValue[1] %= tens;
		}

		//显示负号
		if( sign && dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
		//  有符号数  数据为负     显示前导零    不显示前导零   当前数字为非0 不显示前导零 有小数点            到达小数点前的0的前面
		{
			if(overflow == 1)
				texturePtr->FocusedSlice = ' ' - 0x20;
			else
				texturePtr->FocusedSlice = '-' - 0x20;
			texturePtr++;

			dataMinus = 0;//负号已显示，认为为正数
			signFlag = 1;
			i++;
		}
		//显示正号
		else if( sign && !dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
		//  有符号数  数据为负     显示前导零    不显示前导零   当前数字为非0 不显示前导零 有小数点            到达小数点前的0的前面
		{
			if(overflow == 1)
				texturePtr->FocusedSlice = ' ' - 0x20;
			else
				texturePtr->FocusedSlice = 0;
			texturePtr++;

			signFlag = 1;
			i++;
		}
		//正数第一位显示空格
		else if( sign && !dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
			//  有符号数  数据为正     显示前导零    不显示前导零   当前数字为非0 不显示前导零 有小数点            到达小数点前的0的前面
		{
			texturePtr->FocusedSlice = ' ' - 0x20;
			texturePtr++;
			signFlag = 1;
			i++;
		}
		
		if(numRadix==1 && markingMode==1 && i==0)
		{
			texturePtr->FocusedSlice = '0' - 0x20;
			tens /= radixBase;
			texturePtr++;
			continue;
		}else if(numRadix && markingMode==1 && i==1){
			texturePtr->FocusedSlice = 'X' - 0x20;
			tens /= radixBase;
			texturePtr++;
			continue;
		}

		//显示小数点
		if(i == pointPosLeft)
		{
			if(overflow == 1)
				texturePtr->FocusedSlice = ' ' - 0x20;
			else
				texturePtr->FocusedSlice = '.' - 0x20;//显示小数点
			texturePtr++;
			offsetpoint = i;
			i++;
		}

			

		//显示前导零
		if (!showingZero && code == 0 && !leadZero && (i != pointPosLeft - 1))
		//   不显示0        当前数字为0   不显示前导零   未到达小数点前一位数
		{
			code=' ' - 0x20;//不显示前导零
		}
		//显示小数点前的0
		else if(!showingZero && code == 0 && !leadZero && i == pointPosLeft - 1)
		{
			code= '0' - 0x20;//显示小数点前的0
			showingZero = 1;
		}
		else
		{
			if(code>9){
				// 十六进制显示
				if(numRadix==1){
					if(transformMode==0){
						code += lowerLetterOffset;
					}else if(transformMode ==1){
						code += upperLetterOffset;
					}
				}
			}
			code = code + 0x30/*ASIIC中数字的起始位置*/ - 0x20/*去掉前面控制符*/;

			showingZero = 1;
		}
		// tens =tens /10;
		tens /= radixBase;
		if(overflow == 1)
			texturePtr->FocusedSlice = ' ' - 0x20; 
		else
			texturePtr->FocusedSlice = code;
		texturePtr++;
	}

	//next
	if((p_wptr->NumOfLine & 0xff) == 2) //with animation
	{
		tens=1;
		signFlag = 1 - sign; 
		showingZero = 0;
		numofNumber = (p_wptr->WidgetAttr >>5) & 0xf;


		for(i=0;i<numofNumber;i++) 
			tens *=10;

		if(sign)
		{
			numofNumber++; //表示纹理数量，不是数字数量
			//pointPosLeft++;//小数点左边的字符数，包括符号
		}

		if(pointPosRight != 0 )
		{
			numofNumber++;
			//nextNumOfDisTexture++;
		}

		dataMinus = 0;
		if(sValue[0] < 0)
		{
			dataMinus = 1;
			sValue[0] = - sValue[0];
			//nextNumOfDisTexture++;//显示的纹理数
		}
		// 截断超过显示范围的数字
		uValue[0] = uValue[0] % tens;
		sValue[0] = sValue[0] % tens;
		tens /= 10;
		for(i=0;i<numofNumber;i++)
		{

			if(sign)
			{
				code = (u16)(sValue[0] / tens);		
				sValue[0] %= tens;
			}
			else
			{
				code = (u16)(uValue[0] / tens);
				uValue[0] %= tens;
			}

			//显示负号
			if( sign && dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
				//  有符号数  数据为负     显示前导零    不显示前导零   当前数字为非0 不显示前导零 有小数点            到达小数点前的0的前面
			{
				if(overflow == 1)
					nextTexturePtr->FocusedSlice = ' ' - 0x20;
				else
					nextTexturePtr->FocusedSlice = '-' - 0x20;
				nextTexturePtr++;

				dataMinus = 0;//负号已显示，认为为正数
				signFlag = 1;
				i++;
			}
			//正数的正号不显示
			else if( sign && !dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
				//  有符号数  数据为负     显示前导零    不显示前导零   当前数字为非0 不显示前导零 有小数点            到达小数点前的0的前面
			{
				if(overflow == 1)
					nextTexturePtr->FocusedSlice = ' ' - 0x20;
				else
					nextTexturePtr->FocusedSlice = 0;
				nextTexturePtr++;

				signFlag = 1;
				i++;
			}
			//正数第一位显示空格
			else if( sign && !dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
				//  有符号数  数据为正     显示前导零    不显示前导零   当前数字为非0 不显示前导零 有小数点            到达小数点前的0的前面
			{
				nextTexturePtr->FocusedSlice = ' ' - 0x20;
				nextTexturePtr++;
				signFlag = 1;
				i++;
			}

			//显示小数点
			if(i == pointPosLeft)
			{
				if(overflow == 1)
					nextTexturePtr->FocusedSlice = ' ' - 0x20;
				else
					nextTexturePtr->FocusedSlice = '.' - 0x20;//显示小数点
				nextTexturePtr++;
				offsetpoint = i;
				i++;
			}

			//显示前导零
			if (!showingZero && code == 0 && !leadZero && (i != pointPosLeft - 1))
				//   不显示0        当前数字为0   不显示前导零   未到达小数点前一位数
			{
				code=' ' - 0x20;//不显示前导零
			}
			//显示小数点前的0
			else if(!showingZero && code == 0 && !leadZero && i == pointPosLeft - 1)
			{
				code= '0' - 0x20;//显示小数点前的0
				showingZero = 1;
			}
			else
			{
				code = code + 0x30/*ASIIC中数字的起始位置*/ - 0x20/*去掉前面控制符*/;
				showingZero = 1;
			}
			tens =tens /10;
			if(overflow == 1)
				nextTexturePtr->FocusedSlice = ' ' - 0x20;
			else
				nextTexturePtr->FocusedSlice = code;
			nextTexturePtr++;
		}
	}

	texturePtr = texturePtr - numofNumber;
	if((p_wptr->NumOfLine & 0xff) == 2) //with animation
		nextTexturePtr = nextTexturePtr - numofNumber;
	for(i=0;i<numofNumber;i++)
	{
		texturePtr[i].mTexAttr |= (DRAWING);
		if((p_wptr->NumOfLine & 0xff) == 2) //with animation
			nextTexturePtr[i].mTexAttr |= (DRAWING);
	}

	//判断对齐方式
	if(align == RIGHTALIGN)   //默认右对齐
	{
		difOfNumber = 0;
		if(numofNumber == 1) //only one texture
			widthOfFont = 0;
		else
			widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 2].OffsetX - gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].OffsetX;
		shiftSize = 0;
		//shiftSize  = (shiftSize << 4);  //等价于左移4位
	}
	else if(align == CENTERALIGN)  //中间对齐
	{
		difOfNumber = numofNumber - numOfDisTexture;
		if(numofNumber == 1) //only one texture
			widthOfFont = 0;
		else
			widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 2].OffsetX - gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].OffsetX;
		shiftSize = widthOfFont * difOfNumber;
		shiftSize  = (shiftSize >> 1);			//等价于先左移4位再除以2
		if( (shiftSize & 0x08) != 0)           //尽量不出现小数以免开启双线性滤波
			shiftSize += 0x08;
	}
	else if(align == LEFTALIGN) //左对齐
	{
		difOfNumber = numofNumber - numOfDisTexture;
		if(numofNumber == 1) //only one texture
			widthOfFont = 0;
		else
			widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 2].OffsetX - gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].OffsetX;
		shiftSize = widthOfFont * difOfNumber;
		//shiftSize  = (shiftSize << 4);  //等价于左移4位

	}
	

	//操作纹理的位置
	for(i=0;i<numofNumber;i++)
	{
		
		texturePtr[i].OffsetX = ((s16)p_wptr->WidgetOffsetX) << 4;
		// offetpoint为小数点位置
		if(i <= pointPosLeft)
			texturePtr[i].OffsetX += widthOfFont * i;
		else 
			texturePtr[i].OffsetX += widthOfFont * i - (widthOfFont + 1)/2 + SpacingX/2* 16;
		texturePtr[i].OffsetX -= shiftSize;

		if(texturePtr[i].FocusedSlice == 0) //初始位置不在页面内，但动画后在页面内需要显示 by Mr.z
		{
			texturePtr[i].mTexAttr &= (~DRAWING);
		}
		else 
		{
			texturePtr[i].TexLeftTileBox = texturePtr[i].OffsetX >> 9;
			texturePtr[i].TexRightTileBox = (texturePtr[i].OffsetX + texturePtr[i].TexWidth * 16) >> 9;
		}
	}

	//next
	if((p_wptr->NumOfLine & 0xff) == 2) //with animation
	{
		//判断对齐方式
		if(align == RIGHTALIGN)   //默认右对齐
		{
			difOfNumber = 0;
			//widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].TexWidth;
			shiftSize = widthOfFont * difOfNumber;
			shiftSize  = shiftSize;  //等价于左移4位
		}
		else if(align == CENTERALIGN)  //中间对齐
		{
			difOfNumber = numofNumber - nextNumOfDisTexture;
			//widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].TexWidth;
			shiftSize = widthOfFont * difOfNumber;
			shiftSize  = (shiftSize >> 1);			//等价于除以2
			if( (shiftSize & 0x08) != 0)           //尽量不出现小数以免开启双线性滤波
			shiftSize += 0x08;

		}
		else if(align == LEFTALIGN) //左对齐
		{
			difOfNumber = numofNumber - nextNumOfDisTexture;
			//widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].TexWidth;
			shiftSize = widthOfFont * difOfNumber;
			shiftSize  = (shiftSize);  //等价于左移4位

		}
		for(i=0;i<numofNumber;i++)
		{
			nextTexturePtr[i].OffsetX = ((s16)p_wptr->WidgetOffsetX) << 4;
			//offetpoint为小数点位置
			if(i <= pointPosLeft)
				nextTexturePtr[i].OffsetX += widthOfFont * i;
			else 
				nextTexturePtr[i].OffsetX += widthOfFont * i - (widthOfFont + 1)/2+ SpacingX/2* 16;;
			nextTexturePtr[i].OffsetX -= shiftSize;
			if(nextTexturePtr[i].FocusedSlice == 0) //初始位置不在页面内，但动画后在页面内需要显示 by Mr.z
			{
				nextTexturePtr[i].mTexAttr &= (~DRAWING);
			}
			else 
			{
				nextTexturePtr[i].TexLeftTileBox = nextTexturePtr[i].OffsetX >> 9;
				nextTexturePtr[i].TexRightTileBox = (nextTexturePtr[i].OffsetX + nextTexturePtr[i].TexWidth * 16) >> 9;
			}
		}
	}
	

	//for(i=0;i<numofNumber;i++)
	//{
	//	if((p_wptr->NumOfLine & 0xff) == 1 && !u8_pageRefresh)
	//	{
	//		if(movingDir == 0) //下移
	//		{
	//			texturePtr[i].OffsetY = (p_wptr->WidgetOffsetY - p_wptr->WidgetHeight) << 4;
	//			preTexturePtr[i].OffsetY =  (p_wptr->WidgetOffsetY) << 4;
	//		}
	//		else if(movingDir == 1)//上移
	//		{
	//			texturePtr[i].OffsetY = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight) << 4;
	//			preTexturePtr[i].OffsetY =  (p_wptr->WidgetOffsetY) << 4;
	//		}
	//	}
	//}
	

	if(u8_pageRefresh == 0)
	{
		//更新此控件
		if((p_wptr->NumOfLine & 0xff) == 0 || (p_wptr->NumOfLine & 0xff) == 1) //无切换动画
		{
			//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
			refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
			refreshMsg.mElementPtr.wptr = p_wptr;
			sendToRefreshQueue(&refreshMsg);
#endif
		}
		else if((p_wptr->NumOfLine & 0xff) == 2) //平移切换动画
		{
			//if(movingDir == 0)
			//	p_wptr->ANIMATION_TYPE = ANIMATION_VERTICAL_SHIFTING_DOWN;
			//else 
			//	p_wptr->ANIMATION_TYPE = ANIMATION_VERTICAL_SHIFTING_UP;
			p_wptr->mWidgetMatrix.matrixInit();
			p_wptr->loadTextureWithAnimation();
		}

		if(oldValueinit)
		{
			oldValue = (p_wptr->OldValueH  << 16) + p_wptr->OldValueL;
			//tagchange的支持
			if(oldValue != value){
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->TagChangeAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}
			if(sign)
			{
				if(p_wptr->EnterLowAlarmAction && (s32)oldValue > (s32)lowAlarmValue && (s32)value <= (s32)lowAlarmValue)
				{
					//进入低值预警
					if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}		
				}
				else if(p_wptr->LeaveLowAlarmAction && (s32)oldValue <= (s32)lowAlarmValue && (s32)value > (s32)lowAlarmValue)
				{
					//离开低值预警
					if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}		
				}
				if(p_wptr->EnterHighAlarmAction && (s32)oldValue < (s32)highAlarmValue && (s32)value >= (s32)highAlarmValue)
				{
					//进入高值预警
					if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
					}
				}
				else if(p_wptr->LeaveHighAlarmAction && (s32)oldValue >= (s32)highAlarmValue && (s32)value < (s32)highAlarmValue)
				{
					//离开高值预警
					if(p_wptr->LeaveHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveHighAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}	
				}
			}
			else
			{
				if(p_wptr->EnterLowAlarmAction && oldValue > (u32)lowAlarmValue && value <= (u32)lowAlarmValue)
				{
					//进入低值预警
					if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}		
				}
				else if(p_wptr->LeaveLowAlarmAction && oldValue <= (u32)lowAlarmValue && value > (u32)lowAlarmValue)
				{
					//离开低值预警
					if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}		
				}
				if(p_wptr->EnterHighAlarmAction && oldValue < (u32)highAlarmValue && value >= (u32)highAlarmValue)
				{
					//进入高值预警
					if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
					}
				}
				else if(p_wptr->LeaveHighAlarmAction && oldValue >= (u32)highAlarmValue && value < (u32)highAlarmValue)
				{
					//离开高值预警
					if(p_wptr->LeaveHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveHighAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}	
				}
			}
			
		}
		p_wptr->WidgetAttr |= 0x8000;
		p_wptr->OldValueL = (u16)value;
		p_wptr->OldValueH = (u16)(value >> 16);

	}
	return AHMI_FUNC_SUCCESS;

}

//-----------------------------
// 函数名： NumberClass::initWidget
// 初始化并绘制数字框，如果是全屏刷新，先根据所绑定的tag刷新该控件，在绘制该控件
// 参数列表：
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u8 value                 // the value of ATAG
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus NumberClass::setATag(
	WidgetClassPtr p_wptr,   //控件指针
	u8 value
	)
{
	
//	s16 preTextureOffsetX; //1.11.4
	s16 preTextureOffsetY; //1.11.4
//	s16 nextTextureOffsetX; //1.11.4
	s16 nextTextureOffsetY; //1.11.4
	u16 transitionParamIn; //0.10
	u16 transitionParamOut; //0.10
	u16 widgetHeight;
//	RefreshMsg refreshMsg;
	TextureClassPtr preTexturePtr, nextTexturePtr;
	TagClassPtr  bindTag;
	u32 bindTagValue;
	u8 numofNumber;
	u8 totalFrame;
	u8 pointPosRight;
	u8 sign;
	u8 i;
	s32 maxValue;
	s32 minValue;
	


	totalFrame = (u8)( (p_wptr->TOTALFRAME_AND_NOWFRAME) >> 8);
	numofNumber = (p_wptr->WidgetAttr >>5) & 0xf;
	pointPosRight = (p_wptr->WidgetAttr >>9) & 0xf;
	sign = (p_wptr->WidgetAttr & 0x4000 )?1:0;
	widgetHeight = p_wptr->WidgetHeight;
	bindTag = &TagPtr[p_wptr->BindTagID];
	bindTagValue = (u32)(bindTag->getValue());

	if(totalFrame == 0 || ( (p_wptr->NumOfLine & 0xff) == 1) ) //no animation
		return AHMI_FUNC_SUCCESS;

	taskENTER_CRITICAL();
#ifdef AHMI_DEBUG
	char text[100];
	sprintf(text,"set widget tag %d", value);
	ERROR_PRINT(text);
#endif
	p_wptr->TOTALFRAME_AND_NOWFRAME |= value;
	if(sign)
	{
		numofNumber++; //表示纹理数量，不是数字数量
	}

	if(pointPosRight != 0 )
	{
		numofNumber++;
	}
	//caculating the moving parameter
	transitionParamIn = (value) * 1024 / totalFrame;//0.10
	gPagePtr[WorkingPageID].inoutTransitioin(transitionParamIn, transitionParamOut);
//	preTextureOffsetX = 0;
//	nextTextureOffsetX = 0;
	if(p_wptr -> PreviousTexturePtrFlag == 0) //动画下移
	{
		preTextureOffsetY = ((widgetHeight) * transitionParamOut) >> 6;   //0->h
		nextTextureOffsetY = (s16)( (((widgetHeight) * transitionParamOut) >> 6) - (widgetHeight << 4) ); //-h ->0
	}
	else //动画上移
	{
		preTextureOffsetY =  - ( ((widgetHeight) * transitionParamOut) >> 6); //0->-h
		nextTextureOffsetY = (s16)( (widgetHeight << 4)  -(((widgetHeight) * transitionParamOut) >> 6) ); //h->0
	}

	preTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1]);
	nextTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + numofNumber + 1]);

	//modify the position
	for(i=0;i<numofNumber;i++)
	{
		preTexturePtr[i].OffsetY = ( (p_wptr->WidgetOffsetY) << 4 ) + preTextureOffsetY;
		nextTexturePtr[i].OffsetY = ( (p_wptr->WidgetOffsetY) << 4 ) + nextTextureOffsetY;
	}

	//modify the value
	if( value == ((p_wptr->TOTALFRAME_AND_NOWFRAME) >> 8)) //the last frame
	{
		maxValue = (s32)((p_wptr->MaxValueH << 16) + p_wptr->MaxValueL);
		minValue = (s32)((p_wptr->MinValueH << 16) + p_wptr->MinValueL);
		if(sign)
		{
			if((s32)bindTagValue > (s32)maxValue)
			{
				bindTagValue = (s32)maxValue;
			}
			if((s32)bindTagValue < (s32)minValue)
			{
				bindTagValue = (s32)minValue;
			}
		}
		else
		{
			if((u32)bindTagValue > (u32)maxValue)
			{
				bindTagValue = (u32)maxValue;
			}
			if((u32)bindTagValue < (u32)minValue)
			{
				bindTagValue = (u32)minValue;
			}
		}
		p_wptr->CurValueL = (u16)bindTagValue;
		p_wptr->CurValueH = (u16)(bindTagValue >> 16);
	}

	taskEXIT_CRITICAL();
#ifndef WHOLE_TRIBLE_BUFFER
	refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
	refreshMsg.mElementPtr.wptr = p_wptr;
	sendToRefreshQueue(&refreshMsg);
#endif
	return AHMI_FUNC_SUCCESS;
}


#endif
