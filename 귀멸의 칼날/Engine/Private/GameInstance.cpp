#include "..\Public\GameInstance.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	:
	m_pFilePath_Manager(CFilePath_Manager::Get_Instance())
	, m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pGamePad_Device(CGamePad_Device::Get_Instance())
	, m_pFont_Manager(CFont_Manager::Get_Instance())
	, m_pRandomCount(CRanCount::Get_Instance())
	, m_pSound_Manager(CSound_Manager::Get_Instance())
	, m_pThread_Manager(CThread_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pFrustCulling(CFrustCulling::Get_Instance())
	, m_pPicking(CPicking::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pDrawing(CDrawing::Get_Instance())
	, m_pEffect_Manager(CEffect_Manager::Get_Instance())
	, m_pPostProcessing(CPostProcessing::Get_Instance())
	, m_pObjPoolManager(CObjPooling_Manager::Get_Instance())
{
	Safe_AddRef(m_pFilePath_Manager);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGamePad_Device);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pRandomCount);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pThread_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustCulling);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pDrawing);
	Safe_AddRef(m_pEffect_Manager);
	Safe_AddRef(m_pPostProcessing);
	Safe_AddRef(m_pObjPoolManager);
}

HRESULT CGameInstance::Initialize_Engine(const _uint& _iNumLevels, const HINSTANCE& _hInst, const GRAPHICDESC& GraphicDesc,
	ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWindowed,
		GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDeviceOut, ppContextOut)))
		return E_FAIL;

	if (FAILED(m_pInput_Device->Initialize_Input_Device(_hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pGamePad_Device->Initailize_Inpuit_Device(_hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Objects(_iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(_iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pPicking->NativeConstruct(GraphicDesc.hWnd, *ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->NativeConstruct(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	if (FAILED(m_pEffect_Manager->NativeConstruct(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	/* Initialize PostProcessing */
	if (FAILED(m_pPostProcessing->Initialize(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	if (FAILED(m_pDrawing->Initialize(GraphicDesc.hWnd, *ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	m_pSound_Manager->Initialize();
	return S_OK;
}

void CGameInstance::Tick_Engine(const _double& _dTimeDelta)
{
	if (nullptr == m_pLevel_Manager || nullptr == m_pObject_Manager || nullptr == m_pInput_Device
		|| nullptr == m_pPicking || nullptr == m_pPipeLine || nullptr == m_pGamePad_Device)
		return;
	m_pInput_Device->Update();
	m_pGamePad_Device->Update();
	// Update Proj, View
	m_pFrustCulling->Update_Matrix();
	m_pPicking->Compute_RayInWorldSpace();

	m_pObject_Manager->Tick(_dTimeDelta);
	m_pLevel_Manager->Tick(_dTimeDelta);
}

void CGameInstance::LateTick_Engine(const _double& _dTimeDelta)
{
	if (nullptr == m_pLevel_Manager || nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Late_Tick(_dTimeDelta);
	m_pLevel_Manager->Late_Tick(_dTimeDelta);
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pLevel_Manager->Render();
	return S_OK;
}

HRESULT CGameInstance::Clear_Container(const _uint& _iLevelIndex)
{
	if (nullptr == m_pObject_Manager || nullptr == m_pComponent_Manager)
		return E_FAIL;

	// Object Manager Clear
	if (FAILED(m_pObject_Manager->Clear(_iLevelIndex)))
		return E_FAIL;
	// Component Clear 
	if (FAILED(m_pComponent_Manager->Clear(_iLevelIndex)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGameInstance::Clear_Objects(const _uint& _iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	return m_pObject_Manager->Clear(_iLevelIndex);
}

HRESULT CGameInstance::Clear_Components(const _uint& _iLevelIndex)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Clear(_iLevelIndex);
}

FILEGROUP* CGameInstance::Get_FileGroup()
{
	if (nullptr == m_pFilePath_Manager)
		return nullptr;

	return m_pFilePath_Manager->Get_FileGroup();
}

HRESULT CGameInstance::Load_FirstFolder(const _tchar* _pFindPath)
{
	if (nullptr == m_pFilePath_Manager)
		return E_FAIL;
	return m_pFilePath_Manager->Load_FirstFolder(_pFindPath);
}

FILEGROUP* CGameInstance::Find_Folder(const _wstring& _strPath)
{
	if (nullptr == m_pFilePath_Manager)
		return nullptr;
	return m_pFilePath_Manager->Find_Folder(_strPath);
}

HRESULT CGameInstance::Open_SavePath(_wstring* _pOutPath)
{
	if (nullptr == m_pFilePath_Manager)
		return E_FAIL;
	return m_pFilePath_Manager->Open_SavePath(_pOutPath);
}

HRESULT CGameInstance::Open_SaveFolder(_wstring* _pOutPath)
{
	if (nullptr == m_pFilePath_Manager)
		return E_FAIL;
	return m_pFilePath_Manager->Open_SaveFolder(_pOutPath);
}

HRESULT CGameInstance::Open_FolderSelectRoot(const _wstring& _strPath, const _wstring& _strName, _wstring* _pOutPath)
{
	if (nullptr == m_pFilePath_Manager)
		return E_FAIL;
	return m_pFilePath_Manager->Open_FolderSelectRoot(_strPath, _strName, _pOutPath);
}

FILEGROUP* CGameInstance::Find_Folder(vector<_wstring>* _pFolderList)
{
	if (nullptr == m_pFilePath_Manager)
		return nullptr;
	return m_pFilePath_Manager->Find_Folder(_pFolderList);
}

list<FILEDATA> CGameInstance::Find_FolderList(vector<_wstring>* _pFolderList)
{
	if (nullptr == m_pFilePath_Manager)
		return list<FILEDATA>();

	return m_pFilePath_Manager->Find_FolderList(_pFolderList);
}

HRESULT CGameInstance::Clear_BackBuffer_View(const _float4& v4ClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(v4ClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

ID3D11ShaderResourceView* CGameInstance::Get_BackBufferSRV() const
{
	if (nullptr == m_pGraphic_Device)
		return nullptr;

	return m_pGraphic_Device->Get_BackBufferSRV();
}

_bool CGameInstance::Get_DIKeyDown(const _ubyte& _eKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIKeyDown(_eKeyID);
}

_bool CGameInstance::Get_DIKeyUp(const _ubyte& _eKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIKeyUp(_eKeyID);
}

_bool CGameInstance::Get_DIKeyPressing(const _ubyte& _eKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIKeyPressing(_eKeyID);
}

_bool CGameInstance::Get_DIMBtnDown(const CInput_Device::DIMOUSEBUTTON& _eMouseBtn)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIMBtnDown(_eMouseBtn);
}

_bool CGameInstance::Get_DIMBtnUp(const CInput_Device::DIMOUSEBUTTON& _eMouseBtn)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIMBtnUp(_eMouseBtn);
}

_bool CGameInstance::Get_DIMBtnPressing(const CInput_Device::DIMOUSEBUTTON& _eMouseBtn)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIMBtnPressing(_eMouseBtn);
}

_long CGameInstance::Get_DIMMoveState(const CInput_Device::DIMOUSEMOVE& _eMouseMove)
{
	if (nullptr == m_pInput_Device)
		return -1;
	return m_pInput_Device->Get_DIMMoveState(_eMouseMove);
}

_float2 CGameInstance::Get_MousePosition()
{
	if (nullptr == m_pInput_Device)
		return _float2(0.f, 0.f);
	return m_pInput_Device->Get_MousePosition();
}

_int CGameInstance::Judge_MouseOverCenterX(const _float& _fMousePosX, const _float& _fValue)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Judge_MouseOverCenterX(_fMousePosX, _fValue);
}

_int CGameInstance::Judge_MouseOverCenterY(const _float& _fMousePosY, const _float& _fValue)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Judge_MouseOverCenterY(_fMousePosY, _fValue);
}

void CGameInstance::Set_CursorMode(const CInput_Device::CURSOR_LOCKMODE& _eCursorMode)
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Set_CursorLock(_eCursorMode);
}

CInput_Device::CURSOR_LOCKMODE CGameInstance::Get_CursorMode()
{
	if (nullptr == m_pInput_Device)
		return CInput_Device::CURSOR_END;

	return m_pInput_Device->Get_CursorMode();
}

void CGameInstance::Add_KeyInputContainers(const _ubyte& _eByte)
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Add_KeyInputContainers(_eByte);
}

void CGameInstance::Add_MouseInputContainers(const CInput_Device::DIMOUSEBUTTON& _eMouseButton)
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Add_MouseInputContainers(_eMouseButton);
}

_uint CGameInstance::Get_KeyBoardInputs_InContainers(const _ubyte& _eByte)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Get_KeyBoardInputs_InContainers(_eByte);
}

_uint CGameInstance::Get_MouseButtonInputs_InContainers(const CInput_Device::DIMOUSEBUTTON& _eMouseButton)
{
	if (nullptr == m_pInput_Device)
		return 0;
	return m_pInput_Device->Get_MouseButtonInputs_InContainers(_eMouseButton);
}

_bool CGameInstance::Is_KeyBoardInput_InContainers(const _ubyte& _eByte)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Is_KeyBoardInput_InContainers(_eByte);
}

_bool CGameInstance::Is_MouseButtonInputs_InContainers(const CInput_Device::DIMOUSEBUTTON& _eMouseButton)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Is_MouseButtonInputs_InContainers(_eMouseButton);
}

void CGameInstance::Reset_KeyBoardContainers()
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Reset_KeyBoardContainers();
}

void CGameInstance::Reset_MouseButtonContainers()
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Reset_MouseButtonContainers();
}

_bool CGameInstance::Get_PadDown(const GAMEPADKEYS& _eKeyID)
{
	if (nullptr == m_pGamePad_Device)
		return false;
	return m_pGamePad_Device->Get_PadDown(_eKeyID);
}

_bool CGameInstance::Get_PadUp(const GAMEPADKEYS& _eKeyID)
{
	if (nullptr == m_pGamePad_Device)
		return false;
	return m_pGamePad_Device->Get_PadUp(_eKeyID);
}

_bool CGameInstance::Get_PadPressing(const GAMEPADKEYS& _eKeyID)
{
	if (nullptr == m_pGamePad_Device)
		return false;
	return m_pGamePad_Device->Get_PadPressing(_eKeyID);
}

const _float CGameInstance::Get_Yaw()
{
	if (nullptr == m_pGamePad_Device)
		return 0.f;
	return m_pGamePad_Device->Get_Yaw();
}

const _float CGameInstance::Get_Pitch()
{
	if (nullptr == m_pGamePad_Device)
		return 0.f;
	return m_pGamePad_Device->Get_Pitch();
}

const _float CGameInstance::Get_Theta()
{
	if (nullptr == m_pGamePad_Device)
		return 0.f;
	return m_pGamePad_Device->Get_Theta();
}

const _float CGameInstance::Get_Phi()
{
	if (nullptr == m_pGamePad_Device)
		return 0.f;
	return m_pGamePad_Device->Get_Phi();
}

const _float CGameInstance::Get_Radius()
{
	if (nullptr == m_pGamePad_Device)
		return 0.f;
	return m_pGamePad_Device->Get_Radius();
}

HRESULT CGameInstance::Add_Font(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFontTag,
	const _tchar* _pFontFIlePath, CCustomFont** _ppOut)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
	return m_pFont_Manager->Add_Font(_pDevice, _pContext, _pFontTag, _pFontFIlePath, _ppOut);
}

HRESULT CGameInstance::Add_Font_FileData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFontTag,
	FILEDATA* _pFileData, CCustomFont** _ppOut)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
	return m_pFont_Manager->Add_Font_FileData(_pDevice, _pContext, _pFontTag, _pFileData, _ppOut);
}

HRESULT CGameInstance::Render_Font(const _tchar* _pFontTag, const _tchar* _pString, const _float2& _vPos, _fvector& _vColor)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
	return m_pFont_Manager->Render_Font(_pFontTag, _pString, _vPos, _vColor);
}

HRESULT CGameInstance::Align_RenderFont(const _tchar* _pFontTag, const _tchar* pString, const _float2& vPosition, _fvector vColor, _bool alignCenter)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
	return m_pFont_Manager->Align_RenderFont(_pFontTag, pString, vPosition, vColor, alignCenter);
}



CCustomFont* CGameInstance::Find_Font(const _tchar* _pFontTag)
{
	if (nullptr == m_pFont_Manager)
		return nullptr;

	return m_pFont_Manager->Find_Font(_pFontTag);
}

_uint CGameInstance::Get_RandomCount(const _uint& _iCount)
{
	if (nullptr == m_pRandomCount)
		return 0;
	return m_pRandomCount->Get_RandomCount(_iCount);
}

map<const _tchar*, FMOD_SOUND*>* CGameInstance::Get_SoundMap()
{
	if (nullptr == m_pSound_Manager)
		return nullptr;
	return m_pSound_Manager->Get_SoundMap();
}

HRESULT CGameInstance::Load_SoundResource(PATHS* _pFolderList)
{
	if (nullptr == m_pSound_Manager)
		return E_FAIL;
	return m_pSound_Manager->LoadSoundFile(_pFolderList);
}

_int CGameInstance::Volume_Up(const CSound_Manager::CHANNELID& _eID, const _float& _fVol)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->Volume_Up(_eID, _fVol);
}

_int CGameInstance::Volume_Down(const CSound_Manager::CHANNELID& _eID, const _float& _fVol)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->Volume_Down(_eID, _fVol);
}

_int CGameInstance::BGM_Volume_Up(const _float& _fVol)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->BGM_Volume_Up(_fVol);
}

