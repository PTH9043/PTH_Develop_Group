#include "UGameInstance.h"
//#include "UGraphicRenderManager.h"
#include "UShaderBufferManager.h"
#include "UGraphicDevice.h"
#include "UTimerManager.h"
#include "UInputManager.h"
#include "UThreadManager.h"
#include  "UFontManager.h"
#include "UActorManager.h"
#include "UComponentManager.h"
#include "UResourceManager.h"
#include "USceneManager.h"

#include "UPipeLine.h"
#include "URenderTargetManager.h"
#include "UFilePathManager.h"
#include "UNetworkBaseController.h"
#include "UAudioSystemManager.h"
#include "UCharacterManager.h"
#include "UMaterialManager.h"
#include "UNetworkQueryProcessing.h"
#include "UFontManager.h"
#include "URenderer.h"
#include "UNetworkSender.h"


#include "URootSignature.h"
#include "UComputeRootSignature.h"
#include "UShaderConstantBuffer.h"
#include "UGlobalConstantBuffer.h"

#include "UDevice.h"
#include "UGpuCommand.h"
#include "UTableDescriptor.h"
#include "UComputeCommand.h"

#include "UDefferedCamera.h"
#include "USmallDefferedCamera.h"
#include "UShadowCamera.h"
#include "UDefaultCube.h"
#include "UDefaultDebugging.h"
#include "UDefaultCell.h"

#include "UShader.h"
#include "UComputeShader.h"

#include "UVIBufferRect.h"
#include "UVIBufferPoint.h"
//#include "UVIBufferTriangle.h"
#include "UVIBufferSphere.h"
#include "UVIBufferCube.h"
#include "UVIBufferPlane.h"
#include "UVIBufferGrid.h"
#include "UVIBufferTrail.h"
//#include "UVIBufferSkyBox.h"

#include "UParticleSystem.h"

//#include "URectTransform.h"
//#include "USkyBox.h"
//#include "UTerrain.h"
#include "UParticle.h"
#include "UFire.h"
#include "UFog.h"
#include "UCollider.h"
#include "UTrail.h"
#include "UGuard.h"
#include "UBlood.h"
#include "UDust.h"
#include "UMat.h"

//#include "UMirror.h"
//#include "UScreenRenderObj.h"
//#include "UMirrorCamera.h"

#include "UPawn.h"
#include "UPicking.h"
#include "UGrid.h"
#include "UModelMaterial.h"
#include "UPawn.h"
#include "URectTransform.h"

IMPLEMENT_SINGLETON(UGameInstance);

UGameInstance::UGameInstance() :
	m_isGamming{ true },
	m_spGraphicDevice(Create<UGraphicDevice>()),
	m_spShaderBufferManager{ Create<UShaderBufferManager>() },
	m_spTimerManager{ Create<UTimerManager>() },
	m_spInputManager{ Create<UInputManager>() },
	m_spThreadManager{Create<UThreadManager>()},

	//m_spFontMananger{ Create<UFontManager>() },
	m_spActorManager(Create<UActorManager>()),
	m_spComponentManager(Create<UComponentManager>()),
	m_spResourceManager(Create<UResourceManager>()),
	m_spSceneManager{ Create<USceneManager>() },

	m_spRenderTargetManager(Create<URenderTargetManager>()),
	//m_spComputeManager(Create<UComputeManager>()),
	m_spPipeLine{ Create<UPipeLine>() },
	m_spPicking{ Create<UPicking>() },
	m_spFilePathManager{ Create<UFilePathManager>() },
	//m_spRandomManager{ Create<URandomManager>() },
	m_spAudioSystemManager{ Create<UAudioSystemManager>() },
	m_spCharacterManager{Create<UCharacterManager>()},
	m_spMaterialManager{ Create<UMaterialManager>()},
	m_spRenderer{ nullptr },
	m_spFontMananger{ Create<UFontManager>() },
	m_isFreeModeCameraEnable{false},
	m_isGamePause{false}
	//m_spGraphicRenderObject{ nullptr }
{
}

UGameInstance::~UGameInstance()
{
}

void UGameInstance::Free()
{
	ClearOnceTypeData();
	m_spNetworkQueryProcessing.reset();
	m_spNetworkBaseController.reset();
	m_isGamming = false;
	//ClearOnceTypeData();
	m_spRenderer->ClearAllData();
	m_spRenderer.reset();
	//m_spGraphicRenderObject.reset();
	//m_spRandomManager.reset();
	m_spFontMananger.reset();
	m_spMaterialManager.reset();
	m_spCharacterManager.reset();
	m_spNetworkBaseController.reset();
	m_spAudioSystemManager.reset();
	m_spFilePathManager.reset();
	m_spPicking.reset();
	m_spPipeLine.reset();
	//m_spComputeManager.reset();
	m_spRenderTargetManager.reset();
	m_spSceneManager.reset();
	m_spResourceManager.reset();
	m_spComponentManager.reset();
	m_spActorManager.reset();
	m_spThreadManager.reset();
	m_spInputManager.reset();
	m_spTimerManager.reset();
	m_spGraphicDevice.reset();
	m_spShaderBufferManager.reset();
}

HRESULT UGameInstance::ReadyInstance(const GRAPHICDESC& _stDesc, OUTPUTDATA& _stOutDesc)
{
	RETURN_CHECK_FAILED(m_spGraphicDevice->ReadyGraphicDevice(_stDesc, _stOutDesc), E_FAIL);
	RETURN_CHECK_FAILED(m_spShaderBufferManager->ReadyShaderBufferManager(_stOutDesc.wpDevice.lock()), E_FAIL);
	RETURN_CHECK_FAILED(m_spInputManager->ReadyInpuDevice(m_spGraphicDevice->GetGraphicDesc()), E_FAIL);
	RETURN_CHECK_FAILED(m_spRenderTargetManager->ReadyRenderTarget(m_spGraphicDevice, m_spGraphicDevice->GetDevice()), E_FAIL);

	RETURN_CHECK_FAILED(ReadyRenderTarget(_stOutDesc), E_FAIL);
	RETURN_CHECK_FAILED(ReadyResource(_stOutDesc), E_FAIL);
	RETURN_CHECK_FAILED(ReadyComp(_stOutDesc), E_FAIL);
	RETURN_CHECK_FAILED(ReadyActor(_stOutDesc), E_FAIL);

	RETURN_CHECK_FAILED(m_spActorManager->ReadyActorManager(m_spRenderer), E_FAIL);
	RETURN_CHECK_FAILED(m_spSceneManager->ReadySceneManager(this), E_FAIL);
	
	RETURN_CHECK_FAILED(m_spFontMananger->ReadyFontManager(this, m_spRenderTargetManager), E_FAIL);
	RETURN_CHECK_FAILED(m_spPipeLine->ReadyPipeLine(this), E_FAIL);
	RETURN_CHECK_FAILED(m_spAudioSystemManager->ReadyAudioSystemManager(this), E_FAIL);
	RETURN_CHECK_FAILED(m_spPicking->ReadyPickingDesc(m_spGraphicDevice->GetGraphicDesc()), E_FAIL);
	m_isGamming = true;
	return S_OK;
}

HRESULT UGameInstance::CreateGraphicsShader(const _wstring& _wstrProtoName, const CLONETYPE _eCloneType,
	const SHADERDESC& _stShaderDesc, const GRAPHICRENDEROBJECT_TYPE _eType)
{
	
	SHPTR<UShader> pShader = CreateConstructorToNative<UShader>(
		m_spGraphicDevice->GetDevice(), m_spGraphicDevice->GetRootSignature(),
		_stShaderDesc
	);
	RETURN_CHECK(nullptr == pShader, E_FAIL);
	// AddPrototype
	return AddPrototype(_wstrProtoName, _eCloneType, pShader);
}

HRESULT UGameInstance::CreateComputeShader(const _wstring& _wstrProtoName, const CLONETYPE _eCloneType, const SHADERDESC& _stShaderDesc)
{
	RETURN_CHECK(_stShaderDesc.pElement != nullptr, E_FAIL);

	// Shader 
	SHPTR<UComputeShader> pShader = CreateConstructorToNative<UComputeShader>(
		m_spGraphicDevice->GetDevice(), m_spGraphicDevice->GetComputeRootSignature(),
		_stShaderDesc
		);
	RETURN_CHECK(nullptr == pShader, E_FAIL);
	// AddPrototype
	return AddPrototype(_wstrProtoName, _eCloneType, pShader);
}

void UGameInstance::OtherFrame(const _double& _dTimeDelta, const WPARAM& _wParam)
{
	m_spInputManager->InvokeKeyMethods((_ubyte)_wParam, _dTimeDelta);
}

void UGameInstance::AwakeTick()
{
	if (nullptr != m_spNetworkQueryProcessing)
	{
		m_spNetworkQueryProcessing->ProcessNetworkInitData();
		m_spNetworkQueryProcessing->ProcessQueryData();
	}

	m_spInputManager->KeyTick();
	m_spInputManager->MouseTick();
	m_spPicking->CastRayInWorldSpace(this);
	m_spPipeLine->FrustomTick();
	m_spRenderer->ClearRenderingData();
}

void UGameInstance::Tick( _double _dTimeDelta)
{
	if (nullptr != m_spNetworkQueryProcessing)
	{
		m_spNetworkQueryProcessing->ProcessQueryData();
	}

	m_spSceneManager->Tick(_dTimeDelta);
	if (m_isPause) {
		m_spRenderer->Tick(0.f);
		m_spActorManager->Tick(0.f);
	}
	else {
		m_spRenderer->Tick(_dTimeDelta);
		m_spActorManager->Tick(_dTimeDelta);
	}
}

