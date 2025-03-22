#include "RectTransform.h"
#include "Transform.h"

CRectTransform::CRectTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CRectTransform::CRectTransform(const CRectTransform& _rhs)
	: CComponent(_rhs)
{
}

HRESULT CRectTransform::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRectTransform::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_tRectTransformDesc = *static_cast<RECTDESC*>(pArg);

		Resize_MinMax();
	}
	return S_OK;
}

void CRectTransform::Update_Rect(CTransform* _pTransform, const _uint& _iWinSizeX, const _uint& _iWinSizeY, const _float& _fRatio)
{
	if (nullptr == _pTransform)
		return;

	_pTransform->Set_Scale(_float3(m_tRectTransformDesc.v2Size.x, m_tRectTransformDesc.v2Size.y, 1.f));
	_pTransform->Set_Pos(XMVectorSet(m_tRectTransformDesc.v2Pos.x - _iWinSizeX * 0.5f,
		-m_tRectTransformDesc.v2Pos.y + _iWinSizeY * 0.5f, _fRatio, 1.f));
}

_bool CRectTransform::Is_MouseOnRect(const _float2& _v2MousePos)
{
	if (m_v2MinPos.x <= _v2MousePos.x && m_v2MaxPos.x >= _v2MousePos.x)
	{
		if (m_v2MinPos.y <= _v2MousePos.y && m_v2MaxPos.y >= _v2MousePos.y)
		{
			return true;
		}
	}
	return false;
}

void CRectTransform::Resize_MinMax()
{
	_vector vPos = XMLoadFloat2(&m_tRectTransformDesc.v2Pos);
	_vector vSize = XMLoadFloat2(&m_tRectTransformDesc.v2Size) / 2.f;

	_float2 v2MinPos, v2MaxPos;

	XMStoreFloat2(&v2MinPos, (vPos - vSize));
	XMStoreFloat2(&v2MaxPos, (vPos + vSize));

	if (v2MinPos.x > v2MaxPos.x)
		Swap(v2MinPos.x, v2MaxPos.x);

	if (v2MinPos.y > v2MaxPos.y)
		Swap(v2MinPos.x, v2MaxPos.x);

	m_v2MaxPos = v2MaxPos;
	m_v2MinPos = v2MinPos;
}

CRectTransform* CRectTransform::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRectTransform* pInstance = new CRectTransform(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CRectTransform::Create To Failed");
	}
	return pInstance;
}

CComponent* CRectTransform::Clone(void* _pArg)
{
	CRectTransform* pInstance = new CRectTransform(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CRectTransform::Clone To Failed");
#endif
	}
	return pInstance;
}

void CRectTransform::Free()
{
	__super::Free();
}
