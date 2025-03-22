#include "CPlayerUIController.h"
#include "CGameInstance.h"

CPlayerUIController::CPlayerUIController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPlayerUIController::NativeConstruct(CPlayer* _pOwner)
{
	this->m_pOwner = _pOwner;

	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		// CKwownKey_UI E
		{
			CUserInterface::UIDESC tUIDesc;
			tUIDesc = CUserInterface::UIDESC(L"Key_E",
				_float2((_float)(g_iWinSizeX >> 1), 600.f), _float2(40.f, 40.f), 0.1f);
			m_pKnownKey_E = static_cast<CKnownKey_UI*>(pGameInstance->Clone_GameObject_Add(
				g_iCurrentLevel, LAYER_UI, PROTO_GOB_UI_KNOWNKEY_UI, &tUIDesc));
			Safe_AddRef(m_pKnownKey_E);
		}

		{
			CUserInterface::UIDESC tUIDesc;
			tUIDesc = CUserInterface::UIDESC(L"CheckPoint_Background",
				_float2((_float)(g_iWinSizeX) / 2.f, (_float)(g_iWinSizeY) / 2.f), _float2((_float)g_iWinSizeX, (_float)g_iWinSizeY), 0.1f);
			m_pCheckPointInterface = static_cast<CCheckPointInterface*>(pGameInstance->Clone_GameObject_Add(
				g_iCurrentLevel, LAYER_UI, PROTO_GOB_UI_CHECKPOINTINTERFACE, &tUIDesc));
			Safe_AddRef(m_pCheckPointInterface);
		}
	}
	END_SINGLETON(pGameInstance);

	return S_OK;
}

CPlayerUIController* CPlayerUIController::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CPlayer* _pOwner)
{
	CPlayerUIController* pInstance = new CPlayerUIController(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pOwner)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPlayerUIController::Create To Failed");
	}
	return pInstance;
}

void CPlayerUIController::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pKnownKey_E);
	Safe_Release(m_pCheckPointInterface);
	__super::Free();
}
