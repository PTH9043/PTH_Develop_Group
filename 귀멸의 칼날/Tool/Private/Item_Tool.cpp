#include "Item_Tool.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "AnimViewerModel.h"
#include "CutSceneModel_Tool.h"

CItem_Tool::CItem_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPawn_Tool(_pDevice, _pContext)
{
	XMStoreFloat4x4(&m_SocketMatrix, XMMatrixIdentity());
}

CItem_Tool::CItem_Tool(const CItem_Tool& _rhs)
	: CPawn_Tool(_rhs), m_SocketMatrix(_rhs.m_SocketMatrix)
{

}

HRESULT CItem_Tool::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_Tool::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CItem_Tool::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CItem_Tool::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
	if(nullptr != m_pRendererCom )
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	Update_BindTransform();
}

HRESULT CItem_Tool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG // 대사툴 작업 끝나면 다시 풀기
	//if (nullptr != m_pColliderCom)
		//m_pColliderCom->Render_Debug();
#endif
	
	if (FAILED(SetUp_ConstantValue()))
		return E_FAIL;

	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;

			if(nullptr != m_pOwnerModel)
				m_pModelCom->Render_NonAnim(i, m_pShaderCom, 1);
			else
				m_pModelCom->Render_NonAnim(i, m_pShaderCom, 0);
		}
	}
	return S_OK;
}

void CItem_Tool::Bine_Model(CModel* _pModel)
{
	if (nullptr == _pModel)
		return;

	Safe_Release(m_pModelCom);
	m_pModelCom = _pModel;
	Safe_AddRef(_pModel);
}

void CItem_Tool::Find_Bones(CGameObject* _pGameObject)
{
	if (nullptr == _pGameObject)
		return;

	m_pOwnerObject = _pGameObject;

	if (nullptr != m_pOwnerObject)
	{
		CAnimViewerModel* pAnimViewModel = dynamic_cast<CAnimViewerModel*>(m_pOwnerObject);
		if (nullptr != pAnimViewModel)
		{
			m_pOwnerModel = pAnimViewModel->Get_Model();
		}
		else
		{
			CCutSceneModel_Tool* pCutSceneModel = dynamic_cast<CCutSceneModel_Tool*>(m_pOwnerObject);
			if (nullptr != pCutSceneModel)
			{
				m_pOwnerModel = pCutSceneModel->Get_Model();
			}
		}
	}

	if (nullptr == m_pOwnerModel)
		return;

	_char INPUT[MAX_PATH] = "";
	strcpy_s(INPUT, m_strText.c_str());
	_bool bInput = false;
	bInput = ImGui::InputText("Bones", INPUT, sizeof(_char) * MAX_PATH);
	m_strText = INPUT;

	if (!strcmp(m_strText.c_str(), ""))
	{
		for (auto& iter : m_pOwnerModel->Get_HierachyNodeList())
		{
			if (ImGui::Selectable(iter->Get_Name().c_str()))
			{
				m_pBindBoneNode = iter;
			}
		}
	}
	else if (true == bInput)
	{
		m_SaveToFindBoneList.clear();
		for (auto& iter : m_pOwnerModel->Get_HierachyNodeList())
		{
			if (CMethod::Is_Same_Text(iter->Get_Name().c_str(), m_strText.c_str()))
				m_SaveToFindBoneList.push_back(iter);
		}
	}
	else
	{
		for (auto& iter : m_SaveToFindBoneList)
		{
			if (ImGui::Selectable(iter->Get_Name().c_str()))
			{
				m_pBindBoneNode = iter;
			}
		}
	}
}

