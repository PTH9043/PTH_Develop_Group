#pragma once
#include "UActor.h"

BEGIN(Engine)
class UPipeLine;

class UCamera abstract : public UActor{
public:
	enum class PROJECTION_TYPE
	{
		PERSPECTIVE, // 원근 투영
		ORTHOGRAPHIC, // 직교 투영
		PROJ_END
	};
	// Cam Proj
	typedef struct tagCameraProj
	{
		tagCameraProj() = default;
		tagCameraProj(const PROJECTION_TYPE& _eProjType, const _float3& _v3Eye, const _float3& _v3At,
			const _float& _fForv, const _float& _fWidth, const _float& _fHeight, const _float& _fNear = 0.f, const _float& _fFar = 1000.f,
			const _float& _fScale = 1.f)
			: eProjType(_eProjType), v3Eye(_v3Eye), v3At(_v3At), fFovy(_fForv), fWidth(_fWidth), fHeight(_fHeight), fNear(_fNear), fFar(_fFar),
			fScale(_fScale)
		{}

		PROJECTION_TYPE		eProjType = PROJECTION_TYPE::PROJ_END;
		_float3 v3Eye = _float3(0.f, 0.f, 0.f), v3At = _float3(0.f, 0.f, 0.f);
		_float fFovy = 0.f, fWidth = 0.f, fHeight = 0.f, fNear = 0.f, fFar = 1000.f;
		_float fScale = 1.f;
	}CAMPROJ;
	// Cam Value
	typedef struct tagCamValue
	{
		tagCamValue() = default;
		tagCamValue(const _float& _fCamMove, const _float& _fCamRot) : fCamMoveSpeed(_fCamMove), fCamRotSpeed(_fCamRot)
		{}
		_float fCamMoveSpeed = 0.f;
		_float fCamRotSpeed = 0.f;
	}CAMVALUE;
	// CamDesc 
	typedef struct tagCamDesc
	{
		tagCamDesc() : stCamProj{},	stCamValue{}, eCamType{ CAMERATYPE::OTHER } {}
		tagCamDesc(const CAMPROJ& _stCamProj, const CAMVALUE& _stCamValue, const CAMERATYPE _eCameraType = CAMERATYPE::OTHER)
			: stCamProj(_stCamProj), stCamValue(_stCamValue), eCamType(_eCameraType)
		{}
		// Cam Name
		CAMPROJ				stCamProj;
		CAMVALUE			stCamValue;
		CAMERATYPE		eCamType;
	}CAMDESC;

	enum VOIDDATAS_ID
	{
		CAMDESC_ID = 0,
	};

public:
	UCamera(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType);
	UCamera(const UCamera& _rhs);
	DESTRUCTOR(UCamera)
public:
	const CAMID& GetCamID() const { return m_iCamID; }
	const _float4x4& GetProjMatrix() const { return m_mProjMatrix; }
	const _bool& IsFrustomOn() const { return m_isFrustomOn; }
	const _float GetCamFar() const { return m_stCamProj.fFar; }
	const _float GetCamMoveSpeed() const { return m_stCamValue.fCamMoveSpeed; }
	const _float GetCamRotSpeed() const { return m_stCamValue.fCamRotSpeed; }

	void SetCamID(const _uint& _iCamID) { this->m_iCamID = _iCamID; }
	void SetFrunstomOn(const _bool _isFrustomOn) { this->m_isFrustomOn = _isFrustomOn; }
	void SetCamMoveSpeed(const _float _fMoveSpeed) { this->m_stCamValue.fCamMoveSpeed = _fMoveSpeed; }
	void SetCamRotSpeed(const _float _fRotSpeed) { this->m_stCamValue.fCamRotSpeed = _fRotSpeed; }
public:
	virtual void Free() override PURE;
	virtual SHPTR<UCloneObject> Clone(const VOIDDATAS& _vecDatas) override PURE;
	// Native
	virtual HRESULT NativeConstruct() override PURE;
	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override PURE;
	// Bind Matrix
	HRESULT BindMatrix(VIEWPROJMATRIX& _stViewProjMatrix);
protected:
	virtual void TickActive(const _double& _dTimeDelta) PURE;
	virtual void LateTickActive(const _double& _dTimeDelta) PURE;
	virtual HRESULT RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor) override;
	virtual HRESULT RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass = true) override;
private:
	// Make ProjMatrix
	HRESULT MakeProjMatrix(const CAMPROJ& _stCamProj);
#ifdef _USE_IMGUI
	virtual void ShowObjectInfo();
#endif
private:
	_float4x4														m_mPrevViewMatrix{ _float4x4::Identity };
	_float4x4														m_mProjMatrix{ _float4x4::Identity };
	PROJECTION_TYPE										m_eProjType{ PROJECTION_TYPE::PROJ_END };
	CAMVALUE													m_stCamValue{};
	CAMPROJ														m_stCamProj{};
	CAMID															m_iCamID{ 0 };
	_bool																m_isFrustomOn;
};

END