void UGameInstance::LateTick(_double _dTimeDelta)
{
	if (nullptr != m_spNetworkQueryProcessing)
	{
		m_spNetworkQueryProcessing->ProcessQueryData();
	}

	_double dTimeDelta = _dTimeDelta;
	if (m_isPause)
		dTimeDelta = 0.0;

	m_spActorManager->LateTick(dTimeDelta);
	m_spCharacterManager->TickCollider(dTimeDelta);
	m_spSceneManager->LateTick(dTimeDelta);

	m_spActorManager->NetworkTick(dTimeDelta);

	if (nullptr != m_spNetworkSender)
	{
		m_spNetworkSender->SendDataInQuery();
	}
}

void UGameInstance::RenderBegin()
{
	m_spGraphicDevice->MainRenderBegin();
	m_spRenderer->Render();
	m_spFontMananger->Render();
}

void UGameInstance::RenderEnd()
{
	/* Gpu 동기화 시키는 부분 */
	m_spGraphicDevice->MainRenderEnd();
}

void UGameInstance::SetImGuiContext(ImGuiContext* _pContext)
{
	ImGui::SetCurrentContext(_pContext);
}

HRESULT UGameInstance::OnWindowResize(const _uint& _iWinSizeX, const _uint& _iWinSizeY, const GRAPHICDESC::WINMODE _eWindowMode)
{
	RETURN_CHECK_FAILED(m_spGraphicDevice->OnResize(_iWinSizeX, _iWinSizeY, _eWindowMode), E_FAIL);
	m_spRenderTargetManager->OnResizeWindow(m_spGraphicDevice);
	m_spPicking->ReadyPickingDesc(m_spGraphicDevice->GetGraphicDesc());
#ifdef _USE_DEBUGGING
	//m_spComputeManager->OnResizeDebugRenderObject(m_spGraphicDevice);
#endif 
	return S_OK;
}

void UGameInstance::SetDefaultViewPort()
{
	m_spGraphicDevice->SetDefaultViewPort();
}

void UGameInstance::SetTemporaryViewPort(D3D12_VIEWPORT _viewport, D3D12_RECT _rect)
{
	m_spGraphicDevice->SetTemporaryViewPort(_viewport, _rect);
}

void UGameInstance::ClearOnceTypeData()
{
	m_spRenderer->ClearRenderingData();
	m_spActorManager->ClearOnceTypeData();
	m_spResourceManager->ClearOnceTypeData();
	m_spPipeLine->ClearOneTypeCamera();
	m_spCharacterManager->ClearData();
}


/*
-----------------------------
UGameInstance
-----------------------------
UGraphicDevice
-----------------------------
*/

const _bool UGameInstance::IsGamePaused() const
{
	return m_spGraphicDevice->IsGamePaused();
}

const _bool UGameInstance::IsScreenMinimized() const
{
	return m_spGraphicDevice->IsScreenMinimized();
}

const _bool UGameInstance::IsScreenMaximized() const
{
	return m_spGraphicDevice->IsScreenMaximized();
}

const _bool UGameInstance::IsScreenFullScreen() const
{
	return m_spGraphicDevice->IsScreenFullScreen();
}

const _bool UGameInstance::IsMssa4xState() const
{
	return m_spGraphicDevice->IsMssa4xState();
}

const _uint& UGameInstance::GetMssa4xQuality() const
{
	return m_spGraphicDevice->GetMssa4xQuality();
}

const D3D12_VIEWPORT& UGameInstance::GetD3DViewport() const
{
	return m_spGraphicDevice->GetD3DViewport();
}

const D3D12_RECT& UGameInstance::GetD3DWindowSizeRect() const
{
	return m_spGraphicDevice->GetD3DWindowSizeRect();
}

CSHPTRREF<GRAPHICDESC>UGameInstance::GetGraphicDesc() const
{
	return m_spGraphicDevice->GetGraphicDesc();
}

SHPTR<UTexture> UGameInstance::GetDefaultBackTexture() const
{
	return m_spGraphicDevice->GetDefaultBackTexture();
}

CSHPTRREF<UDevice> UGameInstance::GetDevice() const
{
	return m_spGraphicDevice->GetDevice();
}

CSHPTRREF<UGpuCommand> UGameInstance::GetGpuCommand() const
{
	return m_spGraphicDevice->GetGpuCommand();
}

CSHPTRREF<UComputeCommand> UGameInstance::GetComputeCommand() const
{
	return m_spGraphicDevice->GetComputeCommand();
}

CSHPTRREF<UTableDescriptor> UGameInstance::GetTableDescriptor() const
{
	return m_spGraphicDevice->GetTableDescriptor();
}

CSHPTRREF<UTableDescriptor> UGameInstance::GetComputeTableDescriptor() const
{
	return m_spGraphicDevice->GetComputeTableDescriptor();
}


CSHPTRREF<URootSignature> UGameInstance::GetRootSignature() const
{
	return m_spGraphicDevice->GetRootSignature();
}

CSHPTRREF<USwapChain> UGameInstance::GetSwapChain() const
{
	return m_spGraphicDevice->GetSwapChain();
}

HRESULT UGameInstance::AddRenderTargetGroup(const RTGROUPID& _eGroupID, const std::vector<RTDESC>& _rtVec)
{
	return m_spRenderTargetManager->AddRenderTargetGroup(_eGroupID, _rtVec);
}

void UGameInstance::RemoveRenderTargetGroup(const RTGROUPID _eGroupID)
{
	m_spRenderTargetManager->RemoveRenderTargetGroup(_eGroupID);
}

/*
==================================================
GraphicDevice
==================================================
ShaderBufferManager
==================================================
*/

void UGameInstance::BindGlobalBuffer(const GLOBAL_CBUFFERTYPE _eGrobalCBuffer, CSHPTRREF<UCommand> _spCommand, const void* _pBuffer, const _uint _iSize)
{
	m_spShaderBufferManager->BindGlobalBuffer(_eGrobalCBuffer, _spCommand, _pBuffer, _iSize);
}

HRESULT UGameInstance::GetGlobalConstantBuffer(const GLOBAL_CBUFFERTYPE _eGrobalCBuffer, SHPTRREF<UGlobalConstantBuffer> _spGrobalConstantBuffer)
{
	return m_spShaderBufferManager->GetGlobalConstantBuffer(_eGrobalCBuffer, _spGrobalConstantBuffer);
}

HRESULT UGameInstance::GetPreAllocatedConstantBuffer(const PREALLOCATED_CBUFFERTYPE _ePreAllocatedCBufferType, SHPTRREF<UShaderConstantBuffer> _spShaderConstantBuffer)
{
	return m_spShaderBufferManager->GetPreAllocatedConstantBuffer(_ePreAllocatedCBufferType, _spShaderConstantBuffer);
}


/*
==================================================
ShaderBufferManager
==================================================
UTimerManager
==================================================
*/

HRESULT UGameInstance::CreateTimer(const _wstring& _wstrName)
{
	return m_spTimerManager->CreateTimer(_wstrName);
}

SHPTR<UTimer> UGameInstance::CreateTimerAdd(const _wstring& _wstrName)
{
	return m_spTimerManager->CreateTimerAdd(_wstrName);
}

HRESULT UGameInstance::RemoveTimer(const _wstring& _wstrName)
{
	return m_spTimerManager->RemoveTimer(_wstrName);
}

/*
==================================================
UTimerManager
==================================================
UInputManager
==================================================
*/

void UGameInstance::AddKeyMethod(_ubyte _bFirstKeyID, const _wstring& _wstrSecondsKeyName, const KEYACTIONFUNC& _keyMethod)
{
	m_spInputManager->AddKeyMethod(_bFirstKeyID, _wstrSecondsKeyName, _keyMethod);
}

void UGameInstance::DeleteKeyMethod( _ubyte _bFirstKeyID, const _wstring& _wstrSecondsKeyName)
{
	m_spInputManager->DeleteKeyMethod(_bFirstKeyID, _wstrSecondsKeyName);
}

_bool UGameInstance::GetDIKeyDown( _ubyte _bKeyID)
{
	return m_spInputManager->GetDIKeyDown(_bKeyID);
}

_bool UGameInstance::GetDIKeyUp( _ubyte _bKeyID)
{
	return m_spInputManager->GetDIKeyUp(_bKeyID);
}

_bool UGameInstance::GetDIKeyPressing( _ubyte _bKeyID)
{
	return m_spInputManager->GetDIKeyPressing(_bKeyID);
}

_bool UGameInstance::GetDIMBtnDown(_ubyte _eMouseBtn)
{
	return m_spInputManager->GetDIMBtnDown(_eMouseBtn);
}

_bool UGameInstance::GetDIMBtnUp(_ubyte _eMouseBtn)
{
	return m_spInputManager->GetDIMBtnUp(_eMouseBtn);
}

_bool UGameInstance::GetDIMBtnPressing(_ubyte _eMouseBtn)
{
	return m_spInputManager->GetDIMBtnPressing(_eMouseBtn);
}

_long UGameInstance::GetDIMMoveState(_ubyte _eMouseMove)
{
	return m_spInputManager->GetDIMMoveState(_eMouseMove);
}

_float2 UGameInstance::GetMousePosition()
{
	return m_spInputManager->GetMousePosition();
}

_bool UGameInstance::IsMouseInWindowSize(const float _Width, const float _Height)
{
	_float2 vMosuePos = m_spInputManager->GetMousePosition();
	if (0.f <= vMosuePos.x && 0.f <= vMosuePos.y)
	{
		if (_Width >= vMosuePos.x && _Height >= vMosuePos.y)
		{
			return true;
		}
	}
	return false;
}

