#include "CPlayerStateMachine.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CCamera_Free.h"
#include "CWeapon.h"
#include "CHierarchyNode.h"
#include "COneHandSword.h"

void CPlayerStateMachine::Set_Weapon(CWeapon* _pWeapon)
{
	Safe_Release(m_pWeapon); 
	this->m_pWeapon = _pWeapon;
	Safe_AddRef(m_pWeapon);
}

CPlayerStateMachine::CPlayerStateMachine(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CStateMachine(_pDevice, _pContext)
{
}

HRESULT CPlayerStateMachine::NativeConstruct(CGameObject* _pOwnerObject)
{
	if (FAILED(__super::NativeConstruct(_pOwnerObject)))
		return E_FAIL;

	m_iState = STATE_IDLE;

	m_pLeftLeg = m_pModel->Get_HierarchyNodes("LeftUpLeg");
	m_pRightLeg = m_pModel->Get_HierarchyNodes("RightUpLeg");

	if (nullptr == m_pLeftLeg || nullptr == m_pRightLeg)
		return E_FAIL;

	Safe_AddRef(m_pLeftLeg);
	Safe_AddRef(m_pRightLeg);
	return S_OK;
}

void CPlayerStateMachine::Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{

}

void CPlayerStateMachine::Late_Tick(const _double& _dTimeDelta, CGameObject* _pOwner)
{
}

void CPlayerStateMachine::Change_State(const _uint& _iState, CPlayer* _pOwnerObject)
{
	if (nullptr != _pOwnerObject)
		m_pWeapon = static_cast<CWeapon*>(_pOwnerObject->Get_Equip(CEquipment::EQUIP_WEAPON));

	m_iPrevState = m_iState;
	m_iState = _iState;

	if (STATE_IDLE == _iState)
		Idle_State(_pOwnerObject);
	else if (STATE_WALK == _iState)
		Walk_State(_pOwnerObject);
	else if (STATE_RUN == _iState)
		Run_State(_pOwnerObject);
	else if (STATE_ROLL == _iState)
		Roll_State(_pOwnerObject);
	else if (STATE_ATTACK == _iState)
		Attack_State(_pOwnerObject);
	else if (STATE_SPATTACK == _iState)
		AttackSp_State(_pOwnerObject);
	else if (STATE_HEALING == _iState)
		Healing_State(_pOwnerObject);
	else if (STATE_HIT == _iState)
		Hit_State(_pOwnerObject);
	else if (STATE_SKILL == _iState)
		Skill_State(_pOwnerObject);
	else if (STATE_OPENSIGHT == _iState)
		OpenSight_State(_pOwnerObject);
	else if (STATE_ECT_BEHABIOR_FINDCHECKPOINTS == _iState)
		Find_CheckPoints_Ect_Behavior(_pOwnerObject);
	else if (STATE_DIE == _iState)
		Dead_State(_pOwnerObject);

	m_pModel->Get_NextAnimation()->Set_FastValue(2.5f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.5f);
}

HRESULT CPlayerStateMachine::Idle_State(CPlayer* _pOwnerObject)
{
	if (nullptr == m_pWeapon)
	{
		if (STATE_ROLL == m_iPrevState)
		{
			m_pModel->Change_Animation("TCmn_Idle_N_Loop");
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_SOUR);
		}
		else
			m_pModel->Change_Animation("TCmn_Idle_N_Loop");
	}
	else
	{
		if (STATE_HIT != m_iPrevState)
		{
			switch (m_pWeapon->Get_WeaponType())
			{
			case CWeapon::WEAPON_ONEHAND:
				m_pModel->Change_Animation("TCmn_Idle_N_Loop");
			case CWeapon::WEAPON_TWOHAND:
				m_pModel->Change_Animation("TCmn_Idle_N_Loop");
				break;
			}
		}
		else
		{
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
			m_pModel->Change_Animation("TCmn_Idle_N_Loop");
		}
	}

	return S_OK;
}

