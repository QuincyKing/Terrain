#include "Kriging.h"
#include "TerrainModel.h"
#include "D3DInputClass.h"
#include "D3DCameraClass.h"
#include "D3DUtil.h"
#include "BaseData.h"
#include "Sag.h"
#include "KeyLevel.h"
#include "Math.h"
#include "RW/ReaderBase.h"
#include "RW/WriteBase.h"
#include "RW/ReaderInfo.h"
#include "RW/ReaderTerrainData.h"
#include "RW/ReaderData.h"
#include "RW/ReaderPara.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include <atlstr.h>
#include <Windows.h>
#include <algorithm>
#include <memory>
using namespace std;

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")   
#pragma comment(lib, "dxguid.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 700
#define WINDOW_TITLE L"三维地质图"
#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZ | D3DFVF_TEX1)

wchar_t					g_strFPS[50];
IDirect3DDevice9		*device = 0;
ID3DXFont				*pFont = NULL;
D3DInputClass			*g_pD3DInput = NULL;
D3DXMATRIX				g_matWorld;
IDirect3DVertexBuffer9  *g_pVertexBuffer = NULL;
IDirect3DVertexBuffer9	*g_pUpgradeVertexBuffer = NULL;
IDirect3DTexture9		*g_pTexture1 = NULL;
IDirect3DTexture9		*g_pTexture2 = NULL;
IDirect3DTexture9		*g_pTexture3 = NULL;
IDirect3DTexture9		*g_pTexture4 = NULL;
IDirect3DTexture9		*g_pTexture5 = NULL;
IDirect3DTexture9		*g_pTexture6 = NULL;
IDirect3DTexture9		*g_pTexture7 = NULL;
CameraClass				*g_pCamera = NULL;
TerrainModel			terrainModel;
TerrainModel			UTM;
D3DXMATRIX				g_projection;
int						flag = 0;
int						upgrade = 1;
ReaderPara<>*			rp;
ReaderInfo<>*			rif;
ReaderData<>*			rdt;
extern vector<double> H;
extern double mm;
vector<double> E;
vector<double> V;
vector<double> K;
vector<double> mP0; 
const double mPi = 400000; 
const double mQi = 400000; 
const double mq = 1.1;
const double mbeta = BasicLib::ToRadian(5.0);

#pragma comment(linker, "/subsystem:console /entry:WinMainCRTStartup")

struct CUSTOMVERTEX
{
	float _x, _y, _z;
	float _u, _v;
	CUSTOMVERTEX() {}
	CUSTOMVERTEX(Point3D temp) :_x(temp.x), _y(temp.y), _z(temp.z), _u(temp.u), _v(temp.v)
	{ }
	CUSTOMVERTEX(Point3D temp, float _u, float _v) :_x(temp.x), _y(temp.y), _z(temp.z), _u(_u), _v(_v)
	{ }
};

LRESULT CALLBACK		WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HRESULT					Direct3D_Init(HWND hwnd);
HRESULT					Objects_Init();
VOID					Direct3D_Render(HWND hwnd);
VOID					Direct3D_CleanUp();
VOID					Direct3D_Update(HWND hwnd);
BOOL					Device_Read(IDirectInputDevice8 *pDIDevice, void* pBuffer, long lSize);
float					Get_FPS();
VOID					InputModel(TerrainModel &terrainModel);
VOID					SagUpgrade(const vector<double>, const double, const double, const vector<double> &, const double, const double, const double);
VOID					UpgradeInit();
VOID					CalP0(vector<double> &P0);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wndClass = { 0 };					
	wndClass.cbSize = sizeof(WNDCLASSEX);			
	wndClass.style = CS_HREDRAW | CS_VREDRAW;		
	wndClass.lpfnWndProc = WndProc;					
	wndClass.cbClsExtra = 0;						
	wndClass.cbWndExtra = 0;						
	wndClass.hInstance = hInstance;					
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE); 
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);					
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);		
	wndClass.lpszMenuName = NULL;									
	wndClass.lpszClassName = L"Terrain";			

	if (!RegisterClassEx(&wndClass))				
		return -1;

	HWND hwnd = CreateWindow(L"Terrain", WINDOW_TITLE,		
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (E_FAIL == Direct3D_Init(hwnd))
	{
		MessageBox(hwnd, _T("Direct初始化失败！"), _T("消息窗口"), 0);
	}

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);	
	ShowWindow(hwnd, nShowCmd);				
	UpdateWindow(hwnd);						

	g_pD3DInput = new D3DInputClass();
	g_pD3DInput->Init(hwnd, hInstance, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	MSG msg = { 0 };						
	while (msg.message != WM_QUIT)			
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   
		{
			TranslateMessage(&msg);		
			DispatchMessage(&msg);		
		}
		else
		{
			Direct3D_Render(hwnd);  
			Direct3D_Update(hwnd);
		}
	}
	UnregisterClass(L"Terrain", wndClass.hInstance);  
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)						
	{
	case WM_PAINT:						
		Direct3D_Render(hwnd);                 
		ValidateRect(hwnd, NULL);	
		break;									

	case WM_KEYDOWN:					
		if (wParam == VK_ESCAPE)    
			DestroyWindow(hwnd);
		break;		

	case WM_KEYUP:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		break;

	case WM_DESTROY:					
		Direct3D_CleanUp();			
		PostQuitMessage(0);			
		break;	

	default:										
		return DefWindowProc(hwnd, message, wParam, lParam);		
	}

	return 0;
}

