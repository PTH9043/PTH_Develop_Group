#include "CMainView.h"
#include "CNavigationTool.h"
#include "CObjectTool.h"
#include "CImGui_Manager.h"
#include "CWorldObj.h"
#include "CGameInstance.h"
#include "CData_Manager.h"
#include "CDebugModel.h"

CWorldObj::CWorldObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject_Client(pDevice, pContext)
{
}

CWorldObj::CWorldObj(const CWorldObj& _rhs)
    : CGameObject_Client(_rhs)
{
}

const char* CWorldObj::Get_Name()
{
    return m_strModelName.c_str();
}

HRESULT CWorldObj::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWorldObj::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        WORLDSAVES tData = *static_cast<WORLDSAVES*>(pArg);
        m_tGobDesc = tData.tGobClientDesc;
        m_strModelName = tData.strModelName;
        m_tColliderDesc = tData.tColliderDesc;
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    Reset_TransformData();

    if (nullptr != m_pCollCollider)
        m_pCollCollider->Tick(m_pTransformCom->Get_WorldMatrix());

    m_bActive = true;
    return S_OK;
}

void CWorldObj::Tick(const _double& _dTimeDelta)
{
    __super::Tick(_dTimeDelta);
}

void CWorldObj::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (false == m_bActive)
        return;

    if (LEVEL_TOOL == g_iCurrentLevel)
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            if (true == pGameInstance->Is_Rendering(m_pCollCollider->Get_CurPos(), m_pCollCollider->Get_CurScaleV3().x / 2.f))
            {
                if (nullptr != m_pRendererCom)
                {
#ifdef _DEBUG
                    BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
                    {
                        CObjectTool* pObject = static_cast<CObjectTool*>(pImGuiManager->Get_MainView()->Get_ImGuiClass(CMainView::GUI_OBJ));

                        if (true == pObject->Is_WorldObjectDrawCall())
                            m_pRendererCom->Add_Components(m_pCollCollider);
                    }
                    END_SINGLETON(pImGuiManager)
#endif
                        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
                }
                m_bPicking = true;
            }
            else
                m_bPicking = false;
        }
        END_SINGLETON(pGameInstance);
    }
    else
    {
        BEGIN_SINGLETON(CGameInstance, pGameInstance)
        {
            if (true == pGameInstance->Is_Rendering(m_pCollCollider->Get_CurPos(), m_pCollCollider->Get_CurScaleV3().x / 2.f))
            {
                if (nullptr != m_pRendererCom)
                {
                    m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
                }
                m_bPicking = true;
            }
            else
            {
                m_bPicking = false;
            }
            END_SINGLETON(pGameInstance);
        }
    }
}

HRESULT CWorldObj::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (nullptr == m_pModelCom)
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

    {
        _uint iNumMeshContainers = m_pModelCom->Get_NumMeshContainers();
        BEGIN_SINGLETON(CImGui_Manager, pImGuiManager)
        {
            CNavigationTool* pNavTool = static_cast<CNavigationTool*>(pImGuiManager->Get_MainView()->Get_ImGuiClass(CMainView::GUI_NAV));

            if (nullptr != m_pShaderCom)
            {
                if (FAILED(m_pShaderCom->Set_RawValue("g_bNavigationDebugColor", &pNavTool->Is_NavigationDebugColor(), sizeof(_bool))))
                    return E_FAIL;
            }

            for (_uint i = 0; i < iNumMeshContainers; ++i)
            {
                if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
                    return E_FAIL;

                if (FAILED(m_pModelCom->Bind_SRV(m_pShaderCom, "g_NormalTexture", i, TEXTYPE::TextureType_NORMALS)))
                    return E_FAIL;
#ifdef _DEBUG
                if (false == pNavTool->Is_RenderWireFrame())
                {
                    if (true == pImGuiManager->Get_MainView()->Get_ImGuiClass(CMainView::GUI_OBJ)->Is_Open())
                        m_pModelCom->Render_NonAnim(i, m_pShaderCom, 3);
                    else
                        m_pModelCom->Render_NonAnim(i, m_pShaderCom, 4);
                }
                else
                    m_pModelCom->Render_NonAnim(i, m_pShaderCom, 2);
#else
                m_pModelCom->Render_NonAnim(i, m_pShaderCom, 4);
#endif 
            }
        }
        END_SINGLETON(pImGuiManager);
    }
    return S_OK;
}

