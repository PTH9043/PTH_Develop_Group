#pragma once
#include"Client_Defines.h"
#include "UI_Base.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_Boss final : public CUI_Base
{

protected:CUI_Boss(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_Boss(const CUI_Boss& _rhs);
		 virtual ~CUI_Boss() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	//virtual void Extern_ImGuiClass() override;

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

public:
	void HP(const _double& _dTimeDelta);
	void HP_Effect(const _double& _dTimeDelta);
	void Skil(const _double& _dTimeDelta);

private:
	_float4x4			m_ProjMatrix;

	//HP
	_float	m_fPlayerOriginHP = 0.f;
	_float	m_fPlayerHP = 0.f;
	_bool   m_bPlayerEff = false;

	_float	m_fOniOriginHP = 0.f;
	_float	m_fOniHP = 0.f;
	_bool   m_bOniEff = false;
	
	_float				m_fHPPercent = 0.f; //체력 퍼센트
	_float				m_fDamage = 0.f; //받은 데미지 
	_float				m_fMaxHP = 100.f; //최대 체력
	_float			    m_fDamageEnd = 0.f; //시작하는 데미지
	_bool				m_bMinusDamage = true; //데미지 감소
	_double				m_dDamageTime = 0.f; //데미지 감소 딜레이
	_float				m_fDamagePercent = 0.f;//총 데미지 퍼센트


	//HPEffect
	_float2 EffDir = { -3.f,3.f };
	_float  TickFrame = 0.f;
	_float2 OrginPos = { 0.f,0.f };
	_float	EffTime = 0.f;
	_float	AlphaTime = 0.f;


private: //Skil
	_float				m_fSkilCost = 20.f; //스킬사용게이지
	_float				m_fNowSkil = 100.f; //현재 스킬게이지
	_float				m_fMaxSkil = 100.f; //최대 스킬게이지
	_float				m_fSkilPercent = 0.f;//현재 스킬게이지 비율
	_double				m_Delay = 0.f; //스킬게이지 회복 딜레이

private:
	CGameObject* m_pPlayerObject = nullptr;
	CGameObject* m_pOniObject = nullptr;
	CPlayer* pPlayer = nullptr;
	CPlayer* pOni = nullptr;

	
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
