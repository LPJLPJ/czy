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


TextureClass::TextureClass(void)
{
}


TextureClass::~TextureClass(void)
{
}


//-----------------------------
// 函数名：  writesourcebuffer
// 写sourcebuffer函数
// 参数列表：
// @param1 u32*   psourceshift            sourcebuffer偏移量
// @param2 matrixClassPtr addtionalMatrix 额外的矩阵
// 备注(各个版本之间的修改):
// ...
//-----------------------------
funcStatus TextureClass::writeSourceBuffer(u32 *psourceshift, matrixClassPtr addtionalMatrix, WidgetClassPtr p_wptr,TileBoxClassPtr pTileBox)
{
	matrixClass matrixTemp, matrixTemp2; //用于计算source buffer矩阵 by 周钰致
	CanvasClassPtr focusedCanvas;       //用于计算相对坐标
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
	PointClass pointTemp(this->OffsetX,this->OffsetY);  //用来计算中心旋转偏移量的矩阵 NULL == psourceshift ||
	s32 movingX, movingY;    //x,y方向需要偏移的偏移量

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
	
	
	//TexAttr为纹理属性共16bits，不同的比特位包含纹理的不同属性，对应关系如下
	//Resaved(15:11) SB_MaskType(10:9) SB_AddrType(8)
	//SB_texType(7:4) SB_Mask(3) SB_Matrix(2:1) SB_End(0)
	//SB_MaskType = (TexAttr & MASKTYPE_NOT) >> 9;
	SB_Matrix  = (TexAttr & ABCDEFMATRIX) >> 1;
	SB_AddrType = (TexAttr & ADDRTEXTURE) >> 8;
	//TexMinSize为纹理占用的最小内存大小,单位为字节，不同的纹理属性占用的内存大小不同
	//Matrix有四种可能，0表示只有E, F，只有平移；1表示只有A, B（A = D, B = -C），表示只有放缩或者旋转。2表示AB(CD)EF都有。(32/64/96bits)
	//TexSize两种可能，0表示都为8bit, 1表示都为16bit（注一个8bit一个16bit扩展为双16bit)。(16/32bits)
	//AddrType两种可能，0表示为不需要地址，1表示为需要。 (0/32bits)
	//int texshift = TexMinSize
	//	+ (((SB_Matrix+1)>>1) << 2)
	//	+ (SB_AddrType << 2);//用以记录指针的偏移字节总数

	//SB_Matrix = 3;
	//强制使用 ABCDEF 定长矩阵，不再进行矩阵变换包含的类型判断
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
	//TexSize两种可能，0表示都为8bit, 1表示都为16bit（注一个8bit一个16bit扩展为双16bit)。(16/32bits)
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(this->TexWidth & 0xff);
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(this->TexWidth >> 8 & 0xff);
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(this->TexHeight & 0xff);
	*(sourcebufferaddr + (*psourceshift)++) = (u8)(this->TexHeight >> 8 & 0xff);

	//writing matrix
	//Matrix有三种可能，0表示只有E, F，只有平移；1表示有A,B,E,F（A = D, B = -C），表示只有旋转位移.2表示有A,D,E,F放缩位移。3表示有ABCDEF。(32/64/96bits)
	//when writing the matrix, we need to judge whether the current matrix is enough to represent the animation
	// compute and write matrix
	{
		//初始矩阵从BasicTexture结构体中获取，BasicTexture中存储的是正矩阵
		if(p_wptr != NULL)
		{
			//相对于canvas的偏移量需要乘以缩放系数，由于additionalMatrix中平移量是相对于canvas当前位置的平移量，所以需要减去focusedCanvas->offset
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

		//旋转角度从BasicTexture结构体中获取，BasicTexture中存储的是正矩阵的旋转角度,计算旋转角度
		myMath.MatrixRotate(this->RotateAngle, NewMatrix);
		//错切角度从BasicTexture结构体中获取，BasicTexture中存储的是正矩阵,计算错切角度
		if(this->ShearAngleY == 0 && this->ShearAngleX != 0){
			myMath.MatrixShearX(this->ShearAngleX, NewMatrix);
		}else if(this->ShearAngleY != 0 && this->ShearAngleX == 0){
			myMath.MatrixShearY(this->ShearAngleY, NewMatrix);
		}else{ 
			myMath.MatrixShearX(this->ShearAngleX, NewMatrix);
		}
		//需要中心旋转需要额外的偏移量
		//旋转角度从BasicTexture结构体中获取，BasicTexture中存储的是正矩阵的旋转角度
		angle = this->RotateAngle;//1.11.4
		matrixTemp2.E = 0;
		matrixTemp2.F = 0;
		if(this->mTexAttr & TEXTURE_CENTRAL_ROTATE) //中心旋转
		{
			// compute rotate matrix
			para1 = 0x100000;
			para2 = 0;
			myMath.CORDIC_32(angle, &para1, &para2);//先计算旋转的正矩阵									 k
			matrixTemp2.A = para1;
			matrixTemp2.C = -para2;
			matrixTemp2.B = para2;
			matrixTemp2.D = para1; //因为我用的是转置矩阵，所以这样赋值
			
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

		//放大倍数从BasicTexture结构体中获取，BasicTexture中存储的是正矩阵,计算放大倍数
		//myMath.MatrixScaler(this->ScalerX,this->ScalerY,NewMatrix);

		//32矩阵判断越位
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
		//放缩量的矩阵修复（针对宽高不一致的情况）
		matrixClass matrixTempForScale;
		matrixTempForScale.matrixInit();
		matrixTempForScale.A = (0x40000 / this->ScalerX) << 11;
		matrixTempForScale.D = (0x40000 / this->ScalerY) << 11; 
		matrixTemp.matrixMulti(&matrixTempForScale);

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
	//AddrType两种可能，0表示为不需要addr，1表示为需要addr。 (0/32bits)
	if (SB_AddrType==1)
	{
		//addr = (u32)(this->TexAddr + this->SingleSliceSize*(this->FocusedSlice));
		addr = (u32)(this->TexAddr);
		addr += (u32)(this->SingleSliceSize*(this->FocusedSlice));
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(addr & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(addr >> 8 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(addr >> 16 & 0xff);
		*(sourcebufferaddr + (*psourceshift)++) = (u8)(addr >> 24 & 0xff);
	}

	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名：  renewTextureSourceBox
// 根据当期那纹理的位置优化纹理包围盒
// 参数列表：
//      void
// 备注(各个版本之间的修改):
//    added by zhouyuzhi 20160614
//-----------------------------
funcStatus TextureClass::renewTextureSourceBox(
		TileBoxClassPtr pTileBox,
		matrixClassPtr additionalMatrix,
		WidgetClassPtr p_wptr
	)
{
	
	PointClass leftTopPoint(0, 0); //左上角定点
	PointClass leftBottomPoint(0, 0 + (this->TexHeight * 16) * this->ScalerY / 512); //左下角
	PointClass rightTopPoint(0 + (this->TexWidth * 16) * this->ScalerX / 512, 0); //右上角
	PointClass rightButtomPoint(0 + (this->TexWidth * 16) * this->ScalerX / 512, 0 + (this->TexHeight * 16) * this->ScalerY / 512 ); //右下角
	PointClass offsetAfterScaler(0,0);
	s32 minX=0,minY=0,maxX=0,maxY=0; //1.27.4

	if(additionalMatrix != NULL)
	{
		leftBottomPoint.mPointY = (long long)leftBottomPoint.mPointY * 0x100000 / additionalMatrix->A;
		rightTopPoint.mPointX =   (long long)rightTopPoint.mPointX * 0x100000 / additionalMatrix->A;
		rightButtomPoint.mPointX = (long long)rightButtomPoint.mPointX * 0x100000 / additionalMatrix->A;
		rightButtomPoint.mPointY = (long long)rightButtomPoint.mPointY * 0x100000 / additionalMatrix->A;
	}

	//四个坐标点分别做旋转
	leftTopPoint.pointRotating(this->RotateAngle);
	leftBottomPoint.pointRotating(this->RotateAngle);
	rightTopPoint.pointRotating(this->RotateAngle);
	rightButtomPoint.pointRotating(this->RotateAngle);

	//计算最终包围盒
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

	//重新计算包围盒 
	if(pTileBox == NULL)//直接修改纹理包围盒
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
// 函数名:RenewRotateTextureSourceBox
// 重新计算旋转图片的包围盒
// 参数列表：
//		void
// 备注(各个版本之间的修改):
//    added by zhangcheng 20170510
//------------------------------------
funcStatus TextureClass::RenewRotateTextureSourceBox()
{
	PointClass leftTopPoint(0, 0); //左上角定点
	PointClass leftBottomPoint(0, 0 + (this->TexHeight * 16) ); //左下角
	PointClass rightTopPoint(0 + (this->TexWidth * 16), 0); //右上角
	PointClass rightBottomPoint(0 + (this->TexWidth * 16), 0 + (this->TexHeight * 16) ); //右下角
	PointClass CentralPoint(0,0);   //中心点

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
// 函数名：  copyBox
// 重新赋值包围盒
// 参数列表：
//      u16 centralLength 中心线长度
// 备注(各个版本之间的修改):
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
// 函数名：  adjustMoving
// 调整平移分量
// 参数列表：
//      u16 centralLength 中心线长度
// 备注(各个版本之间的修改):
//    added by zhouyuzhi 20160614
//-----------------------------
funcStatus TextureClass::adjustMoving(
	u16 centralLength, //整数
	u16 centralPosX,   //12.4
	u16 centralPosY    //12.4
	)
{
	s16 para1,para2;
	myMathClass myMath;
	para1 = 512; //cos
	para2 = 0;   //sin
	myMath.CORDIC(this->RotateAngle,&para1,&para2);//先计算旋转的正矩阵
	//this->x = centralx - (height + centralLength) * sin
	//this->y = centraly + (height + centralLength) * cos
	this->OffsetX = centralPosX - (( centralLength * para2) >> 5) ;
	this->OffsetY = centralPosY + (( centralLength * para1) >> 5) ;
	return AHMI_FUNC_SUCCESS;

}


//-----------------------------
// 函数名：  adjustSclaring
// 根据动画矩阵调整缩放的偏移量
// 参数列表：
//      u16 centralLength 中心线长度
// 备注(各个版本之间的修改):
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
