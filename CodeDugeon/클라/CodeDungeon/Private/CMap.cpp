#include "ClientDefines.h"
#include "UGameInstance.h"
#include "CMap.h"
#include "UCollider.h"
#include "UModel.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"
#include "UModelMaterial.h"
#include "CRooms.h"
#include "UStageManager.h"
#include "UMapLayout.h"
#include "CTorch.h"
#include "CIronBars.h"
#include "CModelObjects.h"
#include "CMob.h"
#include "CItemChest.h"
#include "CMummy.h"
#include "UAnimModel.h"
#include "UMethod.h"
#include "UNavigation.h"
#include "CWarriorPlayer.h"
#include "CSarcophagus.h"
#include "CMinotaur.h"
#include "CHarlequinn.h"
#include "CAnubis.h"
#include "CMimic.h"
#include "UGuard.h"
#include "CStatue.h"
#include "CCoreAnubis.h"
#include "CCoreHarlequinn.h"
#include "CCoreMinotaur.h"

CMap::CMap(CSHPTRREF<UDevice> _spDevice) : UComponent(_spDevice),
m_spRoomContainer{nullptr},
m_spMapLayout{ nullptr },
m_spMobsContainer{nullptr},
m_iLightCount{ 0 },
m_iSarcophagusCount{0}
{
}

CMap::CMap(const CMap& _rhs) : UComponent(_rhs),
m_spRoomContainer{ nullptr },
m_spMapLayout{ nullptr },
m_spMobsContainer{ nullptr },
m_iLightCount{0},
m_iSarcophagusCount{ 0 }
{
}

void CMap::Free()
{
	m_spRoomContainer.reset();
	m_spMobsContainer.reset();
}

SHPTR<UCloneObject> CMap::Clone(const VOIDDATAS& _tDatas)
{
	return SHPTR<UCloneObject>();
}

HRESULT CMap::NativeConstruct()
{
	RETURN_CHECK_FAILED(__super::NativeConstruct(), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	m_spRoomContainer = Create<ROOMCONTAINER>();

	m_spMapLayout = CreateConstructorNativeNotMsg<UMapLayout>(spGameInstance->GetDevice());

	m_spStaticObjContainer = Create<STATICOBJCONTAINER>();
	m_spMobsContainer = Create<MOBSCONTAINER>();

	return S_OK;
}

HRESULT CMap::NativeConstructClone(const VOIDDATAS& _tDatas)
{
	return S_OK;
}

void CMap::LoadRooms()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	SHPTR<FILEGROUP> MapFolder = spGameInstance->FindFolder(L"Map");

	for (const FOLDERPAIR& Folder : MapFolder->UnderFileGroupList)
	{
		SHPTR<FILEGROUP> ConvertFolder = Folder.second->FindGroup(L"Convert");
		if (nullptr != ConvertFolder && 0 <= ConvertFolder->FileDataList.size())
		{
			for (const FILEPAIR& File : ConvertFolder->FileDataList)
			{
				_wstring FileName = File.second->wstrfileName;

				CRooms::ROOMDESC tDesc;
				tDesc._wsRoomName = FileName;

				SHPTR<CRooms> _Room = std::static_pointer_cast<CRooms>(spGameInstance->CloneActorAdd(
					PROTO_ACTOR_ROOM, { &tDesc }));
				m_spRoomContainer->emplace(FileName, _Room);
			}
		}
		else
		{
			return;
		}
	}
}

