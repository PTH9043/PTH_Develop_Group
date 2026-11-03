#include "CGameScene.h"
#include "CGameInstance.h"
#include "CMainCamera.h"
#include "CCube.h"
#include "CTank.h"
#include "CFloor.h"
#include "CCubeMonster.h"
#include "CTransform.h"
#include "CShooter.h"
#include "CCristal.h"
#include "CShooterMonster.h"
#include "CButton.h"
#include "CTextButton.h"
#include "CMethod.h"
#include "CLogoScene.h"

void CGameScene::Free()
{
}

HRESULT CGameScene::NativeConstruct()
{
	return S_OK;
}

HRESULT CGameScene::LoadSceneData()
{
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	// Main Camera 
	{
		CCamera::CAMDESC tDesc;
		tDesc.fForv = XMConvertToRadians(60);
		tDesc.fNear = 0.2f;
		tDesc.fFar = 1000.f;
		tDesc.fWinHeight = WINDOW_HEIGHT;
		tDesc.fWinWidth = WINDOW_WIDTH;
		tDesc.vLook = _float3(0.f, 0.f, 0.01f);
		tDesc.vPos = _float3(0.f, 0.f, -2.f);
		tDesc.fCamSpeed = 0.5f;

		m_pCamera = static_pointer_cast<CMainCamera>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_MAINCAMERA, 
			LAYER_CAM, VOIDDATAS{ &tDesc }));
		pGameInstance->Add_Camera(MAIN_CAMERA, m_pCamera);
		pGameInstance->ChangeCamera(MAIN_CAMERA);
	}
	// Floor
	{
		CFloor::FLOORDESC tFloorDesc;
		tFloorDesc.vBodyScale = _float3(1.f, 1.f, 1.f);
		tFloorDesc.vColor = _float3(255.f, 100.f, 100.f);
		tFloorDesc.fWidth = 50;
		tFloorDesc.iSubjects = 50;
		tFloorDesc.fDepth = 100;
		m_pFloor = static_pointer_cast<CFloor>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_FLOOR,
			LAYER_FLOOR, VOIDDATAS{ &tFloorDesc }));

		m_pFloor->Get_Transform()->Set_Pos(_float3(0.f, 0.f, 50.f).Get_Vector());

		_float3 vStartPos = _float3(0.f, 0.f, 60.f);
		m_SpawnPosList.reserve(SPAWN_CNT);
		_float fVaueX = (50.f) / SPAWN_CNT;
		_float fVaueZ = (10) / SPAWN_CNT;
		for (_uint i = 0; i < SPAWN_CNT; ++i) {
			m_SpawnPosList.push_back(vStartPos);
			vStartPos.x += fVaueX;
			if (i != 0 && (i % 10 == 0)) {
				vStartPos.z += fVaueZ;
			}
		}
	}
	// Tank 
	{
		CTank::CHARDESC tBodyDesc;
		tBodyDesc.vColor = _float3(0.f, 0.f, 255.f);
		tBodyDesc.vBodyScale = _float3(1.f, 0.5f, 1.5f);

		CTank::TANKDESC tTankDesc;
		tTankDesc.vColor = _float3(0.f, 0.f, 255.f);
		tTankDesc.vBodyScale = _float3(0.5f, 0.5f, 0.5f);
		tTankDesc.fTankMoveSpeed = 2.f;
		tTankDesc.fPosinRotateSpeed = XMConvertToRadians(45.f);
		tTankDesc.fTankRotateSpeed = XMConvertToRadians(45.f);

		CTank::CHARDESC tPosinDesc;
		tPosinDesc.vColor = _float3(0.f, 255.f, 0.f);
		tPosinDesc.vBodyScale = _float3(0.5f, 0.5f, 3.f);

		CTank::CHARDESC tShooterDesc;
		tShooterDesc.vColor = _float3(255.f, 50.f, 255.f);
		tShooterDesc.vBodyScale = _float3(0.25f, 0.25f, 0.25f);

		CShooter::SHOOTINFO tShootInfo;
		tShootInfo.fDamage = 1.f;
		tShootInfo.fFireAbleTime = 2.f;
		tShootInfo.fFireSpeed = 10.f;
		tShootInfo.fLifeTime = 20.f;
		tShootInfo.fReloadTime = 3.f;
		tShootInfo.iBulletNum = 100;
		tShootInfo.vBulletColor = _float3(100.f, 70.f, 20.f);
		tShootInfo.vBulletScale = _float3(0.25f, 0.25f, 0.25f);
		m_pTank = static_pointer_cast<CTank>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_TANK,
			LAYER_PLAYER, VOIDDATAS{ &tBodyDesc, &tTankDesc, &tPosinDesc, &tShooterDesc, &tShootInfo }));

		m_pTank->Set_Floor(m_pFloor);
		m_pTank->Set_MainCamera(m_pCamera);
		m_pTank->Set_GameScene(ThisShared<CGameScene>());
	}
	// Cristal
	{		
		CTank::CHARDESC tBodyDesc;
		tBodyDesc.vColor = _float3(255.f, 0.f, 0.f);
		tBodyDesc.vBodyScale = _float3(3.f, 3.f, 1.f);
		m_pCristal = static_pointer_cast<CCristal>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_CRISTAL,
			LAYER_OBJECT, VOIDDATAS{&tBodyDesc}));
		m_pCristal->Get_Transform()->Set_Pos(_float3(0.f, 1.f, 0.f).Get_Vector());
		m_pCristal->SetGameScene(ThisShared<CGameScene>());
	}
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(_float(WINDOW_WIDTH) - 200.f, (_float)(20.f));
		tRectDesc.v2Size = _float2(20.f, 10.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 0.f, 0.f, 0.f };

		m_pRemainLifeText = static_pointer_cast<CText>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXT,
			LAYER_UI, VOIDDATAS{ &tRectDesc , &tTextDesc }));
		m_pRemainLifeText->SetText(L"지정된 라이프 : " + ::to_wstring(m_pCristal->Get_Health()));
	}
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(20.f, (_float)(20.f));
		tRectDesc.v2Size = _float2(20.f, 10.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 0.f, 0.f, 0.f };

		m_pShockText = static_pointer_cast<CText>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXT,
			LAYER_UI, VOIDDATAS{ &tRectDesc , &tTextDesc }));
		m_pShockText->SetText(L"쇼킹 : " + ::to_wstring(m_pTank->Get_ShockCountDown()));
	}
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(_float(WINDOW_HALF_WIDTH) - 90.f, (_float)(WINDOW_HALF_HEIGHT) - 100.f);
		tRectDesc.v2Size = _float2(20.f, 10.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 0.f, 0.f, 0.f };

		m_pShockingText = static_pointer_cast<CText>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXT,
			LAYER_UI, VOIDDATAS{ &tRectDesc , &tTextDesc }));
		m_pShockingText->SetText(L"쇼킹 중이야!!");
		m_pShockingText->Set_Active(false);
	}
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(_float(WINDOW_HALF_WIDTH) - 90.f, (_float)(WINDOW_HALF_HEIGHT)-100.f);
		tRectDesc.v2Size = _float2(20.f, 10.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 0.f, 0.f, 0.f };

		m_pNextRoundText = static_pointer_cast<CText>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXT,
			LAYER_UI, VOIDDATAS{ &tRectDesc , &tTextDesc }));
		m_pNextRoundText->SetText(L"다음 라운드 : ");
		m_pNextRoundText->Set_Active(false);
	}
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(20.f, (_float)(40.f));
		tRectDesc.v2Size = _float2(20.f, 10.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 100.f, 0.f, 255.f };

		m_pDamgeShowText = static_pointer_cast<CText>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXT,
			LAYER_UI, VOIDDATAS{ &tRectDesc , &tTextDesc }));
		m_pDamgeShowText->SetText(L"데미지 계수 : ");
	}
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(20.f, (_float)(60.f));
		tRectDesc.v2Size = _float2(20.f, 10.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 100.f, 0.f, 255.f };

		m_pShootSpeedShowText = static_pointer_cast<CText>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXT,
			LAYER_UI, VOIDDATAS{ &tRectDesc , &tTextDesc }));
		m_pShootSpeedShowText->SetText(L"발사 속도 : ");
	}
	m_iRoundValue = 0;
	return S_OK;
}

