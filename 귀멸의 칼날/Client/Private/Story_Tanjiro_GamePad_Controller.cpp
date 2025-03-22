#include "Story_Tanjiro_GamePad_Controller.h"
#include "Player.h"
#include "GameInstance.h"
#include "Camera_Free.h"

CStory_Tanjiro_GamePad_Controller::CStory_Tanjiro_GamePad_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CPlayer_GamePad_Controller(_pDevice, _pContext)
{
}

HRESULT CStory_Tanjiro_GamePad_Controller::NativeConstruct_Prototype(CGameObject* _pGameObject)
{
	if (FAILED(__super::NativeConstruct_Prototype(_pGameObject)))
		return E_FAIL;

	m_pPlayer = static_cast<CPlayer*>(_pGameObject);

	return S_OK;
}

void CStory_Tanjiro_GamePad_Controller::Tick(const _double& _dTimeDelta)
{
	Input_Defines();

	m_TriggerList.clear();

	// 카메라의 Right, Up, Look을 받아온다
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
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

		// 현재 사용중인 카메라 Transform를 받아온다.
		CTransform* pCamTransform = (CTransform*)pGameInstance->Get_Component(g_iCurrentLevel, LAYER_CAM, COM_TRANSFORM, 0);
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

		if (true == m_GamePadPressing[GPK_LEFT_STICKUP])
		{
			// 카메라의 Look을 Dir에 더해준다
			vDir += vCamLook;

			// 달리고있을때 L눌리면 STEP
			if (true == m_GamePadPressing[GPK_LEFT_TRIGGER])
				Add_Trigger("WALK");
			else
				Add_Trigger("MOVE");
		}

		if (true == m_GamePadPressing[GPK_LEFT_STICKDOWN])
		{
			// 카메라의 Look을 Dir에 빼준다
			vDir -= vCamLook;

			// 달리고있을때 L눌리면 STEP
			if (true == m_GamePadPressing[GPK_LEFT_TRIGGER])
				Add_Trigger("WALK");
			else
				Add_Trigger("MOVE");
		}
		if (m_GamePadPressing[GPK_LEFT_STICKUP] && m_GamePadPressing[GPK_LEFT_STICKLEFT] && m_GamePadPressing[GPK_LEFT_STICKDOWN] 
			&& m_GamePadPressing[GPK_LEFT_STICKRIGHT])
			vDir = XMVectorZero();

		m_vPlayerDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

		if ("MOVE" == pPlayerAnimator->Get_AnimationType())
		{
			if (CMethod::Is_Vector_Zero(m_vPlayerDir.Get_Vector()))
				Add_Trigger("MOVE_END");
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CStory_Tanjiro_GamePad_Controller::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
}

void CStory_Tanjiro_GamePad_Controller::Insert_Datas(VOIDDATAS _pData)
{
	m_ReceiveVoidDatas = _pData;
}

VOIDDATAS CStory_Tanjiro_GamePad_Controller::Out_Datas()
{
	m_OutVoidDatas.clear();
	m_OutVoidDatas.reserve(1000);
	// Push State Data
	m_OutVoidDatas.push_back(&m_TriggerList);
	// Transform
	m_OutVoidDatas.push_back(&m_vPlayerDir);

	return m_OutVoidDatas;
}

CStory_Tanjiro_GamePad_Controller* CStory_Tanjiro_GamePad_Controller::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CGameObject* _pGameObject)
{
	CStory_Tanjiro_GamePad_Controller* pInstance = new CStory_Tanjiro_GamePad_Controller(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_pGameObject)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CStory_Tanjiro_Controller::Create To Failed");
#endif
	}
	return pInstance;
}

void CStory_Tanjiro_GamePad_Controller::Free()
{
	__super::Free();
}
