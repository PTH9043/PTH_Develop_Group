#include "CTimer_Manager.h"
#include "CInput_Manager.h"
#include "CActor_Manager.h"
#include "CComponent_Manager.h"
#include "CPipeLine.h"
#include "CRenderer.h"
#include "CSceneManager.h"

#include "CGameInstance.h"
#include "CCubeBuffer.h"
#include "CFloorBuffer.h"
#include "CCamera.h"
#include "CCube.h"
#include "COBBCollider.h"
#include "CAABBCollider.h"
#include "CTank.h"
#include "CControlHead.h"
#include "CPosin.h"
#include "CFloor.h"
#include "CEmptyObject.h"
#include "CMainCamera.h"
#include "CShooter.h"
#include "CNormalBullet.h"
#include "CCubeMonster.h"
#include "CRectTransform.h"
#include "CText.h"
#include "CTextButton.h"
#include "CCristalBuffer.h"
#include "CCristal.h"
#include "CBlood.h"


IMPLEMENT_SINGLETON(CGameInstance);

CGameInstance::CGameInstance() :
	m_pTimer_Manager(CreateNew<CTimer_Manager>()),
	m_pInput_Manager(CreateNew<CInput_Manager>()),
	m_pActor_Manager(CreateNew<CActor_Manager>()),
	m_pComponent_Manager(CreateNew<CComponent_Manager>()),
	m_pPipeLine(CreateNew<CPipeLine>()),
	m_pSceneManager(CreateNew<CSceneManager>())
{
}

void CGameInstance::Free()
{
	Safe_Free(m_pSceneManager);
	Safe_Free(m_pPipeLine);
	Safe_Free(m_pComponent_Manager);
	Safe_Free(m_pActor_Manager);
	Safe_Free(m_pInput_Manager);
	Safe_Free(m_pTimer_Manager);
}

HRESULT CGameInstance::NativConstruct(const HWND& _hWnd, const HINSTANCE& _hInstance)
{
	m_Hwnd = _hWnd; 
	m_hInstnace = _hInstance;

	if (nullptr == m_pActor_Manager || nullptr == m_pComponent_Manager)
		return E_FAIL;
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	Ready_Actor();
	Ready_Component();

	GetWindowRect(m_Hwnd, &m_tWindowRect);
	return S_OK;
}

HRESULT CGameInstance::Ready_Actor()
{
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_MAINCAMERA, CreateNew<CMainCamera>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_CUBE, CreateNew<CCube>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_TANK, CreateNew<CTank>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_CONTROLHEAD, CreateNew<CControlHead>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_POSIN, CreateNew<CPosin>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_FLOOR, CreateNew<CFloor>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_EMPTY, CreateNew<CEmptyObject>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_SHOOTER, CreateNew< CShooter>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_NORMALBULLET, CreateNew< CNormalBullet>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_CUBEMONSTER, CreateNew<CCubeMonster>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_UI_TEXT, CreateNew<CText>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_UI_TEXTBUTTON, CreateNew<CTextButton>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_CRISTAL, CreateNew< CCristal>());
	m_pActor_Manager->Add_Prototypes(PROTO_ACTOR_BLOOD, CreateNew<CBlood>());
	return S_OK;
}

HRESULT CGameInstance::Ready_Component()
{
	m_pRenderer = CreateNew<CRenderer>();
	m_pComponent_Manager->Add_Prototypes(PROTO_COMP_RENDER, m_pRenderer);
	m_pComponent_Manager->Add_Prototypes(PROTO_COMP_CUBEBUFFER, CreateNew<CCubeBuffer>());
	m_pComponent_Manager->Add_Prototypes(PROTO_COMP_FLOORBUFFER, CreateNew<CFloorBuffer>());
	m_pComponent_Manager->Add_Prototypes(PROTO_COMP_ABBCOLLIDER, CreateNew<CAABBCollider>());
	m_pComponent_Manager->Add_Prototypes(PROTO_COMP_OBBCOLLIDER, CreateNew<COBBCollider>());
	m_pComponent_Manager->Add_Prototypes(PROTO_COMP_RECTTRANSFORM, CreateNew<CRectTransform>());
	m_pComponent_Manager->Add_Prototypes(PROTO_COMP_CRISTALBUFFER, CreateNew<CCristalBuffer>());
	return S_OK;
}

void CGameInstance::Tick(const _double& _dTimeDelta)
{
	m_pInput_Manager->Tick(m_Hwnd);
	m_pSceneManager->Tick(_dTimeDelta);
	m_pActor_Manager->Tick(_dTimeDelta);
}

void CGameInstance::Late_Tick(const _double& _dTimeDelta)
{
	m_pSceneManager->Late_Tick(_dTimeDelta);
	m_pActor_Manager->Late_Tick(_dTimeDelta);
}

void CGameInstance::Render(const HDC& _hDC)
{
	m_pRenderer->Render(_hDC);
}