void CGameScene::Tick(const _double& _dTimeDelta){

	if (0 >= m_pCristal->Get_Health()) {
		shared_ptr< CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
		pGameInstance->RegisterScene(CreateNew<CLogoScene>());
		return;
	}

	CheckDiedMonster();
}

void CGameScene::Late_Tick(const _double& _dTimeDelta){

	if (false == m_bNextRound) {
		m_pRemainLifeText->SetText(L"지정된 라이프 : " + ::to_wstring(m_pCristal->Get_Health()));
		m_pShockText->SetText(L"쇼킹 : " + ::to_wstring(m_pTank->Get_ShockCountDown()));
		m_pDamgeShowText->SetText(L"데미지 계수 : " + to_wstring(m_pTank->Get_Damage()));
		m_pShootSpeedShowText->SetText(L"발사 속도 : " + to_wstring(m_pTank->Get_Shooter()->Get_ShootInfo().fFireAbleTime));
		m_pShockingText->Set_Active(m_pTank->Is_ShockingState());
		m_pNextRoundText->Set_Active(false);
	}
	else {
		m_pNextRoundText->SetText(L"다음 라운드 : " + to_wstring((_float)(NEXT_ROUND_TIME -m_dNextRoundTime)));
		m_dNextRoundTime += _dTimeDelta;
		if (m_dNextRoundTime >= NEXT_ROUND_TIME) {
			MonsterRadomsSpawn();
			m_dNextRoundTime = 0.;
		}
		m_pNextRoundText->Set_Active(true);
	}
}