_int CGameInstance::BGM_Volume_Down(const _float& _fVol)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->BGM_Volume_Down(_fVol);
}

_int CGameInstance::Pause(const CSound_Manager::CHANNELID& _eID)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->Pause(_eID);
}

_int CGameInstance::Play_Channel(const CSound_Manager::CHANNELID& _eID)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->Play_Channel(_eID);
}

void CGameInstance::Play_In_Silence(const _tchar* pSoundKey, const _float& _fVol, const _float& _fVolDown)
{
	if (nullptr == m_pSound_Manager)
		return;
	return m_pSound_Manager->Play_In_Silence(pSoundKey, _fVol, _fVolDown);
}

void CGameInstance::Play_SoundW(const _tchar* pSoundKey, const _float& _fVol, CTransform* pTransform)
{
	if (nullptr == m_pSound_Manager)
		return;
	return m_pSound_Manager->Play_SoundW(pSoundKey, _fVol, pTransform);
}

void CGameInstance::Play_Sound_ByCondition(const _tchar* pSoundKey, const _float& _fVol, CTransform* pTransform)
{
	if (nullptr == m_pSound_Manager)
		return;
	return m_pSound_Manager->Play_Sound_ByCondition(pSoundKey, _fVol, pTransform);
}

void CGameInstance::Play_BGM(const _tchar* pSoundKey, const _float& _fVol)
{
	if (nullptr == m_pSound_Manager)
		return;
	return m_pSound_Manager->Play_BGM(pSoundKey, _fVol);
}

