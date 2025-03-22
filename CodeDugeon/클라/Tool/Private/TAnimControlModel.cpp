#include "ToolDefines.h"
#include "TAnimControlModel.h"
#include "UAnimModel.h"
#include "UTransform.h"
#include "UShader.h"
#include "UMethod.h"
#include "UAnimation.h"
#include "UTexGroup.h"
#include "TEquipModel.h"
#include "UGameInstance.h"
#include "UBoneNode.h"

TAnimControlModel::TAnimControlModel(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer,
	const CLONETYPE& _eCloneType) : 
	UPawn(_spDevice, _wstrLayer, _eCloneType), m_spModel{nullptr}, m_spModelFolder{nullptr}, m_spCurAnimation{nullptr},
	m_isAnimationStop{false}, m_fAnimTimeAcc{0.f}, m_fTotalAnimFastvalue{1.f},
	m_isAnimEventActive{false},
	m_wstrImguiModifyInputTrigger{L""}, 
	m_wstrInputTrigger{L""},
	m_spSelectedEquipModel{nullptr},
	m_isSelectedEquipModel{false}
{
}

TAnimControlModel::TAnimControlModel(const TAnimControlModel& _rhs) : 
	UPawn(_rhs),
m_spModel{ nullptr }, m_spModelFolder{ nullptr }, m_spCurAnimation{ nullptr }, 
m_isAnimationStop{ false }, m_fAnimTimeAcc{ 0.f }, m_fTotalAnimFastvalue{ 1.f },
m_isAnimEventActive{ false },
m_wstrImguiModifyInputTrigger{ L"" },
m_wstrInputTrigger{ L"" },
m_spSelectedEquipModel{ nullptr },
m_isSelectedEquipModel{ false }
{
}


void TAnimControlModel::Free()
{
}

HRESULT TAnimControlModel::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT TAnimControlModel::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	AddShader(PROTO_RES_ANIMMODELSHADER, RES_SHADER);
//	GetTransform()->SetScale({ 0.05f, 0.05f, 0.05f });
	m_wstrImguiModifyInputTrigger.resize(MAX_BUFFER_LENGTH);
	return S_OK;
}

void TAnimControlModel::ReleaseShowModel()
{
	m_spModel.reset();
	m_spModelFolder.reset();
	m_AnimationClips.clear();
	m_isAnimationStop = false;
}

void TAnimControlModel::SetShowModel(CSHPTRREF<UAnimModel> _spModel, CSHPTRREF<FILEGROUP> _spFileFolder)
{
	RETURN_CHECK(nullptr == _spModel, ;);
	RETURN_CHECK(nullptr == _spFileFolder, ;);
	m_spModel = _spModel;
	m_spModelFolder = _spFileFolder;
	m_AnimationClips.clear();
	for (auto& iter : m_spModel->GetAnimStrings())
	{
		m_AnimationClips.insert(MakePair(UMethod::ConvertWToS(iter.first), iter.second));
		m_FindAnimClips.insert(MakePair(UMethod::ConvertWToS(iter.first), iter.second));
	}
	for (auto& iter : m_spModel->GetBoneNodes())
	{
		m_FindBoneNodeContainer.push_back(iter);
	}
	m_isAnimationStop = false;
	m_fAnimTimeAcc = 0.f;
	m_fTotalAnimFastvalue = 0.f;
	m_isAnimEventActive = false;

	m_spModel->OnShowOriginAnimation();
	SetActive(true);
}

void TAnimControlModel::ShowAnimModify()
{
	SelectAnimation();
	ModifyAnimation();
}

