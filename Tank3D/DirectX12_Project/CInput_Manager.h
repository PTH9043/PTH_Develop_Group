#pragma once
#include "CBase.h"

enum KEY_VALUE
{
	KEY_END = 255
};

enum DMBUTTON
{
	DM_RIGHT = VK_RBUTTON, DM_LEFT = VK_LBUTTON, DM_CENTER = VK_MBUTTON, DM_END
};

typedef shared_ptr<CDelegate<_bool, const _double&>> KEYBIND;
typedef CDelegate<_bool, const _double&> KEYS;

class CInput_Manager : public CBase
{
protected:
	explicit CInput_Manager() = default;
	explicit CInput_Manager(const CInput_Manager& _rhs) : CBase(_rhs){}
	virtual ~CInput_Manager() = default;
public:
	virtual void Free() override;
public:
	HRESULT NativeConstruct() { return S_OK; }
	void Tick(const HWND& _hWnd);
	HRESULT RegisterMethod(const _ubyte& _KEYID ,const function<_bool(const _double&)>& _pDelegate);
	HRESULT RunningKeyMethods(const _ubyte& _KEYID, const _double& _dTimeDeltas);

	bool Is_Press(const _ubyte& _KEYID);
	bool Is_Pressing(const _ubyte& _KEYID);
	bool Is_MouseClick(const DMBUTTON& _eKey);

public:
	const _float2& GetMousePos() const { return m_vMousePos; }

private:
	std::array<KEYBIND, KEY_END>		m_KeyBinds{};
	_bool														m_bMouseCheck[DM_END]{ false };
	_bool														m_bKeyBoardInput[KEY_END]{ false };
	_bool														m_bKeyBoardPressing[KEY_END]{ false };
	_float2													m_vMousePos = _float2(0.f, 0.f);
};

