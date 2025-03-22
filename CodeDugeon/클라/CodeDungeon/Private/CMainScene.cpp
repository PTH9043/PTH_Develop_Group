#include "ClientDefines.h"
#include "CMainScene.h"
#include "UGameInstance.h"
#include "CMainCamera.h"
#include "ULight.h"
#include "UFire.h"
#include "UGuard.h"
#include "UTransform.h"
#include "ULight.h"
#include "USound.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "URenderer.h"
#include "CMap.h"
#include "UStageManager.h"
#include "UStage.h"
#include "UTexGroup.h"
#include "UTexture.h"
#include "CWarriorPlayer.h"
#include "CProtoMaker.h"
#include "UGpuCommand.h"
#include "CRooms.h"
#include "UCollider.h"
#include "UMethod.h"
#include "CTorch.h"
#include "CMummy.h"
#include "CSarcophagus.h"
#include "UAnimModel.h"
#include "CIronBars.h"
#include "CModelObjects.h"
#include "CImageUI.h"
#include "CButtonUI.h"
#include "CLoadingUI.h"
#include "UFont.h"
#include "CHpBarUI.h"
#include "CFreeModeCamera.h"

BEGIN(Client)

CMainScene::CMainScene(CSHPTRREF<UDevice> _spDevice) : 
	UScene(_spDevice, SCENE::SCENE_STAGE1),
	m_spMainCamera{ nullptr },
	m_spMap{nullptr},
	m_bIsFoundPlayer_Minotaur{ false },
    m_bisFoundPlayer_Harlequinn{ false },
    m_bisFoundPlayer_Anubis{ false },
    m_iMinotaurCurHP{ 0 },
    m_iHarlequinnCurHP{ 0 },
    m_iAnubisCurHP{ 0 },
	m_bisMobsAllDead_Interior_Hallway_E{false},
	m_bisMobsAllDead_Interior_Room_D{ false },
	m_bisMobsAllDead_Interior_Room_F{ false },
	m_bisMobsAllDead_Interior_Room_G{ false },
	m_spFreeModeCamera{nullptr}
{
}

void CMainScene::UpdateMobsStatus()
{
	_float3 PlayerPos = m_spMainCamera->GetTransform()->GetPos();
	_int deadmobcount_RoomD = 0;
	_int deadmobcount_HallwayE = 0;
	_int deadmobcount_RoomF = 0;
	_int deadmobcount_RoomG = 0;

	for (auto& mobcontainer : (*m_spMap->GetMobs().get()))
	{
		for (auto& mobs : mobcontainer.second)
		{
			if (mobs->GetAnimModel()->GetModelName() == L"minotaur_FBX.bin") {
				m_bIsDead_Minotaur = mobs->GetDeathState();
			}
			if (mobs->GetAnimModel()->GetModelName() == L"Harlequin1_FBX.bin") {
				m_bisDead_Harlequinn = mobs->GetDeathState();
			}
			if (mobs->GetAnimModel()->GetModelName() == L"Anubis_FBX.bin") {
				m_bisDead_Anubis = mobs->GetDeathState();
			}
			if(mobs->GetDeathState() == false)
			{	
				//보스몹 상태 업데이트
				{
					if (mobs->GetAnimModel()->GetModelName() == L"minotaur_FBX.bin")
					{
						m_iMinotaurCurHP = mobs->GetHealth();
						m_iMinotaurMaxHP = mobs->GetMaxHealth();
						
					
						if (!m_spWarriorPlayer->GetDeathState()) {
							m_bIsFoundPlayer_Minotaur = mobs->GetFoundTargetState();							
						}
						else {
							m_bIsFoundPlayer_Minotaur = false;
						}
							
					}
					if (mobs->GetAnimModel()->GetModelName() == L"Harlequin1_FBX.bin")
					{
						m_iHarlequinnCurHP = mobs->GetHealth();
						m_iHarlequinnMaxHP = mobs->GetMaxHealth();
					
						if (!m_spWarriorPlayer->GetDeathState()) {
							m_bisFoundPlayer_Harlequinn = mobs->GetFoundTargetState();							
						}
						else {
							m_bisFoundPlayer_Harlequinn = false;
						}
						
					}
					if (mobs->GetAnimModel()->GetModelName() == L"Anubis_FBX.bin")
					{
						m_iAnubisCurHP = mobs->GetHealth();
						m_iAnubisMaxHP = mobs->GetMaxHealth();
						if (!m_spWarriorPlayer->GetDeathState()) {
							m_bisFoundPlayer_Anubis = mobs->GetFoundTargetState();							
						}
						else {
							m_bisFoundPlayer_Anubis = false;
						}
						
					}
				}			
			}
			else
			{
				if(mobs->GetAnimModel()->GetModelName() != L"Chest 1_FBX.bin")
				{
					//몹들 사망 상태 업데이트
					if (mobcontainer.first == L"Interior_Room_D" && !m_bisMobsAllDead_Interior_Room_D)
						deadmobcount_RoomD++;
					else if (mobcontainer.first == L"Interior_Hallway_E" && !m_bisMobsAllDead_Interior_Hallway_E)
						deadmobcount_HallwayE++;
					else if (mobcontainer.first == L"Interior_Room_F" && !m_bisMobsAllDead_Interior_Room_F)
						deadmobcount_RoomF++;
					else if (mobcontainer.first == L"Interior_Room_G" && !m_bisMobsAllDead_Interior_Room_G)
						deadmobcount_RoomG++;
				}
			}

			//거리별 최적화
			
			{
				_float3 mobPos = mobs->GetTransform()->GetPos();
				_float3 distance = mobPos - PlayerPos;
				_float ydistance = mobPos.y - PlayerPos.y;
				float distanceSq = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;

				if(abs(ydistance) > 50)
				{
					mobs->SetActive(false);
				}
				else
				{
					if (distanceSq <= 200 * 200)
					{
						if (!mobs->GetDeathState())
						{
							mobs->SetActive(true);
						}
					}
					else
					{
						mobs->SetActive(false);
					}
				}		
			}

		}
	}

	//방별 몹 전체 사망 상태 업데이트
	for (auto& mobcontainer : (*m_spMap->GetMobs().get()))
	{
		if (mobcontainer.first == L"Interior_Room_D")
		{
			if (mobcontainer.second.size() == deadmobcount_RoomD && !m_bisMobsAllDead_Interior_Room_D)
				m_bisMobsAllDead_Interior_Room_D = true;
		}
		else if (mobcontainer.first == L"Interior_Hallway_E")
		{
			if (mobcontainer.second.size() == deadmobcount_HallwayE && !m_bisMobsAllDead_Interior_Hallway_E)
				m_bisMobsAllDead_Interior_Hallway_E = true;
		}
		else if (mobcontainer.first == L"Interior_Room_F")
		{
			if (mobcontainer.second.size() == deadmobcount_RoomF && !m_bisMobsAllDead_Interior_Room_F)
				m_bisMobsAllDead_Interior_Room_F = true;
		}
		else if (mobcontainer.first == L"Interior_Room_G")
		{
			if (mobcontainer.second.size() == deadmobcount_RoomG && !m_bisMobsAllDead_Interior_Room_G)
				m_bisMobsAllDead_Interior_Room_G = true;
		}
	}
}

void CMainScene::TurnGuardsOnRange(const _double& _dTimeDelta)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	_float3 PlayerPos = m_spMainCamera->GetTransform()->GetPos();
	for (auto& guardcontainer : m_spMap->GetGuards())
	{
		_float3 mobPos = guardcontainer.second->GetTransform()->GetPos();
		_float3 distance = mobPos - PlayerPos;
		float distanceSq = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;

		if (distanceSq <= 200 * 200)
		{
			guardcontainer.second->SetActive(true);
		}
		else {
			guardcontainer.second->SetActive(false);
		}

		//가드 상태 업데이트
		if (guardcontainer.first == L"Interior_Room_D")
		{
			if(m_bisMobsAllDead_Interior_Room_D)
			{
				if (!m_fGuardSound_D) {
					spGameInstance->SoundPlayOnce(L"GuardDeactivate");
					spGameInstance->SoundPlayOnce(L"GuardDeactivate2");
					m_fGuardSound_D = true;
				}
				
				if (m_fGuardTimer_D > 2.f) {
					m_spGuardDeactivate_G->SetActive(false);
				}
				else {
					m_fGuardTimer_D += _dTimeDelta;
					m_spGuardDeactivate_G->SetActive(true);
				}
				guardcontainer.second->SetActive(false);
				spGameInstance->RemoveCollisionPawn(guardcontainer.second);
			}
		}
		else if (guardcontainer.first == L"Interior_Hallway_E")
		{
			if (m_bisMobsAllDead_Interior_Hallway_E)
			{
				if (!m_fGuardSound_E) {
					spGameInstance->SoundPlayOnce(L"GuardDeactivate");
					spGameInstance->SoundPlayOnce(L"GuardDeactivate2");
					m_fGuardSound_E = true;
				}
				if (m_fGuardTimer_E > 2.f) {
					m_spGuardDeactivate_D->SetActive(false);
				}
				else {
					m_fGuardTimer_E += _dTimeDelta;
					m_spGuardDeactivate_D->SetActive(true);
				}
				guardcontainer.second->SetActive(false);
				spGameInstance->RemoveCollisionPawn(guardcontainer.second);
			}
		}
		else if (guardcontainer.first == L"Interior_Room_F")
		{
			if (m_bisMobsAllDead_Interior_Room_F)
			{
				if (!m_fGuardSound_F) {
					spGameInstance->SoundPlayOnce(L"GuardDeactivate");
					spGameInstance->SoundPlayOnce(L"GuardDeactivate2");
					m_fGuardSound_F = true;
				}
				if (m_fGuardTimer_F > 2.f) {
					m_spGuardDeactivate_F->SetActive(false);
				}
				else {
					m_fGuardTimer_F += _dTimeDelta;
					m_spGuardDeactivate_F->SetActive(true);
				}
				guardcontainer.second->SetActive(false);
				spGameInstance->RemoveCollisionPawn(guardcontainer.second);
			}
		}
		else if (guardcontainer.first == L"Interior_Room_G")
		{
			if (m_bisMobsAllDead_Interior_Room_G)
			{
				if (!m_fGuardSound_G) {
					spGameInstance->SoundPlayOnce(L"GuardDeactivate");
					spGameInstance->SoundPlayOnce(L"GuardDeactivate2");
					m_fGuardSound_G = true;
				}
				
				if (m_fGuardTimer_G > 2.f) {
					m_spGuardDeactivate_E->SetActive(false);
				}
				else {
					m_fGuardTimer_G += _dTimeDelta;
					m_spGuardDeactivate_E->SetActive(true);
				}
				guardcontainer.second->SetActive(false);
				spGameInstance->RemoveCollisionPawn(guardcontainer.second);
			}
		}
		else if (guardcontainer.first == L"Interior_Room_A")
		{
			if (m_spWarriorPlayer->GetDoneCoreMinotaurState() && m_spWarriorPlayer->GetDoneCoreHarlequinnState() && m_spWarriorPlayer->GetDoneCoreAnubisState()&&m_bisDead_Anubis && m_bisDead_Harlequinn && m_bIsDead_Minotaur)
			{
				if (!m_fGuardSound_Final) {
					spGameInstance->SoundPlayOnce(L"GuardDeactivate");
					spGameInstance->SoundPlayOnce(L"GuardDeactivate2");
					m_fGuardSound_Final = true;
				}
				if (m_fTextTimer_Final > 5.f) {
					m_spEndingText->SetActive(false);
				}
				else {
					m_fTextTimer_Final += _dTimeDelta;
					m_spEndingText->SetActive(true);
				}
				
				guardcontainer.second->SetActive(false);
				spGameInstance->RemoveCollisionPawn(guardcontainer.second);
			}
		}
	}
}

