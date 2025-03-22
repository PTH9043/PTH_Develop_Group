#pragma once
#include "CUserInterface.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CButton : public CUserInterface
{
public:
	typedef struct tagButtonDesc
	{
		UIDESC		tUIDesc;
		_wstring	strOtherImgName;
	}BUTTONDESC;
protected:
	explicit CButton(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CButton(const CButton& _rhs);
	virtual ~CButton() = default;
public:
	const _bool& Is_Press() const { return m_bPress; }
	const _bool& Is_OnRect() const { return m_bOnRect; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
private:
	// Setting Constant Table
	HRESULT SetUp_ConstantTable();
	// Ready Component
	HRESULT Ready_Component();
	// Judge Mouse On Rect
	void Judge_MouseOnRect();
	// Mouse Click Button
	void Mouse_ClickButton(const _double& _dTimeDelta);
protected:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRenderCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIRectCom = nullptr;
protected:
	_wstring					m_strStandardImg = L"";
	_wstring					m_strOtherImg = L"";
protected:
	_bool							m_bPress = false;
	_bool							m_bOnRect = false;
protected:
	CUSTIMER					m_tPressTimer = CUSTIMER(1.f);
public:
	static CButton* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END