void CMap::LoadStaticObjects()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spMapLayout->LoadMapObjects();
	OBJCONTAINER _TorchVec;
	OBJCONTAINER _BarsVec;
	OBJCONTAINER _StatueVec;
	OBJCONTAINER _CoreVec;
	for (auto& it : (*m_spMapLayout->GetMapObjectsContainer().get()))
	{
		for (auto& vecit : it.second)
		{
			if(vecit._sModelName == "Torch_FBX.bin")
			{		
				CTorch::TORCHDESC torchDesc;
				torchDesc._Worldm = vecit._mWorldMatrix;
				SHPTR<CTorch> _Torch = std::static_pointer_cast<CTorch>(spGameInstance->CloneActorAdd(PROTO_ACTOR_TORCH, {&torchDesc}));
				_TorchVec.push_back(_Torch);		
			}
			if (vecit._sModelName == "Bars_FBX.bin")
			{
				CIronBars::IRONBARSDESC BarsDesc;
				BarsDesc._Worldm = vecit._mWorldMatrix;
				SHPTR<CIronBars> _IronBars = std::static_pointer_cast<CIronBars>(spGameInstance->CloneActorAdd(PROTO_ACTOR_IRONBARS, { &BarsDesc }));
				_BarsVec.push_back(_IronBars);
				spGameInstance->AddCollisionPawnList(_IronBars);
			}
			if (vecit._sModelName == "Statue_FBX.bin")
			{
				CStatue::STATUEDESC StatueDesc;
				StatueDesc._Worldm = vecit._mWorldMatrix;
				SHPTR<CStatue> _Statue = std::static_pointer_cast<CStatue>(spGameInstance->CloneActorAdd(PROTO_ACTOR_STATUE, { &StatueDesc }));
				_StatueVec.push_back(_Statue);
				spGameInstance->AddCollisionPawnList(_Statue);
			}
			if (vecit._sModelName == "minotaurhead_FBX.bin")
			{
				CCoreMinotaur::COREMINOTAURDESC coreDesc;
				coreDesc._Worldm = vecit._mWorldMatrix;
				SHPTR<CCoreMinotaur> _Core = std::static_pointer_cast<CCoreMinotaur>(spGameInstance->CloneActorAdd(PROTO_ACTOR_MINOTAURCORE, { &coreDesc }));
				_Core->GetModel()->SetModelName(L"MinotaurCore");
				_CoreVec.push_back(_Core);
				spGameInstance->AddCollisionPawnList(_Core);
			}
			if (vecit._sModelName == "anubishead_FBX.bin")
			{
				CCoreAnubis::COREANUBISNDESC coreDesc;
				coreDesc._Worldm = vecit._mWorldMatrix;
				SHPTR<CCoreAnubis> _Core = std::static_pointer_cast<CCoreAnubis>(spGameInstance->CloneActorAdd(PROTO_ACTOR_ANUBISCORE, { &coreDesc }));
				_Core->GetModel()->SetModelName(L"AnubisCore");
				_CoreVec.push_back(_Core);
				spGameInstance->AddCollisionPawnList(_Core);
			}
			if (vecit._sModelName == "HarlequinnHead_FBX.bin")
			{
				CCoreHarlequinn::COREHARLEQUINNDESC coreDesc;
				coreDesc._Worldm = vecit._mWorldMatrix;
				SHPTR<CCoreHarlequinn> _Core = std::static_pointer_cast<CCoreHarlequinn>(spGameInstance->CloneActorAdd(PROTO_ACTOR_HARLEQUINNCORE, { &coreDesc }));
				_Core->GetModel()->SetModelName(L"HarlequinnCore");
				_CoreVec.push_back(_Core);
				spGameInstance->AddCollisionPawnList(_Core);
			}
		}
	}
	m_spStaticObjContainer->emplace("Torch_FBX.bin", _TorchVec);
	m_spStaticObjContainer->emplace("Bars_FBX.bin", _BarsVec);
	m_spStaticObjContainer->emplace("Statue_FBX.bin", _StatueVec);
	m_spStaticObjContainer->emplace("Cores", _CoreVec);
}

void CMap::LoadGuards()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spMapLayout->LoadMapGuards();

	for (auto& it : (*m_spMapLayout->GetGuardsContainer().get()))
	{
		for (auto& vecit : it.second)
		{
			SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
			UGuard::GUARDDESC guardDesc;
			guardDesc.GuardType = UGuard::TYPE_RECT;
			SHPTR<UGuard> _Guard = std::static_pointer_cast<UGuard>(spGameInstance->CloneActorAdd(PROTO_ACTOR_GUARD, {&guardDesc}));
			_Guard->SetActive(true);
			_Guard->SetColorTexture(L"asdf");
			_Guard->GetTransform()->SetScale(_float3(40, 40, 1));
			_Guard->GetTransform()->SetDirection(vecit._mWorldMatrix.Get_Look());
			_Guard->GetTransform()->SetPos(_float3(vecit._mWorldMatrix.Get_Pos().x, vecit._mWorldMatrix.Get_Pos().y + 20, vecit._mWorldMatrix.Get_Pos().z));
			spGameInstance->AddCollisionPawnList(_Guard);
			m_GuardContainer.emplace(it.first, _Guard);
		}
	}
}

