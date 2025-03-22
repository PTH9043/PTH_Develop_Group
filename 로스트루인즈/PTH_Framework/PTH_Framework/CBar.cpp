#include "pch.h"
#include "CBar.h"
#include "CGameMgr.h"
#include "CBmpMgr.h"

CBar::CBar() : m_pCharacter(nullptr) ,DIVINE_VALUE(100), MUTI_VALUE(10), m_iBarValue(100), 
m_pBarImg(nullptr), m_pBackgroundImg(nullptr), m_pValText(nullptr)
{
	Initialize();
}

CBar::CBar(CCharacter* pOwner, const TCHAR * szMainPath, const TCHAR * szBgPath, const RGBINFO & tInfo, const int& iBarValue)
	: m_tTranspercyRGB(tInfo), m_pCharacter(pOwner), m_iBarValue(iBarValue)
		, DIVINE_VALUE(100), MUTI_VALUE(10), m_bChange(false)
	, m_pValText(nullptr)
{
	CBmpMgr* pBmpMgr = CGameMgr::Get_Instance()->Get_BmpMgr();
	m_pBarImg = pBmpMgr->Find_Image_MyBmp(szMainPath);
	m_pBackgroundImg = pBmpMgr->Find_Image_MyBmp(szBgPath);

	Initialize();
}

CBar::~CBar()
{
	Release();
}

void CBar::Initialize()
{
	m_v2CurPos = VECTOR2(0, 0);
	m_v2DrawSize = VECTOR2(100.f, 40.f);
	m_v2Size = m_v2DrawSize;
	Update_Rect();
}

int CBar::Update()
{
	if (true == m_bDead)
		return OBJ_DEADEVENT;
	Update_Rect();
	return OBJ_NOEVENT;
}

void CBar::Late_Update()
{
}

void CBar::Render(HDC _hdc, const VECTOR2 & v2ScrollPos)
{

}

void CBar::Collision_Actor(CObj *& pObj)
{
}

void CBar::Release()
{
	m_pBackgroundImg = nullptr;
	DELETE_MEMORY(m_pValText);
}

void CBar::Init_Pos(const VECTOR2 & v2Pos)
{
	if (nullptr == m_pValText)
	{
		MessageBox(g_hWnd, L"Warnning", L"BAR_TEXT_ERROR", MB_OK);
		return;
	}

	this->m_v2CurPos = v2Pos;
	m_pValText->Set_CurPos(m_v2CurPos + VECTOR2(3.0f, 3.f));
}

void CBar::Update_Rect()
{
	VECTOR2 v2RealSize = (m_v2Size) / 2.0f;
	// Collision Size
	m_tRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tRect.bottom = static_cast<long>(m_v2CurPos.y);
	// CenterPos
	m_v2CenterPos = VECTOR2(m_v2CurPos.x, m_v2CurPos.y - v2RealSize.y);
	// Draw Size
	v2RealSize = m_v2DrawSize / 2.f;
	m_tDrawRect.left = static_cast<long>(m_v2CurPos.x - v2RealSize.x);
	m_tDrawRect.top = static_cast<long>(m_v2CurPos.y - m_v2Size.y);
	m_tDrawRect.right = static_cast<long>(m_v2CurPos.x + v2RealSize.x);
	m_tDrawRect.bottom = static_cast<long>(m_v2CurPos.y);
}
