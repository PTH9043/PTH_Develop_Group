#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CTexture;
END

BEGIN(Client)
class CMapObject : public CPawn
{
public:
	typedef struct tagMapObjDesc
	{
		_float4x4		fMatrix;
		_tchar			ModelName[MAX_PATH] = L"";
		_float3			vSize;

	}MAPOBJDESC;
protected:
	explicit CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapObject(const CMapObject& _rhs);
	virtual ~CMapObject(void) = default;
public:
	static CMapObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* _pArg) override;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render(void) override;
	virtual HRESULT	RenderLight(void) override;
	virtual void Extern_ImGuiClass(void) override;

protected:
	// Ready Component 
	virtual HRESULT Ready_Component(void);
	// SetUp ConstantValue
	virtual HRESULT SetUp_ConstantTable(void);
public:
	virtual void Free(void) override;
protected:
	_float3			m_vMin{};
	_float3			m_vMax{};
	_bool			m_bColliderRenderCheck = false;
	CTexture*		m_pFilterTexture = nullptr;
	//_bool			m_bLightModelCheck = false;
};
END