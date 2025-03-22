
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

class CUI_Ultimate final : public CUI_Base
{

protected:CUI_Ultimate(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_Ultimate(const CUI_Ultimate& _rhs);
		 virtual ~CUI_Ultimate() = default;

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
	void	Ultimate_Gauge(const _double& _dTimeDelta);
	void    Ultimate_Number();
	void	Ultimate_FireEff(const _double& _dTimeDelta);
	void	Ultimate_FrameEff(const _double& _dTimeDelta);
	
private: 
	_float4x4			m_ProjMatrix;
	
	_float				m_MaxGauge = 100.f; //전체 게이지
	_float				m_NowGauge = 0.f; //현재 게이지
	_float				m_NowPercent = 0.f;//현재 게이지 퍼센트
	
	_int				m_Number = 0;
	_int				m_iPrevNumber = 0;
	_float				m_fFrame = 0;
	_bool				m_Minus_NowGauge = false;
	
	_bool				m_FrameEffect = false;
	_bool				m_Frame2Effect = false;
	_bool				m_NumberEffect = false;

	_float				TickFrame = 0.f;
	_int				m_OriginNumber = 0;
	_int				m_ComboNumber = 0;

private: //2플레이어 
	_int				m_2PNumber = 0;
	_int				m_2PComboNumber = 0;
	_int				m_2POriginNumber = 0;
	_float				m_2PNowGauge = 0.f; //현재 게이지

private:
	CGameObject* m_pPlayerObject = nullptr;
	CGameObject* m_p2PlayerObject = nullptr;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Ultimate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END

