#include "ToolDefines.h"
#include "TAnimControlView.h"
#include "TAnimControlModel.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "TAssimpModel.h"
#include "UAnimModel.h"
#include "UAnimation.h"
#include "AnimOccursEvents.h"
#include "AnimSectionEvents.h"
#include "UBoneNode.h"
#include "UCollider.h"
#include "UAudioSystem.h"
#include "USound.h"
#include "UModel.h"
#include "UMeshFilter.h"
#include "UMeshContainer.h"
#include "TEquipModel.h"

const _char* TAnimControlView::s_AnimTags[1000]{};

TAnimControlView::TAnimControlView(CSHPTRREF<UDevice> _spDevice) :
    TImGuiView(_spDevice, "AnimControlView"), 
	m_stMainDesc{},
	m_isInitSetting{false}, 
	m_spSelectAnimFileData{nullptr},
	m_iSelectAnimEvent{1},
	m_spSelectAnim{nullptr},
	m_AnimMaxTagCount{0},
	m_strSelectAnimationName{"NO_SELECTED"},
	m_eEquipType{EQUIPTYPE::EQUIP_END},
	m_iWeaponOrShieldValue{0},
	m_spSelectEquipModelData{nullptr},
	m_strSelectedEquipModelName{""}
{
	
}

void TAnimControlView::Free()
{
}

HRESULT TAnimControlView::NativeConstruct()
{
    m_stMainDesc = MAINDESC(ImGuiWindowFlags_NoBackground , ImGuiDockNodeFlags_None,
        ImVec2{ (_float)WINDOW_WIDTH, 0.f }, ImVec2{ 500.f, (_float)WINDOW_HEIGHT });

	m_stAnimModelSelectDesc = DOCKDESC("AnimModelSelect", ImGuiWindowFlags_NoFocusOnAppearing ,
		ImGuiDockNodeFlags_CentralNode);
	m_stAnimModifyDesc = DOCKDESC("AnimModifyViewer", ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_HorizontalScrollbar | 
		ImGuiWindowFlags_AlwaysVerticalScrollbar,
		ImGuiDockNodeFlags_CentralNode);

	m_stEquipViewDesc = DOCKDESC("EquipViewDesc", ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_HorizontalScrollbar |
		ImGuiWindowFlags_AlwaysVerticalScrollbar,
		ImGuiDockNodeFlags_CentralNode);

	m_stAnimMeshFilterDesc = DOCKDESC("AniMMeshFilterDesc", ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_HorizontalScrollbar |
		ImGuiWindowFlags_AlwaysVerticalScrollbar,
		ImGuiDockNodeFlags_CentralNode);

	m_spAnimControlModel = std::static_pointer_cast<TAnimControlModel>(GetGameInstance()->CloneActorAdd(PROTO_ACTOR_ANIMCONTROLMODELOBJECT));

	UCollider::COLLIDERDESC Desc{};
	m_AnimEventColliderContainer.insert(MakePair(UCollider::TYPE_AABB,
		std::static_pointer_cast<UCollider>(GetGameInstance()->CloneComp(PROTO_COMP_ABBCOLLIDER, {&Desc}))));

	m_AnimEventColliderContainer.insert(MakePair(UCollider::TYPE_OBB,
		std::static_pointer_cast<UCollider>(GetGameInstance()->CloneComp(PROTO_COMP_OBBCOLLIDER, { &Desc }))));

	m_AnimEventColliderContainer.insert(MakePair(UCollider::TYPE_SPHERE,
		std::static_pointer_cast<UCollider>(GetGameInstance()->CloneComp(PROTO_COMP_SPHERECOLLIDER, { &Desc }))));

    return S_OK;
}

HRESULT TAnimControlView::LoadResource()
{
	SHPTR<FILEGROUP> AnimModels  = GetGameInstance()->FindFolder(L"AnimModel");
	// Animation 
	{
		for (auto& iter : AnimModels->UnderFileGroupList)
		{
			// FBX에서 Convert 라는 하위 폴더를 찾는다. 
			SHPTR<FILEGROUP> spConvert = iter.second->FindGroup(L"Convert");
			if (nullptr == spConvert)
				continue;
			m_spSelectAnimFileFolder = spConvert;
			for (auto& FileData : spConvert->FileDataList)
			{
				m_AnimFileContainer.insert(MakePair(UMethod::ConvertWToS(FileData.first), FileData.second));
			}
		}

		SHPTR<UGameInstance> spGameInstance = GetGameInstance();
		SHPTR<UAudioSystem> spAudioSystem = spGameInstance->GetAudioSystem(SOUND_HARLEQUINN);
		m_FindSoundNames.clear();
		// 미리 넣어두기 
		for (auto& Sound : spAudioSystem->GetSoundOrders())
		{
			m_FindSoundNames.push_back(UMethod::ConvertWToS(Sound.first));
		}
	}
	SHPTR<FILEGROUP> ItemModels = GetGameInstance()->FindFolder(L"Equip");
	// Model
	{
		FindEquipModel(ItemModels);
	}


    return S_OK;
}

HRESULT TAnimControlView::ReleaseResource()
{
	m_spSelectAnim = nullptr;
	m_AnimFileContainer.clear();
	m_spAnimControlModel->SetActive(false);
	m_FindSoundNames.clear();
	m_EquipModelContainer.clear();
    return S_OK;
}

