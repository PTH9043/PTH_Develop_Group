#pragma once
#include "Client_Defines.h"
#include "CGameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
class CTransform;
END

BEGIN(Client)
class CParticle;
class C2DEffectParticle;

class CEffectParticleGroup : public CGameObject
{
public:
	typedef struct tagEffectParticleGroup
	{
		tagEffectParticleGroup() = default;
		tagEffectParticleGroup(const _wstring& _str2DEffectParticle, const _wstring& _strParticlePath)
			: str2DEffectPath(_str2DEffectParticle), strPatriclePath(_strParticlePath)
		{}
		_wstring str2DEffectPath = L" ";
		_wstring strPatriclePath = L" ";
	}EFFECTPARTICLEGROUP;

	typedef struct tagEffectParticleGroupDesc
	{
		tagEffectParticleGroupDesc(CTransform* _pTransform = nullptr, const _float3& _v3Pos = _float3(0.f, 0.f, 0.f))
			: pTransform(_pTransform), v3SupplementPos(_v3Pos)
		{}
		CTransform* pTransform = nullptr;
		_float3	  v3SupplementPos;
	}EFFECTPARTICLEGROUPDESC;
protected:
	explicit CEffectParticleGroup(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg = nullptr);
	explicit CEffectParticleGroup(const CEffectParticleGroup& _rhs);
	virtual ~CEffectParticleGroup() = default;

public:
	void Set_Pos(_vector vPos);
	void Set_Look(_fvector vLook);
	void Set_OwnerTransform(CTransform* _pOwnerTransform) { this->m_pOwnerTransform = _pOwnerTransform; Safe_AddRef(m_pOwnerTransform); }

	C2DEffectParticle* Get_2DEffectParticle() { return m_p2DEffectParticle; }
	CParticle* Get_Particle() { return m_pParticle; }

	void Turn_Off_LifeLoop();
	void Turn_On_LifeLoop();
	void Turn_OffLoop(const _float& _fLifeTime);
	void Turn_OnLoop();
	void Not_Reset(const _bool& _bIsNotReset);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

	void Active(const bool& _bTrue);
	void Reset();
protected:
	HRESULT Ready_Comp();
private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CParticle* m_pParticle = nullptr;
	C2DEffectParticle* m_p2DEffectParticle = nullptr;
	CTransform* m_pOwnerTransform = nullptr;

	_float3			m_v3SupplementPos = _float3(0.f, 0.f, 0.f);
private:
	EFFECTPARTICLEGROUP m_tEffectParticleGroup;

public:
	static CEffectParticleGroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void *_pArg = nullptr);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END