void CMainScene::TurnLightsOnRange()
{

	_float3 PlayerPos = m_spMainCamera->GetTransform()->GetPos();
	for (auto& obj : (*m_spMap->GetStaticObjs().get()))
	{
		int count = 0;
		if (UMethod::ConvertWToS(obj.first) == "Torch_FBX.bin")
		{
			auto torch_it = obj.second.begin();
			while (torch_it != obj.second.end())
			{
				_float3 torchPos = torch_it->get()->GetTransform()->GetPos();
				_float3 distance = torchPos - PlayerPos;
				float distanceSq = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;
				CTorch* pTorch = dynamic_cast<CTorch*>(torch_it->get());

				if (distanceSq <= 120 * 120) {
					pTorch->GetParticle()->SetActive(true);
				}
				else {
					pTorch->GetParticle()->SetActive(false);
				}
				if (distanceSq <= 200 * 200)
				{
					torch_it->get()->SetActive(true);
					ActiveLIght(LIGHTTYPE::TYPE_POINT, count, LIGHTACTIVE::ISACTIVE);
					if (pTorch != nullptr)
					{
						pTorch->GetFire()->SetActive(true);
					}
				}
				else {
					torch_it->get()->SetActive(false);
					ActiveLIght(LIGHTTYPE::TYPE_POINT, count, LIGHTACTIVE::NONACTIVE);
					if (pTorch != nullptr)
					{			
						pTorch->GetFire()->SetActive(false);
					}
				}
				torch_it++;
				count++;
			}
		}
		if (UMethod::ConvertWToS(obj.first) == "Cores")
		{
			auto Cores_it = obj.second.begin();
			while (Cores_it != obj.second.end())
			{
				_wstring Name = Cores_it->get()->GetModel()->GetModelName();
				if (Name == L"MinotaurCore")
					if (m_bIsDead_Minotaur)
						Cores_it->get()->SetActive(true);
				if (Name == L"HarlequinnCore")
					if (m_bisDead_Harlequinn)
						Cores_it->get()->SetActive(true);
				if (Name == L"AnubisCore")
					if (m_bisDead_Anubis)
						Cores_it->get()->SetActive(true);

				Cores_it++;
			}
		}
		else
		{
			auto Models_it = obj.second.begin();
			while (Models_it != obj.second.end())
			{
				_float3 modelPos = Models_it->get()->GetTransform()->GetPos();
				_float3 distance = modelPos - PlayerPos;
				float distanceSq = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;
				if (distanceSq <= 200 * 200)
				{
					Models_it->get()->SetActive(true);
				}
				else
				{
					Models_it->get()->SetActive(false);
				}
				Models_it++;
			}
		}

	}

}

void CMainScene::TurnRoomsOnRange()
{
	_float3 PlayerPos = m_spMainCamera->GetTransform()->GetPos();
	for (auto& Rooms : (*m_spMap->GetRooms().get()))
	{
		_float3 roomPos = Rooms.second->GetRoomCenterPos();
		_float3 distance = roomPos - PlayerPos;
		float distanceSq = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;
		if (distanceSq <= 400 * 400)
		{
			Rooms.second->SetActive(true);
		}
		else
		{
			Rooms.second->SetActive(false);
		}
	}
}

void CMainScene::Free()
{
}

void CMainScene::CreateAbilityUI() {
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	CImageUI::UIDESC tDesc;
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"AbilityFrame";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Priority";
		tDesc.v2Size.x = static_cast<_float>(1280);
		tDesc.v2Size.y = static_cast<_float>(1080);
		tDesc.v2Pos = _float2{ 640,540 };
		m_spAbilityFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spAbilityFrameUI->SetActive(false);
	}
	CButtonUI::UIDESC tDesc2;
	{
		tDesc2.fZBufferOrder = 0.97f;
		tDesc2.strImgName = L"Rec";
		tDesc2._shaderName = PROTO_RES_PLEASEWAITUISHADER;
		tDesc2.DrawOrder = L"Last";
		tDesc2.v2Size.x = static_cast<_float>(200);
		tDesc2.v2Size.y = static_cast<_float>(75);
		tDesc2.v2Pos = _float2{ 1080,150 };
		m_spRecUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc2 }));
		m_spRecUI->SetActive(false);
	}
}

void CMainScene::CreateAttackUI()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	CImageUI::UIDESC tDesc;
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"T_DLCBossMission_103_001";
		tDesc._shaderName = PROTO_RES_COOLTIMEUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(80);
		tDesc.v2Size.y = static_cast<_float>(80);
		tDesc.v2Pos = _float2{ 550,900 };
		m_spUltimateAttackOneUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spUltimateAttackOneUI->SetActive(false);
		m_spUltimateAttackOneUI->SetDefaultCoolTime(ONE_SKILL);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Inventory_Slot_IconFrame";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(90);
		tDesc.v2Size.y = static_cast<_float>(90);
		tDesc.v2Pos = _float2{ 550,900 };
		m_spUltimateAttackOneFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spUltimateAttackOneFrameUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"T_DLCBossMission_105_002";
		tDesc._shaderName = PROTO_RES_COOLTIMEUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(80);
		tDesc.v2Size.y = static_cast<_float>(80);
		tDesc.v2Pos = _float2{ 750,900 };
		m_spUltimateAttackTwoUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spUltimateAttackTwoUI->SetActive(false);
		m_spUltimateAttackTwoUI->SetDefaultCoolTime(TWO_SKILL);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Inventory_Slot_IconFrame";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(90);
		tDesc.v2Size.y = static_cast<_float>(90);
		tDesc.v2Pos = _float2{ 750,900 };
		m_spUltimateAttackTwoFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spUltimateAttackTwoFrameUI->SetActive(false);
	}	
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"T_DLC_ApplyDamage07";
		tDesc._shaderName = PROTO_RES_COOLTIMEUISHADER;
		tDesc.DrawOrder = L"Priority";
		tDesc.v2Size.x = static_cast<_float>(80);
		tDesc.v2Size.y = static_cast<_float>(80);
		tDesc.v2Pos = _float2{ 950,900 };
		m_spShortAttackIconUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spShortAttackIconUI->SetActive(false);
		m_spShortAttackIconUI->SetDefaultCoolTime(Q_SKILL);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Inventory_Slot_IconFrame";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(90);
		tDesc.v2Size.y = static_cast<_float>(90);
		tDesc.v2Pos = _float2{ 950,900 };
		m_spShortAttackIconFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spShortAttackIconFrameUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_Q";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(30);
		tDesc.v2Size.y = static_cast<_float>(30);
		tDesc.v2Pos = _float2{ 980,930 };
		m_spShortAttackKeyIconUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spShortAttackKeyIconUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"T_DLCBossMission_315";
		tDesc._shaderName = PROTO_RES_COOLTIMEUISHADER;
		tDesc.DrawOrder = L"Priority";
		tDesc.v2Size.x = static_cast<_float>(80);
		tDesc.v2Size.y = static_cast<_float>(80);
		tDesc.v2Pos = _float2{ 1150,900 };
		m_spDetactAbilityIconUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spDetactAbilityIconUI->SetDefaultCoolTime(R_SKILL);
		m_spDetactAbilityIconUI->SetActive(false);
	
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Inventory_Slot_IconFrame";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(90);
		tDesc.v2Size.y = static_cast<_float>(90);
		tDesc.v2Pos = _float2{ 1150,900 };
		m_spDetactAbilityIconFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDetactAbilityIconFrameUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_R";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(30);
		tDesc.v2Size.y = static_cast<_float>(30);
		tDesc.v2Pos = _float2{ 1180,930 };
		m_spDetactAbilityKeyIconUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDetactAbilityKeyIconUI->SetActive(false);
	}
}

void CMainScene::CreateInteractUI()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	CImageUI::UIDESC tDesc;
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_F";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(75);
		tDesc.v2Size.y = static_cast<_float>(75);
		tDesc.v2Pos = _float2{ 450,200 };
		m_spFKeyOpenChestTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spFKeyOpenChestTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"OpenChest";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spOpenChestTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spOpenChestTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_F";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(75);
		tDesc.v2Size.y = static_cast<_float>(75);
		tDesc.v2Pos = _float2{ 700,200 };
		m_spFKeyLiftCageTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spFKeyLiftCageTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"LiftText";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spLiftCageTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spLiftCageTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Background";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spLiftLoadGageBackgroundTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spLiftLoadGageBackgroundTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Fill";
		tDesc._shaderName = PROTO_RES_GAGEUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spLiftFillGageBackgroundTextUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spLiftFillGageBackgroundTextUI->SetActive(false);
		m_spLiftFillGageBackgroundTextUI->SetDefaultCoolTime(4.f);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_F";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(75);
		tDesc.v2Size.y = static_cast<_float>(75);
		tDesc.v2Pos = _float2{ 575,200 };
		m_spFKeySaveCheckPointTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spFKeySaveCheckPointTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Checkpoint";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spSaveCheckPointTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spSaveCheckPointTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"CheckPointSaved";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spCheckPointCompleteTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spCheckPointCompleteTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Background";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spCheckPointLoadGageBackgroundTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spCheckPointLoadGageBackgroundTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Fill";
		tDesc._shaderName = PROTO_RES_GAGEUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spCheckPointFillGageBackgroundTextUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spCheckPointFillGageBackgroundTextUI->SetActive(false);
		m_spCheckPointFillGageBackgroundTextUI->SetDefaultCoolTime(4.f);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"GuardText";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spCollideGuardTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spCollideGuardTextUI->SetActive(false);
	}

	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Wasted";
		tDesc._shaderName = PROTO_RES_PLEASEWAITUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(450);
		tDesc.v2Size.y = static_cast<_float>(150);
		tDesc.v2Pos = _float2{ 640,540 };
		m_spDieTextUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spDieTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"PressRespawn";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spDieReviveTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDieReviveTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_G";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(50);
		tDesc.v2Size.y = static_cast<_float>(50);
		tDesc.v2Pos = _float2{ 420,200 };
		m_spDieKeyGUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDieKeyGUI->SetActive(false);
	}
}

