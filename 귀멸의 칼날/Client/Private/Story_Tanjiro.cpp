#include "Story_Tanjiro.h"
#include "GameInstance.h"

#include "Animation.h"
#include "Story_Tanjiro_KeyBoard_Controller.h"
#include "Story_Tanjiro_GamePad_Controller.h"
#include "Camera_Free.h"
#include "Level_Story.h"
#include "Battle_Manager.h"
#include "AnimationClip.h"

#include "Sheath.h"
#include "Sword.h"

CStory_Tanjiro::CStory_Tanjiro(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPlayer(_pDevice, _pContext)
{
}

CStory_Tanjiro::CStory_Tanjiro(const CStory_Tanjiro& _rhs)
	: CPlayer(_rhs)
{
}

HRESULT CStory_Tanjiro::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CStory_Tanjiro::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Animator(L"Story_Tanjiro", m_pModelCom)))
		return E_FAIL;


	// 컨트롤러를 생성한다.
	m_pController = CStory_Tanjiro_KeyBoard_Controller::Create(m_pDevice, m_pContext, this);
	if (nullptr == m_pController)
		return E_FAIL;

	// 컨트롤러를 세팅한다.
	if (FAILED(SetUp_Controller(CONTROLLER_TYPE::CONTROLLER_AI)))
		return E_FAIL;

	// 무기 세팅
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		PATHS Path = { L"Model", L"Item", L"Story_Tanjiro", L"FBX_Converter", L"Data" };
		FILEGROUP* pFolder = pGameInstance->Find_Folder(&Path);
		{
			RETURN_IFNOT(nullptr == pFolder, E_FAIL);
		}
		{
			FILEDATA* pData = pFolder->FindData(L"Tanjiro_Sword");
			if (FAILED(SetUp_Weapon(pData, PROTO_COMP_MODEL_TANJIRO_SWORD, "RootNode", "RootNode")))
				return E_FAIL;
		}
		{
			FILEDATA* pData = pFolder->FindData(L"Tanjiro_Sheath");
			if (FAILED(SetUp_Sheath(pData, PROTO_COMP_MODEL_TANJIRO_SHEATH)))
				return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);


	// 이동속도를 넣어준다.
	m_fSpeed = 60.f;

	
	// 초기 위치 세팅
	if (nullptr != m_pTransformCom)
	{
		if (LEVEL_RATTAN == g_iCurrentLevel)
			m_pTransformCom->Update_Navigation_Cell(XMVectorSet(-60.f, 8.6f, -1.5f, 0.f));
		if (LEVEL_STORY == g_iCurrentLevel)
		{
			m_pTransformCom->Update_Navigation_Cell(XMVectorSet(-489.8f, 162.3f, 322.3f, 0.f));
			m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVectorSet(-0.9734f, 0.f, 0.22898f, 0.f));
		}
	}

	return S_OK;
}

void CStory_Tanjiro::Tick(const _double& _dTimeDelta)
{
	if (LEVEL_STORY == g_iCurrentLevel && CBattle_Manager::Get_Instance()->IsStoryBattle())
	{
		m_pSword->Set_Active(false);
		m_pSheath->Set_Active(false);
		return;
	}
	m_bMotionBlur = false;
	m_pSword->Set_Active(true);
	m_pSheath->Set_Active(true);

	CGameInstance::Get_Instance()->BGM_Volume_Down(0.05f);

	// 받아온 인풋으로 애니메이션 트리거를 넣는다.
	__super::Tick(_dTimeDelta);
}

void CStory_Tanjiro::Late_Tick(const _double& _dTimeDelta)
{
	if (LEVEL_STORY == g_iCurrentLevel && CBattle_Manager::Get_Instance()->IsStoryBattle())
		return;

	__super::Late_Tick(_dTimeDelta);
}

HRESULT CStory_Tanjiro::Render()
{

 	if (FAILED(__super::Render()))
		return E_FAIL;

#ifdef _DEBUG
	Coll_Render();
#endif // _DEBUG

	return S_OK;
}


void CStory_Tanjiro::Collision_Jump(_vector vDir)
{
	if ("IDLE" == m_strAnimType || "MOVE" == m_strAnimType)
	{
		if (true == m_pAnimator->Input_Trigger("JUMP"))
		{
			m_vBehaviorDir = XMVector3Normalize(vDir);
		}
	}
}

void CStory_Tanjiro::Collision_Slide(_vector vDir)
{
	if ("IDLE" == m_strAnimType || "MOVE" == m_strAnimType)
	{
		if (true == m_pAnimator->Input_Trigger("SLIDE"))
		{
			m_vSlideDir = XMVector3Normalize(vDir);
			m_fSpeed = 100.f;
		}
	}
}

