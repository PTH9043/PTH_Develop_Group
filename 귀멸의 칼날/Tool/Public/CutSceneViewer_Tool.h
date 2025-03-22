#pragma once
#include "Tool_Defines.h"
#include "ImGuiClass_Tool.h"
#include "CutSceneTex_Tool.h"
#include "CutSceneEffectGroup_Tool.h"

BEGIN(Engine)
class CModel;
class CCamera;
class CEffectObject;
class CVIBuffer_Terrain;
class CGameInstance;
class CTransform;
class CShader;
class CVIBuffer_Rect;
class CEffectObject;
END

BEGIN(Tool)
class CCutSceneModel_Tool;
class CCamera_Free_Tool;
class CCamera_Cinematic_Tool;
class CItem_Tool;

class CCutSceneViewer_Tool final : public CImGuiClass_Tool
{
public:
	typedef struct tagCutSceneData
	{
		tagCutSceneData() = default;
		// Cur Time
		_float													fCurTime = 0.f;
		_float													fPrevTime = 0.f;
		// Cam Position, Rotation
		_float3												vCamPosition = _float3(0.f, 0.f, 0.f);
		_float3												vCamRotation = _float3(0.f, 0.f, 0.f);
		_float4												vCamQuaternion = _float4(0.f, 0.f, 0.f, 0.f);
		_bool													bCutSceneTexShow = false;
		_bool													bCutSceneEffectGroupShow = false;
		// CamLook Target
		_bool													bCamOwnerLook = false;
		_bool													bCamTargetLook = false;
		_bool													bCamLerp = false;
		_bool													bCamMove = false;
		_bool													bDistanceToRotLerp = false;
		_bool													bCurNodeNearDisNotPlus = false;
		_bool													bCurNodeNearRotNotPlus = false;
		_float													fCamSpeed = 0.f;
		_float													fCamLerpSpeed = 1.f;
		_float													fLerpRestrict = 1.25f;
		// Cam Shake
		_bool													bCamMoveShake = false;
		_bool													bCamRotShake = false;
		_float3												v3ShakePosValue = _float3(1, 1, 1);
		_int3													u3ShakeRotValue = _int3(1, 1, 1);
		_float													fMoveShakePassActiveTime = 0.2f;
		_float													fRotShakePassActiveTime = 0.2f;
		// Spline 
		_bool													bSplineCam = false;
		_float													fSplineCamSpeed = 0.f;
		//Bezier
		_bool													bUseBezier = false;
		_float3												vBezierPos = _float3(0.f, 0.f, 0.f);
		_float3												vBezierPrevPos = _float3(0.f, 0.f, 0.f);
		_float													fBezierSpeed = 0.f;
		// Next fPrevDistance
		_float													fPrevDistance = 0.f;
		_float													fDeltaSpeed = 0.f;
		// 4 Byte
		_int														iTextureIndex = 0;
		_bool													bAllTextureDraw = false;
		_bool													bCamStrongShake = false;		
		_bool													bCamOpposite = false;

		_bool													BYTESAVE_1BOOL = false;
		// + 3
		_float													BYTESAVE_1FLOAT1 = _float();
		// 7
		_float4												BYTESAVE_1FLOAT4 = _float4();
		//23
		_float4												BYTESAVE_2FLOAT4 = _float4();
		//49
		_float4												BYTESAVE_3FLOAT4 = _float4();
		_float4x4											BYTESAVE_2_FLOAT4x4 = _float4x4();
		_float4x4											BYTESAVE_3_FLOAT4x4 = _float4x4();
		_float4x4											BYTESAVE_4_FLOAT4x4 = _float4x4();
	}CUTSCENEDATA;
	// Damage Scene Data
	typedef struct	tagCutCharData
	{
		_float													fCurTime = 0.f;
		_float													fPrevTime = 0.f;
		// Owner Pos Rot 
		_float3												vOwnerPosition = _float3(0.f, 0.f, 0.f);
		_float3												vOwnerRotation = _float3(0.f, 0.f, 0.f);
		// Target Pos, Rot
		_float3												vTargetPosition = _float3(0.f, 0.f, 0.f);
		_float3												vTargetRotation = _float3(0.f, 0.f, 0.f);
		// Owner Move, Target Move
		_bool													bOwnerMove = false;
		_bool													bTargetMove = false;
		// Owner, Target Speed
		_float													fOwnerSpeed = 0.f;
		_float													fTargetSpeed = 0.f;
		_bool													bOwnerActive = true;
		_bool													bTargetActive = true;
		// Jump
		_float3												vAttackDir = _float3(0.f, 0.f, 0.f);
		_float													fDist = 0.f;
		_float													fMaxHeight = 0.f;
		_float													fMaxHeightRatio = 0.f;
		_float													fPlayTime = 0.f;
		_float													fJumpDelayTime = 1.f;
		_bool													bJump = false;

		// 2Byte
		_bool													bUseOwnerGlow = false;
		_bool													bUseTargetGlow = false;
		// + 4
		_bool													bNotResetJumpTarget = false;

		_float													fOwnerDeltaSpeed = 0.f;
		_float													fTargetDeltaSpeed = 0.f;
		_bool													bOwnerRotationUse = false;
		_bool													bTargetRotationUse = false;
		_float3												vOwnerRotationSpeed = _float3(0.f, 0.f, 0.f);
		_float3												vTargetRotationSpeed = _float3(0.f, 0.f, 0.f);
		_float3												vOwnerRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
		_float3												vTargetRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);

