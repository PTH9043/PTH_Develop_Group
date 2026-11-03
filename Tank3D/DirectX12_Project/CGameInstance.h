#pragma once
#include "CBase.h"

class CTimer_Manager;
class CInput_Manager;
class CActor_Manager;
class CComponent_Manager;
class CPipeLine;
class CRenderer;
class CSceneManager;

class CCamera;
class CTimer;
class CActor;
enum DMBUTTON;
class CComponent;
enum MATRIXTYPE;
class CScene;

class CGameInstance : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
protected:
	explicit CGameInstance();
	virtual ~CGameInstance() = default;
public:
	virtual void Free() override;
public:
	HRESULT NativConstruct(const HWND& _hWnd, const HINSTANCE& _hInstance);
private:
	HRESULT Ready_Actor();
	HRESULT Ready_Component();
public:
	// TIck Late Tick
	void Tick(const _double& _dTimeDelta);
	void Late_Tick(const _double& _dTimeDelta);
	void Render(const HDC& _hDC);

public:  // Timer _Manager
	// Get Timer Delta
	_double Get_TimeDelta(const _wstring& _strTimerTag);
	HRESULT Add_Timer(const _tchar* _pTimerTag);
	void Change_CurTimer(const _wstring& _strTimerTag);
	// Find Timer
	shared_ptr<CTimer> Find_Timer(const _wstring& _strTimerTag);
public: // Input _Manager
	HRESULT RegisterMethod(const _ubyte& _KEYID, const function<_bool(const _double&)>& _pDelegate);
	HRESULT RunningKeyMethods(const _ubyte& _KEYID, const _double& _dTimeDelta);
	bool Is_Press( _int _iKey);
	bool Is_Pressing(const _int& _iKey);
	bool Is_MouseClick(const DMBUTTON& _eKey);
	const _float2& GetMousePos() const;
public: /* Actor Manager */
	// Add Prototype
	void Add_Prototypes(const _wstring& _strName, const shared_ptr<CActor>& _pActor);
	void Clone_Actor(const _wstring& _strProtoNames, const _wstring& _strCloneLayer, const VOIDDATAS& _tDatas = {});
	shared_ptr<CActor> Clone_Actor_Add(const _wstring& _strProtoNames, const _wstring& _strCloneLayer, const VOIDDATAS& _tDatas = {});
	void Add_DeleteActor(const _wstring& _strLayer, const shared_ptr<CActor>& _pActor);
	// Get Actor List
	list<shared_ptr<CActor>> Get_ActorList(const _wstring& _strCloneLayer);
	void Clear();
public: /* Component Manager */
	void Add_Prototypes(const _wstring& _strPrototypes, const shared_ptr<CComponent>& _pComponent);
	shared_ptr<CComponent> Clone(const _wstring& _strPrototypes, const VOIDDATAS& _tDatas = {});
public: /* PipeLine */
	// Add Camera
	void Add_Camera(const _wstring& _strCamera, const shared_ptr<CCamera>& _pCamera);
	void ChangeCamera(const _wstring& _strCamera);
	// Bind Cam Matrix
	void BindCameraMatrix();
	void ClearCamera();
	// ComBine Matrix
	_float4x4 CombineMatrix(const shared_ptr<class CActor>& _pActor);
	void ChangeScreenToPos(const _float4x4& _WVP, _float3& _vPos);
	void ChangeScreenToPos(_float3& _vPos);
	void ChangeScreenToPos(const shared_ptr<class CActor>& _pActor, _float3& _vPos);
	void Bind_Matrix4x4(const MATRIXTYPE& _eType, const _float4x4& _vMatrix);
	void Bind_Matrix(const MATRIXTYPE& _eType, const _matrix& _matWorld);
	void Set_CamPosition(const _float3& _vCamPos);

	_float4x4 Get_Matrix4x4(const MATRIXTYPE& _eType);
	_matrix Get_Matrix(const MATRIXTYPE& _eType);
	const _float3& Get_CamPos() const;
public: /* SceneManager */
	// RegisterScene
	void RegisterScene(const shared_ptr<CScene>& _pScene);
	const shared_ptr<CScene>& Get_CurScene() const;
public:

	const RECT& Get_WindowRect() const { return m_tWindowRect; }
private:
	shared_ptr<CTimer_Manager>				m_pTimer_Manager = nullptr;
	shared_ptr<CInput_Manager>				m_pInput_Manager = nullptr;
	shared_ptr<CActor_Manager>				m_pActor_Manager = nullptr;
	shared_ptr<CComponent_Manager>	m_pComponent_Manager = nullptr;
	shared_ptr<CPipeLine>							m_pPipeLine = nullptr;
	shared_ptr<CSceneManager>				m_pSceneManager = nullptr;
	// Renderer 
	shared_ptr< CRenderer>						m_pRenderer = nullptr;
	RECT															m_tWindowRect{};

	HWND															m_Hwnd = NULL;
	HINSTANCE												m_hInstnace = NULL;
};

