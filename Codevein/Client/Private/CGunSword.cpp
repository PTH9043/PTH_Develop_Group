#include "CGunSword.h"

CGunSword::CGunSword(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CWeapon(_pDevice, _pContext)
{
}

CGunSword::CGunSword(const CGunSword& _rhs)
	: CWeapon(_rhs)
{
}

HRESULT CGunSword::NativeConstruct_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CGunSword::NativeConstruct_Clone(void* pArg)
{
	return E_NOTIMPL;
}

void CGunSword::Tick(const _double& _dTimeDelta)
{
}

void CGunSword::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CGunSword::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGunSword::Ready_Component()
{
	return E_NOTIMPL;
}

HRESULT CGunSword::SetUp_ConstantTable()
{
	return E_NOTIMPL;
}
#pragma region USE_TOOL_METHOD
_bool CGunSword::Extern_EditValue()
{
	return _bool();
}

_bool CGunSword::Extern_HierakeyView(const string& _str)
{
	return _bool();
}

HRESULT CGunSword::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CGunSword::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CGunSword::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CGunSword::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CGunSword::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CGunSword::Reset_Data()
{
	__super::Reset_Data();
}
#pragma endregion USE_TOOL_METHOD

CGunSword* CGunSword::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	return nullptr;
}

CGameObject* CGunSword::Clone(void* pArg)
{
	return nullptr;
}

void CGunSword::Free()
{
	__super::Free();
}
