#include "..\Public\CGameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
:	  m_pFile_Manager(CFile_Manager::Get_Instance())
	, m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pFont_Manager(CFont_Manager::Get_Instance())
	, m_pRandomCount(CRandomCount::Get_Instance())
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
	, m_pRayCast(CRayCast::Get_Instance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pRandomCount);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pThread_Manager);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pFile_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pFrustCulling);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pRayCast);
}

HRESULT CGameInstance::Initialize_Engine(const _uint & _iNumLevels, const HINSTANCE & _hInst, const GRAPHICDESC & GraphicDesc, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppContextOut)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 그래픽 디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWindowed, 
		GraphicDesc.iWinCX, GraphicDesc.iWinCY, ppDeviceOut, ppContextOut)))
		return E_FAIL;

	/* 입력 디바이스 초기화. */
	if (FAILED(m_pInput_Device->Initialize_Input_Device(_hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 오브젝트 매니져 준비 */
	if (FAILED(m_pObject_Manager->Reserve_Objects(_iNumLevels)))
		return E_FAIL;

	/* 컴포넌트 매니져 준비 */
	if (FAILED(m_pComponent_Manager->Reserve_Container(_iNumLevels)))
		return E_FAIL;

	/*  픽킹 */
	if (FAILED(m_pPicking->NativeConstruct(GraphicDesc .hWnd, *ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	/*  픽킹 */
	if (FAILED(m_pTarget_Manager->NativeConstruct(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	m_pSound_Manager->Initialize();
	return S_OK;
}

void CGameInstance::Tick_Engine(const _double & _dTimeDelta)
{
	if (nullptr == m_pLevel_Manager || nullptr == m_pObject_Manager || nullptr == m_pInput_Device
		|| nullptr == m_pPicking || nullptr == m_pPipeLine)
		return;
	/* 입력장치의 상태 조사. */
	m_pInput_Device->Update();
	// Update Proj, View
	m_pPipeLine->Tick();
	m_pFrustCulling->Update_Matrix();
	m_pPicking->Compute_RayInWorldSpace();

	/* 오브젝트들의 업데이트를 수행한다. */
	m_pObject_Manager->Tick(_dTimeDelta);
	m_pLevel_Manager->Tick(_dTimeDelta);
}

void CGameInstance::LateTick_Engine(const _double & _dTimeDelta)
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

	m_pGraphic_Device->Set_TargetView();
	m_pLevel_Manager->Render();
	return S_OK;
}

HRESULT CGameInstance::Clear_Container(const _uint & _iLevelIndex)
{
	if (nullptr == m_pObject_Manager || nullptr == m_pComponent_Manager)
		return E_FAIL;

	// Object Manager Clear
	if(FAILED(m_pObject_Manager->Clear(_iLevelIndex)))
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
	if (nullptr == m_pFile_Manager)
		return nullptr;

	return m_pFile_Manager->Get_FileGroup();
}

HRESULT CGameInstance::Load_FirstFolder(const _tchar* _pFindPath)
{
	if (nullptr == m_pFile_Manager)
		return E_FAIL;
	return m_pFile_Manager->Load_FirstFolder(_pFindPath);
}

FILEGROUP* CGameInstance::Find_Folder(const _wstring& _strPath)
{
	if (nullptr == m_pFile_Manager)
		return nullptr;
	return m_pFile_Manager->Find_Folder(_strPath);
}

FILEGROUP* CGameInstance::Find_Folder(vector<_wstring>* _pFolderList)
{
	if (nullptr == m_pFile_Manager)
		return nullptr;
	return m_pFile_Manager->Find_Folder(_pFolderList);
}

list<FILEDATA> CGameInstance::Find_FolderList(vector<_wstring>* _pFolderList)
{
	if (nullptr == m_pFile_Manager)
		return list<FILEDATA>();

	return m_pFile_Manager->Find_FolderList(_pFolderList);
}

HRESULT CGameInstance::Clear_BackBuffer_View(const _float4 & v4ClearColor)
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

_bool CGameInstance::Get_DIKeyDown(const _ubyte & _eKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIKeyDown(_eKeyID);
}

_bool CGameInstance::Get_DIKeyUp(const _ubyte & _eKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIKeyUp(_eKeyID);
}

_bool CGameInstance::Get_DIKeyPressing(const _ubyte & _eKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIKeyPressing(_eKeyID);
}

_bool CGameInstance::Get_DIMBtnDown(const CInput_Device::DIMOUSEBUTTON & _eMouseBtn)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIMBtnDown(_eMouseBtn);
}

_bool CGameInstance::Get_DIMBtnUp(const CInput_Device::DIMOUSEBUTTON & _eMouseBtn)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->Get_DIMBtnUp(_eMouseBtn);
}

_bool CGameInstance::Get_DIMBtnPressing(const CInput_Device::DIMOUSEBUTTON & _eMouseBtn)
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

map<const _tchar*, Sound*>* CGameInstance::Get_SoundMap()
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

_int CGameInstance::Volume_Up(const CSound_Manager::CHANNELID & _eID, const _float & _fVol)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->Volume_Up(_eID, _fVol);
}

_int CGameInstance::Volume_Down(const CSound_Manager::CHANNELID & _eID, const _float & _fVol)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->Volume_Down(_eID, _fVol);
}

_int CGameInstance::BGM_Volume_Up(const _float & _fVol)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->BGM_Volume_Up( _fVol);
}

_int CGameInstance::BGM_Volume_Down(const _float & _fVol)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->BGM_Volume_Down(_fVol);
}

_int CGameInstance::Pause(const CSound_Manager::CHANNELID & _eID)
{
	if (nullptr == m_pSound_Manager)
		return 0;
	return m_pSound_Manager->Pause(_eID);
}

void CGameInstance::Play_SoundW(const _tchar * pSoundKey, const CSound_Manager::CHANNELID & _eID, const _float & _fVol)
{
	if (nullptr == m_pSound_Manager)
		return;
	return m_pSound_Manager->Play_SoundW(pSoundKey, _eID, _fVol);
}

void CGameInstance::Play_BGM(const _tchar * pSoundKey)
{
	if (nullptr == m_pSound_Manager)
		return;
	return m_pSound_Manager->Play_BGM(pSoundKey);
}

void CGameInstance::Stop_Sound(const CSound_Manager::CHANNELID & _eID)
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

HRESULT CGameInstance::Add_Thread(function<void(void*)> _pData, void * _pArg)
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

_double CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.f;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

void CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iCurrentLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Change_Level(iCurrentLevelIndex, pNextLevel);
}

