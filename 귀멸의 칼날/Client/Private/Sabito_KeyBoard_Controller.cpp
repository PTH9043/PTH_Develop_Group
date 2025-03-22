#include "Sabito_KeyBoard_Controller.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Player.h"
#include "Sabito.h"

CSabito_KeyBoard_Controller::CSabito_KeyBoard_Controller(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext)
	: CPlayer_KeyBoard_Controller(_pDevice, _pContext)
{
}

HRESULT CSabito_KeyBoard_Controller::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;
	m_pSabito = static_cast<CSabito*>(_pGameObject);
	m_pPlayer = static_cast<CSabito*>(_pGameObject);
	return S_OK;
}

void CSabito_KeyBoard_Controller::Tick(const _double& _dTimeDelta)
{
	m_InputList.clear();
	__super::Tick(_dTimeDelta);
	//Change_State(_dTimeDelta);
}

void CSabito_KeyBoard_Controller::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CSabito_KeyBoard_Controller::Insert_Datas(VOIDDATAS _pData)
{
	m_ReceiveVoidDatas = _pData;
}

VOIDDATAS CSabito_KeyBoard_Controller::Out_Datas()
{
	m_OutVoidDatas.clear();
	m_OutVoidDatas.reserve(1000);
	// Push State Data
	m_OutVoidDatas.push_back(&m_TriggerList);
	// Transofrm
	m_OutVoidDatas.push_back(&m_vPlayerDir);

	if(nullptr != m_pTargetPlayer)
		m_OutVoidDatas.push_back(m_pTargetPlayer->Get_Transform());

	return m_OutVoidDatas;
}

