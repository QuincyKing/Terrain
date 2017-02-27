#include "Kriging.h"
#include "TerrainModel.h"
#include "D3DInputClass.h"
#include "D3DCameraClass.h"
#include "D3DUtil.h"
#include "BaseData.h"
#include "Math.h"
#include "RW/RWBuilder.h"
#include "resource.h"
#include "TerrainUtility.h"
#include "KeyLevel.h"
#include "Sag.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>
#include <atlstr.h>
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <memory>
using namespace std;

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")   
#pragma comment(lib, "dxguid.lib")

//#pragma comment(linker, "/subsystem:console /entry:WinMainCRTStartup")

#pragma comment(linker,"\"/manifestdependency:type='win32' name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")

#pragma region 声明

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 700
#define WINDOW_TITLE L"三维地质图"
#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZ | D3DFVF_TEX1)
#define MAX_EDIT 20

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
IDirect3DTexture9		*g_pTexture[6] = { 0 };
CameraClass				*g_pCamera = NULL;
TerrainModel			terrainModel;
TerrainModel			UTM;
D3DXMATRIX				g_projection;
int						flag = 0;
int						upgrade = 1;
RWBuilder<Para>			*ParaReader;
vector<double> H;
double mm;
vector<double> E;
vector<double> V;
vector<double> K;
vector<double> mP0; 
DOUBLE Pi = 0;
DOUBLE Qi = 0;
DOUBLE q = 0;
const double mbeta = BasicLib::ToRadian(5.0);
HINSTANCE hgInst = NULL;
wstring Param = L"";
wstring TerrainDataString = L"";
bool InitFlag = false;
HWND GraphicHwnd = NULL;
char SaveResult[MAX_PATH];
char* img[6] = 
{
	"img\\0.jpg",
	"img\\1.jpg",
	"img\\2.jpg",
	"img\\3.jpg",
	"img\\4.jpg",
	"img\\5.jpg"
};

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
VOID					UpgradeInit();
INT_PTR CALLBACK		DlgProcBoard(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK		DlgProcGraphic(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
extern void PackageParam(LPWSTR information, LPWSTR Pi, LPWSTR Qi, LPWSTR q, LPWSTR Param, LPWSTR TerrainData);
extern void UnpackageParam(LPWSTR, DOUBLE &, DOUBLE &, DOUBLE &, wstring &, wstring &);
extern std::vector<wstring> splitManyW(const wstring &original, const wstring &delimiters);
void CalP0(vector<double> &P0);
void InputModel(TerrainModel &terrainModel);
void SagUpgrade(const vector<double> P0, const double Pi, const double Qi, const vector<double> &K,
	const double m, const double q, const double beta);

#pragma endregion

#pragma region Windows区

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	hgInst = hInstance;
	WNDCLASSEX wndClass = { 0 };		
	wndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
	wndClass.cbSize = sizeof(WNDCLASSEX);			
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wndClass.lpfnWndProc = WndProc;					
	wndClass.hInstance = hInstance;					
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

	MoveWindow(hwnd, 250, 20, WINDOW_WIDTH, WINDOW_HEIGHT, true);	
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);			

	WriteLogging LogStart = WriteLogging();
	LogStart.Execute("", " INFO : Start");

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
			if (InitFlag)
			{
				Direct3D_Render(GraphicHwnd);
				Direct3D_Update(GraphicHwnd);
			}
		}
	}
	
	UnregisterClass(L"Terrain", wndClass.hInstance); 
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD wmId = LOWORD(wParam);
	WORD wmEvent = HIWORD(wParam);
	LPWSTR information = {L""};
	HMENU hroot = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU));
	switch (message)						
	{
	case WM_CREATE:
		{
			GraphicHwnd = CreateDialog(hgInst, MAKEINTRESOURCE(IDD_Graphic), hwnd, (DLGPROC)DlgProcGraphic);
			if (GraphicHwnd != NULL)
			{
				ShowWindow(GraphicHwnd, SW_SHOWNA);
			}
		}
		return 0;
	case WM_COMMAND:
	{
		if (wmId == ID_Input)
		{
			information = (LPWSTR)DialogBox(hgInst, MAKEINTRESOURCE(IDD_Board), hwnd, (DLGPROC)DlgProcBoard);
			if (information != NULL)
			{
				UnpackageParam(information, Pi, Qi, q, Param, TerrainDataString);
				Objects_Init();
			}
			else
			{
				MessageBox(hwnd, L"未输入参数", L"错误", MB_ICONERROR);
			}
		}
		if (wmId == ID_About)
		{
			MessageBox(hwnd, L"Terrain软件是用于地形的分层研究", L"关于", MB_OK);
		}
		
		break;
	}
								
	case WM_KEYDOWN:					
		if (wParam == VK_ESCAPE)    
			DestroyWindow(hwnd);
		return 0;		

	case WM_KEYUP:
		if (wParam == VK_ESCAPE) PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
	{
		if (InitFlag)
		{
			Direct3D_CleanUp();
		}
		WriteLogging logWrite = WriteLogging();
		logWrite.Execute("", " INFO : END");
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

INT_PTR CALLBACK DlgProcBoard(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WORD wmId = LOWORD(wParam);
	WORD wmEvent = HIWORD(wParam);
	switch (msg)
	{
	case WM_COMMAND:
	{
		if (wmId == IDC_BTNParam)
		{
			OPENFILENAME opfn;
			WCHAR strFilename[MAX_PATH];
			ZeroMemory(&opfn, sizeof(OPENFILENAME));
			opfn.lStructSize = sizeof(OPENFILENAME);
			opfn.lpstrFilter = L".te\0*.te*\0";
			opfn.nFilterIndex = 1;
			opfn.lpstrFile = strFilename;
			opfn.lpstrFile[0] = '\0';
			opfn.nMaxFile = sizeof(strFilename);
			opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if (GetOpenFileName(&opfn))
			{
				HWND hEdt = GetDlgItem(hdlg, IDC_EDITParam);
				SendMessage(hEdt, WM_SETTEXT, NULL, (LPARAM)opfn.lpstrFile);
			}
		}
		if (wmId == IDC_BTNTerrainData)
		{
			OPENFILENAME opfn;
			WCHAR strFilename[MAX_PATH];
			ZeroMemory(&opfn, sizeof(OPENFILENAME));
			opfn.lStructSize = sizeof(OPENFILENAME);
			opfn.lpstrFilter = L".te\0*.te*\0";
			opfn.nFilterIndex = 1;
			opfn.lpstrFile = strFilename;
			opfn.lpstrFile[0] = '\0';
			opfn.nMaxFile = sizeof(strFilename);
			opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			if (GetOpenFileName(&opfn))
			{
				HWND hEdt = GetDlgItem(hdlg, IDC_EDITTerrainData);
				SendMessage(hEdt, WM_SETTEXT, NULL, (LPARAM)opfn.lpstrFile);
			}
		}
		if (wmId == IDC_BTNGenerate)
		{
			HWND EditPi = GetDlgItem(hdlg, IDC_EDITPi);
			TCHAR Pi[MAX_EDIT];
			GetWindowText(EditPi, Pi, MAX_EDIT);
			HWND EditQi = GetDlgItem(hdlg, IDC_EDITQi);
			TCHAR Qi[MAX_EDIT];
			GetWindowText(EditQi, Qi, MAX_EDIT);
			HWND Editq = GetDlgItem(hdlg, IDC_EDITQ);
			TCHAR q[MAX_EDIT];
			GetWindowText(Editq, q, MAX_EDIT);

			HWND EditParam = GetDlgItem(hdlg, IDC_EDITParam);
			TCHAR Param[MAX_PATH];
			GetWindowText(EditParam, Param, MAX_PATH);
			HWND EditTerrainData = GetDlgItem(hdlg, IDC_EDITTerrainData);
			TCHAR TerrainDataString[MAX_PATH];
			GetWindowText(EditTerrainData, TerrainDataString, MAX_PATH);

			TCHAR information[1000] = L"";
			PackageParam(information, Pi, Qi, q, Param, TerrainDataString);
			EndDialog(hdlg, (INT_PTR)information);
		}
	}
	return 0;
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_CLOSE)
		{
			EndDialog(hdlg, 0);
		}
	}
	return 0;
	default:
		break;
	}
	return FALSE;
}

