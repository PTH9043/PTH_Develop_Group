#pragma once
#include "UBase.h"

BEGIN(Engine)
class UDevice;
class UGpuCommand;

class UTexture : public UBase {
public:
	UTexture();
	NO_COPY(UTexture)
	DESTRUCTOR(UTexture)
public:
	const ComPtr<Dx12Resource>& GetTex() const { return m_cpTexture; }
	const ComPtr<Dx12DescriptorHeap> GetHeap(const DESCRIPTOR_TYPE _eType) { return FindDescriptor(_eType); }

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetShaderViewBeginHandle(const DESCRIPTOR_TYPE& _eType = DESCRIPTOR_TYPE::SRV);

	const DESCRIPTOR_TYPE& GetTextureDescriptorType() const { return m_eTextureDescriptorType; }

	const _float GetHeight() const { return m_fTextureHeight; }
	const _float GetWidth() const { return m_fTextureWidth; }
public:
	virtual void Free() override;
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrPath,
		const TEXTURECREATETYPE _eTextureType = TEXTURECREATETYPE::TEX2D, DirectX::ResourceUploadBatch* _pResourceUploader = nullptr);
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, 	const TEXTURETYPEDESC& _stTextureDesc);
	HRESULT NativeConstruct(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12Resource>& _cpResource);
private:
	HRESULT CreateView(CSHPTRREF<UDevice> _spDevice, const TEXTURECREATETYPE _eTextureType = TEXTURECREATETYPE::TEX2D);
	HRESULT CreateFromResource(CSHPTRREF<UDevice> _spDevice, const ComPtr<Dx12Resource>& _cpResource);

	const ComPtr<Dx12DescriptorHeap> FindDescriptor(const DESCRIPTOR_TYPE _eDescriptor);
private:
	using DESCRIPTORHEAPS = UNORMAP<DESCRIPTOR_TYPE, ComPtr<Dx12DescriptorHeap>>;
	using DESCRIPTORHANDLES = UNORMAP<DESCRIPTOR_TYPE, D3D12_CPU_DESCRIPTOR_HANDLE>;

	DESCRIPTOR_TYPE					m_eTextureDescriptorType;

	ComPtr<Dx12Resource>		m_cpTexture;
	DESCRIPTORHEAPS					m_DescriptorHeaps;
	DESCRIPTORHANDLES			m_DescriptorHandles;
	DXGI_FORMAT							m_eDXTextureFormat;

	_float											m_fTextureWidth;
	_float											m_fTextureHeight;
private:
	static constexpr D3D12_CPU_DESCRIPTOR_HANDLE NULL_HANDLE {NULL};
};

END