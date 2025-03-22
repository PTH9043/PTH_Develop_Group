#pragma once
#include "Base.h"

BEGIN(Engine)

class CProcessHandler final : public CBase
{
public:
	typedef struct tagProcessHandlerDesc
	{
		tagProcessHandlerDesc() = default;
		tagProcessHandlerDesc(const _float& _fWidth, const _float& _fHeight)
			: fWidth(_fWidth), fHeight(_fHeight)
		{}
		_float fWidth = 0.f;
		_float fHeight = 0.f;
	}PHDESC;
public:
	explicit CProcessHandler();
	virtual ~CProcessHandler() = default;

public:
	ID3D11UnorderedAccessView* Get_UAV() const { return m_pUAV; }
	ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }

public:
	HRESULT NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const PHDESC& _tPHDesc);

protected:
	ID3D11Buffer* m_pBuffer = nullptr;
	ID3D11Texture2D* m_pTexture = nullptr;
	ID3D11ShaderResourceView* m_pSRV = nullptr;
	ID3D11UnorderedAccessView* m_pUAV = nullptr;

public:
	static CProcessHandler* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const PHDESC& _tPHDesc);
	virtual void Free() override;
};

END