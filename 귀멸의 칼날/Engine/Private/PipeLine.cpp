#include "PipeLine.h"
#include "Method.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine() 
{
	XMStoreFloat4x4(&m_StateMatrix[D3DTS_VIEW], XMMatrixIdentity());
	XMStoreFloat4x4(&m_StateMatrix[D3DTS_PROJ], XMMatrixIdentity());
	XMStoreFloat3(&m_vCamPosition, XMVectorZero());

	XMStoreFloat4x4(&m_LightMatrix[D3DTS_VIEW], XMMatrixIdentity());
	XMStoreFloat4x4(&m_LightMatrix[D3DTS_PROJ], XMMatrixIdentity());
	XMStoreFloat3(&m_vLightPosition, XMVectorZero());
}

_matrix CPipeLine::Get_TransformMatrixInv(const TRANSFORMSTATE& _eTransState)
{
	return CMethod::XMMatrixInverse_P(m_StateMatrix[_eTransState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4_TP(const TRANSFORMSTATE& _eTransState) const
{
	_float4x4		TransformMatrix;
	XMStoreFloat4x4(&TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_StateMatrix[_eTransState])));
	return TransformMatrix;
}

_float4x4 CPipeLine::Get_TransformFloat4x4Inv_TP(const TRANSFORMSTATE& _eTransState) const
{
	_float4x4		TransformMatrix;
	XMStoreFloat4x4(&TransformMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_StateMatrix[_eTransState]))));
	return TransformMatrix;

	//XMStoreFloat4x4(&TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_StateMatrix[_eTransState])));
	//return CMethod::XMMatrixInverse4x4_P(TransformMatrix);
}

void CPipeLine::Set_Transform(const TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix)
{
	XMStoreFloat4x4(&m_StateMatrix[_eState], _StateMatrix);

	if (D3DTS_VIEW == _eState)
	{
		_matrix		CamWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_StateMatrix[D3DTS_VIEW]));
		XMStoreFloat3(&m_vCamPosition, CamWorldMatrix.r[3]);
	}
}

_vector CPipeLine::Get_WorldToScreen(ID3D11DeviceContext* pContext, _vector vWorldPos)
{
	D3D11_VIEWPORT			ViewPort;

	_uint		iNumViewports = 1;
	pContext->RSGetViewports(&iNumViewports, &ViewPort);

	_vector vProjPos = XMVector3TransformCoord(vWorldPos, XMLoadFloat4x4(&m_StateMatrix[D3DTS_VIEW]));
	_float fViewZ = XMVectorGetZ(vProjPos);
	vProjPos = XMVector3TransformCoord(vProjPos, XMLoadFloat4x4(&m_StateMatrix[D3DTS_PROJ]));
	
	_float3 vScreenPos;
	XMStoreFloat3(&vScreenPos, vProjPos);
	vScreenPos.x = vScreenPos.x * 0.5f + 0.5f;
	vScreenPos.y = vScreenPos.y * -0.5f + 0.5f;
	// z값에는 카메라로부터 거리 넣어준다.
	vScreenPos.z = fViewZ;

	vScreenPos.x *= ViewPort.Width;
	vScreenPos.y *= ViewPort.Height;

	return XMLoadFloat3(&vScreenPos);
}


// vWorldPos로 깊이 판단
_vector CPipeLine::Get_ScreenToWorld(ID3D11DeviceContext* pContext, _vector vScreenPos, _vector vDepthWorldPos)
{
	D3D11_VIEWPORT			ViewPort;

	_uint		iNumViewports = 1;
	pContext->RSGetViewports(&iNumViewports, &ViewPort);

	_vector vDepthPos = XMVector3TransformCoord(vDepthWorldPos, XMLoadFloat4x4(&m_StateMatrix[D3DTS_VIEW]));
	vDepthPos = XMVector3TransformCoord(vDepthPos, XMLoadFloat4x4(&m_StateMatrix[D3DTS_PROJ]));

	_float3 vProjPos;
	XMStoreFloat3(&vProjPos, vScreenPos);
	vProjPos.x /= ViewPort.Width;
	vProjPos.y /= ViewPort.Height;

	vProjPos.x = vProjPos.x * 2.f - 1.f;
	vProjPos.y = vProjPos.y * -2.f + 1.f;
	vProjPos.z = XMVectorGetZ(vDepthPos);

	_matrix viewMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_StateMatrix[D3DTS_VIEW]));
	_matrix projMatrixInv = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_StateMatrix[D3DTS_PROJ]));

	_vector vWorldPos = XMLoadFloat3(&vProjPos);
	vWorldPos = XMVector3TransformCoord(vWorldPos, projMatrixInv);
	vWorldPos = XMVector3TransformCoord(vWorldPos, viewMatrixInv);

	return vWorldPos;
}


_float4x4 CPipeLine::Get_LightTransformFloat4x4_TP(const TRANSFORMSTATE& _eTransState) const
{
	_float4x4		TransformMatrix;
	XMStoreFloat4x4(&TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_LightMatrix[_eTransState])));
	return TransformMatrix;
}

void CPipeLine::Set_LightTransform(const TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix)
{
	XMStoreFloat4x4(&m_LightMatrix[_eState], _StateMatrix);

	if (D3DTS_VIEW == _eState)
	{
		_matrix		LightWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LightMatrix[D3DTS_VIEW]));
		XMStoreFloat3(&m_vLightPosition, LightWorldMatrix.r[3]);
	}
}

void CPipeLine::Set_LightViewMatrix(_vector vPosition, _vector vLookAt)
{
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_matrix viewMatrix = XMMatrixLookAtLH(vPosition, vLookAt, vUp);

	XMStoreFloat4x4(&m_LightMatrix[D3DTS_VIEW], viewMatrix);

	_matrix		lightWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_LightMatrix[D3DTS_VIEW]));
	XMStoreFloat3(&m_vLightPosition, lightWorldMatrix.r[3]);
}

void CPipeLine::Set_LightProjMatrix(const _float& fov, const _float& fAspect, const _float& fNear, const _float& fFar)
{
	_matrix projMatrix = XMMatrixPerspectiveFovLH(fov, fAspect, fNear, fFar);

	XMStoreFloat4x4(&m_LightMatrix[D3DTS_PROJ], projMatrix);

	m_fLightFar = fFar;
}

void CPipeLine::Free()
{
}
