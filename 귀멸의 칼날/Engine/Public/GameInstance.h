#pragma once
#include "FilePath_Manager.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "GamePad_Device.h"
#include "Font_Manager.h"
#include "RanCount.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Sound_Manager.h"
#include "Thread_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Light_Manager.h"
#include "Target_Manager.h"
#include "FrustCulling.h"
#include "Picking.h"
#include "PipeLine.h"
#include "Drawing.h"
#include "Effect_Manager.h"
#include "Method.h"
#include "PostProcessing.h"
#include "ObjPooling_Manager.h"
#include "Debug_Manager.h"


/* 클라이언트로 정말 보여줘야할 함수만 모아놓는다. */
/* ㄴ클라이언트 입장에서는 엔진에 기능을 쓰고싶으면 GameIsntance만 생각하면된다. */

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;
public:
	// Current Frame Value
	const _uint& Get_Current_FrameValue() { return m_iCurrentFrameValue; }
	void Set_Current_FrameValue(const _uint & _iFrameValue) { this->m_iCurrentFrameValue = _iFrameValue; }
public: /* For.GameInstance */
	HRESULT Initialize_Engine(const _uint& _iNumLevels, const HINSTANCE& _hInst, const GRAPHICDESC& GraphicDesc,
		ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut); /* 엔진의 기능을 사용하기위해 초기화가 필요한 녀석들을 초기화한다. */
	// Tick Engine
	void Tick_Engine(const _double& _dTimeDelta); /* 매프레임마다 갱신이 필요한 매니져들의 동작. */
	// Late Tick Engine
	void LateTick_Engine(const _double& _dTimeDelta);
	// Render Engine
	HRESULT Render_Engine();
	// Clear Container
	HRESULT Clear_Container(const _uint& _iLevelIndex);
	// Clear Objects
	HRESULT Clear_Objects(const _uint& _iLevelIndex);
	// Clear Components 
	HRESULT Clear_Components(const _uint& _iLevelIndex);
public: /* For.FileManager */
	// Get File Group
	FILEGROUP* Get_FileGroup();
	// Load First Folder
	HRESULT Load_FirstFolder(const _tchar* _pFindPath);
	// Find Folder
	FILEGROUP* Find_Folder(PATHS* _pFolderList);
	FILEGROUP* Find_Folder(const _wstring& _strPath);
	// Find Folder List
	list<FILEDATA> Find_FolderList(PATHS* _pFolderList);
	// Open Save Path
	HRESULT Open_SavePath(_wstring* _pOutPath);
	// Open Folder
	HRESULT Open_SaveFolder(_wstring* _pOutPath);
	// Open Root Save Path
	HRESULT Open_FolderSelectRoot(const _wstring& _strPath, const _wstring& _strName, _wstring* _pOutPath);
public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(const _float4& v4ClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
	ID3D11ShaderResourceView* Get_BackBufferSRV() const;
public: /* For Input Device */
	_bool Get_DIKeyDown(const _ubyte& _eKeyID);
	_bool Get_DIKeyUp(const _ubyte& _eKeyID);
	_bool Get_DIKeyPressing(const _ubyte& _eKeyID);
	_bool Get_DIMBtnDown(const CInput_Device::DIMOUSEBUTTON& _eMouseBtn);
	_bool Get_DIMBtnUp(const CInput_Device::DIMOUSEBUTTON& _eMouseBtn);
	_bool Get_DIMBtnPressing(const CInput_Device::DIMOUSEBUTTON& _eMouseBtn);
	_long Get_DIMMoveState(const CInput_Device::DIMOUSEMOVE& _eMouseMove);
	// Get Mouse Position
	_float2 Get_MousePosition();
	_int Judge_MouseOverCenterX(const _float& _fMousePosX, const _float& _fValue = 0.f);
	_int Judge_MouseOverCenterY(const _float& _fMousePosY, const _float& _fValue = 0.f);
	// Setting
	void Set_CursorMode(const CInput_Device::CURSOR_LOCKMODE& _eCursorMode);
	// GetCursorMode
	CInput_Device::CURSOR_LOCKMODE Get_CursorMode();
	// Add
	void Add_KeyInputContainers(const _ubyte& _eByte);
	void Add_MouseInputContainers(const CInput_Device::DIMOUSEBUTTON& _eMouseButton);
	// Get KeyBoard Inputs
	_uint Get_KeyBoardInputs_InContainers(const _ubyte& _eByte);
	// Get Mouse Button Inputs 
	_uint Get_MouseButtonInputs_InContainers(const CInput_Device::DIMOUSEBUTTON& _eMouseButton);
	// If SameInputs
	_bool Is_KeyBoardInput_InContainers(const _ubyte& _eByte);
	// if SameInputs
	_bool Is_MouseButtonInputs_InContainers(const CInput_Device::DIMOUSEBUTTON& _eMouseButton);
	// Reset Containers
	void Reset_KeyBoardContainers();
	void Reset_MouseButtonContainers();
public: /* GamePad Manager */
	_bool Get_PadDown(const GAMEPADKEYS& _eKeyID);
	_bool Get_PadUp(const GAMEPADKEYS& _eKeyID);
	_bool Get_PadPressing(const GAMEPADKEYS& _eKeyID);

	const _float Get_Yaw();
	const _float Get_Pitch();
	const _float Get_Theta();
	const _float Get_Phi();
	const _float Get_Radius();
public: /* Font Manager */
	HRESULT Add_Font(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pFontTag, const _tchar* _pFontFIlePath, CCustomFont** _ppOut = nullptr);
	HRESULT Add_Font_FileData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pFontTag, FILEDATA* _pFileData, CCustomFont** _ppOut = nullptr);
	// Render Font
	HRESULT Render_Font(const _tchar* _pFontTag, const _tchar* _pString, const _float2& _vPos,_fvector& _vColor);
	HRESULT Align_RenderFont(const _tchar* _pFontTag, const _tchar* pString, const _float2& vPosition, _fvector vColor, _bool alignCenter);
	class CCustomFont* Find_Font(const _tchar* _pFontTag);
