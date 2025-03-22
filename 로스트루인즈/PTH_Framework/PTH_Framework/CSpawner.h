#pragma once
#include "Include.h"

class CScene;

class CSpawner
{
public:
	explicit CSpawner();
	explicit CSpawner(CScene* pOwner, const RECT& tRect, const VECTOR2& v2Pos);
	virtual ~CSpawner();
protected:
	CScene*		m_pOwner;

	VECTOR2		m_v2CurPos;
	RECT			m_tRect;
public:
	virtual void Initialize();
	virtual void Spawn();
	virtual void Release();
public:
	CScene* Get_Owenr() { return m_pOwner; }
	const VECTOR2& Get_CurPos() const { return m_v2CurPos; }
	const RECT& Get_Rect() const { return m_tRect; }

	void Set_CurPos(const VECTOR2& v2CurPos) { this->m_v2CurPos = v2CurPos; }
	void Set_Rect(const RECT& tRect) {this->m_tRect = tRect; }
};

