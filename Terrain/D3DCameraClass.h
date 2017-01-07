#ifndef D3DCAMERACLASS_H
#define D3DCAMERACLASS_H

#include <d3d9.h>
#include <d3dx9.h>

class CameraClass
{
private:
	D3DXVECTOR3			m_vRightVector;
	D3DXVECTOR3			m_vUpVector;
	D3DXVECTOR3			m_vLookVector;
	D3DXVECTOR3			m_vCameraPosition;
	D3DXVECTOR3			m_vTargetPosition;
	D3DXMATRIX			m_matView;
	D3DXMATRIX			m_matProj;
	IDirect3DDevice9	*m_pd3dDevice;
public:
	void CalculateViewMatrix(D3DXMATRIX *pMatrix);
	void GetProjMatrix(D3DXMATRIX *pMatrix) { *pMatrix = m_matProj; }
	void GetCameraPosition(D3DXVECTOR3 *pVector){ *pVector = m_vCameraPosition; }
	void GetLookVector(D3DXVECTOR3 *pVector) { *pVector = m_vLookVector; }
	void SetTargetPosition(D3DXVECTOR3 *pLookat = NULL);
	void SetCameraPosition(D3DXVECTOR3 *pVector = NULL);
	void SetViewMatrix(D3DXMATRIX *pMatrix = NULL);
	void SetProjMatrix(D3DXMATRIX *pMatrix = NULL);
	void MoveAlongRightVec(FLOAT fUnits);
	void MoveAlongUpVec(FLOAT fUnits);
	void MoveAlongLookVec(FLOAT fUnits);
	void RotationRightVec(FLOAT fAngle);
	void RotationUpVec(FLOAT fAngle);
	void RotationLookVec(FLOAT fAngle);

public:
	CameraClass(IDirect3DDevice9 *pd3dDevice);
	virtual ~CameraClass(void);
};
#endif