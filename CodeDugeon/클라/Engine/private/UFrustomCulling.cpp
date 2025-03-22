#include "EngineDefine.h"
#include "UFrustomCulling.h"
#include "UGameInstance.h"
#include "UCamera.h"
#include "UTransform.h"

UFrustomCulling::UFrustomCulling() :
	m_spOwnerCamera{ nullptr }
{
}

void UFrustomCulling::Free()
{
}

HRESULT UFrustomCulling::NativeConstruct(CSHPTRREF<UCamera> _spCamera)
{
	RETURN_CHECK(nullptr == _spCamera, E_FAIL);

	m_arrLocalPosArr[0] = _float3(-1.f, 1.f, 0.f);
	m_arrLocalPosArr[1] = _float3(1.f, 1.f, 0.f);
	m_arrLocalPosArr[2] = _float3(1.f, -1.f, 0.f);
	m_arrLocalPosArr[3] = _float3(-1.f, -1.f, 0.f);
	m_arrLocalPosArr[4] = _float3(-1.f, 1.f, 1.f);
	m_arrLocalPosArr[5] = _float3(1.f, 1.f, 1.f);
	m_arrLocalPosArr[6] = _float3(1.f, -1.f, 1.f);
	m_arrLocalPosArr[7] = _float3(-1.f, -1.f, 1.f);

	m_spOwnerCamera = _spCamera;
	return S_OK;
}

void UFrustomCulling::Tick()
{
	const _float4x4& vViewMatrix = m_spOwnerCamera->GetTransform()->GetWorldMatrixInv();
	const _float4x4& vProjMatrix = m_spOwnerCamera->GetProjMatrix();
	_float4x4 CombineMatrix = vViewMatrix * vProjMatrix;
	for (_uint i = 0; i < MAX_FRUSTOMSIZE; ++i) {
		m_arrChangePosArr[i] = _float3::TransformCoord(m_arrLocalPosArr[i], CombineMatrix);
	}

	// 위
	XMStoreFloat4(&m_arrPlaneArr[0], XMPlaneFromPoints(m_arrChangePosArr[3], m_arrChangePosArr[2], m_arrChangePosArr[1]));
	// 아래
	XMStoreFloat4(&m_arrPlaneArr[1], XMPlaneFromPoints(m_arrChangePosArr[7], m_arrChangePosArr[5], m_arrChangePosArr[6]));
	// 오른쪽
	XMStoreFloat4(&m_arrPlaneArr[2], XMPlaneFromPoints(m_arrChangePosArr[4], m_arrChangePosArr[1], m_arrChangePosArr[5]));
	// 왼쪽
	XMStoreFloat4(&m_arrPlaneArr[3], XMPlaneFromPoints(m_arrChangePosArr[6], m_arrChangePosArr[1], m_arrChangePosArr[2]));
	// 앞쪽
	XMStoreFloat4(&m_arrPlaneArr[4], XMPlaneFromPoints(m_arrChangePosArr[6], m_arrChangePosArr[2], m_arrChangePosArr[7]));
	// 오른쪽
	XMStoreFloat4(&m_arrPlaneArr[5], XMPlaneFromPoints(m_arrChangePosArr[3], m_arrChangePosArr[0], m_arrChangePosArr[7]));
}

_bool UFrustomCulling::IsContains(const _float3& _vPos, const _float _fRadius)
{
	for (auto& iter : m_arrPlaneArr)
	{
		_float fDist = DirectX::XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&iter), _vPos));
		if (fabs(fDist) <= _fRadius)
			return false;
	}

	return true;
}