#pragma once
#include "UObject.h"

BEGIN(Engine)
class UShaderConstantBuffer;
class UShader;
class UVIBufferRect;

class URenderObject;

class ULight : public UObject {
public:
	ULight(CSHPTRREF<UDevice> _spDevice, const LIGHTINFO& _stLightInfo);
	NO_COPY(ULight)
		DESTRUCTOR(ULight)
public:
	// Get 
	const LIGHTINFO& GetLightInfo() const { return m_stLightParam.tLightInfo; }
	const _bool IsActive() const { return m_isActive; }
	const LIGHTTYPE GetLightType() const { return m_stLightParam.tLightInfo.eLightType; }
	// set 
	void SetLightType(const LIGHTTYPE _eLightType) { this->m_stLightParam.tLightInfo.eLightType = _eLightType; }
	void SetActive(const LIGHTACTIVE& _bIsActive) { this->m_stLightParam.tLightInfo.eLightActive = _bIsActive; m_isActive = static_cast<_bool>(_bIsActive);}
	void SetActive(const _bool& _bIsActive) { m_isActive = _bIsActive; }
	// Set Diffuse Ambient Specular
	void SetDiffuse(const _float3& _vDiffuse) { this->m_stLightParam.tLightInfo.vDiffuse = _vDiffuse; }
	void SetAmbient(const _float3& _vAmbient) { this->m_stLightParam.tLightInfo.vAmbient = _vAmbient; }
	void SetSpecular(const _float3& _vSpecular) { this->m_stLightParam.tLightInfo.vSpecular = _vSpecular; }
	// Direction Light Range Angle
	void SetDirection(const _float3& _v3Direction) { this->m_stLightParam.tLightInfo.vDirection = _v3Direction; }
	void SetLightPos(const _float3& _v3LightPos) { this->m_stLightParam.tLightInfo.vPosition = _v3LightPos; }
	void SetRange(const _float& _fRange) { this->m_stLightParam.tLightInfo.fRange = _fRange; }
	void SetAngle(const _float& _fAngle) { this->m_stLightParam.tLightInfo.fAngle = _fAngle; }
	void SetLightPower(const _float& _fPower) { this->m_stLightParam.tLightInfo.fLightPower = _fPower; }
	void SetSpecularPowValue(const _float _fPowValue) { this->m_stLightParam.tLightInfo.fSpecularPowValue = _fPowValue; }
	
public:
	// Free
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;

	// Render
	void Render(SHPTR<UCommand> spCmdList, CSHPTRREF<UVIBufferRect> _spVIBufferRect, CSHPTRREF<UShader> _spShader);

	void OutLightInfo(LIGHTINFO& _stLightInfo);
	
private:
	
	LIGHTCONTROLPARAM							m_stLightParam;
	_bool															m_isActive{ false };
};

END