HRESULT Direct3D_Init(HWND hwnd)
{
	IDirect3D9 *d3d9;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	D3DCAPS9 caps;
	int vp = 0;
	
	if (FAILED(d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		MessageBox(hwnd, L"", L"window", 0);
		return E_FAIL;
	}
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = WINDOW_WIDTH;
	d3dpp.BackBufferHeight = WINDOW_HEIGHT;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = true;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		vp, &d3dpp, &device)))
	{
		MessageBox(hwnd, L"2", L"window", 0);
		return E_FAIL;
	}
	SAFE_RELEASE(d3d9);

	Objects_Init();
	return S_OK;
}

HRESULT Objects_Init()
{
	Para para;
	rp = new ReaderPara<>("para.te");
	para = rp->GetData();
	E = para.GetE();
	V = para.GetV();
	K = para.GetK();
	Kriging *kriging = new Kriging();
	delete  kriging;
	InputModel(terrainModel);
	const int count = 8*terrainModel.m_gtp.m_GTPSet.size();
	D3DXCreateFont(device, 18, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &pFont);
	device->CreateVertexBuffer(3*count*sizeof(CUSTOMVERTEX), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pVertexBuffer, 0);
	
	CUSTOMVERTEX *pVertices;
	g_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	multimap<int, GTP>::iterator index = terrainModel.m_gtp.m_GTPSet.begin();
	for (size_t i = 0; i < terrainModel.m_gtp.m_GTPSet.size(); i++, index++)
	{
		for (int j = 0; j < 8; j++)
		{
			pVertices[ i*24+j*3 ] = CUSTOMVERTEX(terrainModel.m_ver[terrainModel.m_ts[ (*index).second.triangles[j] ][0]], 0.0f, 0.0f);
			pVertices[ i*24+j*3+1 ] = CUSTOMVERTEX(terrainModel.m_ver[terrainModel.m_ts[ (*index).second.triangles[j] ][1]], 1.0f, 0.0f);
			pVertices[ i*24+j*3+2 ] = CUSTOMVERTEX(terrainModel.m_ver[ terrainModel.m_ts[ (*index).second.triangles[j] ][2]], 1.0f, 1.0f);
		}
	}
	g_pVertexBuffer->Unlock();
	CalP0(mP0);
	SagUpgrade(mP0, mPi, mQi, K, mm, mq, mbeta);
	UpgradeInit();

	g_pCamera = new CameraClass(device);
	g_pCamera->SetCameraPosition(&D3DXVECTOR3(1200.0f, 548.03f, 1200.49f));
	g_pCamera->SetTargetPosition(&D3DXVECTOR3(10156.0f, 1737.0f, 2000.49f));
	g_pCamera->SetViewMatrix();
	g_pCamera->SetProjMatrix();

	D3DXCreateTextureFromFile(device, L"./img/0.jpg", &g_pTexture1);
	D3DXCreateTextureFromFile(device, L"./img/1.jpg", &g_pTexture2);
	D3DXCreateTextureFromFile(device, L"./img/2.jpg", &g_pTexture3);
	D3DXCreateTextureFromFile(device, L"./img/3.jpg", &g_pTexture4);
	D3DXCreateTextureFromFile(device, L"./img/4.jpg", &g_pTexture5);
	D3DXCreateTextureFromFile(device, L"./img/5.jpg", &g_pTexture6);

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	device->SetRenderState(D3DRS_LIGHTING, false);
	return S_OK;
}