void CGameInstance::Stop_Sound(const CSound_Manager::CHANNELID& _eID)
{
	if (nullptr == m_pSound_Manager)
		return;
	return m_pSound_Manager->Stop_Sound(_eID);
}

void CGameInstance::Stop_All()
{
	if (nullptr == m_pSound_Manager)
		return;
	return m_pSound_Manager->Stop_All();
}

HRESULT CGameInstance::Add_Thread(function<void(void*)> _pData, void* _pArg)
{
	if (nullptr == m_pThread_Manager)
		return E_FAIL;

	return m_pThread_Manager->Add_Thread(_pData, _pArg);
}

HRESULT CGameInstance::Join_Thread()
{
	if (nullptr == m_pThread_Manager)
		return E_FAIL;

	return m_pThread_Manager->Join_Thread();
}

HRESULT CGameInstance::Detach_Thread()
{
	if (nullptr == m_pThread_Manager)
		return E_FAIL;

	return m_pThread_Manager->Detach_Thread();
}

HRESULT CGameInstance::End_Threads()
{
	if (nullptr == m_pThread_Manager)
		return E_FAIL;

	return m_pThread_Manager->End_Threads();
}

_uint CGameInstance::Get_ThreadCount()
{
	if (nullptr == m_pThread_Manager)
		return 0;
	return m_pThread_Manager->Get_ThreadCount();
}