void CGameScene::DeleteMonster(const shared_ptr<CCharacter>& _pCharcter)
{
	for (MONACTIVE::iterator iter = m_MonActiveList.begin(); iter != m_MonActiveList.end();) {
		if ((*iter) == _pCharcter)
		{
			m_MonActiveList.erase(iter);
			break;
		}
		else
			++iter;
	}
}

void CGameScene::MonsterRadomsSpawn()
{
	if (true == m_bNextRound) {
		m_pTank->Set_Damage((_float)(m_iRoundValue - 1) * 0.3f + 1.f);
		m_pTank->Get_Shooter()->Get_ShootInfo().fFireAbleTime -= ((m_iRoundValue - 1) * 0.01f);
		if (1 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 3, 0);
		}
		else if (2 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 6, 0);
		}
		else if (3 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 10, 0);
		}
		else if (4 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 3, 0);
			Add_MonList(MONSHOOTER, 1, 1);
		}
		else if (5 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 3, 0);
			Add_MonList(MONSHOOTER, 6, 1);
		}
		else if (6 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 3, 0);
			Add_MonList(MONSHOOTER, 3, 2);
		}
		else if (7 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 3, 0);
			Add_MonList(MONSHOOTER, 3, 1);
			Add_MonList(MONSHOOTER, 3, 2);
		}
		else if (8 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 10, 0);
			Add_MonList(MONSHOOTER, 3, 1);
			Add_MonList(MONSHOOTER, 3, 2);
			Add_MonList(MONSHOOTER, 2, 4);
		}
		else if (9 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 3, 0);
			Add_MonList(MONSHOOTER, 3, 1);
			Add_MonList(MONSHOOTER, 3, 2);
			Add_MonList(MONSHOOTER, 1, 3);
			Add_MonList(MONSHOOTER, 2, 4);
		}
		else if (10 == m_iRoundValue) {
			m_pTank->Reset();
			Add_MonList(MONSHOOTER, 2, 4);
			Add_MonList(MONSHOOTER, 3, 3);
		}


		m_bNextRound = false;
	}
}

