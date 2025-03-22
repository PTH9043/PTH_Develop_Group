
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

class CUI_HP final : public CUI_Base
{

protected:CUI_HP(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CUI_HP(const CUI_HP& _rhs);
	virtual ~CUI_HP() = default;

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
	void HP(const _double& _dTimeDelta);
	void Skil(const _double& _dTimeDelta);
	void Skil_2P(const _double& _dTimeDelta);
	void HP_Effect(const _double& _dTimeDelta);
	void Timer(const _double& _dTimeDelta);
	void Round(const _double& _dTimeDelta);
	void Reset_Round(const _double& _dTimeDelta);
	void Load_SelectIndex();

public:
	_bool Get_Start() { return m_bPlayerEff; }
	

private: //HP
	_float4x4			m_ProjMatrix;
	
	_float			    m_fDamageEnd = 0.f; //시작하는 데미지
	_float				m_fDamageMax = 0.f; //최종 데미지
	_float				m_fDamagePercent = 0.f;//총 데미지 퍼센트
	_bool				m_bMinusDamage = true; //데미지 감소
	_float				m_fDamageTime = 0.f; //데미지 감소 딜레이
	
	_float				m_fDamage = 0.f; //받은 데미지 
	_float				m_fMaxHP = 100.f; //최대 체력
	_float			    m_fHP = 100.f; //현재 체력 - 데미지 		
	_float				m_fHPPercent = 0.f; //체력 퍼센트

private: //Skil
	_float				m_fSkilCost = 20.f; //스킬사용게이지
	_float				m_fNowSkil = 100.f; //현재 스킬게이지
	_float				m_f2PNowSkil = 100.f; //현재 스킬게이지
	_float				m_fMaxSkil = 100.f; //최대 스킬게이지
	_float				m_fSkilPercent = 0.f;//현재 스킬게이지 비율
	_double				m_Delay = 0.f; //스킬게이지 회복 딜레이

	_float fPlayerHP = 0.f;
	_float fPlayerOriginHP = 0.f;
	_float fAIHP = 0.f;
	_float fAIOriginHP = 0.f;
	
private: //HPEffect
	_float2 EffDir = { -3.f,3.f };
	_bool	m_bPlayerEff = false;
	_bool	m_bAIEff = false;
	_float	EffTime = 0.f;
	_float2 OrginPos = { 0.f,0.f };
	_float  TickFrame = 0.f;
	_float	AlphaTime = 0.f;

	//save용
	_int PlayerIndex = 0;
	_int AIIndex = 0;

	//제한시간
	_int m_CountNumber10 = 9; //10의자리 
	_int m_CountNumber1 = 9; //1의자리 숫자
	_int m_Number = 99; //총 타이머 시간 
	_float m_RoundTime = 0.f;
	_double m_StartTime = 0.0;

	//대사
	_int TextIndex = 0;
	_double TextTimeAcc = 0.0;
	_int TextTime = 0;
	_bool TextRender = false;

	_bool StartBattle = false;
	_int  RoundCount = 0;
	_bool RoundRender = false;

	_bool Shack = false;
	_bool RoundSize = false;
	_bool EndRound = false;

	_bool SoundPlay = false;
	_bool SoundPlay2 = false;

private:
	CGameObject* m_pPlayerObject = nullptr;
	CGameObject* m_pAIObject = nullptr;
	CUI_HP* Start = nullptr;
	CPlayer* pPlayer = nullptr;
	CPlayer* pAI = nullptr;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_HP* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
