#include "EngineDefine.h"
#include "URectTransform.h"
#include "UMethod.h"
#include "UTransform.h"

URectTransform::URectTransform(CSHPTRREF<UDevice> _spDevice) :
	UComponent(_spDevice)
{
}

URectTransform::URectTransform(const URectTransform& _rhs) :
	UComponent(_rhs)
{
}

void URectTransform::Free()
{
}

HRESULT URectTransform::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	return S_OK;
}

HRESULT URectTransform::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	if (FAILED(__super::NativeConstructClone(_vecDatas)))
		return E_FAIL;
	if (_vecDatas.size() > 0)
	{
		m_stRectDesc = UMethod::ConvertTemplate_Index<URectTransform::RECTDESC>(_vecDatas, 0);
		RETURN_CHECK(m_stRectDesc.pTransform == nullptr, E_FAIL);

	ResizeMinMax();
	ComputeRectSize();
	}
	return S_OK;
}

void URectTransform::ChangeSize(const _uint _iWinX, const _uint _iWinY)
{
	RETURN_CHECK(nullptr == m_stRectDesc.pTransform, ;);
	m_stRectDesc.iWindowX = static_cast<_float>(_iWinX);
	m_stRectDesc.iWindowY = static_cast<_float>(_iWinY);

	ResizeMinMax();
	ComputeRectSize();
}

void URectTransform::UpdateRatio(const _float _fRatio)
{
	RETURN_CHECK(nullptr == m_stRectDesc.pTransform, ;);

	m_stRectDesc.fRatio = _fRatio;
	ResizeMinMax();
	ComputeRectSize();
}

void URectTransform::UpdatePos(const _float2& _vPos)
{
	RETURN_CHECK(nullptr == m_stRectDesc.pTransform, ;);
	m_stRectDesc.vPos = _vPos;
	ResizeMinMax();
	ComputeRectSize();
}

void URectTransform::UpdateSize(const _float2& _vSize)
{
	RETURN_CHECK(nullptr == m_stRectDesc.pTransform, ;);
	m_stRectDesc.vSize = _vSize;
	ResizeMinMax();
	ComputeRectSize();
}

_bool URectTransform::IsMouseOnRect(const _float2& _vMousePos)
{
	if (m_vMinPos.x <= _vMousePos.x && m_vMaxPos.x >= _vMousePos.x)
	{
		if (m_vMinPos.y <= _vMousePos.y && m_vMaxPos.y >= _vMousePos.y)
		{
			return true;
		}
	}
	return false;
}

void URectTransform::ResizeMinMax()
{
	_float2 vMinPos, vMaxPos, vSize = m_stRectDesc.vSize / 2.f;

	XMStoreFloat2(&vMinPos, (m_stRectDesc.vPos - vSize));
	XMStoreFloat2(&vMaxPos, (m_stRectDesc.vPos + vSize));

	if (vMinPos.x > vMaxPos.x)
		std::swap(vMinPos.x, vMaxPos.x);

	if (vMinPos.y > vMaxPos.y)
		std::swap(vMinPos.y, vMaxPos.y);

	m_vMaxPos = vMaxPos;
	m_vMinPos = vMinPos;
}

void URectTransform::ComputeRectSize()
{
	m_stRectDesc.pTransform->SetScale(_float3(m_stRectDesc.vSize.x, m_stRectDesc.vSize.y, 1.f));
	m_stRectDesc.pTransform->SetPos(_float3(m_stRectDesc.vPos.x - m_stRectDesc.iWindowX * 0.5f,
		-m_stRectDesc.vPos.y + m_stRectDesc.iWindowY * 0.5f, m_stRectDesc.fRatio));
}