void CSabito_KeyBoard_Controller::Change_State(const _double& _dTimeDelta)
{
	if (m_ReceiveVoidDatas.size() <= 0)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		Find_CameraFree(pGameInstance);
		Find_TargetPlayer(pGameInstance);

		if (nullptr != m_pCamera && nullptr != m_pTargetPlayer)
		{
			CTransform* pCamTransform = m_pCamera->Get_Transform();
			CTransform* pTargetPlayer = m_pTargetPlayer->Get_Transform();

			m_pCurAnimationClip = static_cast<CAnimationClip*>(m_ReceiveVoidDatas[CSabito::INPUT_CURANIMATION]);
			m_pPrevAnimationClip = static_cast<CAnimationClip*>(m_ReceiveVoidDatas[CSabito::INPUT_PREVANIMATION]);

			m_iCurStateData = CSabito::STATE_END;

			Input_Defines();
			Judge_ChargeState(_dTimeDelta, pGameInstance, pTargetPlayer, pCamTransform);
			Move_State(_dTimeDelta, pGameInstance, pTargetPlayer, pCamTransform);
			Judge_Step(_dTimeDelta, pGameInstance, pTargetPlayer, pCamTransform);
			Judge_Attack(_dTimeDelta, pGameInstance, pTargetPlayer, pCamTransform);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CSabito_KeyBoard_Controller::Move_State(const _double& _dTimeDelta, CGameInstance* _pGameInstance,
	CTransform* _pTargetTransform, CTransform* _pCameraTransform)
{
	_vector vDir = XMVectorZero();

	_vector vCamRight = _pCameraTransform->Get_Right();
	_vector vCamUp = _pCameraTransform->Get_Up();
	_vector vCamLook = _pCameraTransform->Get_Look();

	CAnimator* pAnimator = m_pSabito->Get_Animator();
	if (nullptr == pAnimator)
		return;

	_string strType = pAnimator->Get_AnimationType();

	_uint iPressing = 0;
	
	if ("Jump" == strType)
	{
		if (true == m_DownKey[DIK_J])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_JUMPATTACK;
			return;
		}
	}

	if ("JUMP_ATTACK_COMBO" == pAnimator->Get_CurrentTrigger())
	{
		if (true == m_DownKey[DIK_J])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_JUMPATTACK;
		}
		return;
	}
	
	if ("Move" == strType)
	{
		if (false == m_PressingKey[DIK_UP] || false == m_PressingKey[DIK_DOWN] || false == m_PressingKey[DIK_LEFT] || false == m_PressingKey[DIK_RIGHT])
		{
			m_iCurStateData = CSabito::STATE_IDLE + CSabito::STATE_RETURN_IDLE;
		}
		else
		{
			if (true == m_PressingKey[DIK_UP])
				iPressing = DIK_UP;
			else if (true == m_PressingKey[DIK_DOWN])
				iPressing = DIK_DOWN;
			else if (true == m_PressingKey[DIK_LEFT])
				iPressing = DIK_LEFT;
			else if (true == m_PressingKey[DIK_RIGHT])
				iPressing = DIK_RIGHT;
		}
	}

	if (true == m_DownKey[DIK_L])
	{
		vDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
		m_iCurStateData = CSabito::STATE_DASH + CSabito::STATE_DASH_GROUND;
	}
	if (true == m_DownKey[DIK_K])
	{
		m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_NOTMOVE;
	}
	if (true == m_DownKey[DIK_J])
	{
		vDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
		m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_NORMAL;
	}
	if (true == m_DownKey[DIK_I])
	{
		vDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
		m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DRAGON;
	}

	if (true == m_UpKey[DIK_J])
	{
		m_iCurStateData = CSabito::STATE_IDLE + CSabito::STATE_RETURN_IDLE;
	}

	 if (true == m_PressingKey[DIK_UP])
	{
		vDir = vCamLook;
		// 카메라의 Look을 Dir에 더해준다
		// 달리고있을때 L눌리면 STEP
		if (true == m_DownKey[DIK_L])
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_FORWARD;
		}
		else if (true == m_DownKey[DIK_J])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_AIR;
		}
		else if (true == m_DownKey[DIK_K])
		{
			m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_FORWARD;
		}
		else if (true == m_DownKey[DIK_I])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_MORNING;
		}
		else if (true == m_DownKey[DIK_O])
		{
			m_iCurStateData = CSabito::STATE_GUARD;
		}
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
		}
	}
	else if (true == m_PressingKey[DIK_DOWN])
	{
		vDir = -vCamLook;
		// 카메라의 Look을 Dir에 빼준다
		// 달리고있을때 L눌리면 STEP
		if (true == m_DownKey[DIK_L])
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
		}
		else if (true == m_DownKey[DIK_J])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_DOWN;
		}
		else if (true == m_DownKey[DIK_K])
		{
			m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_BACK;
		}
		else if (true == m_DownKey[DIK_I])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_MORNING;
		}
		else if (true == m_DownKey[DIK_O])
		{
			m_iCurStateData = CSabito::STATE_GUARD;
		}
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
		}
	}
	else if (true == m_PressingKey[DIK_LEFT])
	{
		vDir = -vCamRight;
		// 카메라의 Right을 Dir에 뻬준다
		// 달리고있을때 L눌리면 STEP
		if (true == m_DownKey[DIK_L])
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_LEFT;
		}
		else if (true == m_DownKey[DIK_J])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_NORMAL;
		}
		else if (true == m_DownKey[DIK_K])
		{
			m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_LEFT;
		}
		else if (true == m_DownKey[DIK_I])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_MORNING;
		}
		else if (true == m_DownKey[DIK_O])
		{
			m_iCurStateData = CSabito::STATE_GUARD;
		}
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
		}
	}
	else if (true == m_PressingKey[DIK_RIGHT])
	{
		vDir = vCamRight;
		// 카메라의 Look을 Dir에 빼준다
		// 달리고있을때 L눌리면 STEP
		if (true == m_DownKey[DIK_L])
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_RIGHT;
		}
		else if (true == m_DownKey[DIK_J])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_NORMAL;
		}
		else if (true == m_DownKey[DIK_K])
		{
			m_iCurStateData = CSabito::STATE_JUMP + CSabito::STATE_LEFT;
		}
		else if (true == m_DownKey[DIK_I])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_MORNING;
		}
		else if (true == m_DownKey[DIK_O])
		{
			m_iCurStateData = CSabito::STATE_GUARD;
		}
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
		}
	} 

	if (true == m_DownKey[DIK_UP])
	{
		// 공격과 이동을 동시에 눌렀다면 강공격 실행
		if (true == m_DownKey[DIK_J])
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_STRONG;
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
			vDir += vCamLook;
		}
	}
	if (true == m_DownKey[DIK_LEFT])
	{
		// 공격과 이동을 동시에 눌렀다면 강공격 실행
		if (true == m_DownKey[DIK_J])
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_STRONG;
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
			vDir -= vCamRight;
		}
	}
	if (true == m_DownKey[DIK_DOWN])
	{
		// 공격과 이동을 동시에 눌렀다면 강공격 실행
		if (true == m_DownKey[DIK_J])
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_STRONG;
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
			vDir -= vCamLook;
		}
	}
	if (true == m_DownKey[DIK_RIGHT])
	{
		// 공격과 이동을 동시에 눌렀다면 강공격 실행
		if (true == m_DownKey[DIK_J])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_STRONG;
		}
		else
		{
			m_iCurStateData = CSabito::STATE_MOVE + CSabito::STATE_FORWARD;
			vDir += vCamRight;
		}
	}

	if (true == m_PressingKey[DIK_L])
	{
		if (true == m_DownKey[DIK_UP])
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_FORWARD;
		}

		else if (true == m_DownKey[DIK_DOWN])
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
		}

		else if (true == m_DownKey[DIK_LEFT])
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_LEFT;
		}

		else if (true == m_DownKey[DIK_RIGHT])
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_RIGHT;
		}
	}

	if ("Guard" == strType)
	{
		vDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
		// O버튼이 Pressing되고 있을때
		if (m_PressingKey[DIK_O])
		{
			if (m_DownKey[DIK_UP])
			{
				m_iCurStateData = CSabito::STATE_GUARD +  CSabito::STATE_GUARD_ATTACK;
			}
			else if (m_DownKey[DIK_LEFT])
			{
				m_iCurStateData = CSabito::STATE_GUARD +  CSabito::STATE_GUARD_ATTACK;
			}
			else if (m_DownKey[DIK_DOWN])
			{
				m_iCurStateData = CSabito::STATE_GUARD +  CSabito::STATE_GUARD_ATTACK;
			}
			else if (m_DownKey[DIK_RIGHT])
			{
				m_iCurStateData = CSabito::STATE_GUARD + CSabito::STATE_GUARD_ATTACK;
			}
			else if (m_DownKey[DIK_I])
			{
				m_vPlayerDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
				m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_CATCH;
			}
		}
		else
		{
			if (m_DownKey[DIK_I])
			{
				m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
			}
			else
			{
				// Pressing되고있지 않다면 GUARD_END를 보낸다.
				m_iCurStateData = CSabito::STATE_GUARD + CSabito::STATE_GUARD_CANCEL;
			}
		}
	}
	else
	{
		// 가드
		if (m_DownKey[DIK_O] || m_PressingKey[DIK_O])
		{
			if (m_DownKey[DIK_I])
			{
				m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
			}
			else
			{
				m_iCurStateData = CSabito::STATE_GUARD + CSabito::STATE_GUARD_JUST;
				vDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
			}
		}
	}

	if (m_PressingKey[DIK_I] || m_DownKey[DIK_I])
	{
		if (m_PressingKey[DIK_O] || m_DownKey[DIK_O])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
		}
	}
	else if (m_PressingKey[DIK_O] || m_DownKey[DIK_O])
	{
		if (m_PressingKey[DIK_I] || m_DownKey[DIK_I])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
		}
	}
	
	if (m_DownKey[DIK_J])
	{
		if (m_DownKey[DIK_I])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_TYPHOON;
		}
		else if (true == m_PressingKey[DIK_UP] || true == m_PressingKey[DIK_DOWN] || true == m_PressingKey[DIK_LEFT] || true == m_PressingKey[DIK_RIGHT])
		{
			m_iCurStateData = CSabito::STATE_ATTACK + CSabito::STATE_ATK_STRONG;
		}
	}

	m_vPlayerDir = vDir;
	m_vPlayerDir.y = 0.f;
}