		_float													fOwnerAnimSpeed = 1.f;
		_float													fTargetAnimSpeed = 1.f;
		_float													fControlTotalUpdateSpeed = 1.f;

		_bool													BYTESAVE_1BOOL1 = false;
		_bool													BYTESAVE_1BOOL2 = false;
		// 7
		_float4												BYTESAVE_1FLOAT4 = _float4();
		//23
		_float4												BYTESAVE_2FLOAT4 = _float4();
		//23
		_float4												BYTESAVE_3FLOAT4 = _float4();
		//23
		_float													BYTESAVE_1FLOAT = _float();
		_float4x4											BYTESAVE_3_FLOAT4x4 = _float4x4();
		_float4x4											BYTESAVE_4_FLOAT4x4 = _float4x4();
	}CUTCHARDATA;

	typedef struct tagCutDmgData
	{
		_float													fCurTime = 0.f;
		_float													fDmgData = 0.f;


		_float4x4											BYTESAVE_1_FLOAT4x4 = _float4x4();
		_float4x4											BYTESAVE_2_FLOAT4x4 = _float4x4();
		_float4x4											BYTESAVE_3_FLOAT4x4 = _float4x4();
		_float4x4											BYTESAVE_4_FLOAT4x4 = _float4x4();
	}CUTDMGDATA;

	typedef enum EDrawType_ToCutViewer
	{
		DR_CUTVIEWER_CAMERA, DR_EFFECT_POSITION,
		DR_CUTVIEWER_MODEL_OWNER, DR_CUTVIEWER_MODEL_TARGET,
		DR_CUTVIEWER_END
	}DRAWTYPE_CUTVIEWER;
private:
	explicit  CCutSceneViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutSceneViewer_Tool() = default;
public:
	virtual HRESULT NativeConstruct() override;
	// Native Inner Value (Reset ImGuiClass Variable)
	virtual HRESULT NativeConstruct_InnerValue() override;
	virtual HRESULT Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Late_Tick(const _double& _dTimeDelta) override;
	// Render
	virtual HRESULT Render() override;
	// Release Inner Value (Reset ImGuiClass Variable)
	virtual HRESULT Release_InnerValue() override;
