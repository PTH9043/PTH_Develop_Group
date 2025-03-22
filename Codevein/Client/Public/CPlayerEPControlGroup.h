#pragma once
#include "Client_Defines.h"
#include "CBase.h"
#include "C2DEffectParticle.h"
#include "CParticle.h"
#include"CEffectParticleGroup.h"

BEGIN(Client)

class CPlayerEPControlGroup : public CBase
{
private:
	explicit CPlayerEPControlGroup(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPlayerEPControlGroup() = default;

	vector<CEffectParticleGroup*> Get_PlayerWalkSmokeList() { return m_PlayerWalkSmokeList; }

public:
	HRESULT NativeConstruct(class CPlayer* _pPlayer);
	void Update(class CPlayer* _pPlayer, class CPlayerStateMachine* _pPlayerStateMachine, const _double& _dTimeDelta);
	void Late_Update(class CPlayer* _pPlayer, class CPlayerStateMachine* _pPlayerStateMachine, const _double& _dTimeDelta);

	void Active_PlayerWalSmokeList();
	// Pos
	void Weapon_Pos_Particle(class CEquipment* _pWeapon);
private:
	// Player Walk Smoke ist
	vector<CEffectParticleGroup*> m_PlayerWalkSmokeList;
	_uint m_iPlayerWalkSmokeIndex = 0;
	// Player Sword  
	CEffectParticleGroup* m_pPlayerSworldParticle;

	CUSTIMER		m_tWalkTime = CUSTIMER(1.5f);
public:
	static CPlayerEPControlGroup* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CPlayer* _pPlayer);
	virtual void Free() override;
};

END