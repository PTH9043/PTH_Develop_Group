#pragma once
#include "CManager.h"

class CScrollMgr  final  : public CManager
{
public:
	explicit CScrollMgr();
	explicit CScrollMgr(CGameMgr*& pGameMgr);
	virtual ~CScrollMgr();
protected:
	VECTOR2 m_v2Pos;
	VECTOR2 m_v2SelectPos;
public:
	void Scroll_Lock();
public:
	// CManager을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	void Render(HDC _hdc);
	virtual void Release() override;
protected:
	void Move_Scroll();
public:
	// get 
	const VECTOR2& Get_Pos() const { return m_v2Pos; }
	const VECTOR2& Get_SelectPos() const { return m_v2SelectPos; }
	// set
	void Set_Pos(const VECTOR2& v2Pos) { this->m_v2Pos = v2Pos; }
};

