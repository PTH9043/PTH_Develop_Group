#include "MiniGame_Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "UI_MiniGame.h"

CMiniGame_Model::CMiniGame_Model(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext)
{
}

CMiniGame_Model::CMiniGame_Model(const CMiniGame_Model& _rhs)
    : CUI_Base(_rhs)
{
}

HRESULT CMiniGame_Model::NativeConstruct_Prototype()
{

    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMiniGame_Model::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;


    if (FAILED(SetUp_Components()))
        return E_FAIL;

    type = *(_int*)pArg;

    if (pArg != nullptr && type == 0) 
    {
        m_pTransformCom->Set_PosF3(_float3(44.f, 32.1f, 130.7f));
        m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 30.f);
        Change_Model(NURSE_0);
    }

    if (pArg != nullptr && type == 1) 
    {
        m_pTransformCom->Set_PosF3(_float3(35.4f, 32.1f, 131.7f));
        m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 30.f);
        Change_Model(NURSE_1);
    }

    if (pArg != nullptr && type == 2)
    {
        m_pTransformCom->Set_PosF3(_float3(52.6f, 32.1f, 125.6f));
        m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 30.f);
        Change_Model(NURSE_2);
    }


    return S_OK;
}


void CMiniGame_Model::Tick(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    __super::Tick(_dTimeDelta);

    if (m_pAnimator != nullptr)
        m_pAnimator->Tick(_dTimeDelta);


    if (nullptr != m_pModel && nullptr != m_pAllAnimModel)
    {
        for (auto& iter : m_pModel->Get_HierachyNodeList())
        {
            for (auto& pAnimBone : m_pAllAnimModel->Get_HierachyNodeList())
            {
                if (iter->Get_Name() == pAnimBone->Get_Name())
                {
                    iter->Set_TransformationMatrix(pAnimBone->Get_TransformMatrix());
                    continue;
                }
            }
        }
    }

    TriggerTime += _dTimeDelta;

    if (Trigger <= 0.0)
        Trigger = 0.0;

    if (TriggerTime >= 1)
    {
        Trigger -= 1.0;
        TriggerTime = 0.0;
    }
	
    Setting_Trigger(_dTimeDelta);

    RELEASE_INSTANCE(CGameInstance);
}

void CMiniGame_Model::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (nullptr != m_pRenderer)
        m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMiniGame_Model::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetUp_ConstantTable();

 
        if (nullptr != m_pModel)
        {
            _uint iNumMeshContainers = m_pModel->Get_NumMeshContainers();

            for (_uint i = 0; i < iNumMeshContainers; ++i)
            {
                if (FAILED(m_pModel->Bind_SRV(m_pShader, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
                    return E_FAIL;

                m_pModel->Render(i, m_pShader, 0);
            }
        }
 
    return S_OK;
}

void CMiniGame_Model::Extern_ImGuiClass()
{
    _float3 vPos;
    XMStoreFloat3(&vPos, m_pTransformCom->Get_Pos());

    _float fRotate;
	
    _float3 fScale = m_pTransformCom->Get_Scale();

    if (ImGui::DragFloat3("Pos", &vPos.x, 0.1f, -10000.f, 10000.f))
        m_pTransformCom->Set_PosF3(vPos);

    if (ImGui::DragFloat("Rotate", &fRotate, 0.1f, -300.f, 300.f))
        m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), fRotate);

    if (ImGui::DragFloat3("Scale", &fScale.x, 0.1f, -300.f, 300.f))
        m_pTransformCom->Set_Scale(fScale);

}

void CMiniGame_Model::Change_Model(_uint State)
{
    switch (State)
    {

    case Client::CMiniGame_Model::NURSE_0:
        m_pModel = m_ModelList[NURSE_0];
        m_pAnimator = m_AnimatorList[NURSE_0];
        break;
    case Client::CMiniGame_Model::NURSE_1:
        m_pModel = m_ModelList[NURSE_1];
        m_pAnimator = m_AnimatorList[NURSE_1];
        break;
    case Client::CMiniGame_Model::NURSE_2:
        m_pModel = m_ModelList[NURSE_2];
        m_pAnimator = m_AnimatorList[NURSE_2];
        break;
    default:
        break;
    }
}

