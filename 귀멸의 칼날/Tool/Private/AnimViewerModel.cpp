#include "AnimViewerModel.h"
#include "GameInstance.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "Channel.h"
#include "Animator.h"
#include "AnimationClip.h"
#include "BonesCollider.h"

CAnimViewerModel::CAnimViewerModel(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	:CGameObject_Tool(_pDevice, _pContext)
{
}

CAnimViewerModel::CAnimViewerModel(const CAnimViewerModel& _rhs)
	:CGameObject_Tool(_rhs)
{
}

const _string& CAnimViewerModel::Get_CurrentAnimationName()
{
	return m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimationName();
}

const _uint& CAnimViewerModel::Get_CurretnAnimationIndex()
{
	return m_iCurrentAnimationIndex;
}

const _double& CAnimViewerModel::Get_AnimationDuration()
{
	// 현재 재생중인 애니메이션의 재생시간을 받아온다.
	return m_pModelCom->Get_CurAnimation()->Get_Duration();
}

const _double& CAnimViewerModel::Get_AnimationPlayTime()
{
	// 현재 재생중인 애니메이션의 재생시간을 받아온다.
	return m_pModelCom->Get_CurAnimation()->Get_TimeAcc();
}

const _double& CAnimViewerModel::Get_TickPerSecond()
{
	return m_pModelCom->Get_CurAnimation()->Get_TickPerSecond();
}

_double CAnimViewerModel::Get_AnimationRatio()
{
	return m_pModelCom->Get_CurAnimation()->Get_Ratio();
}

vector<CHierarchyNode*> CAnimViewerModel::Get_HierarchyNodeList()
{
	return m_pModelCom->Get_HierachyNodeList();
}

vector<CChannel*> CAnimViewerModel::Get_ChannelList()
{
	return m_pAnimatorCom->Get_CurrentAnimationClip()->Get_MainAnimation()->Get_Channels();
}

vector<CMeshContainer*> CAnimViewerModel::Get_MeshContainerList()
{
	return m_pModelCom->Get_MeshContainers();
}

void CAnimViewerModel::Set_Stop()
{
	if (nullptr == m_pModelCom)
		return;

	// 플레이를 false로 전환
	m_bIsPlay = false;
	// 현재 재생중이었던 애니메이션으로 다시 애니메이션 교체
	m_pModelCom->Set_Animation(m_iCurrentAnimationIndex);
	// 뼈값을 초기화
	m_pModelCom->Update_Animation(0.0);
}

void CAnimViewerModel::Set_ProgressRatio(_double Ratio)
{
	if (nullptr == m_pAnimatorCom)
		return;

	// 플레이를 false로 전환
	m_bIsPlay = false;

	// 애니메이터의 재생 설정을 변경한다.
	m_pAnimatorCom->Set_Play(false);

	// 애니메이션의 진행도를 변경한다.
	m_pAnimatorCom->Set_Ratio(Ratio);	
}

void CAnimViewerModel::Play_OneFrame(_double TimeDelta)
{
	// 플레이를 false로 전환
	m_bIsPlay = false;

	// 뼈값을 업데이트한다.
	if (0 < m_pModelCom->Get_CurAnimation()->Get_TimeAcc() + TimeDelta)
		m_pModelCom->Update_Animation(TimeDelta);
	else if (0 >= m_pModelCom->Get_CurAnimation()->Get_TimeAcc() + TimeDelta)
	{
#ifdef _DEBUG
		m_pModelCom->Get_CurAnimation()->Set_TimeAcc(0.0);
#endif
		m_pModelCom->Update_Animation(0.0);
	}
}

void CAnimViewerModel::Add_AnimationClip(CAnimationClip* pClip)
{
	if (nullptr == pClip)
		return;

	// 애니메이션 클립을 넣어준다.
	m_AnimClips.push_back(pClip);
}

HRESULT CAnimViewerModel::Set_Model(CModel* pModel)
{
	if (nullptr == pModel)
		return E_FAIL;

	// 원래 모델이 있었다면 삭제한다. ( Ref Count 증가를 하지 않았기 때문에 여기서 Clone된 모델 객체들을 지운다. )
	//Safe_Release(m_pModelCom);

	// 모델을 교체한다.
	m_pModelCom = pModel;

	// 애니메이터 컴포넌트가 nullptr이 아니라면 애니메이터가 갖고있는 모델또한 교체한다.
	if (nullptr != m_pAnimatorCom)	
		m_pAnimatorCom->Set_Model(m_pModelCom);		
		
	return S_OK;
}

HRESULT CAnimViewerModel::Set_Animator(CAnimator* pAnimator)
{
	if (nullptr == pAnimator)
		return E_FAIL;

	// 이전에 있던 애니메이터 컴포넌트는 지워준다.
	Safe_Release(m_pAnimatorCom);
	 
	// 애니메이터 컴포넌트를 인자로 들어온 객체와 교체한다.
	m_pAnimatorCom = pAnimator;

	Safe_AddRef(m_pAnimatorCom);

	// 애니메이터 컴포넌트에 현재 객체가 가진 트랜스폼 컴포넌트를 넘겨준다.
	m_pAnimatorCom->Set_Transform(this->m_pTransformCom);
	
	return S_OK;
}

HRESULT CAnimViewerModel::Set_Animation(_uint iAnimIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (iAnimIndex >= m_pModelCom->Get_Animations().size())
		return E_FAIL;

	// 모델 객체의 애니메이션을 교체한다.
	m_pModelCom->Set_Animation(iAnimIndex);

	// 멤버변수로 갖고있는 애니메이션 인덱스 변수의 값을 변경한다.
	m_iCurrentAnimationIndex = iAnimIndex;

	// 애니메이션을 선택했다면 해당 애니메이션으로 뼈값을 업데이트한다.
	m_pModelCom->Update_Animation(0.0);

	// 애니메이션의 재생을 멈춘다.
	m_bIsPlay = false;

	return S_OK;
}

HRESULT CAnimViewerModel::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CAnimViewerModel::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	// 컴포넌트 세팅
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CCollider::COLLIDERDESC tDesc;
	tDesc.vScale = _float3(1.f, 1.f, 1.f);
	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		m_pSphereCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(LEVEL_STATIC,
			PROTO_COMP_SPHERECOLLIDER, &tDesc));

		m_pCubeCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(LEVEL_STATIC,
			PROTO_COMP_OBBCOLLIDER, &tDesc));

		tDesc.vTranslation = _float3(0.f, 5.f, 0.f);
		tDesc.vScale = _float3(2.f, 2.f, 2.f);

		m_pAttackColliderCom = static_cast<CCollider*>(pGameInstance->Clone_Component(LEVEL_STATIC,
			PROTO_COMP_OBBCOLLIDER, &tDesc));
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CAnimViewerModel::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pModelCom)
		return;

	__super::Tick(_dTimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyDown(DIK_W))
	{
		m_pAnimatorCom->Input_Trigger("Run");
	}
	if (pGameInstance->Get_DIKeyDown(DIK_A))
	{
		m_pAnimatorCom->Input_Trigger("Run");
	}
	if (pGameInstance->Get_DIKeyDown(DIK_S))
	{
		m_pAnimatorCom->Input_Trigger("Run");
	}
	if (pGameInstance->Get_DIKeyDown(DIK_D))
	{
		m_pAnimatorCom->Input_Trigger("Run");
	}

	if (pGameInstance->Get_DIKeyDown(DIK_K))
	{
		m_pAnimatorCom->Input_Trigger("Jump");
	}
	
	if (pGameInstance->Get_DIKeyUp(DIK_W))
	{
		m_pAnimatorCom->Input_Trigger("Run_Stop");
	}
	if (pGameInstance->Get_DIKeyUp(DIK_A))
	{
		m_pAnimatorCom->Input_Trigger("Run_Stop");
	}
	if (pGameInstance->Get_DIKeyUp(DIK_S))
	{
		m_pAnimatorCom->Input_Trigger("Run_Stop");
	}
	if (pGameInstance->Get_DIKeyUp(DIK_D))
	{
		m_pAnimatorCom->Input_Trigger("Run_Stop");
	}

	RELEASE_INSTANCE(CGameInstance);


	if (nullptr != m_pAnimatorCom)
	{
		m_pAnimatorCom->Tick(_dTimeDelta);
	}

