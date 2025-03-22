#pragma once
#include "UBase.h"

BEGIN(Engine)
class UCamera;
class UGameInstance;
class UGlobalConstantBuffer;
class UGraphicDevice;
class UFrustomCulling;
class UCommand;
class URenderer;
class UTransform;

class UPipeLine : public UBase {
	using CAMERAARR = ARRAY<SHPTR<UCamera>, MAX_CAM_SIZE>;
	using FRUNSTOMARR = ARRAY<SHPTR<UFrustomCulling>, MAX_CAM_SIZE>;
	using RENDERCAMIDLIST = LIST<CAMID>;
public:
	UPipeLine();
	NO_COPY(UPipeLine)
		DESTRUCTOR(UPipeLine)
public:
	virtual void Free() override;
	HRESULT ReadyPipeLine(UGameInstance* _pGameInstance);
	void RegisterCameraInPipeline(CSHPTRREF<UCamera> _spCamera, CAMID& _iCamID,
		const VIEWPROJMATRIX& _stViewProjMatrix, const CAMERATYPE _eCamType = CAMERATYPE::OTHER);
	void RemoveCameraInPipeLine(CAMID _CamID);

	void FrustomTick();
	_bool IsFrustomContains(const _float3& _vPos, const _float _fRadius, const CAMID& _iCamID = MAIN_CAMERA_ID);

	void UpdateViewProjMatrix(const _float _fGrobalDeltaTime);
	void BindViewProjMatrix(CSHPTRREF<UCommand> _spCommand);

	const _float4x4 GetMainCamViewMatrix() const;
	//2024-4-9 수정 서정원
	const _float4x4 GetMainCamWorldMatrix() const;
	const _float4x4& GetMainCamProjMatrix() const;
	const _float3& GetMainCamPosition();
	SHPTR<UTransform> GetMainCamTransform();
	const _float4x4 GetCamViewMatrix(const CAMID& _iID) const;
	const _float4x4& GetCamProjMatrix(const CAMID& _iID) const;
	const _float3& GetCameraPosition(const CAMID& _iID);
	const _float GetCamFar(const CAMID& _iID);
	// Clear 
	void ClearOneTypeCamera();
	void ChangeRenderCamera(CSHPTRREF<UCamera> _spCamera);
	void ChangeRenderCamera(CAMID _iID);

	void AddRenderCamList(CSHPTRREF<UCamera> _spCamera);
	void AddRenderCamList(CAMID _iID);

	const CAMID GetRenderCamID() const { return m_RenderCamID; }
private:
	void ArrangeToCameraArr();
	_uint GetID(const CAMERATYPE _eCamerType);
private:
	static constexpr _uint								VIEWPARAM_SIZE{ sizeof(VIEWPROJPARAM) };
	CAMERAARR												m_arrCameras;
	FRUNSTOMARR											m_arrFrustoms;
	SHPTR<UCamera>										m_spMainCamera;
	// BindParams
	SHPTR< UGlobalConstantBuffer>			m_spViewProjGlobalBuffer;
	VIEWPROJPARAM										m_stViewProjParam;
	// Render Cam List
	RENDERCAMIDLIST										m_RenderCamList;
	CAMID															m_RenderCamID;
};

END
