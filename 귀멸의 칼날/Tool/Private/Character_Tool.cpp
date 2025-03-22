#include "Character_Tool.h"

CCharacter_Tool::CCharacter_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPawn_Tool(_pDevice, _pContext)
{
}

CCharacter_Tool::CCharacter_Tool(const CCharacter_Tool& _rhs) 
	: CPawn_Tool(_rhs)
{
}

HRESULT CCharacter_Tool::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CCharacter_Tool::NativeConstruct_Clone(void* pArg)
{
	m_ePawnType = PAWN_CHARACTER;
	return S_OK;
}

void CCharacter_Tool::Tick(const _double& _dTimeDelta)
{
}

void CCharacter_Tool::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CCharacter_Tool::Render()
{
	return E_NOTIMPL;
}

HRESULT CCharacter_Tool::Reset_TransformData()
{
	return E_NOTIMPL;
}

HRESULT CCharacter_Tool::Receive_ModelNames()
{
	return E_NOTIMPL;
}

_bool CCharacter_Tool::Extern_EditValue()
{
	return _bool();
}

_bool CCharacter_Tool::Extern_HierakeyView(const string& _str)
{
	return _bool();
}

HRESULT CCharacter_Tool::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CCharacter_Tool::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CCharacter_Tool::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CCharacter_Tool::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CCharacter_Tool::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CCharacter_Tool::Reset_Data()
{
}

void CCharacter_Tool::Control_ColliderData(void* _pData)
{
}

CGameObject* CCharacter_Tool::Clone(void* pArg)
{
	return nullptr;
}

void CCharacter_Tool::Free()
{
}
