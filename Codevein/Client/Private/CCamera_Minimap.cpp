#include "CCamera_Minimap.h"
#include "CPlayer.h"
#include "CGameInstance.h"
#include "CCamera_Minimap.h"

CCamera_Minimap::CCamera_Minimap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CCamera_Minimap::CCamera_Minimap(const CCamera_Minimap& _rhs)
	: CCamera(_rhs)
{
}

HRESULT CCamera_Minimap::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Minimap::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	if (FAILED(InitializeCamSetting()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Minimap::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CCamera_Minimap::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	if (nullptr == m_pPlayer)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance);
		{
			m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_STATIC,
				LAYER_PLAYER, 0));
		}
		END_SINGLETON(pGameInstance);
	}
	else
	{
		// Setting Position
		m_pTransformCom->Set_Pos(m_pPlayer->Get_OffsetPos());
	}
}

HRESULT CCamera_Minimap::Render()
{
	return E_NOTIMPL;
}

HRESULT CCamera_Minimap::InitializeCamSetting()
{
	if (nullptr != m_pTransformCom)
		m_pTransformCom->Rotate_Fix(XMVectorSet(1.f, 0.f, 0.f, 0.f), 90.f);

	return S_OK;
}

CCamera_Minimap* CCamera_Minimap::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Minimap* pInstance = new CCamera_Minimap(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCamera_Minimap::Create To Failed");
	}
	return pInstance;
}

CGameObject* CCamera_Minimap::Clone(void* pArg)
{
	CCamera_Minimap* pInstance = new CCamera_Minimap(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCamera_Minimap::Clone To Failed");
	}
	return pInstance;
}

void CCamera_Minimap::Free()
{
	__super::Free();
}
