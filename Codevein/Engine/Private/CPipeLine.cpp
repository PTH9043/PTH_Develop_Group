#include "CPipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

_float4x4 CPipeLine::Get_TransformFloat4x4_TP(const TRANSFORMSTATE& _eTransState)
{
	_float4x4		TransformMatrix;
	XMStoreFloat4x4(&TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_StateMatrix[_eTransState])));
	return TransformMatrix;
}

void CPipeLine::Tick()
{
	_matrix		CamWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_StateMatrix[D3DTS_VIEW]));
	XMStoreFloat3(&m_v3CamPosition, CamWorldMatrix.r[3]);
}

void CPipeLine::Free()
{
}
