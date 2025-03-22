#pragma once
#include "CManager.h"

class CInputMgr final  : public CManager
{
public:
	explicit CInputMgr();
	explicit CInputMgr(CGameMgr*& pGameMgr);
	virtual ~CInputMgr();
protected:
	bool m_bKeyList[0xff];
	// Mouse Position 
	VECTOR2 m_v2MousePos;
public:
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Release() override;

	bool Is_Press(const int & iKey);
	bool Is_Pressing(const int & iKey);
	bool Is_MouseClick();
public:
	const VECTOR2& Get_MousePos() const { return m_v2MousePos; }
};

