#pragma once
#include "CEffect.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CParticle;

class CEffectGroup final : public CGameObject
{
private:
	explicit CEffectGroup(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CEffectGroup(const CEffectGroup& _rhs);
	virtual ~CEffectGroup() = default;
public:
	void Active(const _bool& _bIsActive);
	void Update_Pos(_fvector _vPos);
	void Update_Rot(const _float3& _v3Rot);
	void Update_Scale(const _float3& _v3Scale);
	void Update_Look(_fvector _vLook);
	void Set_LifeTime(const _float& _fLifeTime) { this->m_fLifeTime = _fLifeTime; }
	void Get_Collider(void** _ppOut);
	void Get_Collider(list<CCollider*>* _pOut);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(const _wstring& _strFilePath);
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

public:
	void Modify_Value(vector<CModel*>* _pModel, int* _pCount, int* _pIndex, CParticle* _pParticle);
	// Save Load
	void Save();
	void Load(const _wstring& _strPath);
private:
	vector<CEffect*>										m_EffectList;
protected:
	_wstring														m_strEffectName = L" ";
	_float																m_fLifeTime = 0.f;
	CUSTIMER														m_tLifeTimer = CUSTIMER();
	_bool																m_bLoop = false;
	_wstring														m_strPath = L"";
public:
	static CEffectGroup* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	static CEffectGroup* Create_LoadFile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _strFilePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END