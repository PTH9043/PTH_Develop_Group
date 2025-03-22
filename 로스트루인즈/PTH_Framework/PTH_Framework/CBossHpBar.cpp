#include "pch.h"
#include "CBossHpBar.h"

CBossHpBar::CBossHpBar()
{
	Initialize();
}

CBossHpBar::CBossHpBar(CCharacter * pOwner, const TCHAR * szMainPath, const TCHAR * szBgPath,
	const RGBINFO & tInfo, const int & iBarValue)
	: CHpBar(pOwner, szMainPath, szBgPath, tInfo, iBarValue)
{

}

CBossHpBar::~CBossHpBar()
{
	Release();
}

void CBossHpBar::Initialize()
{

}

int CBossHpBar::Update()
{
	if (true == m_bDead)
		return OBJ_DEADEVENT;
	Update_Rect();
	return OBJ_NOEVENT;
}

void CBossHpBar::Late_Update()
{
}

void CBossHpBar::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	Calculate_Bar();
	Draw_Img_Blt(_hdc, m_v2CurPos, m_v2DrawSize + VECTOR2(6.f, 6.f), m_pBackgroundImg);
	Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(3.f, 3.f), m_v2Size, m_pBarImg, m_tTranspercyRGB);
	m_pValText->Render(_hdc, v2ScrollPos);
}

void CBossHpBar::Collision_Actor(CObj *& pObj)
{
}

void CBossHpBar::Release()
{
}

void CBossHpBar::Calculate_Bar()
{
	float fHp = static_cast<float>(m_pCharacter->Get_Status().iCurHp);
	float fMaxHp = static_cast<float>(m_pCharacter->Get_Status().iMaxHp);

	float fDivValue = fHp / fMaxHp;
	float fValue = m_v2DrawSize.x;
	m_v2Size.x = static_cast<int>(fDivValue * fValue);


	TCHAR szValue[50];
	wsprintf(szValue, L"%d / %d", m_pCharacter->Get_Status().iCurHp, m_pCharacter->Get_Status().iMaxHp);
	m_pValText->Set_Text(wstring(szValue));
	m_pValText->Set_CurPos(VECTOR2(3.f, 3.f) + m_v2CurPos);
}
