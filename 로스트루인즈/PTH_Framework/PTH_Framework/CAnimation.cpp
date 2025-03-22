#include "pch.h"
#include "CAnimation.h"
#include "CAnimClip.h"
#include "CGameMgr.h"
#include "CMyBmp.h"

CAnimation::CAnimation() : m_pAnimBP(nullptr), m_iAnimIndex(0), m_iRed(0), m_iGreen(0), m_iBlue(0)
, m_bLastAnimFix(false) , m_iLastIndex(0), m_iFirstIndex(0), m_eDir(DIR_FOR), m_bPassAnimation(false)
, m_bAnimFix(false)
{
}

CAnimation::CAnimation(const TCHAR * pAnimName) : m_pAnimBP(nullptr), m_iAnimIndex(0)
, m_iRed(0), m_iGreen(0), m_iBlue(0), m_bLastAnimFix(false), m_iLastIndex(0), m_iFirstIndex(0)
, m_eDir(DIR_FOR), m_bPassAnimation(false)
, m_bAnimFix(false)
{
	// 애니메이션 MyBmp를 가져옴
	m_pAnimBP = CGameMgr::Get_Instance()->Get_BmpMgr()->Find_Image_MyBmp(pAnimName);

	m_dwTime = GetTickCount();
	// FPS 100으로 고정
	m_iFps = 50;
}

CAnimation::~CAnimation()
{
	for (auto& iter : m_vecAnimClip)
	{
		DELETE_MEMORY(iter);
	}
	m_vecAnimClip.clear();
}

void CAnimation::Insert_Anim(CAnimClip*& pAnimClip)
{
	// 애니메이션 클립이 비어있으면 리턴
	if (nullptr == pAnimClip)
		return;

	m_vecAnimClip.push_back(pAnimClip);
	m_iLastIndex = static_cast<int>(m_vecAnimClip.size() - 1);
}

int CAnimation::Update()
{
	// 일정 시간이 지났으면
	if (m_iFps + m_dwTime <= GetTickCount())
	{
		// 애니메이션 인덱스를 증가시킨다.
		if (false == m_bAnimFix)
			++m_iAnimIndex;
		else
			m_bAnimFix = false;

		// 다시 시간 초기화
		m_dwTime = GetTickCount();
		m_bPassAnimation = true;
	}

	if (false == m_bLastAnimFix)
	{
		// 애니메이션 인덱스 증가
		if (static_cast<int>(m_vecAnimClip.size()) <= m_iAnimIndex)
		{
			m_iAnimIndex = 0;
		}
	}
	else
	{
		// 애니메이션 인덱스 증가
		if (static_cast<int>(m_vecAnimClip.size()) <= m_iAnimIndex)
		{
			m_bLastAnimFix = false;
			m_iAnimIndex = m_iLastIndex;
		}
	}

	return 0;
}

void CAnimation::Render(HDC hDC, CObj*& pObj, const VECTOR2& v2ScrollPos)
{
	HDC hdc = NULL;
	if (nullptr != m_pAnimBP)
		hdc = m_pAnimBP->Get_MemDC();

	if (static_cast<int>(m_vecAnimClip.size()) < m_iAnimIndex)
		m_iAnimIndex = 0;
	// Play Animation Clip
	m_vecAnimClip[m_iAnimIndex]->Play(hDC, pObj, hdc, v2ScrollPos, m_iRed, m_iGreen, m_iBlue);
	m_bPassAnimation = false;
}

