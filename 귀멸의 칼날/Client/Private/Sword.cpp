#include "Sword.h"
#include "TrailObject.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Level_Story.h"

CSword::CSword(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CItem(_pDevice, _pContext)
{
}

CSword::CSword(const CSword& _rhs)
	: CItem(_rhs)
{
}

void CSword::Set_TrailActive(_bool bActive)
{
	m_pTrailObject->Set_Active(bActive);
}

void CSword::Change_Bind_Matrix(CHierarchyNode* _pNode)
{
	RETURN_IFNOT(nullptr == _pNode, ;);  
	Safe_Release(m_pTargetBoneMatrix);
	m_pTargetBoneMatrix = _pNode;
	Safe_AddRef(m_pTargetBoneMatrix);
}

void CSword::Change_Trail_Texture(const _wstring& _strTextureName)
{
	if (nullptr == m_pTrailObject)
		return;

	m_pTrailObject->Change_Texture(_strTextureName);
}

void CSword::Return_Trail_Texture()
{
	if (nullptr == m_pTrailObject)
		return;

	m_pTrailObject->Return_Texture();
}

void CSword::Active_TrailObject(const _bool& _bActive)
{
	if (nullptr == m_pTrailObject)
		return;

	m_pTrailObject->Set_Active(_bActive);
}

void CSword::Update_TrailMatrix(const _float& _fStartValue, const _float& _fEndValue)
{
	if (nullptr == m_pTrailObject)
		return;

	m_pTrailObject->Update_TrailMatrix(_fStartValue, _fEndValue);
}

HRESULT CSword::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_eItemType = ITEM_SWORD;
	return S_OK;
}

HRESULT CSword::NativeConstruct_Clone(void* pArg)
{
	RETURN_IFNOT(nullptr == pArg, E_FAIL);

	SWORDDESC tSwordDesc = *static_cast<SWORDDESC*>(pArg);

	if (FAILED(__super::NativeConstruct_Clone(&tSwordDesc.tItemDesc)))
		return E_FAIL;

	CTrailObject::TRAILOBJ tObj;
	tObj.pTargetModel = m_pModelCom;
	tObj.pTargetTrasform = m_pSocketTransform;
	tObj.strStartBoneName = tSwordDesc.strStartBoneName;
	tObj.strFinishBoneName = tSwordDesc.strFinishBoneName;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		m_pTrailObject = static_cast<CTrailObject*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel, LAYER_TRAIL, PROTO_GOB_TRAILOBJ, &tObj));
		Safe_AddRef(m_pTrailObject);
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CSword::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	if (nullptr != m_pSocketTransform)
		m_pSocketTransform->Bind_WorldMatrix(CMethod::XMMatrixLoadFloat4x4_P(m_SocketMatrix));
}

void CSword::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CSword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::RenderLight()
{
	if (FAILED(__super::RenderLight()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	m_pSocketTransform = CTransform::Create(m_pDevice, m_pContext);
	return S_OK;
}

CSword* CSword::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CSword* pInstance = new CSword(_pDevice, _pContext);
	CHECK_CREATE_PROTOTYPE_NOTVALUE(pInstance, "CSword::Create To Failed");
	return pInstance;
}

CGameObject* CSword::Clone(void* pArg)
{
	CSword* pInstance = new CSword(*this);
	CHECK_CREATE_CLONE(pInstance, pArg,"CSword::Clone To Failed");
	return pInstance;
}

void CSword::Free()
{
	Safe_Release(m_pTrailObject);
	Safe_Release(m_pSocketTransform);
	__super::Free();
}
