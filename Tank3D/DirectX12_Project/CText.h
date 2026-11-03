#pragma once
#include "CUserInterface.h"

class CText : public CUserInterface {
public:
	typedef struct tagTextDesc {
		// RGBINFo 
		RGBINFO tInfo{};
	}TEXTDESC;
protected:
	explicit CText() : CUserInterface(UITEXT) {}
	CText(const CText& _rhs) : CUserInterface(_rhs) {}
	virtual ~CText() = default;
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
protected:
	const _wstring& Get_Text() const { return m_strText; }
	const RGBINFO& Get_RGBInfo() const { return m_tRGBInfo; }

	const HFONT GetFont() const { return m_hFont; }
	const HFONT GetOldFont() const { return m_hOldFont; }
	// Set Font, OldFont 
	void SetFont(const HFONT& _hFont) { this->m_hFont = _hFont; }
	void SetOldFont(const HFONT& _hOldFont) { this->m_hOldFont = _hOldFont; }
private:
	_wstring			m_strText{ L"" };
	HFONT			m_hFont{ NULL };
	HFONT			m_hOldFont{ NULL };
	// RGBINFo
	RGBINFO		m_tRGBInfo{};
};

