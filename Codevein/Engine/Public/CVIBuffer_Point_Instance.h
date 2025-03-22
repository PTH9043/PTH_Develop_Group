#pragma once

#include "CVIBuffer.h"

BEGIN(Engine)

class CVIBuffer_Point_Instance final : public CVIBuffer
{
private:
	explicit CVIBuffer_Point_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& _rhs);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _uint& _iNumInstance);
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual HRESULT Render() override;

public:
	void Update(const _double& _dTimeDelta);

private:
	ID3D11Buffer* m_pVBInstance = nullptr;
	_uint							m_iStrideInstance = 0;
	_uint							m_iNumInstance = 0;

private:
	_float* m_pSpeeds = nullptr;

public:
	static CVIBuffer_Point_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _uint& _iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
