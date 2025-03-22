#include "EngineDefine.h"
#include "UPicking.h"
#include "UTransform.h"
#include "UVIBuffer.h"
#include "UActor.h"
#include "UPawn.h"
#include "UGameInstance.h"
#include "UGrid.h"
#include "UCollider.h"


UPicking::UPicking() :
	m_vWindowSize{ 0.f, 0.f },
	m_stClientRect{ 0, 0, 0, 0 },
	m_vRayPos{ 0.f, 0.f, 0.f },
	m_vRayDir{ 0.f, 0.f, 0.f },
	m_lsPickingList{},
	m_stPickingDesc{},
	m_isMouseInScreen{ false }
{
}

void UPicking::Free()
{
}

HRESULT UPicking::ReadyPickingDesc(CSHPTRREF<GRAPHICDESC> _spGraphicDesc)
{
	RETURN_CHECK(nullptr == _spGraphicDesc, E_FAIL);

	GetClientRect(_spGraphicDesc->hWnd, &m_stClientRect);
	m_vWindowSize.x = static_cast<_float>(_spGraphicDesc->iWinCX);
	m_vWindowSize.y = static_cast<_float>(_spGraphicDesc->iWinCY);
	return S_OK;
}

void UPicking::CastRayInWorldSpace(UGameInstance* _pGameInstance)
{
	RETURN_CHECK(nullptr == _pGameInstance, ;);
	m_WaitCheckPawnList.clear();

	_float2 vMousePos{ _pGameInstance->GetMousePosition() };
	if ((m_stClientRect.left > vMousePos.x || vMousePos.x > m_stClientRect.right) &&
		(m_stClientRect.top > vMousePos.y || vMousePos.y > m_stClientRect.bottom))
	{
		m_isMouseInScreen = true;
		return;
	}
	m_isMouseInScreen = false;
	const _float4x4 mViewMatrixInv = _pGameInstance->GetMainCamViewMatrix().Invert();
	const _float4x4& mProjMatrix = _pGameInstance->GetMainCamProjMatrix();
	
	// Position 
	_float2		vPosition{};
	vPosition.x = (2.f * vMousePos.x / m_vWindowSize.x - 1.f) / mProjMatrix._11;
	vPosition.y = (-2.f * vMousePos.y / m_vWindowSize.y + 1.f) / mProjMatrix._22;

	m_vRayPos = { 0.f, 0.f, 0.f };
	m_vRayDir = { vPosition.x, vPosition.y, 1.f };

	m_vRayPos = _float3::TransformCoord(m_vRayPos, mViewMatrixInv);
	m_vRayDir = _float3::TransformNormal(m_vRayDir, mViewMatrixInv);
	m_vRayDir.Normalize();
}

void UPicking::AddPickingObject(CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer)
{
	WAITCHECKPAWN stPawn;
	stPawn.spPawn = _spPawn;
	stPawn.spVIBuffer = _spVIBuffer;
	m_WaitCheckPawnList.insert(stPawn);
}

void UPicking::DeletePickingObject(CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer)
{
	WAITCHECKPAWN stPawn;
	stPawn.spPawn = _spPawn;
	stPawn.spVIBuffer = _spVIBuffer;

	// 해당 객체를 찾아서 제거
	auto it = m_WaitCheckPawnList.find(stPawn);
	if (it != m_WaitCheckPawnList.end())
	{
		m_WaitCheckPawnList.erase(it);
	}
}

SHPTR<UPawn> UPicking::GetPickingPawn()
{
	m_stPickingDesc = GetPickDesc();
	return m_stPickingDesc.spPawn;
}

