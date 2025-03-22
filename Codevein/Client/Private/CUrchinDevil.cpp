#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CUrchinDevil.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CPlayer.h"
#include "CMonsterEPController.h"

CUrchinDevil::CUrchinDevil(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CMonster(_pDevice, _pContext)
{
}

CUrchinDevil::CUrchinDevil(const CUrchinDevil& _rhs)
	: CMonster(_rhs)
{
}

HRESULT CUrchinDevil::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUrchinDevil::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		URCHINDEVILDESC tDesc = *static_cast<URCHINDEVILDESC*>(pArg);
		m_eInitIdleType = tDesc.eInitAnimType;
		m_tGobDesc = tDesc.tGobDesc;
		m_tMonsterDesc = tDesc.tMonsterDesc;
		m_tPawnStatus = tDesc.tPawnStatus;

		m_tMonsterDesc.tSightDesc.fFindAngle = 60.f;
		m_tMonsterDesc.tSightDesc.fFindDistance = 10.f;
		m_tMonsterDesc.tSightDesc.fYFindAngle = 1.f;
		m_tCharInfo.UpdateInfo(m_tPawnStatus);
	}

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Others()))
		return E_FAIL;

	m_fCurSpeed = 5.f;
	m_fRotationStandardValue = XMConvertToRadians(90.f);

	m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_IDLE, this);

	if (nullptr != m_pCollCollider)
		m_fDrawRadius = sqrtf(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f, XMLoadFloat3(&m_pCollCollider->Get_Scale()) * 2.f)));

	m_pNavigation->Find_Cell(m_pTransformCom->Get_Pos());
	return S_OK;
}

void CUrchinDevil::Tick(const _double& _dTimeDelta)
{
	if (nullptr != m_pModelCom)
		m_pModelCom->Update_Animation(_dTimeDelta);

	if (false == g_bGameRun)
	{
		Coll_Tick(m_pTransformCom->Get_WorldMatrix());
		return;
	}
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		Bind_Supplement_Position();
		IDLE_STATE(_dTimeDelta, pGameInstance);
		FIND_STATE(_dTimeDelta, pGameInstance);
		RETARGET_STATE(_dTimeDelta, pGameInstance);
		RUN_STATE(_dTimeDelta, pGameInstance);
		ATTACK_STATE(_dTimeDelta, pGameInstance);
		DAMAGED_STATE(_dTimeDelta, pGameInstance);
		DIE_STATE(_dTimeDelta, pGameInstance);
		m_pUrchinDevilStateMachine->Tick(_dTimeDelta, this);
		Calculate_OffsetPosition();
		Add_AroundPossibleAttackPlayer();
	}
	END_SINGLETON(pGameInstance);
}

void CUrchinDevil::Late_Tick(const _double& _dTimeDelta)
{
	if (true == g_bGameRun)
	{
		m_pUrchinDevilStateMachine->Late_Tick(_dTimeDelta, this);
		Restrict_AnimationToMoveOffset(m_pUrchinDevilStateMachine);
	}
	__super::Late_Tick(_dTimeDelta);
}

