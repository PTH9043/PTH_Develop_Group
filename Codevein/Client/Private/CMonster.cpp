#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CMonster.h"
#include "CGameInstance.h"
#include "CPlayer.h"
#include "CRayCast.h"
#include "CWeapon.h"
#include "CMonsterState.h"
#include "CMonsterEPController.h"

CMonster::CMonster(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CMonster::CMonster(const CMonster& _rhs)
	: CCharacter(_rhs)
{
}

HRESULT CMonster::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMonster::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	m_fRotValue = XMConvertToRadians(90.f);
	m_byPassIndex = 1;
	return S_OK;
}

void CMonster::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CMonster::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		if (true == g_bGameRun)
		{
			Find_Player(pGameInstance);
			Dead_Monster(_dTimeDelta);
		}

		if (true == pGameInstance->Is_Rendering(m_pTransformCom->Get_Pos(), m_pRangeCollider->Get_Scale().x))
		{
#ifdef _DEBUG
			Add_Collider_Render();
#endif
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
	}
	END_SINGLETON(pGameInstance);
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (nullptr != m_pModelCom)
	{
		_uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();

		for (_uint i = 0; i < iNumMeshContainers; ++i)
		{
			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TEXTYPE::TextureType_NORMALS)))
				return E_FAIL;

			m_pModelCom->Render(i, m_pShaderCom, m_byPassIndex);
		}
	}
	return S_OK;
}

_float CMonster::PlayerToDistance()
{
	if (nullptr == m_pTargetPlayer)
		return 0.f;

	_float fDistance = 0.f;
	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));
	if (nullptr != pPlayerTransform)
		fDistance = XMVectorGetX(XMVector3Length(pPlayerTransform->Get_Pos() - m_pTransformCom->Get_Pos()));

	return  fDistance;
}

_float CMonster::PlayerToDot_DirToSelfLook()
{
	_float fDot = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fDot;

	_vector vLook1 = m_pTargetPlayer->Get_OffsetPos() - m_pTransformCom->Get_Pos();
	_vector vLook2 = m_pTransformCom->Get_Look();

	//XMVectorSetY(vLook1, 0.0f);

	_vector vDot = XMVector3Dot(XMVector3Normalize(vLook1), XMVector3Normalize(vLook2));
	return XMVectorGetX(vDot);
}

_float CMonster::PlayerToDot_DirToSelfRight()
{
	_float fDot = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fDot;

	_vector vLook1 = m_pTargetPlayer->Get_OffsetPos() - m_pTransformCom->Get_Pos();
	_vector vLook2 = m_pTransformCom->Get_Right();

	XMVectorSetY(vLook1, 0.0f);

	_vector vDot = XMVector3Dot(XMVector3Normalize(vLook1), XMVector3Normalize(vLook2));
	return XMVectorGetX(vDot);
}

_float CMonster::PlayerToDot_LookToLook()
{
	_float fDot = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fDot;

	CTransform* pTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));
	if (nullptr == pTransform)
		return fDot;

	_vector vLook1 = pTransform->Get_Look();
	_vector vLook2 = -m_pTransformCom->Get_Look();

	_vector vDot = XMVector3Dot(XMVector3Normalize(vLook1), XMVector3Normalize(vLook2));
	return XMVectorGetX(vDot);
}

_float CMonster::PlayerToDot_DirToSelfLookConvert()
{
	_float fDot = XMConvertToDegrees(acosf(PlayerToDot_DirToSelfLook()));
	_float fValue = PlayerToDot_DirToSelfRight();

	if (fValue < 0)
		fDot = -fDot;

	return fDot;
}

_float CMonster::SelfToDot_DirToPlayerLook()
{
	_float fDot = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fDot;

	CTransform* pTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));
	if (nullptr == pTransform)
		return fDot;

	_vector vLook1 = m_pTransformCom->Get_Pos() - m_pTargetPlayer->Get_OffsetPos();
	_vector vLook2 = pTransform->Get_Look();

	XMVectorSetY(vLook1, 0.0f);

	_vector vDot = XMVector3Dot(XMVector3Normalize(vLook1), XMVector3Normalize(vLook2));
	return XMVectorGetX(vDot);
}

_float CMonster::SelfToDot_DirToPlayerRight()
{
	_float fDot = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fDot;

	CTransform* pTransform = static_cast<CTransform*>(m_pTargetPlayer->Get_Component(COM_TRANSFORM));
	if (nullptr == pTransform)
		return fDot;

	_vector vLook1 = m_pTransformCom->Get_Pos() - m_pTargetPlayer->Get_OffsetPos();
	_vector vLook2 = pTransform->Get_Right();

	XMVectorSetY(vLook1, 0.0f);

	_vector vDot = XMVector3Dot(XMVector3Normalize(vLook1), XMVector3Normalize(vLook2));
	return XMVectorGetX(vDot);
}

