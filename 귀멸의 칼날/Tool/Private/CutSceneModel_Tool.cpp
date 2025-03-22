#include "CutSceneModel_Tool.h"
#include "GameInstance.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "Channel.h"
#include "Animator.h"
#include "AnimationClip.h"
#include "BonesCollider.h"


#pragma region CUTSCENEANIM

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Add_AnimationClip()
{
	if (nullptr == pOwnerAnimator)
		return;

	if (nullptr == pFileData)
		return;

	_string str = CMethod::ConvertWToS(pFileData->strFileName);
	str.append(" Add");
	if (ImGui::TreeNode(str.c_str()))
	{
		_char NAME[MAX_PATH] = "";
		strcpy_s(NAME, strFindName.c_str());
		_bool bFind = ImGui::InputText("Find Anim Clip", NAME, sizeof(_char) * MAX_PATH);
		strFindName = NAME;

		if ("" == strFindName)
		{
			if (ImGui::BeginListBox("CutSceneAnimation Add List", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto& iter : pOwnerAnimator->Get_AnimationClipList())
				{
					if (ImGui::Selectable(iter->Get_Name().c_str()))
					{
						pSelectAnimClip = iter;
						if (nullptr != iter)
						{
							pOwnerAnimator->Set_CurrentAnimationClip(iter);
							pOwnerAnimator->Set_Play(true);
						}
					}
				}

				ImGui::EndListBox();
			}
		}
		else
		{
			if (true == bFind)
			{
				FindAnimList.clear();
				for (auto& iter : pOwnerAnimator->Get_AnimationClipList())
				{
					if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strFindName.c_str()))
						FindAnimList.push_back(iter);
				}
			}
			else
			{
				if (ImGui::BeginListBox("CutSceneAnimation Add List", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
				{
					for (auto& iter : FindAnimList)
					{
						if (ImGui::Selectable(iter->Get_Name().c_str()))
						{
							pSelectAnimClip = iter;
							if (nullptr != iter)
							{
								pOwnerAnimator->Set_CurrentAnimationClip(iter);
								pOwnerAnimator->Set_Play(true);
							}
						}
					}

					ImGui::EndListBox();
				}
			}
		}

		if (nullptr == pSelectAnimClip)
		{
			ImGui::TreePop();
			return;
		}

		if (ImGui::Button("Add CutSceneAnimClip"))
		{
			for (auto& iter : AnimClipVector)
				if (iter == pSelectAnimClip)
				{
					ImGui::TreePop();
					return;
				}

			AnimClipVector.push_back(pSelectAnimClip);
			Calculate_TotalPlayTime();
			pSelectAnimClip = nullptr;
		}

		ImGui::TreePop();
	}
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Update_AnimClip(const _float& _fTimeAcc)
{
	if (ClipOrder.size() > 1)
	{
		for (_uint i = 0; i < ClipOrder.size(); ++i)
		{
			if (_fTimeAcc >= ClipOrder[i].dTime - 0.001f)
			{
				pCurAnimClip = ClipOrder[i].pAnimClip;
			}
		}

		if (nullptr == pCurAnimClip)
			return;

		if (nullptr != pOwnerAnimator)
		{
			if (pPrevAnimClip != pCurAnimClip)
			{
				pOwnerAnimator->Set_CurrentAnimationClip(pCurAnimClip->Get_Name());
				pOwnerAnimator->Set_Play(true);
				pPrevAnimClip = pCurAnimClip;
			}
		}
	}
	else
	{
		for (_uint i = 0; i < ClipOrder.size(); ++i)
		{
			if (_fTimeAcc >= ClipOrder[i].dTime)
			{
				pCurAnimClip = ClipOrder[i].pAnimClip;
			}
		}

		if (nullptr == pCurAnimClip)
			return;

		if (nullptr != pOwnerAnimator)
		{
			if (pPrevAnimClip != pCurAnimClip)
			{
				pOwnerAnimator->Set_CurrentAnimationClip(pCurAnimClip->Get_Name());
				pOwnerAnimator->Set_Play(true);
				pPrevAnimClip = pCurAnimClip;
			}
		}
	}
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Control_AnimClip(const _float& _fTimeAcc)
{
	if (ClipOrder.size() <= 0)
		return;

	if (ClipOrder.size() <= 1.f)
	{
		_double dTime = 0.;
		for (_uint i = 0; i < ClipOrder.size(); ++i)
		{
			if (_fTimeAcc >= ClipOrder[i].dTime)
			{
				pCurAnimClip = ClipOrder[i].pAnimClip;
				dTime += ClipOrder[i].dTime;
			}
		}

		if (nullptr == pCurAnimClip)
			return;

		if (nullptr != pOwnerAnimator)
		{
			if (pPrevAnimClip != pCurAnimClip)
			{
				pOwnerAnimator->Set_CurrentAnimationClip(pCurAnimClip->Get_Name());
				pPrevAnimClip = pCurAnimClip;
			}
			else
			{
				_float fRatio = (_float)((_fTimeAcc - dTime) / (pCurAnimClip->Get_Duration() / pCurAnimClip->Get_TickPerSecond()));
				if (fRatio >= 0.f)
					pOwnerAnimator->Set_Ratio(fRatio);
			}
			pOwnerAnimator->Set_Play(true);
		}
	}
	else
	{
		_double dTime = 0.;
		for (_uint i = 0; i < ClipOrder.size(); ++i)
		{
			if (_fTimeAcc >= ClipOrder[i].dTime)
			{
				pCurAnimClip = ClipOrder[i].pAnimClip;
				dTime += ClipOrder[i].dTime;
			}
		}

		if (nullptr == pCurAnimClip)
			return;

		if (nullptr != pOwnerAnimator)
		{
			if (pPrevAnimClip != pCurAnimClip)
			{
				pOwnerAnimator->Set_NextDirectAnimationClip(pCurAnimClip->Get_Name());
				pPrevAnimClip = pCurAnimClip;
			}
			else
			{
				_float fRatio = (_float)((_fTimeAcc - dTime) / (pCurAnimClip->Get_Duration() / pCurAnimClip->Get_TickPerSecond()));
				if (fRatio >= 0.f)
					pOwnerAnimator->Set_Ratio(fRatio);
			}
			pOwnerAnimator->Set_Play(true);
		}
	}
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::RootBoneTransform()
{
	if (nullptr == pOwnerAnimator || nullptr == pOwnerTransform)
		return;

	if (false == pOwnerAnimator->Is_RootBone())
		return;

	pOwnerTransform->Set_Pos(pOwnerAnimator->Get_RootBoneMatrix().r[3] + vSavePosition.Get_Vector());
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::ShowList()
{
	if (nullptr == pFileData)
		return;

	_string str = CMethod::ConvertWToS(pFileData->strFileName);
	str.append("-ShowList");
	vector<ORDERANIM> ANIM = ClipOrder;
	if (ImGui::TreeNode(str.c_str()))
	{
		_uint i = 0;
		for (auto& iter : ClipOrder)
		{
			if (nullptr != iter.pAnimClip)
			{
				if (ImGui::TreeNode(iter.pAnimClip->Get_Name().c_str()))
				{
					pOwnerAnimator->Set_CurrentAnimationClip(iter.pAnimClip);
					pOwnerAnimator->Set_Play(true);

					ImGui::Text(CMethod::Convert_Number_S(iter.dTime, 3).c_str());
					if (nullptr != iter.pAnimClip)
						ImGui::Text(CMethod::Convert_Number_S(iter.pAnimClip->Get_Duration() / iter.pAnimClip->Get_TickPerSecond(), 3).c_str());
					_float fTime = (_float)(iter.dTime);
					ImGui::DragFloat("Time Edit", &fTime, 0.01f, 0.f, 100000.f);
					iter.dTime = (_double)(fTime);
					ImGui::TreePop();
				}
			}
			else
			{
				if (ImGui::TreeNode(CMethod::Convert_Number_S(i).c_str()))
				{
					if (ImGui::TreeNode("Change"))
					{
						_char NAME[MAX_PATH] = "";
						strcpy_s(NAME, strFindName.c_str());
						_bool bFind = ImGui::InputText("Find Anim Clip", NAME, sizeof(_char) * MAX_PATH);
						strFindName = NAME;

						if ("" == strFindName)
						{
							if (ImGui::BeginListBox("CutSceneAnimation Add List", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
							{
								for (auto& iter : pOwnerAnimator->Get_AnimationClipList())
								{
									if (ImGui::Selectable(iter->Get_Name().c_str()))
									{
										pSelectAnimClip = iter;
										if (nullptr != iter)
										{
											pOwnerAnimator->Set_CurrentAnimationClip(iter);
											pOwnerAnimator->Set_Play(true);
										}
									}
								}
								ImGui::EndListBox();
							}
						}
						else
						{
							if (true == bFind)
							{
								FindAnimList.clear();
								for (auto& iter : pOwnerAnimator->Get_AnimationClipList())
								{
									if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), strFindName.c_str()))
										FindAnimList.push_back(iter);
								}
							}
							else
							{
								if (ImGui::BeginListBox("CutSceneAnimation Add List", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
								{
									for (auto& iter : FindAnimList)
									{
										if (ImGui::Selectable(iter->Get_Name().c_str()))
										{
											pSelectAnimClip = iter;
											if (nullptr != iter)
											{
												pOwnerAnimator->Set_CurrentAnimationClip(iter);
												pOwnerAnimator->Set_Play(true);
											}
										}
									}
									ImGui::EndListBox();
								}
							}
						}
						if (nullptr != pSelectAnimClip)
						{
							if (ImGui::Button("Change Clip"))
							{
								_bool bIsTrue = true;
								for (auto& iter : AnimClipVector)
									if (iter == pSelectAnimClip)
									{
										bIsTrue = false;
										break;
									}
								if (true == bIsTrue)
								{
									iter.pAnimClip = pSelectAnimClip;
									pSelectAnimClip = nullptr;

									AnimClipVector.clear();
									for (auto& iter : ClipOrder)
									{
										if (nullptr != iter.pAnimClip)
											AnimClipVector.push_back(iter.pAnimClip);
									}
								}
							}
						}
						ImGui::TreePop();
					}

					ImGui::Text(CMethod::Convert_Number_S(iter.dTime, 3).c_str());
					_float fTime = (_float)(iter.dTime);
					ImGui::DragFloat("Time Edit", &fTime, 0.01f, 0.f, 100000.f);
					iter.dTime = (_double)(fTime);
					ImGui::TreePop();
				}
			}
			++i;
		}

		ImGui::NewLine();
		if (ImGui::Button("Reset Time"))
		{
			Calculate_TotalPlayTime();
		}
		ImGui::TreePop();
	}
}

CAnimationClip* CCutSceneModel_Tool::tagCutSceneAnimDesc::Get_AnimClip(const _float& _fTimeAcc)
{
	CAnimationClip* pClip = nullptr;
	for (auto& iter : AnimClipVector)
		if (_fTimeAcc <= iter->Get_Duration())
			pClip = iter;

	return pClip;
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Calculate_TotalPlayTime()
{
	_double dTime = 0.f;
	ClipOrder.clear();
	for (auto& iter : AnimClipVector)
	{
		if (nullptr == iter)
			continue;

		ORDERANIM tAnim;
		tAnim.pAnimClip = iter;
		tAnim.dTime = dTime;
		ClipOrder.push_back(tAnim);
		dTime += iter->Get_Duration() / iter->Get_TickPerSecond();
	}

	dTotalPlayTime = dTime;
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Just_Calculate_TotalPlayTime()
{
	_double dTime = 0.f;
	for (auto& iter : ClipOrder)
		dTime += iter.dTime;

	dTotalPlayTime = dTime;
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Remove_AnimClip()
{
	if (nullptr == pFileData)
		return;

	_string str = CMethod::ConvertWToS(pFileData->strFileName);
	str.append(" Delete");
	if (ImGui::TreeNode(str.c_str()))
	{
		if (ImGui::BeginListBox("CutSceneAnimation Delete List", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& iter : AnimClipVector)
			{
				if (nullptr != iter)
				{
					if (ImGui::Selectable(iter->Get_Name().c_str()))
					{
						pDeleteAnimClip = iter;
					}
				}
			}

			ImGui::EndListBox();
		}

		if (nullptr == pDeleteAnimClip)
		{
			ImGui::TreePop();
			return;
		}

		if (ImGui::Button("Delete AnimClip To List"))
		{
			for (vector<CAnimationClip*>::iterator it = AnimClipVector.begin(); it != AnimClipVector.end(); ++it)
			{
				if ((*it) == pDeleteAnimClip)
				{
					AnimClipVector.erase(it);
					pDeleteAnimClip = nullptr;
					Calculate_TotalPlayTime();
					break;
				}
			}
		}

		ImGui::TreePop();
	}
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Reset()
{
	pCurAnimClip = nullptr;
	pPrevAnimClip = nullptr;
	bInitStart = false;
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Reset_All()
{
	// Total Play Time
	dTotalPlayTime = 0.;
	// AnimClip Vector 
	AnimClipVector.clear();
	pCurAnimClip = nullptr;
	pPrevAnimClip = nullptr;
	pNextAnimClip = nullptr;
	pSelectAnimClip = nullptr;
	pDeleteAnimClip = nullptr;

	strFindName = "";
	FindAnimList.clear();
	ClipOrder.clear();
	bInitStart = false;
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::End_Anim()
{
	if (nullptr != pOwnerAnimator)
	{
		pOwnerAnimator->Set_Play(false);
	}
}

void CCutSceneModel_Tool::tagCutSceneAnimDesc::Receive_ClipOrder(vector<ORDERANIM> _AnimOrderClip)
{
	ClipOrder.clear();
	for (auto& iter : _AnimOrderClip)
	{
		ORDERANIM tAnim;
		tAnim.dTime = iter.dTime;
		tAnim.pAnimClip = nullptr;
		ClipOrder.push_back(tAnim);
	}
}

#pragma endregion CUTSCENEANIM

CCutSceneModel_Tool::CCutSceneModel_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CGameObject_Tool(_pDevice, _pContext)
{
}

CCutSceneModel_Tool::CCutSceneModel_Tool(const CCutSceneModel_Tool& _rhs)
	: CGameObject_Tool(_rhs)
{
}

const char* CCutSceneModel_Tool::Get_Name()
{
	return "CutSceneModel_Tool";
}

const _tchar* CCutSceneModel_Tool::Get_Proto()
{
	return PROTO_GOB_CUTSCENEMODEL_TOOL;
}

const _tchar* CCutSceneModel_Tool::Get_Layer()
{
	return LAYER_TEST;
}

HRESULT CCutSceneModel_Tool::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCutSceneModel_Tool::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_bActive = false;
	return S_OK;
}

void CCutSceneModel_Tool::Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;

	if (false == m_bCutScenePlay)
	{
		__super::Tick(_dTimeDelta);
		if (nullptr != m_pAnimatorCom)
			m_pAnimatorCom->Tick(_dTimeDelta);
	}
}

void CCutSceneModel_Tool::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;
	if (false == m_bCutScenePlay)
	{
		__super::Late_Tick(_dTimeDelta);
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CCutSceneModel_Tool::Render()
{
	if (FAILED(SetUp_ConstantValue()))
		return E_FAIL;

	// 메쉬 인덱스를 받아와서 렌더한다.
	// 모델이 없다면 Render하지 않는다.
	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				continue;

			m_pModelCom->Bind_SRV(m_pShaderCom, "g_OutLineTexture", i, TextureType_SPECULAR);

			if(true == m_pAnimatorCom->Is_MeshActive())
				m_pModelCom->Render(i, m_pShaderCom, 0);
		}
	}
	return S_OK;
}
// Load Cut Scene Model
HRESULT CCutSceneModel_Tool::Load_CutSceneModel(FILEGROUP* _pModelFileGroup, FILEDATA* _pModelFileData)
{
	if (nullptr == _pModelFileGroup && nullptr == _pModelFileData)
		return E_FAIL;

	m_pModelCom = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, _pModelFileGroup, _pModelFileData,
		XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(XMConvertToRadians(180.0f)));

	if (nullptr == m_pModelCom)
		return E_FAIL;

	m_pModelCom->Set_SetSupplementAnim(false);
	PATHS paths = { L"Data", L"Animator", _pModelFileData->strFileName };
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		FILEGROUP* AnimatorGroup = pGameInstance->Find_Folder(&paths);

		if (nullptr != AnimatorGroup)
		{
			CAnimator::ANIMATORDESC tDesc;
			tDesc.pModel = m_pModelCom;
			tDesc.strFilePath = _pModelFileData->strFileName;
			tDesc.pTransform = m_pTransformCom;

			if (FAILED(Add_Component(COM_ANIMATOR, LEVEL_STATIC,
				PROTO_COMP_ANIMATOR, (CComponent**)&m_pAnimatorCom, &tDesc)))
			{
				Safe_Release(m_pModelCom);
				return E_FAIL;
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);

	m_pFileData = _pModelFileData;
	m_tCutSceneAnimDesc.pOwnerAnimator = m_pAnimatorCom;
	m_tCutSceneAnimDesc.pOwnerTransform = m_pTransformCom;
	m_tCutSceneAnimDesc.pFileData = m_pFileData;
	return S_OK;
}

void CCutSceneModel_Tool::Cut_Tick(const _double& _dTimeDelta)
{
	if (true == m_bCutScenePlay)
	{
		__super::Tick(_dTimeDelta);
		if (nullptr != m_pAnimatorCom)
			m_pAnimatorCom->Tick(_dTimeDelta);
	}
}

void CCutSceneModel_Tool::Cut_Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;
	__super::Late_Tick(_dTimeDelta);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

void CCutSceneModel_Tool::Save(HANDLE& _hFile, DWORD* _pByte)
{
	_uint iSize = (_uint)m_tCutSceneAnimDesc.AnimClipVector.size();
	WriteFile(_hFile, &iSize, sizeof(_uint), _pByte, nullptr);
	for (auto& iter : m_tCutSceneAnimDesc.AnimClipVector)
	{
		if(nullptr != iter)
		CMethod::Save_File_String(_hFile, *_pByte, iter->Get_Name());
	}
	iSize = (_uint)m_tCutSceneAnimDesc.ClipOrder.size();
	WriteFile(_hFile, &iSize, sizeof(_uint), _pByte, nullptr);
	for (auto& iter : m_tCutSceneAnimDesc.ClipOrder)
	{
		if (nullptr != iter.pAnimClip)
			CMethod::Save_File_String(_hFile, *_pByte, iter.pAnimClip->Get_Name());
		else
			CMethod::Save_File_String(_hFile, *_pByte, "NULL");

		WriteFile(_hFile, &iter.dTime, sizeof(_double), _pByte, nullptr);

	}
	
	if (m_tCutSceneAnimDesc.ClipOrder.size() > 0)
	{
		if (nullptr != m_tCutSceneAnimDesc.ClipOrder.back().pAnimClip)
		{
			_double dLastTime = m_tCutSceneAnimDesc.ClipOrder.back().pAnimClip->Get_Duration()
				/ m_tCutSceneAnimDesc.ClipOrder.back().pAnimClip->Get_TickPerSecond();
			WriteFile(_hFile, &dLastTime, sizeof(_double), _pByte, nullptr);
		}
	}
}

void CCutSceneModel_Tool::Load(HANDLE& _hFile, DWORD* _pByte)
{
	list<_string> LIST;
	_uint iSize = 0;
	ReadFile(_hFile, &iSize, sizeof(_uint), _pByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		_string str = CMethod::Load_File_String(_hFile, *_pByte);
		LIST.push_back(str);
	}
	for (auto& iter : LIST)
	{
		CAnimationClip* Clip = m_pAnimatorCom->Get_AnimationClip(iter);
		m_tCutSceneAnimDesc.AnimClipVector.push_back(Clip);
	}
	iSize = (_uint)m_tCutSceneAnimDesc.ClipOrder.size();
	ReadFile(_hFile, &iSize, sizeof(_uint), _pByte, nullptr);
	for (_uint i = 0; i < iSize; ++i)
	{
		_string Read = "";
		Read = CMethod::Load_File_String(_hFile, *_pByte);
		_double dTime;
		ReadFile(_hFile, &dTime, sizeof(_double), _pByte, nullptr);
		ORDERANIM tAnim;
		if ("NULL" != Read)
		{
			CAnimationClip* Clip = m_pAnimatorCom->Get_AnimationClip(Read);
			tAnim.pAnimClip = Clip;
		}
		tAnim.dTime = dTime;
		m_tCutSceneAnimDesc.ClipOrder.push_back(tAnim);
		m_tCutSceneAnimDesc.dTotalPlayTime += dTime;
	}
	_double dLastTime = 0.0;
	ReadFile(_hFile, &dLastTime, sizeof(_double), _pByte, nullptr);
	m_tCutSceneAnimDesc.dTotalPlayTime += dLastTime;
}

// Ready Component
HRESULT CCutSceneModel_Tool::Ready_Component()
{
	if (FAILED(Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER,
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL,
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}
// SetUp Constant Value
HRESULT CCutSceneModel_Tool::SetUp_ConstantValue()
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

	m_PrevWorldMatrix = WorldMatrixTP;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

#pragma region USE_TOOL_METHOD
void CCutSceneModel_Tool::Push_Trigger(const _string& _strTrigger)
{
	if (nullptr == m_pAnimatorCom)
		return;

	m_pAnimatorCom->Input_Trigger(_strTrigger);
}

_bool CCutSceneModel_Tool::Extern_EditValue()
{
	return true;
}

_bool CCutSceneModel_Tool::Extern_HierakeyView(const string& _str)
{
	if (nullptr == m_pTransformCom || nullptr == m_pAnimatorCom)
		return false;

	{
		// Position
		_float3 v3Position = m_pTransformCom->Get_PosF3();
		_float fArrValue[3] = { v3Position.x, v3Position.y, v3Position.z };
		ImGui::DragFloat3("CusSceneModel_Position", fArrValue, 0.05f, -1000000.f, 1000000.f);
		v3Position = _float3(fArrValue[0], fArrValue[1], fArrValue[2]);
		m_pTransformCom->Set_PosF3(v3Position);
	}
	{
		// Rotation
		_bool bIsTrue = ImGui::DragFloat3("CusSceneModel_Rotation", &m_v3Rotation.x, 0.05f, -1000000.f, 1000000.f);
		if (true == bIsTrue)
		{
			m_pTransformCom->Rotate_Turn(m_v3Rotation);
		}
	}

	return true;
}

HRESULT CCutSceneModel_Tool::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CCutSceneModel_Tool::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CCutSceneModel_Tool::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CCutSceneModel_Tool::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CCutSceneModel_Tool::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CCutSceneModel_Tool::Reset_Data()
{
}

void CCutSceneModel_Tool::Show_AnimationClip()
{
	if (nullptr == m_pAnimatorCom)
		return;

	m_tCutSceneAnimDesc.Add_AnimationClip();
	m_tCutSceneAnimDesc.Remove_AnimClip();
	m_tCutSceneAnimDesc.ShowList();
}

#pragma endregion USE_TOOL_METHOD

CCutSceneModel_Tool* CCutSceneModel_Tool::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CCutSceneModel_Tool* pInstance = new CCutSceneModel_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCutSceneModel_Tool::Clone(void* pArg)
{
	CCutSceneModel_Tool* pInstance = new CCutSceneModel_Tool(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCutSceneModel_Tool::Free()
{
	Safe_Release(m_pAnimatorCom);
	Safe_Release(m_pModelCom);
	__super::Free();
}
