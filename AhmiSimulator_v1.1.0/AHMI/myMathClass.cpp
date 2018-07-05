////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     myMathClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    the math function
// 
////////////////////////////////////////////////////////////////////////////////

#include "myMathClass.h"
#include "publicInclude.h"

#ifdef AHMI_CORE

//*****************************
// CORDIC �㷨������ֵ���Ƕ�
//  CORDIC (s16 *s32p_COS,   // ��ʼ��λ����ת��ķ�λ��X��������һ�����൱�ھ���1/2048
//          s16 *s32p_SIN);  // ��ʼ��λ����ת��ķ�λ��Y��������һ�����൱�ھ���1/2048
//*****************************
void myMathClass::FindAngle(s32* s32p_COS, s32 *s32p_SIN)
{
	s16 s16_para[12] = { 1448, 1295, 1257, 1247, 1244, 1244, 1244, 1244, 1244, 1244, 1244,1244};    //cos@�Ľ������2048
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
		if (*s32p_SIN>0)      //����0��ʾ����Ŀ��Ƕ����棬Ҫ������ʱ����ת��С��0��ʾĿ��Ƕ����棬Ҫ��ת
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
// CORDIC �㷨������Ǻ���
//  CORDIC (s32 s32_Radian, // ��ת�Ƕȣ�����ϵ��˳ʱ��Ϊ�����򣬾���1.27.4
//          s16 *s32p_COS,   // ��ʼ��λ����ת��ķ�λ��X��������һ�����൱�ھ���1/2048
//          s16 *s32p_SIN);  // ��ʼ��λ����ת��ķ�λ��Y��������һ�����൱�ھ���1/2048
//*****************************
void myMathClass::CORDIC(s32 s32_Radian, s16* s32p_COS, s16 *s32p_SIN)
{
	
	s16 s16_Angle[12] = { 5760, 3400 ,1797 ,912 , 458, 229, 115, 57, 29, 14, 7,4 };             //cordic�ǶȾ���Ϊ9.7
	s16 s16_para[12] = { 1448, 1295, 1257, 1247, 1244, 1244, 1244, 1244, 1244, 1244, 1244,1244};    //cos@�Ľ������2048
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
		else if (s32_Radian > 90 * 16 && s32_Radian < 180 * 16) //��ת�Ƕ����ڶ����ޣ����԰�Ŀ��Ƕ���s32_y�᾵��ʹ����ת�����ڵ�һ�����ڡ������ת���پ���s32_x��ֵ
		{
			s32_Radian =180 * 16 - s32_Radian ; // �Ƕ���s32_y�᾵��
			s32_coeffX = -1, s32_coeffY = 1; //����Xֵ
		}
		else if (s32_Radian > 180 * 16 && s32_Radian < 270 * 16) //��ת�Ƕ����������ޣ����԰�Ŀ��Ƕ���Բ�㾵��ʹ����ת�����ڵ�һ�����ڡ������ת���پ���s32_x,s32_y��ֵ
		{
			s32_Radian = s32_Radian - 180 * 16;// �Ƕ���ԭ�㾵��
			s32_coeffX = -1, s32_coeffY = -1;//����s32_x,s32_y��ֵ
		}
		else if (s32_Radian > 270 * 16 && s32_Radian < 360 * 16) //��ת�Ƕ����������ޣ����԰�Ŀ��Ƕ���s32_x�᾵��ʹ����ת�����ڵ�һ�����ڡ������ת���پ���s32_y��ֵ
		{
			s32_Radian = 360 * 16 - s32_Radian; // �Ƕ���s32_x�᾵��
			s32_coeffX = 1, s32_coeffY = -1;//����s32_y��ֵ
		}
		s32_Radian = s32_Radian << 3;// ��1.11.4��չ��1.9.7
		for (s32_count_i = 0; s32_count_i < 12; s32_count_i++)
		{
			if (s32_Radian>0)      //����0��ʾ����Ŀ��Ƕ����棬Ҫ������ʱ����ת��С��0��ʾĿ��Ƕ����棬Ҫ��ת
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
	s16 s16_Angle[12] = { 5760, 3400 ,1797 ,912 , 458, 229, 115, 57, 29, 14, 7,4 };             //cordic�ǶȾ���Ϊ9.7
	s16 s16_para[12] = { 1448, 1295, 1257, 1247, 1244, 1244, 1244, 1244, 1244, 1244, 1244,1244};    //cos@�Ľ������2048
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
		else if (s32_Radian > 90 * 16 && s32_Radian < 180 * 16) //��ת�Ƕ����ڶ����ޣ����԰�Ŀ��Ƕ���s32_y�᾵��ʹ����ת�����ڵ�һ�����ڡ������ת���پ���s32_x��ֵ
		{
			s32_Radian =180 * 16 - s32_Radian ; // �Ƕ���s32_y�᾵��
			s32_coeffX = -1, s32_coeffY = 1; //����Xֵ
		}
		else if (s32_Radian > 180 * 16 && s32_Radian < 270 * 16) //��ת�Ƕ����������ޣ����԰�Ŀ��Ƕ���Բ�㾵��ʹ����ת�����ڵ�һ�����ڡ������ת���پ���s32_x,s32_y��ֵ
		{
			s32_Radian = s32_Radian - 180 * 16;// �Ƕ���ԭ�㾵��
			s32_coeffX = -1, s32_coeffY = -1;//����s32_x,s32_y��ֵ
		}
		else if (s32_Radian > 270 * 16 && s32_Radian < 360 * 16) //��ת�Ƕ����������ޣ����԰�Ŀ��Ƕ���s32_x�᾵��ʹ����ת�����ڵ�һ�����ڡ������ת���پ���s32_y��ֵ
		{
			s32_Radian = 360 * 16 - s32_Radian; // �Ƕ���s32_x�᾵��
			s32_coeffX = 1, s32_coeffY = -1;//����s32_y��ֵ
		}
		s32_Radian = s32_Radian << 3;// ��1.11.4��չ��1.9.7
		for (s32_count_i = 0; s32_count_i < 12; s32_count_i++)
		{
			if (s32_Radian>0)      //����0��ʾ����Ŀ��Ƕ����棬Ҫ������ʱ����ת��С��0��ʾĿ��Ƕ����棬Ҫ��ת
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
				temp = (unsigned long long)(*s32p_COS)*s16_para[s32_count_i]; //��ֹ����λԽλ
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
//��������ת�任��˳ʱ��Ϊ������ֻ����2�׵Ĳ���
//| s32_ipointmatrix[0],s32_ipointmatrix[1] |  \/  | cos(theta),-sin(theth)|
//| s32_ipointmatrix[2],s32_ipointmatrix[3] |  /\  | sin(theta), cos(theta)|
//****************************
void myMathClass::MatrixRotate(s16 s16_degrees,s32 s32_ipointmatrix[])
{
	s32 cos = 1024*1024, sin = 0;u8 i;
	//cos�����ֵΪ����1024����ֵ
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
//��������ת�任��ֻ����2�׵Ĳ���
//  | s32_ipointmatrix[0],s32_ipointmatrix[1] | \/ | 1/ScaleX,      0|
//  | s32_ipointmatrix[2],s32_ipointmatrix[3] | /\ |      0,1/ScaleY)|
//ScaleX��ScaleY��ʽΪ1.6.9��s32_ipointmatrix��ʽΪ1.6.9
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
//�����Ĵ��б任��ֻ����2�׵Ĳ��֣�X������У�
//s16_degrees��ʾX�����������нǶȵ�tanֵ��tan0��ʾû�з������У�tan90��ʾ����Ϊֱ��
//| s32_ipointmatrix[0],s32_ipointmatrix[1] |  \/  |       1      ,      0       |
//| s32_ipointmatrix[2],s32_ipointmatrix[3] |  /\  | -tan(s16_degrees),      1       |
//****************************
void myMathClass::MatrixShearX(s16 s16_degrees,s32 s32_ipointmatrix[])
{
	s16 cos = 1024, sin = 0;
	//cos�����ֵΪ����1024����ֵ
	s32 shearTan;
	s32 s32_currentmatrix[4];
	u8 i;

	CORDIC(s16_degrees, &cos, &sin);
	if(cos == 0) shearTan = -32768;//�����ֵ
	else shearTan = 512 * sin /cos ;// 1.6.9
	
	for (i = 0; i < 4; i++)
		s32_currentmatrix[i] = s32_ipointmatrix[i];
	s32_ipointmatrix[0] = s32_currentmatrix[0] - ((s32_currentmatrix[1]*shearTan)>> 9);
	s32_ipointmatrix[2] = s32_currentmatrix[2] - ((s32_currentmatrix[3]*shearTan)>> 9);

}
//****************************
//�����Ĵ��б任��ֻ����2�׵Ĳ���(Y������У�
//s16_degrees��ʾX�����������нǶȵ�tanֵ��tan0��ʾû�з������У�tan90��ʾ����Ϊֱ��
//| s32_ipointmatrix[0],s32_ipointmatrix[1] |  \/  |       1   ,  -tan(s16_degrees)       |
//| s32_ipointmatrix[2],s32_ipointmatrix[3] |  /\  |       0   ,      1                   |
//****************************
void myMathClass::MatrixShearY(s16 s16_degrees,s32 s32_ipointmatrix[])
{
	s16 cos = 1024, sin = 0;
	//cos�����ֵΪ����1024����ֵ
	s32 shearTan;
	s32 s32_currentmatrix[4];
	u8 i;

	CORDIC(s16_degrees, &cos, &sin);
	if(cos == 0) shearTan = -32768;//�����ֵ
	else shearTan = 512 * sin /cos ;// 1.6.9
	
	for (i = 0; i < 4; i++)
		s32_currentmatrix[i] = s32_ipointmatrix[i];

	s32_ipointmatrix[1] = s32_currentmatrix[1] - ((s32_currentmatrix[0]*shearTan)>> 9);
	s32_ipointmatrix[3] = s32_currentmatrix[3] - ((s32_currentmatrix[2]*shearTan)>> 9);
}



//���������s32_x��s32_y�������Ƕ�
//CORDIC_ANGLE(s32 s32_Radian,   �Ƕȣ�����1.27.4
//             s16* s32p_COS,     s32_x���� ��ȷ��1.11.4
//             s16 *s32p_SIN)     s32_y���� ��ȷ��1.11.4
void myMathClass::CORDIC_ANGLE(u16* s32_Radian, s16 s32p_COS, s16 s32p_SIN)
{
	s16 s16_Angle[12] = { 5760, 3400 ,1797 ,912 , 458, 229, 115, 57, 29, 14, 7,4 };             //cordic�ǶȾ���Ϊ9.7
//	s16 s16_para[12] = { 1448, 1295, 1257, 1247, 1244, 1244, 1244, 1244, 1244, 1244, 1244,1244};    //cos@�Ľ������2048
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
//�������float����ת��Ϊs1.8.9��ʽ
//�����б�
//     @param1 float f_in float��������
//     @param2 s16 s16_out s16���������1.8.9
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
