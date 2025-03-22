#include "AIController_Zako.h"
#include "Zako.h"
#include "GameInstance.h"

CAIController_Zako::CAIController_Zako(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CAI_CharController(_pDevice, _pContext)
{
}

HRESULT CAIController_Zako::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	m_pZako = static_cast<CZako*>(_pGameObject);
	return S_OK;
}

void CAIController_Zako::Tick(const _double& _dTimeDelta)
{
}

void CAIController_Zako::Late_Tick(const _double& _dTimeDelta)
{
}

void CAIController_Zako::Judge_State(const _double& _dTimeDelta)
{
}

CAIController_Zako* CAIController_Zako::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CGameObject* _pGameObject)
{
	CAIController_Zako* pInstance = new CAIController_Zako(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pGameObject)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CAIController_Zako::Create To Failed");
#endif
	}
	return pInstance;
}

void CAIController_Zako::Free(void)
{
    __super::Free();
}
