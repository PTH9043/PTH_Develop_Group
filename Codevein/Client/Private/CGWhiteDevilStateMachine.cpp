#include "CGWhiteDevilStateMachine.h"
#include "CGiantWhiteDevil.h"
#include "CGameInstance.h"
#include "CWeapon.h"

CGWhiteDevilStateMachine::CGWhiteDevilStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CMonsterState(_pDevice, _pContext)
{
}

HRESULT CGWhiteDevilStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
    if (FAILED(__super::NativeConstruct(_pOwnerObject)))
        return E_FAIL;

    m_iState = STATE_IDLE;
    CGiantWhiteDevil* pWhiteDevil = static_cast<CGiantWhiteDevil*>(_pOwnerObject);
    if (nullptr != pWhiteDevil)
        m_pWeapon = static_cast<CWeapon*>(pWhiteDevil->Get_Equip(CEquipment::EQUIP_WEAPON));

    return S_OK;
}

void CGWhiteDevilStateMachine::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{

}

void CGWhiteDevilStateMachine::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CGWhiteDevilStateMachine::Change_State(const _uint& _iState, CGiantWhiteDevil* _pOwnerObject)
{
	m_iPrevState = m_iState;
	m_iState = _iState;

	if (STATE_IDLE == _iState)
		Idle_State(_pOwnerObject);
	else if (STATE_FIND == _iState)
		Find_State(_pOwnerObject);
	else if (STATE_RETARGET == _iState)
		Retarget_State(_pOwnerObject);
	else if (STATE_DODGE == _iState)
		Dodge_State(_pOwnerObject);
	else if (STATE_MOVE == _iState)
		Move_State(_pOwnerObject);
	else if (STATE_RUN == _iState)
		Run_State(_pOwnerObject);
	else if (STATE_ATTACK == _iState)
		Attack_State(_pOwnerObject);
	else if (STATE_HIT == _iState)
		Hit_State(_pOwnerObject);
	else if (STATE_DIE == _iState)
		Die_State(_pOwnerObject);

	m_pModel->Get_NextAnimation()->Set_FastValue(2.0f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.0f);
}

void CGWhiteDevilStateMachine::Idle_State(CGiantWhiteDevil* _pOwnerObject)
{
	switch (_pOwnerObject->Get_InitAnimType())
	{
	case CGiantWhiteDevil::INIT_IDLE:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Idle_N_Loop");
		break;
	case CGiantWhiteDevil::INIT_WALK:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_F_Loop");
		break;
	}
}

void CGWhiteDevilStateMachine::Find_State(CGiantWhiteDevil* _pOwnerObject)
{
	_float fAngle = _pOwnerObject->PlayerToDot_DirToSelfLookConvert();
	_uint iTurn = 0;
	if (fAngle <= 90.f && fAngle >= 10.f)
		iTurn = 1;
	else if (fAngle > 90.f && fAngle <= 180.f)
		iTurn = 2;
	else if (fAngle >= -180.f && fAngle <= -90.f)
		iTurn = 3;
	else if (fAngle > -90.f && fAngle <= -10.f)
		iTurn = 4;

	switch (iTurn)
	{
	case 0:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Idle_N_Loop");
		break;
	case 1:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_IdleTurn_R90");
		break;
	case 2:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_IdleTurn_R180");
		break;
	case 3:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_IdleTurn_L180");
		break;
	case 4:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_IdleTurn_L90");
		break;
	}
}

void CGWhiteDevilStateMachine::Retarget_State(CGiantWhiteDevil* _pOwnerObject)
{
	if (STATE_DODGE == m_iPrevState)
	{
		m_pModel->Get_CurAnimation()->Set_AnimSupIndex(CAnimation::SUPINDEX_LAST);
		m_pModel->Get_CurAnimation()->Set_SuppleFastValue(0.5f);
		m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Idle_N_Loop");
	}
	else
	{
		if (CGiantWhiteDevil::ATK_6 == _pOwnerObject->Get_AttackIndex())
		{
			m_pModel->Get_CurAnimation()->Set_AnimSupIndex(CAnimation::SUPINDEX_INIT);
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Idle_N_Loop");
		}
		else
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Idle_N_Loop");
		}
	}
}

void CGWhiteDevilStateMachine::Dodge_State(CGiantWhiteDevil* _pOwnerObject)
{
	_float fValue = _pOwnerObject->PlayerToDot_DirToSelfLookConvert();
	if (fValue >= 0)
	{
		m_pModel->Change_Animation("AS_TGiantWhite_TCmn_Dodge_L");
	}
	else
	{
		m_pModel->Change_Animation("AS_TGiantWhite_TCmn_Dodge_R");
	}
}