void CMainScene::CreateDeactivateUI()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	CImageUI::UIDESC tDesc;
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"PressToDeactMino";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spDeactivateMinotaurTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeactivateMinotaurTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"DeactivateMinotaur";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(600);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spDeActivatedMinotaurTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeActivatedMinotaurTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Background";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spDeActivateMinotaurBackgroundUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeActivateMinotaurBackgroundUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Fill";
		tDesc._shaderName = PROTO_RES_GAGEUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spDeActivateMinotaurGageUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spDeActivateMinotaurGageUI->SetActive(false);
		m_spDeActivateMinotaurGageUI->SetDefaultCoolTime(5.f);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"PressToDeactHarlequin";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spDeactivateHarelequinTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeactivateHarelequinTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"DeactivateHarlequin";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(600);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spDeActivatedHarelequinTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeActivatedHarelequinTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Background";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spDeActivateHarelequinBackgroundUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeActivateHarelequinBackgroundUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Fill";
		tDesc._shaderName = PROTO_RES_GAGEUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spDeActivateHarelequinGageUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spDeActivateHarelequinGageUI->SetActive(false);
		m_spDeActivateHarelequinGageUI->SetDefaultCoolTime(5.f);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"PressToDeactAnubis";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(800);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spDeactivateAnubisTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeactivateAnubisTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"DeactivateAnubis";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(600);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spDeActivatedAnubisTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeActivatedAnubisTextUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Background";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Middle";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spDeActivateAnubisBackgroundUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spDeActivateAnubisBackgroundUI->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"XPBar_Fill";
		tDesc._shaderName = PROTO_RES_GAGEUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(500);
		tDesc.v2Size.y = static_cast<_float>(25);
		tDesc.v2Pos = _float2{ 640,280 };
		m_spDeActivateAnubisGageUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc }));
		m_spDeActivateAnubisGageUI->SetActive(false);
		m_spDeActivateAnubisGageUI->SetDefaultCoolTime(5.f);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_F";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(50);
		tDesc.v2Size.y = static_cast<_float>(50);
		tDesc.v2Pos = _float2{ 545,200 };
		m_spKeyFUIMinotaur = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spKeyFUIMinotaur->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_F";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(50);
		tDesc.v2Size.y = static_cast<_float>(50);
		tDesc.v2Pos = _float2{ 535,200 };
		m_spKeyFUIHarelequin = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spKeyFUIHarelequin->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"Key_F";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(50);
		tDesc.v2Size.y = static_cast<_float>(50);
		tDesc.v2Pos = _float2{ 555,200 };
		m_spKeyFUIAnubis = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spKeyFUIAnubis->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"FirstGuardDeActivated";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(600);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spGuardDeactivate_D = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spGuardDeactivate_D->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"SecondGuardDeActivated";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(600);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spGuardDeactivate_E = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spGuardDeactivate_E->SetActive(false);
	} {
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"ThirdGuardDeActivated";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(600);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spGuardDeactivate_F = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spGuardDeactivate_F->SetActive(false);
	}
	{
		tDesc.fZBufferOrder = 0.99f;
		tDesc.strImgName = L"FourthGuardDeActivated";
		tDesc._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc.DrawOrder = L"Last";
		tDesc.v2Size.x = static_cast<_float>(600);
		tDesc.v2Size.y = static_cast<_float>(140);
		tDesc.v2Pos = _float2{ 640,200 };
		m_spGuardDeactivate_G = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
		m_spGuardDeactivate_G->SetActive(false);
	}
}

void CMainScene::CreateStartSceneUI()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	{
		CImageUI::UIDESC tDesc;
		{
			tDesc.fZBufferOrder = 0.99f;
			tDesc.strImgName = L"Background";
			tDesc._shaderName = PROTO_RES_BACKGROUNDUISHADER;
			tDesc.DrawOrder = L"Priority";
			tDesc.v2Size.x = static_cast<_float>(1280);
			tDesc.v2Size.y = static_cast<_float>(1080);
			tDesc.v2Pos = _float2{ 640,540 };
			m_spBackgroundUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc }));
			m_spBackgroundUI->SetActive(true);
		}
		CImageUI::UIDESC tDesc1;
		{
			// ZBufferOrder는 이미지 Order 순서를 표현한다. 0에 가까울수록 맨 위, 1에 가까울수록 맨 뒤에 있는다. (0, 1)는 사용 X
			tDesc1.fZBufferOrder = 0.97f;
			tDesc1.strImgName = L"LoadingBar_Background";
			tDesc1._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
			tDesc1.DrawOrder = L"Middle";
			tDesc1.v2Size.x = static_cast<_float>(1020);
			tDesc1.v2Size.y = static_cast<_float>(27);
			tDesc1.v2Pos = _float2{ 640,840 };
			m_spLoadingBackgroundUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc1 }));
			m_spLoadingBackgroundUI->SetActive(false);
		}
		CButtonUI::UIDESC tDesc2;
		{
			tDesc2.fZBufferOrder = 0.97f;
			tDesc2.strImgName = L"LoadingBar_Fill";
			tDesc2._shaderName = PROTO_RES_LOADINGUISHADER;
			tDesc2.DrawOrder = L"Last";
			tDesc2.v2Size.x = static_cast<_float>(1000);
			tDesc2.v2Size.y = static_cast<_float>(25);
			tDesc2.v2Pos = _float2{ 640,840 };
			m_spLoadingFillingUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc2 }));
			m_spLoadingFillingUI->SetActive(false);
		}
		CImageUI::UIDESC tDesc3;
		{
			// ZBufferOrder는 이미지 Order 순서를 표현한다. 0에 가까울수록 맨 위, 1에 가까울수록 맨 뒤에 있는다. (0, 1)는 사용 X
			tDesc3.fZBufferOrder = 0.88f;
			tDesc3.strImgName = L"MainTitle";
			tDesc3._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
			tDesc3.DrawOrder = L"Last";
			tDesc3.v2Size.x = static_cast<_float>(640);
			tDesc3.v2Size.y = static_cast<_float>(240);
			tDesc3.v2Pos = _float2{ 640,220 };
			m_spMainTitleUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc3 }));
			m_spMainTitleUI->SetActive(true);
		}
		{
			// ZBufferOrder는 이미지 Order 순서를 표현한다. 0에 가까울수록 맨 위, 1에 가까울수록 맨 뒤에 있는다. (0, 1)는 사용 X
			tDesc3.fZBufferOrder = 0.88f;
			tDesc3.strImgName = L"THANKYOU";
			tDesc3._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
			tDesc3.DrawOrder = L"Middle";
			tDesc3.v2Size.x = static_cast<_float>(640);
			tDesc3.v2Size.y = static_cast<_float>(240);
			tDesc3.v2Pos = _float2{ 640,500 };
			m_spEndingMent = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc3 }));
			m_spEndingMent->SetActive(false);
		}
		CButtonUI::UIDESC tDesc4;
		{
			tDesc4.fZBufferOrder = 0.47f;
			tDesc4.strImgName = L"Enter";
			tDesc4._shaderName = PROTO_RES_BUTTONUISHADER;
			tDesc4.DrawOrder = L"Last";
			tDesc4.v2Size.x = static_cast<_float>(200);
			tDesc4.v2Size.y = static_cast<_float>(100);
			tDesc4.v2Pos = _float2{ 640, 650 } ;
			m_spEnterButtonUI = std::static_pointer_cast<CButtonUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_BUTTONUI, { &tDesc4 }));
			m_spEnterButtonUI->SetActive(true);
			
		}
		{
			tDesc4.fZBufferOrder = 0.47f;
			tDesc4.strImgName = L"Exit";
			tDesc4._shaderName = PROTO_RES_BUTTONUISHADER;
			tDesc4.DrawOrder = L"Last";
			tDesc4.v2Size.x = static_cast<_float>(180);
			tDesc4.v2Size.y = static_cast<_float>(80);
			tDesc4.v2Pos = _float2{ 640, 770 };
			m_spExitButtonUI = std::static_pointer_cast<CButtonUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_BUTTONUI, { &tDesc4 }));
			m_spExitButtonUI->SetActive(true);
			
		}
		CImageUI::UIDESC tDesc5;
		{
			// ZBufferOrder는 이미지 Order 순서를 표현한다. 0에 가까울수록 맨 위, 1에 가까울수록 맨 뒤에 있는다. (0, 1)는 사용 X
			tDesc5.fZBufferOrder = 0.8f;
			tDesc5.strImgName = L"T_TitleLogo_Shadow_SmokeWave_UI";
			tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
			tDesc5.DrawOrder = L"Middle";
			tDesc5.v2Size.x = static_cast<_float>(640);
			tDesc5.v2Size.y = static_cast<_float>(250);
			tDesc5.v2Pos = _float2{ 640,212.5 };
			m_spMainTitleEffectUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
			m_spMainTitleEffectUI->SetActive(true);
			
		}
		
		{
			// ZBufferOrder는 이미지 Order 순서를 표현한다. 0에 가까울수록 맨 위, 1에 가까울수록 맨 뒤에 있는다. (0, 1)는 사용 X
			tDesc5.fZBufferOrder = 0.48f;
			tDesc5.strImgName = L"T_TitleLogo_Shadow_Line_UI";
			tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
			tDesc5.DrawOrder = L"Last";
			tDesc5.v2Size.x = static_cast<_float>(1080);
			tDesc5.v2Size.y = static_cast<_float>(25);
			tDesc5.v2Pos = _float2{ 640,870 };
			m_spLineEffectUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
			m_spLineEffectUI->SetActive(true);
			
		}
		{
			// ZBufferOrder는 이미지 Order 순서를 표현한다. 0에 가까울수록 맨 위, 1에 가까울수록 맨 뒤에 있는다. (0, 1)는 사용 X
			tDesc5.fZBufferOrder = 0.43f;
			tDesc5.strImgName = L"Loading";
			tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
			tDesc5.DrawOrder = L"Last";
			tDesc5.v2Size.x = static_cast<_float>(150);
			tDesc5.v2Size.y = static_cast<_float>(50);
			tDesc5.v2Pos = _float2{ 1050,800 };
			m_spLoadingTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
			m_spLoadingTextUI->SetActive(false);
		}
		{
			// ZBufferOrder는 이미지 Order 순서를 표현한다. 0에 가까울수록 맨 위, 1에 가까울수록 맨 뒤에 있는다. (0, 1)는 사용 X
			tDesc5.fZBufferOrder = 0.43f;
			tDesc5.strImgName = L"LoadingDot";
			tDesc5._shaderName = PROTO_RES_LOADINGDOTUISHADER;
			tDesc5.DrawOrder = L"Last";
			tDesc5.v2Size.x = static_cast<_float>(60);
			tDesc5.v2Size.y = static_cast<_float>(25);
			tDesc5.v2Pos = _float2{ 1145,805 };
			m_spLoadingDotsUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc5 }));
			m_spLoadingDotsUI->SetActive(false);
		}
		{
			// ZBufferOrder는 이미지 Order 순서를 표현한다. 0에 가까울수록 맨 위, 1에 가까울수록 맨 뒤에 있는다. (0, 1)는 사용 X
			tDesc5.fZBufferOrder = 0.43f;
			tDesc5.strImgName = L"PleaseWait";
			tDesc5._shaderName = PROTO_RES_PLEASEWAITUISHADER;
			tDesc5.DrawOrder = L"Last";
			tDesc5.v2Size.x = static_cast<_float>(200);
			tDesc5.v2Size.y = static_cast<_float>(75);
			tDesc5.v2Pos = _float2{ 640, 650 };
			m_spPleaseWaitTextUI = std::static_pointer_cast<CLoadingUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_LOADINGUI, { &tDesc5 }));
			m_spPleaseWaitTextUI->SetActive(false);
		}
	}
}

