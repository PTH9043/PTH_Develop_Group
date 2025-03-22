#include "CMainView.h"
#include "CNavigationTool.h"
#include "CObjectTool.h"
#include "CImGui_Manager.h"
#include "CWOEffect.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CEffectParticleGroup.h"
#include "CLight.h"

CWOEffect::CWOEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject_Client(pDevice, pContext)
{
}

CWOEffect::CWOEffect(const CWOEffect& _rhs)
    : CGameObject_Client(_rhs)
{
}

const char* CWOEffect::Get_Name()
{
    return "WOEffect";
}

HRESULT CWOEffect::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWOEffect::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        WODEFFECTDESC tData = *static_cast<WODEFFECTDESC*>(pArg);
        m_tGobDesc = tData.tGobClientDesc;
        m_iSelectIndex = tData.iProtoSelectIndex;

        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            switch (m_iSelectIndex)
            {
            case 0:
                m_pEffectParticleGroup = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                    LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_WOG_BLOODHEART_1));

                ZeroMemory(&m_tLightDesc, sizeof(LIGHTDESC));

                m_tLightDesc.eLightType = tagLightDesc::TYPE_POINT;
                m_tLightDesc.v3Position = m_tGobDesc.v3Pos;
                m_tLightDesc.fRange = ((m_tGobDesc.v3Scale.x + m_tGobDesc.v3Scale.y + m_tGobDesc.v3Scale.z) / 3.f) * 2.f;
                m_tLightDesc.v4Direction = _float4(0.f, -1.f, 0.f, 0.f);
                m_tLightDesc.v4Diffuse = _float4(1.f, 0.2f, 0.2f, 1.f);
                m_tLightDesc.v4Ambient = _float4(0.3f, 0.3f, 0.3f, 1.f);
                m_tLightDesc.v4Specular = _float4(0.0f, 0.0f, 0.0f, 0.0f);

                pGameInstance->Add_Light(m_pDevice, m_pContext, m_tLightDesc, &m_pLight);
                break;
            case 1:
                m_pEffectParticleGroup = static_cast<CEffectParticleGroup*>(pGameInstance->Clone_GameObject_Add(g_iCurrentLevel,
                    LAYER_EFFECTPARTICLE, PROTO_GOB_EFFPARTICLE_WOG_BLOODHEART_2));

                ZeroMemory(&m_tLightDesc, sizeof(LIGHTDESC));

                m_tLightDesc.eLightType = tagLightDesc::TYPE_POINT;
                m_tLightDesc.v3Position = m_tGobDesc.v3Pos;
                m_tLightDesc.fRange = ((m_tGobDesc.v3Scale.x + m_tGobDesc.v3Scale.y + m_tGobDesc.v3Scale.z) / 3.f) * 3.5f;
                m_tLightDesc.v4Direction = _float4(0.f, -1.f, 0.f, 0.f);
                m_tLightDesc.v4Diffuse = _float4(1.f, 0.0f, 0.0f, 1.f);
                m_tLightDesc.v4Ambient = _float4(0.4f, 0.4f, 0.4f, 1.f);
                m_tLightDesc.v4Specular = _float4(0.0f, 0.0f, 0.0f, 0.0f);

                pGameInstance->Add_Light(m_pDevice, m_pContext, m_tLightDesc, &m_pLight);
                break;
            }

            Safe_AddRef(m_pEffectParticleGroup);
        }
        END_SINGLETON(pGameInstance);
    }

    if (FAILED(Ready_Transform(COM_TRANSFORM)))
        return E_FAIL;
    
    _vector vPos = XMLoadFloat3(&m_tGobDesc.v3Pos);
    m_pTransformCom->Set_Pos(vPos);
    m_pTransformCom->Set_Scale(m_tGobDesc.v3Scale);

    if (nullptr != m_pEffectParticleGroup)
        m_pEffectParticleGroup->Set_OwnerTransform(m_pTransformCom);
    return S_OK;
}

void CWOEffect::Tick(const _double& _dTimeDelta)
{
}

void CWOEffect::Late_Tick(const _double& _dTimeDelta)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        if (false == pGameInstance->Is_Rendering(m_pTransformCom->Get_Pos(), m_tLightDesc.fRange))
        {
            m_pLight->Set_NotCaluclation(true);
            if (nullptr != m_pEffectParticleGroup)
                m_pEffectParticleGroup->Active(false);
        }
        else
        {
            m_pLight->Set_NotCaluclation(false);

            if (true == pGameInstance->Is_Rendering(m_pTransformCom->Get_Pos(), 1.f))
            {
                if (nullptr != m_pEffectParticleGroup)
                    m_pEffectParticleGroup->Active(true);
            }
            else
            {
                if (nullptr != m_pEffectParticleGroup)
                    m_pEffectParticleGroup->Active(false);
            }
        }
    }
    END_SINGLETON(pGameInstance);
}

HRESULT CWOEffect::Render()
{
    return E_NOTIMPL;
}

HRESULT CWOEffect::Ready_Component()
{
    return S_OK;
}

