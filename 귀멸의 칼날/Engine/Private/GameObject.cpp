#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "Transform.h"

CGameObject::CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

CGameObject::CGameObject(const CGameObject& _Prototype)
	: m_bClone(true), m_pDevice(_Prototype.m_pDevice), m_pContext(_Prototype.m_pContext), m_bActive(true)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

CComponent* CGameObject::Get_Component(const _tchar* pComponentTag)
{
	return Find_Component(pComponentTag);
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct_Clone(void* pArg)
{
	return S_OK;
}

void CGameObject::Tick(const _double& _dTimeDelta)
{

}

void CGameObject::Late_Tick(const _double& _dTimeDelta)
{

}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::RenderLight()
{
	return S_OK;
}

void CGameObject::Extern_ImGuiClass()
{
}

HRESULT CGameObject::Add_Component(const _tchar* pComponentTag, const _uint& _iLevelIndex,
	const _tchar* pPrototypeTag, class CComponent** ppOut, void* pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CComponent* pComponent = pGameInstance->Clone_Component(_iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.insert(pair<const _tchar*, CComponent*>(pComponentTag, pComponent));

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CGameObject::Ready_Transform(const _tchar* _pTag)
{
	m_pTransformCom = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	m_Components.insert(pair<const _tchar*, CComponent*>(_pTag, m_pTransformCom));

	Safe_AddRef(m_pTransformCom);
	return S_OK;
}

CComponent* CGameObject::Find_Component(const _tchar* pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);
}