void CMainScene::CreateGameSceneUI()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	CImageUI::UIDESC tDesc5;
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Player";
		tDesc5._shaderName = PROTO_RES_HPBARUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(350);
		tDesc5.v2Size.y = static_cast<_float>(32.5);
		tDesc5.v2Pos = _float2{ 250, 900 };
		m_spHpBarUI = std::static_pointer_cast<CHpBarUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_HPBARUI, { &tDesc5 }));
		m_spHpBarUI->SetActive(false);
		
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"UnitFrame_Dragon";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Priority";
		tDesc5.v2Size.x = static_cast<_float>(142.5);
		tDesc5.v2Size.y = static_cast<_float>(142.5);
		tDesc5.v2Pos = _float2{ 110,832.5 };
		m_spBackDragonPlayerFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spBackDragonPlayerFrameUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"UnitFrame_Avatar_Background";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Middle";
		tDesc5.v2Size.x = static_cast<_float>(80);
		tDesc5.v2Size.y = static_cast<_float>(80);
		tDesc5.v2Pos = _float2{ 115,840 };
		m_spBackPlayerFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spBackPlayerFrameUI->SetActive(false);
	}
		
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"UnitFrame_Avatar_Example";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(75);
		tDesc5.v2Size.y = static_cast<_float>(75);
		tDesc5.v2Pos = _float2{ 115,840 };
		m_spFrontPlayerFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spFrontPlayerFrameUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"PlayerLogo";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(100);
		tDesc5.v2Size.y = static_cast<_float>(45);
		tDesc5.v2Pos = _float2{ 220,865 };
		m_spPlayerNameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spPlayerNameUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_Tab";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(50);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 115,960 };
		m_spTABUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spTABUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"KeySettings";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(100);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 200,960 };
		m_spTABTEXTUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spTABTEXTUI->SetActive(false);
	}
	
	//=====================Minotaur UI=========================
	CImageUI::UIDESC tDesc1;
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"BossHpGauge_Deco";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Middle";
		tDesc5.v2Size.x = static_cast<_float>(1000);
		tDesc5.v2Size.y = static_cast<_float>(15);
		tDesc5.v2Pos = _float2{ 640,25 };
		m_spBossHpBarFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spBossHpBarFrameUI->SetActive(false);
	}

	{
		tDesc1.fZBufferOrder = 0.43f;
		tDesc1.strImgName = L"Boss";
		tDesc1._shaderName = PROTO_RES_HPBARUISHADER;
		tDesc1.DrawOrder = L"Middle";
		tDesc1.v2Size.x = static_cast<_float>(1000);
		tDesc1.v2Size.y = static_cast<_float>(35);
		tDesc1.v2Pos = _float2{ 640, 50 };
		m_spMinotaurHpBarUI = std::static_pointer_cast<CHpBarUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_HPBARUI, { &tDesc1 }));
		m_spMinotaurHpBarUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"MinotaurFrame";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Middle";
		tDesc5.v2Size.x = static_cast<_float>(80);
		tDesc5.v2Size.y = static_cast<_float>(80);
		tDesc5.v2Pos = _float2{ 100,50 };
		m_spMinotaurFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spMinotaurFrameUI->SetActive(false);
	}
	//=====================Harlequinn UI=========================
	{
		tDesc1.fZBufferOrder = 0.43f;
		tDesc1.strImgName = L"Boss";
		tDesc1._shaderName = PROTO_RES_HPBARUISHADER;
		tDesc1.DrawOrder = L"Middle";
		tDesc1.v2Size.x = static_cast<_float>(1000);
		tDesc1.v2Size.y = static_cast<_float>(35);
		tDesc1.v2Pos = _float2{ 640, 50 };
		m_spHarlequinnHpBarUI = std::static_pointer_cast<CHpBarUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_HPBARUI, { &tDesc1 }));
		m_spHarlequinnHpBarUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Harlequinn";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Middle";
		tDesc5.v2Size.x = static_cast<_float>(80);
		tDesc5.v2Size.y = static_cast<_float>(80);
		tDesc5.v2Pos = _float2{ 100,50 };
		m_spHarlequinnFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spHarlequinnFrameUI->SetActive(false);
	}
	//=====================Anubis UI=========================
	{
		tDesc1.fZBufferOrder = 0.43f;
		tDesc1.strImgName = L"Boss";
		tDesc1._shaderName = PROTO_RES_HPBARUISHADER;
		tDesc1.DrawOrder = L"Middle";
		tDesc1.v2Size.x = static_cast<_float>(1000);
		tDesc1.v2Size.y = static_cast<_float>(35);
		tDesc1.v2Pos = _float2{ 640, 50 };
		m_spAnubisHpBarUI = std::static_pointer_cast<CHpBarUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_HPBARUI, { &tDesc1 }));
		m_spAnubisHpBarUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"NasusFrame";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Middle";
		tDesc5.v2Size.x = static_cast<_float>(80);
		tDesc5.v2Size.y = static_cast<_float>(80);
		tDesc5.v2Pos = _float2{ 100,50 };
		m_spAnubisFrameUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spAnubisFrameUI->SetActive(false);
	}
	//===================ENDING=====================FreedomText
	{
		tDesc5.fZBufferOrder = 0.99f;
		tDesc5.strImgName = L"FreedomText";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Middle";
		tDesc5.v2Size.x = static_cast<_float>(800);
		tDesc5.v2Size.y = static_cast<_float>(175);
		tDesc5.v2Pos = _float2{ 640,400 };
		m_spEndingText = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spEndingText->SetActive(false);
	}
}

void CMainScene::CreateKeyInfoUI()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	CImageUI::UIDESC tDesc5;
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"BookPage";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Middle";
		tDesc5.v2Size.x = static_cast<_float>(1000);
		tDesc5.v2Size.y = static_cast<_float>(650);
		tDesc5.v2Pos = _float2{ 640,400 };
		m_spBOOKPAGEUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spBOOKPAGEUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"KeyboardControls";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(260);
		tDesc5.v2Size.y = static_cast<_float>(80);
		tDesc5.v2Pos = _float2{ 315,125};
		m_spKEYBOARDCONTROLSUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spKEYBOARDCONTROLSUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"wasd";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(180);
		tDesc5.v2Size.y = static_cast<_float>(100);
		tDesc5.v2Pos = _float2{ 352.5,230 };
		m_spMOVEUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spMOVEUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Move";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(120);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 350,310 };
		m_spMOVETextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spMOVETextUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_ShiftL";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(180);
		tDesc5.v2Size.y = static_cast<_float>(60);
		tDesc5.v2Pos = _float2{ 625,250 };
		m_spRUNUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spRUNUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Run";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(120);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 625,310 };
		m_spRUNTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spRUNTextUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_Space";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(200);
		tDesc5.v2Size.y = static_cast<_float>(60);
		tDesc5.v2Pos = _float2{925,250 };
		m_spJUMPSTANDUPUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spJUMPSTANDUPUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"JumpStandUp";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(240);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 925,315 };
		m_spJUMPSTANDUPTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spJUMPSTANDUPTextUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_Q";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(50);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 325,400 };
		m_spSHORTATTACKUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spSHORTATTACKUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"ShortDoubleAttack";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(200);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 325,450 };
		m_spSHORTATTACKTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spSHORTATTACKTextUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_1";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(50);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 525,400 };
		m_spULTIMATEATTACKONESUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spULTIMATEATTACKONESUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"UltimateAttack1";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(200);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 525,450 };
		m_spULTIMATEATTACKONETextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spULTIMATEATTACKONETextUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_2";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(50);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 725,400 };
		m_spULTIMATEATTACKTWOUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spULTIMATEATTACKTWOUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"UltimateAttack2";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(200);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 725,450 };
		m_spULTIMATEATTACKTWOTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spULTIMATEATTACKTWOTextUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_R";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(50);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 925,400 };
		m_spDETACTABILITYUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spDETACTABILITYUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"DetactAbility";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(200);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 925,450 };
		m_spDETACTABILITYTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spDETACTABILITYTextUI->SetActive(false);
	}


	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_C";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(50);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 325,600 };
		m_spROLLUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spROLLUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Roll";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(100);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 325,650 };
		m_spROLLTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spROLLTextUI->SetActive(false);
	}

	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Key_G";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(50);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 525,600 };
		m_spREVIVEUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spREVIVEUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Revive";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(100);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 525,650 };
		m_spREVIVETextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spREVIVETextUI->SetActive(false);
	}

	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Trg_Mouse_LeftClick";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(100);
		tDesc5.v2Size.y = static_cast<_float>(100);
		tDesc5.v2Pos = _float2{ 725,600 };
		m_spCOMBOATTACKONEUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spCOMBOATTACKONEUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"ComboAttack1";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(100);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 725,672 };
		m_spCOMBOATTACKONETextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spCOMBOATTACKONETextUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"Trg_Mouse_RightClick";
		tDesc5._shaderName = PROTO_RES_DEFAULTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(100);
		tDesc5.v2Size.y = static_cast<_float>(100);
		tDesc5.v2Pos = _float2{ 925,600 };
		m_spCOMBOATTACKTWOUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spCOMBOATTACKTWOUI->SetActive(false);
	}
	{
		tDesc5.fZBufferOrder = 0.43f;
		tDesc5.strImgName = L"ComboAttack2";
		tDesc5._shaderName = PROTO_RES_DEFAULTHIGHLIGHTUISHADER;
		tDesc5.DrawOrder = L"Last";
		tDesc5.v2Size.x = static_cast<_float>(100);
		tDesc5.v2Size.y = static_cast<_float>(50);
		tDesc5.v2Pos = _float2{ 925,672 };
		m_spCOMBOATTACKTWOTextUI = std::static_pointer_cast<CImageUI>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IMAGEUI, { &tDesc5 }));
		m_spCOMBOATTACKTWOTextUI->SetActive(false);
	}
}

