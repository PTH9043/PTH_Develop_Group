
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

class CUI_Minimap final : public CUI_Base
{

protected:CUI_Minimap(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_Minimap(const CUI_Minimap& _rhs);
		 virtual ~CUI_Minimap() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	virtual void Extern_ImGuiClass() override;


private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

public:
	void Map_Move();
	void Map_Track();
	_float2 Make_UIPos_Form_WorldPos(_vector pos);
	void Turn_PlayerIcon();


private:
	_float4x4			m_ProjMatrix;
	_float				m_MapX = 0.543f;
	_float				m_MapY = 0.813f;

	_float4				m_PrevPos = { 0.f,0.f,0.f,0.f };

	_float4				m_MoveDist = { 0.f,0.f,0.f,0.f };
	_float				m_MoveRatio = 0.5f;//0.001f;//¡Ÿ¿Ã¿⁄

	_float				m_TickFrame = 0.f;
	_float test = 0.f;
	_float test2 = 0.f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Minimap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
