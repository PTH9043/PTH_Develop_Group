#pragma once
#include "CManager.h"
#include "CLine.h"

class CLineMgr final : public CManager
{
public:
	explicit CLineMgr();
	explicit CLineMgr(CGameMgr*& pGameMgr);
	virtual ~CLineMgr();
protected:
	list<CLine*> m_LineList;
	LINEINFO m_tLineInfo;
	LINEINFO m_tDrawInfo;
	bool m_bIsMakeLine;

	const int CATCH_OBJ_VALUE;
public:
	// Inis
	virtual void Initialize() override;
	// Update
	virtual int Update() override;
	// Render
	void Render(HDC _hdc, const VECTOR2& v2ScrollPos);
	// Release 
	virtual void Release() override;
	// Check Character In Line
	bool	 Collision_Line(VECTOR2& v2Pos, float* pY);
	CLine* Get_Collision_Line(VECTOR2& v2Pos, float* pY);

	void Insert(CLine*& pLine) { m_LineList.emplace_back(pLine); }



};

