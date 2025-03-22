#pragma once
#include"Include.h"

class CAnimClip;
class CMyBmp;
class CObj;

class CAnimation
{
public:
	explicit CAnimation();
	explicit CAnimation(const TCHAR* pAnimName);
	virtual ~CAnimation();
protected:
	// 애니메이션을 저장해 놓은 vector(안에 vector(AnimationClip))
	vector<CAnimClip*>	m_vecAnimClip;
	CMyBmp*					m_pAnimBP;
	// 출력할 애니메이션 인덱스
	int								m_iAnimIndex;
	// 시간
	DWORD						m_dwTime;
	// 프레임
	int								m_iFps;
	// RGB
	int								m_iRed;
	int								m_iGreen;
	int								m_iBlue;
	// 마지막 애니메이션 고정
	bool								m_bLastAnimFix;	
	// 애니메이션의 마지막 인덱스
	int								m_iLastIndex;
	// 애니메이션의 첫 번째 인덱스
	int								m_iFirstIndex;
	// Direciton
	EDIR								m_eDir;
	// 애니메이션 패스
	bool								m_bPassAnimation;
	// 애니메이션을 해당 프레임에 고정시키기 위한 변수
	bool								m_bAnimFix;
public:
	// Insert Animation
	virtual void Insert_Anim(CAnimClip*& pAnimClip);
	// 업데이트
	virtual int Update();
	// 렌더
	virtual void Render(HDC hDC, CObj*& pObj, const VECTOR2& v2ScrollPos);

	virtual void Reset_Animation_Index() { m_iAnimIndex = 0; }
public:
	void Set_RGB(const int& iRed, const int& iGreen, const int& iBlue) { this->m_iRed = iRed; this->m_iGreen = iGreen; m_iBlue = iBlue; }
	void Set_AnimationIndex(const int& iIndex) { this->m_iAnimIndex = iIndex; }
	void Set_LastAnimFix(const bool& bIsLastAnim) { this->m_bLastAnimFix = bIsLastAnim; }
	void Set_Fps(const int& iFps) { this->m_iFps = iFps; }
	void Set_AnimFix(const bool& bAnimFix) { this->m_bAnimFix = bAnimFix; }
	// Get Animation Index
	const  int& Get_AnimIndex() const { return m_iAnimIndex; }
	// 첫번째, 마지막 인덱스 체크
	const  int& Get_FirstIndex() const { return m_iFirstIndex; }
	const  int& Get_LastIndex() const { return m_iLastIndex; }
	// Dir
	const EDIR& Get_Direction() const { return m_eDir; }
	// 마지막인지 체크
	bool Is_last() const { if (m_iLastIndex == m_iAnimIndex) return true; else return false; }
	bool Is_First() const { if (m_iFirstIndex == m_iAnimIndex)return true; else return false; }
	const bool& Is_Pass() const { return m_bPassAnimation; }
};

