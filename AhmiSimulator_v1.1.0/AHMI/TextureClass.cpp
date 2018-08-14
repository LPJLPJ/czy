#include "TextureClass.h"
#include "publicInclude.h"
#include "drawImmediately_cd.h"
#include "myMathClass.h"
#ifdef EMBEDDED	
#include "ahmi3_function.h"
#endif
#ifdef AHMI_CORE

extern u8       sourceBuffer[2048];
extern DynamicPageClassPtr gPagePtr;
extern u16             WorkingPageID;
extern	u8 addr_w;
extern s16 screenratio; //������������һ�µ����

TextureClass::TextureClass(void)
{
}


TextureClass::~TextureClass(void)
{
}


//-----------------------------
// ��������  writesourcebuffer
// дsourcebuffer����
// �����б�
// @param1 u32*   psourceshift            sourcebufferƫ����
// @param2 matrixClassPtr addtionalMatrix ����ľ���
// ��ע(�����汾֮����޸�):
// ...
//-----------------------------
funcStatus TextureClass::writeSourceBuffer(u32 *psourceshift, matrixClassPtr addtionalMatrix, WidgetClassPtr p_wptr,TileBoxClassPtr pTileBox)
{
	matrixClass matrixTemp, matrixTemp2; //���ڼ���source buffer���� by ������
	CanvasClassPtr focusedCanvas;       //���ڼ����������
	matrixTemp.matrixInit();
	matrixTemp2.matrixInit();
	s32 angle;
	s32 para1,para2;
	u32 addr;
	s32 NewMatrix[4] = {0x100000,0,0,0x100000};//1.11.20
	int i;
	u8 *sourcebufferaddr;
	sourcebufferaddr = (u8 *)SoureBufferAddr;
	int TexAttr = this->mTexAttr;
	//u8 SB_MaskType,
	u8 SB_Matrix, SB_AddrType;
	myMathClass myMath;
	PointClass pointTemp(this->OffsetX,this->OffsetY);  //��������������תƫ�����ľ��� NULL == psourceshift ||
	s32 movingX, movingY;    //x,y������Ҫƫ�Ƶ�ƫ����


	if(  NULL == addtionalMatrix || NULL == psourceshift){
		ERROR_PRINT("ERROR: data is NULL pointer");
		return AHMI_FUNC_FAILURE;
	}

	if(p_wptr != NULL)
		focusedCanvas = &gPagePtr[WorkingPageID].pCanvasList[p_wptr->ATTATCH_CANVAS];
#ifdef EMBEDDED	
	TextureClass newTexture;
	if(*psourceshift >= (SRCBUFSIZE - 96))
	{
		*(sourcebufferaddr + (*psourceshift)++) = ENDFLAG;
		AHMIDraw(psourceshift);
		newTexture.TexAddr = START_ADDR_OF_RAM + START_ADDR_OF_DISPLAY + addr_w * SIZE_OF_DISPLAY_BUFFER;
		newTexture.OffsetX = 0 * 16;
		newTexture.OffsetY = 0 * 16;
		newTexture.FocusedSlice = 0;
		newTexture.RotateAngle = 0 * 16;
		newTexture.ScalerX = 1 * 512;
		newTexture.ScalerY = 1 * 512;
		newTexture.ShearAngleX = 0 * 16;
		newTexture.SingleSliceSize = SCREEN_WIDTH * SCREEN_HEIGHT * 2;
		newTexture.mTexAttr = NONMASKTEX | EFMATRIX | ADDRTEXTURE | RGBA8888 | DRAWING;
		newTexture.TexWidth = SCREEN_WIDTH;
		newTexture.TexHeight = SCREEN_HEIGHT;
		newTexture.TexLeftTileBox = 0;
		newTexture.TexRightTileBox = TILE_NUM_X;
		newTexture.TexTopTileBox = 0;
		newTexture.TexButtomTileBox = TILE_NUM_Y;
		newTexture.writeSourceBuffer(psourceshift, addtionalMatrix, p_wptr,NULL);
	}
#endif
	
	
	//TexAttrΪ�������Թ�16bits����ͬ�ı���λ��������Ĳ�ͬ���ԣ���Ӧ��ϵ����
	//Resaved(15:11) SB_MaskType(10:9) SB_AddrType(8)
	//SB_texType(7:4) SB_Mask(3) SB_Matrix(2:1) SB_End(0)
	//SB_MaskType = (TexAttr & MASKTYPE_NOT) >> 9;
	SB_Matrix  = (TexAttr & ABCDEFMATRIX) >> 1;
	SB_AddrType = (TexAttr & ADDRTEXTURE) >> 8;
	//TexMinSizeΪ����ռ�õ���С�ڴ��С,��λΪ�ֽڣ���ͬ����������ռ�õ��ڴ��С��ͬ
	//Matrix�����ֿ��ܣ�0��ʾֻ��E, F��ֻ��ƽ�ƣ�1��ʾֻ��A, B��A = D, B = -C������ʾֻ�з���������ת��2��ʾAB(CD)EF���С�(32/64/96bits)
	//TexSize���ֿ��ܣ�0��ʾ��Ϊ8bit, 1��ʾ��Ϊ16bit��עһ��8bitһ��16bit��չΪ˫16bit)��(16/32bits)
	//AddrType���ֿ��ܣ�0��ʾΪ����Ҫ��ַ��1��ʾΪ��Ҫ�� (0/32bits)
	//int texshift = TexMinSize
	//	+ (((SB_Matrix+1)>>1) << 2)
	//	+ (SB_AddrType << 2);//���Լ�¼ָ���ƫ���ֽ�����

		SB_Matrix = 3;
	//ǿ��ʹ�� ABCDEF �������󣬲��ٽ��о���任�����������ж�
	TexAttr |= ABCDEFMATRIX;
	
	// modified by xt 2015/05/07 
	// write texture attribute
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(TexAttr & 0xff);//texfisrtheader
	*(sourcebufferaddr + (*psourceshift)++) = (u8)((TexAttr >> 8) & 0x7f);//texsecondheader

	// writing boxsize
	if(pTileBox == NULL)
	{
		*(sourcebufferaddr + (*psourceshift)++) = this->TexLeftTileBox;//startx
		*(sourcebufferaddr + (*psourceshift)++) = this->TexTopTileBox; //starty
		*(sourcebufferaddr + (*psourceshift)++) = this->TexRightTileBox; //stopx
		*(sourcebufferaddr + (*psourceshift)++) = this->TexButtomTileBox;//stopy
	}
	else
	{
		*(sourcebufferaddr + (*psourceshift)++) = pTileBox->LeftBox;//startx
		*(sourcebufferaddr + (*psourceshift)++) = pTileBox->TopBox; //starty
		*(sourcebufferaddr + (*psourceshift)++) = pTileBox->RightBox; //stopx
		*(sourcebufferaddr + (*psourceshift)++) = pTileBox->ButtomBox;//stopy
	}

	//writing texsize
	//TexSize���ֿ��ܣ�0��ʾ��Ϊ8bit, 1��ʾ��Ϊ16bit��עһ��8bitһ��16bit��չΪ˫16bit)��(16/32bits)
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(this->TexWidth & 0xff);
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(this->TexWidth >> 8 & 0xff);
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(this->TexHeight & 0xff);
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(this->TexHeight >> 8 & 0xff);

	//writing matrix
	//Matrix�����ֿ��ܣ�0��ʾֻ��E, F��ֻ��ƽ�ƣ�1��ʾ��A,B,E,F��A = D, B = -C������ʾֻ����תλ��.2��ʾ��A,D,E,F����λ�ơ�3��ʾ��ABCDEF��(32/64/96bits)
	//when writing the matrix, we need to judge whether the current matrix is enough to represent the animation
	// compute and write matrix
	{
		//��ʼ�����BasicTexture�ṹ���л�ȡ��BasicTexture�д洢����������
		if(p_wptr != NULL)
		{
			//�����canvas��ƫ������Ҫ��������ϵ��������additionalMatrix��ƽ�����������canvas��ǰλ�õ�ƽ������������Ҫ��ȥfocusedCanvas->offset
			matrixTemp.E =-(this->OffsetX - (long long)(focusedCanvas->moffsetX * 16)) * 0x100000 / addtionalMatrix->A - focusedCanvas->moffsetX * 16;
			matrixTemp.F =-(this->OffsetY - (long long)(focusedCanvas->moffsetY * 16)) * 0x100000 / addtionalMatrix->A - focusedCanvas->moffsetY * 16;
		}
		else 
		{
			matrixTemp.E = -(this->OffsetX );
			matrixTemp.F = -(this->OffsetY );
		}
		matrixTemp.E <<= 9;
		matrixTemp.F <<= 9;

		//��ת�Ƕȴ�BasicTexture�ṹ���л�ȡ��BasicTexture�д洢�������������ת�Ƕ�,������ת�Ƕ�
		myMath.MatrixRotate(this->RotateAngle, NewMatrix);
		//���нǶȴ�BasicTexture�ṹ���л�ȡ��BasicTexture�д洢����������,������нǶ�
		if(this->ShearAngleY == 0 && this->ShearAngleX != 0){
			myMath.MatrixShearX(this->ShearAngleX, NewMatrix);
		}else if(this->ShearAngleY != 0 && this->ShearAngleX == 0){
			myMath.MatrixShearY(this->ShearAngleY, NewMatrix);
		}else{ 
			myMath.MatrixShearX(this->ShearAngleX, NewMatrix);
		}
		//��Ҫ������ת��Ҫ�����ƫ����
		//��ת�Ƕȴ�BasicTexture�ṹ���л�ȡ��BasicTexture�д洢�������������ת�Ƕ�
		angle = this->RotateAngle;//1.11.4
		matrixTemp2.E = 0;
		matrixTemp2.F = 0;
		if(this->mTexAttr & TEXTURE_CENTRAL_ROTATE) //������ת
		{
			// compute rotate matrix
			para1 = 0x100000;
			para2 = 0;
			myMath.CORDIC_32(angle, &para1, &para2);//�ȼ�����ת��������									 k
			matrixTemp2.A = para1;
			matrixTemp2.C = -para2;
			matrixTemp2.B = para2;
			matrixTemp2.D = para1; //��Ϊ���õ���ת�þ�������������ֵ
			
			//������������޸������޸��������Ļ��һ�µ������
			matrixClass matrixTempForScale;
			matrixTempForScale.matrixInit();
			//���ڸ��Ĳ�������Ļ��ʾ��߱�����������ʹ�õ�ʱ��Ҫ����ȡ����ʵ������
			matrixTempForScale.A = (512);
			matrixTempForScale.D = (screenratio) ; 

			PointClass pointTemp2(0,0), pointTemp3(0,0);
			//������ת���µ�����ƫ����
 			pointTemp2.mPointX = ( (s32)(this->TexWidth) * 8);//* 16 / 2;
			pointTemp2.mPointY = ( (s32)(this->TexHeight) * 8);// * 16 / 2;
			pointTemp2.leftMulMatrix(&matrixTemp2);
			//�������������ʹ�÷������������µ�����ƫ����
			pointTemp3.mPointX = (pointTemp2.mPointX);//* 16 / 2;
			pointTemp3.mPointY = (pointTemp2.mPointY);// * 16 / 2;
			pointTemp3.leftMulMatrix(&matrixTempForScale);

			movingX = pointTemp2.mPointX - ( (s32)(this->TexWidth) *8) + pointTemp3.mPointX - pointTemp2.mPointX;
			movingY = pointTemp2.mPointY - ( (s32)(this->TexHeight)*8) + pointTemp3.mPointY - pointTemp2.mPointY;

			matrixTemp2.E += movingX * 512 / addtionalMatrix->A ;
			matrixTemp2.F += movingY * 512 / addtionalMatrix->A ;

			pointTemp.mPointX = ( (s32)(this->TexWidth) * 8); // * 16 / 2;
			pointTemp.mPointY = ( (s32)(this->TexHeight) * 8); // * 16 / 2;
			pointTemp.leftMulMatrix(&matrixTemp2);
			movingX = pointTemp.mPointX - ( (s32)(this->TexWidth) * 8);
			movingY = pointTemp.mPointY - ( (s32)(this->TexHeight) * 8);
			
			matrixTemp2.E += ((movingX * 0x100000 / addtionalMatrix->A) << 9) ;
			matrixTemp2.F += ((movingY * 0x100000 / addtionalMatrix->A) << 9) ;
		}

		matrixTemp.E += matrixTemp2.E;
		matrixTemp.F += matrixTemp2.F;
		//�Ŵ�����BasicTexture�ṹ���л�ȡ��BasicTexture�д洢����������,����Ŵ���
		//myMath.MatrixScaler(this->ScalerX,this->ScalerY,NewMatrix);

		//32�����ж�Խλ
		for(i=0;i<4;i++)
		{
			if((long long)NewMatrix[i]>(long long)0x7fffffff)  NewMatrix[i] = 0x7fffffff;
			if((long long)NewMatrix[i]<(long long)-0x7fffffff) NewMatrix[i] = -0x7fffffff;
		}
		matrixTemp.A = NewMatrix[0];
		matrixTemp.B = NewMatrix[1];
		matrixTemp.C = NewMatrix[2];
		matrixTemp.D = NewMatrix[3];

		matrixTemp.matrixMulti(addtionalMatrix);
		//�������ľ����޸�����Կ�߲�һ�µ������
		matrixClass matrixTempForScale;
		matrixTempForScale.matrixInit();
		matrixTempForScale.A = (0x40000 / this->ScalerX) << 11;
		matrixTempForScale.D = (0x40000 / this->ScalerY) << 11; 
		matrixTemp.matrixMulti(&matrixTempForScale);

		//�������ľ����޸��������Ļ��߲�һ�µ������
 		//���ڸ��Ĳ�������Ļ��ʾ��߱�����������ʹ�õ�ʱ��Ҫ����ȡ����ʵ������
		if(TexAttr & USING_PIXEL_RATIO) //needs to fix the ellipse pixel, by zuz 20180802
		{
		    matrixClass matrixTempForScale;
		    matrixTempForScale.matrixInit(); 
		    matrixTempForScale.A = (0x40000 / 512);
		    matrixTempForScale.D = (0x40000 / screenratio) ; 
		    matrixTemp.matrixMulti(&matrixTempForScale);
		}

        *(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.A & 0xff);
        *(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.A >> 8 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.A >> 16) & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.A >> 24) & 0xff); 
        *(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.B & 0xff);
        *(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.B >> 8 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.B >> 16) & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.B >> 24) & 0xff); 
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.C & 0xff);
        *(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.C >> 8 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.C >> 16) & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.C >> 24) & 0xff); 
        *(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.D & 0xff);
        *(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.D >> 8 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.D >> 16) & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.D >> 24) & 0xff); 
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.E & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.E >> 8 & 0xff); 
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.E >> 16) & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.E >> 24) & 0xff); 
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.F & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(matrixTemp.F >> 8 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.F >> 16) & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)( (matrixTemp.F >> 24) & 0xff); 
	}


	//writing addr or color
	//AddrType���ֿ��ܣ�0��ʾΪ����Ҫaddr��1��ʾΪ��Ҫaddr�� (0/32bits)
	if (SB_AddrType==1)
	{
#if 0	//STM32Ӳ��BUG����һ�������ͬʱʹ��"+"��"*"����������
		addr = (u32)(this->TexAddr + this->SingleSliceSize*(this->FocusedSlice));
//		addr += (u32)(this->SingleSliceSize*(this->FocusedSlice));
#else
		addr = (u32)(this->SingleSliceSize*(this->FocusedSlice));
		addr += (u32)(this->TexAddr);
#endif
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(addr & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(addr >> 8 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(addr >> 16 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(addr >> 24 & 0xff);
	}

	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// ��������  renewTextureSourceBox
// ���ݵ����������λ���Ż������Χ��
// �����б�
//      void
// ��ע(�����汾֮����޸�):
//    added by zhouyuzhi 20160614
//-----------------------------
funcStatus TextureClass::renewTextureSourceBox(
		TileBoxClassPtr pTileBox,
		matrixClassPtr additionalMatrix,
		WidgetClassPtr p_wptr
	)
{
	
	PointClass leftTopPoint(0, 0); //���ϽǶ���
	PointClass leftBottomPoint(0, 0 + (this->TexHeight * 16) * this->ScalerY / 512); //���½�
	PointClass rightTopPoint(0 + (this->TexWidth * 16) * this->ScalerX / 512, 0); //���Ͻ�
	PointClass rightButtomPoint(0 + (this->TexWidth * 16) * this->ScalerX / 512, 0 + (this->TexHeight * 16) * this->ScalerY / 512 ); //���½�
	PointClass offsetAfterScaler(0,0);
	s32 minX=0,minY=0,maxX=0,maxY=0; //1.27.4

	if(additionalMatrix != NULL)
	{
		leftBottomPoint.mPointY = (long long)leftBottomPoint.mPointY * 0x100000 / additionalMatrix->A;
		rightTopPoint.mPointX =   (long long)rightTopPoint.mPointX * 0x100000 / additionalMatrix->A;
		rightButtomPoint.mPointX = (long long)rightButtomPoint.mPointX * 0x100000 / additionalMatrix->A;
		rightButtomPoint.mPointY = (long long)rightButtomPoint.mPointY * 0x100000 / additionalMatrix->A;
	}

	//�ĸ������ֱ�����ת
	leftTopPoint.pointRotating(this->RotateAngle);
	leftBottomPoint.pointRotating(this->RotateAngle);
	rightTopPoint.pointRotating(this->RotateAngle);
	rightButtomPoint.pointRotating(this->RotateAngle);

	//�������հ�Χ��
	minX = leftTopPoint.mPointX;
	minY = leftTopPoint.mPointY;
	maxX = leftTopPoint.mPointX;
	maxY = leftTopPoint.mPointY;
	if(leftBottomPoint.mPointX < minX)
		minX = leftBottomPoint.mPointX;
	if(leftBottomPoint.mPointX > maxX)
		maxX = leftBottomPoint.mPointX;
	if(leftBottomPoint.mPointY  < minY)
		minY = leftBottomPoint.mPointY;
	if(leftBottomPoint.mPointY  > maxY)
		maxY = leftBottomPoint.mPointY;

	if(rightTopPoint.mPointX < minX)
		minX = rightTopPoint.mPointX;
	if(rightTopPoint.mPointX > maxX)
		maxX = rightTopPoint.mPointX;
	if(rightTopPoint.mPointY  < minY)
		minY = rightTopPoint.mPointY;
	if(rightTopPoint.mPointY  > maxY)
		maxY = rightTopPoint.mPointY;

	if(rightButtomPoint.mPointX < minX)
		minX = rightButtomPoint.mPointX;
	if(rightButtomPoint.mPointX > maxX)
		maxX = rightButtomPoint.mPointX;
	if(rightButtomPoint.mPointY  < minY)
		minY = rightButtomPoint.mPointY;
	if(rightButtomPoint.mPointY  > maxY)
		maxY = rightButtomPoint.mPointY;

	this->adjustSclaring(additionalMatrix, &offsetAfterScaler,p_wptr);

	//���¼����Χ�� 
	if(pTileBox == NULL)//ֱ���޸������Χ��
	{
		this->TexLeftTileBox = ( (offsetAfterScaler.mPointX + minX ) >> 4 ) / TILESIZE;
		this->TexRightTileBox = ( ( (offsetAfterScaler.mPointX + maxX) >> 4)) / TILESIZE;
		this->TexTopTileBox = ( (offsetAfterScaler.mPointY + minY) >> 4 ) / TILESIZE;
		this->TexButtomTileBox = ( ( (offsetAfterScaler.mPointY + maxY) >> 4)) / TILESIZE;
	
		if(offsetAfterScaler.mPointX + minX < 0)
			this->TexLeftTileBox = 0;
		if(offsetAfterScaler.mPointX + maxX < 0)
			this->TexRightTileBox = 0;
		if(offsetAfterScaler.mPointY + minY < 0)
			this->TexTopTileBox = 0;
		if(offsetAfterScaler.mPointY + maxY < 0)
			this->TexButtomTileBox = 0;

		if((s8) this->TexLeftTileBox < 0)
			this->TexLeftTileBox = 0;
		if((s8) this->TexTopTileBox < 0)
			 this->TexTopTileBox = 0;
	}
	else 
	{
		pTileBox->LeftBox = ( (offsetAfterScaler.mPointX + minX ) >> 4 ) / TILESIZE;
		pTileBox->RightBox = ( ( (offsetAfterScaler.mPointX + maxX) >> 4)) / TILESIZE;
		pTileBox->TopBox = ( (offsetAfterScaler.mPointY + minY) >> 4 ) / TILESIZE;
		pTileBox->ButtomBox = ( ( (offsetAfterScaler.mPointY + maxY) >> 4)) / TILESIZE;
	
		if(offsetAfterScaler.mPointX + minX < 0)
			pTileBox->LeftBox = 0;
		if(offsetAfterScaler.mPointX + maxX < 0)
			pTileBox->RightBox = 0;
		if(offsetAfterScaler.mPointY + minY < 0)
			pTileBox->TopBox = 0;
		if(offsetAfterScaler.mPointY + maxY < 0)
			pTileBox->ButtomBox = 0;

		if((s8) pTileBox->LeftBox < 0)
			pTileBox->LeftBox = 0;
		if((s8) pTileBox->TopBox < 0)
			 pTileBox->TopBox = 0;
	}

	return AHMI_FUNC_SUCCESS;
}

//------------------------------------
// ������:RenewRotateTextureSourceBox
// ���¼�����תͼƬ�İ�Χ��
// �����б�
//		void
// ��ע(�����汾֮����޸�):
//    added by zhangcheng 20170510
//------------------------------------
funcStatus TextureClass::RenewRotateTextureSourceBox()
{
	PointClass leftTopPoint(0, 0); //���ϽǶ���
	PointClass leftBottomPoint(0, 0 + (this->TexHeight * 16) ); //���½�
	PointClass rightTopPoint(0 + (this->TexWidth * 16), 0); //���Ͻ�
	PointClass rightBottomPoint(0 + (this->TexWidth * 16), 0 + (this->TexHeight * 16) ); //���½�
	PointClass CentralPoint(0,0);   //���ĵ�

	s32 RelativeRound=0; //1.27.4

	CentralPoint.mPointX=(rightTopPoint.mPointX - leftTopPoint.mPointX)/2;
	CentralPoint.mPointY=(rightBottomPoint.mPointY - rightTopPoint.mPointY)/2;
	
	// 1.414 * 512 = 724 << 9
	if(CentralPoint.mPointX > CentralPoint.mPointY )
		RelativeRound = 724 * CentralPoint.mPointX >> 9;
	else
		RelativeRound = 724 * CentralPoint.mPointY >> 9;

	this->TexLeftTileBox = ( (this->OffsetX - (RelativeRound - CentralPoint.mPointX) ) >> 4 ) / TILESIZE;
	this->TexRightTileBox = ( ( (this->OffsetX + RelativeRound + CentralPoint.mPointX) >> 4) ) / TILESIZE;
	this->TexTopTileBox = ( (this->OffsetY - (RelativeRound - CentralPoint.mPointY) ) >> 4 ) / TILESIZE;
	this->TexButtomTileBox = ( ( (this->OffsetY + RelativeRound + CentralPoint.mPointY) >> 4)) / TILESIZE;

	if(this->TexLeftTileBox < 0)
		this->TexLeftTileBox = 0;
	if(this->TexRightTileBox < 0)
		this->TexRightTileBox = 0;
	if(this->TexTopTileBox < 0)
		this->TexTopTileBox = 0;
	if(this->TexButtomTileBox < 0)
		this->TexButtomTileBox = 0;

	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// ��������  copyBox
// ���¸�ֵ��Χ��
// �����б�
//      u16 centralLength �����߳���
// ��ע(�����汾֮����޸�):
//    added by zhouyuzhi 20160614
//-----------------------------
funcStatus TextureClass::copyBox(
	TileBoxClassPtr pTileBox
	)
{
	this->TexButtomTileBox = pTileBox->ButtomBox;
	this->TexTopTileBox = pTileBox->TopBox;
	this->TexLeftTileBox = pTileBox->LeftBox;
	this->TexRightTileBox = pTileBox->RightBox;
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// ��������  adjustMoving
// ����ƽ�Ʒ���
// �����б�
//      u16 centralLength �����߳���
// ��ע(�����汾֮����޸�):
//    added by zhouyuzhi 20160614
//-----------------------------
funcStatus TextureClass::adjustMoving(
	u16 centralLength, //����
	u16 centralPosX,   //12.4
	u16 centralPosY    //12.4
	)
{
	s16 para1,para2;
	myMathClass myMath;
	para1 = 512; //cos
	para2 = 0;   //sin
	myMath.CORDIC(this->RotateAngle,&para1,&para2);//�ȼ�����ת��������
	//this->x = centralx - (height + centralLength) * sin
	//this->y = centraly + (height + centralLength) * cos
	this->OffsetX = centralPosX - (( centralLength * para2) >> 5) ;
	this->OffsetY = centralPosY + (( centralLength * para1) >> 5) ;
	return AHMI_FUNC_SUCCESS;

}


//-----------------------------
// ��������  adjustSclaring
// ���ݶ�������������ŵ�ƫ����
// �����б�
//      u16 centralLength �����߳���
// ��ע(�����汾֮����޸�):
//    added by zhouyuzhi 20160614
//-----------------------------
funcStatus TextureClass::adjustSclaring(
	matrixClassPtr addtionalMatrix,
	PointClass*  pointAfterScaler,
	WidgetClassPtr p_wptr
	)
{
	CanvasClassPtr focusedCanvas;
	if(p_wptr != NULL && addtionalMatrix != NULL)
	{
		focusedCanvas = &gPagePtr[WorkingPageID].pCanvasList[p_wptr->ATTATCH_CANVAS];
		pointAfterScaler->mPointX =(this->OffsetX - (long long)(focusedCanvas->moffsetX * 16)) * 0x100000 / addtionalMatrix->A + focusedCanvas->moffsetX - (addtionalMatrix->E >> 9);
		pointAfterScaler->mPointY =(this->OffsetY - (long long)(focusedCanvas->moffsetY * 16)) * 0x100000 / addtionalMatrix->A + focusedCanvas->moffsetY - (addtionalMatrix->F >> 9);
	}
	else 
	{
		pointAfterScaler->mPointX = this->OffsetX;
		pointAfterScaler->mPointY = this->OffsetY;
	}
	return AHMI_FUNC_SUCCESS;

}

#endif
