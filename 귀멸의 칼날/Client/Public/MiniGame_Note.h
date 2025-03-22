
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

class CMiniGame_Note final : public CUI_Base
{
public:
	enum NoteType { KEY_Y, KEY_B, KEY_A, KEY_X, EFFECT_Y, EFFECT_B, EFFECT_A, EFFECT_X,
		EFFECT_GREAT, EFFECT_GOOD, EFFECT_RING, EFFECT_STAR, KEY_END}; //초기 위치를 변경
	//EFFECT_GREAT : 큰링 
	//EFFECT_GOOD : 작은별 
	//EFFECT_STAR : 큰별
	//EFFECT_Ring : 작은링

protected:
	explicit CMiniGame_Note(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CMiniGame_Note(const CMiniGame_Note& _rhs);
	virtual ~CMiniGame_Note() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;


public:
	void Setting_Positon(_uint Type);//KeyType에 따라 초기 값을 설정 
	void Tick_Event(const _double& _dTimeDelta); //계속 반복적으로해야하는 조건들
	void Bezier(const _double& _dTimeDelta); //베지어 곡선 
	void Great_Eff(const _double& _dTimeDelta);//great이거나 good일때 나오는 효과 
	
private:
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

private:
	_float4x4			m_ProjMatrix;
	_uint m_iType = KEY_END;
	_float2 Note[3]; //시작점,제어점,도착점 

	_float m_t = 0.f;
	_double m_time = 0.0;
	_double deletetime = 0.0;

	_int type = KEY_END;
	_float3 m_RGBColor = { 0.f,0.f,0.f };
	
	_bool GreatEff = false;
	_int Efftype = KEY_END;
	_double EffTime = 0.0;
	_bool CreatEff = false;
	_double timeeff = 0.0;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMiniGame_Note* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END
