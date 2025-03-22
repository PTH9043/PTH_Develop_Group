#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CNavigation;
END

BEGIN(Client)
class CMapGround : public CPawn
{
public:
	enum GROUNDTYPE { GROUNDTYPE_GROUND, GROUNDTYPE_SKY, GROUNDTYPE_END };
protected:
	explicit CMapGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapGround(const CMapGround& _rhs);
	virtual ~CMapGround(void) = default;
public:
	static CMapGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* _pArg) override;
public:
	virtual HRESULT NativeConstruct_Prototype(void) override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
	virtual HRESULT	RenderLight(void) override;
	virtual void Extern_ImGuiClass() override;

protected:
	// Ready Component 
	virtual HRESULT Ready_Component(void);
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable(void);
public:
	virtual void Free(void) override;
protected:
	CNavigation* m_pNavigationCom = nullptr;
private: 
	GROUNDTYPE m_eGroundType = GROUNDTYPE_END;
	_bool m_bEffectCreateCheck = false; 
	_uint m_iMapCount = 0;
	_float4		m_fogColor = _float4(0.f, 0.f, 0.f, 1.f);
};
END