void CSabito_KeyBoard_Controller::Judge_ChargeState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform)
{
	CAnimator* pAnimator = m_pSabito->Get_Animator();
	if (nullptr == pAnimator)
		return;

	_string strType = pAnimator->Get_AnimationType();

	if ("Idle" == strType)
	{
		if (true == m_tChargeStateTimer.Is_Over(_dTimeDelta))
		{

		}
	}
	else
	{
		m_tChargeStateTimer.Reset_Timer();
	}
}

void CSabito_KeyBoard_Controller::Find_TargetPlayer(CGameInstance* _pGameInstance)
{
	if (nullptr == m_pTargetPlayer)
	{
		list<CGameObject*> List = _pGameInstance->Find_GameObjectList(g_iCurrentLevel, LAYER_PLAYER);
		for (auto& iter : List)
		{
			if (iter != m_pOwnerObject)
			{
				m_pTargetPlayer = dynamic_cast<CPlayer*>(iter);
				break;
			}
		}
	}
}

void CSabito_KeyBoard_Controller::Find_CameraFree(CGameInstance* _pGameInstance)
{
	if (nullptr == m_pCamera)
	{
		CCamera_Free* pCamera = (CCamera_Free*)_pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_CAM, 0);
		m_pCamera = pCamera;
	}
}

