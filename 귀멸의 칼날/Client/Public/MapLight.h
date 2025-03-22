#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)
class CMapLight : public CPawn
{
public:
	typedef struct tagMapLightDesc
	{
		_float3 vScale = _float3(1.f, 1.f, 1.f);
		_float3 vPos = _float3(0.f, 0.f, 0.f);
		_float3 vRot = _float3(0.f, 1.f, 0.f);
		_float fAngle = 0.f;
	}MAPLIGHTDESC;

protected:
	explicit CMapLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapLight(const CMapLight& _rhs);
	virtual ~CMapLight(void) = default;
public:
	static CMapLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* _pArg) override;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
	virtual void Extern_ImGuiClass(void) override;
protected:
	// Ready Component 
	virtual HRESULT Ready_Component(void);
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable(void);
private:
	MAPLIGHTDESC m_tMapLight;

public:
	virtual void Free(void) override;

};
END