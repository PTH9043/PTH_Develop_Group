#pragma once
#include "CText.h"

class CTextButton : public CText {
protected:
	explicit CTextButton() : CText() {}
	CTextButton(const CTextButton& _rhs) : CText(_rhs) {}
	virtual ~CTextButton() = default;

public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas) override;
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;
public:
	virtual HRESULT CreateComponent() override;
public:
	virtual HRESULT Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(const HDC& _hDC) override;
private:
	// select Text
	void SelectText(const _double& _dTimeDelta);
public:
	// Selected 
	const _bool IsSelected() const { return m_bSelected; }
	const _bool IsClickedTextButton() const { return m_bClickedTextButton; }
private:
	RGBINFO		m_tSelectRgbInfo{};
	_bool				m_bSelected{ false };
	_bool				m_bClickedTextButton{ false };
};

