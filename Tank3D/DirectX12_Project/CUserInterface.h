#pragma once
#include "CActor.h"

class CRectTransform;
class CRenderer;

enum UIINTERFACETYPE {

	UITEXT, UIBUTTON, UI_END
};

class CUserInterface abstract : public CActor {
public:
	typedef struct tagUIDesc
	{
		// Pos, Size
		_float2 v2Pos;
		_float2 v2Size;
	}UIDESC;
protected:
	explicit CUserInterface(const UIINTERFACETYPE _eUIType) : m_eUIType(_eUIType){}
	CUserInterface(const CUserInterface& _rhs) : CActor(_rhs) {}
	virtual ~CUserInterface() = default;

public:
	virtual shared_ptr<CActor> Clone(const VOIDDATAS& _tDatas) override;
	virtual void Free() override;

public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct(VOIDDATAS _tDatas) override;

protected:
	virtual HRESULT CreateComponent() override;
	HRESULT CreateRectComp(const VOIDDATAS& _tDatas);
public:
	virtual HRESULT Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(const HDC& _hDC) override;

public:
	const shared_ptr<CRectTransform>& Get_RectTransform() const { return m_pRectTransform; }
	const UIINTERFACETYPE& Get_InterfaceType() const { return m_eUIType; }
protected:
	const shared_ptr<CRenderer>& Get_Renderer() const { return m_pRenderer; }
private:
	// Rect Transfrom
	shared_ptr<CRectTransform>	m_pRectTransform{ nullptr };
	shared_ptr<CRenderer>				m_pRenderer{ nullptr};
	// InerfaceType
	UIINTERFACETYPE						m_eUIType{ UI_END };
};