HRESULT CStory_Tanjiro::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStory_Tanjiro::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	if (FAILED(Add_Component(COM_MODEL, g_iCurrentLevel,
		PROTO_COMP_MODEL_STORY_TANJIRO, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStory_Tanjiro::SetUp_Controller(const CONTROLLER_TYPE& _eType)
{
	__super::SetUp_Controller(_eType);
	if (CONTROLLER_GAMEPAD == _eType)
	{
		// 컨트롤러를 생성한다.
		m_pController = CStory_Tanjiro_GamePad_Controller::Create(m_pDevice, m_pContext, this);
		if (nullptr == m_pController)
			return E_FAIL;
	}
	else if (CONTROLLER_KEYBOARD == _eType)
	{	// 컨트롤러를 생성한다.
		m_pController = CStory_Tanjiro_KeyBoard_Controller::Create(m_pDevice, m_pContext, this);
		if (nullptr == m_pController)
			return E_FAIL;
	}
	m_eControllerType = _eType;
	return S_OK;
}

void CStory_Tanjiro::Fill_Datas()
{
	m_InsertVoidDatas.clear();
	m_InsertVoidDatas.reserve(1000);
	// Current State
	m_InsertVoidDatas.push_back((void*)&m_pAnimator->Get_AnimationType());
	// TransitionList
	m_InsertVoidDatas.push_back((void*)&m_pAnimator->Get_ActiveTransitionList());
	// Current AnimCLip
	m_InsertVoidDatas.push_back((void*)m_pAnimator->Get_CurrentAnimationClip());
	// Prev Animation Clip
	m_InsertVoidDatas.push_back((void*)m_pAnimator->Get_PrevAnimationClip());
}

void CStory_Tanjiro::Behavior_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{

	// 움직이기
	if ("MOVE" == m_strAnimType)
	{
		_float3* vDirection = (_float3*)_Datas[OUTDATA::OUT_DIRECTION];
		if (!CMethod::Is_Vector_Zero(vDirection->Get_Vector()))
		{
			// 이동한다.
			m_pTransformCom->Translate_Dir(vDirection->Get_Vector(), _dTimeDelta, m_fSpeed);
		}
	}
	// 점프
	else if ("JUMP" == m_strAnimType)
	{
		Jump_State(_dTimeDelta, _Datas);
	}
	// 슬라이드
	else if ("STEP" == m_strAnimType)
	{
		if (m_pAnimator->Get_CurrentAnimationClip()->Get_EventCheckDesc().bIsActive)
		{
			m_pTransformCom->Translate_DirSliding(m_vSlideDir.Get_Vector(), _dTimeDelta, m_fSpeed);
			m_fSpeed -= (_float)_dTimeDelta * 2.f;
		}
	}
}

void CStory_Tanjiro::Animation_Control_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	RETURN_IFNOT(_Datas.size() <= 0, ;);

	m_TriggerList = *static_cast<vector<_string>*>(_Datas[OUTDATA::OUT_STATE]);
	m_vDirection = *static_cast<_float3*>(_Datas[OUTDATA::OUT_DIRECTION]);

	if (nullptr != m_pAnimator)
	{
		m_strAnimType = m_pAnimator->Get_AnimationType();

		for (auto& trigger : m_TriggerList)
		{
			// 기본 상태일 때
			if ("IDLE" == m_strAnimType)
			{
				// 방향값이 0이 아니라면 MOVE 상태로 진행한다.
				if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
				{
					if ("MOVE" == trigger || "WALK" == trigger)
					{
						if (true == m_pAnimator->Input_Trigger(trigger))
						{
							// 이동방향쪽을 바라본다.
							if ("MOVE" == trigger)
								m_fSpeed = 60.f;
							else
								m_fSpeed = 15.f;
							m_pTransformCom->LookAt(Get_MoveDirection());
							break;
						}
					}
				}
			}
			else if ("MOVE" == m_strAnimType)
			{
				// 움직이고있다면 움직여야하는 방향을 쳐다본다.
				if (!CMethod::Is_Vector_Zero(m_vDirection.Get_Vector()))
				{
					m_pTransformCom->LookAt(Get_MoveDirection());
					if (15.f == m_fSpeed)
					{
						if ("MOVE" == trigger)
						{
							if (true == m_pAnimator->Input_Trigger(trigger))
								m_fSpeed = 60.f;
						}
					}
					else if (60.f == m_fSpeed)
					{
						if ("WALK" == trigger)
						{
							if (true == m_pAnimator->Input_Trigger(trigger))
								m_fSpeed = 15.f;
						}
					}
				}

				// 달리기를 멈춘다.
				if ("MOVE_END" == trigger)
				{
					if (true == m_pAnimator->Input_Trigger(trigger))
						break;
				}
			}
		}
	}

}

void CStory_Tanjiro::Jump_State(const _double& _dTimeDelta, VOIDDATAS _Datas)
{
	_vector vMovePos = Compute_MovePos(_dTimeDelta, m_vBehaviorDir.Get_Vector(), 80.f, 25.f, 0.3f, 1.f);

	// 점프 위치가 네비 Y값보다 작으면 네비 Y로 고정
	_vector vPlayerPos = m_pTransformCom->Get_Pos() + vMovePos;
	_float fCurCellYPos = m_pNavigationCom->Compute_Height(vPlayerPos);
	if (fCurCellYPos >= XMVectorGetY(vPlayerPos))
	{
		m_dFallTimeAcc = 0.0;
		vPlayerPos = XMVectorSetY(vPlayerPos, fCurCellYPos);
		m_pAnimator->Input_Trigger("JUMP_END");
		m_bIsAir = false;
	}
	else
		m_bIsAir = true;

	// Y는 내가 설정한 Y값, XZ는 네비에 따라 제한
	m_pTransformCom->Move_Pos_NotNavigateY(vPlayerPos);
	m_pTransformCom->LookAt(m_pTransformCom->Get_Pos() + XMVectorSet(m_vBehaviorDir.x, 0.f, m_vBehaviorDir.z, 0.f));

}


CStory_Tanjiro* CStory_Tanjiro::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CStory_Tanjiro* pInstance = new CStory_Tanjiro(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CStory_Tanjiro::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CStory_Tanjiro::Clone(void* pArg)
{
	CStory_Tanjiro* pInstance = new CStory_Tanjiro(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CStory_Tanjiro::Clone To Failed");
#endif
	}
	return pInstance;
}

void CStory_Tanjiro::Free()
{
	__super::Free();
}
