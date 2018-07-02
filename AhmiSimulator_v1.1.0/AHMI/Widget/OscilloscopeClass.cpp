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
#include "OscilloscopeClass.h"
#include "drawImmediately_cd.h"
#include "myMathClass.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;
extern u8					sourceBuffer[2048];

//-----------------------------
// �������� OSCClass
// ���캯��
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
OSCClass::OSCClass()
{

}

//-----------------------------
// �������� ~OSCClass
// ��������
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
OSCClass::~OSCClass()
{

}

//****************************
// �������� OSCClass::initWidget
// ��ʼ��������ʱ��������ȫ��ˢ�£��ȸ������󶨵�tagˢ�¸ÿؼ����ڻ��Ƹÿؼ�
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //ҳ��ˢ��
//  @param   TileBoxClassPtr pTileBox //��Χ��
// ***************************
funcStatus OSCClass::initWidget(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u32 *u32p_sourceShift,   //sourcebufferָ��
	u8 u8_pageRefresh,       //ҳ��ˢ��
	u8 RefreshType ,       //���ƵĶ������ͣ����ݶ������͸ı���ƿؼ��İ�Χ��
	TileBoxClassPtr pTileBox, //��Χ��
	u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
	)

{
	TextureClass newTexture;
	//s16 A,B,E,F;
//	u16 texAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE;
	//s16 Height,Width;
	s32 height0,width0;
	s16 COS,SIN;
	s32 COS0,SIN0;
	u8   lineWidth;
	u8   stepX;
	u16* samplePointBuf;
	u16  SampleBufSize;
	u16 curposi;//��ǰ�����
	u16 curposX; //��ǰ��X����
	
	//u16 EndPosX;//������X����
	u16 endPosi;//���������

//	u16 startPosX;//��ʼ��X����
	s16 startPosi;//��ʼ�����

	u8 p1,p2;
	int i;
//	u8 flag=0;
	u8 *sourcebufferaddr;
	u8 breakLine;
	u8 crossTile;//���ڴ���Tile�ֽ����ϵ��ߣ���Ҫ������Tile������
	matrixClass matrixTemp;
	myMathClass mathTemp;
	float f_tany;
	s16   s_tany;

	if(NULL == p_wptr)
		return AHMI_FUNC_FAILURE;

	//д�뱳���ļ�
	matrixTemp.matrixInit();
	newTexture = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + p_wptr->NumOfTex - 1];
	newTexture.writeSourceBuffer(u32p_sourceShift,&matrixTemp,p_wptr,NULL);


	lineWidth = (p_wptr->WidgetAttr >>12) & 0x7;
	stepX = (p_wptr->WidgetAttr>>5);
	stepX = stepX  & 0x3f;
	samplePointBuf=(u16*)&(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
	SampleBufSize = ( (p_wptr->NumOfTex - 2) * sizeof(TextureClass)) >> 1;
	curposi = p_wptr->CurPosI - 1;
	curposX = curposi * stepX  + p_wptr->WidgetOffsetX ; 


	matrixTemp.matrixInit();								//��ʼ��Ϊ��λ��
	sourcebufferaddr = (u8 *)SoureBufferAddr;
	breakLine = (u8)0;//��ǰ�ĵ�

	if(curposi!=0 && (curposX>>5) != ((curposX - stepX)>>5))//��Ҫ�����߿�Խ��2��tile
	{
		crossTile = 1;
	}
	else crossTile = 0;

//	startPosX = curposX & (~0x1f) ;// ȡTile�߽磬Tile��С32
	if(crossTile) 
		startPosi = curposi - ((curposX & 0x1f) + 32 + stepX -1)/stepX;
	else	
		startPosi = curposi - ((curposX & 0x1f) + stepX -1)/stepX;//Curposi - ((CurposX - StartPosX + StepX-1)/StepX )

	if(startPosi<0) 
		startPosi = 0;



	if(p_wptr->WidgetAttr & 0x800)
	{
		/*if(crossTile) 
			endPosi = startPosi + (32+31)/stepX + 1;
		else 
			endPosi = startPosi + 31/stepX + 1;	
		breakLine = 1;*/
		endPosi = startPosi = 0;
		p_wptr->WidgetAttr = p_wptr->WidgetAttr & (~0x800);
		p_wptr->CurPosI = 0;
		return AHMI_FUNC_SUCCESS;
	}
	else
	{
		endPosi = curposi;
		breakLine = 0;
	}
	if(endPosi>=SampleBufSize || endPosi >= (p_wptr->WidgetWidth - p_wptr->BLANK_X)/stepX) 
		endPosi = ( (p_wptr->WidgetWidth) - (p_wptr->BLANK_X) )/ stepX - 1;

	//���alpha buffer
	newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE | MASKTYPE_OVERLAP ;
	newTexture.TexWidth = 0;
	newTexture.TexHeight = 0;
	newTexture.OffsetX = 0;
	newTexture.OffsetY = 0;
	/*newTexture.TexLeftTileBox = curposX>>5;
	if(crossTile) 
		newTexture.TexLeftTileBox --;
	newTexture.TexTopTileBox = (p_wptr->WidgetOffsetY) >>5;
	newTexture.TexRightTileBox = (curposX)>>5;
	newTexture.TexButtomTileBox = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight + lineWidth)>>5;*/
	newTexture.TexLeftTileBox = pTileBox->LeftBox;
	newTexture.TexRightTileBox = pTileBox->RightBox;
	newTexture.TexTopTileBox = pTileBox->TopBox;
	newTexture.TexButtomTileBox = pTileBox->ButtomBox;

	newTexture.writeSourceBuffer(u32p_sourceShift,  &matrixTemp,p_wptr,NULL);

	//д��ֱ�ߵ�alpha+����ɫ
	if(startPosi == endPosi)//��һ������
	{
		newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE | MASKTYPE_OVERLAP;
		newTexture.TexWidth = lineWidth;
		newTexture.TexHeight = lineWidth;
		newTexture.OffsetX = (curposX <<4);
		newTexture.OffsetY = (samplePointBuf[startPosi] <<4);	
		newTexture.TexLeftTileBox = (curposX)>>5;
		newTexture.TexTopTileBox =  (samplePointBuf[startPosi])>>5;
		newTexture.TexRightTileBox = (curposX)>>5;
		newTexture.TexButtomTileBox = (samplePointBuf[startPosi] + lineWidth)>>5;
		newTexture.RotateAngle = 0;


		newTexture.writeSourceBuffer( u32p_sourceShift,  &matrixTemp,p_wptr,NULL);

	}
	else
	{

		for(i=startPosi+1;i <= endPosi;i++)
		{
			if(breakLine && i-1 == curposi) continue;
			p1 = i-1;
			p2 = i;
			newTexture.OffsetX = ((p_wptr->WidgetOffsetX + p1*stepX ) <<4 );
			newTexture.OffsetY = (samplePointBuf[p1] <<4 );
			
			//���������
			width0 = (1 + stepX)<<6;//1.25.6
			height0 = (samplePointBuf[p2] - samplePointBuf[p1])<<6;//1.25.6
			

			//�����Χ��
			if(height0>=0)//���߷�������
			{

				newTexture.TexLeftTileBox =   (curposX)>>5;
				if(crossTile) 
					newTexture.TexLeftTileBox --;
				newTexture.TexTopTileBox =    (samplePointBuf[p1])>>5;
				newTexture.TexRightTileBox =  (curposX)>>5;
				newTexture.TexButtomTileBox = (samplePointBuf[p2]+lineWidth)>>5;
			}
			else//���߷�������
			{

				newTexture.TexLeftTileBox =   (curposX)>>5;
				if(crossTile) 
					newTexture.TexLeftTileBox --;
				newTexture.TexTopTileBox =    (samplePointBuf[p2])>>5;
				newTexture.TexRightTileBox =  (curposX)>>5;
				newTexture.TexButtomTileBox = (samplePointBuf[p1]+lineWidth)>>5;
			}

			COS0 = width0<<9;  //1.16.15
			SIN0 = height0<<9; //1.16.15
			mathTemp.FindAngle(&width0,&height0);
			COS0 /=width0;	//1.22.9
			SIN0 /=width0;	//1.22.9
			COS = (s16)COS0; //1.6.9
			SIN = -(s16)SIN0; //1.6.9
			newTexture.TexWidth = (s16)((width0)>>6);
			newTexture.TexHeight =(s16)(lineWidth);

			//д��ֱ�ߵ�alpha����
			if(i == startPosi+1)
				newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE | MASKTYPE_OVERLAP ;
			else 
				newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABEFMATRIX | NONADDRTEXTURE | MASKTYPE_OR ;

			//writing headers
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.mTexAttr & 0xff);//texfisrtheader
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((newTexture.mTexAttr >> 8) & 0xff);//texsecondheader

			//writing boxsize
			*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexLeftTileBox;//startx
			*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexTopTileBox; //starty
			*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexRightTileBox; //stopx
			*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexButtomTileBox;//stopy
								   
			//writing texsize	  
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth >> 8 & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight >> 8 & 0xff);
							
			//writing matrix
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(COS & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(COS >> 8 & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(SIN & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(SIN >> 8 & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) >> 8 & 0xff); 
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) & 0xff);
			*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) >> 8 & 0xff);

		}
	}

	newTexture.mTexAttr = PURECOLOR | MASKTEX | EFMATRIX | ADDRTEXTURE;
	newTexture.TexWidth = p_wptr->WidgetWidth;
	newTexture.TexHeight = p_wptr->WidgetHeight;
	newTexture.OffsetX = (p_wptr->WidgetOffsetX)<<4;
	newTexture.OffsetY = (p_wptr->WidgetOffsetY)<<4;
	newTexture.TexLeftTileBox =  (p_wptr->WidgetOffsetX  )>>5;
	newTexture.TexTopTileBox =  (p_wptr->WidgetOffsetY   )>>5;
	newTexture.TexRightTileBox = (p_wptr->WidgetOffsetX + p_wptr->WidgetWidth )>>5 ;
	newTexture.TexButtomTileBox = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight)>>5 ;
	newTexture.TexAddr = ((p_wptr->OSCColor & 0xf800) << 8) //r
						+((p_wptr->OSCColor & 0x07e0) << 5) //g
						+((p_wptr->OSCColor & 0x001f) << 3) //b
						+0xff000000;
	newTexture.SingleSliceSize = 0;
	newTexture.FocusedSlice = 0;

	newTexture.writeSourceBuffer( u32p_sourceShift,&matrixTemp,p_wptr,NULL);

	//д����е�alpha��������ʾ����
	if(startPosi == endPosi)//��һ������
	{
	}
	else
	{
		for(i=startPosi+1;i <= endPosi;i++)
		{
			if(breakLine && i-1 == curposi) continue;
			p1 = i-1;
			p2 = i;
			
			//���������
			width0 = (1 + stepX)<<6;//1.25.6
			height0 = (samplePointBuf[p2] - samplePointBuf[p1])<<6;//1.25.6
			f_tany = (float)(-height0 * 1.0) / width0;
			mathTemp.transferFloatToS16(f_tany, &s_tany);

			if(i == startPosi+1)
				newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABCDEFMATRIX | NONADDRTEXTURE | MASKTYPE_OVERLAP ;
			else 
				newTexture.mTexAttr = PUREMASK | NONMASKTEX | ABCDEFMATRIX | NONADDRTEXTURE | MASKTYPE_OR ;

			

			newTexture.TexWidth = stepX * (p2 - p1);
			newTexture.TexHeight = (p_wptr->WidgetHeight) + height0 + (p_wptr->BLANK_Y);	

			//�����Χ��
			if(height0>=0)//���߷�������
			{

				newTexture.TexLeftTileBox =   (curposX)>>5;
				if(crossTile) 
					newTexture.TexLeftTileBox --;
				newTexture.TexTopTileBox =    (samplePointBuf[p1])>>5;
				newTexture.TexRightTileBox =  (curposX)>>5;
				newTexture.TexButtomTileBox = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight)>>5;
				newTexture.OffsetX = ((p_wptr->WidgetOffsetX + p1*stepX) <<4 );
				newTexture.OffsetY = (samplePointBuf[p1] <<4 );
				

				//writing headers
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.mTexAttr & 0xff);//texfisrtheader
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((newTexture.mTexAttr >> 8) & 0xff);//texsecondheader

				//writing boxsize
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexLeftTileBox;//startx
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexTopTileBox; //starty
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexRightTileBox; //stopx
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexButtomTileBox;//stopy
									   
				//writing texsize	  
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth >> 8 & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight >> 8 & 0xff);
								
				//writing matrix(����)
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(512 & 0xff);      //A
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( (512 >> 8) & 0xff); //A
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( s_tany & 0xff) ;       //B
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(( (s_tany)>> 8) & 0xff);        //B
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)0;//(s_tany & 0xff);        //C
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)0;//( (s_tany >> 8) & 0xff);        //C
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(512 & 0xff);      //D
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( (512 >> 8) & 0xff); //D
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) >> 8 & 0xff); 
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) >> 8 & 0xff);

			}
			else//���߷�������
			{

				newTexture.TexLeftTileBox =   (curposX)>>5;
				if(crossTile) 
					newTexture.TexLeftTileBox --;
				newTexture.TexTopTileBox =    (samplePointBuf[p2])>>5;
				newTexture.TexRightTileBox =  (curposX)>>5;
				newTexture.TexButtomTileBox = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight)>>5;
				newTexture.OffsetX = ((p_wptr->WidgetOffsetX + p1*stepX) <<4 );
				newTexture.OffsetY = (samplePointBuf[p1] <<4);

				//writing headers
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.mTexAttr & 0xff);//texfisrtheader
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((newTexture.mTexAttr >> 8) & 0xff);//texsecondheader

				//writing boxsize
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexLeftTileBox;//startx
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexTopTileBox; //starty
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexRightTileBox; //stopx
				*(sourcebufferaddr + (*u32p_sourceShift)++) = newTexture.TexButtomTileBox;//stopy
									   
				//writing texsize	  
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexWidth >> 8 & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(newTexture.TexHeight >> 8 & 0xff);
								
				//writing matrix(����)
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(512 & 0xff);      //A
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( (512 >> 8) & 0xff); //A
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( s_tany & 0xff) ;       //B
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(( (s_tany)>> 8) & 0xff);        //B
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( 0* 512 & 0xff) ;//(s_tany & 0xff);        //C
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(( (0 * 512)>> 8) & 0xff);//( (s_tany >> 8) & 0xff);        //C
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)(512 & 0xff);      //D
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)( (512 >> 8) & 0xff); //D
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetX) >> 8 & 0xff); 
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) & 0xff);
				*(sourcebufferaddr + (*u32p_sourceShift)++) = (u8)((-(s16)newTexture.OffsetY) >> 8 & 0xff);

			}

		}
	}

	//д�뱳���ļ�
	newTexture = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + p_wptr->NumOfTex - 2];
	newTexture.writeSourceBuffer(u32p_sourceShift,&matrixTemp,p_wptr,NULL);

	
	//if(p_wptr->CurPosI > p_wptr->WidgetWidth/stepX)
	//{
	//	p_wptr->CurPosI = 0;
	//}

	return AHMI_FUNC_SUCCESS;
}

