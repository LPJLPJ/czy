////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 3.00 - File Created 2016/04/13 by �ڴ�Ӫ
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
// �������� NumberClass
// ���캯��
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
NumberClass::NumberClass()
{

}

//-----------------------------
// �������� ~NumberClass
// ��������
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
NumberClass::~NumberClass()
{

}


//-----------------------------
// �������� NumberClass::initWidget
// ��ʼ�����������ֿ������ȫ��ˢ�£��ȸ������󶨵�tagˢ�¸ÿؼ����ڻ��Ƹÿؼ�
// �����б�
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //ҳ��ˢ��
//  @param   TileBoxClassPtr pTileBox //��Χ��
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus NumberClass::initWidget(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u32 *u32p_sourceShift,   //sourcebufferָ��
	u8 u8_pageRefresh,       //ҳ��ˢ��
	u8 RefreshType ,       //���ƵĶ������ͣ����ݶ������͸ı���ƿؼ��İ�Χ��
	TileBoxClassPtr pTileBox, //��Χ��
	u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
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
//���ֿ�Ϊ0x8���ֿ���ƺ���������
//��ʾN�����֣���ҪN+2������
//֧��С����һ��С���㣩,N+3������
//ֻ֧��tag�������͡�
//��һ������Ϊȡ�ǵĴ���Ĥ�����������alpha buffer
//��Ϊ�з���������Ҫһ�������ʾ���ţ�֮��N��N+1)�������ӦN�����֣���1��С���㣩��ָ���ֿ��е�λ�ã���������ΪALPHA1��˳���������ҡ�
//���һ������Ϊ��ɫ�������������
//WidgetAttr��ʶ��
//14:������ʾģʽ��0Ϊ�޷���������Χ4294967295~0,1Ϊ�з���������Χ2147483647~-2147483648
//13:ǰ������ʾģʽ��0Ϊ����ʾǰ���㣬1Ϊ��ʾǰ����
//12-9:�м�λС������С�����������С���ֵ���߼�λ������2��ʾ����λС��
//8-5:��ʾ���ַ���������������λ��С����
//4-0:�ؼ����ͣ����ֿ�Ϊ0x8
//**********************************
funcStatus NumberClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
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
	u8	showingZero; //��ʾ�Ƿ���Ҫ��ʾ����0���ڲ���ʾǰ���������£���Ҫ�ô˱�����ʾ����0�Ƿ���ʾ.
	u8	signFlag; //�Ƿ��Ѿ���ʾ����
	                  // 0��ʾ����ʾ0��1��ʾ��ʾ0
	u8	dataMinus; // ��ʾ����ʾ���ֵ������ԣ�1��ʾ������0��ʾ�����������Ѿ���ӡ
	u8  numOfDisTexture;
	u8  nextNumOfDisTexture;
	u8  difOfNumber;
	u16 widthOfFont;
	u16 shiftSize;
	u16 code;

	u8 overflow;
	u8 align;
	u8 overflowStyle;    //���ݳ������ֵ��Ĵ���ʽ 0����ʾ���ֵ��ģʽ��ʽ 1������ʾ����
	u8 numRadix;    // ���ֽ��� 0-ʮ����  1-ʮ������
	u8 markingMode; // ʮ�������Ƿ���ʾ0x   0-��ʾ  1-����ʾ
	u8 transformMode; // ��ĸ�Ƿ��д��ʾ  0-Сд  1-��д
	u8 radixBase = 10;
	u8 upperLetterOffset = 7;   // ��д��ĸ�����ֵ�ƫ����   
	u8 lowerLetterOffset = 39;  // Сд��ĸ��ƫ����

	u64 tens;
	TextureClassPtr nextTexturePtr, texturePtr;
	u32 oldValue=0;
	u16 oldValueinit=0;
	u8 movingDir;  //0��ʾ���ƣ�1��ʾ����

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

	//Ŀǰ����ֻ����1-10λ���֣�С��λ��(0,����λ����1)
	if(numofNumber > 10 || numofNumber < 1){
		ERROR_PRINT("ERROR: don't support such length of number");
		return AHMI_FUNC_FAILURE;
	}
	//С��λ�����ܳ������ֵ�λ�����Ҳ�С��0
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
		if(pointPosRight != 0) //������ʾС����1����
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
		if(pointPosRight != 0) //������ʾС����1����
			if(nextNumOfDisTexture < pointPosRight+1)
				nextNumOfDisTexture = pointPosRight+1;
	}
	else 
		nextNumOfDisTexture = numofNumber;

	//���ݳ������ֵ��Ĵ���ʽ 0����ʾ���ֵ��ģʽ��ʽ 1������ʾ����
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

	pointPosLeft = numofNumber - pointPosRight;//С������ߵ��ַ�������������

	if(pointPosLeft <1 ) pointPosLeft = 1;

	if(sign)
	{
		numofNumber++; //��ʾ����������������������
		pointPosLeft++;//С������ߵ��ַ�������������
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

	gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].mTexAttr |= TEXTURE_USING_WIDGET_BOX; //puremask��һ�����ʹ�ÿؼ��İ�Χ��
	if((p_wptr->NumOfLine & 0xff) == 0 || (p_wptr->NumOfLine & 0xff) == 1)   //�޶����ķ�ʽ�л�����
	{
		texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex+1]);
		#ifdef EMBEDDED
		if((texturePtr < (void*)startOfDynamicPage) || (texturePtr > (void*)endOfDynamicPage))
			return AHMI_FUNC_FAILURE;
		#endif

	}
	else if((p_wptr->NumOfLine & 0xff) == 2) //�Զ�����ʽ�л�����
	{
		texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1]);
		nextTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + numofNumber + 1]);
	}
	else 
		return AHMI_FUNC_FAILURE;


	//�ж�ƽ�Ʒ���
	uValue[0] = (u32)value;
	sValue[0] = (s32)value;
	uValue[1] = (u32)curValue;
	sValue[1] = (s32)curValue;

    //bug ����
	if((p_wptr->NumOfLine & 0xff) == 2) {
		if(sign)
		{
			if(sValue[0] > (s32)curValue)
			{
				//����
				movingDir = 0;
			}
			else if(sValue[0] < (s32)curValue)
			{
				 //����
				movingDir = 1;
			}
			else
			{
				//���ƶ�
				if(u8_pageRefresh == 0) 
					return AHMI_FUNC_SUCCESS;
			}
		}
		else
		{
			if(uValue[0] > (u32)curValue)
			{
				//����
				movingDir = 0;
			}
			else if(uValue[0] < (u32)curValue)
			{
				 //����
				movingDir = 1;
			}
			else
			{
				//���ƶ�
				if(u8_pageRefresh == 0)
					return AHMI_FUNC_SUCCESS;
			}
		}
	}
	else{
		movingDir = 0;
	}
	if(movingDir == 0) //����
	{
		p_wptr->PreviousTexturePtrFlag = 0;
	}
	else if(movingDir == 1)//����
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

	// �ضϳ�����ʾ��Χ������
	uValue[1] = uValue[1] % tens;
	sValue[1] = sValue[1] % tens;
	// tens /= 10;
	tens /= radixBase;
	

	//���������ֵ
	//pre
	for(i=0;i<numofNumber;i++)
	{
		// ���ڲ����ַ������Ƿ�׼ȷ
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

		//��ʾ����
		if( sign && dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
		//  �з�����  ����Ϊ��     ��ʾǰ����    ����ʾǰ����   ��ǰ����Ϊ��0 ����ʾǰ���� ��С����            ����С����ǰ��0��ǰ��
		{
			if(overflow == 1)
				texturePtr->FocusedSlice = ' ' - 0x20;
			else
				texturePtr->FocusedSlice = '-' - 0x20;
			texturePtr++;

			dataMinus = 0;//��������ʾ����ΪΪ����
			signFlag = 1;
			i++;
		}
		//��ʾ����
		else if( sign && !dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
		//  �з�����  ����Ϊ��     ��ʾǰ����    ����ʾǰ����   ��ǰ����Ϊ��0 ����ʾǰ���� ��С����            ����С����ǰ��0��ǰ��
		{
			if(overflow == 1)
				texturePtr->FocusedSlice = ' ' - 0x20;
			else
				texturePtr->FocusedSlice = 0;
			texturePtr++;

			signFlag = 1;
			i++;
		}
		//������һλ��ʾ�ո�
		else if( sign && !dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
			//  �з�����  ����Ϊ��     ��ʾǰ����    ����ʾǰ����   ��ǰ����Ϊ��0 ����ʾǰ���� ��С����            ����С����ǰ��0��ǰ��
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

		//��ʾС����
		if(i == pointPosLeft)
		{
			if(overflow == 1)
				texturePtr->FocusedSlice = ' ' - 0x20;
			else
				texturePtr->FocusedSlice = '.' - 0x20;//��ʾС����
			texturePtr++;
			offsetpoint = i;
			i++;
		}

			

		//��ʾǰ����
		if (!showingZero && code == 0 && !leadZero && (i != pointPosLeft - 1))
		//   ����ʾ0        ��ǰ����Ϊ0   ����ʾǰ����   δ����С����ǰһλ��
		{
			code=' ' - 0x20;//����ʾǰ����
		}
		//��ʾС����ǰ��0
		else if(!showingZero && code == 0 && !leadZero && i == pointPosLeft - 1)
		{
			code= '0' - 0x20;//��ʾС����ǰ��0
			showingZero = 1;
		}
		else
		{
			if(code>9){
				// ʮ��������ʾ
				if(numRadix==1){
					if(transformMode==0){
						code += lowerLetterOffset;
					}else if(transformMode ==1){
						code += upperLetterOffset;
					}
				}
			}
			code = code + 0x30/*ASIIC�����ֵ���ʼλ��*/ - 0x20/*ȥ��ǰ����Ʒ�*/;

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
			numofNumber++; //��ʾ����������������������
			//pointPosLeft++;//С������ߵ��ַ�������������
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
			//nextNumOfDisTexture++;//��ʾ��������
		}
		// �ضϳ�����ʾ��Χ������
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

			//��ʾ����
			if( sign && dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
				//  �з�����  ����Ϊ��     ��ʾǰ����    ����ʾǰ����   ��ǰ����Ϊ��0 ����ʾǰ���� ��С����            ����С����ǰ��0��ǰ��
			{
				if(overflow == 1)
					nextTexturePtr->FocusedSlice = ' ' - 0x20;
				else
					nextTexturePtr->FocusedSlice = '-' - 0x20;
				nextTexturePtr++;

				dataMinus = 0;//��������ʾ����ΪΪ����
				signFlag = 1;
				i++;
			}
			//���������Ų���ʾ
			else if( sign && !dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
				//  �з�����  ����Ϊ��     ��ʾǰ����    ����ʾǰ����   ��ǰ����Ϊ��0 ����ʾǰ���� ��С����            ����С����ǰ��0��ǰ��
			{
				if(overflow == 1)
					nextTexturePtr->FocusedSlice = ' ' - 0x20;
				else
					nextTexturePtr->FocusedSlice = 0;
				nextTexturePtr++;

				signFlag = 1;
				i++;
			}
			//������һλ��ʾ�ո�
			else if( sign && !dataMinus && (leadZero || ( !leadZero && code != 0 ) || (!leadZero && /*PointPosRight != 0 &&*/ i == pointPosLeft - 2) ) && !signFlag)
				//  �з�����  ����Ϊ��     ��ʾǰ����    ����ʾǰ����   ��ǰ����Ϊ��0 ����ʾǰ���� ��С����            ����С����ǰ��0��ǰ��
			{
				nextTexturePtr->FocusedSlice = ' ' - 0x20;
				nextTexturePtr++;
				signFlag = 1;
				i++;
			}

			//��ʾС����
			if(i == pointPosLeft)
			{
				if(overflow == 1)
					nextTexturePtr->FocusedSlice = ' ' - 0x20;
				else
					nextTexturePtr->FocusedSlice = '.' - 0x20;//��ʾС����
				nextTexturePtr++;
				offsetpoint = i;
				i++;
			}

			//��ʾǰ����
			if (!showingZero && code == 0 && !leadZero && (i != pointPosLeft - 1))
				//   ����ʾ0        ��ǰ����Ϊ0   ����ʾǰ����   δ����С����ǰһλ��
			{
				code=' ' - 0x20;//����ʾǰ����
			}
			//��ʾС����ǰ��0
			else if(!showingZero && code == 0 && !leadZero && i == pointPosLeft - 1)
			{
				code= '0' - 0x20;//��ʾС����ǰ��0
				showingZero = 1;
			}
			else
			{
				code = code + 0x30/*ASIIC�����ֵ���ʼλ��*/ - 0x20/*ȥ��ǰ����Ʒ�*/;
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

	//�ж϶��뷽ʽ
	if(align == RIGHTALIGN)   //Ĭ���Ҷ���
	{
		difOfNumber = 0;
		if(numofNumber == 1) //only one texture
			widthOfFont = 0;
		else
			widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 2].OffsetX - gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].OffsetX;
		shiftSize = 0;
		//shiftSize  = (shiftSize << 4);  //�ȼ�������4λ
	}
	else if(align == CENTERALIGN)  //�м����
	{
		difOfNumber = numofNumber - numOfDisTexture;
		if(numofNumber == 1) //only one texture
			widthOfFont = 0;
		else
			widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 2].OffsetX - gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].OffsetX;
		shiftSize = widthOfFont * difOfNumber;
		shiftSize  = (shiftSize >> 1);			//�ȼ���������4λ�ٳ���2
		if( (shiftSize & 0x08) != 0)           //����������С�����⿪��˫�����˲�
			shiftSize += 0x08;
	}
	else if(align == LEFTALIGN) //�����
	{
		difOfNumber = numofNumber - numOfDisTexture;
		if(numofNumber == 1) //only one texture
			widthOfFont = 0;
		else
			widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 2].OffsetX - gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].OffsetX;
		shiftSize = widthOfFont * difOfNumber;
		//shiftSize  = (shiftSize << 4);  //�ȼ�������4λ

	}
	

	//���������λ��
	for(i=0;i<numofNumber;i++)
	{
		
		texturePtr[i].OffsetX = ((s16)p_wptr->WidgetOffsetX) << 4;
		// offetpointΪС����λ��
		if(i <= pointPosLeft)
			texturePtr[i].OffsetX += widthOfFont * i;
		else 
			texturePtr[i].OffsetX += widthOfFont * i - (widthOfFont + 1)/2 + SpacingX/2* 16;
		texturePtr[i].OffsetX -= shiftSize;

		if(texturePtr[i].FocusedSlice == 0) //��ʼλ�ò���ҳ���ڣ�����������ҳ������Ҫ��ʾ by Mr.z
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
		//�ж϶��뷽ʽ
		if(align == RIGHTALIGN)   //Ĭ���Ҷ���
		{
			difOfNumber = 0;
			//widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].TexWidth;
			shiftSize = widthOfFont * difOfNumber;
			shiftSize  = shiftSize;  //�ȼ�������4λ
		}
		else if(align == CENTERALIGN)  //�м����
		{
			difOfNumber = numofNumber - nextNumOfDisTexture;
			//widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].TexWidth;
			shiftSize = widthOfFont * difOfNumber;
			shiftSize  = (shiftSize >> 1);			//�ȼ��ڳ���2
			if( (shiftSize & 0x08) != 0)           //����������С�����⿪��˫�����˲�
			shiftSize += 0x08;

		}
		else if(align == LEFTALIGN) //�����
		{
			difOfNumber = numofNumber - nextNumOfDisTexture;
			//widthOfFont = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].TexWidth;
			shiftSize = widthOfFont * difOfNumber;
			shiftSize  = (shiftSize);  //�ȼ�������4λ

		}
		for(i=0;i<numofNumber;i++)
		{
			nextTexturePtr[i].OffsetX = ((s16)p_wptr->WidgetOffsetX) << 4;
			//offetpointΪС����λ��
			if(i <= pointPosLeft)
				nextTexturePtr[i].OffsetX += widthOfFont * i;
			else 
				nextTexturePtr[i].OffsetX += widthOfFont * i - (widthOfFont + 1)/2+ SpacingX/2* 16;;
			nextTexturePtr[i].OffsetX -= shiftSize;
			if(nextTexturePtr[i].FocusedSlice == 0) //��ʼλ�ò���ҳ���ڣ�����������ҳ������Ҫ��ʾ by Mr.z
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
	//		if(movingDir == 0) //����
	//		{
	//			texturePtr[i].OffsetY = (p_wptr->WidgetOffsetY - p_wptr->WidgetHeight) << 4;
	//			preTexturePtr[i].OffsetY =  (p_wptr->WidgetOffsetY) << 4;
	//		}
	//		else if(movingDir == 1)//����
	//		{
	//			texturePtr[i].OffsetY = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight) << 4;
	//			preTexturePtr[i].OffsetY =  (p_wptr->WidgetOffsetY) << 4;
	//		}
	//	}
	//}
	

	if(u8_pageRefresh == 0)
	{
		//���´˿ؼ�
		if((p_wptr->NumOfLine & 0xff) == 0 || (p_wptr->NumOfLine & 0xff) == 1) //���л�����
		{
			//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
			refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
			refreshMsg.mElementPtr.wptr = p_wptr;
			sendToRefreshQueue(&refreshMsg);
#endif
		}
		else if((p_wptr->NumOfLine & 0xff) == 2) //ƽ���л�����
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
			//tagchange��֧��
			if(oldValue != value){
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->TagChangeAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}
			if(sign)
			{
				if(p_wptr->EnterLowAlarmAction && (s32)oldValue > (s32)lowAlarmValue && (s32)value <= (s32)lowAlarmValue)
				{
					//�����ֵԤ��
					if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}		
				}
				else if(p_wptr->LeaveLowAlarmAction && (s32)oldValue <= (s32)lowAlarmValue && (s32)value > (s32)lowAlarmValue)
				{
					//�뿪��ֵԤ��
					if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}		
				}
				if(p_wptr->EnterHighAlarmAction && (s32)oldValue < (s32)highAlarmValue && (s32)value >= (s32)highAlarmValue)
				{
					//�����ֵԤ��
					if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
					}
				}
				else if(p_wptr->LeaveHighAlarmAction && (s32)oldValue >= (s32)highAlarmValue && (s32)value < (s32)highAlarmValue)
				{
					//�뿪��ֵԤ��
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
					//�����ֵԤ��
					if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}		
				}
				else if(p_wptr->LeaveLowAlarmAction && oldValue <= (u32)lowAlarmValue && value > (u32)lowAlarmValue)
				{
					//�뿪��ֵԤ��
					if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
					}		
				}
				if(p_wptr->EnterHighAlarmAction && oldValue < (u32)highAlarmValue && value >= (u32)highAlarmValue)
				{
					//�����ֵԤ��
					if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
					{
						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
					}
				}
				else if(p_wptr->LeaveHighAlarmAction && oldValue >= (u32)highAlarmValue && value < (u32)highAlarmValue)
				{
					//�뿪��ֵԤ��
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
// �������� NumberClass::initWidget
// ��ʼ�����������ֿ������ȫ��ˢ�£��ȸ������󶨵�tagˢ�¸ÿؼ����ڻ��Ƹÿؼ�
// �����б�
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u8 value                 // the value of ATAG
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus NumberClass::setATag(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
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
		numofNumber++; //��ʾ����������������������
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
	if(p_wptr -> PreviousTexturePtrFlag == 0) //��������
	{
		preTextureOffsetY = ((widgetHeight) * transitionParamOut) >> 6;   //0->h
		nextTextureOffsetY = (s16)( (((widgetHeight) * transitionParamOut) >> 6) - (widgetHeight << 4) ); //-h ->0
	}
	else //��������
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