void UpgradeInit()
{
	const int count = 8 * UTM.m_gtp.m_GTPSet.size();
	device->CreateVertexBuffer(3 * count*sizeof(CUSTOMVERTEX), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pUpgradeVertexBuffer, 0);
	CUSTOMVERTEX *pVertices;
	g_pUpgradeVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	multimap<int, GTP>::iterator index = UTM.m_gtp.m_GTPSet.begin();
	for (size_t i = 0; i < UTM.m_gtp.m_GTPSet.size(); i++, index++)
	{
		for (int j = 0; j < 8; j++)
		{
			pVertices[i * 24 + j * 3] = CUSTOMVERTEX(UTM.m_ver[terrainModel.m_ts[(*index).second.triangles[j]][0]], 0.0f, 0.0f);
			pVertices[i * 24 + j * 3 + 1] = CUSTOMVERTEX(UTM.m_ver[terrainModel.m_ts[(*index).second.triangles[j]][1]], 1.0f, 0.0f);
			pVertices[i * 24 + j * 3 + 2] = CUSTOMVERTEX(UTM.m_ver[terrainModel.m_ts[(*index).second.triangles[j]][2]], 1.0f, 1.0f);
		}
	}
	g_pUpgradeVertexBuffer->Unlock();
}

void  UnitFrameStates()
{
	device->SetFVF(NULL);

	device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	device->SetRenderState(D3DRS_FOGENABLE, FALSE);

	device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
}

void Direct3D_Render(HWND hwnd)
{
	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xffffff, 1.0f, 0);
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);
	vector<double> tm;
	
	for (int f = 0; f < 11; f++)
	{
		tm.push_back(terrainModel.m_ver[f].z - UTM.m_ver[f].z);
	}
	
	device->BeginScene();
	
	if (upgrade == 1)
	{
		device->SetFVF(D3DFVF_CUSTOMVERTEX);
		device->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
		int nums = 24 * terrainModel.m_gtp.m_GTPSet.size() / terrainModel.m_z;
		device->SetTexture(0, g_pTexture1);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nums);
		device->SetTexture(0, g_pTexture2);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, nums, nums);
		device->SetTexture(0, g_pTexture3);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 2 * nums, nums);
		device->SetTexture(0, g_pTexture4);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 3 * nums, nums);
		device->SetTexture(0, g_pTexture5);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 4 * nums, nums);
		device->SetTexture(0, g_pTexture6);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 5 * nums, nums);

		/*int nums1 = (terrainModel.m_x - 1)*(terrainModel.m_y - 1) * 2;
		int nums2 = (terrainModel.m_x - 1)*(terrainModel.m_y - 1) * 2 + (terrainModel.m_x - 1)*terrainModel.m_y * 2 +
		(terrainModel.m_y - 1)*terrainModel.m_x * 2;
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nums1);
		for (int k = 1; k <= terrainModel.m_z; k++)
		{
		device->SetTexture(0, g_pTexture[k]);
		nums1 += nums2;
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 3*nums1, nums2);
		}*/
		/*device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, terrainModel.m_ver.m_vertices.size(), 0, terrainModel.m_ts.m_triangleSet.size());*/
	}
	else
	{
		device->SetFVF(D3DFVF_CUSTOMVERTEX);
		device->SetStreamSource(0, g_pUpgradeVertexBuffer, 0, sizeof(CUSTOMVERTEX));
		int nums = 24 * UTM.m_gtp.m_GTPSet.size() / UTM.m_z;
		device->SetTexture(0, g_pTexture1);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nums);
		device->SetTexture(0, g_pTexture2);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, nums, nums);
		device->SetTexture(0, g_pTexture3);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 2 * nums, nums);
		device->SetTexture(0, g_pTexture4);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 3 * nums, nums);
		device->SetTexture(0, g_pTexture5);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 4 * nums, nums);
		device->SetTexture(0, g_pTexture6);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 5 * nums, nums);
	}
	int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.0f"), Get_FPS());
	pFont->DrawText(NULL, g_strFPS, charCount, &formatRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));
	UnitFrameStates();
	device->EndScene();
	device->Present(0, 0, 0, 0);
}

