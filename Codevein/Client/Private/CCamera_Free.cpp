#include "CCamera_Free.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CSkyBox.h"
#include "CStage.h"
#include "CCell.h"
#include "CCollider.h"
#include "CCinematicCamera.h"

CCamera_Free::CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& _rhs)
	: CCamera(_rhs)
{
}

void CCamera_Free::Target_Mode(const _bool& _bIsTargetMode, _fvector _vPos, _fvector _vTargetPos)
{
	m_bTargetMode = _bIsTargetMode;
	XMStoreFloat3(&m_v3Fix_LockOnPos, _vPos);
	XMStoreFloat3(&m_v3Fix_TargetPos, _vTargetPos);
}

void CCamera_Free::Insert_CinematicCam(CCinematicCamera* _pCamera)
{
}

HRESULT CCamera_Free::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_bActive = true;
	return S_OK;
}

HRESULT CCamera_Free::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(_pArg)))
		return E_FAIL;

	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		pGameInstance->Set_CursorMode(CInput_Device::CURSOR_LOCKED);

		CCollider::COLLIDERDESC tDesc;
		tDesc.vScale = _float3(1.f, 1.f, 1.f);
		tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
		m_pCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER, &tDesc));
	}
	END_SINGLETON(pGameInstance);
	m_v3SupplementPos = _float3(0.f, +4.f, -3.f);

	m_pTransformCom->Set_Pos(XMLoadFloat3(&m_v3SupplementPos));

	BEGIN_SINGLETON(CData_Manager, pDataManager)
	{
		m_pStage = pDataManager->Get_StageMgr()->Get_Stage();
		Safe_AddRef(m_pStage);

		if (nullptr != m_pStage)
		{
			m_pRegion = m_pStage->Get_Region(0);

			if (nullptr != m_pRegion)
			{
				CNavigation::NAVIDESC tDesc;
				tDesc.iCurIndex = 0;
				m_pNavigation = static_cast<CNavigation*>(m_pRegion->Get_Navigation()->Clone(&tDesc));
			}
		}
	}
	END_SINGLETON(pDataManager);
	
	m_bActive = true;
	return S_OK;
}

void CCamera_Free::Tick(const _double& _dTimeDelta)
{
	if (true == m_bActive)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance)
		{
			if (pGameInstance->Get_DIKeyDown(DIK_ESCAPE))
				pGameInstance->Set_CursorMode(CInput_Device::CORSOR_NONE);

			if (CInput_Device::CURSOR_LOCKED == pGameInstance->Get_CursorMode())
			{
			}
			else
			{
				if (pGameInstance->Get_DIKeyDown(DIK_LCONTROL))
					pGameInstance->Set_CursorMode(CInput_Device::CURSOR_LOCKED);
			}
			Rotation(_dTimeDelta);
			Calculate_Pos();
			Move(_dTimeDelta);

			if (nullptr != m_pSkyBox)
			{
				CTransform* pTransform = static_cast<CTransform*>(m_pSkyBox->Get_Component(COM_TRANSFORM));
				if (nullptr == pTransform)
				{
					END_SINGLETON(pGameInstance);
					return;
				}
				pTransform->Set_Pos(m_pTransformCom->Get_Pos());
			}
			else
			{
				m_pSkyBox = static_cast<CSkyBox*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_SKYBOX, 0));
			}
		}
		END_SINGLETON(pGameInstance);

		Collision_Wall();
	}
	else
	{
		Move(_dTimeDelta);
	}
	__super::Tick(_dTimeDelta);
}

void CCamera_Free::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CCamera_Free::Render()
{
	return S_OK;
}

void CCamera_Free::Calculate_Pos()
{
	if (nullptr == m_pTargetTransform)
		return;

	if (false == m_bTargetMode)
	{
		_vector pos = XMLoadFloat3(&m_v3OffsetPos) + XMLoadFloat3(&m_v3SupplementPos);
		XMStoreFloat3(&m_v3GoalPos, pos);
	}
	else
	{
		_vector pos = XMLoadFloat3(&m_v3OffsetPos) + XMLoadFloat3(&m_v3Fix_LockOnPos);
		XMStoreFloat3(&m_v3GoalPos, pos);
	}
}

_float CCamera_Free::PlayerToDot_DirToSelfLook()
{
	return CFuncClass::Dot_DirToSelfLook(m_pTransformCom->Get_Look(), 
		m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos());
}

_float CCamera_Free::PlayerToDot_DirToSelfRight()
{
	return CFuncClass::Dot_DirToSelfRight(m_pTransformCom->Get_Right(),
		m_pTargetTransform->Get_Pos() - m_pTransformCom->Get_Pos());
}

_float CCamera_Free::PlayerToDot_DirToSelfLookConverter()
{
	_float fDot = XMConvertToDegrees(acosf(PlayerToDot_DirToSelfLook()));
	_float fValue = PlayerToDot_DirToSelfRight();

	if (fValue < 0)
		fDot = -fDot;

	return fDot;
}