HRESULT CPlayerStateMachine::Walk_State(CPlayer* _pOwnerObject)
{
	if (false == m_bTarget)
	{
		if (nullptr == m_pWeapon)
		{
			m_pModel->Change_Animation("TCmn_Run_F_Loop");
		}
		else
		{
			switch (m_pWeapon->Get_WeaponType())
			{
			case CWeapon::WEAPON_ONEHAND:
				m_pModel->Change_Animation("TCmn_Run_F_Loop");
				break;
			}
		}
	}
	else
	{
		if (nullptr == m_pWeapon || CWeapon::WEAPON_ONEHAND == m_pWeapon->Get_WeaponType())
		{
			if (CCharacter::DIR_FOR == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Walk_F_Loop");
			}
			else  if (CCharacter::DIR_BACK == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Walk_B_Loop");
			}
			else  if (CCharacter::DIR_RIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Walk_R_Loop");
			}
			else  if (CCharacter::DIR_LEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Walk_L_Loop");
			}
			else  if (CCharacter::DIR_FORRIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Walk_FR_Loop");
			}
			else  if (CCharacter::DIR_FORLEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Walk_FL_LeftFoot_Loop");
			}
			else  if (CCharacter::DIR_BACKRIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Walk_BR_LeftFoot_Loop");
			}
			else  if (CCharacter::DIR_BACKLEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Walk_BL_Loop");
			}
		}
	}
	return S_OK;
}

HRESULT CPlayerStateMachine::Run_State(CPlayer* _pOwnerObject)
{
	if (false == m_bTarget)
	{
		if (nullptr == m_pWeapon)
		{
			m_pModel->Change_Animation("TCmn_Run_F_Loop");
		}
		else
		{
			switch (m_pWeapon->Get_WeaponType())
			{
			case CWeapon::WEAPON_ONEHAND:
				m_pModel->Change_Animation("TCmn_Run_F_Loop");
				break;
			}
		}
	}
	else
	{
		if (nullptr == m_pWeapon || CWeapon::WEAPON_ONEHAND == m_pWeapon->Get_WeaponType())
		{
			if (CCharacter::DIR_FOR == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Run_F_Loop");
			}
			else  if (CCharacter::DIR_BACK == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Run_B_Loop");
			}
			else  if (CCharacter::DIR_RIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Run_R_Loop");
			}
			else  if (CCharacter::DIR_LEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Run_L_Loop");
			}
			else  if (CCharacter::DIR_FORRIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Run_FR_Loop");
			}
			else  if (CCharacter::DIR_FORLEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Run_FL_RightFoot_Loop");
			}
			else  if (CCharacter::DIR_BACKRIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Run_BR_RightFoot_Loop");
			}
			else  if (CCharacter::DIR_BACKLEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("AS_TStdFemale_TCmn_Run_BL_Loop");
			}
		}
	}
	m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.f);
	return S_OK;
}

