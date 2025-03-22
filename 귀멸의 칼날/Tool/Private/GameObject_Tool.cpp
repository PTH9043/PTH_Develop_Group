#include "MainView_Tool.h"
#include "GameObject_Tool.h"
#include "GameInstance.h"

CGameObject_Tool::CGameObject_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CGameObject_Tool::CGameObject_Tool(const CGameObject_Tool& _rhs)
	: CGameObject(_rhs)
{
}

const char* CGameObject_Tool::Get_Name()
{
	return nullptr;
}

const _tchar* CGameObject_Tool::Get_Proto()
{
	return nullptr;
}

const _tchar* CGameObject_Tool::Get_Layer()
{
	return nullptr;
}

HRESULT CGameObject_Tool::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject_Tool::NativeConstruct_Clone(void* pArg)
{
	return S_OK;
}

void CGameObject_Tool::Tick(const _double& _dTimeDelta)
{
}

void CGameObject_Tool::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CGameObject_Tool::Render()
{
	return S_OK;
}

HRESULT CGameObject_Tool::SetUp_ConstantValue()
{
	return S_OK;
}

HRESULT CGameObject_Tool::Ready_Components()
{
	return S_OK;
}

void CGameObject_Tool::Coll_Tick(_matrix _WorldMatrix)
{
	if (nullptr != m_pColliderCom)
	{
		m_pColliderCom->Tick(_WorldMatrix);
	}
}

_bool CGameObject_Tool::Extern_EditValue()
{
	return _bool();
}

_bool CGameObject_Tool::Extern_HierakeyView(const string& _str)
{
	return _bool();
}

#pragma region USE_TOOL_METHOD

HRESULT CGameObject_Tool::Create_Self(CGameObject** _ppOut)
{
	return S_OK;
}

HRESULT CGameObject_Tool::Delete_Self()
{
	return S_OK;
}

HRESULT CGameObject_Tool::Save_Data()
{
	return S_OK;
}

HRESULT CGameObject_Tool::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return S_OK;
}

_bool CGameObject_Tool::Is_Picking(_float3* _pOut)
{
	return _bool();
}

void CGameObject_Tool::Reset_Data()
{
	// Reset Data
	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tGobDesc.v3Pos));
		m_pTransformCom->Set_Scale(m_tGobDesc.v3Scale);
		m_pTransformCom->Rotate_Turn(m_tGobDesc.v3Rotate);
	}
}

void CGameObject_Tool::Extern_GobData(void* _pData)
{
	_float Position[3] = { m_tGobDesc.v3Pos.x,  m_tGobDesc.v3Pos.y, m_tGobDesc.v3Pos.z };
	_float Rot[3] = { m_tGobDesc.v3Rotate.x,  m_tGobDesc.v3Rotate.y, m_tGobDesc.v3Rotate.z };
	_float Scale[3] = { m_tGobDesc.v3Scale.x,  m_tGobDesc.v3Scale.y, m_tGobDesc.v3Scale.z };

	if (ImGui::DragFloat3("Position", Position, 0.5f, -10000000.f, 10000000.f, "%.4f"))
	{
		m_tGobDesc.v3Pos = _float3(Position[0], Position[1], Position[2]);
		if (nullptr != m_pTransformCom)
			m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tGobDesc.v3Pos));
	}
	if (ImGui::DragFloat3("Rotate", Rot, 0.5f, -10000000.f, 10000000.f, "%.4f"))
	{
		m_tGobDesc.v3Rotate = _float3(Rot[0], Rot[1], Rot[2]);
		if (nullptr != m_pTransformCom)
		{
			m_pTransformCom->Rotate_Turn(m_tGobDesc.v3Rotate);
		}
	}
	if (ImGui::DragFloat3("Scale", Scale, 0.5f, -10000000.f, 10000000.f, "%.4f"))
	{
		m_tGobDesc.v3Scale = _float3(Scale[0], Scale[1], Scale[2]);
		if (nullptr != m_pTransformCom)
			m_pTransformCom->Set_Scale(m_tGobDesc.v3Scale);
	}
}

void CGameObject_Tool::Control_ColliderData(void* _pData)
{
}

CGameObject* CGameObject_Tool::Clone(void* pArg)
{
	return nullptr;
}

#pragma endregion USE_TOOL_METHOD

void CGameObject_Tool::Free()
{
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	__super::Free();
}
