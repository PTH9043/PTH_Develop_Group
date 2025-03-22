#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CVariable_Manager.h"
#include "CGameInstance.h"

CVariable_Manager::CVariable_Manager()
{
#ifdef _DEBUG
    XMStoreFloat4x4(&m_fSaveOffsetMatrix, XMMatrixIdentity());
#endif
}
#ifdef _DEBUG
void CVariable_Manager::Initialize_Debug()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        CCollider::COLLIDERDESC tCollider;
        tCollider.vScale = _float3(1.f, 1.f, 1.f);
        tCollider.vTranslation = _float3(0.f, 0.f, 0.f);
        m_pTestOBBCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, 
            &tCollider));

        m_pTestCircleCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
            &tCollider));
    }
    END_SINGLETON(pGameInstance);
}

void CVariable_Manager::Control_Collider()
{
    ImGui::Checkbox("FindOffset", &m_bFindOffsetMatrix);
    _uint iType = 0;

    const char* ColliderType[2] = { "OBB", "ABB" };
    ImGui::Combo("ColliderType", (_int*)&m_iColliderType, ColliderType, 2);

    if (0 == m_iColliderType)
    {
        if (ImGui::TreeNodeEx("TestCollider"))
        {
            if (nullptr != m_pTestOBBCollider)
            {
                CCollider::COLLIDERDESC tCollData;
                tCollData.vScale = m_pTestOBBCollider->Get_Scale();
                tCollData.vTranslation = m_pTestOBBCollider->Get_Translate();

                _float Translate[3] = { tCollData.vTranslation.x,  tCollData.vTranslation.y, tCollData.vTranslation.z };
                _float CollSclae[3] = { tCollData.vScale.x ,  tCollData.vScale.y , tCollData.vScale.z };

                if (ImGui::InputFloat3("TSHTrans", Translate, "%.2f"))
                {
                    m_pTestOBBCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
                }
                if (ImGui::InputFloat3("TSHScale", CollSclae, "%.2f"))
                {
                    m_pTestOBBCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
                }
            }
            ImGui::TreePop();
        }
    }
    else if (1 == m_iColliderType)
    {
        if (ImGui::TreeNodeEx("TestCollider"))
        {
            if (nullptr != m_pTestCircleCollider)
            {
                CCollider::COLLIDERDESC tCollData;
                tCollData.vScale = m_pTestCircleCollider->Get_Scale();
                tCollData.vTranslation = m_pTestCircleCollider->Get_Translate();

                _float Translate[3] = { tCollData.vTranslation.x,  tCollData.vTranslation.y, tCollData.vTranslation.z };
                _float CollSclae[3] = { tCollData.vScale.x ,  tCollData.vScale.y , tCollData.vScale.z };

                if (ImGui::InputFloat3("TSHTrans", Translate, "%.2f"))
                {
                    m_pTestCircleCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
                }
                if (ImGui::InputFloat3("TSHScale", CollSclae, "%.2f"))
                {
                    m_pTestCircleCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
                }
            }
            ImGui::TreePop();
        }
    }
}

void CVariable_Manager::Render_TestColliderOffsetMatrix(CRenderer* _pRenderer)
{
    if (nullptr == _pRenderer)
        return;

    if(true == m_bFindOffsetMatrix)
    {
        if (0 == m_iColliderType)
        {
            if (nullptr == m_pTestOBBCollider)
                return;
            m_pTestOBBCollider->Tick(XMLoadFloat4x4(&m_fSaveOffsetMatrix));
            _pRenderer->Add_Components(m_pTestOBBCollider);
        }
        else if (1 == m_iColliderType)
        {
            if (nullptr == m_pTestCircleCollider)
                return; 
            m_pTestCircleCollider->Tick(XMLoadFloat4x4(&m_fSaveOffsetMatrix));
            _pRenderer->Add_Components(m_pTestCircleCollider);
        }
    }
}
#endif
HRESULT CVariable_Manager::NativeConstruct(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return S_OK;
}

CVariable_Manager* CVariable_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CVariable_Manager* pInstance = new CVariable_Manager();
    if (FAILED(pInstance->NativeConstruct(_pDevice, _pContext)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CVariable_Manager::Create To Failed");
    }
    return pInstance;
}

void CVariable_Manager::Free()
{
#ifdef _DEBUG
    Safe_Release(m_pTestOBBCollider);
    Safe_Release(m_pTestCircleCollider);
#endif
    __super::Free();
}
