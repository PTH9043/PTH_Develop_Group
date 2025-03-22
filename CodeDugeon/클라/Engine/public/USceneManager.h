#pragma once
#include "UBase.h"

BEGIN(Engine)
class UDevice;
class UGpuCommand;
class UScene;
class ULight;
class URenderer;
class UShaderConstantBuffer;
class UGlobalConstantBuffer;
class UPipeLine;
class UGameInstance;
class URenderTargetManager;


class USceneManager : public UBase {
public:
	USceneManager();
	NO_COPY(USceneManager)
	DESTRUCTOR(USceneManager)
public:
	CSHPTRREF<UScene> GetScene() const { return m_spCurScene; }
public:
	virtual void Free() override;
	HRESULT ReadySceneManager(UGameInstance* _pGameInstance);
	void RegisterScene(CSHPTRREF<UScene> _spScene);
	void RegisterSceneWithOutLoadingData(CSHPTRREF<UScene> _spScene);
	void Tick(const _double& _dTimeDelta);
	void LateTick(const _double& _dTimeDelta);

	void OutLightControlInfo(LIGHTPARAM& _stLightControl);
	void ChangeLightCamIndex(const CAMID& _iID);
	// Render Light
	void RenderLight(CSHPTRREF<UPipeLine> _spPipeLine, CSHPTRREF<URenderTargetManager> _spRenderTargetManager,
		const TRANSFORMPARAM& _stDefferendParam);

	// Add 
	HRESULT AddLight(const LIGHTINFO& _stInfo);
	// Out Light
	void OutLight(const LIGHTTYPE& _eLightType, const _uint& _iIndex, SHPTR<ULight>& _spLight);
	// Active
	HRESULT ActiveLIght(const LIGHTTYPE& _eLightType, const _uint& _iIndex, const _bool& _isActive);
	// Delete
	HRESULT DeleteLight(const LIGHTTYPE& _eLightType, const _uint& _iIndex);
	// Clear
	HRESULT ClearLight();
private:
	const _uint											m_c_iLightParamSize;
	_uint														m_iLightCamIndex;
	SHPTR<UScene>									m_spCurScene;

	LIGHTPARAM											m_stLightParam;
	SHPTR< UGlobalConstantBuffer>	m_spLightGlobalBuffer;
};

END

