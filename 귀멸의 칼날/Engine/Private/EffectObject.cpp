#include "EffectObject.h"
#include "GameInstance.h"
#include "Effect.h"
#include "HierarchyNode.h"

CEffectObject::CEffectObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CEffectObject::CEffectObject(const CEffectObject& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CEffectObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEffectObject::NativeConstruct_Clone(void* _pArg)
{
	if (FAILED(__super::Ready_Transform(TEXT("Com_Transform"))))
		return E_FAIL;

	ZeroMemory(&m_tEffectDesc, sizeof(EFFECTDESC));
	m_tEffectDesc.vScl = _float3(1.f, 1.f, 1.f);

	if (nullptr != _pArg)
	{
		m_pInitTransform = CTransform::Create(m_pDevice, m_pContext);
		memcpy(&m_tEffectDesc, (EFFECTDESC*)_pArg, sizeof(EFFECTDESC));
		m_pInitTransform->Set_Scale(m_tEffectDesc.vScl);
		m_pInitTransform->Rotate_Turn(m_tEffectDesc.vRot);
		m_pInitTransform->Set_Pos(XMLoadFloat3(&m_tEffectDesc.vPos));

	}

	ZeroMemory(&matBoneParent, sizeof(_float4x4));
	m_pEffectObjectTR = CTransform::Create(m_pDevice, m_pContext);
	m_pEffectObjectTR->Set_Scale(m_tEffectDesc.vScl);
	m_pEffectObjectTR->Rotate_Turn(m_tEffectDesc.vRot);
	m_pEffectObjectTR->Set_PosF3(m_tEffectDesc.vPos);

	m_tEffectDesc.bPlay = false;
	//m_bActive = false;
	if (PROJECT_TOOL == m_eCurProject)
		m_bActive = true;

	return S_OK;
}

void CEffectObject::Tick(const _double& _dTimeDelta)
{
	if (!m_bActive)
		return;

	if (m_tEffectDesc.bPlay)
	{
		if (m_tEffectDesc.bPause)
			m_fPauseVelau = 0.f;
		else
			m_fPauseVelau = 1.f;

		m_tEffectDesc.fCurFrame += (_float)_dTimeDelta * m_fPauseVelau;

		if (!m_tEffectDesc.bLoop)
		{
			if (m_tEffectDesc.fMaxFrame < m_tEffectDesc.fCurFrame)
			{
				m_tEffectDesc.bPlay = false;

				if (PROJECT_CLIENT == m_eCurProject || PROJECT_ANIMTOOL == m_eCurProject)
				{
					m_bActive = false;
					EffectObjectReset();
				}
				else
					EffectObjectReset();
			}
		}


		StateChange(_dTimeDelta);

		RotationAngle();
		RotationLoop();

		Billboard();

		EffectObjectUpdate();
	}
	else
	{
		if (PROJECT_TOOL == m_eCurProject)
			EffectObjectUpdate();

		m_pTransformCom->Rotate_Turn(m_tEffectDesc.vRot);
	}
}

void CEffectObject::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CEffectObject::Render()
{
	return S_OK;
}

void CEffectObject::Set_EffectObjectDesc(EFFECTDESC tEffectDesc)
{
	m_tEffectDesc = tEffectDesc;

	m_pTransformCom->Set_Scale(m_tEffectDesc.vScl);
	m_pTransformCom->Rotate_Turn(m_tEffectDesc.vRot);
	m_pTransformCom->Set_Pos(XMLoadFloat3(&m_tEffectDesc.vPos));
}

void CEffectObject::EffectObjectUpdate()
{
	if (0 >= m_vecEffect.size())
		return;

	for (auto& iter : m_vecEffect)
		static_cast<CEffect*>(iter)->SetUp_Position();
}

void CEffectObject::EffectObjectReset()
{
	m_pEffectObjectTR->Set_RightF3(_float3(1.f, 0.f, 0.f));
	m_pEffectObjectTR->Set_UpF3(_float3(0.f, 1.f, 0.f));
	m_pEffectObjectTR->Set_LookF3(_float3(0.f, 0.f, 1.f));

	m_pEffectObjectTR->Set_Scale(m_tEffectDesc.vScl);
	m_pEffectObjectTR->Rotate_Turn(m_tEffectDesc.vRot);
	m_pEffectObjectTR->Set_PosF3(m_tEffectDesc.vPos);
	m_tEffectDesc.vRotDelta = m_tEffectDesc.vRot;
	m_tEffectDesc.fCurFrame = 0.f;

	if (0 >= m_vecEffect.size())
		return;

	for (auto& iter : m_vecEffect)
		iter->EffectReset();
}

void CEffectObject::Billboard()
{
	if (m_tEffectDesc.bBillBoardExceptY || m_tEffectDesc.bBillBoardAll)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CGameObject* pGameObject = nullptr;

		if (PROJECT_TOOL == m_eCurProject || PROJECT_ANIMTOOL == m_eCurProject)
			pGameObject = pGameInstance->Find_GameObject(pGameInstance->Get_CurLevel(), TEXT("Layer_Cam"), 0);
		else
		{
			/*if (4 == pGameInstance->Get_CurLevel())
			{
				pGameObject = pGameInstance->Find_GameObject(4, TEXT("Layer_Cam"), 0);
			}
			else if (5 == pGameInstance->Get_CurLevel())
			{
				pGameObject = pGameInstance->Find_GameObject(5, TEXT("Layer_Cam"), 0);
			}*/
			pGameObject = pGameInstance->Find_GameObject(pGameInstance->Get_CurLevel(), TEXT("Layer_Cam"), 0);
		}

		if (nullptr != pGameObject)
		{
			if (m_tEffectDesc.bBillBoardExceptY)
			{
				_float3 vPos = pGameObject->Get_Transform()->Get_PosF3();
				vPos.y = m_pTransformCom->Get_PosF3().y;
				m_pTransformCom->LookAt(XMLoadFloat3(&vPos));
			}
			else if (m_tEffectDesc.bBillBoardAll)
			{
				m_pTransformCom->LookAt(pGameObject->Get_Transform()->Get_Pos());
			}
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CEffectObject::StateChange(const _double& _dTimeDelta)
{
	if (m_iMode == 1)
	{
		XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix());
		_matrix matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

		if (m_bReverse)
		{
			if (1 == m_fAngleValue)
				m_fAngleValue = 180.f;

			_matrix matRot = XMMatrixRotationY(XMConvertToRadians(m_fAngleValue));
			m_pEffectObjectTR->Bind_WorldMatrix(matRot * m_pEffectObjectTR->Get_WorldMatrix());
			m_bReverse = false;
		}

		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * matScale * XMLoadFloat4x4(&matBoneParent) * matScale * m_pOwnerTR->Get_WorldMatrix());

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (m_iMode == 2)
	{
		XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix());
		_matrix matParentPos = XMMatrixTranslation(m_pOwnerTR->Get_PosF3().x, m_pOwnerTR->Get_PosF3().y, m_pOwnerTR->Get_PosF3().z);
		_matrix matBonePos = XMMatrixTranslation(matBoneParent._41, matBoneParent._42, matBoneParent._43);
		_matrix matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

		_float3 vRight = _float3(0.f, 0.f, 0.f);
		_float3 vUp = _float3(0.f, 0.f, 0.f);
		_float3 vLook = m_vAnimDir;

		vRight = XMVector3Cross(XMLoadFloat3(&_float3(0.f, 1.f, 0.f)), XMLoadFloat3(&vLook));
		vUp = XMVector3Cross(XMLoadFloat3(&vLook), XMLoadFloat3(&vRight));

		m_pEffectObjectTR->Set_RightF3(vLook);
		m_pEffectObjectTR->Set_UpF3(vUp);
		m_pEffectObjectTR->Set_LookF3(vRight);

		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * matScale * matBonePos * matScale * matParentPos);

		m_vOwnerPos = m_pTransformCom->Get_PosF3();

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (3 == m_iMode)
	{
		XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix());
		_matrix matScale2 = XMMatrixScaling(0.1f, 0.1f, 0.1f);

		_matrix matParentPos = XMMatrixTranslation(m_pOwnerTR->Get_PosF3().x, m_pOwnerTR->Get_PosF3().y, m_pOwnerTR->Get_PosF3().z);
		_matrix matBonePos = XMMatrixTranslation(matBoneParent._41, matBoneParent._42, matBoneParent._43);

		_matrix MultipleWorld = XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			m_pHierarchyNode->Get_CombinedTransformationMatrix() * XMMatrixRotationY(XMConvertToRadians(180.0f)) * m_pEffectObjectTR->Get_WorldMatrix();

		m_pTransformCom->Bind_WorldMatrix(MultipleWorld * matScale2 * matParentPos);

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (4 == m_iMode)
	{
		_matrix World = CMethod::XMMatrixLoadFloat4x4_P(m_SetUpOwnerTransform);
		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * XMMatrixRotationY(XMConvertToRadians(180.f)) * World);

		m_vOwnerPos = m_pTransformCom->Get_PosF3();

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (5 == m_iMode)
	{
		_float3 vLook = m_vRootBoneLook.Normalize();
		_float3 vUp = _float3(0.f, 1.f, 0.f);
		_float3 vRight = XMVector3Cross(XMLoadFloat3(&vUp), XMLoadFloat3(&vLook));
		vUp = XMVector3Cross(XMLoadFloat3(&vLook), XMLoadFloat3(&vRight));

		_float3 vScale = m_pEffectObjectTR->Get_Scale();

		m_pEffectObjectTR->Set_RightF3(vRight);
		m_pEffectObjectTR->Set_UpF3(vUp);
		m_pEffectObjectTR->Set_LookF3(vLook);

		m_pEffectObjectTR->Set_Scale(vScale);

		_matrix matBonePos = XMMatrixTranslation(matBoneParent._41, matBoneParent._42, matBoneParent._43);
		_matrix matParentPos = XMMatrixTranslation(m_pOwnerTR->Get_PosF3().x, m_pOwnerTR->Get_PosF3().y, m_pOwnerTR->Get_PosF3().z);
		_matrix matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * matScale * matBonePos * matScale * matParentPos);

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (6 == m_iMode)
	{
		//XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix());
		//_matrix matScale = XMMatrixScaling(0.01f, 0.01f, 0.01f);

		///*m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * matScale * XMLoadFloat4x4(&matBoneParent)
		//	* matScale * m_pOwnerTR->Get_WorldMatrix());*/

		//m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix());

		//for (auto& iter : m_vecEffect)
		//	iter->SetUp_Position(false);
	}
	else if (m_iMode == 8)
	{
		_matrix matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		_matrix World = CMethod::XMMatrixLoadFloat4x4_P(m_SetUpOwnerTransform);
		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * matScale *
			static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix() * matScale * World);

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (m_iMode == 9)
	{
		std::random_device myRandom;
		_int iRandomX = (myRandom() % 14) - 7;
		_int iRandomY = myRandom() % 13;
		_int iRandomZ = (myRandom() % 6) + 25;

		_float3 vPos;
		vPos.x += (_float)iRandomX;
		vPos.y += (_float)iRandomY;
		vPos.z += (_float)iRandomZ;
		m_pEffectObjectTR->Set_PosF3(vPos);

		XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix());
		_matrix matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * m_pOwnerTR->Get_WorldMatrix());
	}
	else if (m_iMode == 10)
	{
		_matrix World = CMethod::XMMatrixLoadFloat4x4_P(m_SetUpOwnerTransform);
		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * World);

		m_vOwnerPos = m_pTransformCom->Get_PosF3();

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (m_iMode == 11)
	{
		XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix());
		_matrix matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * matScale * XMLoadFloat4x4(&matBoneParent) * matScale * XMMatrixRotationY(XMConvertToRadians(180.f)) * m_pOwnerTR->Get_WorldMatrix());

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (12 == m_iMode)
	{
		XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix());
		_matrix matScale2 = XMMatrixScaling(0.1f, 0.1f, 0.1f);

		_matrix World = CMethod::XMMatrixLoadFloat4x4_P(m_SetUpOwnerTransform);
		World = CMethod::XMMatrixSetFloat3(_float3(0.f, 0.f, 0.f), MATROW_POS, World);

		_matrix matParentPos = XMMatrixTranslation(m_pOwnerTR->Get_PosF3().x, m_pOwnerTR->Get_PosF3().y, m_pOwnerTR->Get_PosF3().z);
		_matrix matBonePos = XMMatrixTranslation(matBoneParent._41, matBoneParent._42, matBoneParent._43);

		_matrix MultipleWorld = XMMatrixScaling(0.1f, 0.1f, 0.1f) *
			m_pHierarchyNode->Get_CombinedTransformationMatrix() * XMMatrixRotationY(XMConvertToRadians(180.0f)) * m_pEffectObjectTR->Get_WorldMatrix();

		m_pTransformCom->Bind_WorldMatrix(World * MultipleWorld * matScale2 * matParentPos);

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	// 투사체 타겟 따라가기
	else if (m_iMode == 13)
	{
		//m_pTransformCom->LookAt(m_pTargetRT->Get_Pos());

		//_float3 vPos = m_pTransformCom->Get_PosF3();
		//vPos += m_vTargetDir * 100.f * (_float)_dTimeDelta;

		//_float3 vPos = m_pTargetRT->Get_PosF3();
		//m_pTransformCom->Set_PosF3(vPos);

		m_pTransformCom->Bind_WorldMatrix(m_pTargetRT->Get_WorldMatrix());

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();
	}
	else if (m_iMode == 14)
	{
		/*m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix());

		for (auto& iter : m_vecEffect)
			iter->SetUp_Position();*/
	}
	else
	{
		if (m_bReverse)
		{
			_matrix matRot = XMMatrixRotationY(XMConvertToRadians(180.f));
			m_pEffectObjectTR->Bind_WorldMatrix(matRot * m_pEffectObjectTR->Get_WorldMatrix());
			m_bReverse = false;
		}
	}
}