_float CMonster::SelfToDot_DirToPlayerLookConvert()
{
	_float fDot = XMConvertToDegrees(acosf(SelfToDot_DirToPlayerLook()));
	_float fValue = SelfToDot_DirToPlayerRight();

	if (fValue < 0)
		fDot = -fDot;

	return fDot;
}

_float CMonster::PlayerToTan2ZXValue()
{
	_float fValue = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fValue;

	_vector vValue = m_pTransformCom->Get_Pos() - m_pTargetPlayer->Get_OffsetPos();
	return XMConvertToDegrees(atan2f(XMVectorGetZ(vValue), XMVectorGetX(vValue)));
}

_float CMonster::PlayerToTan2XZValue()
{
	_float fValue = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fValue;

	_vector vValue = m_pTransformCom->Get_Pos() - m_pTargetPlayer->Get_OffsetPos();
	return XMConvertToDegrees(atan2f(XMVectorGetX(vValue), XMVectorGetZ(vValue)));
}

_float CMonster::PlayerToGetZValue()
{
	_float fValue = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fValue;

	_vector vValue = m_pTransformCom->Get_Pos() - m_pTargetPlayer->Get_OffsetPos();
	return XMVectorGetZ(vValue);
}

_float CMonster::PlayerToGetYValue()
{
	_float fValue = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fValue;

	_vector vValue = m_pTransformCom->Get_Pos() - m_pTargetPlayer->Get_OffsetPos();
	return XMVectorGetY(vValue);
}

_float CMonster::PlayerToGetXValue()
{
	_float fValue = 0.f;

	if (nullptr == m_pTargetPlayer)
		return fValue;

	_vector vValue = m_pTransformCom->Get_Pos() - m_pTargetPlayer->Get_OffsetPos();
	return XMVectorGetX(vValue);
}

_bool CMonster::RotateLookToPlayer(const _double& _dTimeDelta, const double& _dAngleValue)
{
	_float fValue = PlayerToDot_DirToSelfLookConvert();

	if (fValue >= _dAngleValue || fValue <= -_dAngleValue)
	{
		if (fValue >= _dAngleValue)
			m_pTransformCom->Rotate_Turn(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), _dTimeDelta, m_fRotValue);
		else if (fValue <= -_dAngleValue)
			m_pTransformCom->Rotate_Turn(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), _dTimeDelta, -m_fRotValue);

		return false;
	}

	return true;
}

_bool CMonster::RotateLookToPlayer_JudgeNotRepeat(const _double& _dTimeDelta, const _double& _dAngleValue)
{
	_float fValue = PlayerToDot_DirToSelfLookConvert();
	_float fTanValue = PlayerToTan2ZXValue();
	_float fRotValue = m_fRotValue;
	if (m_iRotateCount == 0)
	{
		if (false == m_bInitRotate)
		{
			m_bInitRotate = true;
			m_fLookToRotateValue = fTanValue;
		}
		else
		{
			if (fTanValue - 1.2f <= (_int)m_fLookToRotateValue &&
				(_int)m_fLookToRotateValue <= fTanValue + 1.2f)
			{
				++m_iRotateCount;
			}
		}
	}
	else
	{
		_float fDistance = PlayerToDistance();
		if (fDistance >= 8)
		{
			m_bInitRotate = false;
			m_iRotateCount = 0;
		}
		else
			fRotValue /= 4.f;
	}

	if (fValue >= _dAngleValue || fValue <= -_dAngleValue)
	{
		if (fValue >= _dAngleValue)
			m_pTransformCom->Rotate_Turn(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), _dTimeDelta, fRotValue);
		else if (fValue <= -_dAngleValue)
			m_pTransformCom->Rotate_Turn(XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 0.f)), _dTimeDelta, -fRotValue);

		return false;
	}

	m_iRotateCount = 0;
	return true;
}

_bool CMonster::JustRotate(const _double& _dTimeDelta, const _float& _fPMValue)
{
	return _bool();
}

