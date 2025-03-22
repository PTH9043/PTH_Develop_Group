#pragma once
#include "CIMGUI_MD.h"

BEGIN(Engine)
class CTexture;
class CGameObject;
END
BEGIN(Model)
class CCamera_MD;
class CCamTool_MD final : public CIMGUI_MD
{
protected:
	explicit CCamTool_MD(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCamTool_MD() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds) override;
	virtual HRESULT Render() override;
protected:
	// Show Window
	virtual void Show_Window() override;
	// Control Camera
	void Control_Camera();
protected:
	CCamera_MD* m_pCamera = nullptr;
public:
	static CCamTool_MD* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END