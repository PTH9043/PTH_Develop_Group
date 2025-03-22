#include "Player_GamePad_Controller.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Animator.h"
#include "Player.h"


CPlayer_GamePad_Controller::CPlayer_GamePad_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGamePad_InputController(_pDevice, _pContext)
{
}

HRESULT CPlayer_GamePad_Controller::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_GamePad_Controller::Tick(const _double& _dTimeDelta)
{	// InputController에서 이번 프레임의 키 입력값을 받아온다.
	__super::Tick(_dTimeDelta);

	m_TriggerList.clear();

	// 카메라의 Right, Up, Look을 받아온다

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CAnimator* pPlayerAnimator = nullptr;
	// 플레이어 객체를 받아왔다면 애니메이터를 받아온다.
	if (nullptr != m_pPlayer)
		pPlayerAnimator = m_pPlayer->Get_Animator();

	// 플레이어 애니메이터가 nullptr이라면 리턴한다.
	if (nullptr == pPlayerAnimator)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// 현재 사용중인 카메라를 받아온다.
	CCamera_Free* pCamera = (CCamera_Free*)pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_CAM, 0);
	if (Is_Nullptr(pCamera))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	CTransform* pCamTransform = pCamera->Get_Transform();
	if (Is_Nullptr(pCamTransform))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	// 카메라의 Right Up Look을 이용하여 Look을 구할 변수
	_vector vDir = XMVectorZero();

	_vector vCamRight = pCamTransform->Get_Right();
	_vector vCamUp = pCamTransform->Get_Up();
	_vector vCamLook = pCamTransform->Get_Look();

	// MOVE체크 -> MOVE의 경우에는 vDir에 카메라의 Right Up Look을 바꿔줘야한다.
	{
		if (true == m_GamePadPressing[GPK_LEFT_STICKUP])
		{
			// 카메라의 Look을 Dir에 더해준다
			vDir += vCamLook;

			// 달리고있을때 L눌리면 STEP
			if (true == m_GamePadDown[GPK_B])
				Add_Trigger("STEP");
			else if (true == m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK_UPPER");
				Add_Trigger("ATTACK");

				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			else if (true == m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
			else if (true == m_GamePadDown[GPK_Y])
				Add_Trigger("SKILL_2");
			else if (true == m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");
			else
				Add_Trigger("MOVE");
		}

		if (true == m_GamePadPressing[GPK_LEFT_STICKLEFT])
		{
			// 카메라의 Right을 Dir에 뻬준다
			vDir -= vCamRight;

			// 달리고있을때 L눌리면 STEP
			if (true == m_GamePadDown[GPK_B])
				Add_Trigger("STEP");
			else if (true == m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK");

				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			else if (true == m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
			else if (true == m_GamePadDown[GPK_Y])
				Add_Trigger("SKILL_2");
			else if (true == m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");
			else
				Add_Trigger("MOVE");
		}

		if (true == m_GamePadPressing[GPK_LEFT_STICKDOWN])
		{
			// 카메라의 Look을 Dir에 빼준다
			vDir -= vCamLook;

			// 달리고있을때 L눌리면 STEP
			if (true == m_GamePadDown[GPK_B])
				Add_Trigger("STEP");
			else if (true == m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK_DOWN");
				Add_Trigger("ATTACK");

				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			else if (true == m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
			else if (true == m_GamePadDown[GPK_Y])
				Add_Trigger("SKILL_2");
			else if (true == m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");
			else
				Add_Trigger("MOVE");
		}

		if (true == m_GamePadPressing[GPK_LEFT_STICKRIGHT])
		{
			// 카메라의 Right을 Dir에 더해준다
			vDir += vCamRight;

			// 달리고있을때 L눌리면 STEP
			if (true == m_GamePadDown[GPK_B])
				Add_Trigger("STEP");
			else if (true == m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK");

				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			else if (true == m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
			else if (true == m_GamePadDown[GPK_Y])
				Add_Trigger("SKILL_2");
			else if (true == m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");
			else
				Add_Trigger("MOVE");
		}

		// STEP 체크 -> L키가 이미 눌려져있는 상황에서 방향키의 Down
		if (m_GamePadPressing[GPK_B])
		{
			if (m_GamePadPressing[GPK_LEFT_STICKUP])
				Add_Trigger("STEP");
			else if (m_GamePadPressing[GPK_LEFT_STICKDOWN])
				Add_Trigger("STEP");
			else if (m_GamePadPressing[GPK_LEFT_STICKLEFT])
				Add_Trigger("STEP");
			else if (m_GamePadPressing[GPK_LEFT_STICKRIGHT])
				Add_Trigger("STEP");

			if (true == m_GamePadDown[GPK_LEFT_STICKUP])
				Add_Trigger("STEP");

			else if (true == m_GamePadDown[GPK_LEFT_STICKDOWN])
				Add_Trigger("STEP");

			else if (true == m_GamePadDown[GPK_LEFT_STICKLEFT])
				Add_Trigger("STEP");

			else if (true == m_GamePadDown[GPK_LEFT_STICKRIGHT])
				Add_Trigger("STEP");
		}

		if (true == m_GamePadDown[GPK_LEFT_STICKUP])
		{
			// 공격과 이동을 동시에 눌렀다면 강공격 실행
			if (true == m_GamePadDown[GPK_X])
				Add_Trigger("CHARGE_ATTACK");
			else
			{
				//L키가 눌려져있지 않은 경우에만 MOVE 트리거를 생성한다.
				if (false == m_GamePadPressing[GPK_B])
				{
					Add_Trigger("MOVE");
					vDir += vCamLook;
				}
			}
		}
		if (true == m_GamePadDown[GPK_LEFT_STICKLEFT])
		{
			// 공격과 이동을 동시에 눌렀다면 강공격 실행
			if (true == m_GamePadDown[GPK_X])
				Add_Trigger("CHARGE_ATTACK");
			else
			{
				//L키가 눌려져있지 않은 경우에만 MOVE 트리거를 생성한다.
				if (false == m_GamePadPressing[GPK_B])
				{
					Add_Trigger("MOVE");
					vDir -= vCamRight;
				}
			}
		}
		if (true == m_GamePadDown[GPK_LEFT_STICKDOWN])
		{
			// 공격과 이동을 동시에 눌렀다면 강공격 실행
			if (true == m_GamePadDown[GPK_X])
				Add_Trigger("CHARGE_ATTACK");
			else
			{
				//L키가 눌려져있지 않은 경우에만 MOVE 트리거를 생성한다.
				if (false == m_GamePadPressing[GPK_B])
				{
					Add_Trigger("MOVE");
					vDir -= vCamLook;
				}
			}
		}
		if (true == m_GamePadDown[GPK_LEFT_STICKRIGHT])
		{
			// 공격과 이동을 동시에 눌렀다면 강공격 실행
			if (true == m_GamePadDown[GPK_X])
				Add_Trigger("CHARGE_ATTACK");
			else
			{
				//L키가 눌려져있지 않은 경우에만 MOVE 트리거를 생성한다.
				if (false == m_GamePadPressing[GPK_B])
				{
					Add_Trigger("MOVE");
					vDir += vCamRight;
				}
			}
		}
		// Dash 체크
		{
			if (m_GamePadDown[GPK_B])
			{
				// 방향키를 누르지 않았을때 L버튼을 누르면 대쉬한다.
				if (0.f == XMVectorGetX(vDir) && 0.f == XMVectorGetY(vDir) && 0.f == XMVectorGetZ(vDir))
					Add_Trigger("DASH");
			}
		}
		// 스킬 체크
		{
			if (m_GamePadDown[GPK_Y])
			{
				// 방향키를 누르지 않았을때 L버튼을 누르면 대쉬한다.
				if (0.f == XMVectorGetX(vDir) && 0.f == XMVectorGetY(vDir) && 0.f == XMVectorGetZ(vDir))
					Add_Trigger("SKILL_1");
			}
		}
		// 오의 사용 체크
		{
			if (m_GamePadDown[GPK_LEFT_TRIGGER])
				Add_Trigger("ULTIMATE");
		}
		// 점프 체크
		{
			if (m_GamePadDown[GPK_A])
				Add_Trigger("JUMP");
		}

		// 낙법 체크
		{
			if (nullptr != m_pPlayer)
			{
				_string strAnimType = m_pPlayer->Get_Animator()->Get_AnimationType();
				if ("AIR_DOWN" == strAnimType)
				{
					if (m_GamePadPressing[GPK_A])
					{
						Add_Trigger("PASSIVE");
					}
				}
			}
		}

		// 강공격 시 공격 뗐는지 체크 / 공격 체크
		{
			if (m_GamePadDown[GPK_X])
			{
				Add_Trigger("ATTACK");
				if (true == m_pPlayer->Get_BattleData().bIsAwake)
					Add_Trigger("ATTACK_AWAKE");
			}
			if (m_GamePadPressing[GPK_RIGHT_TRIGGER])
				Add_Trigger("CHARGE_ATTACK");
			if (!m_GamePadPressing[GPK_RIGHT_TRIGGER])
				Add_Trigger("CHARGE_ATTACK_END");
		}

		// 개방 체크
		{
			if (m_GamePadDown[GPK_LEFT_SHOULDER])
				Add_Trigger("AWAKE");
		}

		// 방어 체크
		{
			if (m_GamePadDown[GPK_RIGHT_SHOULDER])
				Add_Trigger("GUARD");

			// O버튼이 Pressing되고 있을때
			if (m_GamePadPressing[GPK_RIGHT_SHOULDER])
			{
				// 대쉬중이었다면 GUARD 트리거를 생성한다.
				if ("DASH" == pPlayerAnimator->Get_AnimationType())
					Add_Trigger("GUARD");

				if (m_GamePadDown[GPK_LEFT_STICKUP])
					Add_Trigger("PUSH");
				else if (m_GamePadDown[GPK_LEFT_STICKLEFT])
					Add_Trigger("PUSH");
				else if (m_GamePadDown[GPK_LEFT_STICKDOWN])
					Add_Trigger("PUSH");
				else if (m_GamePadDown[GPK_LEFT_STICKRIGHT])
					Add_Trigger("PUSH");
				else if (m_GamePadDown[GPK_Y])
					Add_Trigger("SKILL_3");
				else if (m_GamePadDown[GPK_X])
					Add_Trigger("THROW");
			}
			else
				// Pressing되고있지 않다면 GUARD_END를 보낸다.
				Add_Trigger("GUARD_END");
		}

		if (m_GamePadPressing[GPK_LEFT_STICKUP] && m_GamePadPressing[GPK_LEFT_STICKRIGHT]
			&& m_GamePadPressing[GPK_LEFT_STICKDOWN] && m_GamePadPressing[GPK_LEFT_STICKRIGHT])
			vDir = XMVectorZero();

		m_vPlayerDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

		// 만약 플레이어가 움직이고있는데 아무키도 눌리지 않았다면 MOVE_END를 트리거에 넣는다.
		if ("MOVE" == pPlayerAnimator->Get_AnimationType())
		{
			if (CMethod::Is_Vector_Zero(m_vPlayerDir.Get_Vector()))
				Add_Trigger("MOVE_END");
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}
void CPlayer_GamePad_Controller::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CPlayer_GamePad_Controller::Insert_Datas(VOIDDATAS _pData)
{
	m_ReceiveVoidDatas = _pData;
}

VOIDDATAS CPlayer_GamePad_Controller::Out_Datas()
{
	m_OutVoidDatas.clear();
	m_OutVoidDatas.push_back(&m_TriggerList);
	m_OutVoidDatas.push_back(&m_vPlayerDir);
	return m_OutVoidDatas;
}

_bool CPlayer_GamePad_Controller::Make_Trigger(_string _strTrigger, _bool* pFirstInput, _bool* pSecondInput)
{
	if (false == *pFirstInput || false == *pSecondInput)
		return false;

	if (nullptr == pSecondInput)
	{
		if (true == *pFirstInput)
			Add_Trigger(_strTrigger);
	}
	else
	{
		if (true == *pFirstInput)
		{
			if (true == *pSecondInput)
				Add_Trigger(_strTrigger);
		}
	}

	return true;
}

void CPlayer_GamePad_Controller::Add_Trigger(_string strTrigger)
{
	_bool isSame = false;
	for (auto& trigger : m_TriggerList)
	{
		if (trigger == strTrigger)
		{
			isSame = true;
			return;
		}
	}

	// 동일한게 없다면 트리거 리스트에 트리거를 추가한다.
	if (false == isSame)
		m_TriggerList.push_back(strTrigger);
}

void CPlayer_GamePad_Controller::Free()
{
	__super::Free();
}
