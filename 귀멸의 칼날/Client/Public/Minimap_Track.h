
#pragma once
#include"Client_Defines.h"
#include "UI_Base.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CMinimap_Track final : public CUI_Base
{

protected:CMinimap_Track(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CMinimap_Track(const CMinimap_Track& _rhs);
		 virtual ~CMinimap_Track() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

public:
	void SetWorldPos(_vector pos) { XMStoreFloat4(&m_WorldPos, pos); }


private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4			m_ProjMatrix;
	_float				m_DeadTime = 0.f;
	_float4				m_WorldPos{ 0.f,0.f,0.f,1.f };

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMinimap_Track* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END

