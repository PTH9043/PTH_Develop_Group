#include "COBBCollider.h"
#include "CActor.h"
#include "CTransform.h"
#include "CAABBCollider.h"

COBBCollider::COBBCollider(const COBBCollider& _rhs) 
    : CCollider(_rhs), m_pOriginBox(_rhs.m_pOriginBox)
{
 
}

shared_ptr<CComponent> COBBCollider::Clone(const VOIDDATAS& _tDatas)
{
    shared_ptr<COBBCollider> pInstance = CSmartPointerFactory<COBBCollider>::CreateShared(*this);
    if (FAILED(pInstance->NativeConstruct(_tDatas)))
    {
        Safe_Free(pInstance);
        DEBUG_MSG("COBBCollider::Create To Failed");
    }
    return pInstance;
}

void COBBCollider::Free()
{
    __super::Free();
}

HRESULT COBBCollider::NativeConstruct()
{
    m_pOriginBox = make_shared<DirectX::BoundingOrientedBox>();
    m_pOriginBox->Center = _float3(0.f, 0.f, 0.f);
    m_pOriginBox->Extents = _float3(0.5f, 0.5f, 0.5f);
    return S_OK;
}

HRESULT COBBCollider::NativeConstruct(VOIDDATAS _tDatas)
{
    RETURN_CHECK_FAILED(__super::NativeConstruct(_tDatas), E_FAIL);
    if (nullptr != m_pOriginBox) {
        m_pOriginBox->Center = _float3(0.f, 0.f, 0.f);
   //     m_pOriginBox->Extents = Get_Scale();
    }

    m_pCollisionBox = make_shared<DirectX::BoundingOrientedBox>(*m_pOriginBox);
    return S_OK;
}

void COBBCollider::Tick(const _double& _dTimeDelta){

    __super::Tick(_dTimeDelta);
    shared_ptr<CActor> pActor = Get_Actor().lock();
    if (nullptr != pActor) {
        m_pOriginBox->Transform(*m_pCollisionBox, pActor->Get_Transform()->Get_ParentsToWorldMatrix());
        m_pCollisionBox->Extents = Get_Scale();
#ifdef _DEBUG
        if (nullptr != Get_ShowActor())
        {
            Get_ShowActor()->Get_Transform()->Bind_WorldMatrix(pActor->Get_Transform()->Get_ParentsToWorldMatrix());
            Get_ShowActor()->Get_Transform()->Set_Scale(Get_Scale());
        }
#endif
    }
}

_bool COBBCollider::IsCollider(const shared_ptr<CCollider>& _pCollider)
{
    if (_pCollider->Get_CollderType() == ColliderType::COLL_OBB)
    {
        COBBCollider* pCollider = static_cast<COBBCollider*>(_pCollider.get());
        return m_pCollisionBox->Intersects(*pCollider->m_pCollisionBox);
    } 
    else    if (_pCollider->Get_CollderType() == ColliderType::COLL_AABB)
    {
        CAABBCollider* pCollider = static_cast<CAABBCollider*>(_pCollider.get());
        return m_pCollisionBox->Intersects(*pCollider->Get_CollisionBox());
    }
    return false;
}
