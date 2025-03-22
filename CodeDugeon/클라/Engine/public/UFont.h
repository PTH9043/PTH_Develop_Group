#pragma once

#include "UBase.h"

BEGIN(Engine)
class UDevice;
class UGpuCommand;

class UFont final : public UBase {
public:
	UFont();
	UFont(const UFont& _rhs);
	DESTRUCTOR(UFont)
public:
	SHPTR<UFont> Clone();
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UGpuCommand> _spGpuCommand,
		CSHPTRREF<DirectX::DescriptorHeap> _descriptor, _uint& _iFontIndex, const _wstring& _wstrFontPath);
	void Render(DirectX::SpriteBatch* _pBatch, const D3D12_VIEWPORT& _tViewPort);
	virtual void Free() override;
public:
	void SetText(const _wstring& _wstrText) { this->m_tFontDesc.wstrText = _wstrText; }
	void SetPos(const _float2& _vPos) { this->m_tFontDesc.vPos = _vPos; }
	void SetColor(const _float4& _vColor) { this->m_tFontDesc.vColor = _vColor; }
	void SetRotation(const _float _fRot) { this->m_tFontDesc.fRotation = _fRot; }
	void SetOrigin(const _float2& _vOrigin) { this->m_tFontDesc.vOrigin = _vOrigin; }
	void SetScale(const _float2& _vScale) { this->m_tFontDesc.vScale = _vScale; }
	void SetSpriteEffects(const DirectX::DX12::SpriteEffects& _eEffects) { this->m_tFontDesc.eSpriteEffects = _eEffects; }
	void SetDepths(const _float _fDepths) { this->m_tFontDesc.fDepths = _fDepths; }
	void SetRender(const _bool _render) { m_isRender = _render; }
	void SetFontDesc(const FONTDESC& _tFontDesc) { this->m_tFontDesc = _tFontDesc; }

	const _uint& GetFontIndex() const { return m_iFontIndex; }

	const _wstring& GetText() const { return m_tFontDesc.wstrText; }
private:
	_uint															m_iFontIndex;
	// Font Desc
	FONTDESC												m_tFontDesc;
	_bool															m_bClone;
	SHPTR<DirectX::SpriteFont>				m_SpriteFont;
	SHPTR<UDevice>									m_spDevice;
	SHPTR<UGpuCommand>						m_spGpuCommand;

	_bool		m_isRender=false;
};

END