const PICKINGDESC UPicking::GetPickDesc()
{
	bool bFoundValidPick = false;
	for (auto& iter : m_WaitCheckPawnList)
	{
		_float3 v3Pos = _float3(0.f, 0.f, 0.f);
		_float ftoColliderDist = 0.f;
		if(iter.spPawn->GetColliderContainer().size() > 0)
		{
			if (true == PickingCollider(m_vRayPos, m_vRayDir, iter.spPawn, &ftoColliderDist))
			{
				bFoundValidPick = true;
				_float ftoMeshDist = 0.f;
				if (true == PickingMesh(m_vRayPos, m_vRayDir, iter.spPawn, iter.spVIBuffer, &ftoMeshDist, &v3Pos))
				{
					bFoundValidPick = true;
					AddPickingObject(PICKINGDESC(iter.spPawn, v3Pos, ftoMeshDist, true));
				}
			}
		}
		else
		{
			bFoundValidPick = true;
			_float ftoMeshDist = 0.f;
			if (true == PickingMesh(m_vRayPos, m_vRayDir, iter.spPawn, iter.spVIBuffer, &ftoMeshDist, &v3Pos))
			{
				bFoundValidPick = true;
				AddPickingObject(PICKINGDESC(iter.spPawn, v3Pos, ftoMeshDist, true));
			}
		}
		
	}

//#ifdef _USE_DEBUGGING
//	//만약 유의미한 픽킹이 진행이 안되었을 시
//	//그리드 위의 좌표를 반환 (y = 0의 좌표)
//	if (!bFoundValidPick)
//	{
//		_float3 v3Pos = _float3(0.f, 0.f, 0.f);
//		_float fDist = 0.f;
//		if (true == PickingOnGrid(m_spMainGrid.spGrid, &fDist, &v3Pos))
//		{
//			m_stPickingDesc = PICKINGDESC(m_spMainGrid.spGrid, v3Pos, fDist, true);
//			return m_stPickingDesc;
//		}
//		else
//			return PICKINGDESC{nullptr, v3Pos, fDist, false};
//	}
//#endif

	SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
	_float3 vCamPos = pGameInstance->GetMainCamPosition();
	{
		m_lsPickingList.sort([&](const PICKINGDESC& _t1, const PICKINGDESC& _t2)
			{
				_float fLen1 = _float3::LengthSquared(_t1.vPickPos - vCamPos);
				_float fLen2 = _float3::LengthSquared(_t2.vPickPos - vCamPos);
				return fLen1 < fLen2;
			});
		if (m_lsPickingList.size() > 0)
		{
			m_stPickingDesc = m_lsPickingList.front();
			m_lsPickingList.clear();
			return m_stPickingDesc;
		}
		m_stPickingDesc = PICKINGDESC{};
	}
	return m_stPickingDesc;
}

_bool UPicking::PickingMesh(const _float3& _RayPos, const _float3& _RayDir, CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer,
	_float* _pDist, _float3* _pOut)
{
	_float3 vLocalRayDir, vLocalRayPos;
	_float4x4 WorldInv = _spPawn->GetTransform()->GetWorldMatrixInv();

	vLocalRayPos = _float3::TransformCoord(_RayPos, WorldInv);
	vLocalRayDir = _float3::TransformNormal(_RayDir, WorldInv);
	vLocalRayDir.Normalize();

	_uint iNumFaces = _spVIBuffer->GetIndexCnt();
	const VECTOR<_float3>& pVerticesPos = *_spVIBuffer->GetVertexPos().get();

	const void* pIndices = _spVIBuffer->GetIndices();
	DXGI_FORMAT eFormat = _spVIBuffer->GetIndexFormat();

	_uint iSize = 0;
	_bool bHit = false;
	_float fMinDist = FLT_MAX;
	_float3 vClosestHit;

	if (DXGI_FORMAT_R16_UINT == eFormat)
	{
		iSize = sizeof(INDICIES16);
		_ushort iIndices = 0;
		for (_uint i = 0; i < iNumFaces; ++i)
		{
			_float3 v1 = pVerticesPos[iIndices++];
			_float3 v2 = pVerticesPos[iIndices++];
			_float3 v3 = pVerticesPos[iIndices++];

			_float fDist = 0.f;
			_float3 vHitPos = _float3(0.f, 0.f, 0.f);
			if (true == IsPickingCheck(vLocalRayPos, vLocalRayDir, v1, v2, v3, _spPawn->GetTransform()->GetWorldMatrix(),
				&fDist, &vHitPos))
			{
				bHit = true;
				if (fDist < fMinDist)
				{
					fMinDist = fDist;
					vClosestHit = vHitPos;
				}
			}
		}
	}
	else
	{
		iSize = sizeof(INDICIES32);
		_uint iIndices = 0;
		for (_uint i = 0; i < iNumFaces; ++i)
		{
			_float3 v1 = pVerticesPos[iIndices++];
			_float3 v2 = pVerticesPos[iIndices++];
			_float3 v3 = pVerticesPos[iIndices++];

			_float fDist = 0.f;
			_float3 vHitPos = _float3(0.f, 0.f, 0.f);
			if (true == IsPickingCheck(vLocalRayPos, vLocalRayDir, v1, v2, v3, _spPawn->GetTransform()->GetWorldMatrix(),
				&fDist, &vHitPos))
			{
				bHit = true;
				if (fDist < fMinDist)
				{
					fMinDist = fDist;
					vClosestHit = vHitPos;
				}
			}
		}
	}

	if (bHit)
	{
		*_pDist = fMinDist;
		*_pOut = vClosestHit;
		return true;
	}

	return false;
}