void CEffectObject::RotationLoop()
{

}

void CEffectObject::RotationAngle()
{
	if (0.f == m_tEffectDesc.fDestAngle)
	{
		//_float3 vRot = _float3(m_tEffectDesc.vRot.x, m_tEffectDesc.vRot.y, m_tEffectDesc.vRot.z);
		//m_pTransformCom->Rotate_Turn(m_tEffectDesc.vRot);
	}
	else
	{
		m_tEffectDesc.fDeltaAngleRatio = m_tEffectDesc.fCurFrame / m_tEffectDesc.fMaxFrame;
		if (1.f >= m_tEffectDesc.fDeltaAngleRatio)
		{
			fDeltaValue = (m_tEffectDesc.fDestAngle * m_tEffectDesc.fDeltaAngleRatio);

			_vector vAxis;

			if (EFFECTDESC::Axis_X == m_tEffectDesc.eRotAxisType)
			{
				vAxis = XMVectorSet(1.f, 0.f, 0.f, 0.f);
				m_pTransformCom->Rotate_Fix(vAxis, fDeltaValue);
				m_tEffectDesc.vRotDelta = _float3(m_tEffectDesc.vRot.x + fDeltaValue, m_tEffectDesc.vRot.y, m_tEffectDesc.vRot.z);
			}
			else if (EFFECTDESC::Axis_Y == m_tEffectDesc.eRotAxisType)
			{
				vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				m_pTransformCom->Rotate_Fix(vAxis, fDeltaValue);
				m_tEffectDesc.vRotDelta = _float3(m_tEffectDesc.vRot.x, m_tEffectDesc.vRot.y + fDeltaValue, m_tEffectDesc.vRot.z);
			}
			else if (EFFECTDESC::Axis_Z == m_tEffectDesc.eRotAxisType)
			{
				vAxis = XMVectorSet(0.f, 0.f, 1.f, 0.f);
				m_pTransformCom->Rotate_Fix(vAxis, fDeltaValue);
				m_tEffectDesc.vRotDelta = _float3(m_tEffectDesc.vRot.x, m_tEffectDesc.vRot.y, m_tEffectDesc.vRot.z + fDeltaValue);
			}
			else
			{
				_float3 vRot = _float3(m_tEffectDesc.vRot.x + fDeltaValue, m_tEffectDesc.vRot.y + fDeltaValue, m_tEffectDesc.vRot.z + fDeltaValue);
				m_pTransformCom->Rotate_Turn(vRot);
				m_tEffectDesc.vRotDelta = vRot;
			}
		}
	}
}

