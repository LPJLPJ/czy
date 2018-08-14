////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     myMathClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    the math function
// 
////////////////////////////////////////////////////////////////////////////////

#include "myMathClass.h"
#include "publicInclude.h"

#ifdef AHMI_CORE

//*****************************
// CORDIC 算法求解绝对值及角度
//  CORDIC (s16 *s32p_COS,   // 初始方位及旋转后的方位的X分量，归一化后相当于精度1/2048
//          s16 *s32p_SIN);  // 初始方位及旋转后的方位的Y分量，归一化后相当于精度1/2048
//*****************************
void myMathClass::FindAngle(s32* s32p_COS, s32 *s32p_SIN)
{
	s16 s16_para[12] = { 1448, 1295, 1257, 1247, 1244, 1244, 1244, 1244, 1244, 1244, 1244,1244};    //cos@的结果乘以2048
	s32 s32_x, s32_y;
	s32 s32_count_i;
	if(*s32p_SIN==0)
	{
		*s32p_COS = (*s32p_COS>=0)?*s32p_COS:-*s32p_COS;
		return;
	}
	if(*s32p_COS == 0)
	{
		*s32p_COS = (*s32p_SIN>=0)?*s32p_SIN:-*s32p_SIN;
		return;
	}

	if(*s32p_COS<0 && *s32p_SIN>=0)
	{
		*s32p_COS = -*s32p_COS;
	}
	else if(*s32p_COS<0 && *s32p_SIN<0)
	{
		*s32p_COS = -*s32p_COS;
		*s32p_SIN = -*s32p_SIN;
	}
	else if(*s32p_COS>=0 && *s32p_SIN<0)
	{
		*s32p_SIN = -*s32p_SIN;
	}
	s32_x=*s32p_COS;
	s32_y=*s32p_SIN;
	for (s32_count_i = 0; s32_count_i < 12; s32_count_i++)
	{
		if (*s32p_SIN>0)      //大于0表示，在目标角度下面，要继续逆时针旋转，小于0表示目标角度上面，要回转
		{
			*s32p_COS = (s32_x + (s32_y >> s32_count_i));
			*s32p_SIN = (s32_y - (s32_x >> s32_count_i));
			s32_x = *s32p_COS;
			s32_y = *s32p_SIN;
		}
		else if (*s32p_SIN < 0)
		{
			*s32p_COS = (s32_x - (s32_y >> s32_count_i));
			*s32p_SIN = (s32_y + (s32_x >> s32_count_i));
			s32_x = *s32p_COS;
			s32_y = *s32p_SIN;
		}
		if ((s32_y >-2 && s32_y < 2 )|| s32_count_i ==11)
		{
			*s32p_COS = (s32_x*s16_para[s32_count_i])>>11;
			*s32p_SIN = (s32_y*s16_para[s32_count_i])>>11;
			break;
		}
	}
}