void CGameScene::Add_MonList(const MONTYPE _eType, const _uint _iMonsterNum, const _uint& _iMobLevel)
{
	shared_ptr< CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);

	CMonster::CHARDESC tBodyDesc;
	switch (_eType) {
	case MONSHOOTER:
		CShooterMonster::MONSTERSHOOTERDESC tMonsterDesc;
		tBodyDesc.vColor = _float3(70.f, 100.f, 180.f);
		tBodyDesc.vBodyScale = _float3(0.7f, 1.f, 0.7f);
		{
			tMonsterDesc.fMoveSpeed = (rand() % 10) * 0.001f + 0.75f;
			tMonsterDesc.fPosinRotateSpeed = 2.f;
			tMonsterDesc.fRotateSpeed = 1.f;
			tMonsterDesc.vBodyScale = _float3(0.5f, 0.5f, 0.5f);
			tMonsterDesc.vColor = _float3(70.f, 100.f, 180.f);
			tMonsterDesc.vFirePos = _float3(0.f, 0.5f, 1.f);
			tMonsterDesc.fDamage = 1.f;
			tMonsterDesc.fFireAbleTime = (rand() % 40) * 0.01f + 3.f;
			tMonsterDesc.fFireSpeed = (rand() % 40) * 0.01f + 2.f;
			tMonsterDesc.fLifeTime = 20.f;
			tMonsterDesc.fReloadTime = 5.f;
			tMonsterDesc.iBulletNum = 20;
			tMonsterDesc.vBulletColor = _float3(255.f, 100.f, 70.f);
			tMonsterDesc.vBulletScale = _float3(0.2f, 0.2f, 0.2f);
			tMonsterDesc.iHp = 1;
		}

		switch (_iMobLevel) {
		case 1:
			tMonsterDesc.iHp += 2;
			tBodyDesc.vBodyScale *= 1.5f;
			tMonsterDesc.fDamage += 1.f;
			tBodyDesc.vColor = _float3(70.f, 100.f, 255.f);
			break;
		case 2:
			tMonsterDesc.iHp += 4;
			tBodyDesc.vBodyScale *= 3.f;
			tMonsterDesc.iBulletNum = 10;
			tMonsterDesc.fFireAbleTime -= 0.25f;
			tMonsterDesc.fMoveSpeed += 0.25f;
			tMonsterDesc.fDamage += 2.f;
			tBodyDesc.vColor = _float3(255.f, 100.f, 255.f);
			break;
		case 3:
			tMonsterDesc.iHp +=6;
			tBodyDesc.vBodyScale *= 6.f;
			tMonsterDesc.iBulletNum = 20;
			tMonsterDesc.fFireAbleTime -= 0.5f;
			tMonsterDesc.fMoveSpeed += 1.f;
			tMonsterDesc.fFireSpeed -= 0.5f;
			tMonsterDesc.fDamage += 3.f;
			tBodyDesc.vColor = _float3(0.f, 100.f, 0.f);
			break;
		case 4:
			tMonsterDesc.iHp += 10;
			tBodyDesc.vBodyScale *= 15.f;
			tMonsterDesc.iBulletNum = 20;
			tMonsterDesc.fFireAbleTime -= 1.f;
			tMonsterDesc.fMoveSpeed += 1.f;
			tMonsterDesc.fFireSpeed -= 1.f;
			tMonsterDesc.fDamage += 4.f;
			tBodyDesc.vColor = _float3(0.f, 100.f, 255.f);
			break;
		case 5:
			tMonsterDesc.iHp += 5;
			tBodyDesc.vBodyScale *= 3.f;
			tMonsterDesc.iBulletNum = 10;
			tMonsterDesc.fFireAbleTime -= 0.25f;
			tMonsterDesc.fMoveSpeed += 3.f;
			tMonsterDesc.fDamage += 3.f;
			tBodyDesc.vColor = _float3(255.f, 100.f, 255.f);
			break;
		}

		CreateMonster(VOIDDATAS{ &tBodyDesc, &tMonsterDesc }, _eType, _iMonsterNum, 
			PROTO_ACTOR_CUBEMONSTER);
		break;
	}
}

void CGameScene::CheckDiedMonster()
{
	if (false == m_bNextRound)
	{
		_bool bActive = m_MonActiveList.size() == 0;
		if (true == bActive)
		{
			++m_iRoundValue;
			m_bNextRound = true;
		}

		if (m_iRoundValue >= 11) {
			shared_ptr< CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
			pGameInstance->RegisterScene(CreateNew<CLogoScene>());
		}
	}
}

void CGameScene::CreateMonster(const VOIDDATAS& _tDatas, const MONTYPE _eType, const _uint _iMonsterNum
	, const _wstring& _strProtoType)
{
	shared_ptr< CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);

	CMonster::CHARDESC tBodyDesc;

	for (_uint i = 0; i < _iMonsterNum; ++i) {
		shared_ptr<CMonster> pMonster = static_pointer_cast<CMonster>(pGameInstance->Clone_Actor_Add(_strProtoType,
			LAYER_MONSTER, _tDatas));
		pMonster->Set_Floor(m_pFloor);
		pMonster->Set_Target(m_pTank);
		pMonster->Set_Cristal(m_pCristal);
		pMonster->Set_GameScene(ThisShared<CGameScene>());
		pMonster->Set_Active(true);
		pMonster->Get_Transform()->Set_Pos(m_SpawnPosList[rand() % SPAWN_CNT].Get_Vector());
		m_MonActiveList.push_back(pMonster);
	}
}
