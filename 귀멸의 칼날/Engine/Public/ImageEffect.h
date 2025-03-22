#pragma once

#include "Effect.h"

BEGIN(Engine)

class ENGINE_DLL CImageEffect final : public CEffect
{
private:
	explicit CImageEffect(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CImageEffect(const CEffect& _rhs);
	virtual ~CImageEffect() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct_Clone(void* _pArg);
	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render();

public:
	virtual HRESULT Set_Texture(_wstring strFileName) override { return S_OK; }
	HRESULT Set_Texture(CTexture* pTexture);

	_uint Get_TextureIndex() { return m_iTextureIndex; }
	_uint Get_NoiseIndex() { return m_iNoiseIndex; }

	void Set_TextrueIndex(_uint iIndex) { m_iTextureIndex = iIndex; }
	void Set_NoiseIndex(_uint iIndex) { m_iNoiseIndex = iIndex; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	_float4x4			m_ProjMatrix;
	_float				m_fX, m_fY, m_fSizeX, m_fSizeY = 0.f;

public:
	static CImageEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CEffect* Clone(void* _pArg) override;
	virtual void Free() override;
};

END