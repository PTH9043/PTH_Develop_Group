#pragma once

#include "VIBuffer.h"

BEGIN(Engine)
class CGameObject;

class ENGINE_DLL CVIBuffer_Effect_Instance : public CVIBuffer
{
public:
	typedef struct ParticleDesc
	{
		_float			fSpeed = 1.f;
		_float			fTime = 0.f;
		_float			fBillboardSize = 1.f;
		_float4			vMovement = _float4(0.f, 0.f, 1.f, 0.f);
	}PARTICLEDESC;

	typedef struct KeyFramDesc
	{
		_float			KeyFrame_TimeEnd = 0.f;
		_float			KeyFrame_RelaxSpeed = 1.f;
		_float			KeyFrame_Speed = 0.f;
		_float			KeyFrame_fRotation = 0.f;
		_float			KeyFrame_fRelaxRotation = 0.f;
		_float4			KeyFrame_Movement = _float4(0.f, 0.f, 0.f, 0.f);
	}KEYFRAMDESC;

private:
	explicit CVIBuffer_Effect_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CVIBuffer_Effect_Instance(const CVIBuffer_Effect_Instance& _rhs);
	virtual ~CVIBuffer_Effect_Instance() = default;

public:
	HRESULT NativeConstruct_Prototype(_uint iNumInstance);
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	void Tick(const _double& _dTimeDelta);
	virtual HRESULT Render() override;
	HRESULT CreateInstanceBuffer(const D3D11_BUFFER_DESC& _tBufferDesc, const D3D11_SUBRESOURCE_DATA& _tBufferSubResourceData);

public:
	HRESULT ChangeBufferCount(_uint iCount);
	const INSTANCEDESC& Get_InstanceDesc() { return m_tInstanceDesc; }
	void Set_InstanceDesc(INSTANCEDESC tInstanceDesc) { m_tInstanceDesc = tInstanceDesc; }
	void Reset();

private:
	INSTANCEDESC m_tInstanceDesc;

private:
	ID3D11Buffer*			m_pVBInstance = nullptr;
	_uint					m_iStrideInstance = 0;
	_uint					m_iNumInstance = 0;

	vector<PARTICLEDESC>	m_vecParticle;
	KEYFRAMDESC				m_KeyFrameDesc;
	_float					m_fTime = 0.f;
	_float					m_fMaxTime = 10.f;

private:
	HRESULT	SetUp_VertexBuffer();
	HRESULT	SetUp_IndexBuffer();
	HRESULT	SetUp_InstanceBuffer();
	void	Reset_Pariticle();
	void	Reset_InstanceBuffer();

public:
	static CVIBuffer_Effect_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END