void TAnimControlView::TickActive(const _double& _dTimeDelta)
{
}

void TAnimControlView::LateTickActive(const _double& _dTimeDetla)
{
}

void TAnimControlView::RenderActive()
{
	ImGui::SetNextWindowPos(m_stMainDesc.vPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(m_stMainDesc.vSize, ImGuiCond_Appearing);
	// Name, OpenPointer, MainDesc 
	ImGui::Begin(GetName().c_str(), GetOpenPointer(), m_stMainDesc.imgWindowFlags);
	{
		m_stMainDesc.iDockSpaceID = ImGui::GetID(GetName().c_str());
		DockBuildInitSetting();
		ImGui::DockSpace(m_stMainDesc.iDockSpaceID, ImVec2{}, m_stMainDesc.imgDockNodeFlags);
		AnimModelSelectView();
		AnimModifyView();
		EquipView();
		MeshFilterView();
	}
	ImGui::End();
}

void TAnimControlView::DockBuildInitSetting()
{
	RETURN_CHECK(true == m_isInitSetting, ;);
	{
		ImGuiID dock_main_id = m_stMainDesc.iDockSpaceID;
		ImGui::DockBuilderRemoveNode(m_stMainDesc.iDockSpaceID);
		ImGui::DockBuilderAddNode(m_stMainDesc.iDockSpaceID);
		// Docking Build 
	    m_stAnimModelSelectDesc.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.2f, NULL, &dock_main_id);
		m_stAnimModifyDesc.iDockSpaceID = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.8f, NULL, &dock_main_id);

		m_stEquipViewDesc.iDockSpaceID = ImGui::DockBuilderSplitNode(m_stAnimModelSelectDesc.iDockSpaceID,
			ImGuiDir_Up, 0.2f, &m_stAnimModelSelectDesc.iDockSpaceID, & dock_main_id);

		m_stAnimMeshFilterDesc.iDockSpaceID = ImGui::DockBuilderSplitNode(m_stAnimModifyDesc.iDockSpaceID,
			ImGuiDir_Up, 0.2f, &m_stAnimModifyDesc.iDockSpaceID, &dock_main_id);
		// DockBuild
		ImGui::DockBuilderDockWindow(m_stAnimModelSelectDesc.strName.c_str(), m_stAnimModelSelectDesc.iDockSpaceID);
		ImGui::DockBuilderDockWindow(m_stAnimModifyDesc.strName.c_str(), m_stAnimModifyDesc.iDockSpaceID);
		ImGui::DockBuilderDockWindow(m_stEquipViewDesc.strName.c_str(), m_stEquipViewDesc.iDockSpaceID);
		ImGui::DockBuilderDockWindow(m_stAnimMeshFilterDesc.strName.c_str(), m_stAnimMeshFilterDesc.iDockSpaceID);
		ImGui::DockBuilderFinish(m_stMainDesc.iDockSpaceID);
	}
	m_isInitSetting = true;
}

