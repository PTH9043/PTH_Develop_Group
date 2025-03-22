#include "CMainView.h"
#include "CNavigationTool.h"
#include "CObjectTool.h"
#include "CImGui_Manager.h"
#include "CCheckPoints.h"
#include "CGameInstance.h"
#include "CData_Manager.h"

CCheckPoints::CCheckPoints(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CWorldObj(pDevice, pContext)
{
}

CCheckPoints::CCheckPoints(const CCheckPoints& _rhs)
	: CWorldObj(_rhs)
{
}

const char* CCheckPoints::Get_Name()
{
	return m_strModelName.c_str();
}

HRESULT CCheckPoints::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CCheckPoints::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != m_pCollCollider)
	{
		m_pCollCollider->Set_Scale(_float3(2.f, 0.f, 0.f));
	}
	return S_OK;
}

void CCheckPoints::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
	Find_Player();
}

void CCheckPoints::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
	Check_CollisionToPlayer();
}

HRESULT CCheckPoints::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CCheckPoints::Ready_Component()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	m_strModelName = "CheckPoint";
	{
		BEGIN_SINGLETON(CData_Manager, pDataManager)
		{
			_wstring strModelName;
			strModelName.assign(m_strModelName.begin(), m_strModelName.end());
			CModel* _pModel = static_cast<CModel*>(pDataManager->Get_WorldMgr()->Get_CheckPoints_Model());
			if (nullptr != _pModel)
			{
				m_pModelCom = static_cast<CModel*>(_pModel->Clone(nullptr));
				Safe_AddRef(m_pModelCom);
				m_Components.insert(pair<const _tchar*, CComponent*>(COM_MODEL, m_pModelCom));
			}
		}
		END_SINGLETON(pDataManager);
	}
	{
		if (FAILED(Ready_Transform(COM_TRANSFORM)))
			return E_FAIL;
	}
	{
		CCollider::COLLIDERDESC tDesc;
		tDesc.vScale = m_tColliderDesc.v3Scale;
		tDesc.vTranslation = m_tColliderDesc.v3Translate;
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
			PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pCollCollider, &tDesc)))
			return E_FAIL;
	}
	if (nullptr == m_pModelCom)
		return E_FAIL;

	return S_OK;
}

void CCheckPoints::Find_Player()
{
	if (nullptr == m_pPlayer)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance)
		{
			m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel,
				LAYER_PLAYER, 0));
		}
		END_SINGLETON(pGameInstance);
	}
}

void CCheckPoints::Check_CollisionToPlayer()
{
	if (nullptr != m_pPlayer)
	{
		CCollider* pCollider = static_cast<CCollider*>(m_pPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
		if (nullptr == pCollider)
			return;
		if (true == pCollider->Is_Collision(m_pCollCollider))
		{
			m_pPlayer->Find_CheckPoints(this);
		}
	}
}


#pragma region USE_TOOL_METHOD
_bool CCheckPoints::Extern_EditValue()
{
	_bool bActive = false;

	if (ImGui::TreeNodeEx("CheckPoints"))
	{
		ImGui::PushItemWidth(200);

		Extern_GobData();
		Show_Model();
		{
			BEGIN_SINGLETON(CImGui_Manager, pImGuiManager);
			{
				CGameObject* pDebugModel = pImGuiManager->Get_DebugObject();
				if (nullptr != pDebugModel)
				{
					CTransform* pTransform = (CTransform*)pDebugModel->Get_Component(COM_TRANSFORM);
					if (nullptr != pTransform)
					{
						pTransform->Set_Pos(XMLoadFloat3(&m_tGobDesc.v3Pos));
						pTransform->Set_Scale(m_tGobDesc.v3Scale);
					}
				}
			}
			END_SINGLETON(pImGuiManager);
		}
		bActive = ImGui::Button("Create");
		ImGui::PopItemWidth();
		ImGui::TreePop();
		return bActive;
	}
	return false;
}

_bool CCheckPoints::Extern_HierakeyView(const string& _str)
{
	return __super::Extern_HierakeyView(_str);
}

HRESULT CCheckPoints::Create_Self(CGameObject** _ppOut)
{
	return __super::Create_Self(_ppOut);
}

HRESULT CCheckPoints::Delete_Self()
{
	return __super::Delete_Self();
}

HRESULT CCheckPoints::Save_Data()
{
	return __super::Save_Data();
}

HRESULT CCheckPoints::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return __super::Load_Data(_ppOut, _eLevel, _pData);
}

_bool CCheckPoints::Is_Picking(_float3* _pOut)
{
	return __super::Is_Picking(_pOut);
}

_bool CCheckPoints::Is_PickingNav()
{
	return __super::Is_PickingNav();
}

void CCheckPoints::Reset_Data()
{
	__super::Reset_Data();
}
#pragma endregion USE_TOOL_METHOD

HRESULT CCheckPoints::Show_Model()
{
	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		pDataManager->Get_WorldMgr()->Setting_CheckPointModel();
	}
	END_SINGLETON(pDataManager);
	return S_OK;
}

CCheckPoints* CCheckPoints::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCheckPoints* pInstance = new CCheckPoints(pDevice, pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCheckPoints::Create To Failed");
	}
	return pInstance;
}

CGameObject* CCheckPoints::Clone(void* _pArg)
{
	CCheckPoints* pInstance = new CCheckPoints(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCheckPoints::Clone To Failed");
	}
	return pInstance;
}

void CCheckPoints::Free()
{
	__super::Free();
}