protected:
	// Show Window
	virtual void  Show_Window() override;
	// First Modify Cut Scene Viewer Tool
	void First_Modify_CutSceneViewer_Tool();
	// Select Model
	void Show_Select_Model();
	// Modify Total Value
	void Modify_Total_Value();
	// Play CutScene
	void Control_CutScene();
	// Add Cut Scene Viewer
	void Add_CutSceneViewer();
	// Add Dmg Scene Data
	void  Add_CutCharInfo();
	// Lerp Cut Scene Data
	void Lerp_CutSceneData();
	// Lerp Cut Char Data 
	void Lerp_CutCharData();
	// Cut Scene Viewer
	void Show_And_CutSceneViewer();
	// Modify Texture
	void Modify_CutSceneTexture();
	// Show Cut Scene Viewer
	void Show_CutSceneAnimationViewer();
	// Attach Sword Model
	void Attach_SwordModel(CItem_Tool* _pItemTool, CCutSceneModel_Tool* _pCutSceneModel);
protected:
	// Calculate Length
	void CalCulate_CamLoadLength(const _int& _iIndex);
protected:
	// Update Cut Scene
	void Update_CutScene(const _double& _dTimeDelta);
	// Update Cut Scene
	void LateUpdate_CutScene(const _double& _dTimeDelta);
	// Render TO
	void Render_To_All_Position();
	// Render Terrain
	void Render_Terrain();
	// Render Texture
	void Render_Texture();
	// Picking Terrain
	void Picking_Terrain();
protected:
	// Bind Next Scene Data
	void Bind_NextSceneData();
	// Look At Cam
	void LookAt_ToCam(const _double& _dTimeDelta);
	// Move To Cam
	void MoveTo_Cam(const _double& _dTimeDelta);
	// Move To Cam
	void MoveTo_CamJust(const _double& _dTimeDelta);
	// Move To Cam Spline
	void MoveTo_CamSpline(const _double& _dTimeDelta);
	// Move To Cam Bezier
	void MoveTo_CamBezier(const _double& _dTimeDelta);
	// Lerp To cam
	void LerpTo_Cam(const _double& _dTimeDelta);
	// Cam Shake
	void CamShake(const _double& _dTimeDelta);
	// Multiple Minus Random
	_int Multiple_Minus_Random();
