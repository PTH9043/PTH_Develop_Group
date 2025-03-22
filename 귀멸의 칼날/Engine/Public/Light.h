#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
private:
	explicit CLight(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext, const LIGHTDESC & _tLightDesc);
	virtual ~CLight() = default;

public:
	const LIGHTDESC* Get_LightDesc() const { return &m_tLightDesc; }

	const _uint& Get_Index() const { return m_iIndex; }
	void Set_Index(const _uint& _iIndex) { this->m_iIndex = _iIndex; }
public:
	HRESULT NativeConstruct();
	//HRESULT Render(class CShader* _pShader, class CVIBuffer_Rect* _pVIBufferRect);
	void Set_NotCaluclation(const _bool& _bIsCalculation) { m_bNotCalculation = _bIsCalculation; }
	HRESULT Render(class CShader* _pShader, class CVIBuffer_Rect* _pRect);
	void Set_LightDesc(const LIGHTDESC& _tLightDesc);
private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	_uint										m_iIndex = 0;
private:
	// Light Desc
	LIGHTDESC	m_tLightDesc;
	_bool				m_bNotCalculation = false;
public:
	static CLight* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tLightDesc);
	virtual void Free() override;
};

END