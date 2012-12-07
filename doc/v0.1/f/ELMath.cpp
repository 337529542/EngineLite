#include "ELMath.h"
#include <math.h>

void ELMatrix4x4::resetMatrix()
{
	m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = 0.0;
	m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = 0.0;
	m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = 0.0;
	m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
}

void ELMatrix4x4::setTrans( const ELVector3& v )
{
	m[3][0] = v.x;
	m[3][1] = v.y;
	m[3][2] = v.z;
}

void ELMatrix4x4::setScale( const ELVector3& v )
{
	m[0][0] = v.x;
	m[1][1] = v.y;
	m[2][2] = v.z;
}

void ELMatrix4x4::setPitch( const float Radian )
{
	m[1][1] = cos(Radian);
	m[1][2] = sin(Radian);

	m[2][1] = -sin(Radian);
	m[2][2] = cos(Radian);
}

void ELMatrix4x4::setRoll( const float Radian )
{
	m[0][0] = cos(Radian);
	m[0][1] = sin(Radian);

	m[1][0] = -sin(Radian);
	m[1][1] = cos(Radian);
}

void ELMatrix4x4::setYaw( const float Radian )
{
	m[0][0] = cos(Radian);
	m[0][2] = -sin(Radian);

	m[2][0] = sin(Radian);
	m[2][2] = cos(Radian);
}

void ELMatrix4x4::mul( const ELMatrix4x4& m2 )
{
	float md[4][4];

	md[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
	md[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
	md[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
	md[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

	md[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
	md[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
	md[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
	md[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

	md[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
	md[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
	md[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
	md[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

	md[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
	md[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
	md[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
	md[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] = md[i][j];

}

ELMatrix4x4::ELMatrix4x4()
{
	resetMatrix();
}

#include <D3DX10Math.h>

void ELMatrix4x4::makePerspectiveMatrix( float FovY, float Aspect, float Zn, float Zf )
{
	//yScale = cot(fovY/2)
	float yScale = 1/tan(FovY/2);

	//xScale = yScale / aspect ratio
	float xScale = yScale / Aspect;

	//m22 = zf/(zf-zn) 
	float m22 = Zf/(Zf-Zn);

	//m32 = -zn*zf/(zf-zn)
	float m32 = -Zn*Zf/(Zf-Zn);


	m[0][0] = xScale;	m[0][1] = 0.0;		m[0][2] = 0.0;		m[0][3] = 0.0;
	m[1][0] = 0.0;		m[1][1] = yScale;	m[1][2] = 0.0;		m[1][3] = 0.0;
	m[2][0] = 0.0;		m[2][1] = 0.0;		m[2][2] = m22;		m[2][3] = 1.0;
	m[3][0] = 0.0;		m[3][1] = 0.0;		m[3][2] = m32;		m[3][3] = 0.0;

	/*D3DXMATRIX mat;
	D3DXMatrixPerspectiveFovLH(&mat, FovY, Aspect, Zn, Zf);

	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			m[i][j] = mat.m[i][j];*/
}