#pragma once
#include "CIMGUI.h"

BEGIN(Engine)
class CTexture;
class CModel;
END

BEGIN(Client)
class C2DEffectParticle;
class CParticle;
class CEffectGroup;

class CEffect_ParticleTool final : public CIMGUI
{

private:
	explicit CEffect_ParticleTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CEffect_ParticleTool() = default;

public:
	const _bool& Is_Open() const { return m_bOpen; }
	void Set_Open(const _bool& _bOpen) { m_bOpen = _bOpen; }
	// Get Name
	const char* Get_Name() const { return m_pName; }
public:
	virtual HRESULT NativeConstruct();
	virtual HRESULT NativeConstruct_Tool();
	virtual HRESULT Tick(const _double& _dDeltaSeconds);
	virtual HRESULT Render();
	virtual HRESULT Release_Tool();
protected:
	virtual void Show_Window() override;
private:
	// Effect
	C2DEffectParticle*		m_p2DEffectParticle = nullptr;
	CParticle*						m_pParticle = nullptr;
	CEffectGroup*				m_pEffect = nullptr;
	CTexture*						m_pTexture = nullptr;
	vector<CModel*>		m_ModelList;

	_float3  m_v3Pos = _float3(0.f, 0.f, 0.f);

    _float m_fRanRandomForceMultiply = 1.f;
    _float m_fRanRandomAccelerationMultiply = 1.f;
    _float m_fRanJumpValueMultiply = 1.f;
    _float m_fRanJumpTimeMultiply = 1.f;
    _float m_fRanJumpTimeFastMultiply = 1.f;
    _float m_fRanParticleColorMultiply = 1.f;
    _float m_fRanBlendColorMultiply = 1.f;
    _float m_fRanBlendSpeedMultiply = 1.f;
    _float m_fRanRotSpeedMultiply = 1.f;
    _float m_fRanvDirectionMultiply = 1.f;
    _float m_fRanSizeMultiply = 1.f;
    _float m_fRanInitPosMultiplyX = 1.f;
	_float m_fRanLifeTime = 1.f;
	_float m_fRanDirectionMultiplyY = 1.f;
	_float m_fRanDirecitonMultiplyZ = 1.f;
	_float m_fRanInitPosMultiplyY = 1.f;
	_float m_fRanInitPosMultiplyZ = 1.f;


	EFFECTPARTICLEDESC m_tParticleEffectDesc;
	_uint	m_iCreateCount = 0;
	int m_iSelectIndex = 0;
	int m_iSelectInstanceIndex = 0;

	_bool	m_bActivePaticleTool = false;
	_bool m_bActive2DEffectParitlceTool = false;
	_bool m_bActiveEffectTool = false;

	_char m_p2DEffectLoadName[MAX_PATH] = "";
	_char m_pParticleLoadName[MAX_PATH] = "";
	_char m_pEffectLoadName[MAX_PATH] = "";

	_int m_iSelectIndex_EffectTool = 0;
	_int m_iSelectInstanceindex_EffectTool = 0;

	_bool m_bAllRender = false;
	_bool m_bRender_2DEffect = false;
	_bool m_bRender_Particle = false;
	_bool m_bRender_Effect = false;
public:
	static CEffect_ParticleTool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END