void TAnimControlView::AnimModelSelectView()
{
	ImGui::Begin(m_stAnimModelSelectDesc.strName.c_str(), GetOpenPointer(), m_stAnimModelSelectDesc.imgWindowFlags);
	{
		ImGui::Text("AnimModelSelect");
		static _wstring wstrSelectFolderName{ L"" };
		if (ImGui::BeginListBox("AnimModel List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			using ANIMFILEDATA = std::pair < _string, SHPTR<FILEDATA>>;
			for (const ANIMFILEDATA& AnimModel : m_AnimFileContainer)
			{
				if (ImGui::Selectable(AnimModel.first.c_str()))
				{
					m_spSelectAnimFileData = AnimModel.second;
					wstrSelectFolderName = UMethod::ConvertSToW(AnimModel.first);
				 }
			}
			ImGui::EndListBox();
		}
		if (true == ImGui::Button("Load AnimationModel"))
		{
			// Select Folder 
			if (nullptr != m_spSelectAnimFileData)
			{
				m_spSelectAnimFileFolder = m_spSelectAnimFileData->wpFolder.lock();
				m_spShowAnimModel = CreateConstructorNative<UAnimModel>(GetDevice(), m_spSelectAnimFileData->wstrfilePath);
				m_spAnimControlModel->SetShowModel(m_spShowAnimModel, m_spSelectAnimFileFolder);

				for (auto& iter : m_spAnimControlModel->GetAnimationClips())
				{
					s_AnimTags[iter.second] = iter.first.c_str();
				}
				m_AnimMaxTagCount = static_cast<_int>(m_spAnimControlModel->GetAnimationClips().size());


				for (auto& Animation : m_spShowAnimModel->GetAnimations())
				{
					for (auto& iter : Animation->GetAnimEventContainer())
					{
						if (iter.first == ANIMEVENT_COLLIDER)
						{
							for (auto& Event : iter.second)
							{
								ANIMCOLLIDERDESC* ChangeDesc = remove_const<ANIMCOLLIDERDESC*, ANIMOTHEREVENTDESC*>(Event->OutOtherEventDesc());
								ChangeDesc->spCollider = m_AnimEventColliderContainer.find(ChangeDesc->iColliderType)->second;
							}
						}
					}
				}
			}
		}
	
		if (nullptr == m_spSelectAnimFileData)
		{
			ImGui::Text("Plz Select AnimationModel");
		}
		else
		{
			if (nullptr != m_spAnimControlModel)
			{
				ImGui::SameLine();
				if (true == ImGui::Button("Save Server Anim Data"))
				{
					m_spShowAnimModel->OutAnimationDataToServer(m_spSelectAnimFileFolder->wstrPath);
				}
			}
		}
	}
	ImGui::End();
}

void TAnimControlView::AnimModifyView()
{
	ImGui::Begin(m_stAnimModifyDesc.strName.c_str(), GetOpenPointer(), m_stAnimModifyDesc.imgWindowFlags);
	{
		// select Animation
		m_spAnimControlModel->ShowAnimModify();
		MakeAnimEvent();
	}
	ImGui::End();
}

void TAnimControlView::EquipView()
{
	ImGui::Begin(m_stEquipViewDesc.strName.c_str(), GetOpenPointer(), m_stEquipViewDesc.imgWindowFlags);
	{
		if (nullptr != m_spAnimControlModel->GetAnimModel())
		{
			_int EquipmentType = static_cast<_int>(m_eEquipType);
			_int iWeaponOrShieldValue = m_iWeaponOrShieldValue;
			static const _char* EquipType[]{ "Weapon", "DefensiveGear" };
			static const _char* WEAPONANDDEFENSIVEGEAR[]{ "SWORD", "BOW", "BOOTS" };

			if (true == ImGui::Combo("EquipType", &EquipmentType, EquipType, EQUIPTYPE::EQUIP_END))
			{
				m_eEquipType = static_cast<EQUIPTYPE>(EquipmentType);
			}
			if (true == ImGui::Combo("WeaponOrDefensiveGear", &iWeaponOrShieldValue, WEAPONANDDEFENSIVEGEAR, DEFENSIVEGEARTYPE::DFGEAR_END))
			{
				m_iWeaponOrShieldValue = iWeaponOrShieldValue;
			}

			m_spAnimControlModel->SelectBoneNodeName(OUT m_wstrBoneNodeName);

			if (nullptr != m_spAnimControlModel->GetAnimModel())
			{
				if (ImGui::BeginListBox("Equip List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (auto& iter : m_EquipModelContainer)
					{
						if (ImGui::Selectable(iter.first.c_str()))
						{
							m_spSelectEquipModelData = iter.second;
							m_strSelectedEquipModelName = iter.first;
						}
					}
					ImGui::EndListBox();
				}

				if (nullptr != m_spSelectEquipModelData)
				{
					ImGui::Text(m_strSelectedEquipModelName);
				}
				else
				{
					ImGui::Text("Don't Selected EquipModelData");
				}

				if (false == m_spAnimControlModel->IsSelectedEquipModel())
				{
					if (true == ImGui::Button("MakeEquip"))
					{
						if (nullptr != m_spSelectEquipModelData)
						{
							m_spAnimControlModel->MakeEquip(m_spSelectEquipModelData, m_eEquipType, m_wstrBoneNodeName, m_iWeaponOrShieldValue);
						}
					}
				}
				else
				{
					ImGui::Text("Dont Selected Equip CheckBox");
				}

				m_spAnimControlModel->SelectEquip();

				if (true == ImGui::Button("SaveEquip"))
				{
					m_spAnimControlModel->GetSelectedEquipModel()->SaveEquipDesc(m_spSelectEquipModelData->GetFileGroup()->wstrPath);
				}
				ImGui::SameLine();
				if (true == ImGui::Button("LoadEquip"))
				{
					TEquipModel::EQDESC Desc(m_spSelectEquipModelData, m_spSelectEquipModelData->GetFileGroup()->wstrPath);
					m_spAnimControlModel->GetSelectedEquipModel()->ChangeEquipDescInfo(Desc);
				}
			}
			else
			{
				ImGui::Text("Plz Create To AnimModel");
			}
		}
		else
		{
			ImGui::Text("Plz Create To AnimModel");
		}
	}
	ImGui::End();
}

void TAnimControlView::MeshFilterView()
{
	ImGui::Begin(m_stAnimMeshFilterDesc.strName.c_str(), GetOpenPointer(), m_stAnimMeshFilterDesc.imgWindowFlags);
	{
		static _string MESHEnable{ "MeshEnable##" };
		static _string MESHGROUP{ "MeshGroup##" };

		if (nullptr != m_spShowAnimModel)
		{
			SHPTR<UMeshFilter> spMeshFilter = m_spShowAnimModel->GetMeshFilter();
			if (nullptr != spMeshFilter)
			{
				_uint i = 0;
				for (auto& iter : spMeshFilter->GetMeshFilterInfoContainer())
				{
					_string str = UMethod::ConvertWToS(m_spShowAnimModel->GetMeshContainers()[iter.iCurMeshIndex]->GetMeshName());
					if (ImGui::TreeNodeEx(str))
					{
						ImGui::Checkbox(MESHEnable + i, &iter.isMeshFilterEnable);
						ImGui::InputInt(MESHGROUP + i, &iter.iMeshGroupIndex);
						ImGui::TreePop();
					}
				}
				if (ImGui::Button("SaveMeshFilter"))
				{
					spMeshFilter->Save(m_spSelectAnimFileFolder->wstrPath);
				}
			}
		}
		else
		{
			ImGui::Text("Plz Create To AnimModel");
		}
	}
	ImGui::End();
}

void TAnimControlView::MakeAnimEvent()
{
	RETURN_CHECK(nullptr == m_spShowAnimModel, ;);

	static const _char* ANIMTYPETAG[]{ "EFFECT", "SOUND", 
		"COLLIDER", "CAMERA", "OBJACTIVE", "ANIMCAHNGEBETWEEN", "ECT"};

	SHPTR<UAnimation> spCurAnimation = m_spShowAnimModel->GetCurrentAnimation();

	_bool bIsApplyRootBone = false;
	if (nullptr != spCurAnimation) {
		bIsApplyRootBone = spCurAnimation->IsApplyRootBoneMove();
	}

	if (true == ImGui::Checkbox("ApplyRootBoneMove", &bIsApplyRootBone)) {
		spCurAnimation->SetApplyRootBoneMove(bIsApplyRootBone);
	}

	if (ImGui::TreeNodeEx("MakeAnimEvent",ImGuiTreeNodeFlags_Bullet))
	{
		ImGui::Text(m_strSelectAnimationName);


		ImGui::Combo("AnimEvent", &m_iSelectAnimEvent, ANIMTYPETAG, ANIMEVENTTYPE::ANIMEVENT_END);

		if (true == ImGui::Button("SelectAnim"))
		{
			m_spSelectAnim = spCurAnimation;
			m_strSelectAnimationName = UMethod::ConvertWToS(m_spSelectAnim->GetAnimName());
		}
		ImGui::SameLine();
		if (true == ImGui::Button("MakeEvent"))
		{
			ANIMEVENTTYPE Event = static_cast<ANIMEVENTTYPE>(m_iSelectAnimEvent);
			if (nullptr != m_spSelectAnim)
			{
				SHPTR<UAnimEvent> spAnimEvent{ nullptr };
				switch (Event)
				{
				case ANIMEVENT_CAMERA:
					break;
				case ANIMEVENT_ANIMCHANGESBETWEEN:
					spAnimEvent = Create< UAnimChangeBetweenEvent>();
					break;
				case ANIMEVENT_COLLIDER:
					// 0번을 선택하여 채워줌
				{
					spAnimEvent = Create<UAnimColliderEvent>();
					ANIMCOLLIDERDESC* ChangeDesc = remove_const<ANIMCOLLIDERDESC*, ANIMOTHEREVENTDESC*>(spAnimEvent->OutOtherEventDesc());
					ChangeDesc->spCollider = m_AnimEventColliderContainer.find(0)->second;
				}
					break;
				case ANIMEVENT_EFFECT:

					break;
				case ANIMEVENT_OBJACTIVE:

					break;
				case ANIMEVENT_ANIMOCCURSTIMEPASS:
					spAnimEvent = Create< UAnimOccursTimePassEvent>();
					break;
				case ANIMEVENT_SOUND:
					spAnimEvent = Create<UAnimSoundEvent>();
					break;
				}
				m_spSelectAnim->InsertAnimEvent(Event, spAnimEvent);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("AnimEventSave"))
		{
			for (auto& iter : m_spShowAnimModel->GetAnimations())
			{
				iter->SaveAnimEventPathIsFolder(m_spSelectAnimFileFolder->wstrPath);
			}
		}

		if (nullptr != m_spSelectAnim)
		{
			// Options
			static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
				| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;

			// Event Container 
			for (auto& iter : m_spSelectAnim->GetAnimEventContainer())
			{
				switch (iter.first)
				{
				case ANIMEVENT_CAMERA:
					break;
				case ANIMEVENT_ANIMCHANGESBETWEEN:
					AnimSectionShow(m_spSelectAnim, flags, iter.second);
					break;
				case ANIMEVENT_COLLIDER:
					AnimColliderShow(m_spSelectAnim, flags, iter.second);
					break;
				case ANIMEVENT_EFFECT:
					break;
				case ANIMEVENT_OBJACTIVE:
					break;
				//case ANIMEVENT_ANIMOCCURSTIMEPASS:
				//	AnimOccursShow(m_spSelectAnim, flags, iter.second);
					break;
				case ANIMEVENT_SOUND:
					AnimSoundShow(m_spSelectAnim, flags, iter.second);
					break;
				}
			}
		}
		else
		{
			ImGui::Text("Plz select Animation"); 
		}
		ImGui::TreePop();
	}
}

void TAnimControlView::AnimSectionShow(CSHPTRREF<UAnimation> _spAnim, ImGuiTableFlags _flags,  const VECTOR<SHPTR<UAnimEvent>>& _AnimEvent)
{
	if (ImGui::TreeNodeEx("AnimChangesBetweenShow", ImGuiTreeNodeFlags_Bullet))
	{
		static _int SelectRemoveItem{ -1 };
		static _int iSelectAnim;

		if (true == ImGui::Button("Remove##1"))
		{
			if (-1 != SelectRemoveItem)
			{
				_spAnim->RemoveAnimEvent(ANIMEVENTTYPE::ANIMEVENT_ANIMCHANGESBETWEEN, SelectRemoveItem);
				SelectRemoveItem = -1;
			}
		}
		ImGui::SameLine();
		ImGui::Combo("NeedCopy", &iSelectAnim, &s_AnimTags[0], m_AnimMaxTagCount);
		ImGui::SameLine();
		if (true == ImGui::Button("Copy"))
		{
			m_spSelectAnim->CopyAnimEvent(ANIMEVENT_ANIMCHANGESBETWEEN, m_spShowAnimModel->GetAnimations()[iSelectAnim]);
		}

		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::BeginTable("AnimChangesBetween", 10, _flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 20), 0.0f))
		{
			ImGui::TableSetupColumn("Sel", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("InputTrigger", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("StartT", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("EndT", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("NextAnim", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("SupV", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("NextTA", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("ChangeT", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("LastSetting", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("LastValue", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
			ImGui::TableHeadersRow();

			static _float fNextTimeAcc{ 0.f };
			static _string Btn = "Sel##BTN";
			static _string InputTrigger = "##InputTrigger";
			static _string StartT = "##StartT";
			static _string EndT = "##EndT";
			static _string NextAnim = "##NextAnim";
			static _string SupV = "##SupV";
			static _string NextAnimTimeAcc = "##NextAnimTimeAcc";
			static _string ChangeT = "##ChangeT";
			static _string EnableLastSettingAnim = "##EnableLastSettingAnim";
			static _string EnableLastValue = "##EnableLastSettingValue";
			_int iIndex{ 0 };
			_float Duration = static_cast<_float>(_spAnim->GetDuration());
			for (auto& iter : _AnimEvent)
			{
				 _string Index = _string::to_string(iIndex);
				 ANIMEVENTSECTIONDESC* SectionDesc = remove_const<ANIMEVENTSECTIONDESC*, ANIMEVENTDESC*>(iter->OutAnimEventDesc());
				 ANIMCHANGEDESC* ChangeDesc = remove_const<ANIMCHANGEDESC*, ANIMOTHEREVENTDESC*>(iter->OutOtherEventDesc());

				 {
					 ImGui::TableNextColumn();
					 ImGui::SetNextItemWidth(-FLT_MIN);
					 if (true == ImGui::Button(Btn + Index))
					 {
						 SelectRemoveItem = iIndex;
					 }
				 }

				 _int iSelectAnim = ChangeDesc->iNextAnimIndex;
				 {
					 ImGui::TableNextColumn();
					 ImGui::SetNextItemWidth(-FLT_MIN);
					 _string str = UMethod::ConvertWToS(SectionDesc->wstrEventTrigger);
					 if (true == ImGui::InputText(InputTrigger + Index, &str[0], MAX_BUFFER_LENGTH))
					 {
						 SectionDesc->wstrEventTrigger = UMethod::ConvertSToW(str);
					 }
				 }
				 // Start End 
				 {
					 {
						 ImGui::TableNextColumn();
						 ImGui::SetNextItemWidth(-FLT_MIN);
						 _float StartTime = static_cast<_float>(SectionDesc->dStartTime);
						 ImGui::InputFloat(StartT + Index, &StartTime, 0.0f, Duration);
						 SectionDesc->dStartTime = static_cast<_double>(StartTime);
					 }
					 {
						 ImGui::TableNextColumn();
						 ImGui::SetNextItemWidth(-FLT_MIN);
						 _float EndTime = static_cast<_float>(SectionDesc->dEndTime);
						 ImGui::InputFloat(EndT + Index, &EndTime, 0.0f, Duration);
						 SectionDesc->dEndTime = static_cast<_double>(EndTime);
					 }
				 }
				 // Select Animation
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					if (true == ImGui::Combo(NextAnim + Index, &iSelectAnim, &s_AnimTags[0], m_AnimMaxTagCount))
					{
						ChangeDesc->iNextAnimIndex = m_spAnimControlModel->GetAnimationClips().find(s_AnimTags[iSelectAnim])->second;
						fNextTimeAcc = static_cast<_float>(m_spShowAnimModel->GetAnimations()[ChangeDesc->iNextAnimIndex]->GetTimeAcc());
					}
				}
				// SuppyAnim
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					ImGui::InputFloat(SupV + Index, &ChangeDesc->fSupplyAnimValue);
				}
				// NextTimeAcc
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					_float Time = static_cast<_float>(ChangeDesc->dNextAnimTimeAcc);
					ImGui::InputFloat(NextAnimTimeAcc + Index, &Time);
					ChangeDesc->dNextAnimTimeAcc = static_cast<_double>(Time);
				}					
				// Change Anim Time 
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					ImGui::InputFloat(ChangeT + Index, &ChangeDesc->fAnimChangeTime);
				}
				// Last Setting Anim Info
				{
					{
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::Checkbox(EnableLastSettingAnim + Index, &ChangeDesc->isEnableLastSettingAnim);
					}
					{
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::InputFloat(EnableLastValue + Index, &ChangeDesc->fLastProgressValue, 0.01f);
					}
				}
				++iIndex;
				ImGui::TableNextRow();
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}

void TAnimControlView::AnimColliderShow(CSHPTRREF<UAnimation> _spAnim, ImGuiTableFlags _flags, const VECTOR<SHPTR<UAnimEvent>>& _AnimEvent)
{
	static const _char* COLLIDERTAG[]{ "AABB", "OBB", "SPHERE"};
	static _string COLLIDER_NAME{ "MakeCollider##" };

	static _string InputTrigger = "InputTrigger";
	static _string StartT = "Start";
	static _string EndT = "End";
	static _string Collider = "##Collider3";
	static _string BoneNodeStr = "##BoneNode3";
	static _string CreateColliderButton = "Select";
	static _string ReadColliderPos = "CollPos";
	static _string TranslateCollider  = "Translater";
	static _string ScaleCollider = "Scale";
	static _string Remove{ "Remove" };
	static _int SelectRemoveItem{ -1 };
	static _int iSelectAnim;

	if (ImGui::TreeNodeEx("AnimColliderShow", ImGuiTreeNodeFlags_Bullet))
	{
		ImGui::Combo("NeedCopy", &iSelectAnim, &s_AnimTags[0], m_AnimMaxTagCount);
		ImGui::SameLine();
		if (true == ImGui::Button("Copy"))
		{
			m_spSelectAnim->CopyAnimEvent(ANIMEVENTTYPE::ANIMEVENT_COLLIDER, m_spShowAnimModel->GetAnimations()[iSelectAnim]);
		}

		_int iIndex{ 0 };
		_float Duration = static_cast<_float>(_spAnim->GetDuration());
		for (auto& iter : _AnimEvent)
		{
			_string Index = "##T3" + _string::to_string(iIndex);
			if (ImGui::TreeNodeEx(COLLIDER_NAME + Index))
			{
				ANIMEVENTSECTIONDESC* SectionDesc = remove_const<ANIMEVENTSECTIONDESC*, ANIMEVENTDESC*>(iter->OutAnimEventDesc());
				ANIMCOLLIDERDESC* ChangeDesc = remove_const<ANIMCOLLIDERDESC*, ANIMOTHEREVENTDESC*>(iter->OutOtherEventDesc());
				// 1
				{
					ImGui::SetNextItemWidth(300);
					_string str = UMethod::ConvertWToS(SectionDesc->wstrEventTrigger);
					if (true == ImGui::InputText(InputTrigger + Index, &str[0], MAX_BUFFER_LENGTH))
					{
						SectionDesc->wstrEventTrigger = UMethod::ConvertSToW(str);
					}
				}
				// 2
				{
					ImGui::SetNextItemWidth(100);
					_float StartTime = static_cast<_float>(SectionDesc->dStartTime);
					ImGui::InputFloat(StartT + Index, &StartTime, 0.0f, Duration);
					SectionDesc->dStartTime = static_cast<_double>(StartTime);
				}
				// 3
				{
					ImGui::SameLine();
					ImGui::SetNextItemWidth(100);
					_float EndTime = static_cast<_float>(SectionDesc->dEndTime);
					ImGui::InputFloat(EndT + Index, &EndTime, 0.0f, Duration);
					SectionDesc->dEndTime = static_cast<_double>(EndTime);
				}
				// 4
				{
					ImGui::SetNextItemWidth(100);
					_string BoneStr{ "NO_SEL" };
					if (nullptr != ChangeDesc->spBoneNode) {
						BoneStr = UMethod::ConvertWToS(ChangeDesc->spBoneNode->GetName());
					}
					if (ImGui::BeginCombo(BoneNodeStr + Index, BoneStr.c_str())) {

						_bool isTrue;
						static _string NO_SEL{ "NO_SEL" };
						if (ImGui::Selectable(NO_SEL + Index , &isTrue)) {
							ChangeDesc->spBoneNode = nullptr;
							ImGui::SetItemDefaultFocus();
						}
						else
						{
							for (auto& BoneNode : m_spShowAnimModel->GetBoneNodes())
							{
								_string ConvertBoneStr = UMethod::ConvertWToS(BoneNode->GetName());
								if (ImGui::Selectable(ConvertBoneStr.c_str(), &isTrue)) {
									ChangeDesc->spBoneNode = BoneNode;
									ImGui::SetItemDefaultFocus();
								}
							}
						}
						ImGui::EndCombo();
					}
				}
				// 5
				{
					ImGui::SameLine();
					ImGui::SetNextItemWidth(100);
					ImGui::Combo(Collider + Index, &ChangeDesc->iColliderType, COLLIDERTAG, 3);
				}
				// 6
				{
					ImGui::SameLine();
					ImGui::SetNextItemWidth(100);
					if (true == ImGui::Button(CreateColliderButton + Index))
					{
						ChangeDesc->spCollider = m_AnimEventColliderContainer.find(ChangeDesc->iColliderType)->second;
					}
				}
				if (nullptr != ChangeDesc->spCollider)
				{
					// 7 Show Pos
					{
						_float3 vCurPos = ChangeDesc->spCollider->GetCurPos();
						ImGui::InputFloat3(ReadColliderPos + Index, &vCurPos.x,"%.3f", ImGuiSliderFlags_ReadOnly);
					
					}
					// 8 
					{
						ImGui::DragFloat3(TranslateCollider + Index, &ChangeDesc->vColliderTranslation.x, 0.01f);
					//	ChangeDesc->spCollider->SetTranslate(vTranslate);
					}
					// 9
					{
						switch (ChangeDesc->iColliderType)
						{
						case UCollider::TYPE_SPHERE:
							ImGui::InputFloat(ScaleCollider + Index, &ChangeDesc->vColliderScale.x);
							break;
						case UCollider::TYPE_OBB:
						case UCollider::TYPE_AABB:
							ImGui::InputFloat3(ScaleCollider + Index, &ChangeDesc->vColliderScale.x);
							break;
						}
					}
				}
				if (ImGui::Button(Remove + Index))
				{
					SelectRemoveItem = iIndex;
				}
				++iIndex;
				ImGui::TreePop();
			}
		}
		if (-1 != SelectRemoveItem)
		{
			_spAnim->RemoveAnimEvent(ANIMEVENTTYPE::ANIMEVENT_COLLIDER, SelectRemoveItem);
			SelectRemoveItem = -1;
		}
		ImGui::TreePop();
	}
}

void TAnimControlView::AnimSoundShow(CSHPTRREF<UAnimation> _spAnim, ImGuiTableFlags _flags, const VECTOR<SHPTR<UAnimEvent>>& _AnimEvent)
{
	if (ImGui::TreeNodeEx("SoundEventList", ImGuiTreeNodeFlags_Bullet))
	{
		static _string StartT = "Start";
		static _string EndT = "End";
		static _string Velocity{ "SoundVelocity" };
		static _string Volume{ "Volume" };
		static _string MinDistance{ "MinDistance" };
		static _string MaxDistance{ "MaxDistance" };
		static _string Remove{ "Remove" };
		static _string InputSoundName;
		static 	ANIMSOUNDDESC* SelectSoundDesc{ nullptr };
		static _int SelectRemoveItem{ -1 };
		// Find Sounds 
		{

			SHPTR<UGameInstance> spGameInstance = GetGameInstance();
			SHPTR<UAudioSystem> spAudioSystem = spGameInstance->GetAudioSystem(SOUND_HARLEQUINN);

			if (true == ImGui::InputText("FindSName", &InputSoundName[0], MAX_BUFFER_LENGTH))
			{
				m_FindSoundNames.clear();
				// AudioSystem 리스트 보기 
				_wstring wstrConvertInputSoundName = UMethod::ConvertSToW(InputSoundName);
				for (auto& Sound : spAudioSystem->GetSoundOrders())
				{
					if (true == UMethod::Is_Same_Text(Sound.first, wstrConvertInputSoundName))
					{
						m_FindSoundNames.push_back(UMethod::ConvertWToS(Sound.first));
					}
				}
			}
			if (ImGui::BeginListBox("Sound List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto& iter : m_FindSoundNames)
				{
					if (true == ImGui::Selectable(iter))
					{
						if (nullptr != SelectSoundDesc)
						{
							SelectSoundDesc->wstrSoundName = UMethod::ConvertSToW(iter);
						}
					}
				}
				ImGui::EndListBox();
			}

		}
		// Modify
		{
			_int iIndex{ 0 };
			_float Duration = static_cast<_float>(_spAnim->GetDuration());
			static  _string ImGuiSoundNodes = "SoundEvent";
			for (auto& iter : _AnimEvent)
			{
				_string Index = "##T4" + _string::to_string(iIndex);

				if (true == ImGui::TreeNodeEx(ImGuiSoundNodes + Index, ImGuiTreeNodeFlags_Bullet))
				{
					ANIMEVENTSECTIONDESC* SectionDesc = remove_const<ANIMEVENTSECTIONDESC*, ANIMEVENTDESC*>(iter->OutAnimEventDesc());
					ANIMSOUNDDESC* ChangeDesc = remove_const<ANIMSOUNDDESC*, ANIMOTHEREVENTDESC*>(iter->OutOtherEventDesc());
					if (true == ImGui::Button("Selectable Sound"))
					{
						SelectSoundDesc = ChangeDesc;
					}

					// 1
					{
						ImGui::SetNextItemWidth(100);
						_float StartTime = static_cast<_float>(SectionDesc->dStartTime);
						ImGui::InputFloat(StartT + Index, &StartTime, 0.0f, Duration);
						SectionDesc->dStartTime = static_cast<_double>(StartTime);
					}
					// 2
					{
						ImGui::SameLine();
						ImGui::SetNextItemWidth(100);
						_float EndTime = static_cast<_float>(SectionDesc->dEndTime);
						ImGui::InputFloat(EndT + Index, &EndTime, 0.0f, Duration);
						SectionDesc->dEndTime = static_cast<_double>(EndTime);
					}
					// 3
					{
						ImGui::SetNextItemWidth(100);
						ImGui::Text("Select Sound Name: %s", UMethod::ConvertWToS(ChangeDesc->wstrSoundName).c_str());
					}
					// 4
					{
						ImGui::SetNextItemWidth(300);
						ImGui::InputFloat3(Velocity + Index, &ChangeDesc->vSoundVelocity.x);
					}
					// 5
					{
						ImGui::SetNextItemWidth(100);
						ImGui::InputFloat3(MaxDistance + Index, & ChangeDesc->fMaxSoundDistance);
						ImGui::SameLine();
						ImGui::SetNextItemWidth(100);
						ImGui::InputFloat3(MinDistance + Index, &ChangeDesc->fMinSoundDistance);
					}
					// 5
					{
						ImGui::SetNextItemWidth(100);
						ImGui::InputFloat(Volume + Index, &ChangeDesc->fSoundVolume);
					}
					if (ImGui::Button(Remove + Index))
					{
						SelectRemoveItem = iIndex;
					}
					++iIndex;
					ImGui::TreePop();
				}
			}

			if (-1 != SelectRemoveItem)
			{
				_spAnim->RemoveAnimEvent(ANIMEVENTTYPE::ANIMEVENT_SOUND, SelectRemoveItem);
				SelectRemoveItem = -1;
			}
		}
		ImGui::TreePop();
	}
}

void TAnimControlView::FindEquipModel(CSHPTRREF<FILEGROUP> _spFileGroup)
{
	RETURN_CHECK(nullptr == _spFileGroup, ;);

	for (auto& iter : _spFileGroup->UnderFileGroupList)
	{
		// FBX에서 Convert 라는 하위 폴더를 찾는다. 
		if (iter.second->wstrFolderName != L"Convert")
		{
			FindEquipModel(iter.second);
			continue;
		}

		for (auto& FileData : iter.second->FileDataList)
		{
			SHPTR<UModel> spModel = CreateConstructorNative<UModel>(GetDevice(), iter.second, FileData.second);
			m_EquipModelContainer.insert(MakePair(UMethod::ConvertWToS(FileData.first), spModel));
		}
	}
}


/*
* 폐기
* 
void TAnimControlView::AnimOccursShow(CSHPTRREF<UAnimation> _spAnim, ImGuiTableFlags _flags, const VECTOR<SHPTR<UAnimEvent>>& _AnimEvent)
{
	if (ImGui::TreeNodeEx("AnimOccursTimePassShow", ImGuiTreeNodeFlags_Bullet))
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::BeginTable("AnimOccursTimePass", 8, _flags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 20), 0.0f))
		{
			ImGui::TableSetupColumn("InputTrigger", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Over", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("NextAnim", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("SupV", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("NextTA", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("LastSetting", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("LastValue", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
			ImGui::TableHeadersRow();

			static _float fNextTimeAcc{ 0.f };

			static _string InputTrigger = "##InputTrigger2";
			static _string OverT = "##OverT2";
			static _string NextAnim = "##NextAnim2";
			static _string SupV = "##SupV2";
			static _string NextAnimTimeAcc = "##NextAnimTimeAcc2";
			static _string ChangeT = "##ChangeT2";
			static _string EnableLastSettingAnim2 = "##EnableLastSettingAnim2";
			static _string EnableLastValue2 = "##EnableLastSettingValue2";
			static _int SelectRemoveItem{ -1 };

			if (true == ImGui::Button("Remove##2"))
			{
				if (-1 != SelectRemoveItem)
				{
					_spAnim->RemoveAnimEvent(ANIMEVENTTYPE::ANIMEVENT_ANIMOCCURSTIMEPASS, SelectRemoveItem);
					SelectRemoveItem = -1;
				}
			}

			_int iIndex{ 0 };
			_float Duration = static_cast<_float>(_spAnim->GetDuration());
			for (auto& iter : _AnimEvent)
			{
				_string Index = _string::to_string(iIndex);

				ANIMOCURRESDESC* OccursDesc = remove_const<ANIMOCURRESDESC*, ANIMEVENTDESC*>(iter->OutAnimEventDesc());
				ANIMCHANGEDESC* ChangeDesc = remove_const<ANIMCHANGEDESC*, ANIMOTHEREVENTDESC*>(iter->OutOtherEventDesc());

				_int iSelectAnim = ChangeDesc->iNextAnimIndex;
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					_string str = UMethod::ConvertWToS(OccursDesc->wstrEventTrigger);
					if (true == ImGui::InputText(InputTrigger + Index, &str[0], MAX_BUFFER_LENGTH))
					{
						OccursDesc->wstrEventTrigger = UMethod::ConvertSToW(str);
					}
				}
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					_float Time = static_cast<_float>(OccursDesc->dAnimOccursTime);
					ImGui::InputFloat(OverT + Index, &Time, 0.0f, Duration);
					OccursDesc->dAnimOccursTime = static_cast<_double>(Time);
				}
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					if (true == ImGui::Combo(NextAnim + Index, &iSelectAnim, &s_AnimTags[0], m_AnimMaxTagCount))
					{
						ChangeDesc->iNextAnimIndex = m_spAnimControlModel->GetAnimationClips().find(s_AnimTags[iSelectAnim])->second;
						fNextTimeAcc = static_cast<_float>(m_spShowAnimModel->GetAnimations()[ChangeDesc->iNextAnimIndex]->GetTimeAcc());
					}
				}
				// Supply Anim Value
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					ImGui::InputFloat(SupV + Index, &ChangeDesc->fSupplyAnimValue);
				}
				// NextAnimTimeAcc
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					_float Time = static_cast<_float>(ChangeDesc->dNextAnimTimeAcc);
					ImGui::InputFloat(NextAnimTimeAcc + Index, &Time);
					ChangeDesc->dNextAnimTimeAcc = static_cast<_double>(Time);
				}
				// Change Anim Time
				{
					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					ImGui::InputFloat(ChangeT + Index, &ChangeDesc->fAnimChangeTime);
				}
				// Last Setting Anim Info
				{
					{
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::Checkbox(EnableLastSettingAnim2 + Index, &ChangeDesc->isEnableLastSettingAnim);
					}
					{
						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::InputFloat(EnableLastValue2 + Index, &ChangeDesc->fLastProgressValue, 0.01f);
					}
				}

				if (ImGui::IsItemClicked())
					SelectRemoveItem = iIndex;
				++iIndex;
				ImGui::TableNextRow();
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}

*/