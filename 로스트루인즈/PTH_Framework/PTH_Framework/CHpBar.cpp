#include "pch.h"
#include "CHpBar.h"
#include "CGameMgr.h"
#include "CCharacter.h"
#include "CObjMgr.h"

CHpBar::CHpBar()
{
}

CHpBar::CHpBar(CCharacter* pOwner, const TCHAR* szMainPath,
	const TCHAR* szBgPath, const RGBINFO& tInfo, const int& iBarValue)
	: CBar(pOwner, szMainPath,  szBgPath, tInfo, iBarValue)
{
	Initialize();
}

CHpBar::~CHpBar()
{
	Release();
}

void CHpBar::Initialize()
{
	if (nullptr == m_pCharacter)
	{
		MessageBox(g_hWnd, L"Warnning", L"BAR_CHARCTER_ERROR", MB_OK);
		return;
	}

	m_v2DrawSize = VECTOR2(static_cast<float>(m_iBarValue * MUTI_VALUE), 20.f);
	m_v2Size = m_v2DrawSize;
	m_pValText = new CText(this, L"³ª´®°íµñ", tagRGB(255,255,255));
	m_pValText->Set_CurPos(VECTOR2(3.f, 23.f) + m_v2CurPos);
	m_pValText->Set_Size(VECTOR2(20.f, 0.f));
	Update_Rect();
}

int CHpBar::Update()
{
	if (true == m_bDead)
		return OBJ_DEADEVENT;
	Update_Rect();
	return OBJ_NOEVENT;
}

void CHpBar::Late_Update()
{
}

void CHpBar::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	Calculate_Bar();
	Draw_Img_Blt(_hdc, m_v2CurPos, m_v2DrawSize + VECTOR2(6.f, 6.f), m_pBackgroundImg);
	Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(3.f, 3.f), m_v2Size, m_pBarImg, m_tTranspercyRGB);
	m_pValText->Render(_hdc, v2ScrollPos);
}

void CHpBar::Collision_Actor(CObj *& pObj)
{
}

void CHpBar::Release()
{
}

void CHpBar::Calculate_Bar()
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