HRESULT CUrchinDevil::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUrchinDevil::Damaged(CPawn* _pDamageOwner)
{
	if (CUrchinDevilStateMachine::STATE_DIE == m_pUrchinDevilStateMachine->Get_State())
		return E_FAIL;

	if (CPawn::P_ITEM == _pDamageOwner->Get_PawnType())
	{
		CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
		if (nullptr == pWeapon)
			return E_FAIL;;

		m_tCharInfo.Damaged(pWeapon->Get_CharInfo().Get_Damage());
		m_bChangeSuppleCount = true;

		if (m_tCharInfo.Is_CheckDie())
		{
			if (nullptr != m_pMonsterEPController)
			{
				CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
				_float3 v3Pos;
				_vector vChestPos = Get_OffsetArrPosition(NODE_CHEST);
				XMStoreFloat3(&v3Pos, XMLoadFloat3((_float3*)&pWeapon->Get_SockMatrix4x4().m[3][0]));
				_vector vDir = vChestPos - XMLoadFloat3(&v3Pos);
				m_pMonsterEPController->Show_Blood(vChestPos, vDir);
			}
			m_bDead = true;
			m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_DIE, this);
		}
		else
		{
			if (true == m_tHurtTimer.bPass)
			{
				if (nullptr != m_pMonsterEPController)
				{
					CWeapon* pWeapon = static_cast<CWeapon*>(_pDamageOwner);
					_float3 v3Pos;
					_vector vChestPos = Get_OffsetArrPosition(NODE_CHEST);
					XMStoreFloat3(&v3Pos, XMLoadFloat3((_float3*)&pWeapon->Get_SockMatrix4x4().m[3][0]));
					_vector vDir = vChestPos - XMLoadFloat3(&v3Pos);
					m_pMonsterEPController->Show_Blood(vChestPos, vDir);
				}
				m_tHurtTimer.Reset_Timer();
				if (CUrchinDevilStateMachine::STATE_ATTACK == m_pUrchinDevilStateMachine->Get_State() || 
					CUrchinDevilStateMachine::STATE_HIT == m_pUrchinDevilStateMachine->Get_State())
					return E_FAIL;
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_HIT, this);
			}
		}
	}
	else  if (CPawn::P_CHARACTER == _pDamageOwner->Get_PawnType())
	{

	}
	return S_OK;
}

HRESULT CUrchinDevil::Ready_Component()
{
	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(COM_ANIMMODEL, g_iCurrentLevel, m_tMonsterDesc.strModelName.c_str()
		, (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(Reset_TransformData()))
		return E_FAIL;

	{
		// CollCollider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(0.5f, 1.f, 0.5f);
		tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
		/* For.Com_Shader */
		if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
			PROTO_COMP_OBBCOLLIDER, (CComponent**)&m_pCollCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// First Check
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(0.5f, 1.f, 0.5f);
		tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
		if (FAILED(__super::Add_Component(COM_FIRSTCHECKCOLLIDER, LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER,
			(CComponent**)&m_pFirstCheckCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// Find Range Collider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(5.f, 0.7f, 2.5f);
		tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
		if (FAILED(__super::Add_Component(COM_FINDCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
			(CComponent**)&m_pRangeCollider, &tCollDesc)))
			return E_FAIL;
	}

	{
		// Find Range Collider
		CCollider::COLLIDERDESC tCollDesc;
		tCollDesc.vScale = _float3(12.f, 0.7f, 10.f);
		tCollDesc.vTranslation = _float3(0.f, 0.7f, 0.f);
		if (FAILED(__super::Add_Component(COM_ATTACKDISCOLLIDER, LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
			(CComponent**)&m_pAttackNowRangeCollider, &tCollDesc)))
			return E_FAIL;
	}


	if (FAILED(Find_OffsetNode()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUrchinDevil::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUrchinDevil::Ready_Others()
{
	if (FAILED(__super::Ready_Others()))
		return E_FAIL;

	m_pUrchinDevilStateMachine = CUrchinDevilStateMachine::Create(m_pDevice, m_pContext, this);
	if (nullptr == m_pUrchinDevilStateMachine)
		return E_FAIL;

	return S_OK;
}

void CUrchinDevil::Calculate_OffsetPosition()
{
/*	if (false == m_pModelCom->Is_ChangeAnimation())
	{
		_matrix matWorld = 
			m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() * 
			m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		_float3 v3Pos;
		XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
		matWorld = m_pTransformCom->Get_WorldMatrix();
		matWorld.r[3] = XMLoadFloat3(&m_v3OffsetPosition);
		XMStoreFloat4x4(&m_OffsetMatrix, matWorld);
		Coll_Tick(matWorld);
	}
	else*/ if (CUrchinDevilStateMachine::STATE_ATTACK == m_pUrchinDevilStateMachine->Get_State())
	{
		_matrix matWorld = 
			m_pNodeArr[NODE_OFFSET]->Get_CombinedTransformationMatrix() *
			m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();

		_float3 v3Pos;
		XMStoreFloat3(&v3Pos, m_pTransformCom->Get_Pos());
		m_v3OffsetPosition.y = v3Pos.y;

		XMStoreFloat3(&m_v3OffsetPosition, matWorld.r[3]);
		matWorld = m_pTransformCom->Get_WorldMatrix();
		matWorld.r[3] = XMLoadFloat3((_float3*)&m_v3OffsetPosition);
		XMStoreFloat4x4(&m_DrawAnimMatrix, matWorld);
		Coll_Tick(matWorld);
	}
	else
	{
		Coll_Tick(m_pTransformCom->Get_WorldMatrix());
		XMStoreFloat4x4(&m_DrawAnimMatrix, m_pTransformCom->Get_WorldMatrix());
		XMStoreFloat3(&m_v3OffsetPosition, m_pTransformCom->Get_Pos());
		m_bRestrictAttackPos = false;
	}
}

void CUrchinDevil::Add_AroundPossibleAttackPlayer()
{
	if (CUrchinDevilStateMachine::STATE_ATTACK == m_pUrchinDevilStateMachine->Get_State())
	{
		if (nullptr != m_pCollCollider)
		{
			CCollider* pCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
			if (nullptr == pCollider)
				return;

			if (true == m_pRangeCollider->Is_Collision(pCollider))
				return;

			pCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_COLLCOLLIDER));
			if (nullptr == pCollider)
				return;
			if (true == m_pCollCollider->Is_Collision(pCollider))
			{
				m_pTargetPlayer->Damaged(this);
			}
		}
	}
}

void CUrchinDevil::IDLE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (nullptr == m_pTargetPlayer)
		return;

	if (CUrchinDevilStateMachine::STATE_IDLE != m_pUrchinDevilStateMachine->Get_State())
		return;

	CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));

	if (nullptr == pPlayerCheckCollider || nullptr == pPlayerTransform)
		return;

	if (INIT_IDLE == m_eInitIdleType)
	{
		if (false == m_bFindPlayer)
		{
			if (true == m_pAttackNowRangeCollider->Is_Collision(pPlayerCheckCollider))
			{
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_FIND, this);
				m_bFindPlayer = true;
			}
			else
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_IDLE, this);
		}
		else
		{
			m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_FIND, this);
		}
	}
}

