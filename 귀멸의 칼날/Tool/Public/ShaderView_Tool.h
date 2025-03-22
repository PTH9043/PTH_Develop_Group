#pragma once
#include "ImGuiClass_Tool.h"

BEGIN(Tool)

class CShaderView_Tool  final: public CImGuiClass_Tool
{
private:
	explicit CShaderView_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CShaderView_Tool() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT NativeConstruct_InnerValue() override;
	virtual HRESULT Tick(const _double& _dDeltaSeconds)  override;
	virtual HRESULT Late_Tick(const _double& _dDeltaSeconds);
	virtual HRESULT Render();
	virtual HRESULT Release_InnerValue() override;
protected:
	// Show Windows
	virtual void Show_Window() override;
	void Show_RenderTargetImg();
	void LightControlMode();
protected:
	// Render Target Texture List
	map<_wstring, ID3D11ShaderResourceView*> m_RenderTarget_TextureList;
	// Window Desc
	WINDESC										m_tWinDesc;
	_bool										m_bSelectMode = false;
	_wstring									m_strSelectImageName = L"";
	_int										m_iModeSelect = 0;
	// RenderTarget Show 
	// Pixel List
	list<_ulong>								m_PixelList;
	// Show Texture
	ID3D11Texture2D*							m_pShowTexture = nullptr;
	// Pixel Arr 
	_ulong*										m_pPixelArr = nullptr;
	// Light Controller 
	_float3										m_v3Position = _float3(0.f, 0.f, 0.f);
	_float3										m_v3LookAt = _float3(0.f, 0.f, 0.f);
	_bool										m_bReceiveValue = false;
	_bool										m_bUpdateLight = false;

	// Debug Render Obj
	class CDebug_RenderObj_Tool*				m_pDebugRenderObj = nullptr;
public:
	static CShaderView_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END