_bool UGameInstance::IsMouseInWindowSize()
{
	return m_spInputManager->IsMouseInWindowSize();
}

/*
==================================================
UInputManager
==================================================
UThreadManager
==================================================
*/

void UGameInstance::RegisterFuncToRegister(const THREADFUNC& _CallBack, void* _pData)
{
	m_spThreadManager->RegisterFuncToRegister(_CallBack, _pData);
}

void UGameInstance::JoinRegister()
{
	m_spThreadManager->JoinRegister();
}

void UGameInstance::DetachRegister()
{
	m_spThreadManager->DetachRegister();
}


/*
==================================================
ThreadManager
==================================================
ActorManager
==================================================
*/

const CLONEARR& UGameInstance::GetClonesArr() const
{
	return m_spActorManager->GetClonesArr();
}

HRESULT UGameInstance::AddPrototype(const _wstring& _wstrName, CSHPTRREF<UActor> _spActor)
{
	return m_spActorManager->AddPrototypes(_wstrName, _spActor);
}

void UGameInstance::CloneActor(const _wstring& _wstrProto, const VOIDDATAS& _stDatas)
{
	m_spActorManager->CloneActor(_wstrProto, _stDatas);
}

SHPTR<UActor> UGameInstance::CloneActorAdd(const _wstring& _wstrProto, const VOIDDATAS& _stDatas)
{
	return m_spActorManager->CloneActorAdd(_wstrProto, _stDatas);
}

SHPTR<UActor> UGameInstance::CloneActorAddAndNotInLayer(const _wstring& _wstrProto, const VOIDDATAS& _stDatas)
{
	return m_spActorManager->CloneActorAddAndNotInLayer(_wstrProto, _stDatas);
}

void UGameInstance::RemoveActor(CSHPTRREF<UActor> _spActor)
{
	m_spActorManager->RemoveActor(_spActor);
	// Collision pawn 안에 있는 녀석 지우기
	SHPTR<UPawn> spPawn = std::dynamic_pointer_cast<UPawn>(_spActor);
	if (nullptr != spPawn)
	{
		m_spCharacterManager->RemoveCollisionPawn(spPawn);
	}
}

void UGameInstance::EnableLayers(const _wstring& _wstrLayer)
{
	m_spActorManager->EnableLayers(_wstrLayer);
}

void UGameInstance::DisableLayers(const _wstring& _wstrLayer)
{
	m_spActorManager->DisableLayers(_wstrLayer);
}

/*
==================================================
ActorManager
==================================================
ComponentManager
==================================================
*/

HRESULT UGameInstance::AddPrototype(const _wstring& _wstrPrototypes, CSHPTRREF<UComponent> _spComponent)
{
	return m_spComponentManager->AddPrototype(_wstrPrototypes, _spComponent);
}

SHPTR<UComponent> UGameInstance::CloneComp(const _wstring& _wstrPrototypes, const VOIDDATAS& _stDatas)
{
	return m_spComponentManager->CloneComp(_wstrPrototypes, _stDatas);
}

/*
==================================================
ComponentManager
==================================================
ResourceManager
==================================================
*/

HRESULT UGameInstance::AddPrototype(const _wstring & _wstrPrototype, const CLONETYPE & _eCloneType, CSHPTRREF<UResource> _spResource)
{
	return m_spResourceManager->AddPrototype(_wstrPrototype, _eCloneType, _spResource);
}

SHPTR<UResource> UGameInstance::CloneResource(const _wstring& _wstrPrototype, const VOIDDATAS& _stDatas)
{
	return m_spResourceManager->CloneResource(_wstrPrototype, _stDatas);
}

void UGameInstance::FindResourceProtoTag(CSHPTRREF<UResource> _spResource, _wstring& _wstrResourceTag)
{
	m_spResourceManager->FindResourceProtoTag(_spResource, _wstrResourceTag);
}

void UGameInstance::AddPipeLineState(const _wstring& _wstrName, const ComPtr<Dx12PipelineState>& _cpPipeLineState)
{
	m_spResourceManager->AddPipeLineState(_wstrName, _cpPipeLineState);
}

void UGameInstance::SettingPipeLineState(const _wstring& _wstrName, CSHPTRREF<UCommand> _spCommand)
{
	m_spResourceManager->SettingPipeLineState(_wstrName, _spCommand);
}

/*
==================================================
ResourceManager
==================================================
SceneManager
==================================================
*/

void UGameInstance::OutLightControlInfo(LIGHTPARAM& _stLightControl)
{
	m_spSceneManager->OutLightControlInfo(_stLightControl);
}

void UGameInstance::ChangeLightCamIndex(const CAMID& _iID)
{
	m_spSceneManager->ChangeLightCamIndex(_iID);
}

CSHPTRREF<UScene> UGameInstance::GetCurScene() const
{
	return m_spSceneManager->GetScene();
}

void UGameInstance::RegisterScene(CSHPTRREF<UScene> _spScene)
{
	m_spSceneManager->RegisterScene(_spScene);
}

void UGameInstance::RegisterSceneWithOutLoadingData(CSHPTRREF<UScene> _spScene)
{
	m_spSceneManager->RegisterSceneWithOutLoadingData(_spScene);
}

HRESULT UGameInstance::AddLight(const LIGHTINFO& _stInfo)
{
	return m_spSceneManager->AddLight(_stInfo);
}

void UGameInstance::OutLight(const LIGHTTYPE& _eLightType, const _uint& _iIndex, SHPTR<ULight>& _spLight)
{
	m_spSceneManager->OutLight(_eLightType, _iIndex, _spLight);
}

HRESULT UGameInstance::ActiveLIght(const LIGHTTYPE& _eLightType, const _uint& _iIndex, const _bool& _isActive)
{
	return m_spSceneManager->ActiveLIght(_eLightType, _iIndex, _isActive);
}

HRESULT UGameInstance::DeleteLight(const LIGHTTYPE& _eLightType, const _uint& _iIndex)
{
	return m_spSceneManager->DeleteLight(_eLightType, _iIndex);
}

HRESULT UGameInstance::ClearLight()
{
	return m_spSceneManager->ClearLight();
}


/*
==================================================
SceneManager
==================================================
RenderTargetManager
==================================================
*/

// OMSetRenderTarget 
void	 UGameInstance::OmSetRenderTargets(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID, const _uint _index, const _uint _iOffset)
{
	m_spRenderTargetManager->OmSetRenderTargets(_spCommand, _eGroupID, _index, _iOffset);
}
void	 UGameInstance::OmSetRenderTargets(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID)
{
	m_spRenderTargetManager->OmSetRenderTargets(_spCommand, _eGroupID);
}
// ClearRenderTargetView
void	 UGameInstance::ClearRenderTargetView(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID, const _uint _index)
{
	m_spRenderTargetManager->ClearRenderTargetView(_spCommand, _eGroupID, _index);
}
void	 UGameInstance::ClearRenderTargetView(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID)
{
	m_spRenderTargetManager->ClearRenderTargetView(_spCommand, _eGroupID);
}
// Waiting
void	 UGameInstance::WaitTargetToResource(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID)
{
	m_spRenderTargetManager->WaitTargetToResource(_spCommand, _eGroupID);
}
void	 UGameInstance::WaitResourceToTarget(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID)
{
	m_spRenderTargetManager->WaitResourceToTarget(_spCommand, _eGroupID);
}


SHPTR<URenderTargetGroup> UGameInstance::FindRenderTargetGroup(const RTGROUPID& _eGroupID)
{
	return m_spRenderTargetManager->FindRenderTargetGroup(_eGroupID);
}

SHPTR<UTexture> UGameInstance::FindRenderTargetTexture(const RTGROUPID _eGroupID, const RTOBJID _eObjID)
{
	return m_spRenderTargetManager->FindRenderTargetTexture(_eGroupID, _eObjID);
}

/*
==================================================
RenderTargetManager
==================================================
PipeLine
==================================================
*/

void UGameInstance::RegisterCameraInPipeline(CSHPTRREF<UCamera> _spCamera, CAMID& _iCamID, const VIEWPROJMATRIX& _stViewProjMatrix, const CAMERATYPE _eType)
{
	m_spPipeLine->RegisterCameraInPipeline(_spCamera, _iCamID, _stViewProjMatrix, _eType);
}

void UGameInstance::RemoveCameraInPipeLine(CAMID _CamID)
{
	m_spPipeLine->RemoveCameraInPipeLine(_CamID);
}
_bool UGameInstance::IsFrustomContains(const _float3& _vPos, const _float _fRadius, const CAMID& _iCamID)
{
	return m_spPipeLine->IsFrustomContains(_vPos, _fRadius, _iCamID);
}

const _float4x4 UGameInstance::GetMainCamViewMatrix() const
{
	return m_spPipeLine->GetMainCamViewMatrix();
}

const _float4x4& UGameInstance::GetMainCamProjMatrix() const
{
	return m_spPipeLine->GetMainCamProjMatrix();
}

const _float3& UGameInstance::GetMainCamPosition()
{
	return m_spPipeLine->GetMainCamPosition();
}
const _float4x4 UGameInstance::GetCamViewMatrix(const CAMID& _iID) const
{
	return m_spPipeLine->GetCamViewMatrix(_iID);
}

const _float4x4& UGameInstance::GetCamProjMatrix(const CAMID& _iID) const
{
	return m_spPipeLine->GetCamProjMatrix(_iID);
}

const _float3& UGameInstance::GetCameraPosition(const CAMID& _iID)
{
	return m_spPipeLine->GetCameraPosition(_iID);
}
 SHPTR<UTransform> UGameInstance::GetMainCameraTransform()
{
	return m_spPipeLine->GetMainCamTransform();
}