//****************************************
//ʾ�������ƺ���
//-----------------------
//ʾ���������ǳ����⣡����
//-----------------------
//ʾ�������ƺ��������������������в���������ǵ����ߣ�
//
//ʾ�������ƺ���ʹ���˷������������ṹ��Ŀռ䣬������ʹ�ô˽ṹ��
//�������øÿռ�洢���β���������ꡣ
//ÿ��BasicTextureTag�ṹ��ӵ��32�ֽڿռ䣬��˿��Դ洢16����������Ϣ��
//ʾ�������ƺ����ڻ���ʱ��ר��Draw������������������������Ϣ��������BasicTextureTag�ṩ����Ϣ��
//
//WidgetAttr��ʶ��
//15:old_value_init,�����ж�oldֵ�Ƿ���Ч
//14-12:�߿�
//11���Ƿ���Ϊ��һ�����ò�����������ʱ���޸ģ���ˢ��ʱ��λ��Ĭ��ֵΪ0
//10-5:StepX��ʾ��������������
//4-0:�ؼ����ͣ�ʾ����Ϊ0x6
//****************************************
funcStatus OSCClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
	)
{
	
	u32  actionAddr;
	u32	maxValue;
	u32	minValue;
	u32 lowAlarmValue ;
	u32 highAlarmValue;
	u32 value;
	u32 oldValue=0;
//	RefreshMsg refreshMsg;
	u16* samplePointBuf;
	u16  sampleBufSize;
	u16 oldValueinit=0;
	u8   lineWidth;
	u8   stepX;
	

	if((NULL == p_wptr) || (NULL == ActionPtr))
		return AHMI_FUNC_FAILURE;

	samplePointBuf=(u16*)&(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
	sampleBufSize = ( (p_wptr->NumOfTex - 2) * sizeof(TextureClass)) >> 1;
//	refreshOldPoint = (p_wptr->WidgetAttr & 0x800)?1:0;
	lineWidth = (p_wptr->WidgetAttr >>12) & 0x7;
	stepX = (p_wptr->WidgetAttr>>5) & 0x3f;
	oldValueinit = (p_wptr->WidgetAttr & 0x8000);

	maxValue = (p_wptr->MaxValueH << 16) + p_wptr->MaxValueL;
	minValue = (p_wptr->MinValueH << 16) + p_wptr->MinValueL;
	lowAlarmValue  = (p_wptr->LowAlarmValueH << 16) + p_wptr->LowAlarmValueL;
	highAlarmValue = (p_wptr->HighAlarmValueH << 16) + p_wptr->HighAlarmValueL;

	value = ActionPtr->mTagPtr->mValue;
	value += p_wptr->BLANK_Y;

	if(p_wptr->CurPosI>=sampleBufSize) 
		return AHMI_FUNC_FAILURE;//�������


	samplePointBuf[p_wptr->CurPosI++] = p_wptr->WidgetOffsetY + p_wptr->WidgetHeight - lineWidth
								- ((value - minValue) * (p_wptr->WidgetHeight - lineWidth) /(maxValue - minValue));
	if(p_wptr->CurPosI > (p_wptr->WidgetWidth - p_wptr->BLANK_X)/stepX)
	{
		//Ptr->CurPosX = 0;�Ѹþ仰�ŵ���ͼ������
		p_wptr->WidgetAttr = p_wptr->WidgetAttr | 0x800;
	}

	//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
	refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
	refreshMsg.mElementPtr.wptr = p_wptr;
	sendToRefreshQueue(&refreshMsg);
#endif

	if(oldValueinit)
	{
		oldValue = (p_wptr->OldValueH  << 16) + p_wptr->OldValueL;
		if(p_wptr->EnterLowAlarmAction && oldValue > lowAlarmValue && value <= lowAlarmValue)
		{
			//�����ֵԤ��
			if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}		
		}
		else if(p_wptr->LeaveLowAlarmAction && oldValue <= lowAlarmValue && value > lowAlarmValue)
		{
			//�뿪��ֵԤ��
			if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}		
		}
		if(p_wptr->EnterHighAlarmAction && oldValue < highAlarmValue && value >= highAlarmValue)
		{
			//�����ֵԤ��
			if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
			}
		}
		else if(p_wptr->LeaveHighAlarmAction && oldValue >= highAlarmValue && value < highAlarmValue)
		{
			//�뿪��ֵԤ��
			if(p_wptr->LeaveHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveHighAlarmAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}	
		}
		//��tagchange��֧�� by Mr.z
		if(oldValue != value){
			actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->TagChangeAction);
			xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
		}
	}
	p_wptr->WidgetAttr |= 0x8000;
	p_wptr->OldValueL = (u16) value;
	p_wptr->OldValueH = (u16)(value >> 16); 

	return AHMI_FUNC_SUCCESS;

}

#endif