HRESULT CMainScene::LoadSceneData()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	// Font Create 
	{
		m_spPlayerHpFont = spGameInstance->AddFont(FONT_NANUMSQUARE_ACBOLD);
		m_spPlayerHpFont->SetPos(_float2{ 300,860 });
		m_spPlayerHpFont->SetScale(_float2(1.0, 1.0));
		m_spPlayerHpFont->SetDepths(0.f);
		m_spPlayerHpFont->SetRender(false);
	} 
	{
		m_spMinotaurHpFont = spGameInstance->AddFont(FONT_NANUMSQUARE_ACBOLD);
		m_spMinotaurHpFont->SetPos(_float2{ 550,80 });
		m_spMinotaurHpFont->SetScale(_float2(1.25, 1.25));
		m_spMinotaurHpFont->SetDepths(0.f);
		m_spMinotaurHpFont->SetRender(false);
	}
	{
		m_spHarlequinnHpFont = spGameInstance->AddFont(FONT_NANUMSQUARE_ACBOLD);
		m_spHarlequinnHpFont->SetPos(_float2{ 550,80 });
		m_spHarlequinnHpFont->SetScale(_float2(1.25, 1.25));
		m_spHarlequinnHpFont->SetDepths(0.f);
		m_spHarlequinnHpFont->SetRender(false);
	}
	{
		m_spAnubisHpFont = spGameInstance->AddFont(FONT_NANUMSQUARE_ACBOLD);
		m_spAnubisHpFont->SetPos(_float2{ 550,80 });
		m_spAnubisHpFont->SetScale(_float2(1.25, 1.25));
		m_spAnubisHpFont->SetDepths(0.f);
		m_spAnubisHpFont->SetRender(false);
	}
	{
		
		m_spPlayerAbilityLeftTimeFont = spGameInstance->AddFont(FONT_NANUMSQUARE_ACBOLD);
		m_spPlayerAbilityLeftTimeFont->SetPos(_float2{ 150,150 });
		m_spPlayerAbilityLeftTimeFont->SetScale(_float2(2.0, 2.0));
		m_spPlayerAbilityLeftTimeFont->SetDepths(0.f);
		m_spPlayerAbilityLeftTimeFont->SetRender(false);
	}
	
	CProtoMaker::CreateMainSceneProtoData(spGameInstance, GetDevice(), std::static_pointer_cast<UCommand>(spGameInstance->GetGpuCommand()));

#ifdef _ENABLE_PROTOBUFF
	UCamera::CAMDESC tDesc;
	tDesc.stCamProj = UCamera::CAMPROJ(UCamera::PROJECTION_TYPE::PERSPECTIVE, _float3(0.f, 0.f, 0.f),
		_float3(0.f, 0.f, 1.f),
		DirectX::XMConvertToRadians(60.0f), WINDOW_WIDTH, WINDOW_HEIGHT, 0.2f, 1000.f);
	tDesc.stCamValue = UCamera::CAMVALUE(20.f, DirectX::XMConvertToRadians(90.f));
	tDesc.eCamType = CAMERATYPE::MAIN;
	// Actor Add Main Camera

	VOIDDATAS vecDatas;
	vecDatas.push_back(&tDesc);

	m_spMainCamera = std::static_pointer_cast<CMainCamera>(spGameInstance->CloneActorAdd(PROTO_ACTOR_MAINCAMERA, vecDatas));
	spGameInstance->MakeActors({ m_spMainCamera });
#else 
#endif
	CreateStartSceneUI();
	CreateAbilityUI();
	CreateInteractUI();
	CreateAttackUI();
	CreateDeactivateUI();
	CreateKeyInfoUI();
	CreateGameSceneUI();
	{
		m_spMap = CreateConstructorNative<CMap>(spGameInstance->GetDevice());
		m_spMap->LoadRooms();
		m_spMap->LoadStaticObjects();
		spGameInstance->TurnOnFog();

		//AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_DIRECTIONAL,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 1.f}, {0.3f, 0.3f,0.3f, 1.f}, {0.15f, 0.15f, 0.15f, 1.f}, {0.f, -1.f, 0.f,}, {0.f, 100.f, 0.f}, 0.f, 0.f ,1.f, 20.f });

		AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_DIRECTIONAL,LIGHTACTIVE::ISACTIVE, {0.15f, 0.15f, 0.15f, 1.f}, {0.15f, 0.15f,0.15f, 1.f}, {0.07f, 0.07f, 0.07f, 1.f}, {0.f, -1.f, 0.f,}, {0.f, 100.f, 0.f}, 0.f, 0.f ,1.f, 20.f });

		for (auto& obj : (*m_spMap->GetStaticObjs().get()))
		{
			int count = 0;
			if (UMethod::ConvertWToS(obj.first) == "Torch_FBX.bin")
			{
				auto torch_it = obj.second.begin();
				while (torch_it != obj.second.end())
				{
					AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_POINT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 1.f}, {0.4f, 0.2f, 0.08f, 1.f}, {0.8f, 0.4f, 0.16f, 1.f}, {0.f, 0.f, 1.f,},
						_float3(torch_it->get()->GetTransform()->GetPos().x,torch_it->get()->GetTransform()->GetPos().y + 4,torch_it->get()->GetTransform()->GetPos().z), 40.f, 0.f ,
					1.f, 32.f,0.f,0.f,0.f,_float3(1.f,0.01f,0.0001f) });
					m_spMap->AddLightCount();
					torch_it++;
				}
			}
		}

		AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_SPOT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 0.f}, {0.5f, 0.25f, 0.11f, 1.f}, {0.f, 1.5f, 1.2f, 1.f}, {0.f, -1.f, 0.f,}
			, _float3(-364.225,-20,253.010), 100.f, 60.f ,
			100.f, 32.f, 8.0f,(float)cos(DirectX::XMConvertToRadians(30.f)),(float)cos(DirectX::XMConvertToRadians(15.f)),_float3(1.0f, 0.01f, 0.0001f) });

		AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_SPOT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 0.f}, {0.5f, 0.25f, 0.11f, 1.f}, {0.f, 1.5f, 1.2f, 1.f}, {0.f, -1.f, 0.f,}
			, _float3(-535.39,-20,154.5), 100.f, 60.f ,
			100.f, 32.f, 8.0f,(float)cos(DirectX::XMConvertToRadians(30.f)),(float)cos(DirectX::XMConvertToRadians(15.f)),_float3(1.0f, 0.01f, 0.0001f) });

		AddLight(LIGHTINFO{ LIGHTTYPE::TYPE_SPOT,LIGHTACTIVE::ISACTIVE, {0.3f, 0.3f, 0.3f, 0.f}, {0.5f, 0.25f, 0.11f, 1.f}, {0.f, 1.5f, 1.2f, 1.f}, {0.f, -1.f, 0.f,}
			, _float3(-494.5,-45,289.265), 100.f, 60.f ,
			100.f, 32.f, 8.0f,(float)cos(DirectX::XMConvertToRadians(45.f)),(float)cos(DirectX::XMConvertToRadians(30.f)),_float3(1.0f, 0.01f, 0.0001f) });
	}
	{
		UFire::FIREDESC tFireDesc;
		tFireDesc.wstrFireShader = PROTO_RES_2DFIRESHADER;
		m_stFireOne = std::static_pointer_cast<UFire>(spGameInstance->CloneActorAdd(PROTO_ACTOR_FIRE, { &tFireDesc }));
		m_stFireTwo = std::static_pointer_cast<UFire>(spGameInstance->CloneActorAdd(PROTO_ACTOR_FIRE, { &tFireDesc }));
		
		 float ScaleX = 1.99f;
		 float ScaleY = 3.64f;
		_float3 ScaleFloat3 = _float3(ScaleX, ScaleY, 1);

		m_stFireOne->GetTransform()->SetScale(ScaleFloat3);
		m_stFireOne->GetTransform()->SetPos(_float3(-438.7,-50.8,156.4));
		m_stFireOne->SetColorTexture(L"BlueFlame");
		m_stFireOne->SetNoiseTexture(L"WFX_T_NukeFlames");
		m_stFireTwo->GetTransform()->SetScale(ScaleFloat3);
		m_stFireTwo->GetTransform()->SetPos(_float3(-410.6, -50.8, 172.8));
		m_stFireTwo->SetColorTexture(L"BlueFlame");
		m_stFireTwo->SetNoiseTexture(L"WFX_T_NukeFlames");
		m_stFireOne->SetActive(true);
			m_stFireTwo->SetActive(true);
	}
#ifndef _ENABLE_PROTOBUFF
	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
		// Main Camera Load 
		{
			UCamera::CAMDESC tDesc;
			tDesc.stCamProj = UCamera::CAMPROJ(UCamera::PROJECTION_TYPE::PERSPECTIVE, _float3(0.f, 0.f, 0.f),
				_float3(0.f, 0.f, 1.f),
				DirectX::XMConvertToRadians(60.0f), WINDOW_WIDTH, WINDOW_HEIGHT, 0.2f, 1000.f);
			tDesc.stCamValue = UCamera::CAMVALUE(20.f, DirectX::XMConvertToRadians(90.f));
			tDesc.eCamType = CAMERATYPE::MAIN;
			// Actor Add Main Camera

			VOIDDATAS vecDatas;
			vecDatas.push_back(&tDesc);

			m_spMainCamera = std::static_pointer_cast<CMainCamera>(spGameInstance->
				CloneActorAdd(PROTO_ACTOR_MAINCAMERA, {&tDesc }));

			CWarriorPlayer::CHARACTERDESC CharDesc{ PROTO_RES_FEMAILPLAYERANIMMODEL, PROTO_COMP_USERWARRIORANIMCONTROLLER };
			CWarriorPlayer::PLAYERDESC PlayerDesc{ m_spMainCamera };
			m_spWarriorPlayer = std::static_pointer_cast<CWarriorPlayer>(spGameInstance->CloneActorAdd(
				PROTO_ACTOR_WARRIORPLAYER, { &CharDesc, &PlayerDesc }));
			spGameInstance->RegisterCurrentPlayer(m_spWarriorPlayer);
			m_spMainCamera->GetTransform()->SetPos(m_spWarriorPlayer->GetTransform()->GetPos());
		}
	}

	m_spMap->LoadMobs(m_spWarriorPlayer);
	
#endif

	m_spMap->LoadGuards();
	m_spWarriorPlayer = std::static_pointer_cast<CWarriorPlayer>(spGameInstance->GetCurrPlayer());
	m_spMainCamera->GetTransform()->SetPos(m_spWarriorPlayer->GetTransform()->GetPos());

	return S_OK;
}

