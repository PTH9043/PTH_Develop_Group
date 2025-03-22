#pragma once
#include "Client_Defines.h"
#include "CUserInterface.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBar : public CUserInterface
{
public:
	typedef struct tagBarDesc
	{
		tagBarDesc() = default;
		tagBarDesc(const _float& _fMaxValue, const _float2& _v2VerticalSize = _float2(0.f, 0.f)
			, const _wstring& _strBarBack = L"HPBar")
			: fMaxValue(_fMaxValue), v2VerticalSize(_v2VerticalSize), strBarBack(_strBarBack)
		{
			fCurValue = _fMaxValue;
			v2MaxVerticalSize = v2VerticalSize;
		}

		_float			fCurValue = 0.f;
		_float			fMaxValue = 0.f;
		_float2		v2VerticalSize = _float2(0.f, 0.f);
		_float2		v2MaxVerticalSize = _float2(0.f, 0.f);
		_wstring	 strBarBack;
	}BARDESC;

	typedef struct tagBarGroup
	{
		tagBarGroup() = default;
		tagBarGroup(const UIDESC& _tUIDesc, const BARDESC& _tBarDesc)
			: tUIDesc(_tUIDesc), tBarDesc(_tBarDesc)
		{}
		UIDESC tUIDesc;
		BARDESC tBarDesc;
	}BARGROUP;
protected:
	explicit CBar(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CBar(const CBar& _rhs);
	virtual ~CBar() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	void Update_Value(const _float& _fCurValue) { m_tBarDesc.fCurValue = _fCurValue; Update_Bar(); }
protected:
	// Setting Constant Table
	virtual HRESULT SetUp_ConstantTable(const _wstring& _strUIName,
		CRectTransform* _pRectTransform, CTransform* _pTransform);
	// Ready Component
	virtual HRESULT Ready_Component();

protected:
	// Update Bar
	void Update_Bar();

protected:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRenderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIRectCom = nullptr;
	// Back Ground data 
	CVIBuffer_Rect*			m_pVIBackRectCom = nullptr;
	CRectTransform*		m_pBackRectTransform = nullptr;
	CTransform*				m_pBackTransform = nullptr;
protected:
	// Bar Desc 
	BARDESC m_tBarDesc;

	const _tchar* HPBAR_NAME = L"HPBar";
	const _tchar* HPBAR2_NAME = L"HPBar2";
public:
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END