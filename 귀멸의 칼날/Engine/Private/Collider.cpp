#include "Collider.h"
#include "PipeLine.h"
#include "DebugDraw.h"

#include "Drawing.h"

CCollider::CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eType)
	: CComponent(_pDevice, _pContext), m_eType(_eType)
{

}

CCollider::CCollider(const CCollider& _rhs)
	: CComponent(_rhs)
	, m_eType(_rhs.m_eType)
#ifdef _DEBUG
	, m_pInputLayout(_rhs.m_pInputLayout)
	, m_pEffect(_rhs.m_pEffect)
	, m_pBatch(_rhs.m_pBatch)
	, m_v4Color(_float4(1.f, 0.f, 0.f, 1.f))
#endif // _DEBUG
{
	if (nullptr != _rhs.m_pAABB_Original)
		m_pAABB_Original = new BoundingBox(*_rhs.m_pAABB_Original);
	if (nullptr != _rhs.m_pOBB_Original)
		m_pOBB_Original = new BoundingOrientedBox(*_rhs.m_pOBB_Original);
	if (nullptr != _rhs.m_pSphere_Original)
		m_pSphere_Original = new BoundingSphere(*_rhs.m_pSphere_Original);

	#ifdef _DEBUG
	Safe_AddRef_DV(m_pInputLayout);
#endif // _DEBUG
}

_float3 CCollider::Get_CurPosV3()
{
	_float3 v3Pos = _float3(0.f, 0.f, 0.f);
	switch (m_eType)
	{
	case TYPE_AABB:
		if (nullptr != m_pAABB)
			v3Pos = m_pAABB->Center;
		break;
	case TYPE_OBB:
		if (nullptr != m_pOBB)
			v3Pos = m_pOBB->Center;
		break;
	case TYPE_SPHERE:
		if (nullptr != m_pSphere)
			v3Pos = m_pSphere->Center;
		break;
	}
	return v3Pos;
}

_vector CCollider::Get_CurPos()
{
	_float3 v3Pos = _float3(0.f, 0.f, 0.f);
	switch (m_eType)
	{
	case TYPE_AABB:
		if (nullptr != m_pAABB)
			v3Pos = m_pAABB->Center;
		break;
	case TYPE_OBB:
		if (nullptr != m_pOBB)
			v3Pos = m_pOBB->Center;
		break;
	case TYPE_SPHERE:
		if (nullptr != m_pSphere)
			v3Pos = m_pSphere->Center;
		break;
	}
	return XMLoadFloat3(&v3Pos);
}

_float3 CCollider::Get_CurScaleV3()
{
	_float3 v3Scale = _float3(0.f, 0.f, 0.f);
	switch (m_eType)
	{
	case TYPE_AABB:
		if (nullptr != m_pAABB)
			v3Scale = m_pAABB->Extents;
		break;
	case TYPE_OBB:
		if (nullptr != m_pOBB)
			v3Scale = m_pOBB->Extents;
		break;
	case TYPE_SPHERE:
		if (nullptr != m_pSphere)
			v3Scale.x = m_pSphere->Radius;
		break;
	}
	return v3Scale;
}

_vector CCollider::Get_CurScale()
{
	_float3 v3Scale = _float3(0.f, 0.f, 0.f);
	switch (m_eType)
	{
	case TYPE_AABB:
		if (nullptr != m_pAABB)
			v3Scale = m_pAABB->Extents;
		break;
	case TYPE_OBB:
		if (nullptr != m_pOBB)
			v3Scale = m_pOBB->Extents;
		break;
	case TYPE_SPHERE:
		if (nullptr != m_pSphere)
			v3Scale.x = m_pSphere->Radius;
		break;
	}
	return XMLoadFloat3(&v3Scale);
}

_float3 CCollider::Get_Scale()
{
	_float3 v3Scale = _float3(0.f, 0.f, 0.f);
	switch (m_eType)
	{
	case TYPE_AABB:
		v3Scale = m_pAABB_Original->Extents;
		break;
	case TYPE_OBB:
		v3Scale = m_pOBB_Original->Extents;
		break;
	case TYPE_SPHERE:
		v3Scale = _float3(m_pSphere_Original->Radius, 0.f, 0.f);
		break;
	}
	return v3Scale;
}

