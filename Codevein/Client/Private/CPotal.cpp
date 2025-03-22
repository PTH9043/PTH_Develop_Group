#include "CMainView.h"
#include"CImGui_Manager.h"
#include "CPotal.h"
#include "CGameInstance.h"
#include"CData_Manager.h"
#include "CEffectParticleGroup.h"

CPotal::CPotal(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CWorldObj(_pDevice, _pContext)
{
}

CPotal::CPotal(const CPotal& _rhs) 
    : CWorldObj(_rhs)
{
}

const char* CPotal::Get_Name()
{
    return m_strModelName.c_str();
}

HRESULT CPotal::NativeConstruct_Prototype()
{
    return S_OK;
}

HRESULT CPotal::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (nullptr != m_pCollCollider)
    {
        m_pCollCollider->Set_Scale(_float3(8.f, 0.f, 0.f));
    }
    return S_OK;
}

void CPotal::Tick(const _double& _dTimeDelta)
{
    if (false == m_bActive)
        return;

    if(nullptr != m_pPotal)
        m_pPotal->Active(true);

    __super::Tick(_dTimeDelta);
    Find_Player();
    Check_CollisionToPlayer();
}

void CPotal::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);
}

HRESULT CPotal::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
    return S_OK;
}

HRESULT CPotal::Ready_Component()
{
    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, 
        PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, 
        PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    m_strModelName = "Potal";
    {
        if (FAILED(Ready_Transform(COM_TRANSFORM)))
            return E_FAIL;
    }
    {
        CCollider::COLLIDERDESC tDesc;
        tDesc.vScale = m_tColliderDesc.v3Scale;
        tDesc.vTranslation = m_tColliderDesc.v3Translate;
        /* For.Com_Shader */
        if (FAILED(__super::Add_Component(COM_COLLCOLLIDER, LEVEL_STATIC,
            PROTO_COMP_SPHERECOLLIDER, (CComponent**)&m_pCollCollider, &tDesc)))
            return E_FAIL;
    }
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            CEffectParticleGroup::EFFECTPARTICLEGROUPDESC tDesc;
            tDesc.pTransform = m_pTransformCom;
            m_pPotal = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_POTAL, &tDesc));
            Safe_AddRef(m_pPotal);
            m_pPotal->Active(false);

        }
        END_SINGLETON(pGameInstance);
    }

    return S_OK;
}

void CPotal::Find_Player()
{
    if (nullptr == m_pPlayer)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel,
                LAYER_PLAYER, 0));
        }
        END_SINGLETON(pGameInstance);
    }
}

void CPotal::Check_CollisionToPlayer()
{
    if (nullptr != m_pPlayer)
    {
        CCollider* pRealCollider = static_cast<CCollider*>(m_pPlayer->Get_Component(COM_FIRSTCHECKCOLLIDER));
        if (nullptr == pRealCollider)
            return;
        m_pCollCollider->Tick(m_pTransformCom->Get_WorldMatrix());
        if (true == pRealCollider->Is_Collision(m_pCollCollider))
        {
            BEGIN_SINGLETON(CGameInstance, pGameInstance)
            {
                m_pPlayer->Find_Potal(this);
            }
            END_SINGLETON(pGameInstance);
        }
    }
}
#pragma region USE_TOOL_METHOD
_bool CPotal::Extern_EditValue()
{
    return _bool();
}

_bool CPotal::Extern_HierakeyView(const string& _str)
{
    return _bool();
}

HRESULT CPotal::Create_Self(CGameObject** _ppOut)
{
    return E_NOTIMPL;
}

HRESULT CPotal::Delete_Self()
{
    return E_NOTIMPL;
}

HRESULT CPotal::Save_Data()
{
    return E_NOTIMPL;
}

HRESULT CPotal::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    return E_NOTIMPL;
}

_bool CPotal::Is_Picking(_float3* _pOut)
{
    return _bool();
}

_bool CPotal::Is_PickingNav()
{
    return _bool();
}

void CPotal::Reset_Data()
{
}
#pragma endregion USE_TOOL_METHOD
CPotal* CPotal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPotal* pInstance = new CPotal(pDevice, pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CPotal::Create To Failed");
    }
     return pInstance;
}

CGameObject* CPotal::Clone(void* _pArg)
{
    CPotal* pInstance = new CPotal(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CPotal::Clone To Failed");
    }
    return pInstance;
}

void CPotal::Free()
{
    Safe_Release(m_pPotal);
    __super::Free();
}