_uint CGameInstance::Get_CurLevel() const
{
	// // O: 여기에 return 문을 삽입합니다.
	if (nullptr == m_pLevel_Manager)
		return (_uint)10000000000;

	return m_pLevel_Manager->Get_CurLevel();
}

CComponent * CGameInstance::Get_Component(const _uint & _iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, const _uint & _iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(_iLevelIndex, pLayerTag, pComponentTag, _iIndex);
}

HRESULT CGameInstance::Add_ProtoType(const _tchar * _pProtoTypeTag, CGameObject * _pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_ProtoType(_pProtoTypeTag, _pGameObject);
}

HRESULT CGameInstance::Clone_GameObject(const _uint & _iLevelIndex, const _tchar * _pLayerTypeTag, const _tchar * _pProtoTypeTag, void * _pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Clone_GameObject(_iLevelIndex, _pLayerTypeTag, _pProtoTypeTag, _pArg);
}

CGameObject * CGameInstance::Clone_GameObject_Add(const _uint & _iLevelIndex, const _tchar * _pLayerTypeTag, const _tchar * _pProtoTypeTag, void * _pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject_Add(_iLevelIndex, _pLayerTypeTag, _pProtoTypeTag, _pArg);
}

CGameObject * CGameInstance::Find_GameObject(const _uint & _iLevelIndex, const _tchar * _pLayerTypeTag, const _uint & _iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Find_GameObject(_iLevelIndex, _pLayerTypeTag, _iIndex);
}

list<CGameObject*> CGameInstance::Find_GameObjectList(const _uint & _iLevelIndex, const _tchar * _pLayerTypeTag)
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

HRESULT CGameInstance::Clear_Layer(const _uint & _iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	return m_pObject_Manager->Clear(_iLevelIndex);
}

HRESULT CGameInstance::Add_ProtoType(const _uint& _iLevelIndex, const _tchar * _pPrototypeTag, CComponent * _pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;
	return m_pComponent_Manager->Add_Prototype(_iLevelIndex, _pPrototypeTag, _pPrototype);
}

CComponent * CGameInstance::Clone_Component(const _uint& _iLevelIndex, const _tchar * _pPrototypeTag, void * _pArg)
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
	return m_pLight_Manager->Modify_Light( _tLightDesc, _iIndex);
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

_matrix CGameInstance::Get_TransformMatrix(const CPipeLine::TRANSFORMSTATE& _eTransState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();
	return m_pPipeLine->Get_TransformMatrix(_eTransState);
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

void CGameInstance::Set_Transform(const CPipeLine::TRANSFORMSTATE& _eState, const _fmatrix& _StateMatrix)
{
	if (nullptr == m_pPipeLine)
		return;
	m_pPipeLine->Set_Transform(_eState, _StateMatrix);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Destroy_Instance();
	CRayCast::Destroy_Instance();
	CPipeLine::Destroy_Instance();
	CPicking::Destroy_Instance();
	CFrustCulling::Destroy_Instance();
	CTarget_Manager::Destroy_Instance();
	CLight_Manager::Destroy_Instance();
	CComponent_Manager::Destroy_Instance();
	CObject_Manager::Destroy_Instance();
	CLevel_Manager::Destroy_Instance();
	CTimer_Manager::Destroy_Instance();
	CThread_Manager::Destroy_Instance();
	CSound_Manager::Destroy_Instance();
	CRandomCount::Destroy_Instance();
	CFont_Manager::Destroy_Instance();
	CInput_Device::Destroy_Instance();
	CGraphic_Device::Destroy_Instance();
	CFile_Manager::Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pRayCast);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pPicking);
	Safe_Release(m_pFrustCulling);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pThread_Manager);
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pRandomCount);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pFile_Manager);
}
