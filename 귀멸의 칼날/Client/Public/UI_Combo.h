	
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

class CUI_Combo final : public CUI_Base
{

protected:CUI_Combo(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_Combo(const CUI_Combo& _rhs);
		 virtual ~CUI_Combo() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;


private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

public:
	void Combo_Number(const _double& _dTimeDelta);
	void Combo_Gauge(const _double& _dTimeDelta);


	
private: 
	_float4x4			m_ProjMatrix;

	_float				m_ComboEnd = 5.f;
	_int				m_Number = 0;
	_bool				m_Grow = false;
	
	_int				m_Number10 = 0;
	_int				m_Number1 = 0;

	_float				m_Angle = 0.f;
	_float				m_NowAngle = 179.f;
	_float				m_AngleMax = -180.f;
	_bool				m_ComboGauge = false;

	_float  TickFrame = 0.f;
	_int	m_OriginNumber = 0;

private: //2P²¨
	
	_int				m_2PNumber = 0;
	_int	m_2POriginNumber = 0;

	

private:
	CGameObject* m_pPlayerObject = nullptr;
	CGameObject* m_p2PlayerObject = nullptr;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Combo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END