void CMonster::Calculate_DirectionToLookPlayer()
{
	m_ePrevDirection = m_eDirection;

	_float fDot = PlayerToDot_DirToSelfLookConvert();
	if (20.f >= fDot && -20.f <= fDot)
		m_eDirection = CCharacter::DIR_FOR;
	else if (20.f <= fDot && fDot <= 80.f)
		m_eDirection = CCharacter::DIR_FORLEFT;
	else if (80.f <= fDot && fDot <= 120.f)
		m_eDirection = CCharacter::DIR_LEFT;
	else if (120.f <= fDot && fDot <= 160.f)
		m_eDirection = CCharacter::DIR_BACKLEFT;
	else if (160.f <= fDot || fDot <= -160.f)
		m_eDirection = CCharacter::DIR_BACK;
	else if (-160 <= fDot && fDot <= -120)
		m_eDirection = CCharacter::DIR_BACKRIGHT;
	else if (-120 <= fDot && fDot <= -80)
		m_eDirection = CCharacter::DIR_RIGHT;
	else if (-80 <= fDot && fDot <= -20)
		m_eDirection = CCharacter::DIR_FORRIGHT;
}

void CMonster::Calculate_DirectionToPlayerPos()
{
	_float fDot = PlayerToTan2ZXValue();
	if (20.f >= fDot && -20.f <= fDot)
		m_eDirection = CCharacter::DIR_FOR;
	else if (20.f <= fDot && fDot <= 80.f)
		m_eDirection = CCharacter::DIR_FORLEFT;
	else if (80.f <= fDot && fDot <= 120.f)
		m_eDirection = CCharacter::DIR_LEFT;
	else if (120.f <= fDot && fDot <= 160.f)
		m_eDirection = CCharacter::DIR_BACKLEFT;
	else if (160.f <= fDot || fDot <= -160.f)
		m_eDirection = CCharacter::DIR_BACK;
	else if (-160 <= fDot && fDot <= -120)
		m_eDirection = CCharacter::DIR_BACKRIGHT;
	else if (-120 <= fDot && fDot <= -80)
		m_eDirection = CCharacter::DIR_RIGHT;
	else if (-80 <= fDot && fDot <= -20)
		m_eDirection = CCharacter::DIR_FORRIGHT;
}

void CMonster::Move_ToDirection(const _double& _dTimeDelta)
{
	if (DIR_FOR == m_eDirection)
	{
		m_pTransformCom->Move_Forward(_dTimeDelta, m_fCurSpeed);
	}
	else if (DIR_LEFT == m_eDirection)
	{
		m_pTransformCom->Move_Left(_dTimeDelta, m_fCurSpeed);
	}
	else if (DIR_RIGHT == m_eDirection)
	{
		m_pTransformCom->Move_Right(_dTimeDelta, m_fCurSpeed);
	}
	else if (DIR_BACK == m_eDirection)
	{
		m_pTransformCom->Move_Back(_dTimeDelta, m_fCurSpeed);
	}
	else if (DIR_FORLEFT == m_eDirection)
	{
		m_pTransformCom->Translate_Dir(XMVector3Normalize(-m_pTransformCom->Get_Right() + m_pTransformCom->Get_Look()), 
			_dTimeDelta, m_fCurSpeed);
	}
	else if (DIR_FORRIGHT == m_eDirection)
	{
		m_pTransformCom->Translate_Dir(XMVector3Normalize(m_pTransformCom->Get_Right() + m_pTransformCom->Get_Look()),
			_dTimeDelta, m_fCurSpeed);
	}
	else if (DIR_BACKLEFT == m_eDirection)
	{
		m_pTransformCom->Translate_Dir(XMVector3Normalize(-m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look()),
			_dTimeDelta, m_fCurSpeed);
	}
	else if (DIR_BACKRIGHT == m_eDirection)
	{
		m_pTransformCom->Translate_Dir(XMVector3Normalize(m_pTransformCom->Get_Right() - m_pTransformCom->Get_Look()),
			_dTimeDelta, m_fCurSpeed);
	}
}

void CMonster::Dead_Monster(const _double& _dTimeDelta)
{
	if (true == m_bDead)
	{
		m_DeadTime_fPlusTimeDelta += (_float)(_dTimeDelta * 0.7f);
		m_byPassIndex = 2;

		if (m_pModelCom->Get_CurAnimation()->Get_AnimTimeSituation(1.5f))
		{
			_matrix matWorld =
				m_pNodeArr[NODE_CHEST]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() *
				m_pTransformCom->Get_WorldMatrix();
			m_pMonsterEPController->Show_DeadParticle(matWorld.r[3]);
			m_pMonsterEPController->Stop_DeadParticle_Show();
		}
		else
		{
			if (nullptr != m_pMonsterEPController)
			{
				_matrix matWorld =
					m_pNodeArr[NODE_CHEST]->Get_CombinedTransformationMatrix() * m_pModelCom->Get_PivotMatrix() *
					m_pTransformCom->Get_WorldMatrix();
				m_pMonsterEPController->Show_DeadParticle(matWorld.r[3]);
			}
		}
	}
}

