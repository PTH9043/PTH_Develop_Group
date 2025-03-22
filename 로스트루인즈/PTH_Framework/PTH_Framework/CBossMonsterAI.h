#pragma once
#include "CMonsterAI.h"

class CBossMonsterAI : public CMonsterAI
{
public:
	explicit CBossMonsterAI();
	explicit CBossMonsterAI(CMonster * pOwner, CPlayer* pTarget);
	virtual ~CBossMonsterAI();
public:
	// CAI을(를) 통해 상속됨
	virtual int Update() override;
	virtual void Late_Update() override;
	virtual void Relase() override;
protected:
	virtual void Control_Monster() override;
};

