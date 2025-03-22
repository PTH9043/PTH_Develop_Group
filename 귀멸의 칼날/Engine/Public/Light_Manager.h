#pragma once
#include "Base.h"
#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	_uint				Get_LightsNum() const { return (_uint)m_Lights.size(); }
	const LIGHTDESC*	Get_LightDesc(const _uint& _iIndex = 0);
public:
	// Add Light
	HRESULT Add_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tLightDesc, class CLight** _ppOut);
	HRESULT Modify_Light(const LIGHTDESC& _tLightDesc, const _uint& _iIndex);
	HRESULT Active_Light(const _uint& _iIndex, const _bool& _bIsControl);
	HRESULT Delete_Light(const _uint& _iIndex);
	HRESULT Clear_Light();
	HRESULT Render_Lights(class CShader* _pShader, class CVIBuffer_Rect* _pRect);

	HRESULT	Save_LightsData(const _wstring& _tSavePath);
	HRESULT	Load_LightsData(const _wstring& _tLoadPath, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
private:
	typedef list<CLight*> LIGHTS;
	LIGHTS m_Lights;
public:
	virtual void Free() override;
};

END