void CUrchinDevil::FIND_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CUrchinDevilStateMachine::STATE_FIND != m_pUrchinDevilStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_RETARGET, this);
	}
}

void CUrchinDevil::RETARGET_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CUrchinDevilStateMachine::STATE_RETARGET != m_pUrchinDevilStateMachine->Get_State())
		return;

	if (true == RotateLookToPlayer(_dTimeDelta, 12.))
	{
		_float fDistance = PlayerToDistance();
		if (ATK_1 == m_iAttackIndex || ATK_2 == m_iAttackIndex || ATK_SP == m_iAttackIndex)
		{
			if (fDistance <= 5)
			{
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_ATTACK, this);
			}
			else
			{
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_RUN, this);
			}
		}
		else if (ATK_3 == m_iAttackIndex)
		{
			if (fDistance <= 8)
			{
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_ATTACK, this);
			}
			else
			{
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_RUN, this);
			}
		}
	}
}

void CUrchinDevil::RUN_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CUrchinDevilStateMachine::STATE_RUN != m_pUrchinDevilStateMachine->Get_State())
		return;

	if (true == RotateLookToPlayer_JudgeNotRepeat(_dTimeDelta, 12.))
	{
		_float fDistance = PlayerToDistance();
		if (fDistance <= 5)
		{
			m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_ATTACK, this);
		}
		else
		{
			m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
			m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
		}
	}
	else
	{
		m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
	}
}

