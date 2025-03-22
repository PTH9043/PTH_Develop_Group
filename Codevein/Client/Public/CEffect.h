#pragma once
#include "Client_Defines.h"
#include "CGameObject_Client.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CModel;
class CTransform;
class CCollider;
class CNavigation;
END

BEGIN(Client)
class CParticle;

class CEffect : public CGameObject
{
public:
	typedef struct tagUVControl
	{
		_float2															vCurUVPos = _float2(0.f, 0.f);
		_float2															vUVEndPos = _float2(0.f, 0.f);
		_float2															vUVDirection = _float2(0.f, 0.f);
		_float2															vUVRealDireciton = _float2(0.f, 0.f);
		_float																fUvCurSpeed = 0.f;
		_float																fUvEndSpeed = 0.f;

		_float2															vResultUVPos = _float2(0.f, 0.f);
		_float2															vEndResultUVPos = _float2(0.f, 0.f);

		_bool																bUVRestrict = false;

		void Translate_Move(const _double& _dTimeDelta);
		void Reset();
	}UVCONTROL;
protected:
	explicit CEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CEffect(const CEffect& _rhs);
	virtual ~CEffect() = default;
public:
	const _bool& Is_UVControl() const { return m_bUVControl; }
	void Set_UVControl(const _bool& _bIsUVControl) { this->m_bUVControl = _bIsUVControl; }

	void Set_OwnerTransform(CTransform* _pTransform);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(const _wstring& _strFilePath);
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
public:
	void Modify_Value(vector<CModel*>* _pModel, CParticle* _pParticle);

	void Reset_Data();

	void Save(HANDLE& _hHandle, DWORD* _dwByte);
	void Load(HANDLE& _hHandle, DWORD* _dwByte);
	// Find Region
	void Find_Region();
protected:
	HRESULT Ready_Comp();
	HRESULT SetUp_Constant();
private:
	CShader*														m_pShaderCom = nullptr;
	CRenderer*													m_pRendererCom = nullptr;
	CModel*														m_pModelCom = nullptr;
	EFFECTPARTICLEDESC								m_tEffectParticleDesc;
	// Transform
	CTransform*												m_pOwnerTransform = nullptr;
	CCollider*														m_pCollider = nullptr;
	CNavigation*												m_pNavigation = nullptr;
	CParticle*														m_pParticle = nullptr;
	_float3															m_v3ParticleInitPos = _float3(0.f, 0.f, 0.f);
	_float3															m_v3ParticleInitRot = _float3(0.f, 0.f, 0.f);
protected:
	_wstring														m_strEffectPath = L"";
	_bool																m_bUVControl = false;
	_bool																m_bFloor = false;
	_bool																m_bCollider = false;
	_bool																m_bZeroAcceleration = false;
	UVCONTROL													m_tUVControl;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END