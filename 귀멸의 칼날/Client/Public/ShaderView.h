#pragma once
#include "ImGuiClass.h"

BEGIN(Client)

class CShaderView  final: public CImGuiClass
{
private:
	explicit CShaderView(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CShaderView() = default;
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
	map<_wstring, ID3D11ShaderResourceView*>	m_RenderTarget_TextureList;
	// Window Desc
	DOCKDESC									m_tWinDesc;
	DOCKDESC									m_tRTDesc;
	DOCKDESC									m_tLightDesc;
	DOCKDESC									m_tDebugDesc;

	_bool										m_bInitSetting = false;
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
	_bool										m_bChangeToShadowCam = false;
	_bool										m_bUpdateLight = false;
	_uint										m_iLightIndex = 0;
	LIGHTDESC									m_CurLightDesc;
	_char										m_szLightSaveName[MAX_PATH] = "";
	_char										m_szLightLoadName[MAX_PATH] = "";

	// Debug Render Obj
	class CDebug_RenderObj*			m_pDebugRenderObj = nullptr;
public:
	static CShaderView* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END