const _float UGameInstance::GetCamFar(const CAMID& _iID)
{
	return m_spPipeLine->GetCamFar(_iID);
}

void UGameInstance::ChangeRenderCamera(CSHPTRREF<UCamera> _spCamera)
{
	m_spPipeLine->ChangeRenderCamera(_spCamera);
}

void UGameInstance::ChangeRenderCamera(CAMID _iID)
{
	m_spPipeLine->ChangeRenderCamera(_iID);
}

void UGameInstance::AddRenderCamList(CSHPTRREF<UCamera> _spCamera)
{
	m_spPipeLine->AddRenderCamList(_spCamera);
}

void UGameInstance::AddRenderCamList(CAMID _iID)
{
	m_spPipeLine->AddRenderCamList(_iID);
}

const CAMID UGameInstance::GetRenderCamID() const
{
	return m_spPipeLine->GetRenderCamID();
}

/*
==================================================
PipeLine
==================================================
FilePathManager
==================================================
*/

SHPTR<FILEGROUP> UGameInstance::FindFolder(const PATHS& _vecFolderList)
{
	return m_spFilePathManager->FindFolder(_vecFolderList);
}

SHPTR<FILEGROUP> UGameInstance::FindFolder(const _wstring& _wstrFindName, const _wstring& _wstrParentsFolderName)
{
	return m_spFilePathManager->FindFolder(_wstrFindName, _wstrParentsFolderName);
}

VECTOR<SHPTR<FILEGROUP>> UGameInstance::FindSameAllFolder(const _wstring& _wstrFindFolderName)
{
	return m_spFilePathManager->FindSameAllFolder(_wstrFindFolderName);
}

HRESULT UGameInstance::LoadFirstFolder(const _wstring& _wstrFilePath)
{
	return m_spFilePathManager->LoadFirstFolder(_wstrFilePath);
}
/*
==================================================
FilePathManager
==================================================
AudioSystemManager
==================================================
*/

const AUDIOSYSTEMCONTAINER& UGameInstance::GetAudioSystemContainer() const
{
	return m_spAudioSystemManager->GetAudioSystemContainer();
}

SHPTR<UAudioSystem> UGameInstance::GetAudioSystem(const SOUNDTYPE _SoundType)
{
	return m_spAudioSystemManager->GetAudioSystem(_SoundType);
}

HRESULT UGameInstance::CreateAudioSystemAndRegister(SOUNDTYPE _SoundType, const _wstring& _wstrSoundFolderPath)
{
	return m_spAudioSystemManager->CreateAudioSystemAndRegister(this, _SoundType, _wstrSoundFolderPath);
}

HRESULT UGameInstance::CreateAudioSystemAndRegister(SOUNDTYPE _SoundType, CSHPTRREF<FILEGROUP> _spSoundFileGroup)
{
	return m_spAudioSystemManager->CreateAudioSystemAndRegister(this, _SoundType, _spSoundFileGroup);
}

HRESULT UGameInstance::CreateAudioSystemToFolderNameAndRegister(SOUNDTYPE _SoundType, const _wstring& _wstrSoundFolderName)
{
	return m_spAudioSystemManager->CreateAudioSystemToFolderNameAndRegister(this, _SoundType, _wstrSoundFolderName);
}

void UGameInstance::SoundPlay(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->Play(_wstrSoundName);
}

void UGameInstance::SoundPlayWithManyChannels(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->PlayWithManyChannels(_wstrSoundName);
}

void UGameInstance::SoundPlay(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate)
{
	m_spAudioSystemManager->Play(_wstrSoundName, _fVolumeUpdate);
}

void UGameInstance::SoundRestartOnce(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->RestartOnce(_wstrSoundName);
}

void UGameInstance::SoundPlayOnce(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->PlayOnce(_wstrSoundName);
}

void UGameInstance::SoundPlayOnceWithManyChannels(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->PlayOnceWithManyChannels(_wstrSoundName);
}

void UGameInstance::SoundPlayBGM(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->PlayBGM(_wstrSoundName);
}

void UGameInstance::SoundStopBGM(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->StopBGM(_wstrSoundName);
}

void UGameInstance::SoundPauseBGM(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->PauseBGM(_wstrSoundName);
}

void UGameInstance::SoundResumeBGM(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->ResumeBGM(_wstrSoundName);
}


void UGameInstance::SoundPlayBGM(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate)
{
	m_spAudioSystemManager->PlayBGM(_wstrSoundName, _fVolumeUpdate);
}

void UGameInstance::StopSound(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->Stop(_wstrSoundName);
}

void UGameInstance::StopSoundWithManyChannels(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->StopWithManyChannels(_wstrSoundName);
}

void UGameInstance::SoundDisableOncePlay(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->DisableOncePlay(_wstrSoundName);
}

void UGameInstance::Restart(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->Restart(_wstrSoundName);
}

void UGameInstance::Pause(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->Pause(_wstrSoundName);// 사운드 일시 정지
}

void UGameInstance::Resume(const _wstring& _wstrSoundName)//일시 정지된 사운드를 재개합니다.
{
	m_spAudioSystemManager->Resume(_wstrSoundName);
}

void UGameInstance::SetPlaybackPosition(const _wstring& _wstrSoundName, unsigned int position)
{
	m_spAudioSystemManager->SetPlaybackPosition(_wstrSoundName, position);//// 재생 위치를 1000 밀리초(1초)로 설정//USoundInstance->SetPlaybackPosition(1000);
	
}

//unsigned int UGameInstance::GetPlaybackPosition(const _wstring& _wstrSoundName)
//{
//	return m_spAudioSystemManager->GetPlaybackPosition(_wstrSoundName);//// 현재 재생 위치 가져오기
//}

void UGameInstance::SetLooping(const _wstring& _wstrSoundName, bool loop)
{
	m_spAudioSystemManager->SetLooping(_wstrSoundName, loop);// 사운드를 반복 재생으로 설정// 사운드 반복 재생 비활성화
}

void UGameInstance::SetPitch(const _wstring& _wstrSoundName, float pitch)
{
	m_spAudioSystemManager->SetPitch(_wstrSoundName, pitch);// 사운드 피치를 1.2로 설정 (원래 피치보다 높게)

}

void UGameInstance::SetPan(const _wstring& _wstrSoundName, float pan)
{
	m_spAudioSystemManager->SetPan(_wstrSoundName, pan);// 사운드를 왼쪽으로 패닝
}

void UGameInstance::Mute(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->Mute(_wstrSoundName);// 사운드 음소거
}

void UGameInstance::Unmute(const _wstring& _wstrSoundName)
{
	m_spAudioSystemManager->Unmute(_wstrSoundName);
}

void UGameInstance::FadeIn(const _wstring& _wstrSoundName, float fadeDuration)
{
	m_spAudioSystemManager->FadeIn(_wstrSoundName, fadeDuration);// 사운드를 2초 동안 페이드 인 볼륨 서서히 증가
}

void UGameInstance::FadeOut(const _wstring& _wstrSoundName, float fadeDuration)
{
	m_spAudioSystemManager->FadeOut(_wstrSoundName, fadeDuration); //사운드의 볼륨을 서서히 감소시켜 정지합니다.
}

void UGameInstance::UpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume)
{
	m_spAudioSystemManager->UpdateVolume(_wstrSoundName, _fVolume);
}

void UGameInstance::BGMUpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume)
{
	m_spAudioSystemManager->BGMUpdateVolume(_wstrSoundName, _fVolume);
}

void UGameInstance::UpdateSound3D(const _wstring& _wstrSoundName, const _float3& _vSoudPos, const _float3& _vSoundVelocity, CSHPTRREF<UTransform> _spTargetTransform)
{
	m_spAudioSystemManager->UpdateSound3D(_wstrSoundName, _vSoudPos, _vSoundVelocity, _spTargetTransform);
}

void UGameInstance::VolumeUpdate(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate)
{
	m_spAudioSystemManager->VolumeUpdate(_wstrSoundName, _fVolumeUpdate);
}

void UGameInstance::ChangeMinMaxDistance3D(const _wstring& _wstrSoundName, const _float _fMinDistance, const _float _fMaxDistance)
{
	m_spAudioSystemManager->ChangeMinMaxDistance3D(_wstrSoundName, _fMinDistance, _fMaxDistance);
}

SHPTR<USound> UGameInstance::BringSound(const _int _Index)
{
	return m_spAudioSystemManager->BringSound(_Index);
}

SHPTR<USound> UGameInstance::BringSound(const _wstring& _wstrSoundName)
{
	return m_spAudioSystemManager->BringSound(_wstrSoundName);
}


void UGameInstance::HandleSoundsForAnimation(CSHPTRREF<UCharacter> _Owner, const _wstring& animName, const _wstring& SoundName, _float startThreshold, _float endThreshold)
{
	m_spAudioSystemManager->HandleSoundsForAnimation(_Owner, animName, SoundName, startThreshold, endThreshold);
}

void UGameInstance::HandleSoundsForAnimationWithManyChannels(CSHPTRREF<UCharacter> _Owner, const _wstring& animName, const _wstring& SoundName, _float startThreshold, _float endThreshold)
{
	m_spAudioSystemManager->HandleSoundsForAnimationWithManyChannels(_Owner, animName, SoundName, startThreshold, endThreshold);
}
/*
==================================================
AudioSystemManager
==================================================
NetworkManager
==================================================
*/

