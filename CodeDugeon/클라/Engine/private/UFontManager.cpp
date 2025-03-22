#include "EngineDefine.h"
#include "UFontManager.h"
#include "URenderTargetManager.h"
#include "UGameInstance.h"
#include "UGpuCommand.h"
#include "USwapChain.h"
#include "UDevice.h"
#include "UFont.h"

UFontManager::UFontManager() :
	m_FontContainer{},
	m_FontDrawList{},
	m_spDevice{ nullptr }, 
	m_spGpuCommand{ nullptr },
	m_pDescriptor{ nullptr },
	m_pGraphicMemory{ nullptr },
	m_iFontCnt{ 0 },
	m_RemainFontDescriptorIndex{},
	m_spSpriteBatch{ nullptr },
	m_tViewPort{},
	FONT_MAX_SIZE{ 50 }
{
}

void UFontManager::Free()
{
}

HRESULT UFontManager::ReadyFontManager(UGameInstance* _pGameInstance, CSHPTRREF<URenderTargetManager> _spRenderTargetManager)
{
	m_spDevice = _pGameInstance->GetDevice();
	m_spGpuCommand = _pGameInstance->GetGpuCommand();
	SHPTR<USwapChain> spSwapChain = _pGameInstance->GetSwapChain();

	m_tViewPort = _pGameInstance->GetD3DViewport();

	m_pDescriptor = Make::MakeShared<DirectX::DescriptorHeap>(m_spDevice->GetDV().Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, FONT_MAX_SIZE);

	m_pGraphicMemory = Make::MakeShared<DirectX::GraphicsMemory>(m_spDevice->GetDV().Get());
	DXGI_SWAP_CHAIN_DESC1 tDesc = spSwapChain->GetSwapChainDesc();

	DirectX::ResourceUploadBatch Batch(m_spDevice->GetDV().Get());

	Batch.Begin();

	DirectX::RenderTargetState rtState{ tDesc.Format, _spRenderTargetManager->GetDepthFormat() };

	DirectX::SpriteBatchPipelineStateDescription pipeLine{ rtState };

	m_spSpriteBatch = Make::MakeShared<DirectX::SpriteBatch>(m_spDevice->GetDV().Get(), Batch, pipeLine,
		&m_tViewPort);
	auto upLoadResourceFinished = Batch.End(m_spGpuCommand->GetCmdQue().Get());
	upLoadResourceFinished.wait();
	RETURN_CHECK(!upLoadResourceFinished.valid(), E_FAIL);
	return S_OK;
}

HRESULT UFontManager::FontCreate(const _wstring& _strFontName, const _wstring& _strPath)
{
	if (m_FontContainer.size() >= FONT_MAX_SIZE)
		return S_OK;

	SHPTR<UFont> pFont = nullptr;

	if (m_RemainFontDescriptorIndex.empty()) {
		pFont = CreateNative<UFont>(m_spDevice, m_spGpuCommand, m_pDescriptor, m_iFontCnt, _strPath);
	}
	else {
		std::list<_uint>::iterator it = m_RemainFontDescriptorIndex.begin();
		pFont = CreateNative<UFont>(m_spDevice, m_spGpuCommand, m_pDescriptor, *it, _strPath);
		m_RemainFontDescriptorIndex.erase(it);
	}

	RETURN_CHECK(nullptr == pFont, E_FAIL);
	m_FontContainer.insert(MakePair<_wstring, SHPTR<UFont>>(_strFontName, pFont));
	return S_OK;
}

SHPTR<UFont> UFontManager::FontCreateAdd(const _wstring& _strFontName, const _wstring& _strPath)
{
	if (m_FontContainer.size() >= FONT_MAX_SIZE)
		return nullptr;

	SHPTR<UFont> pFont = nullptr;

	if (m_RemainFontDescriptorIndex.empty()) {
		pFont = CreateNative<UFont>(m_spDevice, m_spGpuCommand, m_pDescriptor, m_iFontCnt, _strPath);
	}
	else {
		std::list<_uint>::iterator it = m_RemainFontDescriptorIndex.begin();
		pFont = CreateNative<UFont>(m_spDevice, m_spGpuCommand, m_pDescriptor, *it, _strPath);
		m_RemainFontDescriptorIndex.erase(it);
	}

	RETURN_CHECK(nullptr == pFont, nullptr);
	m_FontContainer.insert(MakePair<_wstring, SHPTR<UFont>>(_strFontName, pFont));
	return pFont->Clone();
}

SHPTR<UFont> UFontManager::AddFont(const _wstring& _strFontName)
{
	FONTCONTAINER::iterator Font = FindFont(_strFontName);
	RETURN_CHECK(m_FontContainer.end() == Font, nullptr);

	SHPTR<UFont> CloneFont = (*Font).second->Clone();
	m_FontDrawList.push_back(CloneFont);
	return CloneFont;
}

void UFontManager::ReleaseOriginFont(const _wstring& _strFontName)
{
	FONTCONTAINER::iterator Font = FindFont(_strFontName);
	RETURN_CHECK(m_FontContainer.end() == Font, ;);

	for (FONTDRAWLIST::iterator it = m_FontDrawList.begin(); it != m_FontDrawList.end();) {
		if ((*it)->GetFontIndex() == (*Font).second->GetFontIndex()) {
			it = m_FontDrawList.erase(it);
		}
		else {
			++it;
		}
	}
	m_RemainFontDescriptorIndex.push_back(Font->second->GetFontIndex());
	m_FontContainer.erase(Font);
}

void UFontManager::RemoveFont(CSHPTRREF<UFont> _pFont)
{
	FONTDRAWLIST::iterator it = std::find(m_FontDrawList.begin(), m_FontDrawList.end(), _pFont);
	RETURN_CHECK(it == m_FontDrawList.end(), ;);
	m_FontDrawList.erase(it);
}

void UFontManager::Render()
{
	Dx12DescriptorHeap* DescHeap[] = { m_pDescriptor->Heap() };
	//CBV , Sampler 2개 인자만 넣을 수 있다.
	m_spGpuCommand->GetGpuCmdList()->SetDescriptorHeaps(1, DescHeap);
	m_spSpriteBatch->SetViewport(m_tViewPort);
	m_spSpriteBatch->Begin(m_spGpuCommand->GetGpuCmdList().Get());
	for (FONTDRAWLIST::iterator it = m_FontDrawList.begin(); it != m_FontDrawList.end(); ++it) {
		(*it)->Render(m_spSpriteBatch.get(), m_tViewPort);
	}
	m_spSpriteBatch->End();
	// GraphicMemory가 필요함, SpriteBatch를 사용하기 위해서는, 왜 그런지 이해 불가
	m_pGraphicMemory->Commit(m_spGpuCommand->GetCmdQue().Get());
}

UFontManager::FONTCONTAINER::iterator UFontManager::FindFont(const _wstring& _strFontName)
{
	FONTCONTAINER::iterator it = m_FontContainer.find(_strFontName);
	return it;
}
