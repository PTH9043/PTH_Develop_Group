#pragma once
#include "EngineDefine.h"
#include "UBase.h"

BEGIN(Engine)
class UGraphicRenderManager;
class UShaderBufferManager;
class UGraphicDevice;
class UTimerManager;
class UInputManager;

class UFontManager;
class UActorManager;
class UComponentManager;
class UResourceManager;
class USceneManager;

class UPipeLine;
class URenderTargetManager;
class UComputeManager;
class UPicking;
class UFilePathManager;
class URandomManager;
class UThreadManager;
class UNetworkBaseController;
class UAudioSystemManager;
class UCharacterManager;
class UMaterialManager;
class UAnimEventManager;
class UNetworkQueryProcessing;
class UFontManager;

class URenderer;
class UCollider;

class URootSignature;
class UShaderConstantBuffer;
class UGlobalConstantBuffer;
class UShader;
class UTexture;
class URenderTargetGroup;

class UActor;
class UComponent;
class UResource;
class UCamera;
class UTimer;
class UVIBuffer;
class UScene;
class ULight;
class UTableDescriptor;
class UGpuCommand;
class UComputeCommand;

class UActorGroup;
class UCommand;
class UFont;
class UNetworkBaseController;
class UProcessedData;
class USound;
class UTransform;
class UCharacter;

class UStageManager;
class UStage;

class UGrid;
class UPawn;
class UModelMaterial;
class USwapChain;
class UFont;
class UPlayer;
class UNetworkSender;
class UNetworkQueryProcessing;

struct PICKINGDESC;
struct WAITCHECKACTOR;
struct MAINGRID;

using CLONES = UNORMAP<_wstring, SHPTR<UActorGroup>>;
using CLONEARR = ARRAY<CLONES, CLONE_MAX_SIZE>;
using AUDIOSYSTEMCONTAINER = ARRAY <SHPTR< class UAudioSystem>, SOUND_END>;

class UGameInstance : public UBase  {
	DECLARE_SINGLETON(UGameInstance)
public:
	UGameInstance();
	virtual ~UGameInstance();
public:
	const _bool IsGamming() const { return m_isGamming; }
	void SetGamming(const _bool _isGamming) { this->m_isGamming = _isGamming; }
public:
	/* GameInstance */
	// Free 
	virtual void Free() override;
	// Ready Instance
	HRESULT ReadyInstance(const GRAPHICDESC& _stDesc, OUTPUTDATA& _stOutDesc);
	// Create Shader
	HRESULT CreateGraphicsShader(const _wstring& _wstrProtoName, const CLONETYPE _eCloneType,
		const SHADERDESC& _stShaderDesc, const GRAPHICRENDEROBJECT_TYPE _eType = GRAPHICRENDEROBJECT_TYPE::COMMON);
	HRESULT CreateComputeShader(const _wstring& _wstrProtoName, const CLONETYPE _eCloneType,
		const SHADERDESC& _stShaderDesc);
public:
	void OtherFrame(const _double& _dTimeDelta, const WPARAM& _wParam);
	void AwakeTick();
	void Tick(_double _dTimeDelta);
	void LateTick(_double _dTimeDelta);
	void RenderBegin();
	void RenderEnd();

