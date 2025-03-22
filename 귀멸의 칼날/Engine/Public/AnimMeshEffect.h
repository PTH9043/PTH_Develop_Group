#pragma once

#include "Effect.h"

BEGIN(Engine)

class ENGINE_DLL CAnimMeshEffect final : public CEffect
{
private:
	explicit CAnimMeshEffect(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	explicit CAnimMeshEffect(const CEffect& _rhs);
	virtual ~CAnimMeshEffect() = default;

private:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct_Clone(void* _pArg);
	virtual void Tick(const _double& _dTimeDelta);
	virtual void Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	virtual HRESULT Set_Texture(_wstring strFileName) override { return S_OK; }
	HRESULT Set_Texture(CTexture* pTexture);

public:
	_uint Get_TextureIndex() { return m_iTextureIndex; }
	_uint Get_MeshIndex() { return m_iMeshIndex; }

	void Set_TextrueIndex(_uint iIndex) { m_iTextureIndex = iIndex; }
	HRESULT Set_Model(CModel* pModel, _uint iIndex = 0);

private:
	_uint m_iMeshIndex = 0;
	_uint m_iTextureIndex = 0;

public:
	static CAnimMeshEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CEffect* Clone(void* _pArg) override;
	virtual void Free() override;
};

END