HRESULT CItem_Tool::SetUp_ConstantValue()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		_float4x4 World = m_pTransformCom->Get_WorldFloat4x4_TP();
		_float4x4 View = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
		_float4x4 Proj = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);

		if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &World, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &View, sizeof(_float4x4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &Proj ,sizeof(_float4x4))))
			return E_FAIL;

		_float4x4 SocketMatrix;
		XMStoreFloat4x4(&SocketMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_SocketMatrix)));

		if (FAILED(m_pShaderCom->Set_RawValue("g_SocketMatrix", &SocketMatrix, sizeof(_float4x4))))
			return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CItem_Tool::Ready_Components()
{
	if (FAILED(Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;

	if (FAILED(Add_Component(COM_SHADER, LEVEL_STATIC,
		PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	if (FAILED(Add_Component(COM_RENDERER, LEVEL_STATIC,
		PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	{
		CCollider::COLLIDERDESC tDesc;
		tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
		tDesc.vScale = _float3(1.f, 1.f, .1f);

		if (FAILED(Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
			PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pColliderCom, &tDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CItem_Tool::Update_BindTransform()
{
	if (nullptr == m_pBindBoneNode || nullptr == m_pTransformCom || nullptr == m_pColliderCom
		|| nullptr == m_pOwnerObject || nullptr == m_pModelCom)
		return E_FAIL;

	CModel* pOwnerModel = m_pOwnerModel;
	CTransform* pOwnerTransform = static_cast<CTransform*>(m_pOwnerObject->Get_Component(COM_TRANSFORM));

	if (nullptr == pOwnerModel || nullptr == pOwnerTransform)
		return E_FAIL;

	_float4x4 mWorld;
	XMStoreFloat4x4(&mWorld,
		m_pTransformCom->Get_WorldMatrix() * m_pModelCom->Get_PivotMatrix() * m_pBindBoneNode->Get_CombinedTransformationMatrix() *
		m_pOwnerModel->Get_PivotMatrix() * pOwnerTransform->Get_WorldMatrix());

	Coll_Tick(XMLoadFloat4x4(&mWorld));
	m_SocketMatrix = mWorld;

	_float3 v3Translate = m_pColliderCom->Get_Translate();

	XMStoreFloat3((_float3*)&mWorld.m[0][0], XMVector4Normalize(XMLoadFloat4((_float4*)&mWorld.m[0][0])) * m_pColliderCom->Get_Scale().z);
	XMStoreFloat3((_float3*)&mWorld.m[1][0], -XMVector4Normalize(XMLoadFloat4((_float4*)&mWorld.m[1][0])) * m_pColliderCom->Get_Scale().y);
	XMStoreFloat3((_float3*)&mWorld.m[2][0], -XMVector4Normalize(XMLoadFloat4((_float4*)&mWorld.m[2][0])) * m_pColliderCom->Get_Scale().z);

//	XMStoreFloat3((_float3*)&m_SocketMatrix.m[3][0], m_pColliderCom->Get_CurPos());
	return S_OK;
}

#pragma region USE_TOOL_METHOD
_bool CItem_Tool::Extern_EditValue()
{
	return _bool();
}

_bool CItem_Tool::Extern_HierakeyView(const string& _str)
{
	if (nullptr == m_pColliderCom)
		return false;

	ImGui::Text(_str.c_str());

	{
		// Position
		_float3 v3Position = m_pTransformCom->Get_PosF3();
		_float fArrValue[3] = { v3Position.x, v3Position.y, v3Position.z };
		ImGui::DragFloat3("Item_Position", fArrValue, 0.05f, -1000000.f, 1000000.f);
		v3Position = _float3(fArrValue[0], fArrValue[1], fArrValue[2]);
		m_pTransformCom->Set_PosF3(v3Position);
	}
	{
		// Rotation
		_bool bIsTrue = ImGui::DragFloat3("Item_Rotation", &m_v3Rotation.x, 0.05f, -1000000.f, 1000000.f);
		if (true == bIsTrue)
		{
			m_pTransformCom->Rotate_Turn(m_v3Rotation);
		}
	}


	CCollider::COLLIDERDESC tCollData;
	tCollData.vScale = m_pColliderCom->Get_Scale();
	tCollData.vTranslation = m_pColliderCom->Get_Translate();

	_float Translate[3] = { tCollData.vTranslation.x,  tCollData.vTranslation.y, tCollData.vTranslation.z };
	_float CollSclae[3] = { tCollData.vScale.x ,  tCollData.vScale.y , tCollData.vScale.z };

	if (ImGui::DragFloat3("CollT", Translate, 1, 0, 0 ,"%.3f"))
	{
		m_pColliderCom->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
	}
	if (ImGui::DragFloat3("CollS", CollSclae, 1, 0, 0 , "%.3f"))
	{
		m_pColliderCom->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
	}

	Save_Value();
	return true;
}

HRESULT CItem_Tool::Create_Self(CGameObject** _ppOut)
{
	return S_OK;
}

HRESULT CItem_Tool::Delete_Self()
{
	return S_OK;
} 

HRESULT CItem_Tool::Save_Data()
{
	return S_OK;
}

HRESULT CItem_Tool::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return S_OK;
}

_bool CItem_Tool::Is_Picking(_float3* _pOut)
{
	return true;
}

void CItem_Tool::Reset_Data()
{

}

void CItem_Tool::Save_Value()
{
	if (nullptr == m_pModelCom)
		return;

	if (ImGui::Button("Save"))
	{
		FILEGROUP* pFileGroup = m_pModelCom->Get_FileGroup();
		FILEDATA* pFileData = m_pModelCom->Get_FileData();
		if (nullptr != pFileGroup && nullptr != pFileData)
		{
			_wstring strPath = pFileGroup->strPath;
			strPath.append(L"\\Data\\");

			_tmkdir(strPath.c_str());

			strPath.append(pFileData->strFileName);
			strPath.append(L".bin");

			HANDLE hFile = CMethod::Begin_Save(strPath.c_str());
			{
				DWORD dwByte = 0;

				CCollider::COLLIDERDESC tCollData;
				tCollData.vScale = m_pColliderCom->Get_Scale();
				tCollData.vTranslation = m_pColliderCom->Get_Translate();
				CMethod::Save_File_NotString(hFile, dwByte, tCollData);
				if (nullptr != m_pBindBoneNode)
				{
					CMethod::Save_File_String(hFile, dwByte, m_pBindBoneNode->Get_Name());
				}
			}
			CloseHandle(hFile);
		}
	}
}


#pragma endregion USE_TOOL_METHOD
CItem_Tool* CItem_Tool::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CItem_Tool* pInstance = new CItem_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CItem_Tool::Create To Failed");
#endif
	}
	return pInstance;
}
CGameObject* CItem_Tool::Clone(void* pArg)
{
	CItem_Tool* pInstance = new CItem_Tool(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CItem_Tool::Clone To Failed");
#endif
	}
	return pInstance;
}

void CItem_Tool::Free()
{
	__super::Free();
}