_double CGameInstance::Get_TimeDelta(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

void CGameInstance::Update_Timer(const _tchar* pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iCurrentLevelIndex, CLevel* pNextLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Change_Level(iCurrentLevelIndex, pNextLevel);
}

_uint CGameInstance::Get_CurLevel() const
{
	if (nullptr == m_pLevel_Manager)
		return (_uint)10000000000;

	return m_pLevel_Manager->Get_CurLevel();
}

CComponent* CGameInstance::Get_Component(const _uint& _iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& _iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(_iLevelIndex, pLayerTag, pComponentTag, _iIndex);
}

HRESULT CGameInstance::Add_ProtoType(const _tchar* _pProtoTypeTag, CGameObject* _pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_ProtoType(_pProtoTypeTag, _pGameObject);
}

HRESULT CGameInstance::Clone_GameObject(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag, const _tchar* _pProtoTypeTag, void* _pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Clone_GameObject(_iLevelIndex, _pLayerTypeTag, _pProtoTypeTag, _pArg);
}

CGameObject* CGameInstance::Clone_GameObject_Add(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag, const _tchar* _pProtoTypeTag, void* _pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject_Add(_iLevelIndex, _pLayerTypeTag, _pProtoTypeTag, _pArg);
}

CGameObject* CGameInstance::Find_GameObject(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag, const _uint& _iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(_iLevelIndex, _pLayerTypeTag, _iIndex);
}

list<CGameObject*> CGameInstance::Find_GameObjectList(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag)
{
	if (nullptr == m_pObject_Manager)
		return list<CGameObject*>();

	return m_pObject_Manager->Find_GameObjectList(_iLevelIndex, _pLayerTypeTag);
}

HRESULT CGameInstance::Add_DeleteObject(const _uint& _iLevelIndex, const _tchar* _pLayerTypeTag, CGameObject* _pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	return m_pObject_Manager->Add_DeleteObject(_iLevelIndex, _pLayerTypeTag, _pGameObject);
}

HRESULT CGameInstance::Clear_Layer(const _uint& _iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	return m_pObject_Manager->Clear(_iLevelIndex);
}

void CGameInstance::Out_LevelUpdateTime(_float*& _pUpdate, _float*& _pLateUpdate)
{
	if (nullptr == m_pObject_Manager)
		return;
	return m_pObject_Manager->Out_LevelUpdateTime(_pUpdate, _pLateUpdate);
}

void CGameInstance::Out_LayerUpdateTime(const _uint& _iLevel, const _tchar* _pLayerTypeTag, _float*& _pUpdate, _float*& _pLateUpdate)
{
	if (nullptr == m_pObject_Manager)
		return;
	return m_pObject_Manager->Out_LayerUpdateTime(_iLevel, _pLayerTypeTag, _pUpdate, _pLateUpdate);
}


void CGameInstance::Out_LayerTags(const _uint& _iLevelIndex, list<const _tchar*>* _pOut)
{
	if (nullptr == m_pObject_Manager)
		return;
	return m_pObject_Manager->Out_LayerTags(_iLevelIndex, _pOut);
}
_uint CGameInstance::Cur_LayerSize(const _uint& _iLevel)
{
	if (nullptr == m_pObject_Manager)
		return 0;
	return m_pObject_Manager->Cur_LayerSize(_iLevel);
}

HRESULT CGameInstance::Add_ProtoType(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag, CComponent* _pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;
	return m_pComponent_Manager->Add_Prototype(_iLevelIndex, _pPrototypeTag, _pPrototype);
}

CComponent* CGameInstance::Clone_Component(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag, void* _pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;
	return m_pComponent_Manager->Clone_Component(_iLevelIndex, _pPrototypeTag, _pArg);
}

CComponent* CGameInstance::Get_ProtoComp(const _uint& _iLevelIndex, const _tchar* _pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;
	return m_pComponent_Manager->Get_ProtoComp(_iLevelIndex, _pPrototypeTag);
}

_uint CGameInstance::Get_LightsNum()
{
	if (nullptr == m_pLight_Manager)
		return 0;

	return m_pLight_Manager->Get_LightsNum();
}

const LIGHTDESC* CGameInstance::Get_LightDesc(const _uint& _iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(_iIndex);
}

HRESULT CGameInstance::Add_Light(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHTDESC& _tLightDesc, CLight** _ppOut)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	return m_pLight_Manager->Add_Light(_pDevice, _pContext, _tLightDesc, _ppOut);
}