#ifdef _DEBUG
	if (nullptr != m_pSelectNode && nullptr != m_pModelCom)
	{
		if (0 == m_iSelectType)
		{
			m_pCubeCollider->Tick(m_pModelCom->Get_PivotMatrix() *  m_pSelectNode->Get_CombinedTransformationMatrix()
				* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix());
		}
		else
		{
			m_pSphereCollider->Tick(XMMatrixScaling(0.1f, 0.1f, 0.1f) * m_pSelectNode->Get_CombinedTransformationMatrix()
				* m_pModelCom->Get_PivotMatrix() *m_pTransformCom->Get_WorldMatrix());
		}

		for (auto& iter : m_BonesCollider)
			iter->MultiplyCollider(m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix());
	}
#endif
}

void CAnimViewerModel::Late_Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pRendererCom && nullptr == m_pAnimatorCom)
		return;

	__super::Late_Tick(_dTimeDelta);

	if (m_pAnimatorCom->Is_Play())
	{
		if (true == m_pAnimatorCom->Is_RootBone())
		{
			_matrix vRootBoneMatrix = m_pAnimatorCom->Get_RootBoneMatrix();
			// 위치값을 RootBoneMatrix로 넣어준다 ( 회전값도 나중에 얻어와야한다. )
			m_pTransformCom->Set_Pos(CMethod::XMMatrixGetVector(vRootBoneMatrix, MATROW_POS));

			//_float3 fCurPos = m_pAnimatorCom->Get_RootBoneMatrix().r[3];
		}
		if (true == m_pAnimatorCom->Is_BoneTransform())
		{
			_vector BoneTransformPos = m_pAnimatorCom->Get_BoneTransformPos();
			m_pTransformCom->Set_Pos(m_pTransformCom->Get_Pos() + BoneTransformPos);
		}
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	if (nullptr != m_pAttackColliderCom)
	{
		m_pAttackColliderCom->Set_Translate(m_pTransformCom->Get_Pos() + m_pAnimatorCom->Get_ColliderPos());
		m_pAttackColliderCom->Set_Scale(m_pAnimatorCom->Get_ColliderScale());
	}
}

