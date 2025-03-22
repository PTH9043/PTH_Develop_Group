#include "CMainView.h"
#include "CObjectTool.h"
#include "CImGui_Manager.h"
#include "CCharacter.h"
#include "CGameInstance.h"
#include "CWeapon.h"
#include "CData_Manager.h"
#include "CStateMachine.h"
#include <direct.h>

#pragma region BONDESCOLLIDERDESC
void CCharacter::tagBonesColliderDesc::Add_Colliders(CModel* _pModel, const _char* _pNodeNames,
	const LEVEL& _eLevel, const _tchar* _pTag, void* _pArg)
{
	if (nullptr == _pModel)
		return;

	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		pHierachyNode = _pModel->Get_HierarchyNodes(_pNodeNames);
		Safe_AddRef(pHierachyNode);
	
		pCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(_eLevel, _pTag, _pArg));
	}
	END_SINGLETON(pGameInstance);
}

_matrix CCharacter::tagBonesColliderDesc::MultiplyCollider(_matrix WorldMatrix)
{
	if (nullptr == pCollider)
		return XMMatrixIdentity();

	_matrix MultipleWorld = pHierachyNode->Get_CombinedTransformationMatrix() * WorldMatrix;

	pCollider->Tick(MultipleWorld);
	return MultipleWorld;
}

_bool CCharacter::tagBonesColliderDesc::Is_Collision(CCollider* _pCollider)
{
	if (nullptr == pCollider || nullptr == _pCollider)
		return false;

	if (false == bIsCollision)
		return false;

	if (pCollider->Is_Collision(_pCollider))
		return true;

	return false;
}

_bool CCharacter::tagBonesColliderDesc::Is_Collision(const char* _pName, CCollider* _pCollider)
{
	if (nullptr == pCollider || nullptr == _pCollider || nullptr == pHierachyNode)
		return false;

	if (false == bIsCollision)
		return false;

	if (!strcmp(pHierachyNode->Get_Name().c_str(), _pName))
	{
		if (true == bIsCollision)
			return true;
	}

	return false;
}

HRESULT CCharacter::tagBonesColliderDesc::Save(CModel* _pModel, FILEGROUP* _pFileGroup)
{
	if (nullptr == _pFileGroup || nullptr == _pModel)
		return E_FAIL;

	_wstring str;
	str.assign(pHierachyNode->Get_Name().begin(), pHierachyNode->Get_Name().end());

	_wstring Path = _pFileGroup->strPath;
	_wstring FolderPath = Path;
	FolderPath.append(L"\\BonesCollider");
	_wmkdir(FolderPath.c_str());

	FolderPath.append(L"\\");
	FolderPath.append(str);
	FolderPath.append(L".dat");

	HANDLE hFile = CreateFile(FolderPath.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	DWORD dwByte = 0;
	{
		_uint iLength = ((_uint)strlen(pHierachyNode->Get_Name().c_str()) + 1) * sizeof(_char);
		WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, pHierachyNode->Get_Name().c_str(), iLength, &dwByte, nullptr);
	}
	WriteFile(hFile, &pCollider->Get_Type(), sizeof(CCollider::TYPE), &dwByte, nullptr);
	WriteFile(hFile, &pCollider->Get_Scale(), sizeof(_float3), &dwByte, nullptr);
	WriteFile(hFile, &pCollider->Get_Translate(), sizeof(_float3), &dwByte, nullptr);

	WriteFile(hFile, &bIsCollision, sizeof(_bool), &dwByte, nullptr);
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CCharacter::tagBonesColliderDesc::Load(CModel* _pModel, FILEDATA* _pFileData, const LEVEL& _eLevel)
{
	if (nullptr == _pModel || nullptr == _pFileData)
		return E_FAIL;

	HANDLE hFile = CreateFile(_pFileData->strFilePath.c_str(),
		GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return E_FAIL;
	}
	string strName = "";
	DWORD dwByte = 0;
	{
		_uint iLength = 0;
		ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
		_char* pName = new _char[iLength];
		ReadFile(hFile, pName, iLength, &dwByte, nullptr);
		strName = pName;
		Safe_Delete_Array(pName);
	}
	CCollider::TYPE eType;
	_float3 v3Scale;
	_float3 v3Translate;
	ReadFile(hFile, &eType, sizeof(CCollider::TYPE), &dwByte, nullptr);
	ReadFile(hFile, &v3Scale, sizeof(_float3), &dwByte, nullptr);
	ReadFile(hFile, &v3Translate, sizeof(_float3), &dwByte, nullptr);
	ReadFile(hFile, &bIsCollision, sizeof(_bool), &dwByte, nullptr);

	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		_wstring _strTag = L"";
		switch (eType)
		{
		case CCollider::TYPE_OBB:
			_strTag = PROTO_COMP_OBBCOLLIDER;
			break;
		case CCollider::TYPE_SPHERE:
			_strTag = PROTO_COMP_SPHERECOLLIDER;
			break;
		}

		if (!lstrcmp(_strTag.c_str(), L""))
		{
			END_SINGLETON(pGameInstance);
			CloseHandle(hFile);
			return E_FAIL;
		}
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = v3Scale;
		tCollDesc.vTranslation = v3Translate;
		Add_Colliders(_pModel, strName.c_str(), LEVEL_STATIC, _strTag.c_str(), &tCollDesc);
	}
	END_SINGLETON(pGameInstance);

	CloseHandle(hFile);
	return S_OK;
}