HRESULT CGameInstance::Modify_Light(const LIGHTDESC& _tLightDesc, const _uint& _iIndex)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	return m_pLight_Manager->Modify_Light(_tLightDesc, _iIndex);
}

HRESULT CGameInstance::Active_Light(const _uint& _iIndex, const _bool& _bIsControl)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	return m_pLight_Manager->Active_Light(_iIndex, _bIsControl);
}

HRESULT CGameInstance::Delete_Light(const _uint& _iIndex)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	return m_pLight_Manager->Delete_Light(_iIndex);
}

HRESULT CGameInstance::Clear_Light()
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	return m_pLight_Manager->Clear_Light();
}

HRESULT	CGameInstance::Save_LightsData(const _wstring& _strName)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	return m_pLight_Manager->Save_LightsData(_strName);
}

HRESULT	CGameInstance::Load_LightsData(const _wstring& _strName, ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;
	return m_pLight_Manager->Load_LightsData(_strName, _pDevice, _pContext);
}

HRESULT CGameInstance::Add_RenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,
	const _tchar* _pRenderTargetTag, const CRenderTarget::RENTDESC& _tDesc)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	return m_pTarget_Manager->Add_RenderTarget(_pDevice, _pContext, _pRenderTargetTag, _tDesc);
}

HRESULT CGameInstance::Add_MRT(const _tchar* _pMRTTag, const _tchar* _pRenderTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	return m_pTarget_Manager->Add_MRT(_pMRTTag, _pRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	return m_pTarget_Manager->Begin_MRT(_pContext, _pMRTTag);
}

HRESULT CGameInstance::End_MRT(ID3D11DeviceContext* _pContext, const _tchar* _pMRTTag)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	return m_pTarget_Manager->Begin_MRT(_pContext, _pMRTTag);
}
ID3D11ShaderResourceView* CGameInstance::Get_RenderTarget_SRV(const _tchar* _pRenderTargetTag) const
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;
	return m_pTarget_Manager->Get_SRV(_pRenderTargetTag);
}

void CGameInstance::Out_RenderTarget_Texture(map<_wstring, ID3D11ShaderResourceView*>* _pData)
{
	if (nullptr == m_pTarget_Manager)
		return;

	m_pTarget_Manager->Out_RenderTarget_Texture(_pData);
}

#ifdef  _DEBUG
ID3D11Texture2D* CGameInstance::Get_DebugTexture(const _tchar* _pRenderTargetTag)
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;
	return m_pTarget_Manager->Get_DebugTexture(_pRenderTargetTag);
}
#endif
HRESULT CGameInstance::NativeConstruct_FrustCull(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CFrustCulling::FRUSTDESC& _tFrustDesc)
{
	if (nullptr == m_pFrustCulling)
		return E_FAIL;
	return m_pFrustCulling->NativeConstruct_FrustCull(_pDevice, _pContext, _tFrustDesc);
}

