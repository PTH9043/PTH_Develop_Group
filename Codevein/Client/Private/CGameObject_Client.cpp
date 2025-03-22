#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CGameObject_Client.h"

CGameObject_Client::CGameObject_Client(ID3D11Device *& _pDevice, ID3D11DeviceContext *& _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CGameObject_Client::CGameObject_Client(const CGameObject_Client & _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CGameObject_Client::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameObject_Client::NativeConstruct_Clone(void * pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;
	return S_OK;
}

void CGameObject_Client::Tick(const _double & _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CGameObject_Client::Late_Tick(const _double & _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CGameObject_Client::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Client::Reset_TransformData()
{
	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tGobDesc.v3Pos));
		m_pTransformCom->Set_Scale(m_tGobDesc.v3Scale);
		m_pTransformCom->Rotate_Turn(m_tGobDesc.v3Rotate);
	}

	return S_OK;
}

HRESULT CGameObject_Client::Receive_ModelNames()
{
	return E_NOTIMPL;
}


_bool CGameObject_Client::Extern_EditValue()
{
	return true;
}

_bool CGameObject_Client::Extern_HierakeyView(const string& _str)
{
	return true;
}

HRESULT CGameObject_Client::Create_Self(CGameObject ** _ppOut)
{
	return S_OK;
}

HRESULT CGameObject_Client::Delete_Self()
{
	return S_OK;
}

HRESULT CGameObject_Client::Save_Data()
{
	return S_OK;
}

HRESULT CGameObject_Client::Load_Data(CGameObject ** _ppOut, const LEVEL& _eLevel, void * _pData)
{
	return S_OK;
}

_bool CGameObject_Client::Is_Picking(_float3* _pOut)
{
	return false;
}

void CGameObject_Client::Reset_Data()
{
	if (nullptr != m_pTransformCom)
	{
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tGobDesc.v3Pos));
		m_pTransformCom->Set_Scale(m_tGobDesc.v3Scale);
		m_pTransformCom->Rotate_Turn(m_tGobDesc.v3Rotate);
	}
}

void CGameObject_Client::Extern_GobData()
{

	_float Position[3] = { m_tGobDesc.v3Pos.x,  m_tGobDesc.v3Pos.y, m_tGobDesc.v3Pos.z };
	_float Rot[3] = { m_tGobDesc.v3Rotate.x,  m_tGobDesc.v3Rotate.y, m_tGobDesc.v3Rotate.z };
	_float Scale[3] = { m_tGobDesc.v3Scale.x,  m_tGobDesc.v3Scale.y, m_tGobDesc.v3Scale.z };

	if (ImGui::DragFloat3("Position", Position, 0.5f, -10000000.f, 10000000.f, "%.4f"))
	{
		m_tGobDesc.v3Pos = _float3(Position[0], Position[1], Position[2]);
		if(nullptr != m_pTransformCom)
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

void CGameObject_Client::Free()
{
	__super::Free();
}