HRESULT CAnimViewerModel::Render()
{
#ifdef _DEBUG
	if (0 == m_iSelectType)
	{
		m_pCubeCollider->Render_Debug();
	}
	else
	{
		m_pSphereCollider->Render_Debug();
	}

	for (auto& iter : m_BonesCollider)
		iter->Get_Collider()->Render_Debug();

	m_pAttackColliderCom->Render_Debug();
#endif

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderValue()))
		return E_FAIL;

	// 메쉬 인덱스를 받아와서 렌더한다.
	// 모델이 없다면 Render하지 않는다.
	if (nullptr != m_pModelCom)
	{

		if (0 != m_MeshList.size())
		{
			for (auto& meshIndex : m_MeshList)
			{
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", meshIndex, TextureType_DIFFUSE)))
					return E_FAIL;
				
				m_pModelCom->Bind_SRV(m_pShaderCom, "g_OutLineTexture", meshIndex, TextureType_SPECULAR);

				m_pModelCom->Render(meshIndex, m_pShaderCom, 0);
			}
		}
		else
		{
			_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

			for (_uint i = 0; i < iNumMeshContainers; ++i)
			{
				if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
					 continue;

				m_pModelCom->Bind_SRV(m_pShaderCom, "g_OutLineTexture", i, TextureType_SPECULAR);

				if (true == m_pAnimatorCom->Is_MeshActive())
				{
					m_pModelCom->Render(i, m_pShaderCom, 0);
				}
			}
		}
	}
	return S_OK;
}

HRESULT CAnimViewerModel::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimViewerModel::SetUp_ShaderValue()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 WorldMatrixTP = m_pTransformCom->Get_WorldFloat4x4_TP();
	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
	_float4x4 PrevViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PREVVIEW);
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrixTP, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_PrevWorldMatrix", &m_PrevWorldMatrix, sizeof(_float4x4))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_PrevViewMatrix", &PrevViewMatrix, sizeof(_float4x4))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	SETUP_CONSTANCE_VALUE(m_pShaderCom, "g_bWrapSampler", m_bWrapSampler);

	m_PrevWorldMatrix = WorldMatrixTP;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CAnimViewerModel* CAnimViewerModel::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CAnimViewerModel* pInstance = new CAnimViewerModel(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAnimViewerModel::Failed To Create");
	}
	return pInstance;
}

