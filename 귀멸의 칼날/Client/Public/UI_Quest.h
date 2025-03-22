
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

class CUI_Quest final : public CUI_Base
{

protected:
	explicit CUI_Quest(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CUI_Quest(const CUI_Quest& _rhs);
	virtual ~CUI_Quest() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

public:
	void Effect_Story_MainQuest(const _double& _dTimeDelta);
	void Effect_Rattan_MainQuest(const _double& _dTimeDelta);
	void Quest_Text(const _double& _dTimeDelta);

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4			m_ProjMatrix;
	_int				TextureIndex = 0;
	_int				BurgerTextureIndex = 7;
	_double				m_AlphaTime = 0.0;
	_float				m_PlusAlpha = 0.f;
	_bool				m_NPCChoice = false;
	
	_bool SoundPlay = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Quest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END

