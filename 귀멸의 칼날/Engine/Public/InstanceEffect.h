#pragma once

#include "Effect.h"

BEGIN(Engine)

class ENGINE_DLL CInstanceEffect final : public CEffect
{
private:
	explicit CInstanceEffect(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CInstanceEffect(const CEffect& _rhs);
	virtual ~CInstanceEffect() = default;

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
	INSTANCEDESC Get_InstanceEffectDesc();

	void Set_TextrueIndex(_uint iIndex) { m_iTextureIndex = iIndex; }
	void Set_InstanceEffectDesc(INSTANCEDESC tEffectDesc);

	HRESULT ChangeInstanceCount(_uint iCount);
	void InstanceBufferReset();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	//_uint m_iTextureIndex = 0;
	_float4x4 m_PrevWorldMatrix;
public:
	static CInstanceEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CEffect* Clone(void* _pArg) override;
	virtual void Free() override;
};

END