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

class CUI final : public CUI_Base
{

protected:
	explicit CUI(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CUI(const CUI& _rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	
public:
	void Logo_Effect(const _double& _dTimeDelta);
	void Logo_SmokeEffect(const _double& _dTimeDelta);
	void Loading_Effect(const _double& _dTimeDelta);
	void Mode_Effect(const _double& _dTimeDelta);

public:
	void Story_Text(const _double& _dTimeDelta);
	_bool Get_Start() { return m_bStartGame; };
	_bool Get_Mode() { return m_bModeStart; };
	_int Get_ModeIndex() { return m_MoadIndex; };

public:
	void Rattan_Text(const _double& _dTimeDelta);
	_bool Get_MiniGameStart() { return MiniGameStart; };
	_bool Get_StoryStart() { return StoryStart; };

public:
	void Eednig_Effect(const _double& _dTimeDelta);
	void UI_Sound();

private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private://로고/로딩
	_float4x4			m_ProjMatrix;
	_int				TextureIndex = 0;
	_double             deadtime = 0.f;
	_float2				v2MousePos = _float2(0.f, 0.f);
	_float				m_UVFlow = 0.f;
	_float				m_fFrame = 0.f;
	_bool				m_bLogoEffect = false;
	_bool				m_bStartGame = false;
	_float				m_LoadAni_fx = 496.6f;
	
private: //스토리모드
	_int Story_TextureIndex = 0;
	_bool Story_NpcText = false;
	_bool Story_NpcText2 = false;
	_bool Story_Stop = false;
	_bool Story_Boss = false;
	
	//모드선택
	_float				m_UVFlowy = 0.f;
	_float				m_UVFlowx = 0.f;
	_float				m_TimeAcc = 0.f;
	_int				m_MoadIndex = 0;
	_bool				m_ModeCharEff = false;
	_bool				m_ModeCharMove = false;
	_bool				m_bModeEffect = false;
	_bool				m_bModeStart = false;
	_bool				m_bModeStartEff = false;
	
private://등나무맵
	_int Rattan_ZenitsuText = 3;
	_bool MiniGameStart = false;
	_double RattanTime = 0.0;
	_bool Rattan_TextRender = false;
	_bool StoryStart = false;
	_bool KanawoRender = false;
	_bool CandidateRender = false;
	_bool GanyaRender = false;

private://엔딩
	_double	EndingTime = 0.0;
	_float  Flower_MinusY = 0.f;
	_float	Text_PlusY = 0.f;
	_int	Text_Index = 0;

	_bool SoundPlay = true;
	_bool BGMPlay = false;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