	void SetImGuiContext(ImGuiContext* _pContext);
public:
	HRESULT	OnWindowResize(const _uint& _iWinSizeX, const _uint& _iWinSizeY, const GRAPHICDESC::WINMODE _eWindowMode);
	// Clear Once Type data 
	//4-27 추가
	void SetDefaultViewPort();
	void SetTemporaryViewPort(D3D12_VIEWPORT _viewport, D3D12_RECT _rect);
	void ClearOnceTypeData();
public:
	CSHPTRREF<UDevice>						GetDevice() const;
	CSHPTRREF<UGpuCommand>		GetGpuCommand() const;
	CSHPTRREF<UComputeCommand>		GetComputeCommand() const;
	CSHPTRREF<UTableDescriptor>	GetTableDescriptor() const;
	CSHPTRREF<UTableDescriptor>	GetComputeTableDescriptor() const;
	CSHPTRREF<URootSignature>	GetRootSignature() const;
	CSHPTRREF<USwapChain> GetSwapChain() const;

public: /*  GraphicDevice */
	const _bool	IsGamePaused() const;
	const _bool	IsScreenMinimized() const;
	const _bool	IsScreenMaximized() const;
	const _bool	IsScreenFullScreen() const;
	const _bool	IsMssa4xState() const;
	const _uint& GetMssa4xQuality() const;
	const D3D12_VIEWPORT& GetD3DViewport() const;
	const D3D12_RECT& GetD3DWindowSizeRect() const;
	CSHPTRREF<GRAPHICDESC> GetGraphicDesc() const;
	// Ready
	SHPTR<UTexture>		GetDefaultBackTexture() const;

public: /* ShaderBufferManager*/
	void	 BindGlobalBuffer(const GLOBAL_CBUFFERTYPE _eGrobalCBuffer, CSHPTRREF<UCommand> _spCommand, const void* _pBuffer, const _uint _iSize);
	HRESULT GetGlobalConstantBuffer(const GLOBAL_CBUFFERTYPE _eGrobalCBuffer, SHPTRREF<UGlobalConstantBuffer> _spGrobalConstantBuffer);
	HRESULT GetPreAllocatedConstantBuffer(const PREALLOCATED_CBUFFERTYPE _ePreAllocatedCBufferType, SHPTRREF<UShaderConstantBuffer> _spShaderConstantBuffer);

public: /* TimerManager */
	HRESULT CreateTimer(const _wstring& _wstrName);
	SHPTR<UTimer> CreateTimerAdd(const _wstring& _wstrName);
	HRESULT RemoveTimer(const _wstring& _wstrName);

public: /* InputManager*/
	void AddKeyMethod( _ubyte _bFirstKeyID, const _wstring& _wstrSecondsKeyName, const KEYACTIONFUNC& _keyMethod);
	void DeleteKeyMethod( _ubyte _bFirstKeyID, const _wstring& _wstrSecondsKeyName);
	_bool GetDIKeyDown( _ubyte _bKeyID);
	_bool GetDIKeyUp( _ubyte _bKeyID);
	_bool GetDIKeyPressing( _ubyte _bKeyID);
	_bool GetDIMBtnDown(_ubyte _eMouseBtn);
	_bool GetDIMBtnUp(_ubyte _eMouseBtn);
	_bool GetDIMBtnPressing(_ubyte _eMouseBtn);
	_long GetDIMMoveState(_ubyte _eMouseMove);
	_float2 GetMousePosition();
	_bool IsMouseInWindowSize(const float _Width, const float _Height);
	_bool IsMouseInWindowSize();
public: /* ThreadManager */
	// 스레드에 THREADFUNC 함수를 등록
	void RegisterFuncToRegister(const THREADFUNC& _CallBack, void* _pData);
	// 등록 된 함수들을 JOIN
	void JoinRegister();
	// 등록된 함수들을 Detach
	void DetachRegister();
public: /* ActorManager */
	const  CLONEARR& GetClonesArr() const;
	HRESULT AddPrototype(const _wstring& _wstrName, CSHPTRREF<UActor> _spActor);
	void  CloneActor(const _wstring& _wstrProto, const VOIDDATAS& _stDatas = VOIDDATAS{});
	SHPTR<UActor> CloneActorAdd(const _wstring& _wstrProto, const VOIDDATAS& _stDatas = VOIDDATAS{});
	SHPTR<UActor> CloneActorAddAndNotInLayer(const _wstring& _wstrProto, const VOIDDATAS& _stDatas = VOIDDATAS{});
	void RemoveActor(CSHPTRREF<UActor> _spActor);

	void EnableLayers(const _wstring& _wstrLayer);
	void DisableLayers(const _wstring& _wstrLayer);
public: /*  ComponentManager */
	HRESULT AddPrototype(const _wstring& _wstrPrototypes, CSHPTRREF<UComponent> _spComponent);
	SHPTR<UComponent> CloneComp(const _wstring& _wstrPrototypes, const VOIDDATAS& _stDatas = VOIDDATAS{});

public: /* ResourceManager*/
	HRESULT AddPrototype(const _wstring& _wstrPrototype, const CLONETYPE& _eCloneType, CSHPTRREF<UResource> _spResource);
	SHPTR<UResource> CloneResource(const _wstring& _wstrPrototype, const VOIDDATAS& _stDatas = VOIDDATAS{});
	void		FindResourceProtoTag(CSHPTRREF<UResource> _spResource, _wstring& _wstrResourceTag);

	void AddPipeLineState(const _wstring& _wstrName, const ComPtr<Dx12PipelineState>& _cpPipeLineState);
	void SettingPipeLineState(const _wstring& _wstrName, CSHPTRREF<UCommand> _spCommand);
public: /* SceneManager*/
	void OutLightControlInfo(LIGHTPARAM& _stLightControl);
	void ChangeLightCamIndex(const CAMID& _iID);

