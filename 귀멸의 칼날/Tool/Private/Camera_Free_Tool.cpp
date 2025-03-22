#include "Camera_Free_Tool.h"
#include "GameInstance.h"

CCamera_Free_Tool::CCamera_Free_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CCamera_Free_Tool::CCamera_Free_Tool(const CCamera_Free_Tool& _rhs) 
	: CCamera(_rhs)
{
}

HRESULT CCamera_Free_Tool::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free_Tool::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free_Tool::Tick(const _double& _dTimeDelta)
{
	if (false == m_bActive)
		return;
	Move(_dTimeDelta);
	__super::Tick(_dTimeDelta);
}

void CCamera_Free_Tool::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CCamera_Free_Tool::Render()
{
	return S_OK;
}

void CCamera_Free_Tool::Move(const _double& _dDeltaSeconds)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		if (true == pGameInstance->Get_DIMBtnPressing(CInput_Device::DIMB_R))
		{
			if (pGameInstance->Get_DIKeyPressing(DIK_W))
				m_pTransformCom->Move_Forward(_dDeltaSeconds, m_tCameraDesc.fSpeed * 10.f);
			if (pGameInstance->Get_DIKeyPressing(DIK_A))
				m_pTransformCom->Move_Left(_dDeltaSeconds, m_tCameraDesc.fSpeed * 10.f);
			if (pGameInstance->Get_DIKeyPressing(DIK_D))
				m_pTransformCom->Move_Right(_dDeltaSeconds, m_tCameraDesc.fSpeed * 10.f);
			if (pGameInstance->Get_DIKeyPressing(DIK_S))
				m_pTransformCom->Move_Back(_dDeltaSeconds, m_tCameraDesc.fSpeed * 10.f);

			_float2 MousePos = pGameInstance->Get_MousePosition();
			if (MousePos.x > 0 && MousePos.x < g_iWinSizeX)
			{
				if (MousePos.y > 0 && MousePos.y < g_iWinSizeY)
				{
					_long		MouseMove = 0;

					if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
						m_pTransformCom->Rotate_Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _dDeltaSeconds * MouseMove * 0.1f, m_tCameraDesc.fRotSpeed);

					if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
						m_pTransformCom->Rotate_Turn(m_pTransformCom->Get_Right(), _dDeltaSeconds * MouseMove * 0.1f, m_tCameraDesc.fRotSpeed);
				}
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

CCamera_Free_Tool* CCamera_Free_Tool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Free_Tool* pInstance = new CCamera_Free_Tool(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("CCamera_Free_Tool::Create to Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Free_Tool::Clone(void* pArg)
{
	CCamera_Free_Tool* pInstance = new CCamera_Free_Tool(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("CCamera_Free_Tool::Clone to Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Free_Tool::Free()
{
	__super::Free();
}
