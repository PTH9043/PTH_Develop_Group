#include "CCube.h"
#include "CGameInstance.h"
#include "CCubeBuffer.h"
#include "CMethod.h"
#include"CTransform.h"
#include "CRenderer.h"

shared_ptr<CActor> CCube::Clone(const VOIDDATAS& _tDatas)
{
    shared_ptr<CCube> pInstance = CSmartPointerFactory<CCube>::CreateShared();
    if (FAILED(pInstance->NativeConstruct(_tDatas))) {
        Safe_Free(pInstance);
        DEBUG_MSG("CCube::Failed To CloneActor");
    }
    return pInstance;
}

void CCube::Free()
{
}

HRESULT CCube::NativeConstruct()
{
    return S_OK;
}

HRESULT CCube::NativeConstruct(VOIDDATAS _tDatas)
{
    _float3 v3Color;
    if (_tDatas.size() > 0) {
        CUBEDESC tDesc = CMethod::ConvertTemplate_PopBack<CUBEDESC>(&_tDatas);
        v3Color = tDesc.vColor;
    }

    if (FAILED(__super::NativeConstruct(_tDatas))) {
        return E_FAIL;
    }

    if (nullptr != m_pCubeBuffer)
        m_pCubeBuffer->Set_Color(v3Color);
    return S_OK;
}

HRESULT CCube::CreateComponent()
{
    if (FAILED(__super::CreateComponent())) {
        return E_FAIL;
    }
    CComponent::COMPDESC tDesc;
    tDesc.pActor = ThisShared<CCube>();
    m_pCubeBuffer = CloneCompData<CCubeBuffer>(PROTO_COMP_CUBEBUFFER, VOIDDATAS{ &tDesc });
    m_pRenderer = CloneCompData<CRenderer>(PROTO_COMP_RENDER, VOIDDATAS{ &tDesc });
    return S_OK;
}

HRESULT CCube::Tick(const _double& _dTimeDelta)
{
    return S_OK;
}

HRESULT CCube::Late_Tick(const _double& _dTimeDelta)
{
    m_pRenderer->Add_Render(RENDER_ACTOR, ThisShared<CActor>());
    return S_OK;
}

HRESULT CCube::Render(const HDC& _hDC)
{
    m_pCubeBuffer->Render(_hDC);
    return S_OK;
}