	CSHPTRREF<UScene> GetCurScene() const;
	void RegisterScene(CSHPTRREF<UScene> _spScene);
	void RegisterSceneWithOutLoadingData(CSHPTRREF<UScene> _spScene);
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

public: /*  RenderTargetManager */
	HRESULT		AddRenderTargetGroup(const RTGROUPID& _eGroupID, const std::vector<RTDESC>& _rtVec);
	void					RemoveRenderTargetGroup(const RTGROUPID _eGroupID);

	// OMSetRenderTarget 
	void					OmSetRenderTargets(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID, const _uint _index, const _uint _iOffset);
	void					OmSetRenderTargets(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID);
	// ClearRenderTargetView
	void					ClearRenderTargetView(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID, const _uint _index);
	void					ClearRenderTargetView(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID);
	// Waiting
	void					WaitTargetToResource(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID);
	void					WaitResourceToTarget(CSHPTRREF<UCommand> _spCommand, const RTGROUPID& _eGroupID);
	// Get RT Group 
	SHPTR<URenderTargetGroup>	 FindRenderTargetGroup(const RTGROUPID& _eGroupID);
	SHPTR<UTexture> FindRenderTargetTexture(const RTGROUPID _eGroupID, const RTOBJID _eObjID);

public: /* PipeLine */
	void RegisterCameraInPipeline(CSHPTRREF<UCamera> _spCamera, CAMID& _iCamID,
		const VIEWPROJMATRIX& _stViewProjMatrix, const CAMERATYPE _eType = CAMERATYPE::OTHER);
	void RemoveCameraInPipeLine(CAMID _CamID);

	_bool IsFrustomContains(const _float3& _vPos, const _float _fRadius, const CAMID& _iCamID = MAIN_CAMERA_ID);

	const _float4x4 GetMainCamViewMatrix() const;
	const _float4x4& GetMainCamProjMatrix() const;
	const _float3& GetMainCamPosition();

	const _float4x4 GetCamViewMatrix(const CAMID& _iID) const;
	const _float4x4& GetCamProjMatrix(const CAMID& _iID) const;
	const _float3& GetCameraPosition(const CAMID& _iID);
	SHPTR<UTransform> GetMainCameraTransform();
	const _float GetCamFar(const CAMID& _iID);

	void ChangeRenderCamera(CSHPTRREF<UCamera> _spCamera);
	void ChangeRenderCamera(CAMID _iID);

	void AddRenderCamList(CSHPTRREF<UCamera> _spCamera);
	void AddRenderCamList(CAMID _iID);

