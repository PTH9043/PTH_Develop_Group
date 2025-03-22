#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class CEffect;
class CHierarchyNode;

class ENGINE_DLL CEffectObject final : public CGameObject
{
public:
	enum PROJECT { PROJECT_CLIENT, PROJECT_ANIMTOOL, PROJECT_TOOL, PROJECT_END };
private:
	explicit CEffectObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CEffectObject(const CEffectObject& _rhs);
	virtual ~CEffectObject() = default;

private:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double & _dTimeDelta) override;
	virtual void Late_Tick(const _double & _dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	EFFECTDESC& Get_EffectObjectDesc() { return m_tEffectDesc; }
	_uint Get_EffectCount() { return (_uint)m_vecEffect.size(); }
	vector<CEffect*>& Get_vecEffect() { return m_vecEffect; }
	CEffect* Get_Effect(_uint iCount) { return m_vecEffect[iCount]; }
	_bool Get_IsDelay() { return m_bDelay; }
	_uint Get_ProjectType() { return m_eCurProject; }
	_wstring Get_Name() { return m_strName; }
	_bool Get_Play() { return m_tEffectDesc.bPlay; }
	CTransform* Get_OwnerTransform() { return m_pOwnerTR; }
	_float3 Get_OwnerPos() { return m_vOwnerPos; }
	_float4x4 Get_BoneMatrix() { return matBoneParent; }
	_int Get_Mode() { return m_iMode; }
	_int Get_ToolCurEffect() { return m_iCurEffectCount; }
	_wstring Get_FileName() { return m_strFileName; }
	_bool Get_LoopState() { return m_tEffectDesc.bLoop; }

	void Set_Name(wstring _strName) { m_strName = _strName; }
	void Set_Name(_tchar * _strName) { m_strName = _strName; }
	void Set_EffectObjectDesc(EFFECTDESC tEffectDesc);
	void Set_Play(_bool bState) { m_tEffectDesc.bPlay = bState; }
	void Set_Loop(_bool bState) { m_tEffectDesc.bLoop = bState; }
	void Set_Pause(_bool bState) { m_tEffectDesc.bPause = bState; }
	HRESULT SetUP_Matrix(CHierarchyNode * pNode, CTransform * pOwnerTransform, _float fValue, _float4 vValue, _float3 vDir, _float3 vLook = _float3(0.f,0.f,0.f));
	void Set_ProjectType(PROJECT eType) { m_eCurProject = eType; }
	void Set_ToolCurEffect(_int iCount) { m_iCurEffectCount = iCount; }
	void Set_FileName(_wstring _FileName) { m_strFileName = _FileName; }

public:
	HRESULT Add_Effect(CEffect * pEffect);
	void EffectObjectUpdate();
	void EffectObjectReset();

public:
	void Billboard();
	void StateChange(const _double& _dTimeDelta);
	// 특정축으로 특정속도 만큼 계속 회전
	void RotationLoop();
	// 즉정 각도만큼 회전
	void RotationAngle();
	void SetEffectState(_fmatrix _matWorld);
	void Set_TargetTransform(CTransform* pTarget);

private:
	PROJECT m_eCurProject = PROJECT_END;
	CHierarchyNode* m_pHierarchyNode = nullptr;
	CTransform* m_pEffectObjectTR = nullptr;
	CTransform* m_pOwnerTR = nullptr;
	CTransform* m_pTargetRT = nullptr;
	_float4x4 matBoneParent;
	_wstring m_strName = L"";
	_wstring m_strFileName = L"";
	vector<CEffect*> m_vecEffect;
	EFFECTDESC m_tEffectDesc;
	_float m_fTime = 0.f;
	_bool m_bDelay = false;
	_bool m_bAddRot = true;
	_float fDeltaValue = 0.f;
	_float3 vRotOrigin;
	_float3 m_vAnimDir = _float3(0.f, 0.f, 0.f);
	_float3 m_vRootBoneLook = _float3(0.f, 0.f, 0.f);
	_float3 m_vTargetDir = _float3(0.f, 0.f, 0.f);
	_bool m_bFollow = false;
	_bool m_bReverse = false;
	_float m_fAngleValue = 0.f;
	_int m_iMode = 0;
	_bool m_bFirst = true;
	_float m_fPauseVelau = 1.f;
	_float3 m_vOwnerPos = _float3(0.f, 0.f, 0.f);
	_bool m_bDontBind = false;
	_float4 m_vMode3Value = _float4(0.f, 0.f, 0.f, 0.f);
	_int							m_iCurEffectCount = 0;
	_float3					m_vRandomDir = _float3(0.f, 0.f, 0.f);

	CTransform*		m_pInitTransform = nullptr;
	_float4x4				m_SetUpOwnerTransform = _float4x4();
public:
	static CEffectObject* Create(ID3D11Device * _pDevice, ID3D11DeviceContext * _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free() override;
};

END