
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

struct SelectInfo
{
	_int PlayerIndex = 0;
	_int AIIndex = 0;
	_int MapIndex = 0;
};


class CUI_CharSelect final : public CUI_Base
{

protected:CUI_CharSelect(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
		 explicit CUI_CharSelect(const CUI_CharSelect& _rhs);
		 virtual ~CUI_CharSelect() = default;

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
	void Smaller_Effect(_float speed, _float endtime, _double timedelta);
	void Cloud_Effect( _double timedelta);
	void Select_Char(_double timedelta);
	void Select_Map(_double timedelta);
	void Select_Save();

private:
	_float4x4			m_ProjMatrix;
	_float				m_fSpeed = 1900.f;
	_float				m_fEndTime = 0.4f;
	_float				CloudEffect = 0.f;
	_float				m_UVFlowy = 0.f;
	_float				m_UVFlowx = 0.f;
	_float				m_TimeAcc = 0.f;
	_int				m_P1_Index = 0;
	_int				m_P2_Index = 3;
	_int				m_Map_Index = 0;


	_bool				m_P1_Select = false;
	_bool				m_P2_Select = false;
	_bool				m_Map_Select = false;
	
	_bool				m_ChangeModel = false;

	_bool				m_StartBattle = false;
	_bool				m_StartTime = false;
	_double				m_StartTimeAcc = 0.0;

public:
_bool Get_StartBattle() {return m_StartBattle;}
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CUI_CharSelect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI_Base* Clone(void* pArg) override;
	virtual void Free() override;

};

END

