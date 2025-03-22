#pragma once
#include "Client_Defines.h"
#include "CGameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect_Instance;
class CCamera;
class CTransform;
END

BEGIN(Client)

class C2DEffectParticle : public CGameObject
{
public:
	typedef struct tagEffectParticleInfo
	{
		_wstring										strTextureName = L"";
		_uint												iPassIndex = 0;
		CVIBuffer_Rect_Instance*			pVIBufferCom = nullptr;
	}EFFECTPARTICLEINFO;
protected:
	explicit C2DEffectParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit C2DEffectParticle(const C2DEffectParticle& Prototype);
	virtual ~C2DEffectParticle() = default;
public:

	const _bool& Is_Loop() const { return m_bLoop; }
	void Set_Loop(const _bool& _bIsLoop) { this->m_bLoop = _bIsLoop; }

	const _bool& Is_LookAt() const { return m_bLookAt; }
	void Set_LookAt(const _bool& _bIsLookAt) { this->m_bLookAt = _bIsLookAt; }

	const _wstring& Get_EffectParticleName() const { return m_strEffectParticleName; }

	void Set_OwnerTransform(CTransform* _pOwnerTransform);

	void Set_OwnerFadeOut(const _float& _fOwnerFadeOut) { this->m_fOwnerFadeOut = _fOwnerFadeOut; }

	void Set_LifeTime(const _float& _fLifeTime) {
		this->m_fLifeTime = _fLifeTime; m_tLifeTimer = CUSTIMER(_fLifeTime); m_tLifeTimer.Reset_Timer();
	}
	void Turn_Off_LifeLoop();
	void Turn_On_LifeLoop();
	void Set_NotReset(const _bool& _bIsNotReset) { m_bNotReset = _bIsNotReset; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(const _wstring& _strPath);
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	void Reset();
public:
	void Modify_Value(CTexture* _pTexture, int* _iSelectIndex, int* _iSelectInstanceIndex, _float3* _pPos, 
		_float* _fRandomwValue, _uint* _iCreateInstanceCount, EFFECTPARTICLEDESC* _pEffectParticleDesc);
	void Save();
	void Load(const _wstring& _strLoad);
protected:
	virtual HRESULT Ready_Comp();
	virtual HRESULT SetUp_Constant();
	HRESULT Find_Camera();
private:
	CCamera*														m_pCameraObject = nullptr;
	CShader*														m_pShaderCom = nullptr;
	CTexture*														m_pTextureCom = nullptr;
	CRenderer*													m_pRendererCom = nullptr;
	CTransform*												m_pOwnerTransform = nullptr;
	// Effect Desc
	vector< EFFECTPARTICLEINFO>				m_EffectParticleList;
protected:
	_wstring														m_strEffectParticleName = L"";
	_float																m_fLifeTime = 0.f;
	CUSTIMER														m_tLifeTimer = CUSTIMER();
	_bool																m_bLoop = false;
	_bool																m_bLookAt = false;
	_bool																m_bNotReset = true;
	_float3															m_v3Scale = _float3(0.f, 0.f, 0.f);
	_float3															m_v3Rot = _float3(0.f, 0.f, 0.f);
	// Owner fade Out
	_float																m_fOwnerFadeOut = 1.f;
public:
	static C2DEffectParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static C2DEffectParticle* Create_LoadFile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _strPath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END