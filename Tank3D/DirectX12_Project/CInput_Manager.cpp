#include "CInput_Manager.h"

void CInput_Manager::Free()
{
}

void CInput_Manager::Tick(const HWND& _hWnd)
{
	// Mouse Move 
	POINT	pt{};
	GetCursorPos(&pt);
	ScreenToClient(_hWnd, &pt);	// 스크린 상태의 마우스 좌표를 우리가 생성한 윈도우 창 좌표로 변경해주는 함수
	m_vMousePos = _float2(static_cast<_float>(pt.x), static_cast<_float>(pt.y));
}

HRESULT CInput_Manager::RegisterMethod(const _ubyte& _KEYID, const function<_bool(const _double&)>& _pDelegate)
{
	if(nullptr == _pDelegate)
		return E_FAIL;

	if (_KEYID >= KEY_END)
		return E_FAIL;

	if (nullptr != m_KeyBinds[_KEYID])
		return E_FAIL;

	m_KeyBinds[_KEYID] = KEYS::Create(_pDelegate);
	return S_OK;
}

HRESULT CInput_Manager::RunningKeyMethods(const _ubyte& _KEYID, const _double& _dTimeDeltas)
{
	if (_KEYID >= KEY_END)
		return E_FAIL;

	if (nullptr == m_KeyBinds[_KEYID])
		return E_FAIL;

	m_KeyBinds[_KEYID]->Invoke(_dTimeDeltas);
	return S_OK;
}

bool CInput_Manager::Is_Press(const _ubyte& _KEYID)
{
	if (m_bKeyBoardInput[_KEYID] && !(GetAsyncKeyState(_KEYID) & 0x8000))
	{
		m_bKeyBoardInput[_KEYID] = !m_bKeyBoardInput[_KEYID];
		return true;
	}
	else  if (!m_bKeyBoardInput[_KEYID] && (GetAsyncKeyState(_KEYID) & 0x8000))
		m_bKeyBoardInput[_KEYID] = !m_bKeyBoardInput[_KEYID];
	return false;
}

bool CInput_Manager::Is_Pressing(const _ubyte& _KEYID)
{
	if (m_bKeyBoardInput[_KEYID] && !(GetAsyncKeyState(_KEYID) & 0x8001))
	{
		m_bKeyBoardInput[_KEYID] = !m_bKeyBoardInput[_KEYID];
		return true;
	}
	else  if (!m_bKeyBoardInput[_KEYID] && (GetAsyncKeyState(_KEYID) & 0x8001))
		return true;

	return false;
}

bool CInput_Manager::Is_MouseClick(const DMBUTTON& _eKey)
{
	bool press = GetAsyncKeyState(_eKey) && 0x8001;
	if (true == press)
	{
		m_bMouseCheck[_eKey] = true;
		return true;
	}
	else
		m_bMouseCheck[_eKey] = false;
	return false;
}
