#include "MainView_Tool.h"
#include "Pawn_Tool.h"
#include "GameInstance.h"

CPawn_Tool::CPawn_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CGameObject_Tool(_pDevice, _pContext)
{
}

CPawn_Tool::CPawn_Tool(const CPawn_Tool& _rhs)
	: CGameObject_Tool(_rhs)
{
}

const char* CPawn_Tool::Get_Name()
{
	return nullptr;
}

const _tchar* CPawn_Tool::Get_Proto()
{
	return nullptr;
}

const _tchar* CPawn_Tool::Get_Layer()
{
	return nullptr;
}

HRESULT CPawn_Tool::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPawn_Tool::NativeConstruct_Clone(void* pArg)
{
	return S_OK;
}

void CPawn_Tool::Tick(const _double& _dTimeDelta)
{
}

void CPawn_Tool::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CPawn_Tool::Render()
{
	return S_OK;
}

HRESULT CPawn_Tool::Ready_Components()
{
	return S_OK;
}

#pragma region USE_TOOL_METHOD

_bool CPawn_Tool::Extern_EditValue()
{
	return _bool();
}

_bool CPawn_Tool::Extern_HierakeyView(const string& _str)
{
	return _bool();
}

HRESULT CPawn_Tool::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CPawn_Tool::Delete_Self()
{
	return E_NOTIMPL;
}

HRESULT CPawn_Tool::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CPawn_Tool::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CPawn_Tool::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CPawn_Tool::Reset_Data()
{
	// Reset Data
	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tGobDesc.v3Pos));
		m_pTransformCom->Set_Scale(m_tGobDesc.v3Scale);
		m_pTransformCom->Rotate_Turn(m_tGobDesc.v3Rotate);
	}
}

void CPawn_Tool::Control_ColliderData(void* _pData)
{
}

CGameObject* CPawn_Tool::Clone(void* pArg)
{
	return nullptr;
}

#pragma endregion USE_TOOL_METHOD

void CPawn_Tool::Free()
{
	__super::Free();
}
