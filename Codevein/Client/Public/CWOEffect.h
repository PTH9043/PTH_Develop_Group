#pragma once
#include "CWorldObj.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CVIBuffer_Terrain;
class CLight;
END


BEGIN(Client)
class CEffectParticleGroup;

class CWOEffect : public CGameObject_Client
{
public:
	typedef struct tagWOEffectDesc
	{
		_uint							iProtoSelectIndex = 0;
		_wstring					strPrototype = L"";
		GOBCLIENTDESC		tGobClientDesc;
	}WODEFFECTDESC;
protected:
	explicit CWOEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWOEffect(const CWOEffect& _rhs);
	virtual ~CWOEffect() = default;
public:
	virtual const char* Get_Name() override;
	virtual const _tchar* Get_Proto() override { return PROTO_GOB_WOEFFECT; }
	virtual const _tchar* Get_Layer() override { return LAYER_WORLDEFFECT; }
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
protected:
	// Ready Component 
	virtual HRESULT Ready_Component();
	// SetUp ConstantValue
	HRESULT SetUp_ConstantTable();
protected:
	CEffectParticleGroup* m_pEffectParticleGroup = nullptr;
	_uint m_iSelectIndex = 0;
	CLight* m_pLight = nullptr;
	LIGHTDESC m_tLightDesc;
public:
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue() override;
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str) override;
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut) override;
	// Delete Self
	virtual HRESULT Delete_Self() override;
	// Save Data 
	virtual HRESULT Save_Data() override;
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr) override;
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr) override;
	// Picking Nav 
	_bool Is_PickingNav();
	// Reset Data 
	virtual void Reset_Data() override;
#pragma endregion USE_TOOL_METHOD
private:
public:
	static CWOEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CWOEffect* Clone(void* _pArg) override;
	virtual void Free() override;
};

END