_bool CGameInstance::Is_Rendering(CTransform* _pTransform, const _float& _fRadius)
{
	if (nullptr == m_pFrustCulling)
		return false;
	return m_pFrustCulling->Is_RenderingSphere(_pTransform, _fRadius);
}

_bool CGameInstance::Is_Rendering(const _vector& _vPos, const _float& _fRadius)
{
	if (nullptr == m_pFrustCulling)
		return false;
	return m_pFrustCulling->Is_RenderingSphere(_vPos, _fRadius);
}

void CGameInstance::Update_LocalMatrix(_fmatrix _WorldMatrix)
{
	if (nullptr == m_pFrustCulling)
		return;

	m_pFrustCulling->Update_LocalMatrix(_WorldMatrix);
}

_bool CGameInstance::Is_Rendering_InLocal(const _vector& _vPos, const _float& _fRange)
{
	if (nullptr == m_pFrustCulling)
		return false;
	return m_pFrustCulling->Is_Rendering_InLocal(_vPos, _fRange);
}

_bool CGameInstance::Picking_Mesh(CGameObject* _pGameObject, CTransform* _pTransform, CVIBuffer* _pVIBuffer, _float3* _pOut)
{
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Picking_Mesh(_pGameObject, _pTransform, _pVIBuffer, _pOut);
}

_bool CGameInstance::Picking_Mesh(CGameObject* _pGameObject, CTransform* _pTransform, CVIBuffer* _pVIBuffer, _matrix _BoneMatrix)
{
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Picking_Mesh(_pGameObject, _pTransform, _pVIBuffer, _BoneMatrix);
}

_bool CGameInstance::Picking_MeshNav(CGameObject* _pGameObject, CTransform* _pTransform, CVIBuffer* _pVIBuffer)
{
	if (nullptr == m_pPicking)
		return false;
	return m_pPicking->Picking_Mesh_Nav(_pGameObject, _pTransform, _pVIBuffer);
}

CPicking::PICKINGDESC* CGameInstance::Get_Picking_GOB()
{
	if (nullptr == m_pPicking)
		return nullptr;
	return m_pPicking->Get_Picking_GOB();
}

void CGameInstance::Release_Pickings()
{
	if (nullptr == m_pPicking)
		return;
	return m_pPicking->Release_Pickings();
}

_matrix CGameInstance::Get_TransformMatrix(const CPipeLine::TRANSFORMSTATE& _eTransState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();
	return m_pPipeLine->Get_TransformMatrix(_eTransState);
}

_matrix CGameInstance::Get_TransformMatrixInv(const CPipeLine::TRANSFORMSTATE& _eTransState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();
	return m_pPipeLine->Get_TransformMatrixInv(_eTransState);
}

_vector CGameInstance::Get_CamPosition() const
{
	if (nullptr == m_pPipeLine)
		return XMVectorSet(0.f, 0.f, 0.f, 0.f);
	return m_pPipeLine->Get_CamPosition();
}

