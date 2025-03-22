#pragma once
#include "Tool_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Tool)

class CCamera_Cinematic_Tool : public CCamera
{
private:
	explicit CCamera_Cinematic_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCamera_Cinematic_Tool(const CCamera_Cinematic_Tool& _rhs);
	virtual ~CCamera_Cinematic_Tool() = default;

public:
	// Get Cinematic Data 
	vector<CINEMATIC_CAMDATA>* Get_CinematicData() { return &m_CinematicDatas; }
	// Set Cinematic Data 
	void Set_CinematicData(const _uint& _iIndex, CINEMATIC_CAMDATA* _pData);
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* _pArg) override;
	virtual void Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

public:
	// Play Cam Move
	void Play_CameraMove();
	void Stop_CameraMove();
public:
	// Cinematic CamData
	void Insert_Data(const CINEMATIC_CAMDATA& _tData);

	void Save_Data(const _wstring& _strFolderName, const _wstring& _strName);
	void Load_Data(const _wstring& _strFolderName, const _wstring& _strName);
protected:
	// Show Bounding Box
	void Show_BoundingBox(const _float3& _v3Size, const _float3& _v3Pos, CGameInstance* _pGameInstance);
	// Camera Position
	void Move_Camera_Position(const _double& _dTimeDelta);
	// Find Camera 
	void Find_Camera();
private:
	// Cinematic Data 
	vector<CINEMATIC_CAMDATA>	m_CinematicDatas;
	// 현재 움직이고 있는 인덱스
	_uint													m_iCurIndex = 0;
	// Init CameraSetting
	_bool													m_bInitCameraSetting = false;
	// Assign Cam List
	vector<CCamera*>					m_AssignCamList;
	_float3											m_v3DeltaRotationValue = _float3(0.f, 0.f, 0.f);
	_float												m_fCamLerpSpeed = 0.f;
	_float4											m_vSlerpRotation = _float4(0.f, 0.f, 0.f, 0.f);
public:
	static CCamera_Cinematic_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END