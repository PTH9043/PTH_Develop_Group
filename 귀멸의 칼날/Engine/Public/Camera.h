#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCamreaDesc
	{
		_float3		v3Eye, v3At;
		_float			fFovy = 0.f, fAspect = 0.f, fNear = 0.f, fFar = 0.f;
		_float			fSpeed = 0.f;
		_float			fRotSpeed = 0.f;
	}CAMDESC;

	enum ECameraType
	{
		CAM_FREE, CAM_TOOL, CAM_CINEMATIC, CAM_END
	};
protected:
	explicit CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera(const CCamera& _rhs);
	virtual ~CCamera() = default;
public:
	// Get Cam name
	const _string& Get_CamName() const { return m_strCamName; }
public:
	// Out CamDesc
	void Out_CameraDesc(CAMDESC*& _pOut);
	// Update ProjMatrix
	void Update_ProjMatrix();
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void Tick(const _double& _dTimeDelta)  override;
	virtual void Late_Tick(const _double& _dTimeDelta)  override;
	virtual HRESULT  Render() override;
protected:
	// Bind Matrix
	HRESULT Bind_Matrices();
protected:
	CAMDESC					m_tCameraDesc;
	class CPipeLine*		m_pPipeLine = nullptr;
	// Name
	_string						m_strCamName = "";
	_float4x4					m_ProjMatrix = _float4x4();
	_float4x4					m_PrevViewMatrix = _float4x4();
public:
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END