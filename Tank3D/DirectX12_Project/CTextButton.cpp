#include "CTextButton.h"
#include"CMethod.h"
#include "CTransform.h"
#include "CRectTransform.h"
#include "CGameInstance.h"
#include "CInput_Manager.h"

shared_ptr<CActor> CTextButton::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CTextButton> pText = CSmartPointerFactory<CTextButton>::CreateShared(*this);
	if (FAILED(pText->NativeConstruct(_tDatas))) {
		Safe_Free(pText);
		DEBUG_MSG("CTextButton::Clone To Failed");
	}
	return pText;
}

void CTextButton::Free()
{
	__super::Free();
}

HRESULT CTextButton::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CTextButton::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	if (_tDatas.size() >= 3) {
		TEXTDESC tDesc = CMethod::ConvertTemplate_Index< TEXTDESC>(&_tDatas, 2);
		m_tSelectRgbInfo = tDesc.tInfo;
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CTextButton::CreateComponent()
{
	if (FAILED(__super::CreateComponent()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTextButton::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	SelectText(_dTimeDelta);
	return S_OK;
}

HRESULT CTextButton::Late_Tick(const _double& _dTimeDelta)
{
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CTextButton::Render(const HDC& _hDC)
{
	if (false == m_bSelected) {
		__super::Render(_hDC);
	}
	else {
		_float2 vScale = Get_RectTransform()->GetSize(), vPos = Get_RectTransform()->GetPos();
		HFONT hFont = GetFont();
		HFONT hOldFont = GetOldFont();
		hFont = CreateFont(static_cast<int>(vScale.x), static_cast<int>(vScale.y), 0, 0,
			0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, Get_Text().c_str());
		hOldFont = static_cast<HFONT>(SelectObject(_hDC, hFont));
		SetTextColor(_hDC, RGB(m_tSelectRgbInfo.r, m_tSelectRgbInfo.g, m_tSelectRgbInfo.b));
		SetBkMode(_hDC, TRANSPARENT);
		TextOut(_hDC, static_cast<int>(vPos.x), static_cast<int>(vPos.y), Get_Text().c_str(),
			lstrlen(Get_Text().c_str()));
		SelectObject(_hDC, hOldFont);
		DeleteObject(hFont);
	}
	return S_OK;
}

void CTextButton::SelectText(const _double& _dTimeDelta)
{
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	
	if (true == Get_RectTransform()->Is_MouseOnRect(pGameInstance->GetMousePos())) {
		m_bSelected = true;
		m_bClickedTextButton = pGameInstance->Is_MouseClick(DMBUTTON::DM_LEFT);
	}
	else
	{
		m_bSelected = false;
		m_bClickedTextButton = false;
	}
}