_bool CSabito_KeyBoard_Controller::Judge_Return_IdleState(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform)
{
	if (CMethod::Is_Vector_Zero(m_vPlayerDir.Get_Vector()))
	{
		m_iCurStateData = CSabito::STATE_IDLE + CSabito::STATE_RETURN_IDLE;
		return true;
	}
	return false;
}

_bool CSabito_KeyBoard_Controller::Judge_Attack(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform)
{
	if ((m_iCurStateData % CSabito::STATE_JUDGE) == CSabito::STATE_ATTACK)
	{
		m_vPlayerDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
		m_vPlayerDir.y = 0.f;
		return true;
	}

	return false;
}

_bool CSabito_KeyBoard_Controller::Judge_Step(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform)
{
	if ("Step" != m_pSabito->Get_Animator()->Get_AnimationType())
		m_bChangeDirection_StepLR = false;


	if ((m_iCurStateData % CSabito::STATE_JUDGE) == CSabito::STATE_STEP)
	{
		if (m_iCurStateData == CSabito::STATE_STEP + CSabito::STATE_FORWARD || m_iCurStateData == CSabito::STATE_STEP + CSabito::STATE_BACK)
		{
			Judge_StepForAndBack_Situation(_dTimeDelta, _pGameInstance, _pTargetTransform, _pCameraTransform);
		}
	}

	if ("STEP_RIGHT" == m_pSabito->Get_Animator()->Get_CurrentTrigger())
	{
		if (true == m_bChangeDirection_StepLR)
		{
			m_pSabito->Get_Transform()->LookAt(m_pSabito->Get_Transform()->Get_Pos() + XMVector3Normalize(m_vPlayerDir.Get_Vector()));
			m_pSabito->Get_Transform()->Rotate_TurnToNotFix(_float3(0.f, -90.f, 0.f));
		}
		else
		{
			m_vPlayerDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
			m_vPlayerDir.y = 0.f;
			m_pSabito->Get_Transform()->LookAt(m_vPlayerDir.Get_Vector() + _pTargetTransform->Get_Pos());
		}
	}
	else if ("STEP_LEFT" == m_pSabito->Get_Animator()->Get_CurrentTrigger())
	{
		if (true == m_bChangeDirection_StepLR)
		{
			m_pSabito->Get_Transform()->LookAt(m_pSabito->Get_Transform()->Get_Pos() + XMVector3Normalize(m_vPlayerDir.Get_Vector()));
			m_pSabito->Get_Transform()->Rotate_TurnToNotFix(_float3(0.f, 90.f, 0.f));
		}
		else
		{
			m_vPlayerDir = _pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos();
			m_vPlayerDir.y = 0.f;
			m_pSabito->Get_Transform()->LookAt(m_vPlayerDir.Get_Vector() + _pTargetTransform->Get_Pos());
		}
	}
	else if ("STEP_FRONT" == m_pSabito->Get_Animator()->Get_CurrentTrigger())
	{
		m_pSabito->Get_Transform()->LookAt(m_pSabito->Get_Transform()->Get_Pos() + XMVector3Normalize(m_vPlayerDir.Get_Vector()));
	}
	else if ("STEP_BACK" == m_pSabito->Get_Animator()->Get_CurrentTrigger())
	{
		m_pSabito->Get_Transform()->LookAt(m_pSabito->Get_Transform()->Get_Pos() + XMVector3Normalize(m_vPlayerDir.Get_Vector() * -1.f));
	}
	return false;
}