_float3 CCollider::Get_Translate()
{
	_float3 v3Translate = _float3(0.f, 0.f, 0.f);
	switch (m_eType)
	{
	case TYPE_AABB:
		v3Translate = m_pAABB_Original->Center;
		break;
	case TYPE_OBB:
		v3Translate = m_pOBB_Original->Center;
		break;
	case TYPE_SPHERE:
		v3Translate = m_pSphere_Original->Center;
		break;
	}
	return v3Translate;
}

void CCollider::Set_Scale(const _float3& _v3Scale)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB_Original->Extents = _float3(_v3Scale.x, _v3Scale.y, _v3Scale.z);
		m_pAABB->Extents = _float3(_v3Scale.x, _v3Scale.y, _v3Scale.z);
		break;
	case TYPE_OBB:
		m_pOBB_Original->Extents = _float3(_v3Scale.x, _v3Scale.y, _v3Scale.z);
		m_pOBB->Extents = _float3(_v3Scale.x, _v3Scale.y, _v3Scale.z);
		break;
	case TYPE_SPHERE:
		m_pSphere_Original->Radius = _v3Scale.x;
		m_pSphere->Radius = _v3Scale.x;
		break;
	}
}

void CCollider::Set_Translate(const _float3& _v3Translate)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB_Original->Center = _v3Translate;
		m_pAABB->Center = _v3Translate;
		break;
	case TYPE_OBB:
		m_pOBB_Original->Center = _v3Translate;
		m_pOBB->Center = _v3Translate;
		break;
	case TYPE_SPHERE:
		m_pSphere_Original->Center = _v3Translate;
		m_pSphere->Center = _v3Translate;
		break;
	}
}

HRESULT CCollider::NativeConstruct_Prototype()
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		break;
	case TYPE_OBB:
		m_pOBB_Original = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		break;
	case TYPE_SPHERE:
		m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		break;
	}


#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = nullptr;
	size_t	ShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &ShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, ShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

#endif // _DEBUG

	return S_OK;
}

HRESULT CCollider::NativeConstruct_Prototype_NotClone(void* _pArg)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		break;
	case TYPE_OBB:
		m_pOBB_Original = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		break;
	case TYPE_SPHERE:
		m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		break;
	}

	if (nullptr != _pArg)
	{
		COLLIDERDESC		ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

		ColliderDesc = *(COLLIDERDESC*)_pArg;

		switch (m_eType)
		{
		case TYPE_AABB:
			m_pAABB_Original->Center = ColliderDesc.vTranslation;
			m_pAABB_Original->Extents = _float3(ColliderDesc.vScale.x, ColliderDesc.vScale.y, ColliderDesc.vScale.z);

			m_pAABB = new BoundingBox(*m_pAABB_Original);
			break;
		case TYPE_OBB:
			m_pOBB_Original->Center = ColliderDesc.vTranslation;
			m_pOBB_Original->Extents = _float3(ColliderDesc.vScale.x, ColliderDesc.vScale.y, ColliderDesc.vScale.z);

			m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);
			break;
		case TYPE_SPHERE:
			m_pSphere_Original->Center = ColliderDesc.vTranslation;
			m_pSphere_Original->Radius = ColliderDesc.vScale.x;

			m_pSphere = new BoundingSphere(*m_pSphere_Original);
			break;
		}
	}

	return S_OK;
}

