#include "CTank.h"
#include "CControlHead.h"
#include "CPosin.h"
#include "CGameInstance.h"
#include "CMethod.h"
#include "CTransform.h"
#include "CMainCamera.h"
#include "COBBCollider.h"
#include "CEmptyObject.h"
#include "CShooter.h"
#include "CBullet.h"
#include "CCube.h"
#include "CFloor.h"
#include "CAABBCollider.h"
#include "CCubeBuffer.h"
#include "CRenderer.h"
#include "CGameScene.h"
#include "CCubeMonster.h"

shared_ptr<CActor> CTank::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CTank> pActor = CSmartPointerFactory<CTank>::CreateShared(*this);
	if (FAILED(pActor->NativeConstruct(_tDatas))) {
		Safe_Free(pActor);
		DEBUG_MSG("CTank::Clone To Failed");
	}
	return pActor;
}

void CTank::Free()
{
	Safe_Free(m_pCubeBuffer);
	Safe_Free(m_pMainCamera);
	Safe_Free(m_pTankHead);
	Safe_Free(m_pTankPosin);
	Safe_Free(m_pTankShooter);
	Safe_Free(m_pTankPosin);
	__super::Free();
}

HRESULT CTank::NativeConstruct()
{
	
	return S_OK;
}

HRESULT CTank::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	// Make CubeBuffer 
	{
		CHARDESC tDesc;
		if (_tDatas.size() > 0) {
			tDesc = CMethod::ConvertTemplate_Index<CHARDESC>(&_tDatas, 0);
		}

		if (nullptr != m_pCubeBuffer)
			m_pCubeBuffer->Set_Color(tDesc.vColor);

		shared_ptr<CTransform> pTransform = Get_Transform();
		if (nullptr != Get_Transform()) {
			pTransform->Set_Scale(tDesc.vBodyScale);
		}
	}

	UpdateColliderScale();

	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);

	TANKDESC tDesc;
	if (_tDatas.size() > 0) {
		tDesc = CMethod::ConvertTemplate_Index<TANKDESC>(&_tDatas, 1);
		m_fTankMoveSpeed = tDesc.fTankMoveSpeed;
		m_fTankRotateSpeed = tDesc.fTankRotateSpeed;
	}
	{
		CCube::CUBEDESC tDesc;
		tDesc.vColor = tDesc.vColor;
		m_pFowardShow = static_pointer_cast<CCube>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_CUBE,
			LAYER_OBJECT, VOIDDATAS{&tDesc }));
		m_pFowardShow->Get_Transform()->Set_ParentsTransform(Get_Transform());
		m_pFowardShow->Get_Transform()->Set_Pos(CMethod::SetVector(0.f, 0.f, Get_Transform()->Get_Scale().z / 3.f));
		m_pFowardShow->Get_Transform()->Set_NotApplyScale(true);
		m_pFowardShow->Get_Transform()->Set_Scale(_float3(0.1f, 0.1f, 0.1f));
		m_pFowardShow->Set_Active(true);
	}
	{
		// 구조체 주소 값 만큼 복사
		CPosin::CUBEPAWNDESC tTankPosinDesc;
		::memcpy(&tTankPosinDesc, &tDesc, sizeof(CPosin::CUBEPAWNDESC));
		// Head
		{
			m_pTankHead = static_pointer_cast<CControlHead>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_CONTROLHEAD,
				LAYER_OBJECT, VOIDDATAS{ &tTankPosinDesc }));
			RETURN_CHECK(nullptr == m_pTankHead, E_FAIL);
			m_pTankHead->Set_PosinRotateSpeed(tDesc.fPosinRotateSpeed);
	//		m_pTankHead->Get_Transform()->Set_ParentsTransform(Get_Transform());

		}
		CHARDESC tPosinDesc = CMethod::ConvertTemplate_Index<CHARDESC>(&_tDatas, 2);
		// Posin 
		{
			m_pTankPosin = static_pointer_cast<CPosin>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_POSIN,
				LAYER_OBJECT, VOIDDATAS{ &tPosinDesc }));
			RETURN_CHECK(nullptr == m_pTankPosin, E_FAIL);
			// Tank Posin 
			m_pTankPosin->Get_Transform()->Set_ParentsTransform(m_pTankHead->Get_Transform());
			m_pTankPosin->Get_Transform()->Set_Pos(CMethod::SetVector(0.f, 0.f, 0.5f));
		}
		// Tank Shooter
		{
			CHARDESC tShooterDesc;
			if (_tDatas.size() >=  3) {
				tShooterDesc = CMethod::ConvertTemplate_Index<CHARDESC>(&_tDatas, 3);
			}
			CShooter::SHOOTINFO tInfo;
			if (_tDatas.size() >= 4) {
				tInfo = CMethod::ConvertTemplate_Index<CShooter::SHOOTINFO>(&_tDatas, 4);
			}
			m_pTankShooter = static_pointer_cast<CShooter>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_SHOOTER,
				LAYER_OBJECT, VOIDDATAS{&tShooterDesc, &tInfo }));
			RETURN_CHECK(nullptr == m_pTankPosin, E_FAIL);
			// Tank Posin 
			m_pTankShooter->Get_Transform()->Set_ParentsTransform(m_pTankPosin->Get_Transform());
			m_pTankShooter->Get_Transform()->Set_Pos(CMethod::SetVector(0.f, 0.f, tPosinDesc.vBodyScale.z / 3.3f));
			m_pTankShooter->Get_Transform()->Set_NotApplyScale(true);
		}
		// Empty Object
		{
			m_pCamerFollowObject = static_pointer_cast<CEmptyObject>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_EMPTY,
				LAYER_EMPTYOBEJCT));
			RETURN_CHECK(nullptr == m_pCamerFollowObject, E_FAIL);
			m_pCamerFollowObject->Get_Transform()->Set_ParentsTransform(m_pTankHead->Get_Transform());
			m_pCamerFollowObject->Get_Transform()->Set_Pos( XMVectorSet(0.f, 2.f, -5.f, 0.f));
		}
	}
	// MakeBullet
	m_pTankShooter->MakeBullet(BUL_NORMAL);

	Set_Active(true);
	return S_OK;
}