INT_PTR CALLBACK DlgProcGraphic(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WORD wmId = LOWORD(wParam);
	WORD wmEvent = HIWORD(wParam);
	switch (msg)
	{
	case WM_PAINT:
		if (InitFlag)
		{
			Direct3D_Render(GraphicHwnd);
		}
		return 0;
	default:
		break;
	}
	return FALSE;
}

#pragma endregion

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

	return S_OK;
}

HRESULT Objects_Init()
{
	char szModuleFilePath[MAX_PATH];
	int n = GetModuleFileNameA(0, szModuleFilePath, MAX_PATH); 
	szModuleFilePath[strrchr(szModuleFilePath, '\\') - szModuleFilePath + 1] = 0;
	strcpy(SaveResult, szModuleFilePath);
	
	ReaderPara *ParaTemp = NULL;
	Para para;
	ReaderPara *rp = new ReaderPara();
	ParaReader = new RWBuilder<Para>(rp, string(Param.begin(), Param.end()));
	ParaReader->Execute();
	ParaTemp = dynamic_cast<ReaderPara *>(ParaReader->GetRWBase());
	para = ParaTemp->GetData();
	delete rp;

	E = para.GetE();
	V = para.GetV();
	K = para.GetK();
	Kriging *kriging = new Kriging(string(TerrainDataString.begin(), TerrainDataString.end()));
	delete  kriging;
	InputModel(terrainModel);
	const int count = 8 * terrainModel.m_gtp.m_GTPSet.size();
	D3DXCreateFont(device, 18, 0, 0, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, _T("微软雅黑"), &pFont);
	device->CreateVertexBuffer(3 * count*sizeof(CUSTOMVERTEX), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pVertexBuffer, 0);

	CUSTOMVERTEX *pVertices;
	g_pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	auto index = terrainModel.m_gtp.m_GTPSet.begin();
	for (size_t i = 0; i < terrainModel.m_gtp.m_GTPSet.size(); i++, index++)
	{
		for (int j = 0; j < 8; j++)
		{
			pVertices[i * 24 + j * 3] = CUSTOMVERTEX(terrainModel.m_ver[terrainModel.m_ts[(*index).second.triangles[j]][0]], 0.0f, 0.0f);
			pVertices[i * 24 + j * 3 + 1] = CUSTOMVERTEX(terrainModel.m_ver[terrainModel.m_ts[(*index).second.triangles[j]][1]], 1.0f, 0.0f);
			pVertices[i * 24 + j * 3 + 2] = CUSTOMVERTEX(terrainModel.m_ver[terrainModel.m_ts[(*index).second.triangles[j]][2]], 1.0f, 1.0f);
		}
	}
	g_pVertexBuffer->Unlock();

	CalP0(mP0);
	SagUpgrade(mP0, Pi, Qi, K, mm, q, mbeta);
	UpgradeInit();

	g_pCamera = new CameraClass(device);
	g_pCamera->SetCameraPosition(&D3DXVECTOR3(1200.0f, 548.03f, 1200.49f));
	g_pCamera->SetTargetPosition(&D3DXVECTOR3(10156.0f, 1737.0f, 2000.49f));
	g_pCamera->SetViewMatrix();
	g_pCamera->SetProjMatrix();

	for (size_t imgIndex = 0; imgIndex < 6; imgIndex++)
	{
		char Result[MAX_PATH] = "";
		strcat(Result, SaveResult);
		strcat(Result, img[imgIndex]);
		CString str = CString(Result);
		USES_CONVERSION;
		LPCWSTR wszClassName = A2CW(W2A(str));
		str.ReleaseBuffer();
		D3DXCreateTextureFromFile(device, wszClassName, &g_pTexture[imgIndex]);
	}

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	device->SetRenderState(D3DRS_LIGHTING, false);

	InitFlag = true;

	return S_OK;
}