void CUrchinDevil::ATTACK_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndAttack)
	{
		Bind_Attack_Postion();
		m_bEndAttack = false;
	}

	if (CUrchinDevilStateMachine::STATE_ATTACK != m_pUrchinDevilStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_bChangeAttackMatrix = true;
		m_bEndAttack = true;
		m_bChangeSuppleCount = true;

		if (ATK_1 == m_iAttackIndex || ATK_SP == m_iAttackIndex)
		{
			if (0 == m_pUrchinDevilStateMachine->Get_AttackMandibleIndex())
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_ATTACK, this);
			else
			{
				_uint k = _pGameInstance->Get_RandomCount() % 100;

				if (m_iAttackIndex == ATK_1)
				{
					if (k < 50)
					{
						m_iAttackIndex = ATK_SP;
					}
					else
					{
						_uint i = _pGameInstance->Get_RandomCount() % 100;
						if (i < 40)
							m_iAttackIndex = ATK_1;
						else if (i < 70)
							m_iAttackIndex = ATK_2;
						else if (i < 100)
							m_iAttackIndex = ATK_3;
					}
				}
				else
				{
					_uint i = _pGameInstance->Get_RandomCount() % 100;
					if (i < 40)
						m_iAttackIndex = ATK_1;
					else if (i < 70)
						m_iAttackIndex = ATK_2;
					else if (i < 100)
						m_iAttackIndex = ATK_3;
				}
				m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_RETARGET, this);
			}
		}
		else
		{
			_uint k = _pGameInstance->Get_RandomCount() % 100;

			if (m_iAttackIndex == ATK_1)
			{
				if (k < 50)
				{
					m_iAttackIndex = ATK_SP;
				}
				else
				{
					_uint i = _pGameInstance->Get_RandomCount() % 100;
					if (i < 40)
						m_iAttackIndex = ATK_1;
					else if (i < 70)
						m_iAttackIndex = ATK_2;
					else if (i < 100)
						m_iAttackIndex = ATK_3;
				}
			}
			else
			{
				_uint i = _pGameInstance->Get_RandomCount() % 100;
				if (i < 40)
					m_iAttackIndex = ATK_1;
				else if (i < 70)
					m_iAttackIndex = ATK_2;
				else if (i < 100)
					m_iAttackIndex = ATK_3;
			}
			m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_RETARGET, this);
		}
	}
	else
	{
		if (ATK_1 == m_iAttackIndex || ATK_SP == m_iAttackIndex)
		{
			if (1 == m_pUrchinDevilStateMachine->Get_AttackCount())
				m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
		}
		else
		{
			Calculate_OffsetMatrix();
		}
	}
}

void CUrchinDevil::DAMAGED_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (CUrchinDevilStateMachine::STATE_HIT != m_pUrchinDevilStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_bChangeDamagedPosition = true;
		m_pUrchinDevilStateMachine->Change_State(CUrchinDevilStateMachine::STATE_RETARGET, this);
		Bind_Damaged_Position();
	}
	else
	{
		Calculate_OffsetMatrix();
	}
}

void CUrchinDevil::DIE_STATE(const _double& _dTimeDelta, CGameInstance* _pGameInstance)
{
	if (true == m_bEndDiePosition)
	{
		m_bEndDiePosition = false;
		Bind_Die_Position();
	}

	if (CUrchinDevilStateMachine::STATE_DIE != m_pUrchinDevilStateMachine->Get_State())
		return;

	if (true == m_pModelCom->Get_CurAnimation()->Is_Finished())
	{
		m_bChangeDamagedPosition = true;
		m_bEndDiePosition = true;
	}
	else
	{
		Calculate_OffsetMatrix();
	}
}

