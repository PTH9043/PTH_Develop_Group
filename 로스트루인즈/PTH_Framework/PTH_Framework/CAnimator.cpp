#include "pch.h"
#include "CAnimator.h"

CAnimator::CAnimator() : m_pCurPlayAnim(nullptr)
{
}

CAnimator::CAnimator(CObj *& pObj) : m_pOwner(pObj), m_pCurPlayAnim(nullptr)
{
}

CAnimator::~CAnimator()
{
	for (auto& iter : m_mAnimList)
	{
		DELETE_MEMORY(iter.second);
	}
	m_mAnimList.clear();
	m_pOwner = nullptr;
}
// 애니메이션 추가
void CAnimator::Add_Animation(const TCHAR* strName, CAnimation *& pAnimation)
{
	if (nullptr == pAnimation)
		return;
	// 만약 Animation이 NULL이 아니라면 Animation 추가
	m_mAnimList.emplace(pair<const TCHAR*, CAnimation*>(strName, pAnimation));
}

void CAnimator::Render_Animation(HDC hDC, const VECTOR2& v2ScrollPos)
{
	// 애니메이션 렌더링
	if(nullptr != m_pCurPlayAnim)
		m_pCurPlayAnim->Render(hDC, m_pOwner, v2ScrollPos);
}

int CAnimator::Update_Animation()
{
	// 애니메이션 업데이트
	if (nullptr != m_pCurPlayAnim)
		m_pCurPlayAnim->Update();
	return 0;
}

void CAnimator::Change_Animation(const TCHAR * strName)
{
	if (true == Is_In_Animation(strName))
	{
		int iIndex = m_pCurPlayAnim->Get_AnimIndex();
		if (nullptr != m_mAnimList[strName])
		{
			if (m_pCurPlayAnim != m_mAnimList[strName])
				m_mAnimList[strName]->Reset_Animation_Index();
		}

		m_chCurAnimation = strName;
		m_pCurPlayAnim = m_mAnimList[m_chCurAnimation];
		m_pCurPlayAnim->Set_AnimationIndex(iIndex);
	}
}

bool CAnimator::Is_In_Animation(const TCHAR * pAnimName)
{
	for (pair<const TCHAR*, CAnimation*> p : m_mAnimList)
	{
		if (nullptr != p.second)
		{
			if (!lstrcmp(p.first, pAnimName))
				return true;
		}
	}
	return false;
}

void CAnimator::Set_CurAnimation(const TCHAR * chCurAnimation)
{
	if (true == Is_In_Animation(chCurAnimation))
	{
		if (nullptr != m_mAnimList[m_chCurAnimation])
		{
			if (m_pCurPlayAnim != m_mAnimList[chCurAnimation])
				m_mAnimList[chCurAnimation]->Reset_Animation_Index();
		}

		m_chCurAnimation = chCurAnimation;
		m_pCurPlayAnim = m_mAnimList[m_chCurAnimation];
	}
}