void SagUpgrade(const vector<double> P0, const double Pi, const double Qi, const vector<double> &K,
	const double m, const double q, const double beta)
{
	UTM = terrainModel;
	shared_ptr<KeyLevel<double> > keylevel = make_shared<KeyLevel<double> >(E, H, V);
	shared_ptr<Sag> sag = make_shared<Sag>(P0, Pi, Qi, K[keylevel->GetKeyLevel()], m, q, beta);
	size_t z;
	int tmp = keylevel->GetKeyLevel();
	for (z = 0; z < K.size() - keylevel->GetKeyLevel(); z++)
	{
		for (size_t x = 0; x < size_t(UTM.m_x); x++)
		{
			for (size_t y = 0; y < size_t(UTM.m_y); y++)
			{
				double Z = sag->Deflection(UTM.m_ver[z*UTM.m_x*UTM.m_y + y*UTM.m_x + x].x,
					UTM.m_ver[z*UTM.m_x*UTM.m_y + y*UTM.m_x + x].y, z);
				if (Z < 0.00000)
				{
					Z *= -1;
				}
				UTM.m_ver[z*UTM.m_x*UTM.m_y + y*UTM.m_x + x].SetZ(float(UTM.m_ver[z*UTM.m_x*UTM.m_y + y*UTM.m_x + x].z - Z));
			}
		}
	}
	
	//非弯曲带整体下移煤层的高度
	for (; z < size_t(UTM.m_z - 1); z++)
	{
		for (size_t x = 0; x < size_t(UTM.m_x); x++)
		{
			for (size_t y = 0; y < size_t(UTM.m_y); y++)
			{
				double diffZ = UTM.m_ver[(UTM.m_z - 1)*UTM.m_x*UTM.m_y + y*UTM.m_x + x].z - UTM.m_ver[(UTM.m_z - 2)*UTM.m_x*UTM.m_y + y*UTM.m_x + x].z;
				UTM.m_ver[z*UTM.m_x*UTM.m_y + y*UTM.m_x + x].SetZ(float(UTM.m_ver[z*UTM.m_x*UTM.m_y + y*UTM.m_x + x].z + diffZ));
			}
		}
	}
}

void Direct3D_Update(HWND hwnd)
{
	g_pD3DInput->GetInput();

	if (g_pD3DInput->IsKeyDown(DIK_1) || g_pD3DInput->IsKeyDown(DIK_NUMPAD1))
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if (g_pD3DInput->IsKeyDown(DIK_1) || g_pD3DInput->IsKeyDown(DIK_NUMPAD2))
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	if (g_pD3DInput->IsKeyDown(DIK_A))		g_pCamera->MoveAlongRightVec(-1.0f);
	if (g_pD3DInput->IsKeyDown(DIK_D))		g_pCamera->MoveAlongRightVec(1.0f);
	if (g_pD3DInput->IsKeyDown(DIK_W))		g_pCamera->MoveAlongLookVec(1.0);
	if (g_pD3DInput->IsKeyDown(DIK_S))		g_pCamera->MoveAlongLookVec(-1.0f);
	if (g_pD3DInput->IsKeyDown(DIK_R))		g_pCamera->MoveAlongUpVec(1.0f);
	if (g_pD3DInput->IsKeyDown(DIK_F))		g_pCamera->MoveAlongUpVec(-1.0f);

	if (g_pD3DInput->IsKeyDown(DIK_LEFT))	g_pCamera->RotationUpVec(-0.003f);
	if (g_pD3DInput->IsKeyDown(DIK_RIGHT))  g_pCamera->RotationUpVec(0.003f);
	if (g_pD3DInput->IsKeyDown(DIK_UP))		g_pCamera->RotationRightVec(-0.003f);
	if (g_pD3DInput->IsKeyDown(DIK_DOWN))	g_pCamera->RotationRightVec(0.003f);
	if (g_pD3DInput->IsKeyDown(DIK_Q))		g_pCamera->RotationLookVec(0.001f);
	if (g_pD3DInput->IsKeyDown(DIK_E))		g_pCamera->RotationLookVec(-0.001f);

	if (g_pD3DInput->IsMouseButtonDown(0))
	{
		g_pCamera->RotationUpVec(g_pD3DInput->MouseDX()* 0.001f);
		g_pCamera->RotationRightVec(g_pD3DInput->MouseDY() * 0.001f);
	}

	if (g_pD3DInput->IsKeyDown(DIK_U) && upgrade == 1)
	{
		upgrade ^= 1;
	}
	if (g_pD3DInput->IsKeyDown(DIK_Y) && upgrade == 0)
	{
		upgrade ^= 1;
	}

	D3DXMATRIX matView;
	g_pCamera->CalculateViewMatrix(&matView);
	device->SetTransform(D3DTS_VIEW, &matView);
}

void Direct3D_CleanUp()
{
	SAFE_RELEASE(pFont);
	SAFE_RELEASE(device);
}