_double CGameInstance::Get_TimeDelta(const _wstring& _strTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(_strTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar* _pTimerTag)
{
	return m_pTimer_Manager->Add_Timer(_pTimerTag);
}

void CGameInstance::Change_CurTimer(const _wstring& _strTimerTag)
{
	return m_pTimer_Manager->Change_CurTimer(_strTimerTag);
}

shared_ptr<CTimer> CGameInstance::Find_Timer(const _wstring& _strTimerTag)
{
	return m_pTimer_Manager->Find_Timer(_strTimerTag);
}

HRESULT CGameInstance::RegisterMethod(const _ubyte& _KEYID, const function<_bool(const _double&)>& _pDelegate)
{
	return m_pInput_Manager->RegisterMethod(_KEYID, _pDelegate);
}

HRESULT CGameInstance::RunningKeyMethods(const _ubyte& _KEYID, const _double& _dTimeDelta)
{
	return m_pInput_Manager->RunningKeyMethods(_KEYID, _dTimeDelta);
}
/*
Input Manager
*/
bool CGameInstance::Is_Press( _int _iKey)
{
	return m_pInput_Manager->Is_Press(_iKey);
}
bool CGameInstance::Is_Pressing(const _int& _iKey)
{
	return m_pInput_Manager->Is_Pressing(_iKey);
}
bool CGameInstance::Is_MouseClick(const DMBUTTON& _eKey)
{
	return m_pInput_Manager->Is_MouseClick(_eKey);
}
const _float2& CGameInstance::GetMousePos() const
{
	return m_pInput_Manager->GetMousePos();
}
/*
Actor Manager
*/
void CGameInstance::Add_Prototypes(const _wstring& _strName, const shared_ptr<CActor>& _pActor)
{
	m_pActor_Manager->Add_Prototypes(_strName, _pActor);
}

void CGameInstance::Clone_Actor(const _wstring& _strProtoNames, const _wstring& _strCloneLayer, const VOIDDATAS& _tDatas)
{
   m_pActor_Manager->Clone_Actor(_strProtoNames, _strCloneLayer, _tDatas);
}

shared_ptr<CActor> CGameInstance::Clone_Actor_Add(const _wstring& _strProtoNames, const _wstring& _strCloneLayer, const VOIDDATAS& _tDatas)
{
	return m_pActor_Manager->Clone_Actor_Add(_strProtoNames, _strCloneLayer, _tDatas);
}
void CGameInstance::Add_DeleteActor(const _wstring& _strLayer, const shared_ptr<CActor>& _pActor)
{
	m_pActor_Manager->Add_DeleteActor(_strLayer, _pActor);
}
list<shared_ptr<CActor>> CGameInstance::Get_ActorList(const _wstring& _strCloneLayer)
{
	return m_pActor_Manager->Get_ActorList(_strCloneLayer);
}
void CGameInstance::Clear(){
	m_pActor_Manager->Clear();
	m_pPipeLine->ClearCamera();
}
/*
Component Manager
*/
void CGameInstance::Add_Prototypes(const _wstring& _strPrototypes, const shared_ptr<CComponent>& _pComponent)
{
	m_pComponent_Manager->Add_Prototypes(_strPrototypes, _pComponent);
}
shared_ptr<CComponent> CGameInstance::Clone(const _wstring& _strPrototypes, const VOIDDATAS& _tDatas)
{
	return m_pComponent_Manager->Clone(_strPrototypes, _tDatas);
}
/*
PipeLine
*/

void CGameInstance::Add_Camera(const _wstring& _strCamera, const shared_ptr<CCamera>& _pCamera){
	m_pPipeLine->Add_Camera(_strCamera, _pCamera);
}
void CGameInstance::ChangeCamera(const _wstring& _strCamera){
	m_pPipeLine->ChangeCamera(_strCamera);
}
void CGameInstance::BindCameraMatrix(){
	m_pPipeLine->BindCameraMatrix();
}
void CGameInstance::ClearCamera()
{
	m_pPipeLine->ClearCamera();
}
_float4x4 CGameInstance::CombineMatrix(const shared_ptr<class CActor>& _pActor)
{
	return m_pPipeLine->CombineMatrix(_pActor);
}
void CGameInstance::ChangeScreenToPos(const _float4x4& _WVP, _float3& _vPos)
{
	m_pPipeLine->ChangeScreenToPos(_WVP, _vPos);
}
void CGameInstance::ChangeScreenToPos(_float3& _vPos)
{
	m_pPipeLine->ChangeScreenToPos(_vPos);
}
void CGameInstance::ChangeScreenToPos(const shared_ptr<class CActor>& _pActor, _float3& _vPos)
{
	m_pPipeLine->ChangeScreenToPos(_pActor, _vPos);
}
void CGameInstance::Bind_Matrix4x4(const MATRIXTYPE& _eType, const _float4x4& _vMatrix)
{
	m_pPipeLine->Bind_Matrix4x4(_eType, _vMatrix);
}

void CGameInstance::Bind_Matrix(const MATRIXTYPE& _eType, const _matrix& _matWorld)
{
	m_pPipeLine->Bind_Matrix(_eType, _matWorld);
}

void CGameInstance::Set_CamPosition(const _float3& _vCamPos)
{
	m_pPipeLine->Set_CamPosition(_vCamPos);
}

_float4x4 CGameInstance::Get_Matrix4x4(const MATRIXTYPE& _eType)
{
	return m_pPipeLine->Get_Matrix4x4(_eType);
}

_matrix CGameInstance::Get_Matrix(const MATRIXTYPE& _eType)
{
	return m_pPipeLine->Get_Matrix(_eType);
}

const _float3& CGameInstance::Get_CamPos() const
{
	return m_pPipeLine->Get_CamPos();
}

void CGameInstance::RegisterScene(const shared_ptr<CScene>& _pScene){
	m_pSceneManager->RegisterScene(_pScene);
}

const shared_ptr<CScene>& CGameInstance::Get_CurScene() const
{
	return m_pSceneManager->Get_CurScene();
}