HRESULT CCollider::NativeConstruct_Clone(void* _pArg)
{
	if (nullptr != _pArg)
	{
		COLLIDERDESC		ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof(COLLIDERDESC));

		ColliderDesc = *(COLLIDERDESC*)_pArg;

		switch (m_eType)
		{
		case TYPE_AABB:
			m_pAABB_Original->Center = ColliderDesc.vTranslation;
			m_pAABB_Original->Extents = _float3(ColliderDesc.vScale.x, ColliderDesc.vScale.y, ColliderDesc.vScale.z);

			m_pAABB = new BoundingBox(*m_pAABB_Original);
			break;
		case TYPE_OBB:
			m_pOBB_Original->Center = ColliderDesc.vTranslation;
			m_pOBB_Original->Extents = _float3(ColliderDesc.vScale.x, ColliderDesc.vScale.y, ColliderDesc.vScale.z);

			m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);
			break;
		case TYPE_SPHERE:
			m_pSphere_Original->Center = ColliderDesc.vTranslation;
			m_pSphere_Original->Radius = ColliderDesc.vScale.x;

			m_pSphere = new BoundingSphere(*m_pSphere_Original);
			break;
		}
	}

	return S_OK;
}

void CCollider::Tick(_fmatrix TransformMatrix)
{
	switch (m_eType)
	{
	case TYPE_AABB:
		m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
		XMStoreFloat4x4(&m_TransformMatrix, Remove_Rotation(TransformMatrix));
		break;
	case TYPE_OBB:
		m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
		break;
	case TYPE_SPHERE:
		m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);
		XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix);
		break;
	}
}


#ifdef _DEBUG
HRESULT CCollider::Render_Debug()
{
	_vector		vColor = m_bCollision == true ? XMVectorSet(0.f, 1.f, 1.f, 1.f) : XMLoadFloat4(&m_v4Color);

	/*switch (m_eType)
	{
	case TYPE_AABB:
		if (FAILED(CDrawing::Get_Instance()->Draw(*m_pAABB, CDrawing::VIEW_3D, vColor)))
			return E_FAIL;
		break;
	case TYPE_OBB:
		if (FAILED(CDrawing::Get_Instance()->Draw(*m_pOBB, CDrawing::VIEW_3D, vColor)))
			return E_FAIL;
		break;
	case TYPE_SPHERE:
		if (FAILED(CDrawing::Get_Instance()->Draw(*m_pSphere, CDrawing::VIEW_3D, vColor)))
			return E_FAIL;
		break;
	}*/
	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	{
		m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	}
	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pBatch->Begin();

	switch (m_eType)
	{
	case TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB, vColor);
		break;
	case TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB, vColor);
		break;
	case TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere, vColor);
		break;
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

_bool CCollider::Is_Collision(CCollider* pCollider)
{

	switch (m_eType)
	{
	case TYPE_AABB:
	{
		TYPE		eTargetType = pCollider->m_eType;

		if (TYPE_AABB == eTargetType)
			m_bCollision = m_pAABB->Intersects(*pCollider->m_pAABB);
		if (TYPE_OBB == eTargetType)
			m_bCollision = m_pAABB->Intersects(*pCollider->m_pOBB);
		if (TYPE_SPHERE == eTargetType)
			m_bCollision = m_pAABB->Intersects(*pCollider->m_pSphere);
	}
	break;
	case TYPE_OBB:
	{
		TYPE		eTargetType = pCollider->m_eType;

		if (TYPE_AABB == eTargetType)
			m_bCollision = m_pOBB->Intersects(*pCollider->m_pAABB);
		if (TYPE_OBB == eTargetType)
			m_bCollision = m_pOBB->Intersects(*pCollider->m_pOBB);
		if (TYPE_SPHERE == eTargetType)
			m_bCollision = m_pOBB->Intersects(*pCollider->m_pSphere);
	}
	break;
	case TYPE_SPHERE:
	{
		TYPE		eTargetType = pCollider->m_eType;

		if (TYPE_AABB == eTargetType)
			m_bCollision = m_pSphere->Intersects(*pCollider->m_pAABB);
		if (TYPE_OBB == eTargetType)
			m_bCollision = m_pSphere->Intersects(*pCollider->m_pOBB);
		if (TYPE_SPHERE == eTargetType)
			m_bCollision = m_pSphere->Intersects(*pCollider->m_pSphere);
	}
	break;

	}
	pCollider->m_bCollision = m_bCollision;

	return m_bCollision;
}

