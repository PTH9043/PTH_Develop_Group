#include "CFloor_Terrain.h"

CFloor_Terrain::CFloor_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject_Client(_pDevice, _pContext)
{
}

CFloor_Terrain::CFloor_Terrain(const CFloor_Terrain& _rhs)
	: CGameObject_Client(_rhs)
{
}

HRESULT CFloor_Terrain::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CFloor_Terrain::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;
	return S_OK;
}

void CFloor_Terrain::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CFloor_Terrain::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CFloor_Terrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}
#pragma region USE_TOOL_METHOD
_bool CFloor_Terrain::Extern_EditValue()
{
	return _bool();
}

_bool CFloor_Terrain::Extern_HierakeyView(const string& _str)
{
	return _bool();
}

HRESULT CFloor_Terrain::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CFloor_Terrain::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CFloor_Terrain::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CFloor_Terrain::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CFloor_Terrain::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CFloor_Terrain::Reset_Data()
{
}
#pragma endregion USE_TOOL_METHOD
CFloor_Terrain* CFloor_Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFloor_Terrain* pInstance = new CFloor_Terrain(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CFloor_Terrain::Create To Failed");
	}
	return pInstance;
}

CFloor_Terrain* CFloor_Terrain::Clone(void* _pArg)
{
	CFloor_Terrain* pInstance = new CFloor_Terrain(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CFloor_Terrain::Clone To Failed");
	}
	return pInstance;
}

void CFloor_Terrain::Free()
{
	__super::Free();
}
