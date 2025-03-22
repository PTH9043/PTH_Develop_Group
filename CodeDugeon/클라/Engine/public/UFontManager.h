#pragma once
#include "UBase.h"

BEGIN(Engine)
class UFont;
class URenderTargetManager;
class USwapChain;
class UDevice;
class UGpuCommand;
class UGameInstance;
class UFont;

class UFontManager final : public UBase {
	using FONTCONTAINER = UNORMAP<_wstring, SHPTR<UFont>>;
	using FONTDRAWLIST = LIST<SHPTR<UFont>>;
public:
	UFontManager();
	NO_COPY(UFontManager)
	DESTRUCTOR(UFontManager)
public:
	virtual void Free() override;
	HRESULT ReadyFontManager(UGameInstance* _pGameInstance, CSHPTRREF<URenderTargetManager> _spRenderTargetManager);
	HRESULT FontCreate(const _wstring& _strFontName, const _wstring& _strPath);
	SHPTR<UFont> FontCreateAdd(const _wstring& _strFontName, const _wstring& _strPath);
	SHPTR<UFont> AddFont(const _wstring& _strFontName);
	void ReleaseOriginFont(const _wstring& _strFontName);
	void RemoveFont(CSHPTRREF<UFont> _pFont);

	void Render();
private:
	FONTCONTAINER::iterator FindFont(const _wstring& _strFontName);
private:
	_uint																m_iFontCnt;
	LIST<_uint>													m_RemainFontDescriptorIndex;
	FONTCONTAINER											m_FontContainer;
	FONTDRAWLIST											m_FontDrawList;

	SHPTR<UDevice>										m_spDevice;
	SHPTR<UGpuCommand>							m_spGpuCommand;

	SHPTR< DirectX::DescriptorHeap>		m_pDescriptor;
	SHPTR<DirectX::GraphicsMemory>		m_pGraphicMemory;


	SHPTR<DirectX::SpriteBatch>				m_spSpriteBatch;
	D3D12_VIEWPORT										m_tViewPort;
	const _uint													FONT_MAX_SIZE;
};

END
