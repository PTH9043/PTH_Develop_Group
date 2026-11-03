#include "CCollider.h"
#include "CGameInstance.h"
#include "CActor.h"
#include "CTransform.h"
#include "CMethod.h"
#include "CCube.h"
#include "CRenderer.h"


CCollider::CCollider(const CCollider& _rhs) 
	: CComponent(_rhs), m_eColliderType(_rhs.m_eColliderType), 
	m_vPosition(_rhs.m_vPosition), m_vScale(_rhs.m_vScale)
{
}

shared_ptr<CComponent> CCollider::Clone(const VOIDDATAS& _tDatas)
{
	return shared_ptr<CComponent>();
}

void CCollider::Free()
{
#ifdef _DEBUG
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	pGameInstance->Add_DeleteActor(LAYER_DEBUG, m_pShowActor);
	Safe_Free(m_pShowActor);
#endif
}

HRESULT CCollider::NativeConstruct()
{
	return S_OK;
}

HRESULT CCollider::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}
	if (_tDatas.size() > 0) {
		COLLDESC tCollDesc = CMethod::ConvertTemplate_PopBack<COLLDESC>(&_tDatas);
		m_vPosition = tCollDesc.vPosition;
		m_vScale = tCollDesc.vScale;
	}
#ifdef _DEBUG
	CreateDebugActor();
#endif
	return S_OK;
}

void CCollider::Tick(const _double& _dTimeDelta) {

}

#ifdef _DEBUG
void CCollider::Add_RendererDebugActor(const shared_ptr<CRenderer>& _pRenderer){
	RETURN_CHECK(nullptr == _pRenderer, ;);
	RETURN_CHECK(nullptr == m_pShowActor, ;);
	_pRenderer->Add_Render(RENDERID::RENDER_DEBUG, m_pShowActor);
}
void CCollider::CreateDebugActor()
{
	switch (m_eColliderType) {
	case COLLIDERTYPE::COLL_AABB:
	case COLLIDERTYPE::COLL_OBB:
	case COLLIDERTYPE::COLL_SPHERE:
	{
		CCube::CUBEDESC tDesc = CCube::CUBEDESC{ _float3(255.f, 255.f, 0.f) };
		VOIDDATAS Datas{ &tDesc };
		CreateActor(PROTO_ACTOR_CUBE, LAYER_DEBUG, Datas);
	}
	break;
	default:
		break;
	}

	if (nullptr != m_pShowActor) {
		m_pShowActor->Get_Transform()->Set_PosF3(m_vPosition);
		m_pShowActor->Get_Transform()->Set_Scale(m_vScale);
	}
}

void CCollider::CreateActor(const _wstring& _strProtoTag, const _wstring& _pCloneTag, 
	const VOIDDATAS& _tDatas)
{
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	m_pShowActor = pGameInstance->Clone_Actor_Add(_strProtoTag, _pCloneTag, _tDatas);
}
#endif