void UGameInstance::StartNetwork(CSHPTRREF<UNetworkBaseController> _spNetworkBaseController, CSHPTRREF<UNetworkQueryProcessing> _spNetworkQueryProcessing)
{
	m_spNetworkBaseController = _spNetworkBaseController;
	m_spNetworkQueryProcessing = _spNetworkQueryProcessing;
	m_spNetworkSender = Create<UNetworkSender>(m_spNetworkBaseController);
}

void UGameInstance::MakeActors(const VECTOR<SHPTR<UActor>>& _actorContainer)
{
	assert(nullptr != m_spNetworkBaseController);
	m_spNetworkBaseController->MakeActors(_actorContainer);
}

void UGameInstance::InsertSendTcpPacketInQuery(_char* _pPacket, _short _PacketType, _short _PacketSize)
{
	assert(nullptr != m_spNetworkSender);
	m_spNetworkSender->InsertSendTcpPacketInQuery(_pPacket, _PacketType, _PacketSize);
}

void UGameInstance::InsertSendProcessPacketInQuery(UProcessedData&& _ProcessData)
{
	assert(nullptr != m_spNetworkSender);
	m_spNetworkSender->InsertSendProcessPacketInQuery(std::move(_ProcessData));
}

SHPTR<UActor> UGameInstance::FindNetworkActor(const _int _NetworkID)
{
	assert(nullptr != m_spNetworkBaseController);
	return m_spNetworkBaseController->FindNetworkActor(_NetworkID);
}

void UGameInstance::NetworkEnd()
{
	m_spNetworkBaseController.reset();
}

void UGameInstance::SetSceneIDToNetController(const _int _iSceneID)
{
	RETURN_CHECK(nullptr == m_spNetworkBaseController, ;);
	m_spNetworkBaseController->SetSceneID(_iSceneID);
}

const _llong UGameInstance::GetNetworkOwnerID() const
{
	assert(nullptr != m_spNetworkBaseController);
	return m_spNetworkBaseController->GetNetworkOwnerID();
}

const _bool UGameInstance::IsNetworkResourceRecvSuccess() const
{
	assert(nullptr != m_spNetworkBaseController);
	return m_spNetworkBaseController->IsNetworkResourceRecvSuccess();
}

/*
==================================================
NetworkManager
==================================================
CharacterManager
==================================================
*/

CSHPTRREF<UCharacter> UGameInstance::GetCurrPlayer() const
{
	return m_spCharacterManager->GetCurrPlayer();
}

void UGameInstance::RegisterCurrentPlayer(CSHPTRREF<UCharacter> _spCurrentPlayer)
{
	m_spCharacterManager->RegisterCurrentPlayer(_spCurrentPlayer);
}

void UGameInstance::AddCollisionPawnList(CSHPTRREF<UPawn> _spPawn)
{
	m_spCharacterManager->AddCollisionPawnList(_spPawn);
}

void UGameInstance::RemoveCollisionPawn(CSHPTRREF<UPawn> _spPawn)
{
	m_spCharacterManager->RemoveCollisionPawn(_spPawn);
}

void UGameInstance::RemovePlayerInContainer(CSHPTRREF<UPlayer> _spPlayer)
{
	m_spCharacterManager->RemovePlayerInContainer(_spPlayer);
}

void UGameInstance::AddPlayerInContainer(SHPTR<UPlayer> _spPlayer)
{
	m_spCharacterManager->AddPlayerInContainer(_spPlayer);
}

SHPTR<UPlayer> UGameInstance::FindPlayerToNetworkID(_int _iNetworkID)
{
	return 	m_spCharacterManager->FindPlayerToNetworkID(_iNetworkID);
}

SHPTR<UPlayer> UGameInstance::FindPlayerToDistance(const _float3& _vPos)
{
	return 	m_spCharacterManager->FindPlayerToDistance(_vPos);
}

/*
==================================================
CharacterManager
==================================================
MaterialManager
==================================================
*/

void UGameInstance::AddModelMaterial(const _uint _MaterialIndex, CSHPTRREF<UModelMaterial> _spModelMaterial)
{
	m_spMaterialManager->AddModelMaterial(_MaterialIndex, _spModelMaterial);
}

void UGameInstance::CopyToMaterialShaderParam(REF_IN GLOBALPARAM& _GrobalParam)
{
	m_spMaterialManager->CopyToMaterialShaderParam(REF_OUT _GrobalParam);
}

void UGameInstance::RemoveModelMaterial(const _uint _MaterialIndex)
{
	RETURN_CHECK(false == m_isGamming, ;);

	m_spMaterialManager->RemoveModelMaterial(_MaterialIndex);
}

const _bool UGameInstance::IsMaterialContainerInfoChange() const
{
	return m_spMaterialManager->IsMaterialContainerInfoChange();
}

/*
==================================================
MaterialManager
==================================================
Renderer Fog Setting
==================================================
*/

void UGameInstance::TurnOnFog() {
	if (m_spRenderer != nullptr)m_spRenderer->TurnOnFog();
}

void UGameInstance::TurnOffFog() {
	if (m_spRenderer != nullptr)m_spRenderer->TurnOffFog();
}

void UGameInstance::SetGameStartEffect()
{
	if (m_spRenderer != nullptr)m_spRenderer->SetGameStartEffect();
}

void UGameInstance::TurnOnDieEffect() {
	if (m_spRenderer != nullptr)m_spRenderer->TurnOnDieEffect();
}

void UGameInstance::TurnOffDieEffect() {
	if (m_spRenderer != nullptr)m_spRenderer->TurnOffDieEffect();
}

void UGameInstance::TurnOnAbilityEffect() {
	if (m_spRenderer != nullptr)m_spRenderer->TurnOnAbilityEffect();
}

void UGameInstance::TurnOffAbilityEffect() {
	if (m_spRenderer != nullptr)m_spRenderer->TurnOffAbilityEffect();
}

void UGameInstance::TurnOnHitEffect() {
	if (m_spRenderer != nullptr)m_spRenderer->TurnOnHitEffect();
}

void UGameInstance::TurnOffHitEffect() {
	if (m_spRenderer != nullptr)m_spRenderer->TurnOffHitEffect();
}

_bool UGameInstance::GetIfAbilityIsOn()
{
	return m_spRenderer->GetIfAbilityIsOn();
}

_float UGameInstance::GetAbilityTime()
{
	return m_spRenderer->GetAbilityTime();
}

void UGameInstance::PauseGame()
{
	RETURN_CHECK(true == m_isPause, ;);
	//m_spActorManager->ActosTickDisable();
	m_isPause = true;
}

void UGameInstance::ResumeGame()
{
	RETURN_CHECK(false == m_isPause, ;);
	//m_spActorManager->ActorsTickEnable();
	m_isPause = false;
}

void UGameInstance::PauseGameTick()
{
	RETURN_CHECK(true == m_isGamePause, ;);
//	m_spActorManager->ActosTickDisable();
	m_isGamePause = true;
}

void UGameInstance::ResumeGameTick()
{
	RETURN_CHECK(false == m_isGamePause, ;);
//	m_spActorManager->ActorsTickEnable();
	m_isGamePause = false;
}

/*
==================================================
Renderer Fog Setting
==================================================
Picking
==================================================
*/

void UGameInstance::AddPickingObject(CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer)
{
	m_spPicking->AddPickingObject(_spPawn, _spVIBuffer);
}
void UGameInstance::DeletePickingObject(CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer)
{
	m_spPicking->DeletePickingObject(_spPawn, _spVIBuffer);
}
void UGameInstance::AddPickingGrid(const MAINGRID& _stGrid)
{
	m_spPicking->AddPickingGrid(_stGrid);
}
SHPTR<UActor> UGameInstance::GetPickingPawn()
{
	return 	m_spPicking->GetPickingPawn();
}
const PICKINGDESC UGameInstance::GetPickDesc()
{
	return m_spPicking->GetPickDesc();
}
_bool UGameInstance::PickingMesh(const _float3& _RayPos, const _float3& _RayDir, CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer,
	_float* _pDist, _float3* _pOut)
{
	return m_spPicking->PickingMesh(_RayPos, _RayDir, _spPawn, _spVIBuffer, _pDist, _pOut);
}


/*
==================================================
Picking
==================================================
FontManager
==================================================
*/

HRESULT UGameInstance::FontCreate(const _wstring& _wstrFontName, const _wstring& _wstrPath)
{
	return m_spFontMananger->FontCreate(_wstrFontName, _wstrPath);
}

SHPTR<UFont> UGameInstance::FontCreateAdd(const _wstring& _wstrFontName, const _wstring& _wstrPath)
{
	return m_spFontMananger->FontCreateAdd(_wstrFontName, _wstrPath);
}

SHPTR<UFont> UGameInstance::AddFont(const _wstring& _wstrFontName)
{
	return m_spFontMananger->AddFont(_wstrFontName);
}

void UGameInstance::ReleaseOriginFont(const _wstring& _wstrFontName)
{
	m_spFontMananger->ReleaseOriginFont(_wstrFontName);
}

void UGameInstance::RemoveFont(CSHPTRREF<UFont> _spFont)
{
	m_spFontMananger->RemoveFont(_spFont);
}

/*
==================================================
FontManager
==================================================
ReadyDatas
==================================================
*/

