#include "Controller.h"
#include "Method.h"
#include "GameObject.h"

CController::CController(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice), m_pContext(_pContext)
{
	Safe_AddRef_DV(m_pDevice);
	Safe_AddRef_DV(m_pContext);
}

HRESULT CController::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	const _char* pName = typeid(this).name();
	m_strName = CMethod::ConvertSToW(_string(pName));
	this->m_pOwnerObject = _pGameObject;
	return S_OK;
}

void CController::Tick(const _double& _dTimeDelta)
{
}

void CController::Late_Tick(const _double& _dTimeDelta)
{
}

void CController::Insert_Datas(VOIDDATAS _pData)
{
	m_ReceiveVoidDatas = _pData;
}

VOIDDATAS CController::Out_Datas()
{
	return m_OutVoidDatas;
}

//void CController::Insert_Datas(VOIDDATAS _pData)
//{
//}
//
//void CController::Out_Datas(VOIDDATAS* _pData)
//{
//}

void CController::Free()
{
	Safe_Release_DV(m_pContext);
	Safe_Release_DV(m_pDevice);
}
