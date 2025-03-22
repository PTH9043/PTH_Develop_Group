#include "pch.h"
#include "CMpBar.h"

CMpBar::CMpBar()
{
	Initialize();
}

CMpBar::CMpBar(CCharacter* pOwner, const TCHAR* szMainPath,
	const TCHAR* szBgPath, const RGBINFO& tInfo, const int& iBarValue)
	: CBar(pOwner, szMainPath, szBgPath, tInfo, iBarValue)
{
	Initialize();
}


CMpBar::~CMpBar()
{
	Release();
}

void CMpBar::Initialize()
{
	if (nullptr == m_pCharacter)
	{
		MessageBox(g_hWnd, L"Warnning", L"BAR_CHARCTER_ERROR", MB_OK);
		return;
	}

	int iMp = m_pCharacter->Get_Status().iCurMp;

	m_v2CurPos = VECTOR2(0.f, 20.f);
	m_v2DrawSize = VECTOR2(static_cast<float>(iMp * MUTI_VALUE), 20.f);
	m_v2Size = m_v2DrawSize;
	m_pValText = new CText(this, L"³ª´®°íµñ", tagRGB(255, 255, 255));
	m_pValText->Set_CurPos(VECTOR2(3.f, 28.f) + m_v2CurPos);
	m_pValText->Set_Size(VECTOR2(20.f, 0.f));
	Update_Rect();
}

int CMpBar::Update()
{
	if (true == m_bDead)
		return OBJ_DEADEVENT;
	Update_Rect();
	return OBJ_NOEVENT;
}

void CMpBar::Late_Update()
{

}

void CMpBar::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{
	Calculate_Bar();
	Draw_Img_Blt(_hdc, m_v2CurPos, m_v2DrawSize + VECTOR2(6.f, 6.f), m_pBackgroundImg);
	Draw_Img_GDI(_hdc, m_v2CurPos + VECTOR2(3.f, 3.f), m_v2Size, m_pBarImg, m_tTranspercyRGB);
	m_pValText->Render(_hdc, v2ScrollPos);
}

void CMpBar::Collision_Actor(CObj *& pObj)
{
}

void CMpBar::Release()
{
}

void CMpBar::Calculate_Bar()
{
	int iMp = m_pCharacter->Get_Status().iCurMp;
	int iMaxMp = m_pCharacter->Get_Status().iMaxMp;
;

	TCHAR szValue[50];
	wsprintf(szValue, L"%d / %d", iMp, iMaxMp);
	m_pValText->Set_Text(wstring(szValue));
	m_pValText->Set_CurPos(VECTOR2(3.f, 3.f) + m_v2CurPos);
}