#include "CRectTransform.h"
#include "CMethod.h"
#include "CTransform.h"

shared_ptr<CComponent> CRectTransform::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CRectTransform> pRectTransform = CSmartPointerFactory<CRectTransform>::CreateShared(*this);
	if (FAILED(pRectTransform->NativeConstruct(_tDatas))) {
		Safe_Free(pRectTransform);
		DEBUG_MSG("CRectTransform::Create To Failed")
	}
	return pRectTransform;
}

void CRectTransform::Free()
{
}

HRESULT CRectTransform::NativeConstruct()
{
	return S_OK;
}

HRESULT CRectTransform::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas)))
		return E_FAIL;

	if (_tDatas.size() >= 2) {
		RECTDESC tRectDesc = CMethod::ConvertTemplate_Index<RECTDESC>(&_tDatas, 1);
		//XMStoreFloat4x4(&m_ProjectMatrix, 
		//	XMMatrixOrthographicLH((_float)WINDOW_WIDTH, (_float)WINDOW_HEIGHT, 0.f, 1.f));
		ResizeMinMax(tRectDesc.v2Pos, tRectDesc.v2Size);
		m_pTransform = tRectDesc.pTransform;
		if (nullptr == m_pTransform.lock())
			return E_FAIL;
	}
	return S_OK;
}

void CRectTransform::Tick(const _double& _dTimeDelta)
{
	m_pTransform.lock()->Set_Scale(_float3(m_v2Size.x, m_v2Size.y, 1.f));
	m_pTransform.lock()->Set_Pos(XMVectorSet(m_v2Pos.x - WINDOW_HALF_WIDTH,
		m_v2Pos.y + WINDOW_HALF_HEIGHT, m_fRatio, 1.f));
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

void CRectTransform::ResizeMinMax(const _float2& _vPos, const _float2& _vSize)
{
	m_v2Pos = _vPos;
	m_v2Size = _vSize;

	_vector vPos = _vPos.Get_Vector();
	_vector vSize = _vSize.Get_Vector();

	_float2 vMinPos = vPos - vSize * 4.f , vMaxPos = vPos + vSize * 4.f;
	
	if (vMinPos.x > vMaxPos.x)
		Swap(vMinPos.x, vMaxPos.x);

	if (vMinPos.y > vMaxPos.y)
		Swap(vMinPos.y, vMaxPos.y);

	vMinPos.x += m_v2Size.x * 4.f;

	vMinPos.y += m_v2Size.y * 3.f;
	vMaxPos.y -= m_v2Size.y * 2.f;

	m_v2MaxPos = vMaxPos;
	m_v2MinPos = vMinPos;
}