CGameObject* CAnimViewerModel::Clone(void* pArg)
{
	CAnimViewerModel* pInstance = new CAnimViewerModel(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAnimViewerModel::Failed To Clone");
	}
	return pInstance;
}

void CAnimViewerModel::Free()
{
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pSphereCollider);
	Safe_Release(m_pCubeCollider);
	Safe_Release(m_pAttackColliderCom);

	for (auto& iter : m_BonesCollider)
		Safe_Release(iter);

	__super::Free();
}

const char* CAnimViewerModel::Get_Name()
{
	return "Test";
}

const _tchar* CAnimViewerModel::Get_Proto()
{
	return nullptr;
}

const _tchar* CAnimViewerModel::Get_Layer()
{
	return nullptr;
}
#pragma region USE_TOOL_METHOD
_bool CAnimViewerModel::Extern_EditValue()
{
	return _bool();
}

_bool CAnimViewerModel::Extern_HierakeyView(const string& _str)
{
	if (nullptr != m_pModelCom)
	{
		const _char* pArr[] = { "OBB", "SP" };
		ImGui::Combo("Select Collider Type", (int*)&m_iSelectType, pArr, 2);

		if (0 == m_iSelectType)
		{
			if (nullptr != m_pCubeCollider)
			{
				CCollider::COLLIDERDESC tCollData;
				tCollData.vScale = m_pCubeCollider->Get_Scale();
				tCollData.vTranslation = m_pCubeCollider->Get_Translate();

				_float Translate[3] = { tCollData.vTranslation.x,  tCollData.vTranslation.y, tCollData.vTranslation.z };
				_float CollSclae[3] = { tCollData.vScale.x ,  tCollData.vScale.y , tCollData.vScale.z };

				if (ImGui::DragFloat3("Bone CollT##001", Translate, 1, -100, 100, "%.3f"))
				{
					m_pCubeCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
				}
				if (ImGui::DragFloat3("Bone  CollS##001", CollSclae, 1, 0, 1000, "%.3f"))
				{
					m_pCubeCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
				}
			}
		}
		else
		{
			if (nullptr != m_pSphereCollider)
			{
				CCollider::COLLIDERDESC tCollData;
				tCollData.vScale = m_pSphereCollider->Get_Scale();
				tCollData.vTranslation = m_pSphereCollider->Get_Translate();

				_float Translate[3] = { tCollData.vTranslation.x,  tCollData.vTranslation.y, tCollData.vTranslation.z };
				_float CollSclae[3] = { tCollData.vScale.x ,  tCollData.vScale.y , tCollData.vScale.z };

				if (ImGui::DragFloat3("Bone CollT##001", Translate, 1, -100, 100, "%.3f"))
				{
					m_pSphereCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
				}
				if (ImGui::DragFloat3("Bone  CollS##001", CollSclae, 1, 0, 1000, "%.3f"))
				{
					m_pSphereCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
				}
			}
		}

		_char INPUT[MAX_PATH] = "";
		strcpy_s(INPUT, m_strText.c_str());
		_bool bInput = false;
		bInput = ImGui::InputText("Bones", INPUT, sizeof(_char) * MAX_PATH);
		m_strText = INPUT;

		if (!strcmp(m_strText.c_str(), ""))
		{
			if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto& iter : m_pModelCom->Get_HierachyNodeList())
				{
					if (ImGui::Selectable(iter->Get_Name().c_str()))
					{
						m_pSelectNode = iter;
					}
				}
				ImGui::EndListBox();
			}
		}
		else if (true == bInput)
		{
			m_SaveToFindBoneList.clear();
			for (auto& iter : m_pModelCom->Get_HierachyNodeList())
			{
				if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), m_strText.c_str()))
					m_SaveToFindBoneList.push_back(iter);
			}
		}
		else
		{
			if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto& iter : m_SaveToFindBoneList)
				{
					if (ImGui::Selectable(iter->Get_Name().c_str()))
					{
						m_pSelectNode = iter;
					}
				}
				ImGui::EndListBox();
			}
		}

		if (ImGui::Button("Add Collider"))
		{
			for (auto& iter : m_BonesCollider)
			{
				if (iter->Get_HierachyNode() == m_pSelectNode)
					return false;
			}

			if (0 == m_iSelectType)
			{
				CBonesCollider* pData = CBonesCollider::Create();
				CCollider::COLLIDERDESC tDesc;
				tDesc.vScale = m_pCubeCollider->Get_Scale();
				tDesc.vTranslation = m_pCubeCollider->Get_Translate();
				pData->Add_Colliders(m_pModelCom, m_pSelectNode->Get_Name().c_str(),
					LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, &tDesc);

				m_BonesCollider.push_back(pData);

			}
			else
			{
				CBonesCollider* pData = CBonesCollider::Create();
				CCollider::COLLIDERDESC tDesc;
				tDesc.vScale = m_pSphereCollider->Get_Scale();
				tDesc.vTranslation = m_pSphereCollider->Get_Translate();
				pData->Add_Colliders(m_pModelCom, m_pSelectNode->Get_Name().c_str(),
					LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER, &tDesc);

				m_BonesCollider.push_back(pData);
			}
		}
		if (ImGui::Button("Save Collider"))
		{
			FILEGROUP* pFileGroup = m_pModelCom->Get_FileGroup();
			FILEDATA* pFileData = m_pModelCom->Get_FileData();
			if (nullptr != pFileGroup && nullptr != pFileData)
			{
				for (auto& iter : m_BonesCollider)
					iter->Save(m_pModelCom, pFileGroup);
			}
		}
		if (ImGui::Button("Load Collider"))
		{
			FILEGROUP* pFileGroup = m_pModelCom->Get_FileGroup();
			FILEDATA* pFileData = m_pModelCom->Get_FileData();
			if (nullptr != pFileGroup && nullptr != pFileData)
			{
				if (nullptr == m_pModelCom)
					return false;

				FILEGROUP* pColliderGroup = pFileGroup->Find_Group(L"BonesCollider");
				if (nullptr == pColliderGroup)
					return false;

				for (auto& iter : pColliderGroup->vecFileDataList)
				{
					CBonesCollider* pBonesCollider = CBonesCollider::Create();
					pBonesCollider->Load(m_pModelCom, &iter, g_iCurrentLevel);
					m_BonesCollider.push_back(pBonesCollider);
				}
			}
		}

		if (ImGui::BeginListBox("##listbox 3", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& iter : m_BonesCollider)
			{
				if (ImGui::Selectable(iter->Get_HierachyNode()->Get_Name().c_str()))
				{
					m_pBonesSelectCollider = iter;
				}
			}
			ImGui::EndListBox();
		}

		if (nullptr != m_pBonesSelectCollider)
		{
			if (nullptr != m_pBonesSelectCollider->Get_Collider())
			{
				CCollider::COLLIDERDESC tCollData;
				tCollData.vScale = m_pBonesSelectCollider->Get_Collider()->Get_Scale();
				tCollData.vTranslation = m_pBonesSelectCollider->Get_Collider()->Get_Translate();

				_float Translate[3] = { tCollData.vTranslation.x,  tCollData.vTranslation.y, tCollData.vTranslation.z };
				_float CollSclae[3] = { tCollData.vScale.x ,  tCollData.vScale.y , tCollData.vScale.z };

				if (ImGui::DragFloat3("Bone CollT##002", Translate, 1, -100, 100, "%.3f"))
				{
					m_pBonesSelectCollider->Get_Collider()->Set_Translate(_float3(Translate[0], 
						Translate[1], Translate[2]));
				}
				if (ImGui::DragFloat3("Bone  CollS##002", CollSclae, 1, 0, 1000, "%.3f"))
				{
					m_pBonesSelectCollider->Get_Collider()->Set_Scale(_float3(CollSclae[0], 
						CollSclae[1], CollSclae[2]));
				}
			}
		}

		if (ImGui::Button("Delete Collider"))
		{
			for (vector<CBonesCollider*>::iterator it = m_BonesCollider.begin();
				it != m_BonesCollider.end();)
			{
				if ((*it) == m_pBonesSelectCollider)
				{
					Safe_Release(m_pBonesSelectCollider);
					m_BonesCollider.erase(it);
					break;
				}
				else
					++it;
			}
		}
	}

	ImGui::Checkbox("WrapSampler", &m_bWrapSampler);
	return _bool();
}

HRESULT CAnimViewerModel::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CAnimViewerModel::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CAnimViewerModel::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CAnimViewerModel::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CAnimViewerModel::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CAnimViewerModel::Reset_Data()
{
}
#pragma endregion USE_TOOL_METHOD