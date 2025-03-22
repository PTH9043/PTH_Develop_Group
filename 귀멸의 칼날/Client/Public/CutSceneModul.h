#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel;
class CCamera;
class CEffectObject;
class CVIBuffer_Terrain;
class CGameInstance;
class CTransform;
class CShader;
class CVIBuffer_Rect;
class CTexture;
class CNavigation;
END

BEGIN(Client)
class CCutSceneTex;
class CCamera_CutScene;
class CCutScene_ModelController;
class CCharacter;
class CCutSceneTex;
class CCamera_Free;

class CCutSceneModul final : public CBase
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
protected:
	explicit CCutSceneModul();
	explicit  CCutSceneModul(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CCutSceneModul() = default;
public:

	const _wstring& Get_Name() { return m_strName; }
	const _string& Get_InputTrigger() { return m_strInputTrigger; }
	const _bool& Is_End() const { return m_bIsEnd; }
	const _bool& Is_NotChangeCutScene() const { return m_bNotChangeCutScene; }
	void Set_NotReceivePosition(const _bool& _bIsNotReceivePos) { m_bNotReceivePosition = _bIsNotReceivePos; }
	void Set_bLastBindCameraFree(const _bool& _bLastBindCameraFree) { this->m_bLastBindCameraFree = _bLastBindCameraFree; }
	void Set_NotChangeCutScene(const _bool& _bNotChangeCutScene) { this->m_bNotChangeCutScene = _bNotChangeCutScene; }
public:
	// Native Construct
	HRESULT NativeConstruct(VOIDDATAS* _pDatas);
	HRESULT Tick(const _double& _dTimeDelta);
	HRESULT Late_Tick(const _double& _dTimeDelta);
	// Load Data
	HRESULT Load_Data(FILEDATA* _pFileData);
public:
	// Update Chracter
	void Start_CutScene(CCharacter* _pChar1, CCharacter* _pChar2);
	// Update Cut Scene
	void Update_CutScene(const _double& _dTimeDelta);
	// Update Cut Scene
	void LateUpdate_CutScene(const _double& _dTimeDelta);
	// Active Cut Scene
	void Active_CutScene();
	// Render Texture
	void Render_Texture();
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
	// Compute Move Pos
	// 프레임당 이동값 계산 함수 ( MoveSpeed를 이용하여 구함. )
	_vector Compute_MovePos(const _double& _dTimeDelta, _vector vMoveDir
		, _float fMoveSpeed, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// 프레임당 이동값 게산 함수 ( 최대 이동할 거리를 이용하여 구함. )
	_vector Compute_MovePos_ByDist(const _double& _dTimeDelta, _vector vMoveDir,
		_float fDist, _float fMaxHeight = 0.f, _float fMaxHeightRatio = 0.f, _float fPlayTime = 0.f);
	// RotationConvert
	_float3 Rotation_Convert(_float3 _vRot);
	// Coord Position
	_float3 Coord_PositionF3(_float3 _vPos);
	// Coord Position
	_vector Coord_Position(_float3 _vPos);
protected:
	// Reset Data
	void Reset_Cam();
	void Reset_Char();
protected:
	ID3D11Device*												m_pDevice = nullptr;
	ID3D11DeviceContext*									m_pContext = nullptr;
protected:
	CCamera_CutScene*										m_pCamCutScene = nullptr;
	CCamera_Free*												m_pCameraFree = nullptr;
	// Scene Data
	vector<CUTSCENEDATA>								m_CutSceneDataVector;
	vector<CUTCHARDATA>								m_CutCharDataVector;
	vector< CUTDMGDATA>								m_CutDmgDataVector;
	CCutScene_ModelController*						m_pCutScene_OwnerModelCom = nullptr;
	CCutScene_ModelController*						m_pCutScene_TargetModelCom = nullptr;
	CCharacter*														m_pOwnerCharacter = nullptr;
	CCharacter*														m_pTargetCharacter = nullptr;
	CNavigation*													m_pOwnerNavigation = nullptr;
	CNavigation*													m_pTargetNavigation = nullptr;
	_float3																m_vOwnerRotAngles = _float3(0.f, 0.f, 0.f);
	_float3																m_vTargetRotAngles = _float3(0.f, 0.f, 0.f);
	_float3																m_vJump_Pos = _float3(0.f, 0.f, 0.f);

	_float4x4															m_StartWorldMatrix;
	_float4x4															m_RotationWorldMatrix;

	_float3																m_vSaveOwnerPosition = _float3(0.f, 0.f, 0.f);
	_float3																m_vSaveTargetPosition = _float3(0.f, 0.f, 0.f);
	_float3																m_vSaveCamPosition = _float3(0.f, 0.f, 0.f);
protected:
	// Total Time
	_double																m_dTotalTime = 0.f;
	// Current Time
	_float																	m_fCurrentCharTime = 0.f;
	_float																	m_fCurrentCamTime = 0.f;
	// Update, Late Update
	_float																	m_fUpdateTime = 0.f;
	_float																	m_fUpdateLateTime = 0.f;
	// Texture
	vector<CCutSceneTex*>								m_CutSceneTexVector;
	CTexture*															m_pTexture = nullptr;
	CVIBuffer_Rect*												m_VIBuffer_CutSceneTexture = nullptr;
	CShader*															m_pCutScene_Shader = nullptr;
	_float4x4															m_Texture_WorldMatrix = _float4x4();
	_float4x4															m_Texture_ViewMatrix = _float4x4();
	_float4x4															m_Texture_ProjMatrix = _float4x4();
	_int																		m_iCutScene_Texture_Index = 0;
	_bool																	m_bIsEnd = false;
protected:
	// Input Trigger
	_wstring															m_strName = L"";
	_string																m_strInputTrigger = "";
	// Use Values
	_float																	m_fLerpAddValue = 0.f;
	_float																	m_fMoveSplineValue = 0.f;
	_float																	m_fMoveBezierValue = 0.f;
	_double																m_dFallTimeAcc = 0.0;
	_bool																	m_bPlayCutScene = false;
	_bool																	m_bPlayCutChar = false;
	// Index
	_int																		m_iCutScene_Index = 0;
	_int																		m_iCutChar_Index = 0;
	_int																		m_iCutDmg_Index = 0;
	_int																		m_iCutSceneTextureIndex = 0;
	// Timer 
	CUSTIMER															m_tMoveShakeTimer = CUSTIMER();
	CUSTIMER															m_tRotShakeTimer = CUSTIMER();
	// Root Bone Owner
	_bool																	m_bRootBone_Owner = false;
	_bool																	m_bRootBone_Target = false;
	// Jump Check Char Index 
	_bool																	m_bJumpEnd = false;
	_bool																	m_bPass = false;
	_int																		m_iJump_CheckCharIndex = -1;
	// Cam Delta Speed Owner DeltaSpeed
	_float																	m_fCamDeltaSpeed = 0.f;
	_float																	m_fOwnerDeltaSpeed = 0.f;
	_float																	m_fTargetDeltaSpeed = 0.f;
	_float3																m_vOwnerRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
	_float3																m_vTargetRotationDeltaSpeed = _float3(0.f, 0.f, 0.f);
	// Not Receive Position
	_bool																	m_bNotReceivePosition = false;
	_bool																	m_bLastBindCameraFree = false;
	_bool																	m_bNotChangeCutScene = false;
public:
	static CCutSceneModul* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, VOIDDATAS* _pDatas = nullptr);
	virtual void Free() override;
};

END