#include "CShooterMonster.h"
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
#include "CTank.h"

shared_ptr<CActor> CShooterMonster::Clone(const VOIDDATAS& _tDatas)
{
	return nullptr;
}

void CShooterMonster::Free()
{
	Safe_Free(m_pShooter);
	__super::Free();
}

HRESULT CShooterMonster::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CShooterMonster::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	Set_Active(true);
	return S_OK;
}

HRESULT CShooterMonster::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CShooterMonster::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	// Move
	ControlMonster(_dTimeDelta); 
	for (auto& iter : m_pShooter->Get_BulletList()) {
		if (true == iter->Is_Active()) {
			if (true == iter->IsCheckTargetHit(Get_Target().lock())) {
				m_pShooter->DeleteFireBullet(iter);
				return S_OK;
			}
		}
	}
	return S_OK;
}

HRESULT CShooterMonster::Late_Tick(const _double& _dTimeDelta)
{
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CShooterMonster::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}

void CShooterMonster::CreateMonster(const MONSTERSHOOTERDESC& _tMonsterDesc)
{
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);

	MONSTERSHOOTERDESC tDesc = _tMonsterDesc;
	Set_MoveSpeed(tDesc.fMoveSpeed);
	Set_RotateSpeed(tDesc.fRotateSpeed);
	Set_Health(tDesc.iHp);
	// Need Values
	{
		// Tank Shooter
		{
			CHARDESC tShooterDesc;
			::memcpy(&tShooterDesc, &tDesc, sizeof(CHARDESC));
			CShooter::SHOOTINFO tInfo;
			Set_Damage(_tMonsterDesc.fDamage);
			tInfo.fDamage = _tMonsterDesc.fDamage;
			tInfo.fFireAbleTime = _tMonsterDesc.fFireAbleTime;
			tInfo.fFireSpeed = _tMonsterDesc.fFireSpeed;
			tInfo.fLifeTime = _tMonsterDesc.fLifeTime;
			tInfo.fReloadTime = _tMonsterDesc.fReloadTime;
			tInfo.iBulletNum = _tMonsterDesc.iBulletNum;
			tInfo.vBulletColor = _tMonsterDesc.vBulletColor;
			tInfo.vBulletScale = _tMonsterDesc.vBulletScale;
			m_pShooter = static_pointer_cast<CShooter>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_SHOOTER,
				LAYER_OBJECT, VOIDDATAS{ &tShooterDesc, &tInfo }));
			// Tank Posin 
			m_pShooter->Get_Transform()->Set_ParentsTransform(Get_Transform());
		//	m_pShooter->Get_Transform()->Set_Pos(tDesc.vFirePos.Get_Vector());
		//	m_pShooter->Get_Transform()->Set_NotApplyScale(true);
		}
	}
	// MakeBullet
	m_pShooter->MakeBullet(BUL_NORMAL);
	//m_pShooter->Get_Transform()->Set_Scale(Get_Transform()->Get_Scale().Get_Vector() * 0.5f);
	UpdateColliderScale();
}

void CShooterMonster::ControlMonster(const _double& _dTimeDelta)
{
	if (true == IsCollisionToCristal()) {
		shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
		pGameInstance->Add_DeleteActor(LAYER_OBJECT, m_pShooter);
		m_pShooter->Clear();
	}
}