void CMap::LoadMobs(CSHPTRREF<CWarriorPlayer> _spPlayer)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spMapLayout->LoadMapMobs();

	MOBCONTAINER _Mobs;

	for (auto& it : (*m_spMapLayout->GetMapMobsContainer().get()))
	{
		_Mobs.clear();
		for (auto& vecit : it.second)
		{
#ifndef _ENABLE_PROTOBUFF

			if (vecit._sAnimModelName == "Mimic_FBX.bin")
			{
				CItemChest::CHARACTERDESC chestDesc{ PROTO_RES_CHESTANIMMODEL, PROTO_COMP_CHESTANIMCONTROLLER };;
				SHPTR<CItemChest> _Chest = std::static_pointer_cast<CItemChest>(spGameInstance->CloneActorAdd(PROTO_ACTOR_CHEST, { &chestDesc }));
				_Chest->GetTransform()->SetNewWorldMtx(vecit._mWorldMatrix);
				_Chest->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
				_Chest->SetChestType(CItemChest::TYPE_MIMIC);
				_Chest->SetTargetPlayer(_spPlayer);
				_Mobs.push_back(_Chest);
				spGameInstance->AddCollisionPawnList(_Chest);

				CMimic::CHARACTERDESC mimicDesc{ PROTO_RES_MIMICANIMMODEL, PROTO_COMP_MIMICANIMCONTROLLER };
				SHPTR<CMimic> _Mimic = std::static_pointer_cast<CMimic>(spGameInstance->CloneActorAdd(PROTO_ACTOR_MIMIC, { &mimicDesc }));
				_Mimic->GetTransform()->SetPos(vecit._mWorldMatrix.Get_Pos());
				_Mimic->GetTransform()->SetDirection(vecit._mWorldMatrix.Get_Look());
				_Mimic->GetAnimModel()->SetAnimation(UMethod::ConvertSToW(vecit._sAnimName));
				_Mimic->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
				_Mimic->SetTargetPlayer(_spPlayer);
				_Mimic->GetCurrentNavi()->FindCell(_Mimic->GetTransform()->GetPos());
				_Mobs.push_back(_Mimic);
				spGameInstance->AddCollisionPawnList(_Mimic);
			}
			else if (vecit._sAnimModelName == "Chest 1_FBX.bin")
			{
				CItemChest::CHARACTERDESC chestDesc{ PROTO_RES_CHESTANIMMODEL, PROTO_COMP_CHESTANIMCONTROLLER };;
				SHPTR<CItemChest> _Chest = std::static_pointer_cast<CItemChest>(spGameInstance->CloneActorAdd(PROTO_ACTOR_CHEST, { &chestDesc }));
				_Chest->GetTransform()->SetNewWorldMtx(vecit._mWorldMatrix);
				_Chest->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
				_Chest->SetTargetPlayer(_spPlayer);
				_Chest->SetChestType(CItemChest::TYPE_CHEST);
				_Mobs.push_back(_Chest);
				spGameInstance->AddCollisionPawnList(_Chest);
			}
			else if (vecit._sAnimModelName == "Mummy_DEMO_1_FBX.bin")
			{
				CMummy::CHARACTERDESC MummyDesc{ PROTO_RES_MUMMYANIMMODEL, PROTO_COMP_MUMMYANIMCONTROLLER };

				SHPTR<CMummy> _Mummy = std::static_pointer_cast<CMummy>(spGameInstance->CloneActorAdd(
					PROTO_ACTOR_MUMMY, { &MummyDesc }));
				_Mummy->GetTransform()->SetPos(vecit._mWorldMatrix.Get_Pos());
				_Mummy->GetTransform()->SetDirection(vecit._mWorldMatrix.Get_Look());
				_Mummy->GetAnimModel()->SetAnimation(UMethod::ConvertSToW(vecit._sAnimName));
				_Mummy->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
				if (vecit._sAnimName == "staticLaying")
				{
					_Mummy->SetMummyType(CMummy::MUMMYTYPE::TYPE_LYING);
				}
				else if (vecit._sAnimName == "staticStanding")
					_Mummy->SetMummyType(CMummy::MUMMYTYPE::TYPE_STANDING);
				else
					_Mummy->SetMummyType(CMummy::MUMMYTYPE::TYPE_WALKING);
				_Mummy->SetTargetPlayer(_spPlayer);
				_Mummy->GetCurrentNavi()->FindCell(_Mummy->GetTransform()->GetPos());
				spGameInstance->AddCollisionPawnList(_Mummy);


				//sarcophagus for mummy
				SHPTR<CSarcophagus> _Sarcophagus;
				if (vecit._sAnimName == "staticLaying")
				{
					CSarcophagus::CHARACTERDESC SarcDesc{ PROTO_RES_SARCOPHAGUSLYINGANIMMODEL, PROTO_COMP_SARCOPHAGUSANIMCONTROLLER };
					_Sarcophagus = std::static_pointer_cast<CSarcophagus>(spGameInstance->CloneActorAdd(
						PROTO_ACTOR_SARCOPHAGUSLYING, { &SarcDesc }));
					_Sarcophagus->SetSarcophagusType(CSarcophagus::SARCOTYPE::TYPE_LYING);
					_Sarcophagus->GetTransform()->SetNewWorldMtx(_Mummy->GetTransform()->GetWorldMatrix());
					_Mummy->GetTransform()->TranslateDir((-_Mummy->GetTransform()->GetLook()), 1, 10);
					_Sarcophagus->GetAnimModel()->SetAnimation(0);
					_Sarcophagus->SetTargetPlayer(_spPlayer);
					_Sarcophagus->SetOwnerMummy(_Mummy);
					_Sarcophagus->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
					_Mobs.push_back(_Sarcophagus);
				}
				else if (vecit._sAnimName == "staticStanding")
				{
					CSarcophagus::CHARACTERDESC SarcDesc{ PROTO_RES_SARCOPHAGUSSTANDINGANIMMODEL, PROTO_COMP_SARCOPHAGUSANIMCONTROLLER };
					_Sarcophagus = std::static_pointer_cast<CSarcophagus>(spGameInstance->CloneActorAdd(
						PROTO_ACTOR_SARCOPHAGUSSTANDING, { &SarcDesc }));
					_Sarcophagus->SetSarcophagusType(CSarcophagus::SARCOTYPE::TYPE_STANDING);
					_Sarcophagus->GetTransform()->SetNewWorldMtx(_Mummy->GetTransform()->GetWorldMatrix());
					_Sarcophagus->GetAnimModel()->SetAnimation(0);
					_Sarcophagus->SetTargetPlayer(_spPlayer);
					_Sarcophagus->SetOwnerMummy(_Mummy);
					_Sarcophagus->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
					_Mobs.push_back(_Sarcophagus);
				}

				_Mobs.push_back(_Mummy);

			}
			else if (vecit._sAnimModelName == "minotaur_FBX.bin")
			{
				CMinotaur::CHARACTERDESC MinotaurDesc{ PROTO_RES_MINOTAURANIMMODEL, PROTO_COMP_MINOTAURANIMCONTROLLER };

				SHPTR<CMinotaur> _Minotaur = std::static_pointer_cast<CMinotaur>(spGameInstance->CloneActorAdd(
					PROTO_ACTOR_MINOTAUR, { &MinotaurDesc }));
				_Minotaur->GetTransform()->SetPos(vecit._mWorldMatrix.Get_Pos());
				_Minotaur->GetTransform()->SetDirection(vecit._mWorldMatrix.Get_Look());
				_Minotaur->GetAnimModel()->SetAnimation(UMethod::ConvertSToW(vecit._sAnimName));
				_Minotaur->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
				_Minotaur->SetTargetPlayer(_spPlayer);
				_Minotaur->GetCurrentNavi()->FindCell(_Minotaur->GetTransform()->GetPos());
				spGameInstance->AddCollisionPawnList(_Minotaur);
				_Mobs.push_back(_Minotaur);
			}
			else if (vecit._sAnimModelName == "Harlequin1_FBX.bin")
			{
				CHarlequinn::CHARACTERDESC HarlequinnDesc{ PROTO_RES_HARLEQUINNANIMMODEL, PROTO_COMP_HARLEQUINNANIMCONTROLLER };

				SHPTR<CHarlequinn> _Harlequinn = std::static_pointer_cast<CHarlequinn>(spGameInstance->CloneActorAdd(
					PROTO_ACTOR_HARLEQUINN, { &HarlequinnDesc }));
				_Harlequinn->GetTransform()->SetPos(vecit._mWorldMatrix.Get_Pos());
				_Harlequinn->GetTransform()->SetDirection(vecit._mWorldMatrix.Get_Look());
				_Harlequinn->GetAnimModel()->SetAnimation(UMethod::ConvertSToW(vecit._sAnimName));
				_Harlequinn->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
				_Harlequinn->SetTargetPlayer(_spPlayer);
				_Harlequinn->GetCurrentNavi()->FindCell(_Harlequinn->GetTransform()->GetPos());
				spGameInstance->AddCollisionPawnList(_Harlequinn);
				_Mobs.push_back(_Harlequinn);
			}
			else if (vecit._sAnimModelName == "Anubis_FBX.bin")
			{
				CAnubis::CHARACTERDESC AnubisDesc{ PROTO_RES_ANUBISANIMMODEL, PROTO_COMP_ANUBISANIMCONTROLLER };
				SHPTR<CAnubis> _Anubis = std::static_pointer_cast<CAnubis>(spGameInstance->CloneActorAdd(
					PROTO_ACTOR_ANUBIS, { &AnubisDesc }));
				_Anubis->GetTransform()->SetPos(vecit._mWorldMatrix.Get_Pos());
				_Anubis->SetOriginPos(_Anubis->GetTransform()->GetPos());
				_Anubis->GetTransform()->SetDirection(vecit._mWorldMatrix.Get_Look());
				_Anubis->SetOriginDirection(vecit._mWorldMatrix.Get_Look());
				_Anubis->GetAnimModel()->SetAnimation(UMethod::ConvertSToW(vecit._sAnimName));
				_Anubis->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
				_Anubis->SetTargetPlayer(_spPlayer);
				_Anubis->GetCurrentNavi()->FindCell(_Anubis->GetTransform()->GetPos());
				spGameInstance->AddCollisionPawnList(_Anubis);
				_Mobs.push_back(_Anubis);
			}
			else if (vecit._sAnimModelName == "Mimic_FBX.bin")
			{
				CMimic::CHARACTERDESC MimicDesc{ PROTO_RES_MIMICANIMMODEL, PROTO_COMP_MIMICANIMCONTROLLER };
				SHPTR<CMimic> _Mimic = std::static_pointer_cast<CMimic>(spGameInstance->CloneActorAdd(
					PROTO_ACTOR_MIMIC, { &MimicDesc }));
				_Mimic->GetTransform()->SetPos(vecit._mWorldMatrix.Get_Pos());
				_Mimic->GetTransform()->SetDirection(vecit._mWorldMatrix.Get_Look());
				_Mimic->GetAnimModel()->SetAnimation(UMethod::ConvertSToW(vecit._sAnimName));
				_Mimic->GetAnimModel()->SetModelName(UMethod::ConvertSToW(vecit._sAnimModelName));
				_Mimic->SetTargetPlayer(_spPlayer);
				_Mimic->GetCurrentNavi()->FindCell(_Mimic->GetTransform()->GetPos());
				spGameInstance->AddCollisionPawnList(_Mimic);
				_Mobs.push_back(_Mimic);
			}
#endif
		}
		m_spMobsContainer->emplace(it.first, _Mobs);
	}

}