HRESULT UGameInstance::ReadyResource(const OUTPUTDATA & _stData)
{
	// VIBuffer
	{
		AddPrototype(PROTO_RES_VIBUFFERPOINT, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferPoint>(
			_stData.wpDevice.lock()));

		AddPrototype(PROTO_RES_VIBUFFERRECT, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferRect>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::GENERIC));

		AddPrototype(PROTO_RES_VIBUFFERNORMALRECT, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferRect>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::NORMAL));
		
		AddPrototype(PROTO_RES_VIBUFFERPLANE, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferPlane>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::GENERIC));

		AddPrototype(PROTO_RES_VIBUFFERNORMALPLANE, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferPlane>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::NORMAL));

		AddPrototype(PROTO_RES_VIBUFFERGRID, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferGrid>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::GENERIC));

		AddPrototype(PROTO_RES_VIBUFFERTRAIL, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferTrail>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::GENERIC));

		AddPrototype(PROTO_RES_VIBUFFERNORMALGRID, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferGrid>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::NORMAL));

		AddPrototype(PROTO_RES_VIBUFFERSPHERE, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferSphere>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::GENERIC));

		AddPrototype(PROTO_RES_VIBUFFERNORMALSPHERE, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferSphere>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::NORMAL));

		AddPrototype(PROTO_RES_VIBUFFERCUBE, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferCube>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::GENERIC));

		AddPrototype(PROTO_RES_VIBUFFERNORMALCUBE, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferCube>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::NORMAL));

	/*	

		AddPrototype(PROTO_RES_VIBUFFERTRIANGLE, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferTriangle>(
			_stData.wpDevice.lock()));

		AddPrototype(PROTO_RES_VIBUFFERNORMALPLANE, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferPlane>(
			_stData.wpDevice.lock(), VIBUFFERTYPE::NORMAL));

		AddPrototype(PROTO_RES_VIBUFFERSKYBOX, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UVIBufferSkyBox>(
			_stData.wpDevice.lock()));*/
	}
	// Graphics Shader
	{
		// Finals 
		{
			CreateGraphicsShader(PROTO_RES_FINALDEFFEREDSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"FinalDeffered", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_BLENDDEFFEREDSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"BlendDeffered", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_MIDDLERENDERSCREENSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"MiddleRenderScreen", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_HDRSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Hdr", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_HDRTWOSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"HdrTwo", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_GRAYSCALESHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"GrayScale", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_TRAILSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Trail", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					}, RASTERIZER_TYPE::CULL_NONE,
					DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));

			CreateGraphicsShader(PROTO_RES_GUARDSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Guard", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					}, RASTERIZER_TYPE::CULL_NONE,
					DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));

			CreateGraphicsShader(PROTO_RES_BLOODSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Blood", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));

			CreateGraphicsShader(PROTO_RES_DUSTSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Dust", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					}, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));

			CreateGraphicsShader(PROTO_RES_MATSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Mat", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					}, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::LESS_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));


			CreateGraphicsShader(PROTO_RES_HORIZONTALBLURSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"HorizontalBlur", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_VERTICALBLURSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"VerticalBlur", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_DOWNSAMPLINGSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"DownSampling", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_DOWNSAMPLINGTWOSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"DownSamplingTwo", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_UPSAMPLINGSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"UpSampling", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_UPSAMPLINGGRAYSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"UpSamplingGray", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_BLOOMSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Bloom", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_DEBUG2DTARGETSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Debug2DTarget", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_SCREENRENDERONBJSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"ScreenRenderObj", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
					DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
					DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));

			CreateGraphicsShader(PROTO_RES_MOTIONBLURSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"MotionBlur", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ 	DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT }, 
					RASTERIZER_TYPE::CULL_BACK));

		}
		//// Light Shadow
		{
			CreateGraphicsShader(PROTO_RES_LIGHTDIRECTIONSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"LightDirection", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT_R8G8B8A8_UNORM },
					RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST
				), GRAPHICRENDEROBJECT_TYPE::LIGHT);

			CreateGraphicsShader(PROTO_RES_LIGHTPOINTSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"LightPoint", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT_R8G8B8A8_UNORM },
					RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST,
					BLEND_TYPE::ADD_BLEND), GRAPHICRENDEROBJECT_TYPE::LIGHT);
		}
		// NonAlpha
		{
			CreateGraphicsShader(PROTO_RES_MODELSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Model", VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT
					}));

		//	CreateGraphicsShader(PROTO_RES_EQUIPMENTSHADER, CLONETYPE::CLONE_STATIC,
		//		SHADERDESC(L"EquipmentModel", VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement,
		//			SHADERLIST{ VS_MAIN, PS_MAIN },
		//			RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		//			DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT
		//			}));

			CreateGraphicsShader(PROTO_RES_ANIMMODELSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"AnimModel", VTXANIMMODEL_DECLARATION::Element, VTXANIMMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT
					} ));

			CreateGraphicsShader(PROTO_RES_SHADOWSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Shadow", VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R32_FLOAT
					}));

			CreateGraphicsShader(PROTO_RES_SHADOWANIMSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"ShadowAnim", VTXANIMMODEL_DECLARATION::Element, VTXANIMMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R32_FLOAT
					}));

			CreateGraphicsShader(PROTO_RES_SHADOWEQUIPMENTSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"ShadowEquipment", VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R32_FLOAT
					}));

			CreateGraphicsShader(PROTO_RES_2DFIRESHADOWSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DFireShadow", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R32_FLOAT
					}));

			CreateGraphicsShader(PROTO_RES_NORPOSSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"NorPos", VTXNORPOSINPUT_DECLARATION::Element, VTXNORPOSINPUT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R32G32B32A32_FLOAT
					}));
			CreateGraphicsShader(PROTO_RES_DEPTHRECORDSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"DepthRecord", VTXSHADOWINPUT_DECLARATION::Element, VTXSHADOWINPUT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R32G32B32A32_FLOAT
					}));

			CreateGraphicsShader(PROTO_RES_ANIMNORPOSSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"AnimNorPos", VTXANIMMODEL_DECLARATION::Element, VTXANIMMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R32G32B32A32_FLOAT
					}));

			CreateGraphicsShader(PROTO_RES_ANIMDEPTHRECORDSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"AnimDepthRecord", VTXANIMMODEL_DECLARATION::Element, VTXANIMMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R32G32B32A32_FLOAT
					}));

			CreateGraphicsShader(PROTO_RES_NOCULL_ANIMMODELSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"AnimModel", VTXANIMMODEL_DECLARATION::Element, VTXANIMMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT
					}, RASTERIZER_TYPE::CULL_NONE));

			CreateGraphicsShader(PROTO_RES_3DNORMALCUBESHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"3DNormalCube", VTXNORMALCUBE_DECLARATION::Element, VTXNORMALCUBE_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT
					}));




			CreateGraphicsShader(PROTO_RES_NORMALOBJECTSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"NormalObject", VTXNORMAL_DECLARATION::Element, VTXNORMAL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT
					}));

			CreateGraphicsShader(PROTO_RES_EQUIPMENTSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Equipment", VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT
					}, RASTERIZER_TYPE::CULL_BACK));

		//	CreateGraphicsShader(PROTO_RES_MIRRORSHADER, CLONETYPE::CLONE_STATIC,
		//		SHADERDESC(L"Mirror", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
		//			SHADERLIST{ VS_MAIN, PS_MAIN },
		//			RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		//			DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT
		//			}));

		//	CreateGraphicsShader(PROTO_RES_WATERSHADER, CLONETYPE::CLONE_STATIC,
		//		SHADERDESC(L"Water", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
		//			SHADERLIST{ VS_MAIN, PS_MAIN },
		//			RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		//			DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT },
		//			RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		//// Alpha
		{
			CreateGraphicsShader(PROTO_RES_PARTICLE2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticle", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_PARTICLEROTATION2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleRotation", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_PARTICLEFOOTPRINT2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleFootPrint", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },

					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_PARTICLEBLOOD2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleBlood", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_PARTICLEATTACK2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleAttack", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_PARTICLEATTACKTWO2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleAttackTwo", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_PARTICLEHEAL2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleHeal", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));
			
			CreateGraphicsShader(PROTO_RES_PARTICLESKULL2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleSkull", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_NONE, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));


			CreateGraphicsShader(PROTO_RES_PARTICLEOPENCHEST2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleOpenChest", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_PARTICLESLASH2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleSlash", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_EQUAL_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_PARTICLEFLARE2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DParticleFlare", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},				
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_2DANIMATEPARTICLESHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DAnimateParticle", VTXPOINT_DELCARTION::Element, VTXPOINT_DELCARTION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN, GS_MAIN },
					
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_NO_WRITE, BLEND_TYPE::ALPHA_BLEND,
					D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST));

			CreateGraphicsShader(PROTO_RES_2DFIRESHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DFire", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM,DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R16G16B16A16_FLOAT
					},
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::LESS_EQUAL, BLEND_TYPE::ALPHA_BLEND
					));

			CreateGraphicsShader(PROTO_RES_2DFOGSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"2DFog", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }
				));
		
		}
		//// SkyBox
		//{
		//	CreateGraphicsShader(PROTO_RES_SKYBOXSHADER, CLONETYPE::CLONE_STATIC,
		//		SHADERDESC(L"SkyBox", VTXSKYBOX_DECLARATION::Element, VTXSKYBOX_DECLARATION::iNumElement,
		//			SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		//			DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT },
		//			RASTERIZER_TYPE::CULL_FRONT, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST));
		//}
		// Ect 
		{

			CreateGraphicsShader(PROTO_RES_DEBUGGINGWIREFRAMESHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"DebugCell", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT },
					RASTERIZER_TYPE::WIREFRAME, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST));

			CreateGraphicsShader(PROTO_RES_DEBUGGINGDEFAULTSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"DebugDefaultObject", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT },
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST, BLEND_TYPE::ALPHA_BLEND));

			CreateGraphicsShader(PROTO_RES_DEBUGGINGDEFAULTWHITESHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"DebugDefaultObjectWhite", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN },
					RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT },
					RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_RECTSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Rect", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }));
		}
		{
			CreateGraphicsShader(PROTO_RES_GRIDSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"Grid", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::WIREFRAME));
		}
		{
			CreateGraphicsShader(PROTO_RES_DEFAULTHIGHLIGHTUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"DefaultHighlightUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_DEFAULTUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"DefaultUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_BACKGROUNDUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"BackGroundUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE));
		}
		{
			CreateGraphicsShader(PROTO_RES_BUTTONUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"ButtonUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_LOADINGUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"LoadingUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_GAGEUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"GageUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_COOLTIMEUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"CoolTimeUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_LOADINGDOTUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"LoadingDotUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_PLEASEWAITUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"PleaseWaitUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		{
			CreateGraphicsShader(PROTO_RES_HPBARUISHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC(L"HpBarUI", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
					SHADERLIST{ VS_MAIN, PS_MAIN }, RASTERIZER_TYPE::CULL_BACK, DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE, BLEND_TYPE::ALPHA_BLEND));
		}
		//// Create  Tess
		//{
		//	CreateGraphicsShader(PROTO_RES_TERRAINTESSALATIONSHADER, CLONETYPE::CLONE_STATIC,
		//		SHADERDESC(L"TerrainTess", VTXNORMAL_DECLARATION::Element, VTXNORMAL_DECLARATION::iNumElement,
		//			SHADERLIST{ VS_MAIN, PS_MAIN, DS_MAIN, HS_MAIN },
		//			RENDERFORMATS{ DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R32G32B32A32_FLOAT,
		//			DXGI_FORMAT_R32G32B32A32_FLOAT,DXGI_FORMAT_R32G32B32A32_FLOAT },
		//			RASTERIZER_TYPE::WIREFRAME, DEPTH_STENCIL_TYPE::LESS, BLEND_TYPE::DEFAULT,
		//			D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST));
		//}
		//// Ect 
		//{
		//	CreateGraphicsShader(PROTO_RES_2DUISHADER, CLONETYPE::CLONE_STATIC,
		//		SHADERDESC(L"UI2D", VTXDEFAULT_DECLARATION::Element, VTXDEFAULT_DECLARATION::iNumElement,
		//			SHADERLIST{ VS_MAIN, PS_MAIN }, RENDERFORMATS{
		//			DXGI_FORMAT_R32G32B32A32_FLOAT }, RASTERIZER_TYPE::CULL_BACK,
		//			DEPTH_STENCIL_TYPE::LESS));
		//}
		}
	//// Compute Shader 
	{
		CreateComputeShader(PROTO_RES_COMPUTEANIMATIONSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"ComputeAnimation" });

		CreateComputeShader(PROTO_RES_COMPUTEPARTICLE2DSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"Compute2DParticle" });
		
		CreateComputeShader(PROTO_RES_COMPUTEROTATIONEFFECT2DSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"Compute2DRotationEffect" }); 

		CreateComputeShader(PROTO_RES_COMPUTEFOOTPRINT2DSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"Compute2DFootPrint" });

		CreateComputeShader(PROTO_RES_COMPUTESLASH2DSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"Compute2DSlash" });

		CreateComputeShader(PROTO_RES_COMPUTEEMITPARTICLE2DSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"Compute2DEmitParticle" });

		CreateComputeShader(PROTO_RES_COMPUTEBLOODEFFECT2DSHADER, CLONETYPE::CLONE_STATIC,
				SHADERDESC{ L"Compute2DBloodEffect" });

		CreateComputeShader(PROTO_RES_COMPUTEATTACKEFFECT2DSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"Compute2DAttackEffect" });

		CreateComputeShader(PROTO_RES_COMPUTEHEAL2DSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"Compute2DHeal" });

		CreateComputeShader(PROTO_RES_COMPUTEOPENCHEST2DSHADER, CLONETYPE::CLONE_STATIC,
			SHADERDESC{ L"Compute2DOpenChest" });
	}

	// Particle System 
	{
		AddPrototype(PROTO_RES_PARTICLESYSTEM, CLONETYPE::CLONE_STATIC, CreateConstructorToNative<UParticleSystem>(_stData.wpDevice.lock()));
	}
	return S_OK;
}