void CEffectObject::SetEffectState(_fmatrix _matWorld)
{
	_matrix WorldMatrix = (_matrix)_matWorld /** m_pEffectObjectTR->Get_WorldMatrix()*/;
	m_pTransformCom->Bind_WorldMatrix(WorldMatrix);
	m_tEffectDesc.bPlay = true;

	for (auto& iter : m_vecEffect)
		iter->EffectReset();
}

void CEffectObject::Set_TargetTransform(CTransform* pTarget)
{
	m_vTargetDir = pTarget->Get_PosF3() - m_pTransformCom->Get_PosF3();
	m_iMode = 13;
	m_pTargetRT = pTarget;

	_float3 vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_LookF3(m_pTargetRT->Get_LookF3());
	m_pTransformCom->Set_UpF3(m_pTargetRT->Get_UpF3());
	m_pTransformCom->Set_RightF3(m_pTargetRT->Get_RightF3());

	m_pTransformCom->Set_Scale(vScale);

	//m_pTransformCom->LookAt(m_pTargetRT->Get_Pos());
}

HRESULT CEffectObject::SetUP_Matrix(CHierarchyNode* pNode, CTransform* pOwnerTransform, _float fValue, _float4 vValue, _float3 vDir, _float3 vLook)
{
	if (nullptr == pNode)
		return E_FAIL;

	m_iMode = (_int)fValue;

	m_vAnimDir = vDir;
	m_vRootBoneLook = vLook;

	_matrix matBone = static_cast<CHierarchyNode*>(pNode)->Get_TransformMatrix();
	XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(pNode)->Get_CombinedTransformationMatrix());
	_matrix matOffset = static_cast<CHierarchyNode*>(pNode)->Get_OffsetMatrix();
	_matrix matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

	m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * matScale * XMLoadFloat4x4(&matBoneParent) * matScale * pOwnerTransform->Get_WorldMatrix());

	if (m_iMode == 1 || m_iMode == 2 || m_iMode == 3 || m_iMode == 4 || m_iMode == 5 || m_iMode == 6 || m_iMode == 7 || m_iMode == 8 || m_iMode == 9 || m_iMode == 10
		|| m_iMode == 11 || m_iMode == 12)
	{
		m_pHierarchyNode = pNode;
		m_pOwnerTR = pOwnerTransform;

		m_SetUpOwnerTransform = CMethod::XMMatrixStoreFloat4x4_P(m_pOwnerTR->Get_WorldMatrix());

		if (6 == m_iMode)
		{
			std::random_device myRandom;
			_int iRandomX = myRandom() % 180 - 90;
			_int iRandomY = myRandom() % 60;
			m_vRandomDir = XMVector3Normalize(XMLoadFloat3(&_float3((_float)iRandomX, (_float)iRandomY, 100.f)) - XMLoadFloat3(&_float3(0.f, 10.f, 0.f)));
			//m_vRandomDir = XMVector3Normalize(XMLoadFloat3(&_float3(cosf(XMConvertToRadians((_float)iRandomX)), sinf(XMConvertToRadians((_float)iRandomY)), 1.f)) - XMLoadFloat3(&_float3(0.f, 0.f, 0.f)));

			_float3 vLook = m_vRandomDir;
			_float3 vUp = _float3(0.f, 1.f, 0.f);
			_float3 vRight = XMVector3Cross(XMLoadFloat3(&vUp), XMLoadFloat3(&vLook));
			vUp = XMVector3Cross(XMLoadFloat3(&vLook), XMLoadFloat3(&vRight));

			_float3 vScale = m_pEffectObjectTR->Get_Scale();

			m_pEffectObjectTR->Set_RightF3(vRight);
			m_pEffectObjectTR->Set_UpF3(vUp);
			m_pEffectObjectTR->Set_LookF3(vLook);

			m_pEffectObjectTR->Set_Scale(vScale);

			for (auto& iter : m_vecEffect)
				iter->Set_ParentLook(m_vRandomDir);

			/*for (auto& iter : m_vecEffect)
				iter->Set_ChangeLook();*/
		}
		else if (7 == m_iMode)
		{
			std::random_device myRandom;
			_int iRandomX = (myRandom() % 14) - 7;
			_int iRandomY = myRandom() % 13;
			_int iRandomZ = (myRandom() % 6) + 25;

			_float3 vPos = m_pOwnerTR->Get_PosF3();
			vPos.x += (_float)iRandomX;
			vPos.y += (_float)iRandomY;
			vPos.z += (_float)iRandomZ;
			m_pEffectObjectTR->Set_PosF3(vPos);
		}

		XMStoreFloat4x4(&matBoneParent, static_cast<CHierarchyNode*>(m_pHierarchyNode)->Get_CombinedTransformationMatrix());
		_matrix matScale = XMMatrixScaling(0.1f, 0.1f, 0.1f);

		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix() * matScale * XMLoadFloat4x4(&matBoneParent) * matScale * m_pOwnerTR->Get_WorldMatrix());


		if (3 == m_iMode)
			m_vMode3Value = vValue;
	}
	else if (m_iMode == 4)
	{
		m_pHierarchyNode = pNode;
		m_pOwnerTR = pOwnerTransform;
		m_pTransformCom->Bind_WorldMatrix(m_pEffectObjectTR->Get_WorldMatrix());
	}

	if (0.f != vValue.x)
	{
		m_bReverse = true;
		m_fAngleValue = vValue.x;
	}

	for (auto& iter : m_vecEffect)
		iter->EffectReset();

	return S_OK;
}

HRESULT CEffectObject::Add_Effect(CEffect* pEffect)
{
	static_cast<CEffect*>(pEffect)->Set_Parent(this);
	m_vecEffect.push_back(pEffect);
	Safe_AddRef(pEffect);

	return S_OK;
}

CEffectObject* CEffectObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEffectObject* pInstance = new CEffectObject(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Create Failed : CEffectObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffectObject::Clone(void* _pArg)
{
	CEffectObject* pInstance = new CEffectObject(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("Cloned Failed : CEffectObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectObject::Free()
{
	__super::Free();

	for (auto& iter : m_vecEffect)
		Safe_Release(iter);

	Safe_Release(m_pEffectObjectTR);
	Safe_Release(m_pInitTransform);

	m_vecEffect.clear();
}
