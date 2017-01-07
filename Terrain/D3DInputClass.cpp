#include "D3DInputClass.h"

D3DInputClass::D3DInputClass()
{
	m_pDirectInput = NULL;
	m_KeyboardDevice = NULL;
	m_MouseDevice = NULL;
	::ZeroMemory(m_KeyBuffer, 256 * sizeof(char));
	::ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}

D3DInputClass::~D3DInputClass()
{
	if (m_KeyboardDevice != NULL)
		m_KeyboardDevice->Unacquire();
	if (m_MouseDevice != NULL)
		m_MouseDevice->Unacquire();
	SAFE_RELEASE(m_pDirectInput);
	SAFE_RELEASE(m_KeyboardDevice);
	SAFE_RELEASE(m_MouseDevice);
	SAFE_DELETE(m_KeyBuffer);
}

HRESULT D3DInputClass::Init(HWND hWnd, HINSTANCE hInstance, DWORD keyboardCoopFlags, DWORD mouseCoopFlags)
{
	HRESULT hr;
	DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_pDirectInput, NULL);

	m_pDirectInput->CreateDevice(GUID_SysMouse, &m_MouseDevice, NULL);
	m_MouseDevice->SetDataFormat(&c_dfDIMouse);
	m_MouseDevice->SetCooperativeLevel(hWnd, mouseCoopFlags);
	m_MouseDevice->Acquire();
	m_MouseDevice->Poll();

	m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_KeyboardDevice, NULL);
	m_KeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	m_KeyboardDevice->SetCooperativeLevel(hWnd, keyboardCoopFlags);
	m_KeyboardDevice->Acquire();
	m_KeyboardDevice->Poll();

	return S_OK;
}

void D3DInputClass::GetInput()
{
	HRESULT h = m_MouseDevice->GetDeviceState(sizeof(m_MouseState), (void**)&m_MouseState);
	if (h)
	{
		m_MouseDevice->Acquire();
		m_MouseDevice->GetDeviceState(sizeof(m_MouseState), (void **)&m_MouseState);
	}

	HRESULT hr = m_KeyboardDevice->GetDeviceState(sizeof(m_KeyBuffer), (void**)&m_KeyBuffer);
	if (hr)
	{
		m_KeyboardDevice->Acquire();
		m_KeyboardDevice->GetDeviceState(sizeof(m_KeyBuffer), (LPVOID)m_KeyBuffer);
	}
}

bool D3DInputClass::IsKeyDown(int iKey)
{
	if (m_KeyBuffer[iKey] & 0x80)
		return true;
	else
		return false;
}

bool D3DInputClass::IsMouseButtonDown(int button)
{
	return (m_MouseState.rgbButtons[button] & 0x80) != 0;
}

float D3DInputClass::MouseDX()
{
	return (float)m_MouseState.lX;
}

float D3DInputClass::MouseDY()
{
	return (float)m_MouseState.lY;
}

float D3DInputClass::MouseDZ()
{
	return (float)m_MouseState.lZ;
}
