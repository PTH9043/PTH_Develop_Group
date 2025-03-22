#include "Tanjiro_KeyBoard_Controller.h"
#include "Player.h"

CTanjiro_KeyBoard_Controller::CTanjiro_KeyBoard_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CPlayer_KeyBoard_Controller(_pDevice, _pContext)
{
}

HRESULT CTanjiro_KeyBoard_Controller::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

void CTanjiro_KeyBoard_Controller::Tick(const _double& _dTimeDelta)
{
	m_InputList.clear();
	__super::Tick(_dTimeDelta);
}

void CTanjiro_KeyBoard_Controller::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CTanjiro_KeyBoard_Controller::Insert_Datas(VOIDDATAS _pData)
{
	m_ReceiveVoidDatas = _pData;
}

VOIDDATAS CTanjiro_KeyBoard_Controller::Out_Datas()
{
	m_OutVoidDatas.clear();
	m_OutVoidDatas.reserve(1000);
	// Push State Data
	m_OutVoidDatas.push_back(&m_TriggerList);
	// Transform
	m_OutVoidDatas.push_back(&m_vPlayerDir);

	return m_OutVoidDatas;
}


CTanjiro_KeyBoard_Controller* CTanjiro_KeyBoard_Controller::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CGameObject* _pGameObject)
{
	CTanjiro_KeyBoard_Controller* pInstance = new CTanjiro_KeyBoard_Controller(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pGameObject)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CTanjiro_Controller::Create To Failed");
#endif
	}
	return pInstance;
}

void CTanjiro_KeyBoard_Controller::Free()
{
	__super::Free();
}
