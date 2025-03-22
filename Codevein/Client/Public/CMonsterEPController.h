#pragma once
#include "Client_Defines.h"
#include "CBase.h"

BEGIN(Client)
class CEffectParticleGroup;

class CMonsterEPController : public CBase
{
public:
	typedef struct tagMonsterEPControlDesc
	{
		class CMonster* _pMonster;
	}MONEPCONDESC;
private:
	explicit CMonsterEPController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMonsterEPController() = default;
public:
	HRESULT NativeConstruct(void* _pArg);
	void Update(const _double& _dDeltaSeconds, class CMonster* _pMonster);

	void Show_Blood(_fvector vPos, _fvector vDir);

	void Show_DeadParticle(_fvector _vCenterPos);
	void Stop_DeadParticle_Show();
protected:
	vector< CEffectParticleGroup*>	m_Blood1List;
	vector<CEffectParticleGroup*>		m_Blood2List;
	vector< CEffectParticleGroup*>	m_Blood3List;
	vector<CEffectParticleGroup*>		m_LittleDust;

	_uint m_iBlood1Index = 0;
	_uint m_iBlood2Index = 0;
	_uint m_iBlood3Index = 0;

	CEffectParticleGroup* m_pDeadOneParticle = nullptr;
protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
public:
	static CMonsterEPController* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	virtual void Free() override;
};

END