public: /* RandomCount */
	_uint Get_RandomCount(const _uint& _iCount = 10000);
public: /* For Sound Manager */
	// Get Sound Datas 
	map<const _tchar*, FMOD_SOUND*>* Get_SoundMap();
	// Load Sound Resource
	HRESULT Load_SoundResource(PATHS* _pFolderList);
	// Sound value
	_int  Volume_Up(const CSound_Manager::CHANNELID& _eID, const _float& _fVol);
	_int  Volume_Down(const CSound_Manager::CHANNELID& _eID, const _float& _fVol);
	_int  BGM_Volume_Up(const _float& _fVol);
	_int  BGM_Volume_Down(const _float& _fVol);
	_int  Pause(const CSound_Manager::CHANNELID& _eID);
	_int  Play_Channel(const CSound_Manager::CHANNELID& _eID);
	void Play_In_Silence(const _tchar* pSoundKey, const _float& _fVol, const _float& _fVolDown);
	void Play_Sound_ByCondition(const _tchar* pSoundKey, const _float& _fVol = 0.5f, class CTransform* pTransform = nullptr);
	void Play_SoundW(const _tchar* pSoundKey, const _float& _fVol = 0.5f, class CTransform* pTransform = nullptr);
	void Play_BGM(const _tchar* pSoundKey, const _float& _fVol = 0.5f);
	void Stop_Sound(const CSound_Manager::CHANNELID& _eID);
	void Stop_All();
public: /* For Thread Manager */
	// 스레드 아이디를 저장할 변수를 리턴함
	HRESULT Add_Thread(function<void(void*)> _pData, void* _pArg);
	// Join Thread
	HRESULT Join_Thread();
	// DetachThread
	HRESULT Detach_Thread();
	// End All Thread
	HRESULT End_Threads();
	// Get Thread COunt
	_uint Get_ThreadCount();
public: /* For.Timer_Manager */
	_double Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT Add_Timer(const _tchar* pTimerTag);
	void Update_Timer(const _tchar* pTimerTag);
public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iCurrentLevelIndex, class CLevel* pNextLevel);
	// Get Level Manger
	 _uint Get_CurLevel() const;
public: /*For.Object_Manager */
	// Get Component
		class CComponent* Get_Component(const _uint& _iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, const  _uint& _iIndex = 0);
		// 객체들의 원형 추가
		HRESULT Add_ProtoType(const _tchar* _pProtoTypeTag, CGameObject* _pGameObject);
		// 객체들의 사본을 추가
		HRESULT Clone_GameObject(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag, const _tchar* _pProtoTypeTag, void* _pArg = nullptr);
		// Clone GameObject
		CGameObject* Clone_GameObject_Add(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag, const _tchar* _pProtoTypeTag, void* _pArg = nullptr);
		// Find GameObject
		CGameObject* Find_GameObject(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag, const _uint& _iIndex);
		// Find GameObject List
		list<CGameObject*> Find_GameObjectList(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag);
		// Delete GameObject
		HRESULT Add_DeleteObject(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag, CGameObject* _pGameObject);
		// Clear
		HRESULT Clear_Layer(const _uint& _iLevelIndex);
		// Level Update Time Control
		void Out_LevelUpdateTime(_float*& _pUpdate, _float*& _pLateUpdate);
		void Out_LayerUpdateTime(const _uint& _iLevel, const _tchar* _pLayerTypeTag, _float*& _pUpdate, _float*& _pLateUpdate );

		// Out Layer Tags
		void Out_LayerTags(const _uint& _iLevelIndex, list<const _tchar*>* _pOut);
		_uint Cur_LayerSize(const _uint& _iLevel);