HRESULT UGameInstance::ReadyComp(const OUTPUTDATA& _stData)
{
	// Add Deffered Camera
	{
		AddPrototype(PROTO_ACTOR_DEFFEREDCAMERA, CreateConstructorToNative<UDefferedCamera>(
			_stData.wpDevice.lock(), LAYER_CAM, CLONETYPE::CLONE_STATIC));
	}
	{
		AddPrototype(PROTO_ACTOR_SMALL_DEFFEREDCAMERA, CreateConstructorToNative<USmallDefferedCamera>(
			_stData.wpDevice.lock(), LAYER_CAM, CLONETYPE::CLONE_STATIC));
	}
	{
		AddPrototype(PROTO_ACTOR_SHADOWCAMERA, CreateConstructorToNative<UShadowCamera>(
			_stData.wpDevice.lock(), LAYER_CAM, CLONETYPE::CLONE_STATIC));
	}
	{
		m_spRenderer = CreateConstructorToNative<URenderer>(_stData.wpDevice.lock(), _stData.wpGpuCmd.lock(), m_spGraphicDevice,
			m_spPipeLine, m_spSceneManager, m_spRenderTargetManager, nullptr);
		AddPrototype(PROTO_COMP_RENDERER, m_spRenderer);
	}
	//{
	//	// Add Rect Transform
	//	AddPrototype(PROTO_COMP_RECTTRANSFORM, CreateConstructorToNativeNotMsg<URectTransform>(_stData.wpDevice.lock()));
	//}
	{
		AddPrototype(PROTO_COMP_SPHERECOLLIDER, CreateConstructorToNativeNotMsg<UCollider>(_stData.wpDevice.lock(), UCollider::TYPE_SPHERE));
		AddPrototype(PROTO_COMP_ABBCOLLIDER, CreateConstructorToNativeNotMsg<UCollider>(_stData.wpDevice.lock(), UCollider::TYPE_AABB));
		AddPrototype(PROTO_COMP_OBBCOLLIDER, CreateConstructorToNativeNotMsg<UCollider>(_stData.wpDevice.lock(), UCollider::TYPE_OBB));
	}
	{
		AddPrototype(PROTO_COMP_RECTTRANSFORM, CreateConstructorToNative<URectTransform>(_stData.wpDevice.lock()));
	}
	return S_OK;
}

