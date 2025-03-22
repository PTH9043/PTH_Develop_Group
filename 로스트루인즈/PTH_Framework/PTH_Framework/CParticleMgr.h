#pragma once
#include "CManager.h"
#include "CParticle.h"

class CParticleMgr : public CManager
{
public:
	explicit CParticleMgr();
	explicit CParticleMgr(CGameMgr*& pGameMgr);
	virtual ~CParticleMgr();
protected:
	vector<CParticle*>	m_vecParticleList[PTCL_END];

	int					m_iUseParticleIndex;
	const int&			USE_NUM = 50;
	const int&			MAX_PARTICLE_NUM = 1000;
public:
	// CManager을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual int Update() override;
	virtual void Release() override;

public:
	void Out_Blood(const VECTOR2& v2Pos, const VECTOR2& v2Dir, const float& fPower);

};