//*****************************
// CORDIC 算法求解三角函数
//  CORDIC (s32 s32_Radian, // 旋转角度，左手系，顺时针为正方向，精度1.27.4
//          s16 *s32p_COS,   // 初始方位及旋转后的方位的X分量，归一化后相当于精度1/2048
//          s16 *s32p_SIN);  // 初始方位及旋转后的方位的Y分量，归一化后相当于精度1/2048
//*****************************
void myMathClass::CORDIC(s32 s32_Radian, s16* s32p_COS, s16 *s32p_SIN)
{
	
	s16 s16_Angle[12] = { 5760, 3400 ,1797 ,912 , 458, 229, 115, 57, 29, 14, 7,4 };             //cordic角度精度为9.7
	s16 s16_para[12] = { 1448, 1295, 1257, 1247, 1244, 1244, 1244, 1244, 1244, 1244, 1244,1244};    //cos@的结果乘以2048
	s16 s32_x, s32_y;
	s32 s32_count_i;
	s32 s32_coeffX = 0, s32_coeffY = 0;
	//float f_realAngle = 0;
	s32_x = *s32p_COS;
	s32_y = *s32p_SIN;
	while (s32_Radian >= 360 * 16)
		s32_Radian = s32_Radian - 360 * 16;
	while (s32_Radian < 0)
		s32_Radian = s32_Radian + 360 * 16;
	if (s32_Radian == 0)
	{
		*s32p_COS = s32_x;
		*s32p_SIN = s32_y;
	}
	else if(s32_Radian == 180 * 16)
	{
		*s32p_COS = -s32_x;
		*s32p_SIN = -s32_y;
	}
	else if (s32_Radian == 90 * 16)
	{
		*s32p_COS = -s32_y;
		*s32p_SIN = s32_x;
	}
	else if(s32_Radian == 270 * 16)
	{
		*s32p_COS = s32_y;
		*s32p_SIN = -s32_x;
	}
	else
	{
		if (s32_Radian > 0 && s32_Radian < 90 * 16)
		{
			s32_coeffX = 1, s32_coeffY = 1;
		}
		else if (s32_Radian > 90 * 16 && s32_Radian < 180 * 16) //旋转角度至第二象限，可以把目标角度沿s32_y轴镜像，使得旋转发生在第一象限内。完成旋转后，再镜像s32_x的值
		{
			s32_Radian =180 * 16 - s32_Radian ; // 角度沿s32_y轴镜像
			s32_coeffX = -1, s32_coeffY = 1; //镜像X值
		}
		else if (s32_Radian > 180 * 16 && s32_Radian < 270 * 16) //旋转角度至第三象限，可以把目标角度沿圆点镜像，使得旋转发生在第一象限内。完成旋转后，再镜像s32_x,s32_y的值
		{
			s32_Radian = s32_Radian - 180 * 16;// 角度沿原点镜像
			s32_coeffX = -1, s32_coeffY = -1;//镜像s32_x,s32_y的值
		}
		else if (s32_Radian > 270 * 16 && s32_Radian < 360 * 16) //旋转角度至第四象限，可以把目标角度沿s32_x轴镜像，使得旋转发生在第一象限内。完成旋转后，再镜像s32_y的值
		{
			s32_Radian = 360 * 16 - s32_Radian; // 角度沿s32_x轴镜像
			s32_coeffX = 1, s32_coeffY = -1;//镜像s32_y的值
		}
		s32_Radian = s32_Radian << 3;// 将1.11.4扩展至1.9.7
		for (s32_count_i = 0; s32_count_i < 12; s32_count_i++)
		{
			if (s32_Radian>0)      //大于0表示，在目标角度下面，要继续逆时针旋转，小于0表示目标角度上面，要回转
			{
				s32_Radian = s32_Radian - s16_Angle[s32_count_i];
				*s32p_COS = (s32_x - (s32_y >> s32_count_i));
				*s32p_SIN = (s32_y + (s32_x >> s32_count_i));
				s32_x = *s32p_COS;
				s32_y = *s32p_SIN;
			}
			else if (s32_Radian < 0)
			{
				s32_Radian = s32_Radian + s16_Angle[s32_count_i];
				*s32p_COS = (s32_x + (s32_y >> s32_count_i));
				*s32p_SIN = (s32_y - (s32_x >> s32_count_i));
				s32_x = *s32p_COS;
				s32_y = *s32p_SIN;
			}
			if ((s32_Radian >-2 && s32_Radian < 2 ) || s32_count_i ==11)
			{
				*s32p_COS = s32_coeffX*(*s32p_COS)*s16_para[s32_count_i] >> 11;
				*s32p_SIN = s32_coeffY*(*s32p_SIN)*s16_para[s32_count_i] >> 11;
				break;
			}
		}
	}
}
void myMathClass::CORDIC_32(s32 s32_Radian, s32* s32p_COS, s32 *s32p_SIN)
{
	s16 s16_Angle[12] = { 5760, 3400 ,1797 ,912 , 458, 229, 115, 57, 29, 14, 7,4 };             //cordic角度精度为9.7
	s16 s16_para[12] = { 1448, 1295, 1257, 1247, 1244, 1244, 1244, 1244, 1244, 1244, 1244,1244};    //cos@的结果乘以2048
	s32 s32_x, s32_y;
	s32 s32_count_i;
	s32 s32_coeffX = 0, s32_coeffY = 0;
	//float f_realAngle = 0;
	s32_x = *s32p_COS;
	s32_y = *s32p_SIN;
	unsigned long long temp;
	s16 sign;
	while (s32_Radian >= 360 * 16)
		s32_Radian = s32_Radian - 360 * 16;
	while (s32_Radian < 0)
		s32_Radian = s32_Radian + 360 * 16;
	if (s32_Radian == 0)
	{
		*s32p_COS = s32_x;
		*s32p_SIN = s32_y;
	}
	else if(s32_Radian == 180 * 16)
	{
		*s32p_COS = -s32_x;
		*s32p_SIN = -s32_y;
	}
	else if (s32_Radian == 90 * 16)
	{
		*s32p_COS = -s32_y;
		*s32p_SIN = s32_x;
	}
	else if(s32_Radian == 270 * 16)
	{
		*s32p_COS = s32_y;
		*s32p_SIN = -s32_x;
	}
	else
	{
		if (s32_Radian > 0 && s32_Radian < 90 * 16)
		{
			s32_coeffX = 1, s32_coeffY = 1;
		}
		else if (s32_Radian > 90 * 16 && s32_Radian < 180 * 16) //旋转角度至第二象限，可以把目标角度沿s32_y轴镜像，使得旋转发生在第一象限内。完成旋转后，再镜像s32_x的值
		{
			s32_Radian =180 * 16 - s32_Radian ; // 角度沿s32_y轴镜像
			s32_coeffX = -1, s32_coeffY = 1; //镜像X值
		}
		else if (s32_Radian > 180 * 16 && s32_Radian < 270 * 16) //旋转角度至第三象限，可以把目标角度沿圆点镜像，使得旋转发生在第一象限内。完成旋转后，再镜像s32_x,s32_y的值
		{
			s32_Radian = s32_Radian - 180 * 16;// 角度沿原点镜像
			s32_coeffX = -1, s32_coeffY = -1;//镜像s32_x,s32_y的值
		}
		else if (s32_Radian > 270 * 16 && s32_Radian < 360 * 16) //旋转角度至第四象限，可以把目标角度沿s32_x轴镜像，使得旋转发生在第一象限内。完成旋转后，再镜像s32_y的值
		{
			s32_Radian = 360 * 16 - s32_Radian; // 角度沿s32_x轴镜像
			s32_coeffX = 1, s32_coeffY = -1;//镜像s32_y的值
		}
		s32_Radian = s32_Radian << 3;// 将1.11.4扩展至1.9.7
		for (s32_count_i = 0; s32_count_i < 12; s32_count_i++)
		{
			if (s32_Radian>0)      //大于0表示，在目标角度下面，要继续逆时针旋转，小于0表示目标角度上面，要回转
			{
				s32_Radian = s32_Radian - s16_Angle[s32_count_i];
				*s32p_COS = (s32_x - (s32_y >> s32_count_i));
				*s32p_SIN = (s32_y + (s32_x >> s32_count_i));
				s32_x = *s32p_COS;
				s32_y = *s32p_SIN;
			}
			else if (s32_Radian < 0)
			{
				s32_Radian = s32_Radian + s16_Angle[s32_count_i];
				*s32p_COS = (s32_x + (s32_y >> s32_count_i));
				*s32p_SIN = (s32_y - (s32_x >> s32_count_i));
				s32_x = *s32p_COS;
				s32_y = *s32p_SIN;
			}
			if ((s32_Radian >-2 && s32_Radian < 2 ) || s32_count_i ==11)
			{
				temp = (unsigned long long)(*s32p_COS)*s16_para[s32_count_i]; //防止符号位越位
				if(*s32p_COS < 0)
					sign = -1;
				else 
					sign = 1;
				*s32p_COS = (s32)( s32_coeffX * (sign * temp / 2048));
				
				temp = (unsigned long long)(*s32p_SIN)*s16_para[s32_count_i];
				if(*s32p_SIN < 0)
					sign = -1;
				else 
					sign = 1;
				*s32p_SIN = (s32)( s32_coeffY * (sign * temp / 2048));

				//*s32p_SIN = (s32)((long long)(s32_coeffY*(*s32p_SIN)*s16_para[s32_count_i])>>11);
				break;
			}
		}
	}
}
//****************************
//逆矩阵的旋转变换，顺时针为正方向，只考虑2阶的部分
//| s32_ipointmatrix[0],s32_ipointmatrix[1] |  \/  | cos(theta),-sin(theth)|
//| s32_ipointmatrix[2],s32_ipointmatrix[3] |  /\  | sin(theta), cos(theta)|
//****************************
void myMathClass::MatrixRotate(s16 s16_degrees,s32 s32_ipointmatrix[])
{
	s32 cos = 1024*1024, sin = 0;u8 i;
	//cos求出的值为扩大1024倍的值
	s32 s32_currentmatrix[9];
	CORDIC_32(s16_degrees, &cos, &sin);
	
	for ( i = 0; i < 4; i++)
		s32_currentmatrix[i] = s32_ipointmatrix[i];
	s32_ipointmatrix[0] = ((long long)s32_currentmatrix[0]*cos + (long long)s32_currentmatrix[1]*sin) >> 20; 
	s32_ipointmatrix[1] = ((long long)s32_currentmatrix[1]*cos - (long long)s32_currentmatrix[0]*sin) >> 20;
	s32_ipointmatrix[2] = ((long long)s32_currentmatrix[2]*cos + (long long)s32_currentmatrix[3]*sin) >> 20;
	s32_ipointmatrix[3] = ((long long)s32_currentmatrix[3]*cos - (long long)s32_currentmatrix[2]*sin) >> 20;


}


