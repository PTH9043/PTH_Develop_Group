#pragma once
#include "CUserInterface.h"

class CButton : public CUserInterface {
public:
	typedef struct tagButtonDesc {
		// RGBINFo 
		RGBINFO tInfo{};
	}BUTTONDESC;
protected:
	explicit CButton() : CUserInterface(UIBUTTON) {}
	CButton(const CButton& _rhs) : CUserInterface(_rhs) {}
	virtual ~CButton() = default;

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

public:
	// SetText
	void SetText(const _wstring& _strText) { this->m_strText = _strText; }
private:
	_wstring			m_strText{ L"" };
};