HRESULT UGameInstance::ReadyActor(const OUTPUTDATA& _stData)
{
	// Add Default 
	{
		AddPrototype(PROTO_ACTOR_DEFAULTCUBE, CreateConstructorToNative<UDefaultCube>(
			_stData.wpDevice.lock(), LAYER_DEFAULT, CLONETYPE::CLONE_ONCE));

#ifdef _USE_DEBUGGING
		AddPrototype(PROTO_ACTOR_DEUBGGINGDEFAULTOBJECT, CreateConstructorToNative<UDefaultDebugging>(
			_stData.wpDevice.lock(), LAYER_DEBUG, CLONETYPE::CLONE_STATIC));
		
		AddPrototype(PROTO_ACTOR_DEUBGGINGDEFAULTCELL, CreateConstructorToNative<UDefaultCell>(
			_stData.wpDevice.lock(), LAYER_DEBUG, CLONETYPE::CLONE_STATIC));

#endif 
		AddPrototype(PROTO_ACTOR_GRID, CreateConstructorToNative<UGrid>(
			_stData.wpDevice.lock(), LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	}
	AddPrototype(PROTO_ACTOR_PARTICLE, CreateConstructorToNative<UParticle>(
		_stData.wpDevice.lock(), LAYER_PARTICLE, CLONETYPE::CLONE_ONCE));

	AddPrototype(PROTO_ACTOR_TRAIL, CreateConstructorToNative<UTrail>(
		_stData.wpDevice.lock(), LAYER_TRAIL, CLONETYPE::CLONE_ONCE));
	
	AddPrototype(PROTO_ACTOR_GUARD, CreateConstructorToNative<UGuard>(
		_stData.wpDevice.lock(), LAYER_GUARD, CLONETYPE::CLONE_ONCE));
	
	AddPrototype(PROTO_ACTOR_BLOOD, CreateConstructorToNative<UBlood>(
		_stData.wpDevice.lock(), LAYER_BLOOD, CLONETYPE::CLONE_ONCE));

	AddPrototype(PROTO_ACTOR_DUST, CreateConstructorToNative<UDust>(
		_stData.wpDevice.lock(), LAYER_DUST, CLONETYPE::CLONE_ONCE));

	AddPrototype(PROTO_ACTOR_MAT, CreateConstructorToNative<UMat>(
		_stData.wpDevice.lock(), LAYER_MAT, CLONETYPE::CLONE_ONCE));

	AddPrototype(PROTO_ACTOR_FIRE, CreateConstructorToNative<UFire>(
		_stData.wpDevice.lock(), LAYER_DEFAULT, CLONETYPE::CLONE_ONCE));

	AddPrototype(PROTO_ACTOR_FOG, CreateConstructorToNative<UFog>(
		_stData.wpDevice.lock(), LAYER_DEFAULT, CLONETYPE::CLONE_ONCE));
	/*{
		AddPrototype(PROTO_ACTOR_TERRAIN, CreateConstructorToNative<UTerrain>(
			_stData.wpDevice.lock(), LAYER_TERRAIN, CLONETYPE::CLONE_ONCE));

		AddPrototype(PROTO_ACTOR_SKYBOX, CreateConstructorToNative<USkyBox>(
			_stData.wpDevice.lock(), LAYER_SKYBOX, CLONETYPE::CLONE_ONCE));

		AddPrototype(PROTO_ACTOR_PARTICLE, CreateConstructorToNative<UParticle>(
			_stData.wpDevice.lock(), LAYER_PARTICLE, CLONETYPE::CLONE_ONCE));



		AddPrototype(PROTO_ACTOR_MIRROR, CreateConstructorToNative<UMirror>(
			_stData.wpDevice.lock(), LAYER_MIRROR, CLONETYPE::CLONE_ONCE));

		AddPrototype(PROTO_ACTOR_SCREENRENDEROBJ, CreateConstructorToNative<UScreenRenderObj>(
			_stData.wpDevice.lock(), LAYER_SCREENRENDEROBJ, CLONETYPE::CLONE_ONCE));

		AddPrototype(PROTO_RES_MIRRORCAMERA, CreateConstructorToNative<UMirrorCamera>(
			_stData.wpDevice.lock(), LAYTER_CAM, CLONETYPE::CLONE_ONCE));
	}*/
	return S_OK;
}

HRESULT UGameInstance::ReadyRenderTarget(const OUTPUTDATA& _stData)
{
	CSHPTRREF<GRAPHICDESC> GraphicDesc = m_spGraphicDevice->GetGraphicDesc();

	{
			// LightShadow 
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::LIGHTSHADE_AMBIENT_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } },
					RTDESC{ RTOBJID::LIGHTSHADE_SHADE_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
						GraphicDesc->iWinCX, GraphicDesc->iWinCY, {0.f, 0.f, 0.f, 1.f} },
					RTDESC{ RTOBJID::LIGHTSHADE_SPECULAR_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
							GraphicDesc->iWinCX, GraphicDesc->iWinCY, {0.f, 0.f, 0.f, 0.f } }
			};
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::LIGHTSHADE_DEFFERED, vecRts);
		}

		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::SHADOW_DEPTH_FOURBYFOUR, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT,
					GraphicDesc->iWinCX*4, GraphicDesc->iWinCY*4, {0.f,0.f, 0.f, 0.f}}
			};
			//16384x16384
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroupWithNewDepthStencilBuffer(RTGROUPID::SHADOW_MAP, vecRts);
		}

		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::OUTLINE_DEPTH_POS, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } }
			};
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::OUTLINE_POS_NOR, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::OUTLINE_DEPTH_POS_FORABILITY, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } }
			};
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::OUTLINE_POS_NOR_FORABILITY, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::DEPTH_RECORD, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 1.f, 1.f, 1.f, 1.f } }
			};
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::DEPTH_RECORD, vecRts);
		}

		// NonAlpha_Deffered 
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::NONALPHA_DIFFUSE_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 1.f, 0.f } },
					RTDESC{RTOBJID::NONALPHA_SPECULAR_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } },
					RTDESC{ RTOBJID::NONALPHA_NORMAL_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
						GraphicDesc->iWinCX, GraphicDesc->iWinCY, {1.f, 1.f, 1.f, 1.f}},
					RTDESC{ RTOBJID::NONALPHA_DEPTH_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
							GraphicDesc->iWinCX, GraphicDesc->iWinCY, {1.f, 1.f, 1.f, 1.f}},
					RTDESC{ RTOBJID::NONALPHA_POSITION_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
							GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }},
					RTDESC{ RTOBJID::NONALPHA_GLOW_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }},
						//GLow 영역을 픽셀단위로 텍스쳐에 식별색상(ex.빨간색)으로 출력하여 그 해당 영역 조명에서 쎄게받게하던지. },
					RTDESC{ RTOBJID::NONALPHA_VELOCITY_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
						GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }}
			};
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED, vecRts);
		}
		// Alpha Deffered
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::ALPHA_DIFFUSE_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } },
					RTDESC{ RTOBJID::ALPHA_GLOW_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM,
						GraphicDesc->iWinCX, GraphicDesc->iWinCY, {0.f, 0.f, 0.f, 0.f} },
					RTDESC{ RTOBJID::ALPHA_BLUR_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT,
							GraphicDesc->iWinCX, GraphicDesc->iWinCY, {0.f, 0.f, 0.f, 0.f} }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::ALPHA_DEFFERED, vecRts);
		}

		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::BLOOM, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::BLOOM, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::BLUR, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX / 2, GraphicDesc->iWinCY / 2, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::BLUR, vecRts);
		}

		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::DOWNSAMPLETWO, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX / 16, GraphicDesc->iWinCY / 16, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::DOWNSAMPLETWO, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::DOWNSAMPLE, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX / 4, GraphicDesc->iWinCY / 4, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::DOWNSAMPLE, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::UPSAMPLE, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::UPSAMPLE, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::UPSAMPLEGRAY, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::UPSAMPLEGRAY, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::BLUR_RESULT, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX / 2, GraphicDesc->iWinCY / 2, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::BLUR_RESULT, vecRts);
		}


		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::HDR, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::HDR, vecRts);
		}

		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::HDRTWO, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::HDRTWO, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::GRAY_SCALE, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::GRAY_SCALE, vecRts);
		}

		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::UI2D_SCREEN_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } }
			};
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::UI2D_DEFFERED, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::BLEND_SCREEN_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } }
			};
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::BLEND_DEFFERED, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::MOTIONBLUR_DIFFUSE_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } },
				RTDESC{ RTOBJID::MOTIONBLUR_DIFFUSE_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f } }
			};
			// Add RenderTargetGroup
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::MOTIONBLUR_DEFFERED, vecRts);
		}
		{
			std::vector<RTDESC> vecRts{
				RTDESC{ RTOBJID::MIDDLERENDER_DIFFUSE_DEFFERED, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,
					GraphicDesc->iWinCX, GraphicDesc->iWinCY, { 0.f, 0.f, 0.f, 0.f }  }
			};
			// Add 
			m_spRenderTargetManager->AddRenderTargetGroup(RTGROUPID::MIDDLERENDERSCREEN_DEFFERED, vecRts);
		}
		
	}

#ifdef _USE_DEBUGGING
	/*
	렌더 타겟 디버깅을 위한 것
	*/

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::SHADOW_MAP, RTOBJID::SHADOW_DEPTH_FOURBYFOUR,
		_float2(300.f, 300.f), _float2(300.f, 300.f), m_spGraphicDevice->GetGraphicDesc());
	/*m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::MIDDLERENDERSCREEN_DEFFERED, RTOBJID::MIDDLERENDER_DIFFUSE_DEFFERED,
		_float2(100.f, 100.f), _float2(200.f, 200.f), m_spGraphicDevice->GetGraphicDesc());

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::NONALPHA_DEFFERED, RTOBJID::NONALPHA_VELOCITY_DEFFERED,
		_float2(100.f, 310.f), _float2(200.f, 200.f), m_spGraphicDevice->GetGraphicDesc());

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::MOTIONBLUR_DEFFERED, RTOBJID::MOTIONBLUR_DIFFUSE_DEFFERED,
		_float2(100.f, 520.f), _float2(200.f, 200.f), m_spGraphicDevice->GetGraphicDesc());


	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::NONALPHA_DEFFERED, RTOBJID::NONALPHA_GLOW_DEFFERED,
		_float2(100.f, 430.f), _float2(100.f, 100.f), m_spGraphicDevice->GetGraphicDesc());


	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::LIGHTSHADE_DEFFERED, RTOBJID::LIGHTSHADE_SHADE_DEFFERED,
		_float2(100.f, 540.f), _float2(100.f, 100.f), m_spGraphicDevice->GetGraphicDesc());

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::LIGHTSHADE_DEFFERED, RTOBJID::LIGHTSHADE_SPECULAR_DEFFERED,
		_float2(100.f, 650.f), _float2(100.f, 100.f), m_spGraphicDevice->GetGraphicDesc());
	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::LIGHTSHADE_DEFFERED, RTOBJID::LIGHTSHADE_AMBIENT_DEFFERED,
		_float2(100.f, 760.f), _float2(100.f, 100.f), m_spGraphicDevice->GetGraphicDesc());


	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::HDR, RTOBJID::HDR,
		_float2(100.f, 870.f), _float2(100.f, 100.f), m_spGraphicDevice->GetGraphicDesc());

	

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::DEPTH_RECORD, RTOBJID::DEPTH_RECORD,
		_float2(300.f, 700.f), _float2(300.f, 300.f), m_spGraphicDevice->GetGraphicDesc());

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::OUTLINE_POS_NOR, RTOBJID::OUTLINE_DEPTH_POS,
		_float2(605.f, 700.f), _float2(300.f, 300.f), m_spGraphicDevice->GetGraphicDesc());

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::NONALPHA_DEFFERED, RTOBJID::NONALPHA_DEPTH_DEFFERED,
		_float2(910.f, 700.f), _float2(300.f, 300.f), m_spGraphicDevice->GetGraphicDesc());
	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::OUTLINE_POS_NOR, RTOBJID::OUTLINE_DEPTH_POS,
		_float2(300.f, 700.f), _float2(300.f, 300.f), m_spGraphicDevice->GetGraphicDesc());

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::OUTLINE_POS_NOR_FORABILITY, RTOBJID::OUTLINE_DEPTH_POS_FORABILITY,
		_float2(605.f, 700.f), _float2(300.f, 300.f), m_spGraphicDevice->GetGraphicDesc());

	m_spRenderTargetManager->AddDebugRenderObjects(RTGROUPID::DEPTH_RECORD, RTOBJID::DEPTH_RECORD,
		_float2(910.f, 700.f), _float2(300.f, 300.f), m_spGraphicDevice->GetGraphicDesc());*/
#endif
	return S_OK;
}


