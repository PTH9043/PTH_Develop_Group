#pragma once
#include "UObject.h"

BEGIN(Engine)
class ULight;
class ULoader;
class UShader;
class UVIBufferRect;
class UShaderConstantBuffer;


using LIGHTCONTAINER = VECTOR<SHPTR<ULight>>;
using LIGHTGROUP = UNORMAP<LIGHTTYPE, LIGHTCONTAINER>;
using LIGHTSHADERS = UNORMAP<LIGHTTYPE, SHPTR<UShader>>;
using LIGHTPARAMVECTOR = VECTOR<LIGHTINFO>;

class UScene abstract : public UObject {
public:
	UScene(CSHPTRREF<UDevice> _spDevice, const _ushort _iSceneID);
	NO_COPY(UScene)
	DESTRUCTOR(UScene)
public:
	CSHPTRREF<ULoader> GetLoader() const { return m_spLoader; }

	const _ushort GetSceneID() const { return m_sSceneID; }
public:
	virtual void Free() override PURE;
	virtual HRESULT NativeConstruct() override;
	virtual HRESULT LoadSceneData() PURE;

	virtual void Tick(const _double& _dTimeDelta) PURE;
	virtual void LateTick(const _double& _dTimeDelta)PURE;
	// Render 
	void RenderLights();

	LIGHTCONTAINER& GetLightContainer() {return m_AllLightContainer; }
	void SetLightNums(_uint _uNum) { MAX_LIGHT_NUMS = _uNum; }
	_uint GetCurLightNums() {return CUR_LIGHT_NUMS; }

	HRESULT AddLight(const LIGHTINFO& _stLightInfo);
	void OutLight(const LIGHTTYPE& _eLightType, const _uint _iIndex, SHPTR<ULight>& _spLight);
	// Active
	HRESULT ActiveLIght(const LIGHTTYPE& _eLightType, const _uint& _iIndex, const _bool& _isActive);
	HRESULT ActiveLIght(const LIGHTTYPE& _eLightType, const _uint& _iIndex, const LIGHTACTIVE& _isActive);
	// Delete
	HRESULT DeleteLight(const LIGHTTYPE& _eLightType, const _uint& _iIndex);
	// Clear
	HRESULT ClearLight();
protected:
	HRESULT StartLoadingResource(const VECTOR<UDelegate<void, _bool&>>& _vecFunctions, const _uint& _iMethodCount);
private:
	SHPTR<ULoader>			m_spLoader;

	//LIGHTSHADERS				m_LightShaders;
	SHPTR<UVIBufferRect>	m_spVIBufferPlane;
	
	SHPTR<UShader> m_LightingShader;

	_ushort								m_sSceneID;
	LIGHTGROUP						m_LightGroup;
	LIGHTCONTAINER						m_AllLightContainer;
	_uint						MAX_LIGHT_NUMS;
	_uint						CUR_LIGHT_NUMS;
	LIGHTPARAMS m_stLightParams;//건네는용도
	LIGHTPARAMVECTOR m_stLightParamVector; //
	SHPTR<UShaderConstantBuffer>		m_spLightConstantBuffer;
};


END