void InputModel(TerrainModel &terrainModel)
{
	rif = new ReaderInfo<>("info.te");
	Info info = rif->GetData();
	terrainModel.m_x = info.countx;
	terrainModel.m_y = info.county;
	terrainModel.m_z = info.countz;

	rdt = new ReaderData<>("data.te");
	terrainModel.m_ver = rdt->GetData();

	int xPos, yPos, zPos;
	int countTri = 0;
	int countZ = info.countx * info.county;
	for (size_t i = 0; i < terrainModel.m_ver.m_vertices.size() - (terrainModel.m_x * terrainModel.m_y); i++)
	{
		zPos = i / (info.countx * info.county);
		yPos = i % (info.countx * info.county) / info.countx;
		xPos = i % (info.countx * info.county) % info.countx;
		if ((xPos + 1 < info.countx) && (yPos + 1 < info.county))
		{
			uint32_t triSet[8];
			Triangle triUp(i, i + info.countx, i + 1);
			terrainModel.m_ts.push(triUp);
			triSet[0] = countTri++;
			Triangle triRight1(i, i + countZ, i + countZ + info.countx);
			terrainModel.m_ts.push(triRight1);
			triSet[1] = countTri++;
			Triangle triRight2(i, i + info.countx, i + countZ + info.countx);
			terrainModel.m_ts.push(triRight2);
			triSet[2] = countTri++;
			Triangle triLeft1(i + 1, i + countZ + 1, i + info.countx);
			terrainModel.m_ts.push(triLeft1);
			triSet[3] = countTri++;
			Triangle triLeft2(i + info.countx + countZ, i + info.countx, i + countZ + 1);
			terrainModel.m_ts.push(triLeft2);
			triSet[4] = countTri++;
			Triangle triFront1(i, i + countZ + 1, i + 1);
			terrainModel.m_ts.push(triFront1);
			triSet[5] = countTri++;
			Triangle triFront2(i, i + countZ, i + countZ + 1);
			terrainModel.m_ts.push(triFront2);
			triSet[6] = countTri++;
			Triangle triDown(i + countZ, i + countZ + 1, i + countZ + info.countx);
			terrainModel.m_ts.push(triDown);
			triSet[7] = countTri++;

			GTP temp(triSet);
			terrainModel.m_gtp.push(temp, zPos);
		}

		if ((xPos - 1 >= 0) && (yPos - 1 >= 0))
		{
			uint32_t triSet[8];
			Triangle triUp(i, i - info.countx, i - 1);
			terrainModel.m_ts.push(triUp);
			triSet[0] = countTri++;
			Triangle triRight1(i, i + countZ, i + countZ - info.countx);
			terrainModel.m_ts.push(triRight1);
			triSet[1] = countTri++;
			Triangle triRight2(i, i - info.countx, i + countZ - info.countx);
			terrainModel.m_ts.push(triRight2);
			triSet[2] = countTri++;
			Triangle triLeft1(i - 1, i + countZ - 1, i - info.countx);
			terrainModel.m_ts.push(triLeft1);
			triSet[3] = countTri++;
			Triangle triLeft2(i - info.countx + countZ, i - info.countx, i + countZ - 1);
			terrainModel.m_ts.push(triLeft2);
			triSet[4] = countTri++;
			Triangle triFront1(i, i + countZ - 1, i - 1);
			terrainModel.m_ts.push(triFront1);
			triSet[5] = countTri++;
			Triangle triFront2(i, i + countZ, i + countZ - 1);
			terrainModel.m_ts.push(triFront2);
			triSet[6] = countTri++;
			Triangle triDown(i + countZ, i + countZ - 1, i + countZ - info.countx);
			terrainModel.m_ts.push(triDown);
			triSet[7] = countTri++;

			GTP temp(triSet);
			terrainModel.m_gtp.push(temp, zPos);
		}
	}

}

void CalP0(vector<double> &P0)
{
	double sumHV = 0.0;
	double sumEH = 0.0;
	double EH;
	for (size_t index = 0; index < H.size(); index++)
	{
		EH = E[index] * pow(H[index], 3);
		sumEH += EH;
		sumHV += H[index] * V[index];
		if (fabs(sumEH) < 0.00000001)
		{
			mP0.push_back(1.0);
			continue;
		}
		mP0.push_back(EH * sumHV / sumEH);
	}
}

float Get_FPS()
{
	static float  fps = 0;
	static int    frameCount = 0;
	static float  currentTime = 0.0f;
	static float  lastTime = 0.0f;

	frameCount++;
	currentTime = timeGetTime()*0.001f;

	if (currentTime - lastTime > 1.0f)
	{
		fps = (float)frameCount / (currentTime - lastTime);
		lastTime = currentTime;
		frameCount = 0;
	}
	return fps;
}