_bool CSabito_KeyBoard_Controller::Judge_Gaurd(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform)
{
	return false;
}

void CSabito_KeyBoard_Controller::Judge_StepForAndBack_Situation(const _double& _dTimeDelta, CGameInstance* _pGameInstance, CTransform* _pTargetTransform, CTransform* _pCameraTransform)
{
	// m_vDirection 과 Target과의 Look 을 비교하여 트리거를 전달한다.
	_vector vLookToTarget = XMVector3Normalize(_pTargetTransform->Get_Pos() - m_pSabito->Get_Transform()->Get_Pos());

	// Target까지의 Look과 진행방향 Direct의 각도가 몇도인지를 판단하기 위해 내적한다.
	_vector vDot = XMVector3Dot(vLookToTarget, XMVector3Normalize(m_vPlayerDir.Get_Vector()));

	// Degree값으로 구한다.
	_float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(vDot)));

	// 타겟을 바라보는 기준으로 진행방향이 오른쪽인지 왼쪽인지를 판단하기위해 외적한다.
	_vector vCross = XMVector3Cross(vLookToTarget, m_vPlayerDir.Get_Vector());

	// fDegree가 50도 보다 작거나 같다면 STEP_FRONT를 시전한다.
	if (50 >= fDegree)
	{
		m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_FORWARD;
	}
	// 50도 보다크고 140보다 작거나 같다면 좌우 판별을 통해 STEP_LEFT 혹은 STEP_RIGHT를 시전한다.
	else if (50 < fDegree && 140 >= fDegree)
	{
		// 타겟을 향한 Look 기준으로 진행방향이 우측일때
		if (0 < XMVectorGetY(vCross))
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_RIGHT;
			m_bChangeDirection_StepLR = true;
		}
		// 타겟을 향한 Look 기준으로 진행방향이 좌측일때
		else
		{
			m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_LEFT;
			m_bChangeDirection_StepLR = true;
		}
	}
	// 140보다 fDegree가 크다면 STEP_BACK을 사용한다.
	else if (140 < fDegree)
	{
		m_iCurStateData = CSabito::STATE_STEP + CSabito::STATE_BACK;
	}
}


CSabito_KeyBoard_Controller* CSabito_KeyBoard_Controller::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CGameObject* _pGameObject)
{
	CSabito_KeyBoard_Controller* pInstance = new CSabito_KeyBoard_Controller(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pGameObject)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CPLContoller_Sabito::Create  To Failed");
#endif
	}
	return pInstance;
}

void CSabito_KeyBoard_Controller::Free()
{
	__super::Free();
}