HRESULT CPlayerStateMachine::Roll_State(CPlayer* _pOwnerObject)
{
	if (false == m_bTarget)
	{
		if (nullptr == m_pWeapon)
		{
			m_pModel->Change_Animation("TCmn_Dodge300_F");
			m_strSaveRollAnim = "TCmn_Dodge300_F";
		}
		else
		{
			switch (m_pWeapon->Get_WeaponType())
			{
			case CWeapon::WEAPON_ONEHAND:
				m_pModel->Change_Animation("TCmn_Dodge300_F");
				m_strSaveRollAnim = "TCmn_Dodge300_F";
				break;
			}
		}
		m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
		m_strSaveRollAnim = "TCmn_Dodge300_F";
	}
	else
	{
		if (nullptr != m_pWeapon)
		{
			if (CWeapon::WEAPON_ONEHAND == m_pWeapon->Get_WeaponType()
				|| CWeapon::WEAPON_TWOHAND == m_pWeapon->Get_WeaponType())
			{
				if (300 <= _pOwnerObject->Get_PawnStatus().fWeight)
				{
					if (CCharacter::DIR_FOR == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("TCmn_Dodge300_F");
						m_strSaveRollAnim = "TCmn_Dodge300_F";
					}
					else  if (CCharacter::DIR_BACK == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_DodgeRoll_B");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_DodgeRoll_B";
					}
					else  if (CCharacter::DIR_RIGHT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_DodgeRoll_R");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_DodgeRoll_R";
					}
					else  if (CCharacter::DIR_LEFT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_DodgeRoll_L");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_DodgeRoll_L";
					}
					else  if (CCharacter::DIR_FORRIGHT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_DodgeRoll_FR");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_DodgeRoll_FR";
					}
					else  if (CCharacter::DIR_FORLEFT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_DodgeRoll_FL");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_DodgeRoll_FL";
					}
					else  if (CCharacter::DIR_BACKRIGHT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_DodgeRoll_BR");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_DodgeRoll_BR";
					}
					else  if (CCharacter::DIR_BACKLEFT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_DodgeRoll_BL");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_DodgeRoll_BL";
					}
				}
				else 
				{
					if (CCharacter::DIR_FOR == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_Dodge300_F");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_Dodge300_F";
					}
					else  if (CCharacter::DIR_BACK == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_Dodge300_B");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_Dodge300_B";
					}
					else  if (CCharacter::DIR_RIGHT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_Dodge300_R");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_Dodge300_R";
					}
					else  if (CCharacter::DIR_LEFT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_Dodge300_L");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_Dodge300_L";
					}
					else  if (CCharacter::DIR_FORRIGHT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_Dodge300_FR");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_Dodge300_FR";
					}
					else  if (CCharacter::DIR_FORLEFT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_Dodge300_FL");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_Dodge300_FL";
					}
					else  if (CCharacter::DIR_BACKRIGHT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_Dodge300_BR");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_Dodge300_BR";
					}
					else  if (CCharacter::DIR_BACKLEFT == _pOwnerObject->Get_Direction())
					{
						m_pModel->Change_Animation("AS_TStdUnisex_TLSword_Dodge300_BL");
						m_strSaveRollAnim = "AS_TStdUnisex_TLSword_Dodge300_BL";
					}
				}
			}
		}
		else
		{
			if (CCharacter::DIR_FOR == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("TCmn_Dodge300_F");
				m_strSaveRollAnim = "TCmn_Dodge300_F";
			}
			else  if (CCharacter::DIR_BACK == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("TCmn_Dodge300_B");
				m_strSaveRollAnim = "TCmn_Dodge300_B";
			}
			else  if (CCharacter::DIR_RIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("TCmn_Dodge300_R");
				m_strSaveRollAnim = "TCmn_Dodge300_R";
			}
			else  if (CCharacter::DIR_LEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("TCmn_Dodge300_L");
				m_strSaveRollAnim = "TCmn_Dodge300_L";
			}
			else  if (CCharacter::DIR_FORRIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("TCmn_Dodge300_FR");
				m_strSaveRollAnim = "TCmn_Dodge300_FR";
			}
			else  if (CCharacter::DIR_FORLEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("TCmn_Dodge300_FL");
				m_strSaveRollAnim = "TCmn_Dodge300_FL";
			}
			else  if (CCharacter::DIR_BACKRIGHT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("TCmn_Dodge300_BR");
				m_strSaveRollAnim = "TCmn_Dodge300_BR";
			}
			else  if (CCharacter::DIR_BACKLEFT == _pOwnerObject->Get_Direction())
			{
				m_pModel->Change_Animation("TCmn_Dodge300_BL");
				m_strSaveRollAnim = "TCmn_Dodge300_BL";
			}
		}
	}

 	if (true == m_bDoubleRoll)
	{
		m_pModel->Change_SameAnimation(m_strSaveRollAnim.c_str());
	}

	m_pModel->Get_NextAnimation()->Set_FastValue(1.5f);
	m_pModel->Get_CurAnimation()->Set_FastValue(1.5f);
	return S_OK;
}