HRESULT CWorldObj::Ready_Component()
{
    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRendererCom)))
        return E_FAIL;
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;
    {
        BEGIN_SINGLETON(CData_Manager, pDataManager)
        {
            _wstring strModelName;
            strModelName.assign(m_strModelName.begin(), m_strModelName.end());
            CModel* _pModel = static_cast<CModel*>(pDataManager->Get_WorldMgr()->Find_WorldObjsModel(strModelName));
            if (nullptr != _pModel)
            {
                m_pModelCom = static_cast<CModel*>(_pModel->Clone(nullptr));
                Safe_AddRef(m_pModelCom);
                m_Components.insert(pair<const _tchar*, CComponent*>(COM_MODEL, m_pModelCom));
            }
        }
        END_SINGLETON(pDataManager);
    }
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
    if (nullptr == m_pModelCom)
        return E_FAIL;

    return S_OK;
}

HRESULT CWorldObj::SetUp_ConstantTable()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
            return E_FAIL;
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

_bool CWorldObj::Extern_EditValue()
{
    _bool bActive = false;

    if (ImGui::TreeNodeEx("Floor"))
    {
        ImGui::PushItemWidth(200);

        Extern_GobData();
        Show_Model();
        {
            BEGIN_SINGLETON(CImGui_Manager, pImGuiManager);
            {
                CGameObject* pDebugModel = pImGuiManager->Get_DebugObject();
                if (nullptr != pDebugModel)
                {
                    CTransform* pTransform = (CTransform*)pDebugModel->Get_Component(COM_TRANSFORM);
                    if (nullptr != pTransform)
                    {
                        pTransform->Set_Pos(XMLoadFloat3(&m_tGobDesc.v3Pos));
                        pTransform->Set_Scale(m_tGobDesc.v3Scale);
                    }
                }
            }
            END_SINGLETON(pImGuiManager);
        }
        bActive = ImGui::Button("Create");
        ImGui::PopItemWidth();
        ImGui::TreePop();
        return bActive;
    }
    return false;
}