HRESULT CMonster::Ready_Component()
{
	if (FAILED(__super::Ready_Component()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(COM_TEXTURE, LEVEL_STATIC,
		PROTO_COMP_TEXTURE_PARTICLEGROUP, (CComponent**)&m_pDisorveTexture)))
		return E_FAIL;

	CMonsterEPController::MONEPCONDESC tDesc;
	tDesc._pMonster = this; 
	m_pMonsterEPController = CMonsterEPController::Create(m_pDevice, m_pContext, &tDesc);

	return S_OK;
}

HRESULT CMonster::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	if (nullptr != m_pShaderCom)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_fPlusTimeDelta", &m_DeadTime_fPlusTimeDelta, sizeof(_float))))
			return E_FAIL;

		if (nullptr != m_pDisorveTexture)
		{
			if (FAILED(m_pDisorveTexture->SetUp_ConstantTable_FileName(m_pShaderCom, "g_DissorveTexture", L"T_FX_Enemy_Executioner_WarterNoiz")))
				return E_FAIL;

			if (FAILED(m_pDisorveTexture->SetUp_ConstantTable_FileName(m_pShaderCom, "g_Dissorve_ShowTexture", L"T_FX_GEZNoise03")))
				return E_FAIL;

		}
	}

	return S_OK;
}

HRESULT CMonster::Ready_Others()
{
	return S_OK;
}

_bool CMonster::Is_FindTarget(CGameInstance* _pGameInstance)
{
	if (nullptr == m_pCollCollider || nullptr == m_pTargetPlayer)
		return false;

	SIGHTDESC tSightDesc = m_tMonsterDesc.tSightDesc;

	CCollider* _pTargetCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_COLLCOLLIDER));
	if (nullptr == _pTargetCollider)
		return false;

	// 원 충돌 검사
	_vector v3Pos = XMLoadFloat3(&m_pCollCollider->Get_Translate()) - XMLoadFloat3(&_pTargetCollider->Get_Translate());
	_vector v3Size_Target = XMLoadFloat3(&_pTargetCollider->Get_Translate()) * tSightDesc.fFindDistance;
	_vector v3Size_Owner = XMLoadFloat3(&m_pCollCollider->Get_Translate());

	_float _fDistance = XMVectorGetX(XMVector3LengthSq(v3Pos));
	_float _fLen = XMVectorGetX(XMVector3LengthSq(v3Size_Target)) + XMVectorGetX(XMVector3LengthSq(v3Size_Owner));

	if (_fDistance > _fLen)
		return false;

	// Look Dir 
	float fAngle = PlayerToDot_DirToSelfLookConvert();
	if (tSightDesc.fFindAngle / 2.f >= fAngle)
		return true;
	else if (tSightDesc.fFindAngle >= fAngle)
	{

	}

	return false;
}

void CMonster::Find_Player(CGameInstance* _pGameInstnace)
{
	if (nullptr != m_pTargetPlayer)
	{
		m_pTargetPlayer->Add_ArroundMonster(this);
	}
	else

	{
		m_pTargetPlayer = static_cast<CPlayer*>(_pGameInstnace->Find_GameObject(g_iCurrentLevel, LAYER_PLAYER, 0));
		Safe_AddRef(m_pTargetPlayer);
	}
}

_bool CMonster::IsShootRayToLook()
{
	if (nullptr == m_pTargetPlayer)
		return false;

	CCollider* pPlayerCheckCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
	if (nullptr == pPlayerCheckCollider)
		return false;

	return false;
}

void CMonster::Setting_CurrentLookPos()
{
	if (nullptr == m_pTargetPlayer)
		return;

	m_pTransformCom->LookAt(m_pTargetPlayer->Get_OffsetPos());
	m_pTransformCom->Set_Look(-m_pTransformCom->Get_Look());
	m_pTransformCom->Set_Right(-m_pTransformCom->Get_Right());
}

void CMonster::Add_AroundPossibleAttackPlayer()
{
	CWeapon* pWeapon = static_cast<CWeapon*>(m_pEquipmentArr[CEquipment::EQUIP_WEAPON]);
	if (nullptr != pWeapon)
	{
		pWeapon->Add_CheckColliderList(m_pTargetPlayer);
	}
	if (nullptr != m_pCollCollider)
	{
		CCollider* pCollider = static_cast<CCollider*>(m_pTargetPlayer->Get_Component(COM_COLLCOLLIDER));
		if (nullptr == pCollider)
			return;
		if (true == m_pCollCollider->Is_Collision(pCollider))
		{
			m_pTargetPlayer->Damaged(this);
		}
	}
}

