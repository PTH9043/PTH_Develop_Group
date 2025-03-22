#include "Item.h"
#include "GameInstance.h"
#include "HierarchyNode.h"

CItem::CItem(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPawn(_pDevice, _pContext)
{
	m_SocketMatrix = CMethod::XMMatrixIdentity_P();
}

CItem::CItem(const CItem& _rhs) 
	: CPawn(_rhs), m_SocketMatrix(_rhs.m_SocketMatrix), m_eItemType(_rhs.m_eItemType)
{
}

HRESULT CItem::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ePawnType = PAWN_ITEM;
	return S_OK;
}

HRESULT CItem::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_LoadData(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	Update_BindTransform();
}

void CItem::Late_Tick(const _double& _dTimeDelta)
{
	if (false == m_bRenderEnable)
		return;

	if (m_bIsColliderActive == true)
	{
		__super::Late_Tick(_dTimeDelta);
	}

	if (nullptr != m_pRendererCom && m_bActive)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	
#ifdef DEBUG
	if (nullptr != m_pColliderCom)
		m_pRendererCom->Add_Components(CRenderer::DEBUG_ETC, m_pColliderCom);
#endif // DEBUG
}

HRESULT CItem::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE)))
				return E_FAIL;

			m_pModelCom->Render_NonAnim(i, m_pShaderCom, 1);
		}
	}

	return S_OK;
}

HRESULT CItem::RenderLight()
{
	return S_OK;
}

HRESULT CItem::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		_float4x4 SocketMatrix;
		XMStoreFloat4x4(&SocketMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_SocketMatrix)));

		if (FAILED(m_pShaderCom->Set_RawValue("g_SocketMatrix", &SocketMatrix, sizeof(_float4x4))))
			return E_FAIL;
		m_PreSocketMatrix = m_SocketMatrix;
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CItem::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_SHADER, LEVEL_STATIC,
		PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::SetUp_LoadData(void* _pArg)
{
	{
		RETURN_IFNOT(nullptr == _pArg, E_FAIL);
	}
	ITEMDESC tItemDesc = *static_cast<ITEMDESC*>(_pArg);
	{
 		RETURN_IFNOT(nullptr == tItemDesc.pItemData, E_FAIL);
		RETURN_IFNOT(nullptr == tItemDesc.pTargetModel, E_FAIL);
		RETURN_IFNOT(nullptr == tItemDesc.pTargetTransform, E_FAIL);

		m_pTargetModel = tItemDesc.pTargetModel;
		m_pTargetTransform = tItemDesc.pTargetTransform;
		m_pTargetAnimator = tItemDesc.pTargetAnimator;
		Safe_AddRef(m_pTargetModel);
		Safe_AddRef(m_pTargetTransform);
		Safe_AddRef(m_pTargetAnimator);
	}
	DWORD dwByte = 0;
	HANDLE hFile = CMethod::Begin_Load(tItemDesc.pItemData->strFilePath.c_str());
	// ====================== 불러오기 ======================
 	CCollider::COLLIDERDESC tDesc;
	ReadFile(hFile, &tDesc, sizeof(CCollider::COLLIDERDESC), &dwByte, nullptr);
	_string str = CMethod::Load_File_String(hFile, dwByte);
	// ====================== 불러오기 ======================
	CloseHandle(hFile);

	m_pTargetBoneMatrix = m_pTargetModel->Get_HierarchyNodes(str.c_str());
	Safe_AddRef(m_pTargetBoneMatrix);
	RETURN_IFNOT(nullptr == m_pTargetBoneMatrix, E_FAIL);

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel, tItemDesc.strModelProtoName.c_str(), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	tDesc.vScale *= 10.f;
	if (FAILED(Add_Component(COM_COLLIDER_OBB, LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER,
		(CComponent**)&m_pColliderCom, &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem::Update_BindTransform()
{
	if (nullptr == m_pTargetBoneMatrix || nullptr == m_pTransformCom || nullptr == m_pColliderCom || nullptr == m_pModelCom)
		return E_FAIL;

	if (nullptr != m_pTargetAnimator)
	{
		_float4x4 mWorld;
		XMStoreFloat4x4(&mWorld, m_pTransformCom->Get_WorldMatrix() *
			m_pModelCom->Get_PivotMatrix() * m_pTargetBoneMatrix->Get_CombinedTransformationMatrix() *
			m_pTargetModel->Get_PivotMatrix() * m_pTargetTransform->Get_WorldMatrix());

		m_SocketMatrix = mWorld;

		XMStoreFloat3((_float3*)&mWorld.m[0][0], XMVector3Normalize(CMethod::XMMatrix4x4GetVector(mWorld, MATRIXROW::MATROW_RIGHT)));
		XMStoreFloat3((_float3*)&mWorld.m[1][0], XMVector3Normalize(CMethod::XMMatrix4x4GetVector(mWorld, MATRIXROW::MATROW_UP)));
		XMStoreFloat3((_float3*)&mWorld.m[2][0], XMVector3Normalize(CMethod::XMMatrix4x4GetVector(mWorld, MATRIXROW::MATROW_LOOK)));

		Coll_Tick(XMLoadFloat4x4(&mWorld));
	}
	else
	{
		_float4x4 mWorld;
		XMStoreFloat4x4(&mWorld, m_pTransformCom->Get_WorldMatrix() *
			m_pModelCom->Get_PivotMatrix() * m_pTargetBoneMatrix->Get_CombinedTransformationMatrix() *
			m_pTargetModel->Get_PivotMatrix() * m_pTargetTransform->Get_WorldMatrix());

		m_SocketMatrix = mWorld;

		XMStoreFloat3((_float3*)&mWorld.m[0][0], XMVector3Normalize(CMethod::XMMatrix4x4GetVector(mWorld, MATRIXROW::MATROW_RIGHT)));
		XMStoreFloat3((_float3*)&mWorld.m[1][0], XMVector3Normalize(CMethod::XMMatrix4x4GetVector(mWorld, MATRIXROW::MATROW_UP)));
		XMStoreFloat3((_float3*)&mWorld.m[2][0], XMVector3Normalize(CMethod::XMMatrix4x4GetVector(mWorld, MATRIXROW::MATROW_LOOK)));

		Coll_Tick(XMLoadFloat4x4(&mWorld));
	}
	return S_OK;
}

_float3 CItem::Get_MoveDirection()
{
	_matrix matCurWorld = m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&m_SocketMatrix);
	_matrix matPreWorld = XMLoadFloat4x4(&m_PrevWorldMatrix) * XMLoadFloat4x4(&m_PreSocketMatrix);

	_vector vCurPosition = matCurWorld.r[3];
	_vector vPrePosition = matPreWorld.r[3];

	_float3 vDir = XMVector3Normalize(vCurPosition - vPrePosition);

	return vDir;
}

CGameObject* CItem::Clone(void* pArg)
{
	return nullptr;
}

void CItem::Free()
{
	Safe_Release(m_pTargetBoneMatrix);
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pTargetAnimator);
	__super::Free();
}
