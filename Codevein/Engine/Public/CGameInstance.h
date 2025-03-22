#pragma once
#include "CFile_Manager.h"
#include "CGraphic_Device.h"
#include "CInput_Device.h"
#include "CFont_Manager.h"
#include "CRandomCount.h"
#include "CLevel_Manager.h"
#include "CTimer_Manager.h"
#include "CSound_Manager.h"
#include "CThread_Manager.h"
#include "CObject_Manager.h"
#include "CComponent_Manager.h"
#include "CLight_Manager.h"
#include "CTarget_Manager.h"
#include "CFrustCulling.h"
#include "CPicking.h"
#include "CPipeLine.h"
#include "CRayCast.h"
#include "CFuncClass.h"

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
public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(const _float4& v4ClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();
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
public: /* Font Manager */
	HRESULT Add_Font(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pFontTag, const _tchar* _pFontFIlePath, CCustomFont** _ppOut = nullptr);
	HRESULT Add_Font_FileData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pFontTag, FILEDATA* _pFileData, CCustomFont** _ppOut = nullptr);

	// Render Font
	HRESULT Render_Font(const _tchar* _pFontTag, const _tchar* _pString, const _float2& _vPos, _fvector& _vColor);
	class CCustomFont* Find_Font(const _tchar* _pFontTag);
public: /* RandomCount */
	_uint Get_RandomCount(const _uint& _iCount = 10000);
public: /* For Sound Manager */
	// Get Sound Datas 
	map<const _tchar*, Sound*>* Get_SoundMap();
	// Load Sound Resource
	HRESULT Load_SoundResource(PATHS* _pFolderList);
	// Sound value
	_int  Volume_Up(const CSound_Manager::CHANNELID& _eID, const _float& _fVol);
	_int  Volume_Down(const CSound_Manager::CHANNELID& _eID, const _float& _fVol);
	_int  BGM_Volume_Up(const _float& _fVol);
	_int  BGM_Volume_Down(const _float& _fVol);
	_int  Pause(const CSound_Manager::CHANNELID& _eID);
	void Play_SoundW(const _tchar* pSoundKey, const CSound_Manager::CHANNELID& _eID, const _float& _fVol);
	void Play_BGM(const _tchar* pSoundKey);
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
	HRESULT Clone_GameObject(const _uint & _iLevelIndex, const _tchar* _pLayerTypeTag, const _tchar* _pProtoTypeTag, void* _pArg = nullptr);
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
public: /* For.Component_Manager */
	HRESULT Add_ProtoType(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag, CComponent* _pPrototype);
	CComponent* Clone_Component(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag, void* _pArg = nullptr);
	// Find Proto Comp
	CComponent* Get_ProtoComp(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag);
public: /* For.Light_Manager */
	const LIGHTDESC* Get_LightDesc(const _uint& _iIndex = 0);
	// Add Light
	HRESULT Add_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tLightDesc, class CLight** _ppOut);
	HRESULT Modify_Light(const LIGHTDESC& _tLightDesc, const _uint& _iIndex);
	HRESULT Active_Light(const _uint& _iIndex, const _bool& _bIsControl);
	HRESULT Delete_Light(const _uint& _iIndex);
	HRESULT Clear_Light();
public: /* CTarget_Manager */
	HRESULT Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
		const _tchar* _pRenderTargetTag, const CRenderTarget::RENTDESC& _tDesc);
	HRESULT Add_MRT(const _tchar* _pMRTTag, const _tchar* _pRenderTargetTag);

	HRESULT Begin_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag);

	ID3D11ShaderResourceView* Get_RenderTarget_SRV(const _tchar* _pRenderTargetTag) const;
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
	_matrix Get_TransformMatrix(const CPipeLine::TRANSFORMSTATE& _eTransState);
	_vector Get_CamPosition() const;
	_float3 Get_CamPositionF3() const;
	_float4x4 Get_TransformFloat4x4_TP(const CPipeLine::TRANSFORMSTATE& _eTransState);
	void Set_Transform(const CPipeLine::TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix);

public: /* RayCast */

private:
	CFile_Manager*								m_pFile_Manager = nullptr;
	CGraphic_Device*							m_pGraphic_Device = nullptr;
	CInput_Device*								m_pInput_Device = nullptr;
	CFont_Manager*							m_pFont_Manager = nullptr;
	CRandomCount*								m_pRandomCount = nullptr;
	CSound_Manager*							m_pSound_Manager = nullptr;
	CThread_Manager*						m_pThread_Manager = nullptr;
	CTimer_Manager*							m_pTimer_Manager = nullptr;
	CLevel_Manager*							m_pLevel_Manager = nullptr;
	CObject_Manager*							m_pObject_Manager = nullptr;
	CComponent_Manager*				m_pComponent_Manager = nullptr;
	CLight_Manager*							m_pLight_Manager = nullptr;
	CTarget_Manager*							m_pTarget_Manager = nullptr;
	CFrustCulling*									m_pFrustCulling = nullptr;
	CPicking*											m_pPicking = nullptr;
	CPipeLine*										m_pPipeLine = nullptr;
	CRayCast*											 m_pRayCast = nullptr;
public:
	static void Release_Engine(); /* 엔진에서 사용된 각종 매니져의 메모리 정리. */
public:
	virtual void Free() override;
};

END