void CGWhiteDevilStateMachine::Move_State(CGiantWhiteDevil* _pOwnerObject)
{

	if (true == m_bPlayerFindIdleState)
	{
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Idle_N_Loop");
	}
	else
	{
		if (true == m_bPlayerNotForward_MoveStation)
		{
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_B_Loop");
		}
		else
		{

			switch (_pOwnerObject->Get_AttackPattern())
			{
			case CGiantWhiteDevil::PAT_1:
				m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Idle_N_Loop");
				break;
			case CGiantWhiteDevil::PAT_2:
			case CGiantWhiteDevil::PAT_3:
				if (CCharacter::DIR_FORLEFT == _pOwnerObject->Get_Direction())
					m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_FL_LeftFoot_Loop");
				else if (CCharacter::DIR_BACKLEFT == _pOwnerObject->Get_Direction())
					m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_BL_Loop");
				else if (CCharacter::DIR_FOR == _pOwnerObject->Get_Direction())
					m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_F_Loop");
				else if (CCharacter::DIR_BACK == _pOwnerObject->Get_Direction())
					m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_B_Loop");
				else if (CCharacter::DIR_FORRIGHT == _pOwnerObject->Get_Direction())
					m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_FRLoop");
				else if (CCharacter::DIR_BACKRIGHT == _pOwnerObject->Get_Direction())
					m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_BR_LeftFoot_Loop");
				else if (CCharacter::DIR_FOR == _pOwnerObject->Get_Direction())
					m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_F_Loop");
				else if (CCharacter::DIR_BACK == _pOwnerObject->Get_Direction())
					m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_B_Loop");
				break;
			}
		}
	}
}

void CGWhiteDevilStateMachine::Run_State(CGiantWhiteDevil* _pOwnerObject)
{
	if (true == m_bPlayerNotForward_MoveStation)
	{
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Walk_B_Loop");
	}
	else
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Run_N_Loop");
}

void CGWhiteDevilStateMachine::Attack_State(CGiantWhiteDevil* _pOwnerObject)
{
	_float fDot = 0.f;
	switch (_pOwnerObject->Get_AttackIndex())
	{
	case CGiantWhiteDevil::ATK_1 :
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_AttackNormal01_N");
		break;
	case CGiantWhiteDevil::ATK_2:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_AttackSmash01_N");
		break;
	case CGiantWhiteDevil::ATK_3:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_AttackSpin01_N");
		break;
	case CGiantWhiteDevil::ATK_4:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_AttackStrong01_N");
		break;
	case CGiantWhiteDevil::ATK_5:
		m_pModel->Change_Animation("AS_TGiantWhite_TCmn_AKetsugiPointShoot01_N");
		break;
	case CGiantWhiteDevil::ATK_6:
		 fDot = _pOwnerObject->PlayerToDot_DirToSelfRight();
		if (fDot > 0.f)
		{
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_AttackLeftRotation01_N");
		}
		else
		{
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_AttackRightRotation01_N");
		}
		break;
	}
}

void CGWhiteDevilStateMachine::Hit_State(CGiantWhiteDevil* _pOwnerObject)
{
	_float fDot = _pOwnerObject->PlayerToDot_DirToSelfLookConvert();
	_uint iJudge = 0;
	if (90.f >= fDot && fDot >= 0)
		iJudge = 1;
	else if (180.f >= fDot && fDot >= 90.f)
		iJudge = 2;
	else if (-90 >= fDot && fDot >= -180.f)
		iJudge = 3;
	else if (-0 >= fDot && fDot >= -90)
		iJudge = 4;


	if (CCharacter::AS_STRONG == _pOwnerObject->Get_AtkStrongEncounter()
		|| CCharacter::AS_NORMAL == _pOwnerObject->Get_AtkStrongEncounter())
	{
		switch (iJudge)
		{
		case 1:
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage02_FL");
			break;
		case 2:
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage02_BL");
			break;
		case 3:
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage02_BR");
			break;
		case 4:
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage02_FR");
			break;
		}
	}
	else
	{
		switch (iJudge)
		{
		case 1:
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage01_FL");
			break;
		case 2:
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage01_BL");
			break;
		case 3:
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage01_BR");
			break;
		case 4:
			m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage01_FR");
			break;
		}
	}
}

void CGWhiteDevilStateMachine::Die_State(CGiantWhiteDevil* _pOwnerObject)
{
	_float fDot = _pOwnerObject->PlayerToDot_DirToSelfLookConvert();
	_uint iJudge = 0;
	if (90.f >= fDot && fDot >= 0)
		iJudge = 1;
	else if (180.f >= fDot && fDot >= 90.f)
		iJudge = 2;
	else if (-90 >= fDot || fDot >= -180.f)
		iJudge = 3;
	else if (-0 >= fDot || fDot >= -90)
		iJudge = 4;

	switch (iJudge)
	{
	case 1:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage02_FL");
		break;
	case 2:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage02_BL");
		break;
	case 3:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage02_BR");
		break;
	case 4:
		m_pModel->Change_Animation("AS_TGiantWhite_THalberd_Damage02_FR");
		break;
	}
}

CGWhiteDevilStateMachine* CGWhiteDevilStateMachine::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject)
{
	CGWhiteDevilStateMachine* pInstance = new CGWhiteDevilStateMachine(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pGameObject)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CGWhiteDevilStateMachine::Create to Failed");
	}
    return pInstance;
}

void CGWhiteDevilStateMachine::Free()
{
	__super::Free();
}
