#include "CEmptyObject.h"
#ifdef _DEBUG
#include "CGameInstance.h"
#include "CCubeBuffer.h"
#include "CMethod.h"
#include"CTransform.h"
#include "CRenderer.h"
#endif

shared_ptr<CActor> CEmptyObject::Clone(const VOIDDATAS& _tDatas)
{
    shared_ptr<CEmptyObject> pActor = CSmartPointerFactory<CEmptyObject>::CreateShared(*this);
    if (FAILED(pActor->NativeConstruct(_tDatas))) {
        Safe_Free(pActor);
        DEBUG_MSG("CEmptyObject::Clone To Failed");
    }
    return pActor;
}

void CEmptyObject::Free()
{
}

HRESULT CEmptyObject::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;
    return S_OK;
}

HRESULT CEmptyObject::NativeConstruct(VOIDDATAS _tDatas)
{
    if (FAILED(__super::NativeConstruct(_tDatas)))
        return E_FAIL;

#ifdef _DEBUG
    if (nullptr != m_pCubeBuffer)
        m_pCubeBuffer->Set_Color(_float3(150.f, 100.f, 50.f));
#endif
    Set_Active(true);
    return S_OK;
}

HRESULT CEmptyObject::CreateComponent()
{
    if (FAILED(__super::CreateComponent())) {
        return E_FAIL;
    }
#ifdef _DEBUG
    CComponent::COMPDESC tDesc;
    tDesc.pActor = ThisShared<CEmptyObject>();
    m_pCubeBuffer = CloneCompData<CCubeBuffer>(PROTO_COMP_CUBEBUFFER, VOIDDATAS{ &tDesc });
    m_pRenderer = CloneCompData<CRenderer>(PROTO_COMP_RENDER, VOIDDATAS{ &tDesc });
#endif
    return S_OK;
}

HRESULT CEmptyObject::Tick(const _double& _dTimeDelta)
{
    return __super::Tick(_dTimeDelta);
}

HRESULT CEmptyObject::Late_Tick(const _double& _dTimeDelta)
{
#ifdef _DEBUG
    m_pRenderer->Add_Render(RENDER_ACTOR, ThisShared<CActor>());
#endif
    return S_OK;
}

HRESULT CEmptyObject::Render(const HDC& _hDC)
{
#ifdef _DEBUG
    m_pCubeBuffer->Render(_hDC);
#endif
    return S_OK;
}
