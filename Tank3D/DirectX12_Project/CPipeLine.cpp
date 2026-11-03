#include "CPipeLine.h"
#include "CActor.h"
#include "CTransform.h"
#include "CGameInstance.h"
#include "CCamera.h"

void CPipeLine::Free()
{
}

void CPipeLine::Add_Camera(const _wstring& _strCamera, const shared_ptr<CCamera>& _pCamera){
	if (nullptr == _pCamera)
		return;

	m_CamMaps.emplace(_strCamera, _pCamera);
}

void CPipeLine::ChangeCamera(const _wstring& _strCamera){

	auto iter = m_CamMaps.find(_strCamera);
	if (m_CamMaps.end() == iter)
		return;

	if (nullptr != m_pSelectCamera.lock()) {
		m_pSelectCamera.lock()->Set_Active(false);
	}
	m_pSelectCamera = (*iter).second;
	m_pSelectCamera.lock()->Set_Active(true);
}

void CPipeLine::BindCameraMatrix(){
	if (nullptr != m_pSelectCamera.lock()) {
		m_pSelectCamera.lock()->BindMatrix(this);
	}
}

_float4x4 CPipeLine::CombineMatrix(const shared_ptr<class CActor>& _pActor)
{
	if (nullptr == _pActor)
		return _float4x4();

	_float4x4 vValue;
	XMStoreFloat4x4(&vValue, _pActor->Get_Transform()->Get_ParentsToWorldMatrix() *
		Get_Matrix(MAT_VIEW) * Get_Matrix(MAT_PROJ));

	return vValue;
}

void CPipeLine::ChangeScreenToPos(_float3& _vPos)
{
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	_float3 f3Screen = _vPos;

	float fHalfWidth = WINDOW_WIDTH * 0.5f;
	float fHalfHeight = WINDOW_HEIGHT * 0.5f;
	f3Screen.x = pGameInstance->Get_WindowRect().left + (_vPos.x * fHalfWidth) + fHalfWidth;
	f3Screen.y = pGameInstance->Get_WindowRect().top + (-_vPos.y * fHalfHeight) + fHalfHeight;
	_vPos = f3Screen;
}

void CPipeLine::ChangeScreenToPos(const shared_ptr<class CActor>& _pActor, _float3& _vPos)
{
	if (nullptr == _pActor)
		return;

	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);

	_vPos = _vPos.Coord4x4(_pActor->Get_Transform()->Get_WorldFloat4x4());
	_vPos = _vPos.Coord4x4(Get_Matrix4x4(MAT_VIEW));
	_vPos = _vPos.Coord4x4(Get_Matrix4x4(MAT_PROJ));

	_float3 f3Screen = _vPos;

	float fHalfWidth = WINDOW_WIDTH * 0.5f;
	float fHalfHeight = WINDOW_HEIGHT * 0.5f;
	f3Screen.x = pGameInstance->Get_WindowRect().left + (_vPos.x * fHalfWidth) + fHalfWidth;
	f3Screen.y = pGameInstance->Get_WindowRect().top + (-_vPos.y * fHalfHeight) + fHalfHeight;
}

void CPipeLine::ChangeScreenToPos(const _float4x4& _WVP, _float3& _vPos)
{
	_vPos = _vPos.Coord4x4(_WVP);
	
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	_float3 f3Screen = _vPos;

	float fHalfWidth = WINDOW_WIDTH * 0.5f;
	float fHalfHeight = WINDOW_HEIGHT * 0.5f;
	f3Screen.x = pGameInstance->Get_WindowRect().left + (_vPos.x * fHalfWidth) + fHalfWidth;
	f3Screen.y = pGameInstance->Get_WindowRect().top + (-_vPos.y * fHalfHeight) + fHalfHeight;

	_vPos = f3Screen;
}

void CPipeLine::ClearCamera()
{
	m_pSelectCamera = weak_ptr<CCamera>();
	m_CamMaps.clear();
}

void CPipeLine::Bind_Matrix4x4(const MATRIXTYPE& _eType, const _float4x4& _vMatrix)
{
	XMStoreFloat4x4(&m_Matrixes[_eType], XMLoadFloat4x4(&_vMatrix));
}

void CPipeLine::Bind_Matrix(const MATRIXTYPE& _eType, const _matrix& _matWorld)
{
	XMStoreFloat4x4(&m_Matrixes[_eType], _matWorld);
}