#pragma once
#include "CManager.h"
#include "CEffect.h"

class CEffectMgr : public CManager
{
public:
	explicit CEffectMgr();
	explicit CEffectMgr(CGameMgr* pGameMgr);
	virtual ~CEffectMgr();
protected:
	const int							MAX_EFFECT_SIZE = 100;
	vector<CEffect*>			m_vecEffectList[EFCT_END];

	int									m_iBloodEffectIndex;
public:
	// CManager을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Release() override;

	void Blood_Effect(CObj* pObj, const VECTOR2& v2Dir ,const int& iIndex);
};