HRESULT CPlayerStateMachine::Attack_State(CPlayer* _pOwnerObject)
{
	m_fAttackAnimationControlValue = 2.5f;
	if (nullptr == m_pWeapon)
	{
		return E_FAIL;
	}
	else
	{
		switch (m_pWeapon->Get_WeaponType())
		{
		case CWeapon::WEAPON_ONEHAND:
			if (!lstrcmp(L"AlisaSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				if (0 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal01_N");
					++m_iAttackCount;
				}
				else if (1 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal02_N");
					++m_iAttackCount;
				}
				else if (2 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal03_N");
					m_iAttackCount = 0;
				}
			}
			else if (!lstrcmp(L"BlackSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"BurningSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalSwordA", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalSwordB", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalSwordC", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalSwordD", m_pWeapon->Get_WeaponName().c_str()))
			{
				if (0 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal05_N");
						++m_iAttackCount;
				}
				else if (1 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal06_N");
					m_iAttackCount = 0;
				}
			}
			else if (!lstrcmp(L"GhostSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"HannibalSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"MilitarySword", m_pWeapon->Get_WeaponName().c_str()))
			{
				if (0 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal04_N");
					++m_iAttackCount;
				}
				else if (1 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal07_N");
					++m_iAttackCount;
				}
				else if (2 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal08_N");
					m_iAttackCount = 0;
				}
			}
			else if (!lstrcmp(L"GreatSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"HannibalGreatSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"MilitaryGreatSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalGreatSwordA", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalGreatSwordB", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalGreatSwordC", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalGreatSwordD", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"SomaGreatSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				if (0 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal10_N");
					++m_iAttackCount;
				}
				else if (1 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal11_N");
					++m_iAttackCount;
				}
				else if (2 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal12_N");
					m_iAttackCount = 0;
				}
			}
			else if (!lstrcmp(L"SlaveSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"VajuraSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				if (0 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal13_N");
					++m_iAttackCount;
				}
				else if (1 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal14_N");
					++m_iAttackCount;
				}
				else if (2 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal15_N");
					m_iAttackCount = 0;
				}
			}
			else if (!lstrcmp(L"WhiteSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				if (0 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal9_N");
					++m_iAttackCount;
				}
				else if (1 == m_iAttackCount)
				{
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackNormal16_N");
					m_iAttackCount = 0;
				}
			}
			break;
		}
	}
		if(STATE_ATTACK == m_iPrevState)
			m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);

	m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
	m_pModel->Get_CurAnimation()->Set_FastValue(2.f);
	return S_OK;
}