public: /* For.Component_Manager */
		HRESULT Add_ProtoType(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag, CComponent* _pPrototype);
		CComponent* Clone_Component(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag, void* _pArg = nullptr);
		// Find Proto Comp
		CComponent* Get_ProtoComp(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag);
public: /* For.Light_Manager */
		_uint Get_LightsNum();
		const LIGHTDESC* Get_LightDesc(const _uint& _iIndex = 0);
		// Add Light
		HRESULT Add_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tLightDesc, class CLight** _ppOut);
		HRESULT Modify_Light(const LIGHTDESC& _tLightDesc, const _uint& _iIndex);
		HRESULT Active_Light(const _uint& _iIndex, const _bool& _bIsControl);
		HRESULT Delete_Light(const _uint& _iIndex);
		HRESULT Clear_Light();

		HRESULT	Save_LightsData(const _wstring& _strName);
		HRESULT	Load_LightsData(const _wstring& _strName, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

public: /* CTarget_Manager */
		HRESULT Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
			const _tchar* _pRenderTargetTag, const CRenderTarget::RENTDESC& _tDesc);
		HRESULT Add_MRT(const _tchar* _pMRTTag, const _tchar* _pRenderTargetTag);

		HRESULT Begin_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);
		HRESULT End_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);

		ID3D11ShaderResourceView* Get_RenderTarget_SRV(const _tchar* _pRenderTargetTag) const;
		void Out_RenderTarget_Texture(map<_wstring, ID3D11ShaderResourceView*>* _pData);
	#ifdef  _DEBUG
		ID3D11Texture2D* Get_DebugTexture(const _tchar* _pRenderTargetTag);
	#endif
public:
		// NativeConstruct Frust Cull
		HRESULT NativeConstruct_FrustCull(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
			const CFrustCulling::FRUSTDESC& _tFrustDesc);
		// Rendering
		_bool Is_Rendering(class CTransform* _pTransform, const _float& _fRadius = 0.0f);
		// Rendering
		_bool Is_Rendering(const _vector& _vPos, const _float& _fRadius = 0.0f);
		// Rendering Local
		void		Update_LocalMatrix(_fmatrix _WorldMatrix);
		_bool Is_Rendering_InLocal(const _vector& _vPos, const _float& _fRange = 0.f);
public: /* Picking */
		// Picking (Use Late Tick)
		_bool Picking_Mesh(class CGameObject* _pGameObject, class CTransform* _pTransform, class CVIBuffer* _pVIBuffer, _float3* _pOut = nullptr);
		_bool Picking_Mesh(CGameObject* _pGameObject, CTransform* _pTransform, CVIBuffer* _pVIBuffer, _matrix _BoneMatrix);
		// Picking MeshNav
		_bool Picking_MeshNav(class CGameObject* _pGameObject, class CTransform* _pTransform, class CVIBuffer* _pVIBuffer);
		// Get Picking Object
		CPicking::PICKINGDESC* Get_Picking_GOB();
		// Get LengthToMouse Pos
		_float Get_LengthToMousePos(const _float3& _v3Pos);
		// Release Get Objects
		void Release_Pickings();
public: /*  PipeLine */
		_matrix Get_TransformMatrix(const CPipeLine::TRANSFORMSTATE& _eTransState) const;
		_matrix Get_TransformMatrixInv(const CPipeLine::TRANSFORMSTATE& _eTransState);
		_vector Get_CamPosition() const;
		_float3 Get_CamPositionF3() const;
		_float4x4 Get_TransformFloat4x4_TP(const CPipeLine::TRANSFORMSTATE& _eTransState);
		_float4x4 Get_TransformFloat4x4Inv_TP(const CPipeLine::TRANSFORMSTATE& _eTransState) const;
		_float	Get_CamFar() const;
		void Set_Transform(const CPipeLine::TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix);
		// For. Shadow LightMatrix
		_float4x4 Get_LightTransformFloat4x4_TP(const CPipeLine::TRANSFORMSTATE& _eTransState) const;
		_float3 Get_LightPositionF3() const;
		void	Set_LightTransform(const CPipeLine::TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix);
		void	Set_LightViewMatrix(_vector vPosition, _vector vLookAt);
		void	Set_LightProjMatrix(_float fov, _float fAspect, _float fNear, _float fFar);
		_float	Get_LightFar() const;
		void	Set_LightFar(_float fFar) const;
		_vector	Get_WorldToScreen(ID3D11DeviceContext* pContext, _vector vWorldPos);
		_vector Get_ScreenToWorld(ID3D11DeviceContext* pContext, _vector vScreenPos, _vector vDepthWorldPos);

