#include "CText.h"
#include"CMethod.h"
#include "CTransform.h"
#include "CRectTransform.h"

shared_ptr<CActor> CText::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CText> pText = CSmartPointerFactory<CText>::CreateShared(*this);
	if (FAILED(pText->NativeConstruct(_tDatas))) {
		Safe_Free(pText);
		DEBUG_MSG("CText::Clone To Failed");
	}
	return pText;
}

void CText::Free()
{
	DeleteObject(m_hFont);
	DeleteObject(m_hOldFont);
	m_hFont = NULL;
	m_hOldFont = NULL;
	__super::Free();
}

HRESULT CText::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CText::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	if (_tDatas.size() >= 2) {
		TEXTDESC tDesc = CMethod::ConvertTemplate_Index< TEXTDESC>(&_tDatas, 1);
		m_tRGBInfo = tDesc.tInfo;
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CText::CreateComponent()
{
	if (FAILED(__super::CreateComponent()))
		return E_FAIL;

	return S_OK;
}

HRESULT CText::Tick(const _double& _dTimeDelta)
{
	return __super::Tick(_dTimeDelta);
}

HRESULT CText::Late_Tick(const _double& _dTimeDelta)
{
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CText::Render(const HDC& _hDC)
{
	_float2 vScale = Get_RectTransform()->GetSize(), vPos = Get_RectTransform()->GetPos();

	m_hFont = CreateFont(static_cast<int>(vScale.x), static_cast<int>(vScale.y), 0, 0,
		0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, m_strText.c_str());
	m_hOldFont = static_cast<HFONT>(SelectObject(_hDC, m_hFont));
	SetTextColor(_hDC, RGB(m_tRGBInfo.r, m_tRGBInfo.g, m_tRGBInfo.b));
	SetBkMode(_hDC, TRANSPARENT);
	TextOut(_hDC, static_cast<int>(vPos.x), static_cast<int>(vPos.y), m_strText.c_str(),
		lstrlen(m_strText.c_str()));
	SelectObject(_hDC, m_hOldFont);
	DeleteObject(m_hFont);

	return S_OK;
}