void UpgradeInit()
{
	const int count = 8 * UTM.m_gtp.m_GTPSet.size();
	device->CreateVertexBuffer(3 * count*sizeof(CUSTOMVERTEX), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &g_pUpgradeVertexBuffer, 0);
	CUSTOMVERTEX *pVertices;
	g_pUpgradeVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	auto index = UTM.m_gtp.m_GTPSet.begin();
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

	return;
}

void Direct3D_Render(HWND hwnd)
{
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffff, 1.0f, 0);
	RECT formatRect;
	GetClientRect(hwnd, &formatRect);

	device->BeginScene();

	if (upgrade == 1)
	{
		device->SetFVF(D3DFVF_CUSTOMVERTEX);
		device->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
		int nums = GTP::GTPPoints * terrainModel.m_gtp.m_GTPSet.size() / terrainModel.m_z;
		int start = 0;
		for (int  levelIndex = 0; levelIndex < terrainModel.m_z; levelIndex++)
		{
			device->SetTexture(0, g_pTexture[levelIndex]);
			device->DrawPrimitive(D3DPT_TRIANGLELIST, start, nums);
			start += nums;
		}
	}
	else
	{
		device->SetFVF(D3DFVF_CUSTOMVERTEX);
		device->SetStreamSource(0, g_pUpgradeVertexBuffer, 0, sizeof(CUSTOMVERTEX));
		int nums = GTP::GTPPoints * UTM.m_gtp.m_GTPSet.size() / UTM.m_z;
		int start = 0;
		for (int levelIndex = 0; levelIndex < UTM.m_z; levelIndex++)
		{
			device->SetTexture(0, g_pTexture[levelIndex]);
			device->DrawPrimitive(D3DPT_TRIANGLELIST, start, nums);
			start += nums;
		}
	}
	int charCount = swprintf_s(g_strFPS, 20, _T("FPS:%0.0f"), Get_FPS());
	pFont->DrawText(NULL, g_strFPS, charCount, &formatRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));
	
	device->EndScene();
	device->Present(0, 0, 0, 0);
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
	if (g_pD3DInput->IsKeyDown(DIK_B) && flag == 0)
	{
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		flag = 1;
	}
	else if (g_pD3DInput->IsKeyDown(DIK_V) && flag == 1)
	{
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		flag = 0;
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

void InputModel(TerrainModel &terrainModel)
{
	RWBuilder<Info>			*InfoReader;
	RWBuilder<Vertices>		*DataReader;

	ReaderInfo *ri = new ReaderInfo();
	InfoReader = new RWBuilder<Info>(ri, "info.te");
	InfoReader->Execute();
	ReaderInfo* InfoTemp = dynamic_cast<ReaderInfo *>(InfoReader->GetRWBase());
	Info info = InfoTemp->GetData();
	delete ri;

	terrainModel.m_x = info.countx;
	terrainModel.m_y = info.county;
	terrainModel.m_z = info.countz;

	ReaderData *rd = new ReaderData();
	DataReader = new RWBuilder<Vertices>(rd, "data.te");
	DataReader->Execute();
	ReaderData * DataTemp = dynamic_cast<ReaderData *>(DataReader->GetRWBase());
	terrainModel.m_ver = DataTemp->GetData();
	delete rd;

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