_bool CCollider::Is_Collision_AABB(CCollider* pCollider)
{
	_float3		vSourMin = Compute_MinPoint();
	_float3		vDestMin = pCollider->Compute_MinPoint();

	_float3		vSourMax = Compute_MaxPoint();
	_float3		vDestMax = pCollider->Compute_MaxPoint();


	m_bCollision = false;

	/* 너비가 겹치지 않았는지? */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
		return false;

	/* 높이가 겹치지 않았는지? */
	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
		return false;

	/* 깊이가 겹치지 않았는지? */
	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
		return false;

	m_bCollision = true;

	return true;
}

_bool CCollider::Is_Collision_OBB(CCollider* pCollider)
{
	OBBDESC			OBBDesc[2];

	OBBDesc[0] = Compute_OBBDesc();
	OBBDesc[1] = pCollider->Compute_OBBDesc();

	m_bCollision = false;
	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			_float		fDistance[3] = { 0.0f };

			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			if (fDistance[0] > fDistance[1] + fDistance[2])
				return false;
		}
	}
	m_bCollision = true;
	return true;
}

_bool CCollider::Is_Collision_SPHERE(CCollider* pCollider)
{
	return _bool();
}

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		Result = TransformMatrix;

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	Result.r[0] = vRight;
	Result.r[1] = vUp;
	Result.r[2] = vLook;

	return Result;
}

_float3 CCollider::Compute_MinPoint()
{
	if (nullptr == m_pAABB)
		return _float3(0.f, 0.f, 0.f);

	return _float3(m_pAABB->Center.x - m_pAABB->Extents.x,
		m_pAABB->Center.y - m_pAABB->Extents.y,
		m_pAABB->Center.z - m_pAABB->Extents.z);
}

_float3 CCollider::Compute_MaxPoint()
{
	if (nullptr == m_pAABB)
		return _float3(0.f, 0.f, 0.f);

	return _float3(m_pAABB->Center.x + m_pAABB->Extents.x,
		m_pAABB->Center.y + m_pAABB->Extents.y,
		m_pAABB->Center.z + m_pAABB->Extents.z);
}

CCollider::OBBDESC CCollider::Compute_OBBDesc()
{
	if (nullptr == m_pOBB)
		return OBBDESC();

	OBBDESC			OBBDesc;
	ZeroMemory(&OBBDesc, sizeof(OBBDESC));

	_float3		vPoints[8];

	m_pOBB->GetCorners(vPoints);

	OBBDesc.vCenter = m_pOBB->Center;

	XMStoreFloat3(&OBBDesc.vCenterAxis[0],
		(XMLoadFloat3(&vPoints[5]) + XMLoadFloat3(&vPoints[2])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));

	XMStoreFloat3(&OBBDesc.vCenterAxis[1],
		(XMLoadFloat3(&vPoints[5]) + XMLoadFloat3(&vPoints[7])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));

	XMStoreFloat3(&OBBDesc.vCenterAxis[2],
		(XMLoadFloat3(&vPoints[5]) + XMLoadFloat3(&vPoints[0])) * 0.5f -
		XMLoadFloat3(&OBBDesc.vCenter));


	XMStoreFloat3(&OBBDesc.vAlignAxis[0],
		XMVector3Normalize(XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[4])));

	XMStoreFloat3(&OBBDesc.vAlignAxis[1],
		XMVector3Normalize(XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[1])));

	XMStoreFloat3(&OBBDesc.vAlignAxis[2],
		XMVector3Normalize(XMLoadFloat3(&vPoints[5]) - XMLoadFloat3(&vPoints[6])));


	return OBBDesc;
}

CCollider* CCollider::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eType)
{
	CCollider* pInstance = new CCollider(_pDevice, _pContext, _eType);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCollider* CCollider::Create_NotClone(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eType, void* _pArg)
{
	CCollider* pInstance = new CCollider(_pDevice, _pContext, _eType);

	if (FAILED(pInstance->NativeConstruct_Prototype_NotClone(_pArg)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CCollider::Clone(void* _pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();
	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pSphere_Original);

	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere);



#ifdef _DEBUG
	Safe_Release_DV(m_pInputLayout);

	if (false == m_bCloned)
	{
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);
	}
#endif // _DEBUG
}