public: /* Target_Manager */
		ID3D11ShaderResourceView* Get_TargetSRV(const _tchar* _pRenderTargetTag) const;

public:/* Object Pooling */
	HRESULT Add_ObjectPool(const _tchar* _pTag, const _tchar* _pProtoTag, const _tchar* _pCloneLayer,
		const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data = nullptr);
	// Add Effect
	HRESULT Add_ObjectPool(const _tchar* _pTag, const _uint& _iLevelIndex, const _uint& _iMaxIndex, const _tchar* _FileName, _uint iSelect);
	// Make Clone List
	HRESULT Make_CloneList(const _tchar* _pTag, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data);
	// Clear List
	HRESULT Clear_List(const _tchar* _pTag);
	// Active Object
	CGameObject* Active_Object(const _tchar* _pTag);
	// Active Object List
	CGameObject* Active_ObjectList(const _tchar* _pTag, vector<CGameObject*>* _pActiveObject, const _uint& _iActiveCount);
	// Get Object
	vector<CGameObject*>* Get_ObjectList(const _tchar* _pTag);
	// 이펙트 호출 및 상태 변경
	HRESULT SetEffectStateCall(const _tchar* _pTag, _fmatrix _matWorld);
	// 이펙트 상태 변경
	HRESULT ChangeEffectState(const _tchar* _pTag, _fmatrix _matWorld);
	// 이펙트가 따라갈 Target 을 설정해줌
	CGameObject* Set_EffectTarget(const _tchar* _pTag, CTransform* pTarget);
public: /* RayCast */

public: /* Effect */
	// Get Effect Maps
	map<wstring, CGameObject*> Get_EffectMaps();
	// Init Effect
	HRESULT Init_Effect(_uint iSelect);
	HRESULT Load_Effect();
	CGameObject* Load_Effect2(const _tchar* _FileName, _uint iSelect);
	CGameObject* Get_Effect(_uint iLevelIndex, wstring pTag);


public: /* Drawing */
	HRESULT	DrawTriangle(const _fvector vPoint1, const _fvector vPoint2, const _fvector vPoint3, CDrawing::VIEWTYPE eType, _fvector color = DirectX::Colors::White);
	HRESULT DrawLine(const _fvector vPoint1, const _fvector vPoint2, CDrawing::VIEWTYPE eType, _fvector color = DirectX::Colors::White);
	HRESULT Draw(const BoundingBox& box, CDrawing::VIEWTYPE eType, _fvector color = DirectX::Colors::White);
	HRESULT Draw(const BoundingOrientedBox& box, CDrawing::VIEWTYPE eType, _fvector color = DirectX::Colors::White);
	HRESULT Draw(const BoundingSphere& sphere, CDrawing::VIEWTYPE eType, _fvector color = DirectX::Colors::White);


private:
		CFilePath_Manager* m_pFilePath_Manager = nullptr;
		CGraphic_Device* m_pGraphic_Device = nullptr;
		CInput_Device* m_pInput_Device = nullptr;
		CGamePad_Device* m_pGamePad_Device = nullptr;
		CFont_Manager* m_pFont_Manager = nullptr;
		CRanCount* m_pRandomCount = nullptr;
		CSound_Manager* m_pSound_Manager = nullptr;
		CThread_Manager* m_pThread_Manager = nullptr;
		CTimer_Manager* m_pTimer_Manager = nullptr;
		CLevel_Manager* m_pLevel_Manager = nullptr;
		CObject_Manager* m_pObject_Manager = nullptr;
		CComponent_Manager* m_pComponent_Manager = nullptr;
		CLight_Manager* m_pLight_Manager = nullptr;
		CTarget_Manager* m_pTarget_Manager = nullptr;
		CFrustCulling* m_pFrustCulling = nullptr;
		CPicking* m_pPicking = nullptr;
		CPipeLine* m_pPipeLine = nullptr;
		CDrawing* m_pDrawing = nullptr;
		CEffect_Manager* m_pEffect_Manager = nullptr;
		CPostProcessing* m_pPostProcessing = nullptr;
		CObjPooling_Manager* m_pObjPoolManager = nullptr;
		// Current Frame Value
		_uint									m_iCurrentFrameValue = 60;
public:
		static void Release_Engine(); /* 엔진에서 사용된 각종 매니져의 메모리 정리. */
public:
		virtual void Free() override;
};

END