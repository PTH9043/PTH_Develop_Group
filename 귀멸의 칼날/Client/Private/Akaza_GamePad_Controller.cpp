#include "Akaza_GamePad_Controller.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Akaza.h"

CAkaza_GamePad_Controller::CAkaza_GamePad_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPlayer_GamePad_Controller(_pDevice, _pContext)
{
}

HRESULT CAkaza_GamePad_Controller::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);
	m_pAkaza = static_cast<CAkaza*>(_pGameObject);

	return S_OK;
}

void CAkaza_GamePad_Controller::Tick(const _double& _dTimeDelta)
{
	m_InputList.clear();
	__super::Tick(_dTimeDelta);
}

void CAkaza_GamePad_Controller::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CAkaza_GamePad_Controller::Insert_Datas(VOIDDATAS _pData)
{
	m_ReceiveVoidDatas.clear();
	m_ReceiveVoidDatas = _pData;
}

VOIDDATAS CAkaza_GamePad_Controller::Out_Datas()
{
	m_OutVoidDatas.clear();
	m_OutVoidDatas.reserve(1000);
	// Push State Data
	m_OutVoidDatas.push_back(&m_TriggerList);
	// Transform
	m_OutVoidDatas.push_back(&m_vPlayerDir);

	if (nullptr != m_pTargetPlayer)
		m_OutVoidDatas.push_back(m_pTargetPlayer->Get_Transform());
	return m_OutVoidDatas;
}

CAkaza_GamePad_Controller* CAkaza_GamePad_Controller::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CGameObject* _pGameObject)
{
	CAkaza_GamePad_Controller* pInstance = new CAkaza_GamePad_Controller(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pGameObject)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("Akaza_GamePad_Controller::Create To Failed");
#endif
	}
	return pInstance;
}

void CAkaza_GamePad_Controller::Free()
{
	__super::Free();
}
