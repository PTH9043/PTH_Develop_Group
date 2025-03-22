#include "EngineDefine.h"
#include "UInputManager.h"
#include "UGameInstance.h"

UInputManager::UInputManager() :
    m_pInput{ nullptr },
    m_pKeyboard{ nullptr },
    m_pMouse{ nullptr },
    m_arrKeyState{},
    m_eMouseState{},
    m_arrPrevKeyState{},
    m_ePrevMouseState{},
    m_eCursorLockMode{ CURSOR_LOCKMODE::EMPTY },
    m_spGraphicDesc{ nullptr },
    m_stWindowRect{}
{
}

void UInputManager::SetCursorLock(const CURSOR_LOCKMODE& _eLockMode)
{
    if (_eLockMode >= CURSOR_LOCKMODE::EMPTY) return;
    RECT rcDest = GetClientAsRect();

    switch (_eLockMode)
    {
    case CURSOR_LOCKMODE::LOCKED:
    case CURSOR_LOCKMODE::CONFINED:
        ClipCursor(&rcDest);
        ShowCursor(false);
        break;
    case CURSOR_LOCKMODE::NONE:
        ClipCursor(nullptr);
        ShowCursor(true);
        break;
    default:
        break;
    }

    m_eCursorLockMode = _eLockMode;
}

void UInputManager::Free()
{
}

HRESULT UInputManager::ReadyInpuDevice(CSHPTRREF<GRAPHICDESC> _spGraphicDesc)
{
    RETURN_CHECK(nullptr == _spGraphicDesc, E_FAIL);
    RETURN_CHECK_DXOBJECT(DirectInput8Create(_spGraphicDesc->hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,
        (void**)&m_pInput, nullptr), E_FAIL);

    m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr);
    m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
    m_pKeyboard->SetCooperativeLevel(_spGraphicDesc->hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_pKeyboard->Acquire();

    m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr);
    m_pMouse->SetDataFormat(&c_dfDIMouse);
    m_pMouse->SetCooperativeLevel(_spGraphicDesc->hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    m_pMouse->Acquire();

    GetClientRect(_spGraphicDesc->hWnd, &m_stWindowRect);
    m_spGraphicDesc = _spGraphicDesc;
    return S_OK;
}

void UInputManager::MouseTick()
{
    memcpy(&m_ePrevMouseState, &m_eMouseState, sizeof(DIMOUSESTATE));
    m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_eMouseState);
}

void UInputManager::KeyTick()
{
    memcpy(&m_arrPrevKeyState, &m_arrKeyState, sizeof(m_arrKeyState));
    m_pKeyboard->GetDeviceState(MAX_KEYSIZE, &m_arrKeyState);
}

void UInputManager::InvokeKeyMethods(const _ubyte& _bFirstKeyID, const _double& _dDeltaTime)
{
    RETURN_CHECK(_bFirstKeyID >= MAX_KEYSIZE, ;);
    KEYMETHODS& Methods = m_arrKeyBinds[_bFirstKeyID];
    // Running Methods
    for (auto& iter : Methods)
    {
        iter.second.Invoke(_dDeltaTime);
    }
}

void UInputManager::AddKeyMethod( _ubyte _bFirstKeyID, const _wstring& _wstrSecondsKeyName, const KEYACTIONFUNC& _keyMethod)
{
    RETURN_CHECK(_bFirstKeyID >= MAX_KEYSIZE, ;);
    const auto& iter = m_arrKeyBinds[_bFirstKeyID].find(_wstrSecondsKeyName);
    RETURN_CHECK(iter != m_arrKeyBinds[_bFirstKeyID].end(), ;);
    m_arrKeyBinds[_bFirstKeyID].insert(std::pair<_wstring, KEYACTIONFUNC>(_wstrSecondsKeyName, _keyMethod));
}

void UInputManager::DeleteKeyMethod( _ubyte _bFirstKeyID, const _wstring& _wstrSecondsKeyName)
{
    RETURN_CHECK(_bFirstKeyID >= MAX_KEYSIZE, ;);
    const auto& iter = m_arrKeyBinds[_bFirstKeyID].find(_wstrSecondsKeyName);
    RETURN_CHECK(iter == m_arrKeyBinds[_bFirstKeyID].end(), ;);

    m_arrKeyBinds[_bFirstKeyID].erase(iter);
}

_bool UInputManager::GetDIKeyDown( _ubyte _bKeyID)
{
    RETURN_CHECK(_bKeyID >= MAX_KEYSIZE, false;);
    if (!m_arrPrevKeyState[_bKeyID] && m_arrKeyState[_bKeyID])
        return true;
    return false;
}

_bool UInputManager::GetDIKeyUp( _ubyte _bKeyID)
{
    RETURN_CHECK(_bKeyID >= MAX_KEYSIZE, false;);
    if (m_arrPrevKeyState[_bKeyID] && !m_arrKeyState[_bKeyID])
        return true;
    return false;
}

_bool UInputManager::GetDIKeyPressing(const _ubyte& _bKeyID)
{
    RETURN_CHECK(_bKeyID >= MAX_KEYSIZE, false;);
    return ((m_arrKeyState[_bKeyID] & 0x80)) > 0;
}

_bool UInputManager::GetDIMBtnDown(_ubyte _eMouseBtn)
{
    if (IsMouseInWindowSize())
    {
        if (!m_ePrevMouseState.rgbButtons[_eMouseBtn] && m_eMouseState.rgbButtons[_eMouseBtn])
        {
            return true;
        }
    }
    return false;
}

_bool UInputManager::GetDIMBtnUp(_ubyte _eMouseBtn)
{
    if (IsMouseInWindowSize())
    {
        if (m_ePrevMouseState.rgbButtons[_eMouseBtn] && !m_eMouseState.rgbButtons[_eMouseBtn])
        {
            return true;
        }
    }
    return false;
}

_bool UInputManager::GetDIMBtnPressing(_ubyte _eMouseBtn)
{
    if (IsMouseInWindowSize())
    {
        return (_bool)(m_eMouseState.rgbButtons[_eMouseBtn] && 0x80);
    }
    return false;
}

_float2 UInputManager::GetMousePosition()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(m_spGraphicDesc->hWnd, &pt);
    return _float2(_float(pt.x), _float(pt.y));
}

_bool UInputManager::IsMouseInWindowSize()
{
    _float2 vMosuePos = GetMousePosition();
    if (m_stWindowRect.left <= vMosuePos.x && m_stWindowRect.top <= vMosuePos.y)
    {
        if (m_stWindowRect.right >= vMosuePos.x && m_stWindowRect.bottom >= vMosuePos.y)
        {
            return true;
        }
    }
    return false;
}

RECT UInputManager::GetClientAsRect()
{
    RECT rcDest;
    GetClientRect(m_spGraphicDesc->hWnd, &rcDest);
    ClientToScreen(m_spGraphicDesc->hWnd, reinterpret_cast<POINT*>(&rcDest.left));
    ClientToScreen(m_spGraphicDesc->hWnd, reinterpret_cast<POINT*>(&rcDest.right));
    return rcDest;
}