void CMonster::Restrict_AnimationToMoveOffset(CMonsterState* _pState)
{
	if (nullptr == _pState || nullptr == m_pNavigation)
		return;

	if (CMonsterState::STATE_ATTACK == _pState->Get_State()
		|| CMonsterState::STATE_DODGE == _pState->Get_State())
	{

	}
	else
	{
		m_bRestrictAttackPos = false;
	}
}

void CMonster::Coll_Tick(_matrix matWorld)
{
	__super::Coll_Tick(matWorld);
	if (nullptr != m_pAttackNowRangeCollider)
		m_pAttackNowRangeCollider->Tick(matWorld);
}
#ifdef _DEBUG
_bool CMonster::Add_Collider_Render()
{
	if (true == __super::Add_Collider_Render())
	{
		if (nullptr != m_pAttackNowRangeCollider)
			m_pRendererCom->Add_Components(m_pAttackNowRangeCollider);
	}

	return false;
}
#endif
void CMonster::Bind_Attack_Postion()
{
	if (false == m_bChangeAttackMatrix)
		return;

	m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
	m_bChangeAttackMatrix = false;
	m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
}

void CMonster::Bind_Supplement_Position()
{
	if (false == m_bChangeSuppleCount)
		return;

	if (false == m_pModelCom->Get_CurAnimation()->Is_SupplementSituation())
	{
		m_pTransformCom->Set_Pos(XMLoadFloat3((_float3*)&m_OffsetMatrix.m[3][0]));
		m_bChangeSuppleCount = false;
		m_OffsetMatrix = m_pTransformCom->Get_WorldFloat4x4();
	}
}

#pragma region USE_TOOL_METHOD
_bool CMonster::Extern_EditValue()
{
	return _bool();
}

_bool CMonster::Extern_HierakeyView(const string& _str)
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
		bActive = ImGui::Button("Delete");
		ImGui::PopItemWidth();
		ImGui::TreePop();
		return bActive;
	}
	return false;
}

HRESULT CMonster::Create_Self(CGameObject** _ppOut)
{
	return E_NOTIMPL;
}

HRESULT CMonster::Delete_Self()
{
	return __super::Delete_Self();
}

HRESULT CMonster::Save_Data()
{
	return E_NOTIMPL;
}

HRESULT CMonster::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
	return E_NOTIMPL;
}

_bool CMonster::Is_Picking(_float3* _pOut)
{
	return __super::Is_Picking(_pOut);
}

void CMonster::Reset_Data()
{
	__super::Reset_Data();
}

void CMonster::Control_ColliderData()
{
	__super::Control_ColliderData();
	if (ImGui::TreeNodeEx("AttackKnownCollider"))
	{
		if (nullptr != m_pAttackNowRangeCollider)
		{
			COLLDATA tCollData;
			tCollData.v3CollScale = m_pAttackNowRangeCollider->Get_Scale();
			tCollData.v3Translation = m_pAttackNowRangeCollider->Get_Translate();

			_float Translate[3] = { tCollData.v3Translation.x,  tCollData.v3Translation.y, tCollData.v3Translation.z };
			_float CollSclae[3] = { tCollData.v3CollScale.x ,  tCollData.v3CollScale.y , tCollData.v3CollScale.z };

			ImGui::InputFloat3("AtkKnownT", Translate, "%.1f");
			ImGui::InputFloat3("AtkKnownS", CollSclae, "%.1f");

			m_pAttackNowRangeCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
			m_pAttackNowRangeCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
		}
		ImGui::TreePop();
	}
}

void CMonster::Extern_ModifySightDesc()
{
	if (ImGui::CollapsingHeader("ModfySightDesc"))
	{
		SIGHTDESC tSightDesc = m_tMonsterDesc.tSightDesc;

		ImGui::DragFloat("FindSightAngle", &tSightDesc.fFindAngle, 1.f, 0, 180.f, "%.2f", ImGuiSliderFlags_Vertical);

		m_tMonsterDesc.tSightDesc = tSightDesc;
	}
}

#pragma endregion USE_TOOL_METHOD

CGameObject* CMonster::Clone(void* pArg)
{
	return nullptr;
}

void CMonster::Free()
{
	Safe_Release(m_pMonsterEPController);
	Safe_Release(m_pAttackNowRangeCollider);
	Safe_Release(m_pTargetPlayer);
	Safe_Release(m_pDisorveTexture);
	__super::Free();
}