void CCamera_Free::Rotation(const _double& _dDeltaSeconds)
{
	if (false == m_bTargetMode)
	{
		BEGIN_SINGLETON(CGameInstance, pGameInstance)
		{
			_float2 MousePos = pGameInstance->Get_MousePosition();
			if (MousePos.x > 0 && MousePos.x < g_iWinSizeX)
			{
				if (MousePos.y > 0 && MousePos.y < g_iWinSizeY)
				{
					_long		MouseMoveX = 0, MouseMoveY = 0;
					_matrix matWorld = XMMatrixIdentity();
					{
						if (MouseMoveX = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
						{
							m_fMouseX = (_float)(MouseMoveX) * 0.05f;
							if (MouseMoveX > 5 || MouseMoveX < -5)
							{
								if (false == m_bYMove)
								{
									if (m_fMouseX > 1.f)
										m_fMouseX = 1.f;
									else if (m_fMouseX < -1.f)
										m_fMouseX = -1.f;

									m_fRotationXAngle = (_float)(m_fMouseX) * 5.f;
									matWorld = XMMatrixRotationRollPitchYaw(0.f,
										XMConvertToRadians(m_fRotationXAngle), 0.f);
									_float3 v3Pos;
									XMStoreFloat3(&v3Pos, XMVector3TransformCoord(XMLoadFloat3(&m_v3SupplementPos),
										matWorld));

									m_v3SupplementPos.x = v3Pos.x;
									m_v3SupplementPos.z = v3Pos.z;
									m_bXMove = true;
								}
							}
							else
								m_bXMove = false;
						}
						if (MouseMoveY = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
						{
							m_fMouseY = (_float)(MouseMoveY) * 0.05f;
							if (MouseMoveY > 5 || MouseMoveY < -5)
							{
								if (false == m_bXMove)
								{
									if (m_fMouseY > 1.f)
										m_fMouseY = 1.f;
									else if (m_fMouseY < -1.f)
										m_fMouseY = -1.f;

									m_fRotationYAngle = (_float)(-m_fMouseY) * 7.f;

									matWorld = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotationYAngle), 0.f, 0.f);
									_float3 v3Pos;
									XMStoreFloat3(&v3Pos, XMVector3TransformCoord(XMLoadFloat3(&m_v3SupplementPos),
										matWorld));

									m_v3SupplementPos.y = v3Pos.y;

									if (m_v3SupplementPos.y > 2.f)
										m_v3SupplementPos.y = 2.f;

									m_bYMove = true;
								}
							}
							else
								m_bYMove = false;
						}
					}
				}
			}
		}
		END_SINGLETON(pGameInstance);
	}
}

void CCamera_Free::Move(const _double& _dDeltaSeconds)
{
	if (nullptr == m_pTargetTransform)
		return;

	if (false == m_bTargetMode)
	{

		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_v3GoalPos));
		m_pTransformCom->LookAt(XMLoadFloat3(&m_v3OffsetPos));
	}
	else
	{
		m_pTransformCom->Set_Pos(XMLoadFloat3(&m_v3GoalPos));
		m_pTransformCom->LookAt(XMLoadFloat3(&m_v3Fix_TargetPos));
	}
}

void CCamera_Free::Collision_Wall()
{

}

_float CCamera_Free::CampAngle(_float _fAngle, const _float& _fMin, const _float& _fMax, const _float& fRadius)
{
	if (_fAngle > fRadius)
		_fAngle -= fRadius;
	else if (_fAngle < -fRadius)
		_fAngle += fRadius;

	if (_fAngle < _fMin)
		_fAngle = _fMin;
	else if (_fAngle > _fMax)
		_fAngle = _fMax;
	return _fAngle;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(_pDevice, _pContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("CCamera_Free::Create to Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		MSG_BOX("CCamera_Free::Clone to Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Free::Free()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		pGameInstance->Set_CursorMode(CInput_Device::CORSOR_NONE);
	}
	END_SINGLETON(pGameInstance);

	Safe_Release(m_pNavigation);
	Safe_Release(m_pStage);
	Safe_Release(m_pCollider);

	Safe_Release(m_pTargetTransform);
	__super::Free();
}


/*
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		_float2 MousePos = pGameInstance->Get_MousePosition();
		if (MousePos.x > 0 && MousePos.x < g_iWinSizeX)
		{
			if (MousePos.y > 0 && MousePos.y < g_iWinSizeY)
			{
				_long		MouseMove = 0;

				if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
				{
					m_fAngle += (_float)(10.f * _dDeltaSeconds);
				}
			}
		}
	}
	END_SINGLETON(pGameInstance);

	m_pTransformCom->Translate_Pos(XMLoadFloat3(&m_v3GoalPos), _dDeltaSeconds);
*/