#pragma once
#include "CIMGUI.h"

BEGIN(Engine)
class CTexture;
class CGameObject;
END
BEGIN(Client)
class CCamera_Tool;
class CCamTool final : public CIMGUI
{
protected:
	explicit CCamTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCamTool() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_Tool() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds) override;
	virtual HRESULT Render() override;
	virtual HRESULT Release_Tool() override;
protected:
	// Show Window
	virtual void Show_Window() override;
	// Control Camera
	void Control_Camera();
protected:
	CCamera_Tool*	m_pCamera = nullptr;
public:
	static CCamTool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END