void TAnimControlModel::SelectBoneNodeName(OUT _wstring& _wstrBoneNameName)
{
	static _char InputBoneName[MAX_BUFFER_LENGTH];
	if (true == ImGui::InputText("InputBoneNode", InputBoneName, MAX_BUFFER_LENGTH))
	{
		m_FindBoneNodeContainer.clear();
		for (auto& iter : m_spModel->GetBoneNodes())
		{
			_string str = UMethod::ConvertWToS(iter->GetName());
			if (true == UMethod::Is_Same_Text(str, InputBoneName))
			{
				m_FindBoneNodeContainer.push_back(iter);
			}
		}
	}
	else
	{
		if (m_FindBoneNodeContainer.empty())
		{
			if (!strcmp(InputBoneName, ""))
			{
				for (auto& iter : m_spModel->GetBoneNodes())
				{
					m_FindBoneNodeContainer.push_back(iter);
				}
			}
		}
	}

	if (ImGui::BeginListBox("BoneNodeName", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (auto& BoneNode : m_FindBoneNodeContainer)
		{
			_bool isTrue{ false };
			_string str = UMethod::ConvertWToS(BoneNode->GetName());
			if (ImGui::Selectable(str.c_str(), &isTrue))
			{
				if (true == m_isSelectedEquipModel)
				{
					if (nullptr != m_spSelectedEquipModel)
					{
						m_spSelectedEquipModel->ChangeBoneNode(BoneNode);
					}
				}
				else
				{
					_wstrBoneNameName = BoneNode->GetName();
				}
			}
		}
		ImGui::EndListBox();
	}

	if (false == _wstrBoneNameName.empty())
	{
		ImGui::Text(UMethod::ConvertWToS(_wstrBoneNameName));
	}
	else
	{
		ImGui::Text("Don't Selected BoneNode");
	}
}

void TAnimControlModel::SelectEquip()
{
	if (ImGui::BeginListBox("Equip", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
	{
		static _string EQUIP{ "Equip_" };
		_int iIndex{ 0 };
		for (auto& iter : m_EquipModelContainer)
		{
			_string str = EQUIP + _string::to_string(iIndex++);
			if (ImGui::Selectable(str.c_str()))
			{
				m_spSelectedEquipModel = iter;
			}
		}
		ImGui::EndListBox();
	}

	ImGui::Checkbox("EquipSelected", &m_isSelectedEquipModel);

	if (nullptr != m_spSelectedEquipModel)
	{
		_float4x4 mTargetPivotMatrix = m_spSelectedEquipModel->GetPivotMatrix();
		_float3 vPosition = mTargetPivotMatrix.Get_Pos();
		_float3 vScale = mTargetPivotMatrix.Get_Scaling();
		
		if (true == ImGui::DragFloat3("EquipPos", &vPosition.x, 0.01f))
		{
			mTargetPivotMatrix.Set_Pos(vPosition);
		}
		if (true == ImGui::DragFloat3("EquipScale", &vScale.x, 0.01f))
		{
			mTargetPivotMatrix = mTargetPivotMatrix.MatrixSetScaling(vScale);
		}
		m_spSelectedEquipModel->SetPivotMatrix(mTargetPivotMatrix);
	}
}

void TAnimControlModel::MakeEquip(CSHPTRREF<UModel> _spEquipModel, const EQUIPTYPE _EquipType, 
	const _wstring& _wstrBoneNodeName, _int _iWeaponOrShieldJudgeValue)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	TEquipModel::EQDESC desc;

	SHPTR<TEquipModel> spEquipModel = std::static_pointer_cast<TEquipModel>(spGameInstance->CloneActorAdd(PROTO_ACTOR_EQUIPMENT));
	spEquipModel->ChangeOwner(ThisShared<UPawn>());
	spEquipModel->UpdateBoneNode(_EquipType, m_spModel, _spEquipModel, _wstrBoneNodeName, _iWeaponOrShieldJudgeValue);
	m_EquipModelContainer.push_back(spEquipModel);
}

void TAnimControlModel::SelectAnimation()
{
	ImGui::Text("SelectAnimation");
	if (ImGui::Button("AnimTransformReset"))
	{
		GetTransform()->SetPos(_float3::Zero);
		GetTransform()->RotateFix(_float3::Zero);
	}
	static _char InputAnim[MAX_BUFFER_LENGTH];
	ImGui::SetNextItemWidth(-FLT_MIN);

	if (true == ImGui::InputText("InputAnim", InputAnim, MAX_BUFFER_LENGTH))
	{
		m_FindAnimClips.clear();
		for (auto& AnimClips : m_AnimationClips)
		{
			if (true == UMethod::Is_Same_Text(AnimClips.first, InputAnim))
			{
				m_FindAnimClips.emplace(MakePair(AnimClips.first, AnimClips.second));
			}
		}
	}
	else
	{
		if (m_FindAnimClips.empty())
		{
			if (!strcmp(InputAnim, ""))
			{
				for (auto& AnimClips : m_AnimationClips)
				{
					m_FindAnimClips.emplace(MakePair(AnimClips.first, AnimClips.second));
				}
			}
		}
	}


	ImGui::SetNextItemWidth(-FLT_MIN);
	if (ImGui::BeginListBox("Select AnimationControl", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (auto& AnimClip : m_FindAnimClips)
		{
			_bool isTrue{ false };
			if (ImGui::Selectable(AnimClip.first.c_str(), &isTrue))
			{
				m_spModel->SetAnimation(AnimClip.second);
			}
		}
		ImGui::EndListBox();
	}


}

void TAnimControlModel::ModifyAnimation()
{
	if (nullptr != m_spModel)
	{
		SHPTR<UAnimation> spAnimation = m_spModel->GetCurrentAnimation();
		if (m_spCurAnimation != spAnimation)
		{
			m_spCurAnimation = spAnimation;
			m_fTotalAnimFastvalue = m_spCurAnimation->GetTotalAnimFastValue();
			m_AnimFastSections = m_spCurAnimation->GetAnimFastSection();	
		}
		_float AnimDuration = (_float)m_spCurAnimation->GetDuration();
		_bool isClicked = false;
		// Button And Slider
		{
			m_fAnimTimeAcc = static_cast<_float>(m_spCurAnimation->GetTimeAcc());
			ImGui::Checkbox("AnimEventActive", &m_isAnimEventActive);
			if (true == m_isAnimEventActive)
			{
				m_spModel->OnAdjustTransformToAnimation();
			}
			else
			{
				m_spModel->OnShowOriginAnimation();
			}
			ImGui::InputFloat("TotalFastValue", &m_fTotalAnimFastvalue, 0.01f);
			isClicked = ImGui::SliderFloat("DeltaTime", &m_fAnimTimeAcc, 0.f, AnimDuration);
			// Input Trigger
			{
				_string str = UMethod::ConvertWToS(m_wstrImguiModifyInputTrigger);
				ImGui::InputText("##InputTrigger", &str[0], MAX_BUFFER_LENGTH);
				m_wstrImguiModifyInputTrigger = UMethod::ConvertSToW(str);
				if (true == ImGui::Button("SetTrigger")) {
					m_wstrInputTrigger = m_wstrImguiModifyInputTrigger;
				}
			}

			if (true == isClicked) {
				m_isAnimationStop = isClicked;
			}
			if (ImGui::Button("AnimStart"))
			{
				m_isAnimationStop = false;
			}
			ImGui::SameLine();
			if (true == ImGui::Button("AnimStop"))
			{
				m_isAnimationStop = true;
			}
		}
		if (ImGui::TreeNodeEx("ModifyAnimation", ImGuiTreeNodeFlags_Bullet))
		{
			// Fast Section Node
			{
				if (ImGui::TreeNodeEx("FastSectionNode", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Bullet))
				{
					if (ImGui::Button("MakeFastSection")) {
						m_AnimFastSections.push_back({});
					}
					ImGui::SameLine();
					if (ImGui::Button("ApplyFastSection")) {
						m_spCurAnimation->UpdateAnimFastSections(m_fTotalAnimFastvalue, m_AnimFastSections);
					}
					ImGui::SameLine();
					if (ImGui::Button("ClearSection")) {
						m_AnimFastSections.clear();
					}
					if (ImGui::Button("LoadSection")) {
						m_spCurAnimation->LoadAnimSectionDataPathIsFolder(m_spModelFolder->wstrPath);
					}
					ImGui::SameLine();
					if (ImGui::Button("Save FastSection"))
					{
						m_spCurAnimation->UpdateAnimFastSections(m_fTotalAnimFastvalue, m_AnimFastSections);
						m_spCurAnimation->SaveAnimSectionPathIsFolder(m_spModelFolder->wstrPath);
					}
					// Options
					static ImGuiTableFlags flags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
						| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
						| ImGuiTableFlags_ScrollY ;

					if (ImGui::BeginTable("FastSection", 3, flags, ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 10), 0.0f))
					{
						ImGui::TableSetupColumn("StartSpot");
						ImGui::TableSetupColumn("EndSpot");
						ImGui::TableSetupColumn("FastValue");
						ImGui::TableHeadersRow();
						_int iIndex{ 0 };
						for (auto& iter : m_AnimFastSections)
						{
							static _string Start = "##S";
							static _string End = "##E";
							static _string Fast = "##F";
							_string Index =std::to_string(iIndex++);

							ImGui::TableNextColumn();
							ImGui::SetNextItemWidth(-FLT_MIN);
							ImGui::DragFloat((Start + Index).c_str(), &iter.fStartSpot, 0.01f, 0.f, AnimDuration);
							ImGui::TableNextColumn();
							ImGui::SetNextItemWidth(-FLT_MIN);
							ImGui::DragFloat((End + Index).c_str(), &iter.fEndSpot, 0.01f, 0.f, AnimDuration);
							ImGui::TableNextColumn();
							ImGui::SetNextItemWidth(-FLT_MIN);
							ImGui::DragFloat((Fast + Index).c_str(), &iter.fFastValue, 0.01f, 0.f, AnimDuration);
							ImGui::TableNextRow();
						}
						ImGui::EndTable();
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
	else
	{
		ImGui::Text("Plz Load To Animation Model");
	}
}

void TAnimControlModel::TickActive(const _double& _dTimeDelta)
{
	if (nullptr != m_spModel)
	{
		if (false == m_isAnimEventActive)
		{
			if (false == m_isAnimationStop)
			{
				m_spModel->TickAnimation(_dTimeDelta);
			}
			else
			{
				m_spModel->UpdateCurAnimationToRatio(static_cast<_double>(m_fAnimTimeAcc));
			}
		}
		else
		{
			if (false == m_isAnimationStop)
			{
				m_spModel->TickAnimChangeTransform(GetTransform(), _dTimeDelta);
				m_spModel->TickEvent(this, m_wstrInputTrigger, _dTimeDelta);
			}
			else
			{
				m_spModel->TickAnimToTimeAccChangeTransform(GetTransform(), _dTimeDelta, static_cast<_double>(m_fAnimTimeAcc));
				m_spModel->TickEvent(this, m_wstrInputTrigger, _dTimeDelta);
			}
			m_wstrInputTrigger = L"";
		}
	}
}

void TAnimControlModel::LateTickActive(const _double& _dTimeDelta)
{
	if (nullptr != m_spModel)
	{
		AddRenderGroup(RI_NONALPHA_MIDDLE);
	}
}

HRESULT TAnimControlModel::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	if (nullptr != m_spModel)
	{
		__super::RenderActive(_spCommand, _spTableDescriptor);

		for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
		{
			// Bind Transform 
			GetTransform()->BindTransformData(GetShader());

			m_spModel->BindTexture(i, SRV_REGISTER::T0, TEXTYPE::TextureType_DIFFUSE, GetShader());
			m_spModel->BindTexture(i, SRV_REGISTER::T1, TEXTYPE::TextureType_NORMALS, GetShader());
			// Render
			m_spModel->Render(i, GetShader(), _spCommand);
		}
	}
	return S_OK;
}

HRESULT TAnimControlModel::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	//if (nullptr != m_spModel)
	//{
	//	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	//	for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
	//	{
	//		// Bind Transform 
	//		GetTransform()->BindTransformData(GetShader());

	//		m_spModel->BindTexture(i, SRV_REGISTER::T0, TEXTYPE::TextureType_DIFFUSE, GetShader());
	//		m_spModel->BindTexture(i, SRV_REGISTER::T1, TEXTYPE::TextureType_NORMALS, GetShader());
	//		// Render
	//		m_spModel->Render(i, GetShader(), _spCommand);
	//	}
	//}
	return S_OK;
}
HRESULT TAnimControlModel::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	//if (nullptr != m_spModel)
	//{
	//	__super::RenderShadowActive(_spCommand, _spTableDescriptor);

	//	for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
	//	{
	//		// Bind Transform 
	//		GetTransform()->BindTransformData(GetShader());

	//		m_spModel->BindTexture(i, SRV_REGISTER::T0, TEXTYPE::TextureType_DIFFUSE, GetShader());
	//		m_spModel->BindTexture(i, SRV_REGISTER::T1, TEXTYPE::TextureType_NORMALS, GetShader());
	//		// Render
	//		m_spModel->Render(i, GetShader(), _spCommand);
	//	}
	//}
	return S_OK;
}
void TAnimControlModel::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}