#include "CCinematicCamera.h"

CCinematicCamera::CCinematicCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CCinematicCamera::CCinematicCamera(const CCinematicCamera& _rhs) 
	: CCamera(_rhs)
{
}

HRESULT CCinematicCamera::NativeConstruct_Prototype()
{
	m_bActive = false;
	return S_OK;
}

HRESULT CCinematicCamera::NativeConstruct_Clone(void* _pArg)
{
	return S_OK;
}

void CCinematicCamera::Tick(const _double& _dTimeDelta)
{
}

void CCinematicCamera::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CCinematicCamera::Render()
{
	return S_OK;
}

CCinematicCamera* CCinematicCamera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCinematicCamera* pInstance = new CCinematicCamera(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCinematicCamera::Create To Failed");
	}
	return pInstance;
}

CGameObject* CCinematicCamera::Clone(void* pArg)
{
	CGameObject* pInstance = new CCinematicCamera(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CCinematicCamera::Create To Failed");
	}
	return pInstance;
}

void CCinematicCamera::Free()
{
	__super::Free();
}