#ifdef  _DEBUG
void CCharacter::tagBonesColliderDesc::Render()
{
	if (nullptr == pCollider)
		return;

	pCollider->Render();
}
#endif //  _DEBUG

void CCharacter::tagBonesColliderDesc::Release()
{
	Safe_Release(pCollider);
	Safe_Release(pHierachyNode);
}
#pragma endregion BONDESCOLLIDERDESC

CCharacter::CCharacter(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPawn(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_DrawAnimMatrix, XMMatrixIdentity());
}

CCharacter::CCharacter(const CCharacter& _rhs)
	: CPawn(_rhs)
{
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_DrawAnimMatrix, XMMatrixIdentity());
}

_vector CCharacter::Get_NodePosition(const NODES& _eNode)
{
	if (nullptr == m_pTransformCom || nullptr == m_pNodeArr[_eNode])
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_matrix matWorld =
		m_pNodeArr[_eNode]->Get_CombinedTransformationMatrix() *
		m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	return matWorld.r[3];
}

_matrix CCharacter::Get_NodeMatrix(const NODES& _eNode)
{
	if (nullptr == m_pTransformCom || nullptr == m_pNodeArr[_eNode])
		return XMMatrixIdentity();
	_matrix matWorld =
		m_pNodeArr[_eNode]->Get_CombinedTransformationMatrix() *
		m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	return matWorld;
}

_vector CCharacter::Get_BonePosition(const _char* _pName)
{
	if (nullptr == m_pTransformCom 
		|| nullptr == m_pModelCom)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	CHierarchyNode* pNode = m_pModelCom->Get_HierarchyNodes(_pName);
	if(nullptr == pNode)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_matrix matWorld =
		pNode->Get_CombinedTransformationMatrix() *
		m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	return matWorld.r[3];
}

_matrix CCharacter::Get_BoneMatrix(const _char* _pName)
{
	if (nullptr == m_pTransformCom || nullptr == m_pModelCom)
		return XMMatrixIdentity();

	CHierarchyNode* pNode = m_pModelCom->Get_HierarchyNodes(_pName);
	if (nullptr == pNode)
		return XMMatrixIdentity();

	_matrix matWorld =
		pNode->Get_CombinedTransformationMatrix() *
		m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	return matWorld;
}

HRESULT CCharacter::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ePawnType = CPawn::P_CHARACTER;
	return S_OK;
}

HRESULT CCharacter::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	m_tHurtTimer = CUSTIMER(3.f);
	m_tHurtTimer.bPass = true;

	m_fMaxSpeed = m_fCurSpeed;
	return S_OK;
}

void CCharacter::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CCharacter::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (true == g_bGameRun)
	{
		if (false == m_tHurtTimer.bPass)
		{
			m_tHurtTimer.Is_Over(_dTimeDelta);
		}
	}
	Update_Bond_OtherCollider();
}

HRESULT CCharacter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