void CMainScene::DrawStartSceneUI(const _double& _dTimeDelta)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	if (m_bStartGameDefault) {
		spGameInstance->SoundPlayBGM(L"BGM2");
		spGameInstance->PauseGame();
		m_spWarriorPlayer->SetIfStartedGame(false);
		m_bStartGameDefault = false;
	}

	if (m_spEnterButtonUI->IsMouseOnRect() && m_spEnterButtonUI->IsActive()) {
		m_spEnterButtonUI->SetIfPicked(true);

		
		if (!wasMouseEnterOverButton) {
			spGameInstance->SoundPlayOnce(L"UI_Select");
			wasMouseEnterOverButton = true;  // 상태를 업데이트
		}

		
		if (true == spGameInstance->GetDIMBtnDown(DIMOUSEBUTTON::DIMB_L) && !m_bStartSceneForUI) {
			spGameInstance->SoundPlayOnce(L"PressedButton");
			spGameInstance->SoundStopBGM(L"BGM2");
			m_bStartSceneForUI = true;
		}
	}
	else {
	
		if (wasMouseEnterOverButton) {
			wasMouseEnterOverButton = false;
			spGameInstance->StopSound(L"UI_Select");
		}
		m_spEnterButtonUI->SetIfPicked(false);
	}

	if (m_spExitButtonUI->IsMouseOnRect()&& m_spExitButtonUI->IsActive()) {
		if (!wasMouseExitOverButton) {
			spGameInstance->SoundRestartOnce(L"UI_Mouseover");
			wasMouseExitOverButton = true;  // 상태를 업데이트
		}
		
		m_spExitButtonUI->SetIfPicked(true);
		
		if (true == spGameInstance->GetDIMBtnDown(DIMOUSEBUTTON::DIMB_L)) {
			spGameInstance->SoundPlayOnce(L"PressedButton2");
			::PostQuitMessage(0);
		}
	}
	else {
		if (wasMouseExitOverButton) {
			wasMouseExitOverButton = false;
			spGameInstance->StopSound(L"UI_Mouseover");
		}
		
		m_spExitButtonUI->SetIfPicked(false);
	}

	if (m_bStartSceneForUI&& !m_bStartGameForUI) {
	
		spGameInstance->SoundPlayOnce(L"BGM3");
		m_fStartSceneLoadingTimer += _dTimeDelta;

		m_spLoadingFillingUI->SetDurationTimePlus(_dTimeDelta);
		m_spLoadingDotsUI->SetDurationTimePlus(_dTimeDelta);
		m_spPleaseWaitTextUI->SetDurationTimePlus(_dTimeDelta);

		m_spEnterButtonUI->SetActive(false);
		m_spLoadingBackgroundUI->SetActive(true);
		m_spLoadingFillingUI->SetActive(true);
		m_spLoadingFillingUI->SetIfPicked(true);
		m_spLoadingTextUI->SetActive(true);
		
		m_spLoadingDotsUI->SetActive(true);
		m_spLoadingDotsUI->SetIfPicked(true);
		m_spPleaseWaitTextUI->SetActive(true);
		m_spPleaseWaitTextUI->SetIfPicked(true);


	}

	if (m_fStartSceneLoadingTimer > 10.f) {
		//GameStart 시
		spGameInstance->StopSound(L"BGM3");
		if (!EnterGameModeSound) {
			spGameInstance->SoundPlayOnce(L"FinishLoading");
			EnterGameModeSound = true;
		}	
		m_spWarriorPlayer->SetIfStartedGame(true);
		spGameInstance->ResumeGame();
		spGameInstance->SetGameStartEffect();
		m_spBackgroundUI->SetActive(false);
		m_spMainTitleUI->SetActive(false);
		m_spLoadingTextUI->SetActive(false);
		m_spLineEffectUI->SetActive(false);
		m_spMainTitleEffectUI->SetActive(false);
		m_spLoadingDotsUI->SetActive(false);
		m_spPleaseWaitTextUI->SetActive(false);
		m_spLoadingFillingUI->SetActive(false);
		m_spLoadingBackgroundUI->SetActive(false);
		m_spEnterButtonUI->SetActive(false);
		m_spExitButtonUI->SetActive(false);
		m_fStartSceneLoadingTimer = 0.f;
		m_bStartSceneForUI = false;
		m_bStartGameForUI = true;
		
	}

	if (!m_bStartSceneForUI && m_bStartGameForUI) {
		if (!EnterGameModeBGM) {
			spGameInstance->SoundPlayBGM(L"GamePlayBGM");//게임 플레이 bgm
			//SHPTR<USound> Bgm = spGameInstance->BringSound(L"GamePlayBGM");
			spGameInstance->BGMUpdateVolume(L"GamePlayBGM",0.5f);
			EnterGameModeBGM = true;
		}
		
		{//font
			m_spPlayerHpFont->SetRender(true);
		}
		if (spGameInstance->GetDIKeyPressing(DIK_TAB)) {
			{//키 조작 UI들 활성화
				m_spBOOKPAGEUI->SetActive(true);
				m_spKEYBOARDCONTROLSUI->SetActive(true);
				m_spMOVEUI->SetActive(true);
				m_spMOVETextUI->SetActive(true);
				m_spRUNUI->SetActive(true);
				m_spRUNTextUI->SetActive(true);
				m_spJUMPSTANDUPUI->SetActive(true);
				m_spJUMPSTANDUPTextUI->SetActive(true);
				m_spSHORTATTACKUI->SetActive(true);
				m_spSHORTATTACKTextUI->SetActive(true);
				m_spULTIMATEATTACKONESUI->SetActive(true);
				m_spULTIMATEATTACKONETextUI->SetActive(true);
				m_spULTIMATEATTACKTWOUI->SetActive(true);
				m_spULTIMATEATTACKTWOTextUI->SetActive(true);
				m_spDETACTABILITYUI->SetActive(true);
				m_spDETACTABILITYTextUI->SetActive(true);
				m_spROLLUI->SetActive(true);
				m_spROLLTextUI->SetActive(true);
				m_spREVIVEUI->SetActive(true);
				m_spREVIVETextUI->SetActive(true);
				m_spCOMBOATTACKONEUI->SetActive(true);
				m_spCOMBOATTACKONETextUI->SetActive(true);
				m_spCOMBOATTACKTWOUI->SetActive(true);
				m_spCOMBOATTACKTWOTextUI->SetActive(true);
			}
		}
		else {
			{//키 조작 UI들 비활성화
				m_spBOOKPAGEUI->SetActive(false);
				m_spKEYBOARDCONTROLSUI->SetActive(false);
				m_spMOVEUI->SetActive(false);
				m_spMOVETextUI->SetActive(false);
				m_spRUNUI->SetActive(false);
				m_spRUNTextUI->SetActive(false);
				m_spJUMPSTANDUPUI->SetActive(false);
				m_spJUMPSTANDUPTextUI->SetActive(false);
				m_spSHORTATTACKUI->SetActive(false);
				m_spSHORTATTACKTextUI->SetActive(false);
				m_spULTIMATEATTACKONESUI->SetActive(false);
				m_spULTIMATEATTACKONETextUI->SetActive(false);
				m_spULTIMATEATTACKTWOUI->SetActive(false);
				m_spULTIMATEATTACKTWOTextUI->SetActive(false);
				m_spDETACTABILITYUI->SetActive(false);
				m_spDETACTABILITYTextUI->SetActive(false);
				m_spROLLUI->SetActive(false);
				m_spROLLTextUI->SetActive(false);
				m_spREVIVEUI->SetActive(false);
				m_spREVIVETextUI->SetActive(false);
				m_spCOMBOATTACKONEUI->SetActive(false);
				m_spCOMBOATTACKONETextUI->SetActive(false);
				m_spCOMBOATTACKTWOUI->SetActive(false);
				m_spCOMBOATTACKTWOTextUI->SetActive(false);
			}
		}

		m_spHpBarUI->SetActive(true);
		m_spBackPlayerFrameUI->SetActive(true);
		m_spBackDragonPlayerFrameUI->SetActive(true);
		m_spFrontPlayerFrameUI->SetActive(true);
		m_spPlayerNameUI->SetActive(true);
		m_spTABUI->SetActive(true);
		m_spTABTEXTUI->SetActive(true);
		{//attack ui
			m_spUltimateAttackOneFrameUI->SetActive(true);
			m_spUltimateAttackOneUI->SetActive(true);
			m_spUltimateAttackTwoFrameUI->SetActive(true);
			m_spUltimateAttackTwoUI->SetActive(true);
			m_spDetactAbilityIconFrameUI->SetActive(true);
			m_spDetactAbilityIconUI->SetActive(true);
			m_spDetactAbilityKeyIconUI->SetActive(true);
			m_spShortAttackIconFrameUI->SetActive(true);
			m_spShortAttackIconUI->SetActive(true);
			m_spShortAttackKeyIconUI->SetActive(true);
		}
	
		m_spWarriorPlayer = std::static_pointer_cast<CWarriorPlayer>(spGameInstance->GetCurrPlayer());
		
		//상자 충돌 시
			if (m_spWarriorPlayer->GetCanInteractChestState() && !m_spWarriorPlayer->GetDeathState()) {
				m_spOpenChestTextUI->SetActive(true);
				m_spFKeyOpenChestTextUI->SetActive(true);
			}
			else {
				m_spOpenChestTextUI->SetActive(false);
				m_spFKeyOpenChestTextUI->SetActive(false);
			}
			//철창 충돌시
			if (m_spWarriorPlayer->GetCanInteractBarState() && !m_spWarriorPlayer->GetDoneInteractBarState() && !m_spWarriorPlayer->GetDeathState()) {
				m_spLiftCageTextUI->SetActive(true);
				m_spFKeyLiftCageTextUI->SetActive(true);
				m_spLiftLoadGageBackgroundTextUI->SetActive(true);
				m_spLiftFillGageBackgroundTextUI->SetActive(true);
			}
			else {
				m_spLiftCageTextUI->SetActive(false);
				m_spFKeyLiftCageTextUI->SetActive(false);
				m_spLiftLoadGageBackgroundTextUI->SetActive(false);
				m_spLiftFillGageBackgroundTextUI->SetActive(false);
			}
			//석상 충돌 시
			if (m_spWarriorPlayer->GetCanInteractStatueState()&&!m_spWarriorPlayer->GetDoneInteractStatueState() && !m_spWarriorPlayer->GetDeathState()) {
				m_spSaveCheckPointTextUI->SetActive(true);
				m_spFKeySaveCheckPointTextUI->SetActive(true);
				m_spCheckPointLoadGageBackgroundTextUI->SetActive(true);
				m_spCheckPointFillGageBackgroundTextUI->SetActive(true);
			}
			else {
				m_spSaveCheckPointTextUI->SetActive(false);
				m_spFKeySaveCheckPointTextUI->SetActive(false);
				m_spCheckPointLoadGageBackgroundTextUI->SetActive(false);
				m_spCheckPointFillGageBackgroundTextUI->SetActive(false);
			}
			//게임 종료 조건
			if (m_spWarriorPlayer->GetDoneCoreMinotaurState() && m_spWarriorPlayer->GetDoneCoreHarlequinnState() && m_spWarriorPlayer->GetDoneCoreAnubisState()) {
				//TODO
				//게임 종료
			}

			//수호자들 코어 충돌시
			//MINO
			if (m_spWarriorPlayer->GetCanInteractMinoCoreState()&& !m_spWarriorPlayer->GetDoneCoreMinotaurState() && !m_spWarriorPlayer->GetDeathState()) {
				m_spDeactivateMinotaurTextUI->SetActive(true);
				//m_spDeActivatedMinotaurTextUI->SetActive(true);
				m_spDeActivateMinotaurBackgroundUI->SetActive(true);
				m_spDeActivateMinotaurGageUI->SetActive(true);
				m_spKeyFUIMinotaur->SetActive(true);
			}
			else {
				m_spDeactivateMinotaurTextUI->SetActive(false);
				//m_spDeActivatedMinotaurTextUI->SetActive(false);
				m_spDeActivateMinotaurBackgroundUI->SetActive(false);
				m_spDeActivateMinotaurGageUI->SetActive(false);
				m_spKeyFUIMinotaur->SetActive(false);
			}

			//할리퀸
			if (m_spWarriorPlayer->GetCanInteractHarlCoreState() && !m_spWarriorPlayer->GetDoneCoreHarlequinnState() && !m_spWarriorPlayer->GetDeathState()) {
				m_spDeactivateHarelequinTextUI->SetActive(true);
				//m_spDeActivatedHarelequinTextUI->SetActive(true);
				m_spDeActivateHarelequinBackgroundUI->SetActive(true);
				m_spDeActivateHarelequinGageUI->SetActive(true);
				m_spKeyFUIHarelequin->SetActive(true);
			}
			else {
				m_spDeactivateHarelequinTextUI->SetActive(false);
				//m_spDeActivatedHarelequinTextUI->SetActive(false);
				m_spDeActivateHarelequinBackgroundUI->SetActive(false);
				m_spDeActivateHarelequinGageUI->SetActive(false);
				m_spKeyFUIHarelequin->SetActive(false);
			}

			//나서스
			if (m_spWarriorPlayer->GetCanInteractAnubisCoreState() && !m_spWarriorPlayer->GetDoneCoreAnubisState()&&!m_spWarriorPlayer->GetDeathState()) {
				m_spDeactivateAnubisTextUI->SetActive(true);
				//m_spDeActivatedAnubisTextUI->SetActive(true);
				m_spDeActivateAnubisBackgroundUI->SetActive(true);
				m_spDeActivateAnubisGageUI->SetActive(true);
				m_spKeyFUIAnubis->SetActive(true);
			}
			else {
				m_spDeactivateAnubisTextUI->SetActive(false);
				//m_spDeActivatedAnubisTextUI->SetActive(false);
				m_spDeActivateAnubisBackgroundUI->SetActive(false);
				m_spDeActivateAnubisGageUI->SetActive(false);
				m_spKeyFUIAnubis->SetActive(false);
			}

			//코어 해제시
			if (m_spWarriorPlayer->GetDeactivatedCoreMinotaurState()&&!m_spWarriorPlayer->GetDeathState()) {
				m_spDeactivateMinotaurTextUI->SetActive(false);
				m_spDeActivatedMinotaurTextUI->SetActive(true);
				m_spDeActivateMinotaurBackgroundUI->SetActive(false);
				m_spDeActivateMinotaurGageUI->SetActive(false);
				m_spKeyFUIMinotaur->SetActive(false);
				DeactivateElapsedTime += _dTimeDelta;
				if (DeactivateElapsedTime > 2.f) {
					DeactivateElapsedTime = 0;
					m_spWarriorPlayer->SetDeactivatedCoreMinotaurState(false);
				}

			}
			else {
				m_spDeActivatedMinotaurTextUI->SetActive(false);
			}
			if (m_spWarriorPlayer->GetDeactivatedCoreHarlequinnState()&&!m_spWarriorPlayer->GetDeathState()) {
				m_spDeactivateHarelequinTextUI->SetActive(false);
				m_spDeActivatedHarelequinTextUI->SetActive(true);
				m_spDeActivateHarelequinBackgroundUI->SetActive(false);
				m_spDeActivateHarelequinGageUI->SetActive(false);
				m_spKeyFUIHarelequin->SetActive(false);
				DeactivateElapsedTime += _dTimeDelta;
				if (DeactivateElapsedTime > 2.f) {
					DeactivateElapsedTime = 0;
					m_spWarriorPlayer->SetDeactivatedCoreHarlequinnState(false);
				}

			}
			else {
				m_spDeActivatedHarelequinTextUI->SetActive(false);
			}
			if (m_spWarriorPlayer->GetDeactivatedCoreAnubisState()&&!m_spWarriorPlayer->GetDeathState()) {
				m_spDeactivateAnubisTextUI->SetActive(false);
				m_spDeActivatedAnubisTextUI->SetActive(true);
				m_spDeActivateAnubisBackgroundUI->SetActive(false);
				m_spDeActivateAnubisGageUI->SetActive(false);
				m_spKeyFUIAnubis->SetActive(false);
				DeactivateElapsedTime += _dTimeDelta;
				if (DeactivateElapsedTime > 2.f) {
					DeactivateElapsedTime = 0;
					m_spWarriorPlayer->SetDeactivatedCoreAnubisState(false);
				}

			}
			else {
				m_spDeActivatedAnubisTextUI->SetActive(false);
			}

			//가드 충돌시
			if (m_spWarriorPlayer->GetCanInteractGuardState()&&!m_spWarriorPlayer->GetDeathState()) {
				m_spCollideGuardTextUI->SetActive(true);
			}
			else {
				m_spCollideGuardTextUI->SetActive(false);
			}
			// 체크포인트 저장시
			if (m_spWarriorPlayer->GetCheckpointSaveState()&&!m_spWarriorPlayer->GetDeathState()) {
				m_spCheckPointCompleteTextUI->SetActive(true);
				m_spSaveCheckPointTextUI->SetActive(false);
				m_spFKeySaveCheckPointTextUI->SetActive(false);
				m_spCheckPointLoadGageBackgroundTextUI->SetActive(false);
				m_spCheckPointFillGageBackgroundTextUI->SetActive(false);
				CheckPointCompleteElapsedTime += _dTimeDelta;
				if (CheckPointCompleteElapsedTime > 1.f) { 
					CheckPointCompleteElapsedTime = 0;
					m_spWarriorPlayer->SetCheckpointSaveState(false);
				}
			}
			else {
				m_spCheckPointCompleteTextUI->SetActive(false);
			}
			//플레이어 죽거나 , 단축키 설명 창 펼칠 시
			if (m_spWarriorPlayer->GetDeathState()||spGameInstance->GetDIKeyPressing(DIK_TAB)) {
				m_spOpenChestTextUI->SetActive(false);
				m_spFKeyOpenChestTextUI->SetActive(false);
				m_spLiftCageTextUI->SetActive(false);
				m_spFKeyLiftCageTextUI->SetActive(false);
				m_spLiftLoadGageBackgroundTextUI->SetActive(false);
				m_spLiftFillGageBackgroundTextUI->SetActive(false);
				m_spSaveCheckPointTextUI->SetActive(false);
				m_spCheckPointCompleteTextUI->SetActive(false);
				m_spFKeySaveCheckPointTextUI->SetActive(false);
				m_spCheckPointLoadGageBackgroundTextUI->SetActive(false);
				m_spCheckPointFillGageBackgroundTextUI->SetActive(false);
				m_spCollideGuardTextUI->SetActive(false);

				//core
				m_spDeactivateMinotaurTextUI->SetActive(false);
				m_spDeActivateMinotaurBackgroundUI->SetActive(false);
				m_spDeActivatedMinotaurTextUI->SetActive(false);
				m_spDeActivateMinotaurGageUI->SetActive(false);
				m_spKeyFUIMinotaur->SetActive(false);
				m_spDeactivateHarelequinTextUI->SetActive(false);			
				m_spDeActivatedHarelequinTextUI->SetActive(false);
				m_spDeActivateHarelequinBackgroundUI->SetActive(false);
				m_spDeActivateHarelequinGageUI->SetActive(false);
				m_spKeyFUIHarelequin->SetActive(false);
				m_spDeactivateAnubisTextUI->SetActive(false);
				m_spDeActivatedAnubisTextUI->SetActive(false);
				m_spDeActivateAnubisBackgroundUI->SetActive(false);
				m_spDeActivateAnubisGageUI->SetActive(false);
				m_spKeyFUIAnubis->SetActive(false);
				m_spEndingText->SetActive(false);
				m_spGuardDeactivate_G->SetActive(false);
				m_spGuardDeactivate_F->SetActive(false);
				m_spGuardDeactivate_E->SetActive(false);
				m_spGuardDeactivate_D->SetActive(false);
				m_spGuardDeactivate_D->SetActive(false);
			}
		
	}
}


