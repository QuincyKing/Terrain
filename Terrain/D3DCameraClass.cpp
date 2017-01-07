#include "D3DCameraClass.h"

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH	800						
#define WINDOW_HEIGHT	600					
#endif

CameraClass::CameraClass(IDirect3DDevice9 *pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
	m_vCameraPosition = D3DXVECTOR3(0.0f, 0.0f, -250.0f);
	m_vLookVector = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_vRightVector = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_vUpVector = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void CameraClass::CalculateViewMatrix(D3DXMATRIX *pMatrix)
{
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);
	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);
	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);

	pMatrix->_11 = m_vRightVector.x;
	pMatrix->_12 = m_vUpVector.x;
	pMatrix->_13 = m_vLookVector.x;
	pMatrix->_14 = 0.0f;

	pMatrix->_21 = m_vRightVector.y;
	pMatrix->_22 = m_vUpVector.y;
	pMatrix->_23 = m_vLookVector.y;
	pMatrix->_24 = 0.0f;

	pMatrix->_31 = m_vRightVector.z;
	pMatrix->_32 = m_vUpVector.z;
	pMatrix->_33 = m_vLookVector.z;
	pMatrix->_34 = 0.0f;

	pMatrix->_41 = -D3DXVec3Dot(&m_vRightVector, &m_vCameraPosition);
	pMatrix->_42 = -D3DXVec3Dot(&m_vUpVector, &m_vCameraPosition);
	pMatrix->_43 = -D3DXVec3Dot(&m_vLookVector, &m_vCameraPosition);
	pMatrix->_44 = 1.0f;
}

void CameraClass::SetTargetPosition(D3DXVECTOR3 *pVector)
{
	if (pVector == NULL) m_vTargetPosition = *pVector;
	else m_vTargetPosition = D3DXVECTOR3(0.f, 0.0f, 1.0f);

	m_vLookVector = m_vTargetPosition - m_vCameraPosition;
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);

	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);
	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);
}

void CameraClass::SetCameraPosition(D3DXVECTOR3 *pVector)
{
	D3DXVECTOR3 V = D3DXVECTOR3(0.0f, 0.0f, -250.0f);
	m_vCameraPosition = pVector ? (*pVector) : V;
}

void CameraClass::SetViewMatrix(D3DXMATRIX *pMatrix)
{
	if (pMatrix) m_matView = *pMatrix;
	else CalculateViewMatrix(&m_matView);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);
	/*m_vRightVector = D3DXVECTOR3(m_matView._11, m_matView._12, m_matView._13);
	m_vUpVector = D3DXVECTOR3(m_matView._21, m_matView._22, m_matView._23);
	m_vLookVector = D3DXVECTOR3(m_matView._31, m_matView._32, m_matView._33);*/
}

void CameraClass::SetProjMatrix(D3DXMATRIX *pMatrix)
{
	if (pMatrix != NULL) m_matProj = *pMatrix;
	else D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(90), (float)((double)WINDOW_WIDTH / WINDOW_HEIGHT), 1.0f, 10000.0f);
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);
}

void CameraClass::MoveAlongRightVec(FLOAT fUnits)
{
	m_vCameraPosition += m_vRightVector * fUnits;
	m_vTargetPosition += m_vRightVector * fUnits;
}

void CameraClass::MoveAlongLookVec(FLOAT fUnits)
{
	m_vCameraPosition += m_vLookVector * fUnits;
	m_vTargetPosition += m_vLookVector * fUnits;
}

void CameraClass::MoveAlongUpVec(FLOAT fUnits)
{
	m_vCameraPosition += m_vUpVector * fUnits;
	m_vTargetPosition += m_vUpVector * fUnits;
}

void CameraClass::RotationRightVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vRightVector, fAngle);
	D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
	//MessageBox(NULL, LPCWSTR(""), LPCWSTR(""), 0);
}

void CameraClass::RotationUpVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vUpVector, fAngle);
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);
}

void CameraClass::RotationLookVec(FLOAT fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vLookVector, fAngle);
	D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);
}

CameraClass::~CameraClass(VOID)
{
}