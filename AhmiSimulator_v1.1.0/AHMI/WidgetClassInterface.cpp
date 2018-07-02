#include "publicInclude.h"
#ifdef AHMI_DEBUG
#include "stdio.h"
#include "trace.h"
#endif
#include "drawImmediately_cd.h"

#ifdef AHMI_CORE
extern DynamicPageClassPtr gPagePtr;
extern u16             WorkingPageID;
extern u8 animationExist;

//#define VISUAL_SOURCE_BOX_EN

WidgetClassInterface::WidgetClassInterface(void)
{
}


WidgetClassInterface::~WidgetClassInterface(void)
{
}


funcStatus WidgetClassInterface::drawTexture(
		WidgetClassPtr p_wptr,   //控件指针
		u32 *u32p_sourceShift,   //sourcebuffer指针
		u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
		TileBoxClassPtr pTileBox, //包围盒
		u8 staticTextureEn          //是否绘制到静态存储空间
		)
{
	if(NULL == p_wptr || NULL == u32p_sourceShift || NULL == pTileBox){
		ERROR_PRINT("ERROR: data is NULL Pointer");
		return AHMI_FUNC_FAILURE;
	}
	TextureClassPtr  DrawingTex = &gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex];
	TileBoxClass     textureTileBox;
	//matrixClass matrixTemp;
	int TextureCount;

	//matrixTemp.matrixInit();

	for (TextureCount = 0; TextureCount < p_wptr->NumOfTex; TextureCount++)
	{
		DrawingTex = &gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + TextureCount];
		DrawingTex->renewTextureSourceBox(&textureTileBox, &(p_wptr->mWidgetMatrix), p_wptr);
		if( (DrawingTex->mTexAttr & TEXTURE_USING_WIDGET_BOX) != 0)
			textureTileBox.sourceBoxCopy(pTileBox);
	//make it visible
	#ifdef VISUAL_SOURCE_BOX_EN
		TextureClass sourceBoxTexture;
		matrixClass sourceBoxMatrix;
		sourceBoxMatrix.matrixInit();
		sourceBoxTexture.TexAddr = 0xffff0000;
		sourceBoxTexture.OffsetX = textureTileBox.LeftBox * 32 * 16;
		sourceBoxTexture.OffsetY = textureTileBox.TopBox * 32 * 16;
		sourceBoxTexture.FocusedSlice = 0;
		sourceBoxTexture.RotateAngle = 0 * 16;
		sourceBoxTexture.ScalerX = 1 * 512;
		sourceBoxTexture.ScalerY = 1 * 512;
		sourceBoxTexture.ShearAngleX = 0 * 16;
		sourceBoxTexture.SingleSliceSize = SCREEN_WIDTH * SCREEN_HEIGHT * 2;
		sourceBoxTexture.mTexAttr = NONMASKTEX | EFMATRIX | ADDRTEXTURE | PURECOLOR | DRAWING;
		sourceBoxTexture.TexWidth = (textureTileBox.RightBox - textureTileBox.LeftBox + 1) * 32;
		sourceBoxTexture.TexHeight = (textureTileBox.RightBox - textureTileBox.LeftBox + 1) * 32;
		sourceBoxTexture.TexLeftTileBox = textureTileBox.LeftBox;
		sourceBoxTexture.TexRightTileBox = textureTileBox.RightBox;
		sourceBoxTexture.TexTopTileBox = textureTileBox.TopBox;
		sourceBoxTexture.TexButtomTileBox = textureTileBox.ButtomBox;
		sourceBoxTexture.writeSourceBuffer(u32p_sourceShift, &sourceBoxMatrix, NULL,NULL);
	#endif
		if (DrawingTex == NULL)
		{
#ifdef AHMI_DEBUG
			ERROR_PRINT("In darwimmediately(),currenttexture + TextureCount is NULL.\r\n");
#endif
			return AHMI_FUNC_FAILURE;
		}

		if((DrawingTex->mTexAttr & DRAWING) == 0)
		{
			continue; //TexAttr[15]为0，该纹理不用绘制
		}
		//调整包围盒
		//DrawingTex->copyBox(pTileBox);
		
		if(DrawingTex->TexLeftTileBox < 0)
			DrawingTex->TexLeftTileBox = 0;
		if(DrawingTex->TexTopTileBox < 0)
			DrawingTex->TexTopTileBox = 0;
		//如果是强制绘制动画，则所有控件的包围盒都是整个屏幕
		//if(DrawingTex->staticTexture == staticTextureEn || animationExist != 0)//更新静态缓存
			DrawingTex->writeSourceBuffer(u32p_sourceShift, &(p_wptr->mWidgetMatrix), p_wptr, &textureTileBox);
	}
	return AHMI_FUNC_SUCCESS;
}

#endif
