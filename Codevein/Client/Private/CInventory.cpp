#include "CInventory.h"
#include "CItemSlot.h"
#include "CGameInstance.h"

CInventory::CInventory(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CUserInterface(_pDevice, _pContext)
{
}

CInventory::CInventory(const CInventory& _rhs)
	: CUserInterface(_rhs)
{
}

HRESULT CInventory::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CInventory::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CInventory::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CInventory::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CInventory::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventory::Ready_Component()
{
	if (FAILED(__super::Ready_Transform(COM_TRANSFORM)))
		return E_FAIL;
	{
		{
			CRectTransform::RECTDESC tRectDesc;
			tRectDesc.v2Pos = m_tUIDesc.v2Pos;
			tRectDesc.v2Size = m_tUIDesc.v2Size;
			XMStoreFloat4x4(&tRectDesc.ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));
			if (FAILED(__super::Ready_RectTransform(COM_RECTRANSFORM, &tRectDesc)))
				return E_FAIL;

			m_pRectTransformCom->Update_Rect(m_pTransformCom, g_iWinSizeX, g_iWinSizeY, m_tUIDesc.fZBufferOrder);
		}
	}

	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT, (CComponent**)&m_pVIRectCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC, PROTO_COMP_TEXTURE_UIGROUP, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventory::SetUp_ConstantTable()
{
	return S_OK;
}

CInventory* CInventory::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CInventory* pInstance = new CInventory(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CInventory::Create To Failed");
	}
	return pInstance;
}

CGameObject* CInventory::Clone(void* _pArg)
{
	CInventory* pInstance = new CInventory(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CInventory::Create To Failed");
	}
	return pInstance;
}

void CInventory::Free()
{
	for (auto& iter : m_SlotList)
		Safe_Release(iter);

	Safe_Release(m_pVIRectCom);
	Safe_Release(m_pRenderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	__super::Free();
}