	const CAMID GetRenderCamID() const;

public: /* FilePath Manager*/
	SHPTR<FILEGROUP> FindFolder(const PATHS& _vecFolderList);
	// 같은 이름을 가진 폴더를 찾아서 리턴한다. 단 PatentsFolderName이 비어있다면 최상위 폴더를 찾아서 리턴, 입력했으면 상위 폴더의 이름까지 검색한다. 
	SHPTR<FILEGROUP>	FindFolder(const _wstring& _wstrFindName, const _wstring& _wstrParentsFolderName = L"");
	// 같은 이름을 가진 모든 폴더를 찾아서 리턴한다. 
	VECTOR<SHPTR<FILEGROUP>> FindSameAllFolder(const _wstring& _wstrFindFolderName);
	HRESULT LoadFirstFolder(const _wstring& _wstrFilePath);
public: /* AudioSystemManager*/
	const AUDIOSYSTEMCONTAINER& GetAudioSystemContainer() const;
	SHPTR<UAudioSystem> GetAudioSystem(const SOUNDTYPE _SoundType);
	HRESULT CreateAudioSystemAndRegister(SOUNDTYPE _SoundType, const _wstring& _wstrSoundFolderPath);
	HRESULT CreateAudioSystemAndRegister(SOUNDTYPE _SoundType, CSHPTRREF<FILEGROUP> _spSoundFileGroup);
	HRESULT CreateAudioSystemToFolderNameAndRegister(SOUNDTYPE _SoundType, const _wstring& _wstrSoundFolderName);
	void SoundPlay(const _wstring& _wstrSoundName);
	void SoundPlayWithManyChannels(const _wstring& _wstrSoundName);
	void SoundPlay(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate);
	void SoundPlayOnce(const _wstring& _wstrSoundName);
	void SoundPlayOnceWithManyChannels(const _wstring& _wstrSoundName);
	void SoundPlayBGM(const _wstring& _wstrSoundName);
	void SoundStopBGM(const _wstring& _wstrSoundName);
	void SoundPauseBGM(const _wstring& _wstrSoundName);
	void SoundResumeBGM(const _wstring& _wstrSoundName);
	void SoundPlayBGM(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate);
	void StopSound(const _wstring& _wstrSoundName);
	void StopSoundWithManyChannels(const _wstring& _wstrSoundName);
	void SoundDisableOncePlay(const _wstring& _wstrSoundName);
	void Restart(const _wstring& _wstrSoundName);
	void SoundRestartOnce(const _wstring& _wstrSoundName);
	void Pause(const _wstring& _wstrSoundName);
	void Resume(const _wstring& _wstrSoundName);
	void SetPlaybackPosition(const _wstring& _wstrSoundName, unsigned int position);
	//unsigned int GetPlaybackPosition(const _wstring& _wstrSoundName);
	void SetLooping(const _wstring& _wstrSoundName, bool loop);
	void SetPitch(const _wstring& _wstrSoundName, float pitch);
	void SetPan(const _wstring& _wstrSoundName, float pan);
	void Mute(const _wstring& _wstrSoundName);
	void Unmute(const _wstring& _wstrSoundName);
	void FadeIn(const _wstring& _wstrSoundName, float fadeDuration);
	void FadeOut(const _wstring& _wstrSoundName, float fadeDuration);
	void UpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume);
	void BGMUpdateVolume(const _wstring& _wstrSoundName, const _float _fVolume);
	void UpdateSound3D(const _wstring& _wstrSoundName, const _float3& _vSoudPos, const _float3& _vSoundVelocity, 
		CSHPTRREF<UTransform> _spTargetTransform = nullptr);
	void VolumeUpdate(const _wstring& _wstrSoundName, const _float& _fVolumeUpdate);
	void ChangeMinMaxDistance3D(const _wstring& _wstrSoundName, const _float _fMinDistance, const _float _fMaxDistance);
	SHPTR<USound> BringSound(const _int _Index);
	SHPTR<USound> BringSound(const _wstring& _wstrSoundName);
	void HandleSoundsForAnimation(CSHPTRREF<UCharacter> _Owner, const _wstring& animName, const _wstring& SoundName, _float startThreshold, _float endThreshold);
void HandleSoundsForAnimationWithManyChannels(CSHPTRREF<UCharacter> _Owner, const _wstring& animName, const _wstring& SoundName, _float startThreshold, _float endThreshold);

public: /* NetworkManager */
	void StartNetwork(CSHPTRREF<UNetworkBaseController> _spNetworkBaseController, CSHPTRREF<UNetworkQueryProcessing> _spNetworkQueryProcessing);
	void MakeActors(const VECTOR<SHPTR<UActor>>& _actorContainer);
	void InsertSendTcpPacketInQuery(_char* _pPacket, _short _PacketType, _short _PacketSize);
	void InsertSendProcessPacketInQuery(UProcessedData&& _ProcessData);
	SHPTR<UActor> FindNetworkActor(const _int _NetworkID);
	void NetworkEnd();
	void SetSceneIDToNetController(const _int _iSceneID);
	const _llong GetNetworkOwnerID() const;
	const _bool IsNetworkResourceRecvSuccess() const;
	/*
	@ Date: 2024-02-04, Writer: 박태현
	@ Explain
	- ProtocolBuffer를 TCP에 연결된 상대에게 보내기 위한 템플릿 함수이다.
	*/
	template<class T>
	void CombineProto(REF_IN ARRAY<_char, MAX_BUFFER_LENGTH>& _Buffer, REF_IN PACKETHEAD& _PacketHead, 
		const T& _data, short _tag)
	{
		_data.SerializePartialToArray((void*)&_Buffer[0], static_cast<int>(_data.ByteSizeLong()));
		short size = static_cast<short>(_data.ByteSizeLong());
		_PacketHead = PACKETHEAD{ size, _tag };
	}
public: /* CharacterManager*/
	CSHPTRREF<UCharacter> GetCurrPlayer() const;
	void RegisterCurrentPlayer(CSHPTRREF<UCharacter> _spCurrentPlayer);
	void AddCollisionPawnList(CSHPTRREF<UPawn> _spPawn);
	void RemoveCollisionPawn(CSHPTRREF<UPawn> _spPawn);
	void RemovePlayerInContainer(CSHPTRREF<UPlayer> _spPlayer);
	void AddPlayerInContainer(SHPTR<UPlayer> _spPlayer);

	SHPTR<UPlayer> FindPlayerToNetworkID(_int _iNetworkID);
	SHPTR<UPlayer> FindPlayerToDistance(const _float3& _vPos);