HRESULT CPlayerStateMachine::AttackSp_State(CPlayer* _pOwnerObject)
{
	m_fAttackAnimationControlValue = 2.5f;
	if (nullptr == m_pWeapon)
	{
		return E_FAIL;
	}
	else
	{
		switch (m_pWeapon->Get_WeaponType())
		{
		case CWeapon::WEAPON_ONEHAND:
			if (!lstrcmp(L"AlisaSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong10B_N");
			}
			else if (!lstrcmp(L"BlackSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"BurningSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				int ran = 0;
				BEGIN_SINGLETON(CGameInstance, pGameInstance)
				{
					ran = pGameInstance->Get_RandomCount() % 2;
				}
				END_SINGLETON(pGameInstance);
				if (0 == ran)
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong01A_N");
				else
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong01B_N");
			}
			else if (!lstrcmp(L"MilitaryGreatSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalGreatSwordA", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalGreatSwordB", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalGreatSwordC", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalGreatSwordD", m_pWeapon->Get_WeaponName().c_str()))
			{
				m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong03A_N");
			}
			else if (!lstrcmp(L"GreatSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"HannibalGreatSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"SomaGreatSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong04B_N");
			}
			else if (!lstrcmp(L"NormalSwordA", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalSwordB", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalSwordC", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"NormalSwordD", m_pWeapon->Get_WeaponName().c_str()))
			{
				m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong06A_N");
			}
			else if (!lstrcmp(L"MilitarySword", m_pWeapon->Get_WeaponName().c_str()))
			{
				int ran = 0;
				BEGIN_SINGLETON(CGameInstance, pGameInstance)
				{
					ran = pGameInstance->Get_RandomCount() % 2;
				}
				END_SINGLETON(pGameInstance);
				if (0 == ran)
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong07A_N");
				else
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong07B_N");
			}
			else if (!lstrcmp(L"GhostSword", m_pWeapon->Get_WeaponName().c_str()) ||
				!lstrcmp(L"SlaveSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong09A_N");
			}
			else if (!lstrcmp(L"WhiteSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong10B_N");
			}
			else if (!lstrcmp(L"VajuraSword", m_pWeapon->Get_WeaponName().c_str())
				|| !lstrcmp(L"HannibalSword", m_pWeapon->Get_WeaponName().c_str()))
			{
				int ran = 0;
				BEGIN_SINGLETON(CGameInstance, pGameInstance)
				{
					ran = pGameInstance->Get_RandomCount() % 2;
				}
				END_SINGLETON(pGameInstance);
				if (0 == ran)
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong14A_N");
				else
					m_pModel->Change_Animation("AS_TStdUnisex_TSword_AttackStrong14B_N");
			}
			break;
		}
	}
	m_pModel->Get_NextAnimation()->Set_FastValue(2.f);
	return S_OK;
}

HRESULT CPlayerStateMachine::Healing_State(CPlayer* _pOwnerObject)
{
	return E_NOTIMPL;
}

HRESULT CPlayerStateMachine::Hit_State(CPlayer* _pOwnerObject)
{
	if (m_iPrevState == STATE_HIT)
		return E_FAIL;

	_float fDot = _pOwnerObject->DamageToDot_DirToSelfLookConverter();
	CCharacter::DIRECTION eDir = CCharacter::DIR_END;

	if (CCharacter::AS_STRONG == _pOwnerObject->Get_AtkStrongEncounter()
		 || CCharacter::AS_NORMAL == _pOwnerObject->Get_AtkStrongEncounter())
	{
		if (CCharacter::AS_STRONG == _pOwnerObject->Get_AtkStrongEncounter())
		{
			if (45.f >= fDot && -45.f <= fDot)
				eDir = CCharacter::DIR_BACK;
			else if (45.f <= fDot && fDot <= 135.f)
				eDir = CCharacter::DIR_LEFT;
			else if (135.f <= fDot || fDot <= -135.f)
				eDir = CCharacter::DIR_FOR;
			else if (-135 <= fDot && fDot <= -45)
				eDir = CCharacter::DIR_RIGHT;

			switch (eDir)
			{
			case CCharacter::DIR_FOR:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_DamageBlow_F");
				break;
			case CCharacter::DIR_LEFT:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_DamageBlow_L");
				break;
			case CCharacter::DIR_BACK:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_DamageBlow_B");
				break;
			case CCharacter::DIR_RIGHT:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_DamageBlow_R");
				break;
			}
		}
		else
		{
			if (90.f >= fDot && -90.f <= fDot)
				eDir = CCharacter::DIR_FOR;
			else if (90.f <= fDot || fDot <= -90.f)
				eDir = CCharacter::DIR_BACK;

			switch (eDir)
			{
			case CCharacter::DIR_FOR:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_DamageStrike_P");
				break;
			case CCharacter::DIR_BACK:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_DamageStrike_S");
				break;
			}
		}
	}
	else
	{
		if (CCharacter::DMG_LT == _pOwnerObject->Get_DmgPercents())
		{
			if (20.f >= fDot && -20.f <= fDot)
				eDir = CCharacter::DIR_BACK;
			else if (20.f <= fDot && fDot <= 80.f)
				eDir = CCharacter::DIR_BACKLEFT;
			else if (80.f <= fDot && fDot <= 120.f)
				eDir = CCharacter::DIR_LEFT;
			else if (120.f <= fDot && fDot <= 160.f)
				eDir = CCharacter::DIR_FORLEFT;
			else if (160.f <= fDot || fDot <= -160.f)
				eDir = CCharacter::DIR_FOR;
			else if (-160 <= fDot && fDot <= -120)
				eDir = CCharacter::DIR_BACKRIGHT;
			else if (-120 <= fDot && fDot <= -80)
				eDir = CCharacter::DIR_RIGHT;
			else if (-80 <= fDot && fDot <= -20)
				eDir = CCharacter::DIR_FORRIGHT;

			switch (eDir)
			{
			case CCharacter::DIR_FOR:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage01_F");
				break;
			case CCharacter::DIR_FORLEFT:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage01_FL");
				break;
			case CCharacter::DIR_LEFT:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage01_L");
				break;
			case CCharacter::DIR_BACKLEFT:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage01_BL");
				break;
			case CCharacter::DIR_BACK:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage01_B");
				break;
			case CCharacter::DIR_BACKRIGHT:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage01_BR");
				break;
			case CCharacter::DIR_RIGHT:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage01_R");
				break;
			case CCharacter::DIR_FORRIGHT:
				m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage01_FR");
				break;
			}
		}
		else
		{
			if (45.f >= fDot && -45.f <= fDot)
				eDir = CCharacter::DIR_FOR;
			else if (45.f <= fDot && fDot <= 135.f)
				eDir = CCharacter::DIR_LEFT;
			else if (135.f <= fDot || fDot <= -135.f)
				eDir = CCharacter::DIR_BACK;
			else if (-135 <= fDot && fDot <= -45)
				eDir = CCharacter::DIR_RIGHT;

			if (CCharacter::DMG_LT == _pOwnerObject->Get_DmgPercents())
			{
				switch (eDir)
				{
				case CCharacter::DIR_FOR:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage02_F");
					break;
				case CCharacter::DIR_LEFT:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage02_L");
					break;
				case CCharacter::DIR_BACK:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage02_B");
					break;
				case CCharacter::DIR_RIGHT:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage02_R");
					break;
				}
			}
			else if (CCharacter::DMG_BG == _pOwnerObject->Get_DmgPercents())
			{
				switch (eDir)
				{
				case CCharacter::DIR_FOR:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage03_F");
					break;
				case CCharacter::DIR_LEFT:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage03_L");
					break;
				case CCharacter::DIR_BACK:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage03_B");
					break;
				case CCharacter::DIR_RIGHT:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage03_R");
					break;
				}
			}
			else if (CCharacter::DMG_GG == _pOwnerObject->Get_DmgPercents())
			{
				switch (eDir)
				{
				case CCharacter::DIR_FOR:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage04_F");
					break;
				case CCharacter::DIR_LEFT:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage04_L");
					break;
				case CCharacter::DIR_BACK:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage04_B");
					break;
				case CCharacter::DIR_RIGHT:
					m_pModel->Change_Animation("AS_TStdUnisex_TCmn_Damage04_R");
					break;
				}
			}
		}
	}

	m_pModel->Get_CurAnimation()->Set_FastValue(10.f);
	return S_OK;
}

HRESULT CPlayerStateMachine::Skill_State(CPlayer* _pOwnerObject)
{
	return E_NOTIMPL;
}

HRESULT CPlayerStateMachine::OpenSight_State(CPlayer* _pOwnerObject)
{
	return E_NOTIMPL;
}

HRESULT CPlayerStateMachine::Find_CheckPoints_Ect_Behavior(CPlayer* _pOwnerObject)
{
	switch (m_iActivation_CheckPoints)
	{
	case 0:
		m_pModel->Change_Animation("AS_TStdUnisex_TCmn_TriggerAc_Checkpoint_Start");
		m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
		++m_iActivation_CheckPoints;
		break;
	case 1:
		m_pModel->Change_Animation("AS_TStdUnisex_TCmn_TriggerAc_Checkpoint_Loop");
		m_pModel->Get_CurAnimation()->Set_SelectAnimPos(CAnimation::POS_DEST);
		m_pModel->Get_NextAnimation()->Set_FastValue(3.f);
		++m_iActivation_CheckPoints;
		break;
	case 2:
		m_pModel->Change_Animation("AS_TStdUnisex_TCmn_TriggerAc_Checkpoint_End");
		++m_iActivation_CheckPoints;
		break;
	}

	return S_OK;
}

HRESULT CPlayerStateMachine::Dead_State(CPlayer* _pOwnerObject)
{
	m_pModel->Change_Animation("AS_TStdUnisex_TCmn_DeathIdle_F");
	return S_OK;
}

CPlayerStateMachine* CPlayerStateMachine::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext, CGameObject* _pGameObject)
{
	CPlayerStateMachine* pInstance = new CPlayerStateMachine(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct(_pGameObject)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPlayerStateMachine::Create To Failed");
	}
	return pInstance;
}

void CPlayerStateMachine::Free()
{
	Safe_Release(m_pLeftLeg);
	Safe_Release(m_pRightLeg);
	__super::Free();
}
