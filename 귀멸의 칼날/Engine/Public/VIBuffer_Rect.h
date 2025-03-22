#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& _rhs);
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;

public:
	static CVIBuffer_Rect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END