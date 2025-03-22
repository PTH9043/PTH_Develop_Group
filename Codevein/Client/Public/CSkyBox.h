#pragma once
#include "Client_Defines.h"
#include "CGameObject.h"

BEGIN(Engine)
class CVIBuffer_Cube;
class CShader;
class CRenderer;
class CTexture;
END

BEGIN(Client)

class CSkyBox final : public CGameObject
{
public:
	typedef struct tagSkyBoxDesc
	{
		_wstring strSkyBoxName = L"";

	}SKYBOXDESC;
protected:
	explicit CSkyBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CSkyBox(const CSkyBox& _rhs);
	virtual ~CSkyBox() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	HRESULT Ready_Component();
	HRESULT SetUp_SRV();
protected:
	CVIBuffer_Cube*	m_pVIBufferCube = nullptr;
	CShader*					m_pShaderComp = nullptr; 
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*					m_pTexture = nullptr;

protected:
	SKYBOXDESC			m_tSkyBoxDesc;
public:
	static CSkyBox* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END