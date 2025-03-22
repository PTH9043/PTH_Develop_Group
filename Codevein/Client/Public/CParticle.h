#pragma once
#include "Client_Defines.h"
#include "CGameObject_Client.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Cube_Instance;
class CTransform;
END

BEGIN(Client)

class CParticle : public CGameObject
{
public:
	typedef struct tagParticleInfo
	{
		_uint												iPassIndex = 0;
		CVIBuffer_Cube_Instance* pVIBufferCom = nullptr;
	}PARTICLEINFO;
protected:
	explicit CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CParticle(const CParticle& _rhs);
	virtual ~CParticle() = default;
public:
	const _bool& Is_Loop() const { return m_bLoop; }
	void Set_Loop(const _bool& _bIsLoop) { this->m_bLoop = _bIsLoop; }

	const _wstring& Get_ParticleName() const { return m_strPaticleName; }

	void Set_OwnerTransform(CTransform* _pOwnerTransform);

	void Set_OwnerInitPos(_fvector vPos) { XMStoreFloat3(&m_v3OwnerInitPos, vPos); }
	void Set_OwnerInitRot(_fvector vPos) { XMStoreFloat3(&m_v3OwnerInitRot, vPos); }
	void Set_OwnerFadeOut(const _float& _fFadeOut) { this->m_fOwnerFadeOut = _fFadeOut; }
	void Set_LifeTime(const _float& _fLifeTime) {
		this->m_fLifeTime = _fLifeTime; m_tLifeTimer = CUSTIMER(_fLifeTime); m_tLifeTimer.Reset_Timer();
	}
	void Turn_Off_LifeLoop();
	void Turn_On_LifeLoop();
	void Set_NotReset(const _bool& _bIsNotReset) { m_bNotReset = _bIsNotReset; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(const _wstring& _strFileName);
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
	 HRESULT Ready_Comp();
	 HRESULT SetUp_Constant();
private:
	CTransform*												m_pOwnerTransform = nullptr;
private:
	CShader*														m_pShaderCom = nullptr;
	CRenderer*													m_pRendererCom = nullptr;
	// Effect Desc
	vector< PARTICLEINFO>							m_ParticleList;
protected:
	_wstring														m_strPaticleName = L"";
	_float																m_fLifeTime = 0.f;
	CUSTIMER														m_tLifeTimer = CUSTIMER();
	_bool																m_bLoop = false;
	_bool																m_bNotReset = true;
	_float3															m_v3OwnerInitPos = _float3(0.f, 0.f, 0.f);
	_float3															m_v3OwnerInitRot = _float3(0.f, 0.f, 0.f);
	_float3															m_v3Scale = _float3(0.f, 0.f, 0.f);
	_float3															m_v3Rot = _float3(0.f, 0.f, 0.f);
	// Owner fade Out
	_float																m_fOwnerFadeOut = 0.f;
public:
	static CParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CParticle* Create_LoadFile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _strFileName);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END