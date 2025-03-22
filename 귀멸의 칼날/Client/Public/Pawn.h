#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CPawn  abstract  : public CGameObject
{
public:
	enum PAWN_TYPE
	{
		PAWN_CHARACTER, PAWN_ITEM,  PAWN_WORLDOBJ, PAWN_END
	};
protected:
	explicit CPawn(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CPawn(const CPawn& _rhs);
	virtual ~CPawn() = default;
public:
	const PAWN_TYPE& Get_PawnType() const { return m_ePawnType; }

	void Set_RenderEnable(const _bool& _bIsRenderEnable) { this->m_bRenderEnable = _bIsRenderEnable; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderLight() override;
	// DamageOwner
	virtual _bool Hit_Pushed(CPawn* _pDamageOwner, VOIDDATAS* _pOtherDatas = nullptr);
protected:
	// 충돌 상황 정의
	virtual HRESULT Collision_Situation(const _double& _dTimeDelta);
protected:
	virtual HRESULT SetUp_ConstantTable();
protected:
	// SetUp Components
	virtual HRESULT SetUp_Components();
	// Coll Init
	virtual void Coll_Init();
	// Coll Tick
	virtual void Coll_Tick(_matrix _WorldMatrix);
#ifdef _DEBUG
	virtual void Coll_Render();
	virtual void Coll_Renderer();
#endif
protected:
	// Model
	CModel*			m_pModelCom = nullptr;
	// Renderer 
	CRenderer*		m_pRendererCom = nullptr;
	// ShaderCom
	CShader*		m_pShaderCom = nullptr;
	// Collider Com
	CCollider*		m_pColliderCom = nullptr;
	// Pawn Type
	PAWN_TYPE		m_ePawnType = PAWN_END;
	// Prev World Matrix
	_float4x4		m_PrevWorldMatrix;
	// SamplerType
	_bool			m_bWrapSampler = false;
	_bool			m_bRenderEnable = true;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END