void CMiniGame_Model::Setting_Trigger(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    list<CGameObject*> Prototype_GameObjects = pGameInstance->Find_GameObjectList(LEVEL_MINIGAME, LAYER_UI);
    CUI_MiniGame* Combo = nullptr;
    for (auto it = Prototype_GameObjects.begin(); it != Prototype_GameObjects.end(); it++)
    {
        if (dynamic_cast<CUI_MiniGame*>(*it)->Is_MyName(L"Mini_Combo") == true)
        {
            Combo = dynamic_cast<CUI_MiniGame*>(*it);
            break;
        }
    }

    if(Combo->Get_Combo() == 0)
        m_pAnimator->Input_Trigger("Cry");

    if (Combo->Get_Combo() == 1)
        m_pAnimator->Input_Trigger("Fighting");


    RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMiniGame_Model::SetUp_ConstantTable()
{
    if (nullptr == m_pTransformCom || nullptr == m_pShader)
        return E_FAIL;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    {
        _float4x4 WorldMatrix, ViewMatrix, ProjMatrix;
        WorldMatrix = m_pTransformCom->Get_WorldFloat4x4_TP();
        ViewMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW);
        ProjMatrix = pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ);
        _float camFar = pGameInstance->Get_CamFar();

        if (FAILED(m_pShader->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShader->Set_RawValue("g_ViewMatrix", &ViewMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShader->Set_RawValue("g_ProjMatrix", &ProjMatrix, sizeof(_float4x4))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
        if (FAILED(m_pShader->Set_RawValue("g_CamFar", &camFar, sizeof(_float))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
    }
    RELEASE_INSTANCE(CGameInstance);

    return S_OK;

}

HRESULT CMiniGame_Model::SetUp_Components()
{

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderer)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL, (CComponent**)&m_pShader)))
        return E_FAIL;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//¸ðµ¨
	
    CModel* pNurse1Model = nullptr;
    pNurse1Model = (CModel*)pGameInstance->Clone_Component(LEVEL_MINIGAME, PROTO_COMP_MODEL_NURSE1, (CComponent**)&pNurse1Model);
    m_ModelList.push_back(pNurse1Model);

    CModel* pNurse2Model = nullptr;
    pNurse2Model = (CModel*)pGameInstance->Clone_Component(LEVEL_MINIGAME, PROTO_COMP_MODEL_NURSE2, (CComponent**)&pNurse2Model);
    m_ModelList.push_back(pNurse2Model);

    CModel* pNurse3Model = nullptr;
    pNurse3Model = (CModel*)pGameInstance->Clone_Component(LEVEL_MINIGAME, PROTO_COMP_MODEL_NURSE3, (CComponent**)&pNurse3Model);
    m_ModelList.push_back(pNurse3Model);

   
    //=====================================================

    CAnimator::ANIMATORDESC tDesc;
    tDesc.pModel = pNurse1Model;
    tDesc.pTransform = m_pTransformCom;
    tDesc.strFilePath = L"NPC_Nurse1";
    CAnimator* pNurse1Animator = nullptr;
    pNurse1Animator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
    pNurse1Animator->Set_Model(m_ModelList[NURSE_0]);
    m_AnimatorList.push_back(pNurse1Animator);


    tDesc.pModel = pNurse2Model;
    tDesc.strFilePath = L"NPC_Nurse2";
    CAnimator* pNurse2Animator = nullptr;
    pNurse2Animator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
    pNurse2Animator->Set_Model(m_ModelList[NURSE_1]);
    m_AnimatorList.push_back(pNurse2Animator);

    tDesc.pModel = pNurse3Model;
    tDesc.strFilePath = L"NPC_Nurse3";
    CAnimator* pNurse3Animator = nullptr;
    pNurse3Animator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
    pNurse3Animator->Set_Model(m_ModelList[NURSE_2]);
    m_AnimatorList.push_back(pNurse3Animator);

  
    RELEASE_INSTANCE(CGameInstance);

    return S_OK;

}

CMiniGame_Model* CMiniGame_Model::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{

    CMiniGame_Model* pInstance = new CMiniGame_Model(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CMiniGame_Model::Create To Failed");
#endif
    }
    return pInstance;
}

CGameObject* CMiniGame_Model::Clone(void* pArg)
{
    CMiniGame_Model* pInstance = new CMiniGame_Model(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CMiniGame_Model::Clone To Failed");
#endif
    }
    return pInstance;
}

void CMiniGame_Model::Free()
{
    for (int i = 0; i < m_ModelList.size(); ++i)
    {
        Safe_Release(m_ModelList[i]);
    }
    m_ModelList.clear();

    for (int i = 0; i < m_AnimatorList.size(); ++i)
    {
        Safe_Release(m_AnimatorList[i]);
    }
    m_AnimatorList.clear();

    Safe_Release(m_pShader);
    Safe_Release(m_pRenderer);

    Safe_Release(m_pAllAnimModel);

    __super::Free();
}