protected:
	// Bind Next Char Data 
	void Bind_NextCharData();
	// Move To Char
	void MoveTo_Char(const _double& _dTimeDelta);
	// RotateTo Char
	void RotateTo_Char(const _double& _dTimeDelta);
	// Jump To Target
	void JumpTo_Target(const _double& _dTimeDelta);
	// Fall Target
	_bool Fall_Target(const _double& _dTimeDelta);
	// Is Air Target
	_bool	Is_AirTarget(CTransform* pTransform);
	// Compute Move Pos
	// 프레임당 이동값 계산 함수 ( MoveSpeed를 이용하여 구함. )
	_vector Compute_MovePos(const _double& _dTimeDelta, _vector vMoveDir
		, _float fMoveSpeed, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// 프레임당 이동값 게산 함수 ( 최대 이동할 거리를 이용하여 구함. )
	_vector Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir,
		_float fDist, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
protected:
	// Show Dmg Scene Data;
	void Show_CutCharData();
protected:
	// Save Load Control Method
	void SaveLoad_ControlMethod();
	// Save Cut Scene Data 
	void Save_CutSceneData(const _wstring& _Path);
	// Load Cut Scene Data
	void Load_CutSceneData();
	// Reset Tool 
	void Reset_Tool();
	// All save 
	void All_Save();
protected:
	// Draw Nodes
	void Draw_Value(CGameInstance* _pGameInstance, const _uint& _iType, const _float3& _v3Size,
		const _float3& _v3Pos, const _float4& _v3Rot = _float4(0.f, 0.f, 0.f, 0.f));
	// Draw Value Model
	void Draw_Value_Model(CGameInstance* _pGameInstance, const _float3& _v3Size,
		const _float3& _v3Pos, const _float4& _vColor = _float4(0.f, 0.f, 0.f, 0.f));
	// Camera Line
	void Draw_CameraLine(CGameInstance* _pGameInstance, _float3 _vPrevIndex, _float3 _vNextIndex, const _float4& _vColor = _float4(0.f, 0.f, 0.f, 0.f));
	// Camera Line
	void Draw_CameraLine(CGameInstance* _pGameInstance, _fvector _vPrevIndex, _fvector _vNextIndex, const _float4& _vColor = _float4(0.f, 0.f, 0.f, 0.f));
	// Reset Tool Data
	void Reset_CharData();
	// Reset Data 3
	void Reset_CamData();
protected:
	// Dock Desc
	DOCKDESC														m_tMainView;
	DOCKDESC														m_tModifyViewer;
	DOCKDESC														m_tCutSceneViewer;
	DOCKDESC														m_tAnimClipViewer;
	DOCKDESC														m_tSaveSceneViewer;
	DOCKDESC														m_tModifyTargetPlayerViewer;
	// Init Setting
	_bool																	m_bInitSetting = false;
protected:
	// Cut OwnerModel
	list< CCutSceneModel_Tool*>						m_CutSceneModelList;
	// Cut Scene Texture List
	vector<CTexture*>										m_TextureVector;
	// Cut Scene Model 
	CCutSceneModel_Tool*								m_pCutScene_OwnerModel = nullptr;
	CCutSceneModel_Tool*								m_pCutScene_TargetModel = nullptr;
	// Cut Scene Texture Save
	vector<CCutSceneTex_Tool*>					m_CutSceneTexVector;
	vector<CCutSceneEffectGroup_Tool*>	m_CutSceneEffectVector;
	// ItemModelList
	vector<CModel*>											m_ItemModelList;
	// Sword Tool 
	CItem_Tool*														m_pOwnerSword = nullptr;
	CItem_Tool*														m_pTargetSword = nullptr;
	_float																	m_fUpdateTime = 1.f;
	_float																	m_fUpdateLateTime = 1.f;
	// Cut Scene Texture Index;
	_int																		m_iCutSceneTextureIndex = 0;
	_int																		m_iCutSceneEffectIndex = 0;
	// bool Make Texure
	_bool																	m_bMakeTexture = false;
	_bool																	m_bMakeEffect = false;
	// Cam Tool
	class CCamera_Tool* m_pCamera_Tool = nullptr;
	// Camera Free Tool  
	class CCamera_Free_Tool* m_pCamera_Free_Tool = nullptr;
	// Change Camera
	_bool																	m_bChangeCamera = false;
	// Camera Rotation
	_float3																m_vCameraRotation = _float3(0.f, 0.f, 0.f);
	// Scene Data
	vector<CUTSCENEDATA>								m_CutSceneDataVector;
	vector<CUTCHARDATA>								m_CutCharDataVector;
	vector< CUTDMGDATA>								m_CutDmgDataVector;
	// Cut Scene Data 
	CUTSCENEDATA*											m_pSelectSceneData = nullptr;
	CUTDMGDATA*												m_pSelectDmgData = nullptr;
	// Total Time
	_double																m_dTotalTime = 0.f;
	// Drag Cur Time
	_float																	m_fDragCamCurTime = 0.f;
	_float																	m_fDragCharCurTime = 0.f;
	// Index 
	_int																		m_iCutScene_Index = 0;
	_int																		m_iCutScene_SelectIndex = 0;
	_int																		m_iCutChar_Index = 0;
	_int																		m_iCutChar_SelectIndex = 0;
	// Select Scene Name
	_string																m_strSelectSceneName = "";
	_string																m_strSelectCutCharName = "";
	_string																m_strSelectDmgName = "";
	// Play Cut Scene
	_bool																	m_bPlayCutChar = false;
	_bool																	m_bPlayCutScene = false;
	// Current Time
	_float																	m_fCurrentCharTime = 0.f;
	_float																	m_fCurrentCamTime = 0.f;
	_float																	m_fMultipleValue = 1.f;
	_bool																	m_bRender_Restrict = false;
	// Target Position
	_float3																m_v3OwnerPosition = _float3(0.f, 0.f, 0.f);
	_float3																m_v3TargetPosition = _float3(0.f, 0.f, 0.f);
	_bool																	m_bIsPicking = false;
	_int														                m_iSelectTarget = 0;
	// Input Trigger
	_string																m_strInputTrigger;
	// Terrain 
	CVIBuffer_Terrain*											m_pVIBuffer_TerrainData = nullptr;
	CShader*															m_pTerrain_ShaderCom = nullptr;
	CTransform*													m_pTerrain_TransformComp = nullptr;
	/*  Texture RECT */
	// Texture
	CVIBuffer_Rect*												m_VIBuffer_CutSceneTexture = nullptr;
	CShader*															m_pCutScene_Shader = nullptr;
	_float4x4															m_Texture_WorldMatrix = _float4x4();
	_float4x4															m_Texture_ViewMatrix = _float4x4();
	_float4x4															m_Texture_ProjMatrix = _float4x4();
	_int																		m_iCutScene_Texture_Index = 0;
	// Texture Render
	_bool																	m_bTexture_Render_DebugMode = false;
	_bool																	m_bRender_Spline = true;
	_bool																	m_bRender_Bezier = true;
	// Effect Object
	_wstring															m_strSelectEffectName = L"";
	FILEGROUP*														m_pEffectFileGroup = nullptr;
	// Save Load String
	_string																m_strSaveCutScene = "";
	_string																m_strLoadCutScene = "";
	// BInd Cut Scene Camera Pos And Rotation Index
	_int																		m_iBindCutScene_CameraPosAndRotation_Index = 0;
	_bool																	m_bControl_PlayAnimation = false;
	_bool																	m_bModelActive = true;

	// Cut Scene Data 
	_bool																	m_bUseLerp = false;
	_bool																	m_bAllCamLerp = false;
	_bool																	m_bUseCharLerp = false;
	_bool																	m_bAllCharLerp = false;
	CUTSCENEDATA*											m_pSelect_lerpPrevSceneData = nullptr;
	CUTSCENEDATA*											m_pSelect_lerpNextSceneData = nullptr;
	CUTCHARDATA*												m_pSelect_lerpPrevCharData = nullptr;
	CUTCHARDATA*												m_pSelect_lerpNextCharData = nullptr;
	_float																	m_fCamLerpAll = 0.5f;
	_float																	m_fCharLerpAll = 0.f;
	_float																	m_fLerpCamPosRatio = 0.5f;
	_float																	m_fLerpCamRotRatio = 0.5f;
	_float																	m_fLerpOwnerPosRatio = 0.5f;
	_float																	m_fLerpOwnerRotRatio = 0.5f;
	_float																	m_fLerpTargetPosRatio = 0.5f;
	_float																	m_fLerpTargetRotRatio = 0.5f;
	_float																	m_fLerpCamSpeedRatio = 0.5f;
	_float																	m_fLerpOwnerSpeedRatio = 0.5f;
	_float																	m_fLerpTargetSpeedRatio = 0.5f;
	_float																	m_fLerpSplineSpeedRatio = 0.5f;
	_float																	m_fLerpBezierSpeedRatio = 0.5f;

	_string																m_strSelect_lerpPrevSceneName = "";
	_string																m_strSelect_lerpNextSceneName = "";
	_string																m_strSelect_lerpPrevCharName = "";
	_string																m_strSelect_lerpNextCharName = "";
	_float																	m_fLerpAddValue = 0.f;
	_float																	m_fMoveSplineValue = 0.f;
	_float																	m_fMoveBezierValue = 0.f;
	CUSTIMER															m_tMoveShakeTimer = CUSTIMER();
	CUSTIMER															m_tRotShakeTimer = CUSTIMER();
	// Dmg Scene Data 
	CUTCHARDATA*												m_pSelectCutCharData = nullptr;
	_bool																	m_bCutCharData = false;

	_bool																	m_bPass = false;
	_bool																	m_bJumpEnd = false;
	_double																m_dFallTimeAcc = 0.0;

	_bool																	m_bRootBone_Owner = false;
	_bool																	m_bRootBone_Target = false;
	// Jump Check Char Index 
	_int																		m_iJump_CheckCharIndex = -1;

	_float																	m_fCamDeltaSpeed = 0.f;
	_float																	m_fOwnerDeltaSpeed = 0.f;
	_float																	m_fTargetDeltaSpeed = 0.f;
	_float3																m_vOwnerRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
	_float3																m_vTargetRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
public:
	static CCutSceneViewer_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END