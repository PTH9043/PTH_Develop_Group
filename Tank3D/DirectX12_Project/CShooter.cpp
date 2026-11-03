#include "CShooter.h"
#include "CGameInstance.h"
#include "CTransform.h"
#include "COBBCollider.h"
#include "CBullet.h"
#include "CNormalBullet.h"
#include "CMethod.h"
#include "CFloor.h"
#include "CCharacter.h"

shared_ptr<CActor> CShooter::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CShooter> pTankShooter = CSmartPointerFactory<CShooter>::CreateShared(*this);
	if (FAILED(pTankShooter->NativeConstruct(_tDatas))) {
		Safe_Free(pTankShooter);
		DEBUG_MSG("CTankShooter::Clone To Failed");
	}
	return pTankShooter;
}

void CShooter::Free()
{
	for (auto& iter : m_BulletSaver) {
		for (auto& value : iter.second) {
			shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
			pGameInstance->Add_DeleteActor(LAYER_OBJECT, value);
		}
	}
	m_BulletSaver.clear();
	m_FireBulletList.clear();
	__super::Free();
}

HRESULT CShooter::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	return S_OK;
}

HRESULT CShooter::NativeConstruct(VOIDDATAS _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(_tDatas), E_FAIL);

	m_tShooterInfo = CMethod::ConvertTemplate_Index<SHOOTINFO>(&_tDatas, 1);
	return S_OK;
}

HRESULT CShooter::CreateComponent()
{
	RETURN_CHECK_FAILED(__super::CreateComponent(), E_FAIL);
	return S_OK;
}

HRESULT CShooter::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	UpdateTimes(_dTimeDelta);
	return S_OK;
}

HRESULT CShooter::Late_Tick(const _double& _dTimeDelta)
{
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CShooter::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}


void CShooter::MakeBullet(const BULLETTYPE _eBulletType)
{
	CBullet::BULLETDESC tBulletDesc;
	tBulletDesc.fDamage = m_tShooterInfo.fDamage;
	tBulletDesc.fLifeTime = m_tShooterInfo.fLifeTime;
	tBulletDesc.fSpeed = m_tShooterInfo.fFireSpeed;
	BULLETLIST BulletList = CBullet::CreateBulletVector(_eBulletType, tBulletDesc, m_tShooterInfo.iBulletNum,
		m_tShooterInfo.vBulletScale, m_tShooterInfo.vBulletColor);
	BULLETSAVER::iterator FindIter = m_BulletSaver.find(m_eCurBulletType);
	if (FindIter == m_BulletSaver.end())
	{
		for (auto& iter : BulletList) {
			iter->Set_Shooter(ThisShared<CShooter>());
		}
		m_BulletSaver.emplace(_eBulletType, BulletList);
		for (auto& iter : BulletList) {
			m_FireBulletList.push_back(iter);
		}
	}
	else {

		shared_ptr<CGameInstance> pGameInstnace = GET_INSTNACE(CGameInstance);
		for (auto& iter : (*FindIter).second) {
			pGameInstnace->Add_DeleteActor(LAYER_BULLET, (iter));
		}
		m_BulletSaver.erase(FindIter);
		m_BulletSaver.emplace(_eBulletType, BulletList);
	}
}

void CShooter::Fire(const shared_ptr<CCharacter>& _pChar, const shared_ptr<CFloor>& _pFloor, const _float& _fShootAngle)
{
	if (false == m_bReloadTime) {
		BULLETSAVER::iterator FindIter = m_BulletSaver.find(m_eCurBulletType);
		if (FindIter == m_BulletSaver.end())
			return;

		if ((*FindIter).second.size() <= m_iBulletIndex) {
			m_bReloadTime = true;
			return;
		}

		if (m_bWaitTime == false)
		{
			(*FindIter).second[m_iBulletIndex++]->UpdateFireDir(_pChar, _pFloor, Get_Transform()->Get_OriginPos(), m_vShootDir.Get_Vector(),
				m_tShooterInfo.fLifeTime, m_tShooterInfo.fDamage, m_tShooterInfo.fFireSpeed, _fShootAngle);
			m_bWaitTime = true;
			m_dBulletFireSaveTime = 0.;
		}
	}
}

void CShooter::DeleteFireBullet(const shared_ptr<CBullet>& _pBullet)
{
}

void CShooter::UpdateTimes(const _double& _dTimeDelta)
{
	if (true == m_bWaitTime) {
 		m_dBulletFireSaveTime += (_dTimeDelta);
		if (m_tShooterInfo.fFireAbleTime <= m_dBulletFireSaveTime) {
			m_bWaitTime = false;
		}
	}
	if (true == m_bReloadTime) {
		m_dReloadSaveTime += _dTimeDelta;
		if (m_dReloadSaveTime <= m_tShooterInfo.fReloadTime) {
			m_bReloadTime = false;
			m_dReloadSaveTime = 0;
			m_iBulletIndex = 0;
		}
	}
}