//****************************
//逆矩阵的旋转变换，只考虑2阶的部分
//  | s32_ipointmatrix[0],s32_ipointmatrix[1] | \/ | 1/ScaleX,      0|
//  | s32_ipointmatrix[2],s32_ipointmatrix[3] | /\ |      0,1/ScaleY)|
//ScaleX和ScaleY格式为1.6.9，s32_ipointmatrix格式为1.6.9
//****************************
void myMathClass::MatrixScaler(s16 ScaleX, s16 ScaleY, s32 s32_ipointmatrix[])
{
	s32 s32_currentmatrix[9];
	int i;
	for (i = 0; i < 4; i++)
		s32_currentmatrix[i] = s32_ipointmatrix[i];
	s32_ipointmatrix[0] = (s32_currentmatrix[0]<<9)/ScaleX; 
	s32_ipointmatrix[1] = (s32_currentmatrix[1]<<9)/ScaleX; 
	s32_ipointmatrix[2] = (s32_currentmatrix[2]<<9)/ScaleY; 
	s32_ipointmatrix[3] = (s32_currentmatrix[3]<<9)/ScaleY; 
}

//****************************
//逆矩阵的错切变换，只考虑2阶的部分（X方向错切）
//s16_degrees表示X方向的纹理错切角度的tan值，tan0表示没有发生错切，tan90表示错切为直线
//| s32_ipointmatrix[0],s32_ipointmatrix[1] |  \/  |       1      ,      0       |
//| s32_ipointmatrix[2],s32_ipointmatrix[3] |  /\  | -tan(s16_degrees),      1       |
//****************************
void myMathClass::MatrixShearX(s16 s16_degrees,s32 s32_ipointmatrix[])
{
	s16 cos = 1024, sin = 0;
	//cos求出的值为扩大1024倍的值
	s32 shearTan;
	s32 s32_currentmatrix[4];
	u8 i;

	CORDIC(s16_degrees, &cos, &sin);
	if(cos == 0) shearTan = -32768;//负最大值
	else shearTan = 512 * sin /cos ;// 1.6.9
	
	for (i = 0; i < 4; i++)
		s32_currentmatrix[i] = s32_ipointmatrix[i];
	s32_ipointmatrix[0] = s32_currentmatrix[0] - ((s32_currentmatrix[1]*shearTan)>> 9);
	s32_ipointmatrix[2] = s32_currentmatrix[2] - ((s32_currentmatrix[3]*shearTan)>> 9);

}
//****************************
//逆矩阵的错切变换，只考虑2阶的部分(Y方向错切）
//s16_degrees表示X方向的纹理错切角度的tan值，tan0表示没有发生错切，tan90表示错切为直线
//| s32_ipointmatrix[0],s32_ipointmatrix[1] |  \/  |       1   ,  -tan(s16_degrees)       |
//| s32_ipointmatrix[2],s32_ipointmatrix[3] |  /\  |       0   ,      1                   |
//****************************
void myMathClass::MatrixShearY(s16 s16_degrees,s32 s32_ipointmatrix[])
{
	s16 cos = 1024, sin = 0;
	//cos求出的值为扩大1024倍的值
	s32 shearTan;
	s32 s32_currentmatrix[4];
	u8 i;

	CORDIC(s16_degrees, &cos, &sin);
	if(cos == 0) shearTan = -32768;//负最大值
	else shearTan = 512 * sin /cos ;// 1.6.9
	
	for (i = 0; i < 4; i++)
		s32_currentmatrix[i] = s32_ipointmatrix[i];

	s32_ipointmatrix[1] = s32_currentmatrix[1] - ((s32_currentmatrix[0]*shearTan)>> 9);
	s32_ipointmatrix[3] = s32_currentmatrix[3] - ((s32_currentmatrix[2]*shearTan)>> 9);
}