HRESULT CTank::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CCharacter>();
	// Create Bufafer
	{
		m_pCubeBuffer = CloneCompData<CCubeBuffer>(PROTO_COMP_CUBEBUFFER, VOIDDATAS{ &tDesc });
		Add_Component(COM_BUFFER, m_pCubeBuffer);
	}
	return S_OK;
}

HRESULT CTank::Tick(const _double& _dTimeDelta){

	if(FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	// Move
	ControlTank(_dTimeDelta);
	return S_OK;
}

HRESULT CTank::Late_Tick(const _double& _dTimeDelta){
	
	__super::Late_Tick(_dTimeDelta);

	ControlCamera(_dTimeDelta);
	Get_Renderer()->Add_Render(RENDERID::RENDER_ACTOR, ThisShared<CActor>());

	list<shared_ptr<CMonster>> pMonsterList = m_pGameScene.lock()->GetMonsterActiveList();
	for (auto& Monster : pMonsterList) {
		for (auto& iter : m_pTankShooter->Get_BulletList()) {
			if (true == iter->Is_Active()) {
				if (true == iter->IsCheckTargetHit(Monster)) {
					m_pTankShooter->DeleteFireBullet(iter);
					return S_OK;
				}
			}
		}
	}

	return S_OK;
}

HRESULT CTank::Render(const HDC& _hDC){
	
	if (FAILED(__super::Render(_hDC)))
		return E_FAIL;

	m_pCubeBuffer->Render(_hDC);
	return S_OK;
}

void CTank::ControlTank(const _double& _dTimeDelta) {

	if (true == m_bShockState)
	{
		m_dShockTime += _dTimeDelta;
		if (m_dShockTime >= SHOCK_TIME)
			m_bShockState = false;
		return;
	}

	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);

	if (false == Get_Floor().lock()->Get_RegionCollider()->IsCollider(Get_FirstCheckCollider())) {
		Get_Transform()->Set_Pos(m_vPrevPos.Get_Vector());
	}

	_float3 vPos = Get_Transform()->Get_Pos();
	vPos.y = Get_Floor().lock()->Get_Transform()->Get_PosF3().y;
	Get_Transform()->Set_Pos(vPos.Get_Vector());

	if (true == pGameInstance->Is_Pressing(VK_UP)) {
		Get_Transform()->Move_Forward(_dTimeDelta, m_fTankMoveSpeed);
	}
	else if (true == pGameInstance->Is_Pressing(VK_DOWN)) {
		Get_Transform()->Move_Back(_dTimeDelta, m_fTankMoveSpeed);
	}
	if (true == pGameInstance->Is_Pressing(VK_RIGHT)) {
		Get_Transform()->Rotate_Turn(UP_VECTOR, _dTimeDelta, -m_fTankRotateSpeed);
	}
	else if (true == pGameInstance->Is_Pressing(VK_LEFT)) {
		Get_Transform()->Rotate_Turn(UP_VECTOR, _dTimeDelta, m_fTankRotateSpeed);
	}

	if (true == pGameInstance->Is_Pressing(VK_SPACE)) {
		m_pTankShooter->Set_ShootDir(m_pTankShooter->Get_Transform()->Get_OriginLook());
		m_pTankShooter->Fire(ThisShared<CCharacter>(), Get_Floor().lock(), m_pTankPosin->Get_ShootAngle());
	}

	m_pTankHead->Get_Transform()->Set_Pos(Get_Transform()->Get_Pos() + CMethod::SetVector(0.f, 0.5f, 0.f));
	// Prev Pos
	m_vPrevPos = Get_Transform()->Get_Pos();
}

void CTank::ControlCamera(const _double& _dTimeDelta){

	RETURN_CHECK(nullptr == m_pMainCamera, ;);
	m_pMainCamera->Get_Transform()->LookAt(m_pTankShooter->Get_Transform()->Get_OriginPos());
}

void CTank::Damaged(const shared_ptr<CCharacter>& _pChar)
{
	if (m_iShockCountDown == 0) {
		m_iShockCountDown = SHOCK_CNT;
		m_bShockState = true;
	}

	--m_iShockCountDown;
}

void CTank::Set_MainCamera(const shared_ptr<CMainCamera>& _pCamera){
	this->m_pMainCamera = _pCamera;
	m_pMainCamera->Get_Transform()->Set_Pos(m_pCamerFollowObject->Get_Transform()->Get_OriginPos());
	m_pMainCamera->Set_MoveTR(m_pCamerFollowObject->Get_Transform());
}
