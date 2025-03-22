#include "CObjectTool.h"
#include "CMainView.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"
#include "CGameObject_Client.h"
#include "CDebugModel.h"
#include "CData_Manager.h"

CObjectTool::CObjectTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CIMGUI(_pDevice, _pContext)
{

}

HRESULT CObjectTool::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;
    // Name
    m_pName = "ObjectTool";
    return S_OK;
}

HRESULT CObjectTool::NativeConstruct_Tool()
{
    if (FAILED(__super::NativeConstruct_Tool()))
        return E_FAIL;

    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        m_pDebugObject = pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, LAYER_DEBUG, PROTO_GOB_DEBUGMODEL);
        Safe_AddRef(m_pDebugObject);

#ifdef _DEBUG
        m_pRenderer = static_cast<CRenderer*>(pGameInstance->Clone_Component(
            LEVEL_STATIC, PROTO_COMP_RENDERER));
#endif
    }
    END_SINGLETON(pGameInstance);

    if (nullptr == m_pDebugObject)
        return E_FAIL;

    return S_OK;
}

HRESULT CObjectTool::Tick(const _double& _dDeltaSeconds)
{
    if (FAILED(__super::Tick(_dDeltaSeconds)))
        return E_FAIL;

#ifdef _DEBUG
    BEGIN_SINGLETON(CData_Manager, pDataManager)
    {
        pDataManager->Get_VariableMgr()->Render_TestColliderOffsetMatrix(m_pRenderer);
    }
    END_SINGLETON(pDataManager);
#endif

    return S_OK;
}

HRESULT CObjectTool::Render()
{
    if (false == m_bOpen)
        return E_FAIL;

    if (FAILED(__super::Render()))
        return E_FAIL;

    Show_Window();
    return S_OK;
}

HRESULT CObjectTool::Release_Tool()
{
    Safe_Release(m_pDebugObject);
    Safe_Release(m_pSpawnObject);

#ifdef _DEBUG
    Safe_Release(m_pRenderer);
#endif
    return S_OK;
}

void CObjectTool::Set_DebugObject_Model(CModel* _pModel)
{
    if (nullptr == m_pDebugObject)
        return;
    // Set Model
    static_cast<CDebugModel*>(m_pDebugObject)->Set_Model(_pModel);
}

void CObjectTool::Show_Window()
{
    ImVec2 InitPos = ImVec2{ static_cast<_float>(g_iWinSizeX + 50),  50.f };
    ImGui::SetNextWindowPos(InitPos, ImGuiCond_Appearing);
    ImGui::Begin(m_pName, &m_bOpen);
    {
#ifdef _DEBUG
        ImGui::Checkbox("WorldObjectCall_Draw", &m_bWorldObjectDrawCall);
        ImGui::Checkbox("CharObjectCall_Draw", &m_bCharObjectDrawCall);
        BEGIN_SINGLETON(CData_Manager, pDataManager)
        {
            pDataManager->Get_VariableMgr()->Control_Collider();
        }
        END_SINGLETON(pDataManager);
#endif
        {
            BEGIN_SINGLETON(CImGui_Manager, pImGuiManager);
            {
                Spawn_Player_Pos();

                for (auto& iter : pImGuiManager->Get_ProtoObjMap())
                {
                    _bool bActive = iter.second->Extern_EditValue();
                    if (true == bActive)
                    {
                        CGameObject* pGameObject = nullptr;
                        iter.second->Create_Self(&pGameObject);
                        Reset_DebugObject_State();
                    }
                }
            }
            END_SINGLETON(pImGuiManager);
        }
    }
    ImGui::End();
}

void CObjectTool::Spawn_Player_Pos()
{
    if (ImGui::CollapsingHeader("Player_Pos"))
    {
        BEGIN_SINGLETON(CData_Manager, pDataManager);
        {
            _float3 v3SpawnPos = pDataManager->Get_PlayerMgr()->Get_PlayerSpawnPos();
            _float Position[3] = { v3SpawnPos.x,  v3SpawnPos.y, v3SpawnPos.z };
            ImGui::InputFloat3("Position", Position, "%.1f");
            pDataManager->Get_PlayerMgr()->Reset_Player(v3SpawnPos);
        }
        END_SINGLETON(pDataManager);
    }
}

void CObjectTool::Reset_DebugObject_State()
{
    if (nullptr == m_pDebugObject)
        return;

    CTransform* _pTransform = static_cast<CTransform*>(m_pDebugObject->Get_Component(COM_TRANSFORM));
    if (nullptr != _pTransform)
    {
        _pTransform->Set_Pos(XMLoadFloat3(&_float3(0.f, 0.f, 0.f)));
        _pTransform->Set_Scale(_float3(1.f, 1.f, 1.f));
        _pTransform->Rotate_Fix(XMLoadFloat3(&_float3(1.f, 0.f, 0.f)), 0.f);
        _pTransform->Rotate_Fix(XMLoadFloat3(&_float3(0.f, 1.f, 0.f)), 0.f);
        _pTransform->Rotate_Fix(XMLoadFloat3(&_float3(0.f, 0.f, 1.f)), 0.f);
      //  static_cast<CDebugModel*>(m_pDebugObject)->Set_Model(nullptr);
    }
}

CObjectTool* CObjectTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CObjectTool* pInstance = new CObjectTool(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CObjectTool::Create to Failed");
    }
    return pInstance;
}

void CObjectTool::Free()
{
     Safe_Release(m_pDebugObject);
     Safe_Release(m_pSpawnObject);

#ifdef _DEBUG
     Safe_Release(m_pRenderer);
#endif
    __super::Free();
}
