#pragma once

#include "Effect.h"

BEGIN(Engine)

class ENGINE_DLL CStaticMeshEffect final : public CEffect
{
private:
	explicit CStaticMeshEffect(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CStaticMeshEffect(const CStaticMeshEffect& _rhs);
	virtual ~CStaticMeshEffect() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct_Clone(void* _pArg);
	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render();

public:
	virtual HRESULT Set_Texture(_wstring strFileName) override { return S_OK; }
	HRESULT Set_Texture(CTexture* pTexture);

public:
	_uint Get_TextureIndex() { return m_iTextureIndex; }
	_uint Get_NoiseIndex() { return m_iNoiseIndex; }
	//_uint Get_MeshIndex() { return m_iMeshIndex; }

	void Set_TextrueIndex(_uint iIndex) { m_iTextureIndex = iIndex; }
	void Set_NoiseIndex(_uint iIndex) { m_iNoiseIndex = iIndex; }
	HRESULT Set_Model(CModel* pModel, _uint iIndex);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

protected:
	//_uint m_iMeshIndex = 0;
	//_uint m_iTextureIndex = 0;
	//_uint m_iNoiseIndex = 0;
	_float m_fAuraTime = 0.f;
	_int m_iMaskIdnex = 0;
	_float3 m_fTemp = _float3(0.f, 1.f, 0.f);
	_float4x4 m_PrevWorldMatrix;
public:
	static CStaticMeshEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CEffect* Clone(void* _pArg) override;
	virtual void Free() override;
};

END