_float3 CGameInstance::Get_CamPositionF3() const
{
	if (nullptr == m_pPipeLine)
		return _float3(0.f, 0.f, 0.f);
	return m_pPipeLine->Get_CamPositionF3();
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(const CPipeLine::TRANSFORMSTATE& _eTransState)
{
	if (nullptr == m_pPipeLine)
	{
		_float4x4 m;
		XMStoreFloat4x4(&m, XMMatrixIdentity());
		return m;
	}
	return m_pPipeLine->Get_TransformFloat4x4_TP(_eTransState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4Inv_TP(const CPipeLine::TRANSFORMSTATE& _eTransState) const
{
	if (nullptr == m_pPipeLine)
	{
		_float4x4 m;
		XMStoreFloat4x4(&m, XMMatrixIdentity());
		return m;
	}
	return m_pPipeLine->Get_TransformFloat4x4Inv_TP(_eTransState);
}

_float CGameInstance::Get_CamFar() const
{
	if (nullptr == m_pPipeLine)
		return 0.f;
	return m_pPipeLine->Get_CamFar();
}

void CGameInstance::Set_Transform(const CPipeLine::TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix)
{
	if (nullptr == m_pPipeLine)
		return;
	m_pPipeLine->Set_Transform(_eState, _StateMatrix);
}

_float4x4 CGameInstance::Get_LightTransformFloat4x4_TP(const CPipeLine::TRANSFORMSTATE& _eTransState) const
{
	if (nullptr == m_pPipeLine)
	{
		_float4x4 m;
		XMStoreFloat4x4(&m, XMMatrixIdentity());
		return m;
	}
	return m_pPipeLine->Get_LightTransformFloat4x4_TP(_eTransState);
}

_float3 CGameInstance::Get_LightPositionF3() const
{
	if (nullptr == m_pPipeLine)
		return _float3(0.f, 0.f, 0.f);
	return m_pPipeLine->Get_LightPositionF3();
}

void CGameInstance::Set_LightTransform(const CPipeLine::TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix)
{
	if (nullptr == m_pPipeLine)
		return;
	return m_pPipeLine->Set_LightTransform(_eState, _StateMatrix);
}

void CGameInstance::Set_LightViewMatrix(_vector vPosition, _vector vLookAt)
{
	if (nullptr == m_pPipeLine)
		return;
	m_pPipeLine->Set_LightViewMatrix(vPosition, vLookAt);
}

void CGameInstance::Set_LightProjMatrix(_float fov, _float fAspect, _float fNear, _float fFar)
{
	if (nullptr == m_pPipeLine)
		return;
	m_pPipeLine->Set_LightProjMatrix(fov, fAspect, fNear, fFar);
}

_float CGameInstance::Get_LightFar() const
{
	if (nullptr == m_pPipeLine)
		return 0.f;
	return m_pPipeLine->Get_LightFar();
}

void CGameInstance::Set_LightFar(_float fFar) const
{
	if (nullptr == m_pPipeLine)
		return;
	return m_pPipeLine->Set_LightFar(fFar);
}

_vector CGameInstance::Get_WorldToScreen(ID3D11DeviceContext* pContext, _vector vWorldPos)
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();
	return m_pPipeLine->Get_WorldToScreen(pContext, vWorldPos);
}

_vector CGameInstance::Get_ScreenToWorld(ID3D11DeviceContext* pContext, _vector vScreenPos, _vector vDepthWorldPos)
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();
	return m_pPipeLine->Get_ScreenToWorld(pContext, vScreenPos, vDepthWorldPos);
}

ID3D11ShaderResourceView* CGameInstance::Get_TargetSRV(const _tchar* _pRenderTargetTag) const
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;
	return m_pTarget_Manager->Get_SRV(_pRenderTargetTag);
}

HRESULT CGameInstance::Add_ObjectPool(const _tchar* _pTag, const _tchar* _pProtoTag, const _tchar* _pCloneLayer, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data)
{
	if (nullptr == m_pObjPoolManager)
		return E_FAIL;
	return m_pObjPoolManager->Add_ObjectPool(_pTag, _pProtoTag, _pCloneLayer, _iLevelIndex, _iMaxIndex, _Data);
}

HRESULT CGameInstance::Add_ObjectPool(const _tchar* _pTag, const _uint& _iLevelIndex, const _uint& _iMaxIndex, const _tchar* _FileName, _uint iSelect)
{
	if (nullptr == m_pObjPoolManager)
		return E_FAIL;
	return m_pObjPoolManager->Add_ObjectPool(_pTag, _iLevelIndex, _iMaxIndex, _FileName, iSelect);
}

HRESULT CGameInstance::Make_CloneList(const _tchar* _pTag, const _uint& _iLevelIndex, const _uint& _iMaxIndex, void* _Data)
{
	if (nullptr == m_pObjPoolManager)
		return E_FAIL;
	return m_pObjPoolManager->Make_CloneList(_pTag, _iLevelIndex, _iMaxIndex, _Data);
}

HRESULT CGameInstance::Clear_List(const _tchar* _pTag)
{
	if (nullptr == m_pObjPoolManager)
		return E_FAIL;
	return m_pObjPoolManager->Clear_List(_pTag);
}

CGameObject* CGameInstance::Active_Object(const _tchar* _pTag)
{
	if (nullptr == m_pObjPoolManager)
		return nullptr;
	return m_pObjPoolManager->Active_Object(_pTag);
}

CGameObject* CGameInstance::Active_ObjectList(const _tchar* _pTag, vector<CGameObject*>* _pActiveObject, const _uint& _iActiveCount)
{
	if (nullptr == m_pObjPoolManager)
		return nullptr;
	return m_pObjPoolManager->Active_ObjectList(_pTag, _pActiveObject, _iActiveCount);
}

vector<CGameObject*>* CGameInstance::Get_ObjectList(const _tchar* _pTag)
{
	if (nullptr == m_pObjPoolManager)
		return nullptr;
	return m_pObjPoolManager->Get_ObjectList(_pTag);
}

HRESULT CGameInstance::SetEffectStateCall(const _tchar* _pTag, _fmatrix _matWorld)
{
	if (nullptr == m_pObjPoolManager)
		return E_FAIL;

	CGameObject* pEffect = m_pObjPoolManager->Active_Object(_pTag);
	if (nullptr == pEffect)
		return E_FAIL;

	static_cast<CEffectObject*>(pEffect)->SetEffectState(_matWorld);
	//static_cast<CEffectObject*>(pEffect)->Set_Play(true);

	return S_OK;
}