void CMainScene::Tick(const _double& _dTimeDelta)
{
	SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
	
	DrawStartSceneUI(_dTimeDelta);
	TurnLightsOnRange();
	TurnRoomsOnRange();

	SHPTR<ULight> DirLight;
	OutLight(LIGHTTYPE::TYPE_DIRECTIONAL, 0, DirLight);
	
	
	SHPTR<ULight> PointLight;
	OutLight(LIGHTTYPE::TYPE_POINT, 0, PointLight);
	{
		m_spHpBarUI->SetMaxHp(m_spWarriorPlayer->GetMaxHealth());
		m_spHpBarUI->SetCurHp(m_spWarriorPlayer->GetHealth());
		m_spMinotaurHpBarUI->SetMaxHp(m_iMinotaurMaxHP);
		m_spMinotaurHpBarUI->SetCurHp(m_iMinotaurCurHP);
		m_spHarlequinnHpBarUI->SetMaxHp(m_iHarlequinnMaxHP);
		m_spHarlequinnHpBarUI->SetCurHp(m_iHarlequinnCurHP);
		m_spAnubisHpBarUI->SetMaxHp(m_iAnubisMaxHP);
		m_spAnubisHpBarUI->SetCurHp(m_iAnubisCurHP);				
	} 
	{
		std::wstringstream ws;
		ws << m_iMinotaurCurHP << L" / " << m_iMinotaurMaxHP;
		std::wstring health_string = ws.str();
		m_spMinotaurHpFont->SetText(health_string);
	}
	{
		std::wstringstream ws;
		ws << m_iHarlequinnCurHP << L" / " << m_iHarlequinnMaxHP;
		std::wstring health_string = ws.str();
		m_spHarlequinnHpFont->SetText(health_string);
	}
	{
		std::wstringstream ws;
		ws << m_iAnubisCurHP << L" / " << m_iAnubisMaxHP;
		std::wstring health_string = ws.str();
		m_spAnubisHpFont->SetText(health_string);
	}
	{
		int max_health = m_spWarriorPlayer->GetMaxHealth();
		int current_health = m_spWarriorPlayer->GetHealth();

		std::wstringstream ws;
		ws << current_health << L" / " << max_health;
		std::wstring health_string = ws.str();	
		m_spPlayerHpFont->SetText(health_string);		
	}
	{	//==========Minotaur Hp===============
		
	}
	{//if Die
		if (m_spWarriorPlayer->GetDeathState()) {
		
			m_spDieTextUI->SetActive(true);
			m_spDieTextUI->SetIfPicked(true);
			m_spDieKeyGUI->SetActive(true);
			m_spDieReviveTextUI->SetActive(true);
		}
		else {
			m_spDieTextUI->SetActive(false);
			m_spDieTextUI->SetIfPicked(false);
			m_spDieKeyGUI->SetActive(false);
			m_spDieReviveTextUI->SetActive(false);
		}
	}
	{  // If Use R Ability
		if (pGameInstance->GetDIKeyDown(DIK_R) && m_bStartGameForUI && !pGameInstance->GetIfAbilityIsOn()&& r_AbilityisAvailable)
		{
			if (!EnterAbilitySound) {
				pGameInstance->SoundPlayOnce(L"Abilitysound");
				pGameInstance->SoundPlayOnce(L"AbilityStart");
				EnterAbilitySound = true;
			}
			pGameInstance->TurnOnAbilityEffect();
			r_AbilityCoolTime = R_SKILL; 
			r_AbilityDurationTime = 5.f;
		}
		if (r_AbilityCoolTime > 0) { 
			r_AbilityisAvailable = false;
			r_AbilityDurationTime -= _dTimeDelta;
			if (r_AbilityDurationTime <= 0) {
				if(!ExitAbilitySound) {
					pGameInstance->SoundPlayOnce(L"ability");
					ExitAbilitySound = true;
				}
				r_AbilityCoolTime -= _dTimeDelta;
			}
			 
		}else {
			EnterAbilitySound = false;
			ExitAbilitySound = false;
			r_AbilityisAvailable = true;
		}
		{ //SKILL COOLTIME

			m_spUltimateAttackOneUI->SetLeftCoolTime(m_spWarriorPlayer->GetUltAttackOneCoolTime());
			m_spUltimateAttackTwoUI->SetLeftCoolTime(m_spWarriorPlayer->GetUltAttackTwoCoolTime());
			m_spDetactAbilityIconUI->SetLeftCoolTime(r_AbilityCoolTime);
			m_spShortAttackIconUI->SetLeftCoolTime(m_spWarriorPlayer->GetShortAttackCoolTime());
		}
		{	//Gage 
			m_spLiftFillGageBackgroundTextUI->SetLeftCoolTime(m_spWarriorPlayer->GetInteractionElapsedTime());
			m_spCheckPointFillGageBackgroundTextUI->SetLeftCoolTime(m_spWarriorPlayer->GetInteractionElapsedTime());
			m_spDeActivateMinotaurGageUI->SetLeftCoolTime(m_spWarriorPlayer->GetInteractionElapsedTime());
			m_spDeActivateHarelequinGageUI->SetLeftCoolTime(m_spWarriorPlayer->GetInteractionElapsedTime());
			m_spDeActivateAnubisGageUI->SetLeftCoolTime(m_spWarriorPlayer->GetInteractionElapsedTime());

		}
		if (m_spWarriorPlayer->GetDieEffectBool()) {
			pGameInstance->TurnOnDieEffect();
		}
		if (m_spWarriorPlayer->GetBlindEffectBool()) {
			if (!EnterBlindSound) {
				pGameInstance->SoundPlayOnce(L"BlindSound");
				EnterBlindSound = true;
			}
			pGameInstance->TurnOnHitEffect();
		}
		else {
			EnterBlindSound = false;
		}
		if (pGameInstance->GetIfAbilityIsOn()) {
			m_spRecUI->SetIfPicked(true);
			m_spRecUI->SetActive(true);
			m_spAbilityFrameUI->SetActive(true);
			_float AbilityLeftOverTime=5.f-pGameInstance->GetAbilityTime();
			std::wstringstream wss;
			wss << std::fixed << std::setprecision(4) << AbilityLeftOverTime;
			std::wstring formattedString = wss.str();
			m_spPlayerAbilityLeftTimeFont->SetText(formattedString);
			m_spPlayerAbilityLeftTimeFont->SetRender(true);
		}
		else {
			m_spRecUI->SetIfPicked(false);
			m_spRecUI->SetActive(false);
			m_spAbilityFrameUI->SetActive(false);
			m_spPlayerAbilityLeftTimeFont->SetRender(false);
		}
	}
	//게임 찐 종료
	if (m_spWarriorPlayer->GetDoneCoreMinotaurState() && m_spWarriorPlayer->GetDoneCoreHarlequinnState() && m_spWarriorPlayer->GetDoneCoreAnubisState() && m_bisDead_Anubis && m_bisDead_Harlequinn && m_bIsDead_Minotaur)
	{
		if (!EndGameSound) {
			pGameInstance->SoundStopBGM(L"GamePlayBGM");
			pGameInstance->SoundPlayOnce(L"EndingSong");
			pGameInstance->SetLooping(L"EndingSong", true);
			EndGameSound = true;
		}
		if (m_spWarriorPlayer->GetIfPlayerIsInEnd()) {
			EndingTimeElapsed += _dTimeDelta;
			if (EndingTimeElapsed > 4.5f) {
				::PostQuitMessage(0);
			}
			m_spBackgroundUI->SetActive(true);
			m_spEndingMent->SetActive(true);
			m_spMainTitleUI->SetActive(true);
			m_spMainTitleEffectUI->SetActive(true);
			//-------
			m_spPlayerHpFont->SetRender(false);
			m_spHpBarUI->SetActive(false);
			m_spBackPlayerFrameUI->SetActive(false);
			m_spBackDragonPlayerFrameUI->SetActive(false);
			m_spFrontPlayerFrameUI->SetActive(false);
			m_spPlayerNameUI->SetActive(false);
			m_spTABUI->SetActive(false);
			m_spTABTEXTUI->SetActive(false);
			{//attack ui
				m_spUltimateAttackOneFrameUI->SetActive(false);
				m_spUltimateAttackOneUI->SetActive(false);
				m_spUltimateAttackTwoFrameUI->SetActive(false);
				m_spUltimateAttackTwoUI->SetActive(false);
				m_spDetactAbilityIconFrameUI->SetActive(false);
				m_spDetactAbilityIconUI->SetActive(false);
				m_spDetactAbilityKeyIconUI->SetActive(false);
				m_spShortAttackIconFrameUI->SetActive(false);
				m_spShortAttackIconUI->SetActive(false);
				m_spShortAttackKeyIconUI->SetActive(false);
			}
		}

	}
	

	if(pGameInstance->GetDIKeyDown(DIK_ESCAPE))
		::PostQuitMessage(0);
}

