#include "EngineDefine.h"
#include "UFont.h"
#include "UGameInstance.h"
#include "UDevice.h"
#include "UGpuCommand.h"

UFont::UFont() : 	m_iFontIndex{0}, m_tFontDesc{}, m_bClone{false},
	m_SpriteFont{},	m_spDevice{nullptr}, m_spGpuCommand{nullptr}
{
}

UFont::UFont(const UFont& _rhs) : m_iFontIndex{ _rhs.m_iFontIndex }, 
m_tFontDesc{ _rhs.m_tFontDesc }, m_bClone{ true }, m_SpriteFont{ _rhs.m_SpriteFont}, 
m_spDevice{ _rhs.m_spDevice }, m_spGpuCommand{ _rhs.m_spGpuCommand }
{
}

SHPTR<UFont> UFont::Clone()
{
	SHPTR<UFont> pFont = CloneThis<UFont>(*this);
	return pFont;
}

HRESULT UFont::NativeConstruct(CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UGpuCommand> _spGpuCommand, 
	CSHPTRREF<DirectX::DescriptorHeap> _descriptor, _uint& _iFontIndex, const _wstring& _wstrFontPath)
{
	m_spDevice = _spDevice;
	m_spGpuCommand = _spGpuCommand;

	ComPtr<Dx12Device> DEVICE = m_spDevice->GetDV();

	DirectX::ResourceUploadBatch Batch{ DEVICE.Get()};
	Batch.Begin();

	m_SpriteFont = Make::MakeShared<DirectX::SpriteFont>(DEVICE.Get(), Batch, _wstrFontPath,
		_descriptor->GetCpuHandle(_iFontIndex), _descriptor->GetGpuHandle(_iFontIndex));

	auto upLoadResourceFinished = Batch.End(m_spGpuCommand->GetCmdQue().Get());
	upLoadResourceFinished.wait();

	m_iFontIndex = _iFontIndex;
	return S_OK;
}

void UFont::Render(DirectX::SpriteBatch* _pBatch, const D3D12_VIEWPORT& _tViewPort)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	RETURN_CHECK(spGameInstance->IsFreeModeCameraEnable() || spGameInstance->IsGamePause(), ;);

	if (m_isRender) {
		m_SpriteFont->DrawString(_pBatch, m_tFontDesc.wstrText.c_str(), m_tFontDesc.vPos, m_tFontDesc.vColor,
			m_tFontDesc.fRotation, m_tFontDesc.vOrigin, m_tFontDesc.vScale, m_tFontDesc.eSpriteEffects, m_tFontDesc.fDepths);
	}
}

void UFont::Free()
{
}
