#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel_Instance;
class CCollider;
class CTexture;
END

BEGIN(Client)
class CMapGroupObject : public CPawn
{
public:
	typedef struct tagMapObjDesc
	{
		MATRIXES Matrix;
		_tchar	ModelName[MAX_PATH] = L"";
		_uint	InstanceCount = 0;
		_float3 ColliderSize{};

	}MAPOBJDESC;
protected:
	explicit CMapGroupObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CMapGroupObject(const CMapGroupObject& _rhs);
	virtual ~CMapGroupObject(void) = default;
public:
	static CMapGroupObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
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
	_float3				m_vMin{};
	_float3				m_vMax{};
	_bool				m_bColliderRenderCheck = false;
	_bool				m_bIsFilter = true;
private:
	//_uint				m_iInstanceCount = 0;
	CModel_Instance*	m_pModelInstanceCom = nullptr;
	//_float3				m_vColliderSize{};
	vector<_tchar*>		m_vecColliderName;
	vector<CCollider*>	m_vecCollider;
	MAPOBJDESC			m_ObjDesc;
	CTexture*			m_pFilterTexture = nullptr;
};
END