//根据输入的s32_x，s32_y坐标计算角度
//CORDIC_ANGLE(s32 s32_Radian,   角度，精度1.27.4
//             s16* s32p_COS,     s32_x坐标 精确度1.11.4
//             s16 *s32p_SIN)     s32_y坐标 精确度1.11.4
void myMathClass::CORDIC_ANGLE(u16* s32_Radian, s16 s32p_COS, s16 s32p_SIN)
{
	s16 s16_Angle[12] = { 5760, 3400 ,1797 ,912 , 458, 229, 115, 57, 29, 14, 7,4 };             //cordic角度精度为9.7
//	s16 s16_para[12] = { 1448, 1295, 1257, 1247, 1244, 1244, 1244, 1244, 1244, 1244, 1244,1244};    //cos@的结果乘以2048
	s16 s32_x, s32_y;
	s32 s32_count_i;
//	s32 s32_coeffX = 0, s32_coeffY = 0;
	s32 offset;
//	float f_realAngle = 0;
	s32_x = s32p_COS;
	s32_y = s32p_SIN;
	if(s32_y == 0 && s32_x >=0) *s32_Radian = 0;
	else if(s32_y == 0 && s32_x <0) *s32_Radian = 180 * 16;
	else if(s32_x == 0 && s32_y > 0) *s32_Radian = 90 * 16;
	else if(s32_x == 0 && s32_y < 0) *s32_Radian = 270 * 16;
	else
	{
		if(s32_x > 0 && s32_y > 0)
			offset = 0;
		else if(s32_x < 0 && s32_y > 0)
		{
			offset = 1;
			s32_x = -s32_x;
		}
		else if(s32_x < 0 && s32_y < 0)
		{
			offset = 2;
			s32_x = -s32_x;
			s32_y = -s32_y;
		}
		else if(s32_x < 0 && s32_y < 0)
		{
			offset = 3;
			s32_y = -s32_y;
		}
		for (s32_count_i = 0; s32_count_i < 12; s32_count_i++)
		{
			if (s32_y>0)
			{
				s32_Radian = s32_Radian + s16_Angle[s32_count_i];
				s32p_COS = (s32_x + (s32_y >> s32_count_i));
				s32p_SIN = (s32_y - (s32_x >> s32_count_i));

				s32_x = s32p_COS;
				s32_y = s32p_SIN;
			}
			else if (s32_y < 0)
			{
				s32_Radian = s32_Radian - s16_Angle[s32_count_i];
				s32p_COS = (s32_x - (s32_y >> s32_count_i));
				s32p_SIN = (s32_y + (s32_x >> s32_count_i));
				s32_x = s32p_COS;
				s32_y = s32p_SIN;
			}
		}
		*s32_Radian = (*s32_Radian) >> 3;
		if(offset == 1)
			*s32_Radian = 180 * 16 - *s32_Radian;
		else if(offset == 2)
			*s32_Radian = 180 * 16 + *s32_Radian;
		else if(offset == 3)
			*s32_Radian = 360 * 16 - *s32_Radian;
	}
}

/////////////////////////////////////////////////
//
//将输入的float变量转换为s1.8.9形式
//参数列表
//     @param1 float f_in float类型输入
//     @param2 s16 s16_out s16类型输出，1.8.9
//
//////////////////////////////////////////////////
funcStatus myMathClass::transferFloatToS16(float f_in, s16* s16_out)
{
	u16 integerPart = (u16)f_in;
	u16 fractionalPart = (u16)((f_in - integerPart) * 512);
	*s16_out = (integerPart << 9) + fractionalPart;
	return AHMI_FUNC_SUCCESS;
}

#endif
