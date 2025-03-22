#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class CGameObject;

class CVIBuffer_Cell final : public CVIBuffer
{
private:
	explicit CVIBuffer_Cell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CVIBuffer_Cell(const CVIBuffer_Cell& _rhs);
	virtual ~CVIBuffer_Cell() = default;
public:


public:
	HRESULT NativeConstruct_Prototype(const _float3* _pPosition);
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render();
public:
	static CVIBuffer_Cell* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _float3* _pPosition);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END