public: /* Material Manager*/
	void AddModelMaterial(const _uint _MaterialIndex, CSHPTRREF<UModelMaterial> _spModelMaterial);
	void  CopyToMaterialShaderParam(REF_IN GLOBALPARAM& _GrobalParam);
	void RemoveModelMaterial(const _uint _MaterialIndex);
	const _bool IsMaterialContainerInfoChange() const;
	/*
	@ Date: 2024-03-08, Writer: 이성현
	@ Explain
	- Picking관련 함수들.
	*/
public: /* Picking */
	void AddPickingObject(CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer);
	void DeletePickingObject(CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer);
	void AddPickingGrid(const MAINGRID& _stGrid);
	SHPTR<UActor> GetPickingPawn();
	const PICKINGDESC GetPickDesc();
	_bool PickingMesh(const _float3& _RayPos, const _float3& _RayDir, CSHPTRREF<UPawn> _spPawn, CSHPTRREF<UVIBuffer> _spVIBuffer,
		_float* _pDist, _float3* _pOut);

public: /* Font Manager */
	HRESULT FontCreate(const _wstring& _wstrFontName, const _wstring& _wstrPath);
	SHPTR<UFont> FontCreateAdd(const _wstring& _wstrFontName, const _wstring& _wstrPath);
	SHPTR<UFont> AddFont(const _wstring& _wstrFontName);
	void ReleaseOriginFont(const _wstring& _wstrFontName);
	void RemoveFont(CSHPTRREF<UFont> _spFont);

private: /* Ready Datas */
	HRESULT ReadyResource(const OUTPUTDATA& _stData);
	HRESULT ReadyComp(const OUTPUTDATA& _stData);
	HRESULT ReadyActor(const OUTPUTDATA& _stData);
	HRESULT ReadyRenderTarget(const OUTPUTDATA& _stData);

public: //Renderer Fog Bool
	void TurnOnFog();
	void TurnOffFog();
	void SetGameStartEffect();
	void TurnOnDieEffect();
	void TurnOffDieEffect();
	void TurnOnAbilityEffect();
	void TurnOffAbilityEffect();
	void TurnOnHitEffect();
	void TurnOffHitEffect();
	_bool GetIfAbilityIsOn();
	_float GetAbilityTime();
	void PauseGame();
	void ResumeGame();
	void PauseGameTick();
	void ResumeGameTick();
public:

	void SetFreeModeCamEnable(const _bool _isFreeModeCamEnable) { this->m_isFreeModeCameraEnable = _isFreeModeCamEnable; }
	_bool IsFreeModeCameraEnable() const { return m_isFreeModeCameraEnable ; }

	void SetGamePause(const _bool _isGamePause) { this->m_isGamePause = _isGamePause; }
	_bool IsGamePause() const { return m_isGamePause; }
private:
	_bool															m_isGamming;
	_bool				m_isPause = false;

	SHPTR<UGraphicDevice>								m_spGraphicDevice;
	SHPTR<UShaderBufferManager>				m_spShaderBufferManager;
	SHPTR<UTimerManager>								m_spTimerManager;
	SHPTR<UInputManager>								m_spInputManager;
	SHPTR<UThreadManager>							m_spThreadManager;

	SHPTR<UActorManager>								m_spActorManager;
	SHPTR<UComponentManager>					m_spComponentManager;
	SHPTR<UResourceManager>						m_spResourceManager;
	SHPTR<USceneManager>							m_spSceneManager;

	SHPTR<URenderTargetManager>				m_spRenderTargetManager;

	SHPTR<UPipeLine>										m_spPipeLine;
	SHPTR<UFilePathManager>							m_spFilePathManager;
	SHPTR< UAudioSystemManager>				m_spAudioSystemManager;
	SHPTR< UNetworkBaseController>			m_spNetworkBaseController;
	SHPTR<UCharacterManager>						m_spCharacterManager;
	SHPTR< UMaterialManager>						m_spMaterialManager;

	SHPTR<URenderer>										m_spRenderer;
	SHPTR< UNetworkQueryProcessing>		m_spNetworkQueryProcessing;
	SHPTR< UNetworkSender>							m_spNetworkSender;

	SHPTR< UFontManager>								m_spFontMananger;
	//SHPTR<UComputeManager>					m_spComputeManager;
	SHPTR<UPicking>											m_spPicking;			

	//SHPTR<URandomManager>				m_spRandomManager;

	_bool																	m_isFreeModeCameraEnable;
	_bool																	m_isGamePause;
};

END