HRESULT CWOEffect::SetUp_ConstantTable()
{
    return E_NOTIMPL;
}
#pragma region USE_TOOL_METHOD
_bool CWOEffect::Extern_EditValue()
{
    _bool bActive = false;
    if (ImGui::TreeNode("WOEffect"))
    {
        ImGui::PushItemWidth(200);

        Extern_GobData();
        const char* pData[] = { "GOEffect_BloodHeart_1", "GOEffect_Blood_Heart_2"};
        ImGui::ListBox("GOEffectSelect", (_int*)&m_iSelectIndex, pData, 2);

        bActive = ImGui::Button("Create");
        ImGui::PopItemWidth();
        ImGui::TreePop();
        return bActive;
    }
    return false;
}

_bool CWOEffect::Extern_HierakeyView(const string& _str)
{
    if (nullptr == m_pTransformCom)
        return false;

    _bool bActive = false;
    if (ImGui::TreeNode(m_strShowName.c_str()))
    {
        ImGui::PushItemWidth(200);

        Extern_GobData();

        {
            if (nullptr != m_pLight)
            {
                if (0 == m_iSelectIndex)
                {
                    LIGHTDESC			LightDesc = *m_pLight->Get_LightDesc();
                    LightDesc.v3Position = m_tGobDesc.v3Pos;
                    LightDesc.fRange = ((m_tGobDesc.v3Scale.x + m_tGobDesc.v3Scale.y + m_tGobDesc.v3Scale.z) / 3.f) * 3.5f;
                    m_pLight->Set_LightDesc(LightDesc);
                }
                else if (1 == m_iSelectIndex)
                {
                    LIGHTDESC			LightDesc = *m_pLight->Get_LightDesc();
                    LightDesc.v3Position = m_tGobDesc.v3Pos;
                    LightDesc.fRange = ((m_tGobDesc.v3Scale.x + m_tGobDesc.v3Scale.y + m_tGobDesc.v3Scale.z) / 3.f) * 3.5f;
                    m_pLight->Set_LightDesc(LightDesc);
                }
            }
        }
        _bool bClone = ImGui::Button("Clone");
        if (true == bClone)
        {
            BEGIN_SINGLETON(CGameInstance, pGameInstance);
            {
                WODEFFECTDESC tDesc;
                tDesc.iProtoSelectIndex = m_iSelectIndex;
                tDesc.tGobClientDesc = m_tGobDesc;
                tDesc.strPrototype = Get_Proto();
                pGameInstance->Clone_GameObject(LEVEL_TOOL, Get_Layer()
                    , Get_Proto(), &tDesc);
            }
            END_SINGLETON(pGameInstance);
        }

        bActive = ImGui::Button("Delete");
        ImGui::PopItemWidth();
        ImGui::TreePop();
        return bActive;
    }
    return false;
}

HRESULT CWOEffect::Create_Self(CGameObject** _ppOut)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        WODEFFECTDESC tDesc;
        tDesc.iProtoSelectIndex = m_iSelectIndex;
        tDesc.tGobClientDesc = m_tGobDesc;
        tDesc.strPrototype = Get_Proto();
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
            , Get_Proto(), &tDesc));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CWOEffect::Delete_Self()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        pGameInstance->Add_DeleteObject(LEVEL_TOOL, Get_Layer(), this);
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CWOEffect::Save_Data()
{
    BEGIN_SINGLETON(CData_Manager, pDataManager);
    {
        WODEFFECTDESC tDesc;
        tDesc.iProtoSelectIndex = m_iSelectIndex;
        tDesc.tGobClientDesc = m_tGobDesc;
        tDesc.strPrototype = Get_Proto();
        // Save 
        pDataManager->Get_WorldMgr()->Insert_WOEffectList(tDesc);
    }
    END_SINGLETON(pDataManager);
    return S_OK;
}

HRESULT CWOEffect::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
            , Get_Proto(), _pData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

_bool CWOEffect::Is_Picking(_float3* _pOut)
{
    return false;
}

_bool CWOEffect::Is_PickingNav()
{
    return false;
}

void CWOEffect::Reset_Data()
{

}
#pragma endregion USE_TOOL_METHOD
CWOEffect* CWOEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWOEffect* pInstance = new CWOEffect(pDevice, pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CWOEffect::Create To Failed");
    }
    return pInstance;
}

CWOEffect* CWOEffect::Clone(void* _pArg)
{
    CWOEffect* pInstance = new CWOEffect(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CWOEffect::Create To Failed");
    }
    return pInstance;
}

void CWOEffect::Free()
{
    if (g_iCurrentLevel == LEVEL_TOOL)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstnace)
        {
            pGameInstnace->Add_DeleteObject(LEVEL_TOOL, LAYER_EFFECTPARTICLE, m_pEffectParticleGroup);
            if(nullptr != m_pLight)
                pGameInstnace->Delete_Light(m_pLight->Get_Index());
        }  
        END_SINGLETON(pGameInstnace);
    }
    Safe_Release(m_pEffectParticleGroup);

    __super::Free();
}