#pragma region USE_TOOL_METHOD
_bool CUrchinDevil::Extern_EditValue()
{
	_bool bActive = false;

	if (ImGui::TreeNodeEx(Get_Name()))
	{
		ImGui::PushItemWidth(200);
		Extern_PawnData();
		Extern_GobData();
		Extern_InitAnation();
		Extern_ModelType();
		Extern_ModifySightDesc();
		bActive = ImGui::Button("Create");
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	return bActive;
}

_bool CUrchinDevil::Extern_HierakeyView(const string& _str)
{
	if (nullptr == m_pTransformCom)
		return false;

	_bool bActive = false;
	if (ImGui::TreeNodeEx(_str.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushItemWidth(200);
		Extern_PawnData();
		Extern_GobData();
		Control_ColliderData();
		Extern_ShowCurAnimation_Data();
		_bool bClone = ImGui::Button("Clone");
		if (true == bClone)
		{
			BEGIN_SINGLETON(CGameInstance, pGameInstance);
			{
				URCHINDEVILDESC tDesc;
				tDesc.eInitAnimType = m_eInitIdleType;
				tDesc.tGobDesc = m_tGobDesc;
				tDesc.tMonsterDesc = m_tMonsterDesc;
				tDesc.tPawnStatus = m_tPawnStatus;
				pGameInstance->Clone_GameObject(g_iCurrentLevel, Get_Layer(), Get_Proto(), &tDesc);
			}
			END_SINGLETON(pGameInstance);
		}
		bActive = ImGui::Button("Delete");
		ImGui::PopItemWidth();
		ImGui::TreePop();
		return bActive;
	}
	return false;
}

HRESULT CUrchinDevil::Create_Self(CGameObject** _ppOut)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		URCHINDEVILDESC tDesc;
		tDesc.eInitAnimType = m_eInitIdleType;
		tDesc.tGobDesc = m_tGobDesc;
		tDesc.tMonsterDesc = m_tMonsterDesc;
		tDesc.tPawnStatus = m_tPawnStatus;
		// Save 
		*_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
			, Get_Proto(), &tDesc));
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CUrchinDevil::Delete_Self()
{
	return __super::Delete_Self();
}

HRESULT CUrchinDevil::Save_Data()
{
	BEGIN_SINGLETON(CData_Manager, pDataManager);
	{
		URCHINDEVILDESC tDesc;
		tDesc.eInitAnimType = m_eInitIdleType;
		tDesc.tGobDesc = m_tGobDesc;
		tDesc.tMonsterDesc = m_tMonsterDesc;
		tDesc.tPawnStatus = m_tPawnStatus;
		// Save 
		pDataManager->Get_MonsterMgr()->Insert_UrchinDevilData(tDesc);
	}
	END_SINGLETON(pDataManager);
	return S_OK;
}

HRESULT CUrchinDevil::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		*_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
			, Get_Proto(), _pData));
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

_bool CUrchinDevil::Is_Picking(_float3* _pOut)
{
	return __super::Is_Picking(_pOut);
}

void CUrchinDevil::Reset_Data()
{
	__super::Reset_Data();
}

void CUrchinDevil::Extern_InitAnation()
{
	const char* IdleType[INIT_ANIMTYPE::INIT_END] = { "Idle", "Walk" };
	ImGui::Combo("IdleType", (_int*)&m_eInitIdleType, IdleType, INIT_ANIMTYPE::INIT_END);
}

void CUrchinDevil::Extern_ModelType()
{
	const char* ModelType[2] = { "V1", "V2" };
	ImGui::Combo("ModelType", &m_iModelID, ModelType, 2);
	switch (m_iModelID)
	{
	case 0:
		m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_URCHINDEVIL_V1;
		break;
	case 1:
		m_tMonsterDesc.strModelName = PROTO_COMP_MODEL_URCHINDEVIL_V2;
		break;
	}
}
#pragma endregion USE_TOOL_METHOD

CUrchinDevil* CUrchinDevil::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CUrchinDevil* pInstance = new CUrchinDevil(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CUrchinDevil::Create To Failed");
	}
	return pInstance;
}

CGameObject* CUrchinDevil::Clone(void* pArg)
{
	CUrchinDevil* pInstance = new CUrchinDevil(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CUrchinDevil::Create To Failed");
	}
	return pInstance;
}

void CUrchinDevil::Free()
{
	Safe_Release(m_pUrchinDevilStateMachine);
	__super::Free();
}