HRESULT CGameInstance::ChangeEffectState(const _tchar* _pTag, _fmatrix _matWorld)
{
	if (nullptr == m_pObjPoolManager)
		return E_FAIL;

	CGameObject* pEffect = m_pObjPoolManager->Get_CurObject(_pTag);
	if (nullptr == pEffect)
		return E_FAIL;

	static_cast<CEffectObject*>(pEffect)->SetEffectState(_matWorld);

	return S_OK;
}

CGameObject* CGameInstance::Set_EffectTarget(const _tchar* _pTag, CTransform* pTarget)
{
	if (nullptr == m_pObjPoolManager)
		return nullptr;

	CGameObject* pEffect = m_pObjPoolManager->Active_Object(_pTag);
	if (nullptr == pEffect)
		return nullptr;

	static_cast<CEffectObject*>(pEffect)->Set_TargetTransform(pTarget);
	static_cast<CEffectObject*>(pEffect)->Set_Play(true);

	return pEffect;
}

map<wstring, CGameObject*> CGameInstance::Get_EffectMaps()
{
	if (nullptr == m_pEffect_Manager)
		return map<wstring, CGameObject*>();

	return m_pEffect_Manager->Get_EffectMaps();
}

HRESULT CGameInstance::Init_Effect(_uint iSelect)
{
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

	return m_pEffect_Manager->Init_Effects(iSelect);
}

HRESULT CGameInstance::Load_Effect()
{
	if (nullptr == m_pEffect_Manager)
		return E_FAIL;

	return m_pEffect_Manager->Load_Effect();
}

CGameObject* CGameInstance::Load_Effect2(const _tchar* _FileName, _uint iSelect)
{
	if (nullptr == m_pEffect_Manager)
		return nullptr;

	return m_pEffect_Manager->Load_Effect2(_FileName, iSelect);
}

CGameObject* CGameInstance::Get_Effect(_uint iLevelIndex, wstring pTag)
{
	if (nullptr == m_pEffect_Manager)
		return nullptr;

	return m_pEffect_Manager->Get_Effect(iLevelIndex, pTag);
}

HRESULT CGameInstance::DrawTriangle(const _fvector vPoint1, const _fvector vPoint2, const _fvector vPoint3, CDrawing::VIEWTYPE eType, _fvector color)
{
	if (nullptr == m_pDrawing)
		return E_FAIL;

	return m_pDrawing->DrawTriangle(vPoint1, vPoint2, vPoint3, eType, color);
}

HRESULT CGameInstance::DrawLine(const _fvector vPoint1, const _fvector vPoint2, CDrawing::VIEWTYPE eType, _fvector color)
{
	if (nullptr == m_pDrawing)
		return E_FAIL;

	return m_pDrawing->DrawLine(vPoint1, vPoint2, eType, color);
}

HRESULT CGameInstance::Draw(const BoundingBox& box, CDrawing::VIEWTYPE eType, _fvector color)
{
	if (nullptr == m_pDrawing)
		return E_FAIL;

	return m_pDrawing->Draw(box, eType, color);
}

HRESULT CGameInstance::Draw(const BoundingOrientedBox& box, CDrawing::VIEWTYPE eType, _fvector color)
{
	if (nullptr == m_pDrawing)
		return E_FAIL;

	return m_pDrawing->Draw(box, eType, color);
}

HRESULT CGameInstance::Draw(const BoundingSphere& sphere, CDrawing::VIEWTYPE eType, _fvector color)
{
	if (nullptr == m_pDrawing)
		return E_FAIL;

	return m_pDrawing->Draw(sphere, eType, color);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Destroy_Instance();
	CPostProcessing::Destroy_Instance();
	CDrawing::Destroy_Instance();
	CPicking::Destroy_Instance();
	CFrustCulling::Destroy_Instance();
	CTarget_Manager::Destroy_Instance();
	CLight_Manager::Destroy_Instance();
	CObjPooling_Manager::Destroy_Instance();
	CEffect_Manager::Destroy_Instance();
	CComponent_Manager::Destroy_Instance();
	CObject_Manager::Destroy_Instance();
	CPipeLine::Destroy_Instance();
	CLevel_Manager::Destroy_Instance();
	CTimer_Manager::Destroy_Instance();
	CThread_Manager::Destroy_Instance();
	CSound_Manager::Destroy_Instance();
	CRanCount::Destroy_Instance();
	CFont_Manager::Destroy_Instance();
	CGamePad_Device::Destroy_Instance();
	CInput_Device::Destroy_Instance();
	CGraphic_Device::Destroy_Instance();
	CFilePath_Manager::Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPostProcessing);
	Safe_Release(m_pDrawing);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pPicking);
	Safe_Release(m_pFrustCulling);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pObjPoolManager);
	Safe_Release(m_pEffect_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pThread_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pRandomCount);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pGamePad_Device);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pFilePath_Manager);
}
