#include "CMonster.h"
#include "CControlHead.h"
#include "CPosin.h"
#include "CGameInstance.h"
#include "CMethod.h"
#include "CTransform.h"
#include "COBBCollider.h"
#include "CShooter.h"
#include "CBullet.h"
#include "CCube.h"
#include "CFloor.h"
#include "CAABBCollider.h"
#include "CCristal.h"
#include "CGameScene.h"
#include "CBlood.h"


shared_ptr<CActor> CMonster::Clone(const VOIDDATAS& _tDatas)
{
	return nullptr;
}

void CMonster::Free()
{
	__super::Free();
}

HRESULT CMonster::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CMonster::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	Set_Active(false);
	return S_OK;
}

HRESULT CMonster::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMonster::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	// Move
	ControlMonster(_dTimeDelta);

	return S_OK;
}

HRESULT CMonster::Late_Tick(const _double& _dTimeDelta)
{
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CMonster::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}

void CMonster::ControlMonster(const _double& _dTimeDelta)
{
}

_bool CMonster::IsCollisionToCristal()
{
	if (false == m_bCollisionCristal) {
		if (true == m_pCristal.lock()->IsDamaged(Get_FirstCheckCollider(), Get_DamgedCollider(), ThisShared<CMonster>())) {
			shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
			pGameInstance->Add_DeleteActor(LAYER_MONSTER, ThisShared<CActor>());
			m_bCollisionCristal = true;
		}
	}
	return m_bCollisionCristal;
}

void CMonster::Damaged(const shared_ptr<CCharacter>& _pChar)
{
	m_iHealth -= (int)(_pChar->Get_Damage());
	if (0 >= m_iHealth) {
		m_pGameScene.lock()->DeleteMonster(ThisShared<CCharacter>());
		shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
		pGameInstance->Add_DeleteActor(LAYER_MONSTER, ThisShared<CActor>());

		CBlood::BLOODDESC tDesc;
		tDesc.iBloodCnt = 100;
		shared_ptr<CActor> pBlood = pGameInstance->Clone_Actor_Add(PROTO_ACTOR_BLOOD, LAYER_BLOOD, VOIDDATAS{ &tDesc });
		if(nullptr != pBlood)
			pBlood->Get_Transform()->Set_Pos(Get_Transform()->Get_Pos());

		Set_Active(false);
	}
}
