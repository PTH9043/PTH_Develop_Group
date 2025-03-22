#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
private:
	explicit CVIBuffer_Cube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	explicit CVIBuffer_Cube(const CVIBuffer_Cube& _rhs);
	virtual ~CVIBuffer_Cube() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual HRESULT Render();
private:

public:
	static CVIBuffer_Cube* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END