_bool UPicking::PickingOnGrid(CSHPTRREF<UGrid> _spGrid, _float* _pDist, _float3* _pOut)
{
	_float3 vLocalRayDir, vLocalRayPos;
	_float4x4 WorldInv = _spGrid->GetTransform()->GetWorldMatrixInv();

	vLocalRayPos = _float3::TransformCoord(m_vRayPos, WorldInv);
	vLocalRayDir = _float3::TransformNormal(m_vRayDir, WorldInv);
	vLocalRayDir.Normalize();

	_float fDist = 0;

	SHPTR<UCollider> spGridCollider = _spGrid->GetCollider();

	SHPTR<UGameInstance> pGameInstance = GET_INSTANCE(UGameInstance);
	_float3 vCamPos = pGameInstance->GetMainCamPosition();

	if (spGridCollider->IsCollisionWithRay(vLocalRayPos, vLocalRayDir, &fDist))
	{
		_float3 v3Pos = vLocalRayPos + vLocalRayDir * fDist;
		v3Pos = XMVector3TransformCoord(v3Pos, _spGrid->GetTransform()->GetWorldMatrix());
		if (nullptr != _pDist)
		*_pDist = fDist;
		if (nullptr != _pOut)
		*_pOut = v3Pos;
		return true;
	}
	return false;
}

_bool UPicking::PickingCollider(const _float3& _RayPos, const _float3& _RayDir, CSHPTRREF<UPawn> _spPawn, _float* _pDist)
{
	_float3 vLocalRayDir, vLocalRayPos;

	COLLIDERCONTAINER PawnColliderList = _spPawn->GetColliderContainer();
	SHPTR<UCollider> PawnCollider;

	_float closestDist = std::numeric_limits<_float>::infinity();
	_bool hasCollision = false;

	for (auto& iter : PawnColliderList)
	{
		PawnCollider = iter.second;

		_float4x4 WorldInv = _spPawn->GetTransform()->GetWorldMatrixInv();

		vLocalRayPos = _float3::TransformCoord(_RayPos, WorldInv);
		vLocalRayDir = _RayDir;
		vLocalRayDir.Normalize();

		_float dist;
		if (PawnCollider->IsCollisionWithRay(_RayPos, vLocalRayDir, &dist))
		{
			hasCollision = true;
			if (dist < closestDist)
			{
				closestDist = dist;
			}
		}
	}
	if (hasCollision)
	{
		*_pDist = closestDist;
		return true;
	}
	else
	{
		return false;
	}
}

_bool UPicking::IsPickingCheck(const _float3& _vLocalRay, const _float3& _vDirRay, const _float3& _vPos1,
	const _float3& _vPos2, const _float3& _vPos3, const _float4x4& _mWorldMatrix, _float* _pDist, _float3* _pOut)
{
	_float fDist;
	if (DirectX::TriangleTests::Intersects(_vLocalRay, _vDirRay, _vPos1, _vPos2, _vPos3, fDist))
	{
		_float3 v3Pos = _vLocalRay + _vDirRay * fDist;
		v3Pos = XMVector3TransformCoord(v3Pos, _mWorldMatrix);
		if (nullptr != _pDist)
			*_pDist = fDist;
		if (nullptr != _pOut)
			*_pOut = v3Pos;
		return true;
	}
	return false;
}

void UPicking::AddPickingObject(const PICKINGDESC& _stDesc)
{
	m_lsPickingList.push_back(_stDesc);
}

void UPicking::AddPickingGrid(const MAINGRID& _stGrid)
{
	m_spMainGrid = _stGrid;
}