void CMainScene::LateTick(const _double& _dTimeDelta)
{

	{
		SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

		if (spGameInstance->GetDIKeyPressing(DIK_F6))
		{
			spGameInstance->SetFreeModeCamEnable(true);
		}
		else if (true == spGameInstance->IsFreeModeCameraEnable() &&
			spGameInstance->GetDIKeyPressing(DIK_F7))
		{
			spGameInstance->SetFreeModeCamEnable(false);
		}

	}

	UpdateMobsStatus();
	TurnGuardsOnRange(_dTimeDelta);
	SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
	RETURN_CHECK(pGameInstance->IsFreeModeCameraEnable(), ;);

	if (!m_spWarriorPlayer->GetDeathState() && ((m_bIsFoundPlayer_Minotaur && !m_bIsDead_Minotaur) || (m_bisFoundPlayer_Harlequinn && !m_bisDead_Harlequinn) || (m_bisFoundPlayer_Anubis && !m_bisDead_Anubis))) {
		m_spBossHpBarFrameUI->SetActive(true);
		pGameInstance->BGMUpdateVolume(L"GamePlayBGM", 0.f);
	}
	else {
		pGameInstance->BGMUpdateVolume(L"GamePlayBGM", 0.5f);
		m_spBossHpBarFrameUI->SetActive(false);
	}
	if (!m_spWarriorPlayer->GetDeathState() && m_bIsFoundPlayer_Minotaur && !m_bIsDead_Minotaur) {
		if (!EnterMinoSound) {
			pGameInstance->SoundPlayOnce(L"MinoEnterMoan");
			pGameInstance->SoundPlay(L"MinoBackground");
			pGameInstance->SetLooping(L"MinoBackground", true);
			EnterMinoSound = true;
		}

		m_spMinotaurHpFont->SetRender(true);
		m_spMinotaurFrameUI->SetActive(true);
		m_spMinotaurHpBarUI->SetActive(true);

	}
	else {

		pGameInstance->StopSound(L"MinoBackground");
		pGameInstance->SetLooping(L"MinoBackground", false);
		EnterMinoSound = false;
		m_spMinotaurHpFont->SetRender(false);
		m_spMinotaurFrameUI->SetActive(false);
		m_spMinotaurHpBarUI->SetActive(false);
	}
	if (!m_spWarriorPlayer->GetDeathState() && m_bisFoundPlayer_Harlequinn && !m_bisDead_Harlequinn) {
		if (!EnterQuinnSound) {

			pGameInstance->SoundPlayOnce(L"12042307_1");
			pGameInstance->SoundPlay(L"QuinnEnterBackground");
			pGameInstance->SetLooping(L"QuinnEnterBackground", true);
			EnterQuinnSound = true;
		}

		m_spHarlequinnHpFont->SetRender(true);
		m_spHarlequinnFrameUI->SetActive(true);
		m_spHarlequinnHpBarUI->SetActive(true);
	}
	else {
		pGameInstance->StopSound(L"QuinnEnterBackground");
		pGameInstance->SetLooping(L"QuinnEnterBackground", false);
		EnterQuinnSound = false;
		m_spHarlequinnHpFont->SetRender(false);
		m_spHarlequinnFrameUI->SetActive(false);
		m_spHarlequinnHpBarUI->SetActive(false);
	}
	if (!m_spWarriorPlayer->GetDeathState() && m_bisFoundPlayer_Anubis && !m_bisDead_Anubis) {
		if (!EnterAnubisSound) {
			pGameInstance->SoundPlayOnce(L"AnubisEnterMoan");
			pGameInstance->SoundPlay(L"AnubisEnterBackground");
			pGameInstance->SetLooping(L"AnubisEnterBackground", true);
			EnterAnubisSound = true;
		}
		m_spAnubisHpFont->SetRender(true);
		m_spAnubisFrameUI->SetActive(true);
		m_spAnubisHpBarUI->SetActive(true);
	}
	else {

		pGameInstance->StopSound(L"AnubisEnterBackground");
		pGameInstance->SetLooping(L"AnubisEnterBackground", false);
		EnterAnubisSound = false;
		m_spAnubisHpFont->SetRender(false);
		m_spAnubisFrameUI->SetActive(false);
		m_spAnubisHpBarUI->SetActive(false);
	}
}

void CMainScene::CollisionTick(const _double& _dTimeDelta)
{
	
}

END