_bool CWorldObj::Extern_HierakeyView(const string& _str)
{
    if (nullptr == m_pTransformCom)
        return false;

    _bool bActive = false;
    if (ImGui::TreeNodeEx(m_strModelName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushItemWidth(200);

        Extern_GobData();
        Extern_ColliderData();

        _bool bClone = ImGui::Button("Clone");
        if (true == bClone)
        {
            BEGIN_SINGLETON(CGameInstance, pGameInstance);
            {
                WORLDSAVES tFloorSaveData;
                tFloorSaveData.strModelName = m_strModelName;
                tFloorSaveData.tGobClientDesc = m_tGobDesc;
                tFloorSaveData.tColliderDesc = m_tColliderDesc;
                pGameInstance->Clone_GameObject(g_iCurrentLevel, Get_Layer(), Get_Proto(), &tFloorSaveData);
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

HRESULT CWorldObj::Create_Self(CGameObject** _ppOut)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        WORLDSAVES tFloorSaveData;
        tFloorSaveData.strModelName = m_strModelName;
        tFloorSaveData.tGobClientDesc = m_tGobDesc;
        tFloorSaveData.tColliderDesc = m_tColliderDesc;
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(LEVEL_TOOL, Get_Layer()
            , Get_Proto(), &tFloorSaveData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CWorldObj::Delete_Self()
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        pGameInstance->Add_DeleteObject(LEVEL_TOOL, Get_Layer(), this);
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

HRESULT CWorldObj::Save_Data()
{
    BEGIN_SINGLETON(CData_Manager, pDataManager);
    {
        WORLDSAVES tFloorSaveData;
        tFloorSaveData.strModelName = m_strModelName;
        tFloorSaveData.tGobClientDesc = m_tGobDesc;
        tFloorSaveData.tColliderDesc = m_tColliderDesc;
        tFloorSaveData.strPrototype = Get_Proto();
        // Save 
        pDataManager->Get_WorldMgr()->Insert_WorldObjs(tFloorSaveData);
    }
    END_SINGLETON(pDataManager);
    return S_OK;
}

HRESULT CWorldObj::Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData)
{
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        *_ppOut = static_cast<CGameObject_Client*>(pGameInstance->Clone_GameObject_Add(_eLevel, Get_Layer()
            , Get_Proto(), _pData));
    }
    END_SINGLETON(pGameInstance);
    return S_OK;
}

_bool CWorldObj::Is_Picking(_float3* _pOut)
{
    _bool bIsPicking = false;
    BEGIN_SINGLETON(CGameInstance, pGameInstance);
    {
        if (true == m_bPicking)
        {
            for (auto& iter : m_pModelCom->Get_MeshContainers())
            {
                bIsPicking = pGameInstance->Picking_Mesh(this, m_pTransformCom, (CVIBuffer*)iter, _pOut);
            }
        }
    }
    END_SINGLETON(pGameInstance);
    return bIsPicking;
}

// Picking Nav
_bool CWorldObj::Is_PickingNav()
{
    _bool bIsPicking = false;
    BEGIN_SINGLETON(CGameInstance, pGameInstance)
    {
        if (true == m_bPicking)
        {
            for (auto& iter : m_pModelCom->Get_MeshContainers())
            {
                bIsPicking = pGameInstance->Picking_MeshNav(this, m_pTransformCom, (CVIBuffer*)iter);
            }
        }
    }
    END_SINGLETON(pGameInstance);
    return bIsPicking;
}

void CWorldObj::Reset_Data()
{
    __super::Reset_Data();
}

HRESULT CWorldObj::Show_Model()
{
    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 8 * ImGui::GetTextLineHeightWithSpacing())))
    {
        BEGIN_SINGLETON(CData_Manager, pDataManager);
        m_strModelName = pDataManager->Get_WorldMgr()->Research_Model(m_strModelName);
        ImGui::EndListBox();
        END_SINGLETON(pDataManager);
    }
    return S_OK;
}

void CWorldObj::Extern_ColliderData()
{
    if (nullptr != m_pCollCollider)
    {
        CCollider::COLLIDERDESC tCollData;
        tCollData.vScale = m_tColliderDesc.v3Scale;
        tCollData.vTranslation = m_tColliderDesc.v3Translate;

        _float Translate[3] = { tCollData.vTranslation.x,  tCollData.vTranslation.y, tCollData.vTranslation.z };
        _float CollSclae[3] = { tCollData.vScale.x ,  tCollData.vScale.y , tCollData.vScale.z };

        if (ImGui::DragFloat3("FindT", Translate, 0.5f, -100000000.f, 100000000.f, "%.4f"))
        {
            m_pCollCollider->Set_Translate(_float3(Translate[0], Translate[1], Translate[2]));
        }
        if (ImGui::DragFloat3("TindS", CollSclae, 0.5f, 0.0f, 100000000.f, "%.4f"))
        {
            m_pCollCollider->Set_Scale(_float3(CollSclae[0], CollSclae[1], CollSclae[2]));
        }

        m_tColliderDesc.v3Translate = _float3(Translate[0], Translate[1], Translate[2]);
        m_tColliderDesc.v3Scale = _float3(CollSclae[0], CollSclae[1], CollSclae[2]);


        if (nullptr != m_pCollCollider)
            m_pCollCollider->Tick(m_pTransformCom->Get_WorldMatrix());
    }
}

CWorldObj* CWorldObj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWorldObj* pInstance = new CWorldObj(pDevice, pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
        MSG_BOX("CFloor::Create to Failed");
    }
    return pInstance;
}

CGameObject* CWorldObj::Clone(void* _pArg)
{
    CWorldObj* pInstance = new CWorldObj(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
    {
        Safe_Release(pInstance);
        MSG_BOX("CFloor::Clone to Failed");
    }
    return pInstance;
}

void CWorldObj::Free()
{
    Safe_Release(m_pModelCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pCollCollider);

    __super::Free();
}
