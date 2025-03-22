
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

class CUI_MiniGame final : public CUI_Base
{

public:
	enum RESULT_Type { RESULT_GREAT, RESULT_GOOD, RESULT_BAD, RESULT_END}; //판정 타입 

protected:CUI_MiniGame(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_MiniGame(const CUI_MiniGame& _rhs);
		 virtual ~CUI_MiniGame() = default;

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

private:
	_float4x4			m_ProjMatrix;

public:
	void Setting_Postion();
	void Count_Number(const _double& _dTimeDelta);
	void Gauge(const _double& _dTimeDelta);
	void KeyButton(const _double& _dTimeDelta);
	void Result(_uint Result_type);
	void Score_Plus(_float Gauge, _int score);
	void Combo_Plus(_int combo);
	void Combo();
	void Dialog(const _double& _dTimeDelta);

private:
	_int m_CountNumber10 = 4; //10의자리 
	_int m_CountNumber1 = 0; //1의자리 숫자
	_int m_Number = 60; //총 타이머 시간 
	_double m_Time = 0.0; //타이머 깎일 시간
	_double m_StartTime = 0.0; //시작타이머 깎일 시간
	
	_tchar	m_szFONTS[MAX_PATH] = TEXT("");
	_int	m_Score = 0; //스코어 점수 
	_int    m_StartNumber = 0; //처음 시작할때의 숫자 
	_bool   m_Grow = false; //커졌다 작아지는 효과

	_float m_NowGauge = 0.f;//현재 게이지바 
	_float m_MaxGauge = 100.f;//최대 게이지바
	_float m_NowPercent = 0.f; //현재 게이지 퍼센트
	_float3 m_RGBColor = { 0.f,0.f,0.f };
	
	_double m_RandomKey = 0.0;
	_uint Resulttype = RESULT_END;
	_bool Result_Eff = false;
	_double Result_time = 0.0;

	_int m_ComboNumber10 = 0; //10의자리 
	_int m_ComboNumber1 = 0; //1의자리 숫자
	_int m_Combo = 0;
	_int ComboNum = 0;

	_int DiaTexture = 0;
	_double DiaTime = 0.0;
	_double Dia = 0.0;

	_bool Return_Rattan = false;

	_bool SoundPlay = false;


public:
	void Set_Result(_uint type) { Resulttype = type; }
	void Set_ResultEff(_bool Eff) { Result_Eff = Eff; }
	_int Get_Combo() { return m_Combo; }
	_bool Get_Return() { return Return_Rattan; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_MiniGame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
