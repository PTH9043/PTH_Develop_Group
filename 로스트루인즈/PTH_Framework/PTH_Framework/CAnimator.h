#pragma once
#include"CObj.h"
#include "CAnimation.h"

class CAnimator
{
public:
	explicit CAnimator();
	explicit CAnimator(CObj*& pObj);
	virtual ~CAnimator();
protected:
	// Animation List
	map<const TCHAR*, CAnimation*> m_mAnimList;
	// Current Play Anim
	CAnimation* m_pCurPlayAnim;
	// 현재 애니메이터의 주인
	CObj* m_pOwner;
	// 현재 애니메이션 
	const TCHAR* m_chCurAnimation;
public:
	// 애니메이션 생성
	void Add_Animation(const TCHAR* strName, CAnimation*& pAnimation);
	// 애니메이션 렌더링
	void Render_Animation(HDC hDC, const VECTOR2& v2ScrollPos);
	// 애니메이션 업데이트
	int Update_Animation();
	// Change Animation 
	void Change_Animation(const TCHAR* strName);
public:
	// 애니메이션이 있는지 체크
	bool Is_In_Animation(const TCHAR* pAnimName);
public:
	// get
	CObj* Get_Owner() { return m_pOwner; }
	// 현재 애니메이션 set
	void Set_CurAnimation(const TCHAR* chCurAnimation);
	// 현재 애니메이션을 가져옴
	CAnimation* Get_Animation() { return m_pCurPlayAnim; }
	// get _Cur Animation
	const TCHAR* Get_CurAnimName() const { return m_chCurAnimation; }
};

