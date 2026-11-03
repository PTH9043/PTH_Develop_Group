#include "CBullet.h"
#include "COBBCollider.h"
#include "CGameInstance.h"
#include "CTransform.h"
#include "CMethod.h"
#include "CShooter.h"
#include "CFloor.h"
#include "CGameScene.h"
#include "CAABBCollider.h"
#include "CCharacter.h"

vector<shared_ptr<CBullet>> CBullet::CreateBulletVector(const BULLETTYPE _eBulletType, 
	BULLETDESC& _tBulletDesc, const _uint _iBulletCnt,
	 const _float3& _vBodyScale, const _float3& _vColor)
{
	vector<shared_ptr<CBullet>> pBullet;
	pBullet.reserve(_iBulletCnt);
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	switch (_eBulletType) {
	case BULLETTYPE::BUL_NORMAL:
		for (_uint i = 0; i < _iBulletCnt; ++i) {
			CUBEPAWNDESC tCubePawnDesc;
			tCubePawnDesc.vBodyScale = _vBodyScale;
			tCubePawnDesc.vColor = _vColor;
			pBullet.push_back(static_pointer_cast<CBullet>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_NORMALBULLET,
				LAYER_BULLET, VOIDDATAS{ &tCubePawnDesc, &_tBulletDesc })));
		}
		break;
	}

	return pBullet;
}

shared_ptr<CActor> CBullet::Clone(const VOIDDATAS& _tDatas)
{
	return shared_ptr<CActor>();
}

void CBullet::Free()
{
	Safe_Free(m_pHitCollider);
	Safe_Free(m_pFirstCheckCollider);
	__super::Free();
}

HRESULT CBullet::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	return S_OK;
}

HRESULT CBullet::NativeConstruct(VOIDDATAS _tDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(_tDatas), E_FAIL);

	BULLETDESC tDesc = CMethod::ConvertTemplate_Index<BULLETDESC>(&_tDatas, 1);

	Set_Active(false);

	m_pHitCollider->Set_Scale(Get_Transform()->Get_Scale());
	_float3 vScale = Get_Transform()->Get_Scale();
	_float3 vSaveScale = vScale;
	vScale *= 1.6f;
	vScale.y = vSaveScale.y * 1.2f;
	m_pFirstCheckCollider->Set_Scale(vScale);
	return S_OK;
}

HRESULT CBullet::CreateComponent()
{
	RETURN_CHECK_FAILED(__super::CreateComponent(), E_FAIL);

	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CCubePawn>();
	{
		CCollider::COLLDESC tCollDesc;
		tCollDesc.vPosition = Get_Transform()->Get_Pos();
		tCollDesc.vScale = Get_Transform()->Get_Scale();
		m_pHitCollider = CloneCompData< COBBCollider>(PROTO_COMP_OBBCOLLIDER, VOIDDATAS{ &tDesc, &tCollDesc });
		Add_Component(COM_COLLIDER, m_pHitCollider);
	}
	{
		CCollider::COLLDESC tCollDesc;
		tCollDesc.vPosition = Get_Transform()->Get_Pos();
		tCollDesc.vScale = Get_Transform()->Get_Scale();
		m_pFirstCheckCollider = CloneCompData< CAABBCollider>(PROTO_COMP_ABBCOLLIDER, VOIDDATAS{ &tDesc, &tCollDesc });
		Add_Component(COM_FIRSTCHECKCOLL, m_pFirstCheckCollider);
	}
	return S_OK;
}

HRESULT CBullet::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	MoveBullet(_dTimeDelta);
	return S_OK;
}

HRESULT CBullet::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta)))
		return E_FAIL;

#ifdef _DEBUG
	m_pHitCollider->Add_RendererDebugActor(Get_Renderer());
	m_pFirstCheckCollider->Add_RendererDebugActor(Get_Renderer());
#endif
	return S_OK;
}

HRESULT CBullet::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}

_float CBullet::GiveDamage()
{
	Set_Active(false);
	return Get_Damage();
}

void CBullet::UpdateFireDir(const shared_ptr<CCharacter>& _pChar, const shared_ptr<CFloor>& _pFloor,
	const _fvector& _vFirePos, const _fvector& _vFireDir, const _float _fLifeTime, const _float _fDamge,
	const _float _fSpeed, const _float& _fShootAngle)
{
	if (false == Is_Active()) {
		Set_Active(true);
		m_vFireDir = _vFireDir;
		Get_Transform()->Set_Pos(_vFirePos);

		m_BulletDesc.fDamage = _fDamge;
		m_BulletDesc.fLifeTime = _fLifeTime;
		m_BulletDesc.fSpeed = _fSpeed;
		m_fShootAngle = _fShootAngle;
		m_pOwner = _pChar;
		m_pFloor = _pFloor;
	}
}

_bool CBullet::IsCheckTargetHit(const shared_ptr<CCharacter>& _pChar)
{
	if (true == _pChar->IsDamaged(m_pFirstCheckCollider, m_pHitCollider, m_pOwner.lock())) {
		Set_Active(false);
		return true;
	}
	return false;
}

void CBullet::MoveBullet(const _double& _dTimeDelta)
{
	if (m_BulletDesc.fLifeTime >= m_fAccumulLifeTime) {
		Get_Transform()->Translate_Dir(m_vFireDir.Get_Vector(), _dTimeDelta, m_BulletDesc.fSpeed);
		m_fAccumulLifeTime += (_float)(_dTimeDelta);
	}
	else {
		Reset();
	}
}

void CBullet::Reset()
{
	Set_Active(false);
	m_fAccumulLifeTime = 0.f;
	m_pShooter.lock()->DeleteFireBullet(ThisShared<CBullet>());
}