_bool CCharacter::Is_Collision(CCollider* _pCollider)
{
	if (nullptr == _pCollider || nullptr == m_pFirstCheckCollider || nullptr == m_pCollCollider)
		return false;

	if (false == m_pFirstCheckCollider->Is_Collision(_pCollider))
		return false;

	if (false == m_pCollCollider->Is_Collision(_pCollider))
		return false;

	return true;
}

HRESULT CCharacter::Damaged(CPawn* _pDamageOwner)
{
	if (FAILED(__super::Damaged(_pDamageOwner)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Ready_Component()
{
	if (FAILED(Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		m_pStage = pDataManager->Get_StageMgr()->Get_Stage();
		Safe_AddRef(m_pStage);
	
		if (nullptr != m_pStage)
		{
			m_pCurRegion = m_pStage->Get_Region(0);
			Safe_AddRef(m_pCurRegion);
			if (nullptr != m_pCurRegion)
			{
				CNavigation::NAVIDESC tDesc;
				tDesc.iCurIndex = 0;
				m_pNavigation = static_cast<CNavigation*>(m_pCurRegion->Get_Navigation()->Clone(&tDesc));
			}
		}
	}
	END_SINGLETON(pDataManager);

	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Update_Nav(m_pNavigation);
	}

	return S_OK;
}

HRESULT CCharacter::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	_float4x4 AnimWorldMatrix;
	XMStoreFloat4x4(&AnimWorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_DrawAnimMatrix)));

	if (FAILED(m_pShaderCom->Set_RawValue("g_AnimWorldMatrix", &AnimWorldMatrix, sizeof(_float4x4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCharacter::Find_OffsetNode()
{
	m_pNodeArr[NODE_OFFSET] = m_pModelCom->Get_HierarchyNodes("CHARA_OFFSET");
	m_pNodeArr[NODE_CHEST] = m_pModelCom->Get_HierarchyNodes("Hips");
	for (_uint i = 0; i < NODE_END; ++i)
	{
		Safe_AddRef(m_pNodeArr[i]);
		if (nullptr == m_pNodeArr[i])
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CCharacter::SetUp_AnimationTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

void CCharacter::Bind_Equipment_Status()
{
	PAWNSTATUS tPawnStatus;
	for (_uint i = 0; i < CEquipment::EQUIP_END; ++i)
	{
		if (nullptr != m_pEquipmentArr[i])
		{
			tPawnStatus += m_tPawnStatus + m_pEquipmentArr[i]->Get_PawnStatus();
		}
	}
	m_tCharInfo.UpdateInfo(tPawnStatus);
	m_tSaveStatus = tPawnStatus;
}

void CCharacter::Update_Bond_OtherCollider()
{
	_matrix WorldMatrix = m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	for (auto& iter : m_vecBones)
		iter.MultiplyCollider(WorldMatrix);
}

void CCharacter::Calculate_OffsetMatrix()
{
	_matrix matWorld = 
		m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
	XMStoreFloat4x4(&m_OffsetMatrix, matWorld);
}

_vector CCharacter::Calculate_OffsetLook()
{
	if (nullptr == m_pTransformCom)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vForward = XMLoadFloat4(&m_v4Forward);
	vForward = XMVector4Transform(vForward, m_pTransformCom->Get_WorldMatrix() * m_pNodeArr[NODE_OFFSET]->Get_OffsetMatrix() * m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix()
		* m_pModelCom->Get_PivotMatrix());
	XMStoreFloat4(&m_v4Forward, vForward);
	cout << m_v4Forward.x << ", " << m_v4Forward.y << ", " << m_v4Forward.z << ", " << endl;
	return vForward;
}

_vector CCharacter::Calculate_OffsetRight()
{
	if (nullptr == m_pTransformCom)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_matrix mRight = m_pTransformCom->Get_WorldMatrix();
	mRight.r[3] = mRight.r[3] + m_pTransformCom->Get_Right();
	mRight = mRight * m_pNodeArr[NODE_OFFSET]->Get_OffsetMatrix() * m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix()
		* m_pModelCom->Get_PivotMatrix();
	return mRight.r[3];
}

_vector CCharacter::Calculate_OffsetMatrixNotPosition()
{
	if (nullptr == m_pTransformCom)
		return XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	WorldMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	_matrix matWorld =
		m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() * WorldMatrix;

	return matWorld.r[3];
}

void CCharacter::Bind_Position()
{
	m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
	m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CCharacter::Bind_Attack_Postion()
{
	if (false == m_bChangeAttackMatrix)
		return;

	Bind_Position();
	m_bChangeAttackMatrix = false;
}

void CCharacter::Bind_Supplement_Position()
{
	if (false == m_bChangeSuppleCount)
		return;

	Bind_Position();
	m_bChangeSuppleCount = false;
}

void CCharacter::Bind_Damaged_Position()
{
	if (false == m_bChangeDamagedPosition)
		return;

	Bind_Position();
	m_bChangeDamagedPosition = false;
}

void CCharacter::Bind_Die_Position()
{
	if (false == m_bChangeDiePosition)
		return;

	Bind_Position();
	m_bChangeDiePosition = false;
}

_float3 CCharacter::Get_OffsetArrPositionV3(const NODES& _eOffsetNode)
{
	if (nullptr == m_pTransformCom || nullptr == m_pModelCom || nullptr == m_pNodeArr[_eOffsetNode])
		return _float3(0.f, 0.f, 0.f);

	_matrix matWorld =
		m_pNodeArr[_eOffsetNode]->Get_CombinedTransformationMatrix() *
		m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();

	_float3 v3Pos;
	XMStoreFloat3(&v3Pos, matWorld.r[3]);
	return v3Pos;
}

_vector CCharacter::Get_OffsetArrPosition(const NODES& _eOffsetNode)
{
	if (nullptr == m_pTransformCom || nullptr == m_pModelCom || nullptr == m_pNodeArr[_eOffsetNode])
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_matrix matWorld =
		m_pNodeArr[_eOffsetNode]->Get_CombinedTransformationMatrix() *
		m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();

	return matWorld.r[3];
}

void CCharacter::Calculate_DmgPercent(const _float& _fDmgPercents)
{
	if (_fDmgPercents >= 0.9f)
		m_eDmgPercents = DMG_LT;
	else if (_fDmgPercents >= 0.75f)
		m_eDmgPercents = DMG_MD;
	else if (_fDmgPercents >= 0.6f)
		m_eDmgPercents = DMG_BG;
	else
		m_eDmgPercents = DMG_GG;
}

void CCharacter::Coll_Tick(_matrix matWorld)
{
	__super::Coll_Tick(matWorld);
	if (nullptr != m_pFirstCheckCollider)
		m_pFirstCheckCollider->Tick(matWorld);
	if (nullptr != m_pRangeCollider)
		m_pRangeCollider->Tick(matWorld);
}

#ifdef _DEBUG
_bool CCharacter::Add_Collider_Render()
{
	BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
	{
		CObjectTool* pObjectTool = static_cast<CObjectTool*>(pImGuiManager->Get_MainView()->Get_ImGuiClass(CMainView::GUI_OBJ));
		if (true == pObjectTool->Is_CharObjectDrawCall())
		{
			if (true == __super::Add_Collider_Render())
			{
				if (nullptr != m_pFirstCheckCollider)
					m_pRendererCom->Add_Components(m_pFirstCheckCollider);
				if (nullptr != m_pRangeCollider)
					m_pRendererCom->Add_Components(m_pRangeCollider);

				for (auto& iter : m_vecBones)
					m_pRendererCom->Add_Components(iter.pCollider);

				END_SINGLETON(pImGuiManager);
				return true;
			}
		}
	}
	END_SINGLETON(pImGuiManager);
	return false;
}
#endif

void CCharacter::Load_BonesCollider(CModel* _pModel, vector<BONESCOLLDESC>* _pBones)
{
	if (nullptr == _pModel || nullptr == _pBones)
		return;

	FILEGROUP* pFileGroup = _pModel->Get_ModelFileGroup();
	if (nullptr == pFileGroup)
		return;

	FILEGROUP* pColliderGroup = pFileGroup->Find_Group(L"BonesCollider");
	if (nullptr == pColliderGroup)
		return;

	for (auto& iter : pColliderGroup->vecFileDataList)
	{
		BONESCOLLDESC tBonesCollider;
		tBonesCollider.Load(_pModel, &iter, LEVEL_STATIC);
		_pBones->push_back(tBonesCollider);
	}
}

void CCharacter::Save_BonesCollider(CModel* _pModel,  vector<BONESCOLLDESC>* _pBones)
{
	if (nullptr == _pModel || nullptr == _pBones)
		return;

	for (auto& iter : *_pBones)
	{
		iter.Save(_pModel, _pModel->Get_ModelFileGroup());
	}

	for (auto& iter : *_pBones)
	{
		iter.Release();
	}
	_pBones->clear();
}

#pragma region USE_TOOL_METHOD
_bool CCharacter::Extern_EditValue()
{
	return _bool();
}

_bool CCharacter::Extern_HierakeyView(const string& _str)
{
	return _bool();
}

HRESULT CCharacter::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CCharacter::Delete_Self()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		pGameInstance->Add_DeleteObject(LEVEL_TOOL, Get_Layer(), this);

		for (auto& iter : m_pEquipmentArr)
		{
			pGameInstance->Add_DeleteObject(LEVEL_TOOL, LAYER_WEAPON, iter);
		}
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CCharacter::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CCharacter::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CCharacter::Is_Picking(_float3* _pOut)
{
	//_bool bIsPicking = false;
	//BEGIN_SINGLETON(CGameInstance, pGameInstance);
	//{
	//	for (auto& iter : m_pModelCom->Get_MeshContainers())
	//	{
	//		bIsPicking = pGameInstance->Picking_Mesh(this, m_pTransformCom, (CVIBuffer*)iter, _pOut);
	//	}
	//}
	//END_SINGLETON(pGameInstance);
	return false;
}

void CCharacter::Reset_Data()
{
	Reset_TransformData();
}
void CCharacter::Extern_ShowCurAnimation_Data()
{

	if (ImGui::TreeNodeEx("Animation Fast Section"))
	{
		for (auto& iter : m_pModelCom->Get_Animations())
		{
			if (ImGui::TreeNodeEx(iter->Get_Name().c_str()))
			{
				_double dDuration = m_pModelCom->Get_CurAnimation()->Get_Duration();
				_double dTimeAcc = m_pModelCom->Get_CurAnimation()->Get_TimeAcc();
				char Durations[MAX_PATH] = "";
				char TimeAcc[MAX_PATH] = "";
				sprintf_s(Durations, "Duration: %.3f", dDuration);
				sprintf_s(TimeAcc, "TimeAcc: %.3f", dTimeAcc);
				ImGui::Text(Durations);
				ImGui::Text(TimeAcc);
				
				BEGIN_SINGLETON(CData_Manager, pDataManager)
				{
					CVariable_Manager::ANIMFASTVALUE tData;
					tData.fStartFastSection = pDataManager->Get_VariableMgr()->Get_AnimFastValue().fStartFastSection;
					tData.fEndFastSection = pDataManager->Get_VariableMgr()->Get_AnimFastValue().fEndFastSection;
					tData.fFastValue = pDataManager->Get_VariableMgr()->Get_AnimFastValue().fFastValue;

					ImGui::DragFloat("Start SE", &tData.fStartFastSection, 0.f, 0.f, (_float)dDuration);
					ImGui::DragFloat("End SE", &tData.fEndFastSection, 0.f, 0.f, (_float)dDuration);
					ImGui::DragFloat("Fast SE", &tData.fFastValue, 0.f, 0.f, 100.f);

					pDataManager->Get_VariableMgr()->Set_AnimFastValue(tData);

					if (ImGui::Button("Push Section"))
					{
						CAnimation::ANIMFASTSECTION tAnimFastSection;
						tAnimFastSection.fStartSpot = tData.fStartFastSection;
						tAnimFastSection.fEndSpot = tData.fEndFastSection;
						tAnimFastSection.fFastValue = tData.fFastValue;
						iter->Add_AnimFastSection(tAnimFastSection);
					}

					if (ImGui::TreeNodeEx("FastValues", ImGuiTreeNodeFlags_DefaultOpen))
					{
						_uint iIndex = 0;
						for (auto& FastSection : *iter->Get_AnimFastSection())
						{
							char Value[MAX_PATH] = "";
							sprintf_s(Value, "Fast Value %d", iIndex++);
							if (ImGui::TreeNodeEx(Value))
							{
								if (ImGui::TreeNodeEx("Modify"))
								{
									CAnimation::ANIMFASTSECTION tAnimFastSection;
									tAnimFastSection.fStartSpot = FastSection.fStartSpot;
									tAnimFastSection.fEndSpot = FastSection.fEndSpot;
									tAnimFastSection.fFastValue = FastSection.fFastValue;

									ImGui::DragFloat("Start MD", &tAnimFastSection.fStartSpot, 0.f, 0.f, (_float)dDuration);
									ImGui::DragFloat("End MD", &tAnimFastSection.fEndSpot, 0.f, 0.f, (_float)dDuration);
									ImGui::DragFloat("Fast MD", &tAnimFastSection.fFastValue, 0.f, 0.f, 100.f);

									FastSection = tAnimFastSection;
									ImGui::TreePop();
								}
								else
								{
									char Tex[MAX_PATH] = "";
									sprintf_s(Tex, "Start Spot: %f\nEnd Spot: %f\nFastValue: %ff",
										FastSection.fStartSpot, FastSection.fEndSpot, FastSection.fFastValue);
									ImGui::Text(Tex);
								}

								if (ImGui::Button("Delete"))
								{
									iter->ReMove_FastSection(FastSection);
								}
								ImGui::TreePop();
							}
						}
						ImGui::TreePop();
					}
					m_pModelCom->Repeat_Animation(iter->Get_Name().c_str());
				}
				END_SINGLETON(pDataManager);
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	} 
	else if (ImGui::TreeNodeEx("Animation Clip Section"))
	{
		for (auto& iter : m_pModelCom->Get_Animations())
		{
			if (ImGui::TreeNodeEx(iter->Get_Name().c_str()))
			{
				_double dDuration = m_pModelCom->Get_CurAnimation()->Get_Duration();
				_double dTimeAcc = m_pModelCom->Get_CurAnimation()->Get_TimeAcc();
				char Durations[MAX_PATH] = "";
				char TimeAcc[MAX_PATH] = "";
				sprintf_s(Durations, "Duration: %.3f", dDuration);
				sprintf_s(TimeAcc, "TimeAcc: %.3f", dTimeAcc);
				ImGui::Text(Durations);
				ImGui::Text(TimeAcc);

				BEGIN_SINGLETON(CData_Manager, pDataManager)
				{
					CVariable_Manager::ANIMCLIPVALUE tAnimClipSection;
					tAnimClipSection.fChange = pDataManager->Get_VariableMgr()->Get_AnimClipValue().fChange;
					tAnimClipSection.fEndValue = pDataManager->Get_VariableMgr()->Get_AnimClipValue().fEndValue;
					strcpy_s(tAnimClipSection.pName, pDataManager->Get_VariableMgr()->Get_AnimClipValue().pName);

					ImGui::DragFloat("Clip Change", &tAnimClipSection.fChange, 0.f, 0.f, (_float)dDuration);
					ImGui::DragFloat("Clip End", &tAnimClipSection.fEndValue, 0.f, 0.f, (_float)dDuration);
					ImGui::InputText("Clip Name", tAnimClipSection.pName, sizeof(_char) * MAX_PATH);

					pDataManager->Get_VariableMgr()->Set_AnimClipValue(tAnimClipSection);
				}
				END_SINGLETON(pDataManager);
				if (ImGui::Button("Push Section"))
				{
					CAnimation::ANIMCLIPSECTION tData;
					tData.fChange = pDataManager->Get_VariableMgr()->Get_AnimClipValue().fChange;
					tData.fEndValue = pDataManager->Get_VariableMgr()->Get_AnimClipValue().fEndValue;
					tData.strName = pDataManager->Get_VariableMgr()->Get_AnimClipValue().pName;
					iter->Add_AnimClipSection(tData);
				}

				if (ImGui::TreeNodeEx("ClipValue", ImGuiTreeNodeFlags_DefaultOpen))
				{
					_uint iIndex = 0;
					for (auto& ClipSection : *iter->Get_AnimClipSection())
					{
						char Value[MAX_PATH] = "";
						sprintf_s(Value, "Clip Value %d", iIndex++);
						if (ImGui::TreeNodeEx(Value))
						{
							if (ImGui::TreeNodeEx("Modify"))
							{
								CAnimation::ANIMCLIPSECTION tAnimClipSection;
								tAnimClipSection.fChange = ClipSection.fChange;
								tAnimClipSection.fEndValue = ClipSection.fEndValue;
								tAnimClipSection.strName = ClipSection.strName;

								ImGui::DragFloat("Clip Change", &tAnimClipSection.fChange, 0.f, 0.f, (_float)dDuration);
								ImGui::DragFloat("Clip End", &tAnimClipSection.fEndValue, 0.f, 0.f, (_float)dDuration);
								_char pName[MAX_PATH] = "";
								strcpy_s(pName, tAnimClipSection.strName.c_str());
								ImGui::InputText("Clip Name", pName, sizeof(_char)* MAX_PATH);
								tAnimClipSection.strName = pName;

								ClipSection = tAnimClipSection;
								ImGui::TreePop();
							}
							else
							{
								char Tex[MAX_PATH] = "";
								sprintf_s(Tex, "Change : %f\nEnd Value: %f\nName: %s",
									ClipSection.fChange, ClipSection.fEndValue, ClipSection.strName.c_str());
								ImGui::Text(Tex);
							}

							if (ImGui::Button("Delete"))
							{
								iter->Remove_AnimClipSection(ClipSection);
							}
							ImGui::TreePop();
						}
					}
					ImGui::TreePop();
				}
				m_pModelCom->Repeat_Animation(iter->Get_Name().c_str());
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	else if (nullptr != m_pModelCom->Get_CurAnimation())
	{
		ImGui::Text(m_pModelCom->Get_CurAnimation()->Get_Name().c_str());
		_double dDuration = m_pModelCom->Get_CurAnimation()->Get_Duration();
		_double dTimeAcc = m_pModelCom->Get_CurAnimation()->Get_TimeAcc();
		char Durations[MAX_PATH] = "";
		char TimeAcc[MAX_PATH] = "";

		sprintf_s(Durations, "Duration: %.3f", dDuration);
		sprintf_s(TimeAcc, "TimeAcc: %.3f", dTimeAcc);

		ImGui::Text(Durations);
		ImGui::Text(TimeAcc);
	}
}
void CCharacter::Control_ColliderData()
{
	__super::Control_ColliderData();
	if (ImGui::TreeNodeEx("FirstDamagedColl"))
	{
		if (nullptr != m_pFirstCheckCollider)
		{
			COLLDATA tCollData;
			tCollData.v3CollScale = m_pFirstCheckCollider->Get_Scale();
			tCollData.v3Translation = m_pFirstCheckCollider->Get_Translate();

			_float Translate[3] = { tCollData.v3Translation.x,  tCollData.v3Translation.y, tCollData.v3Translation.z };
			_float CollSclae[3] = { tCollData.v3CollScale.x ,  tCollData.v3CollScale.y , tCollData.v3CollScale.z  };

			ImGui::InputFloat3("FirstT", Translate, "%.1f");
			ImGui::InputFloat3("FirstS", CollSclae, "%.1f");

			m_pFirstCheckCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
			m_pFirstCheckCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("FindRangeCollider"))
	{
		if (nullptr != m_pRangeCollider)
		{
			COLLDATA tCollData;
			tCollData.v3CollScale = m_pRangeCollider->Get_Scale();
			tCollData.v3Translation = m_pRangeCollider->Get_Translate();

			_float Translate[3] = { tCollData.v3Translation.x,  tCollData.v3Translation.y, tCollData.v3Translation.z };
			_float CollSclae[3] = { tCollData.v3CollScale.x ,  tCollData.v3CollScale.y , tCollData.v3CollScale.z  };

			ImGui::InputFloat3("FindT", Translate, "%.1f");
			ImGui::InputFloat3("TindS", CollSclae, "%.1f");

			m_pRangeCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
			m_pRangeCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
		}
		ImGui::TreePop();
	}
}

void CCharacter::Extern_ShowCurHierckeyNode(vector<BONESCOLLDESC>* _pBones)
{
#ifdef _DEBUG
	if (nullptr == m_pModelCom || nullptr == _pBones)
		return;

	if (nullptr == m_pTransformCom)
		return;

	if (ImGui::TreeNodeEx("HierackyNodes", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (auto& iter : m_pModelCom->Get_HierachyNodeList())
			{
				if (ImGui::Selectable(iter->Get_Name().c_str()))
				{
					_matrix WorldMatrix =  iter->Get_CombinedTransformationMatrix() *
						m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();

					BEGIN_SINGLETON(CData_Manager, pDataManager)
					{
						pDataManager->Get_VariableMgr()->Set_TestColliderOffsetMatrix(WorldMatrix);
						pDataManager->Get_VariableMgr()->Set_NodeName(iter->Get_Name());
					}
					END_SINGLETON(pDataManager);
				}
			}
			ImGui::EndListBox();
			if (ImGui::Button("CreateNode"))
			{
				BEGIN_SINGLETON(CData_Manager, pDataManager)
				{
					_string strName = pDataManager->Get_VariableMgr()->Get_NodeName();
					BONESCOLLDESC tBoneCollider;
					if (0 == pDataManager->Get_VariableMgr()->Get_ColliderType())
					{
						CCollider::COLLIDERDESC tCollDesc;
						tCollDesc.vScale = pDataManager->Get_VariableMgr()->Get_OBBCollider()->Get_Scale();
						tCollDesc.vTranslation = pDataManager->Get_VariableMgr()->Get_OBBCollider()->Get_Translate();
						tBoneCollider.Add_Colliders(m_pModelCom, strName.c_str(), LEVEL_STATIC,
							PROTO_COMP_OBBCOLLIDER, &tCollDesc);
					}
					else	 if (1 == pDataManager->Get_VariableMgr()->Get_ColliderType())
					{
						CCollider::COLLIDERDESC tCollDesc;
						tCollDesc.vScale = pDataManager->Get_VariableMgr()->Get_CircleCollider()->Get_Scale();
						tCollDesc.vTranslation = pDataManager->Get_VariableMgr()->Get_CircleCollider()->Get_Translate();
						tBoneCollider.Add_Colliders(m_pModelCom, strName.c_str(), LEVEL_STATIC,
							PROTO_COMP_SPHERECOLLIDER, &tCollDesc);
					}
					_pBones->push_back(tBoneCollider);
				}
				END_SINGLETON(pDataManager);
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNodeEx("AddBonesCollider", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (vector<BONESCOLLDESC>::iterator it = _pBones->begin(); it != _pBones->end(); ++it)
		{
			if(ImGui::TreeNodeEx(it->pHierachyNode->Get_Name().c_str()))
			{
				COLLDATA tCollData;
				tCollData.v3CollScale = it->pCollider->Get_Scale();
				tCollData.v3Translation = it->pCollider->Get_Translate();

				_float Translate[3] = { tCollData.v3Translation.x,  tCollData.v3Translation.y, tCollData.v3Translation.z };
				_float CollSclae[3] = { tCollData.v3CollScale.x ,  tCollData.v3CollScale.y , tCollData.v3CollScale.z };

				if (ImGui::InputFloat3("BoneCollT", Translate, "%.2f"))
				{
					it->pCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
				}
				if (ImGui::InputFloat3("BoneCollS", CollSclae, "%.2f"))
				{
					it->pCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
				}

				ImGui::Checkbox("BoneCollActive", &it->bIsCollision);
				
				if (ImGui::Button("DeleteBoneColl"))
				{
					it->Release();
					_pBones->erase(it);
					ImGui::TreePop();
					break;
				}

				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
#endif
}

#pragma endregion USE_TOOL_METHOD
CGameObject* CCharacter::Clone(void* pArg)
{
	return nullptr;
}

void CCharacter::Free()
{
	Save_BonesCollider(m_pModelCom, &m_vecBones);

	Safe_Release(m_pNavigation);
	Safe_Release(m_pCurRegion);
	Safe_Release(m_pStage);

	for (_uint i = 0; i < CEquipment::EQUIP_END; ++i)
		Safe_Release(m_pEquipmentArr[i]);

	for (_uint i = 0; i < NODE_END; ++i)
		Safe_Release(m_pNodeArr[i]);

	Safe_Release(m_pFirstCheckCollider);
	Safe_Release(m_pRangeCollider);
	__super::Free();
}