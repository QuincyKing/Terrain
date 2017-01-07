#ifndef D3DINPUTCLASS
#define DDINPUTCLASS

#include "D3DUtil.h"
#include <dinput.h>

class D3DInputClass
{
public:
	IDirectInput8		* m_pDirectInput;
	IDirectInputDevice8 * m_KeyboardDevice;
	char				m_KeyBuffer[256];
	IDirectInputDevice8 * m_MouseDevice;
	DIMOUSESTATE		m_MouseState;
public:
	D3DInputClass(void);
	~D3DInputClass(void);
	HRESULT Init(HWND hWnd, HINSTANCE hInstance, DWORD keyboardCoopFlags, DWORD mouseCoopFlags);
	void GetInput();
	bool IsKeyDown(int iKey);
	bool IsMouseButtonDown(int button);
	float MouseDX();
	float MouseDY();
	float MouseDZ();
};
#endif