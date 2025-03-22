#include "UI_Select_Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "UI_CharSelect.h"

#include "Sheath.h"


CUI_Select_Model::CUI_Select_Model(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
    : CUI_Base(_pDevice, _pContext) 
{
}

CUI_Select_Model::CUI_Select_Model(const CUI_Select_Model& _rhs)
    : CUI_Base(_rhs)
{
}


HRESULT CUI_Select_Model::NativeConstruct_Prototype()
{
    if (FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Select_Model::NativeConstruct_Clone(void* pArg)
{
    if (FAILED(__super::NativeConstruct_Clone(pArg)))
        return E_FAIL;


    if (FAILED(SetUp_Components()))
        return E_FAIL;
	
     Player = *(_int*)pArg;

    if (pArg != nullptr && Player == 0) //플레이어1
    {
        m_pTransformCom->Set_PosF3(_float3(-13.f, -1.f, 0.f));
        Change_Model(MODEL_TANJIRO);
    }

    if (pArg != nullptr && Player == 1) //플레이어2
    {
        m_pTransformCom->Set_PosF3(_float3(13.f, -1.f, 2.f));
        m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f,1.f), 60.f);
        Change_Model(MODEL_SHINOBU);
    }
    
    return S_OK;
}

void CUI_Select_Model::Tick(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
    __super::Tick(_dTimeDelta);
	
	if(m_pAnimator != nullptr)
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



    if ((pGameInstance->Get_DIKeyUp(DIK_LSHIFT) && Player == 0) || (pGameInstance->Get_DIKeyUp(DIK_RSHIFT) && Player == 1))
    {
        if (IsSelect)
            IsSelect = false;
        else
        {
            CurrentState = SELECT;
            IsSelect = true;
        }
    }

  

    Setting_Trigger();
 
    //if (nullptr != m_pModel)
    //    m_pModel->Update_Animation(_dTimeDelta);

    RELEASE_INSTANCE(CGameInstance);

}

void CUI_Select_Model::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (nullptr != m_pRenderer)
        m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CUI_Select_Model::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    SetUp_ConstantTable();

    if (RenderModel == true)
    {
        if (nullptr != m_pModel)
        {
            _uint iNumMeshContainers = m_pModel->Get_NumMeshContainers();

            for (_uint i = 0; i < iNumMeshContainers; ++i)
            {
                if (FAILED(m_pModel->Bind_SRV(m_pShader, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
                    return E_FAIL;

                m_pModel->Bind_SRV(m_pShader, "g_OutLineTexture", i, TextureType_SPECULAR);

                m_pModel->Render(i, m_pShader, 0);
            }
        }
    }
    return S_OK;
}

void CUI_Select_Model::Change_Model(_uint State)
{
   // if (nullptr == m_pModel || nullptr == m_pAnimator)
      //  return;

    switch (State)
    {

    case Client::CUI_Select_Model::MODEL_TANJIRO:
        m_pModel = m_ModelList[MODEL_TANJIRO];
        m_pAnimator = m_AnimatorList[MODEL_TANJIRO];
		
        m_SwordList[MODEL_TANJIRO]->Set_Render(true);
        m_SwordList[MODEL_ZENITSU]->Set_Render(false);
        m_SwordList[MODEL_SABITO]->Set_Render(false);
        m_SwordList[MODEL_SHINOBU]->Set_Render(false);
        m_pAnimator->Set_AnimationClipToEntry();
        break;
    case Client::CUI_Select_Model::MODEL_ZENITSU:
        m_pModel = m_ModelList[MODEL_ZENITSU];
        m_pAnimator = m_AnimatorList[MODEL_ZENITSU];

        m_SwordList[MODEL_TANJIRO]->Set_Render(false);
        m_SwordList[MODEL_ZENITSU]->Set_Render(true);
        m_SwordList[MODEL_SABITO]->Set_Render(false);
        m_SwordList[MODEL_SHINOBU]->Set_Render(false);
        m_pAnimator->Set_AnimationClipToEntry();
        break;
    case Client::CUI_Select_Model::MODEL_SABITO:
        m_pModel = m_ModelList[MODEL_SABITO];
        m_pAnimator = m_AnimatorList[MODEL_SABITO];
		
        m_SwordList[MODEL_TANJIRO]->Set_Render(false);
        m_SwordList[MODEL_ZENITSU]->Set_Render(false);
        m_SwordList[MODEL_SABITO]->Set_Render(true);
        m_SwordList[MODEL_SHINOBU]->Set_Render(false);
        m_pAnimator->Set_AnimationClipToEntry();
        break;
   /* case Client::CUI_Select_Model::MODEL_SHINOBU:
        m_pModel = m_ModelList[MODEL_SHINOBU];
        m_pAnimator = m_AnimatorList[MODEL_SHINOBU];

        m_SwordList[MODEL_TANJIRO]->Set_Render(false);
        m_SwordList[MODEL_ZENITSU]->Set_Render(false);
        m_SwordList[MODEL_SABITO]->Set_Render(false);
        m_SwordList[MODEL_SHINOBU]->Set_Render(true);
        m_pAnimator->Set_AnimationClipToEntry();
        break;*/
    case Client::CUI_Select_Model::MODEL_SHINOBU:
        m_pModel = m_ModelList[MODEL_AKAZA];
        m_pAnimator = m_AnimatorList[MODEL_AKAZA];

       m_SwordList[MODEL_TANJIRO]->Set_Render(false);
        m_SwordList[MODEL_ZENITSU]->Set_Render(false);
        m_SwordList[MODEL_SABITO]->Set_Render(false);
        m_SwordList[MODEL_SHINOBU]->Set_Render(false);
        m_pAnimator->Set_AnimationClipToEntry();
        break;
    default:
        break;
    }
   
}

HRESULT CUI_Select_Model::SetUp_ConstantTable()
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
        _bool bMotionBlur = false;
        if (FAILED(m_pShader->Set_RawValue("g_ObjectMotionBlur", &bMotionBlur, sizeof(_bool))))
        {
            RELEASE_INSTANCE(CGameInstance);
            return E_FAIL;
        }
       
        if (m_pModel == m_ModelList[MODEL_AKAZA])
        {
            _bool WarpSampler = true;
            if (FAILED(m_pShader->Set_RawValue("g_bWrapSampler", &WarpSampler, sizeof(_bool))))
            {
                RELEASE_INSTANCE(CGameInstance);
                return E_FAIL;
            }
        }
        else
        {
            _bool WarpSampler = false;
            if (FAILED(m_pShader->Set_RawValue("g_bWrapSampler", &WarpSampler, sizeof(_bool))))
            {
                RELEASE_INSTANCE(CGameInstance);
                return E_FAIL;
            }
        }
    }
    RELEASE_INSTANCE(CGameInstance);

    return S_OK;


}

HRESULT CUI_Select_Model::SetUp_Components()
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderer)))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL, (CComponent**)&m_pShader)))
        return E_FAIL;
	   
	//캐릭터 모델
	//========================================================================
	
    m_pAllAnimModel = (CModel*)pGameInstance->Clone_Component(LEVEL_SELECT, PROTO_COMP_MODEL_UI_MENU);

    CModel* pTanjiroModel = nullptr;

    pTanjiroModel = (CModel*)pGameInstance->Clone_Component(LEVEL_SELECT, PROTO_COMP_MODEL_UI_TANJIRO,(CComponent**)&pTanjiroModel);
    m_ModelList.push_back(pTanjiroModel);
	
    CModel* pZenitsuModel = nullptr;

	pZenitsuModel = (CModel*)pGameInstance->Clone_Component(LEVEL_SELECT, PROTO_COMP_MODEL_UI_ZENITSU,(CComponent**)&pZenitsuModel);    
    m_ModelList.push_back(pZenitsuModel);

    CModel* pSabitoModel = nullptr;

    pSabitoModel = (CModel*)pGameInstance->Clone_Component(LEVEL_SELECT, PROTO_COMP_MODEL_UI_SABITO, (CComponent**)&pSabitoModel);
    m_ModelList.push_back(pSabitoModel);

    CModel* pShinobuModel = nullptr;

    pShinobuModel = (CModel*)pGameInstance->Clone_Component(LEVEL_SELECT, PROTO_COMP_MODEL_UI_SHINOBU, (CComponent**)&pShinobuModel);
    m_ModelList.push_back(pShinobuModel);

 /*   CModel* pKyoujuroModel = nullptr;

    pKyoujuroModel = (CModel*)pGameInstance->Clone_Component(LEVEL_SELECT, PROTO_COMP_MODEL_UI_KYOUJURO, (CComponent**)&pKyoujuroModel);
    m_ModelList.push_back(pKyoujuroModel);*/

      CModel* pAkazaModel = nullptr;

      pAkazaModel = (CModel*)pGameInstance->Clone_Component(LEVEL_SELECT, PROTO_COMP_MODEL_UI_AKAZA, (CComponent**)&pAkazaModel);
     m_ModelList.push_back(pAkazaModel);

    //애니메이션
    //=====================================================
	
	CAnimator::ANIMATORDESC tDesc;
	tDesc.pModel = m_pAllAnimModel;
	tDesc.pTransform = m_pTransformCom;
    tDesc.strFilePath = L"Menu_Tanjiro";		
	CAnimator* pTanjiroAnimator = nullptr;
    pTanjiroAnimator =(CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
    pTanjiroAnimator->Set_Model(m_ModelList[MODEL_TANJIRO]);
    m_AnimatorList.push_back(pTanjiroAnimator);
	
    tDesc.pModel = m_pAllAnimModel;
    tDesc.strFilePath = L"Menu_Zenitsu";
    CAnimator* pZenitsuAnimator = nullptr;
    pZenitsuAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
    pZenitsuAnimator->Set_Model(pZenitsuModel);
    m_AnimatorList.push_back(pZenitsuAnimator);
	
    tDesc.pModel = m_pAllAnimModel;
    tDesc.strFilePath = L"Menu_Sabito";
    CAnimator* pSabitoAnimator = nullptr;
    pSabitoAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
    pSabitoAnimator->Set_Model(pSabitoModel);
    m_AnimatorList.push_back(pSabitoAnimator);
	
    tDesc.pModel = m_pAllAnimModel;
	tDesc.strFilePath = L"Menu_Shinobu";
    CAnimator* pShinobuAnimator = nullptr;
    pShinobuAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
    pShinobuAnimator->Set_Model(pShinobuModel);
    m_AnimatorList.push_back(pShinobuAnimator);

   /* tDesc.pModel = m_pAllAnimModel;
    tDesc.strFilePath = L"Menu_Kyoujuro";
    CAnimator* pKyoujuroAnimator = nullptr;
    pKyoujuroAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
    pKyoujuroAnimator->Set_Model(pKyoujuroModel);
    m_AnimatorList.push_back(pKyoujuroAnimator);*/

   tDesc.pModel = m_pAllAnimModel;
  tDesc.strFilePath = L"Menu_Akaza";
  CAnimator* pAkazaAnimator = nullptr;
  pAkazaAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
  pAkazaAnimator->Set_Model(pAkazaModel);
  m_AnimatorList.push_back(pAkazaAnimator);
	
    //검집
	//======================================================

  /*  
	
      Path = { L"Model", L"Item", L"Menu_Sabito", L"FBX_Converter", L"Data" };
      pFolder = pGameInstance->Find_Folder(&Path);
      pData = pFolder->FindData(L"Menu_Sabito_Sheath");
	
      CSelect_Item* m_pSabitoSheath = nullptr;
	
      
      tItemDesc.pItemData = pData;
      tItemDesc.pTargetModel = m_ModelList[MODEL_SABITO];
      tItemDesc.pTargetTransform = m_pTransformCom;
      tItemDesc.strModelProtoName = PROTO_COMP_MENU_SABITO_SHEATH;
      m_pSabitoSheath = static_cast<CSelect_Item*>(pGameInstance->Clone_GameObject_Add(LEVEL_SELECT, LAYER_UI, PROTO_GOB_SELECT_ITEM, &tItemDesc));
      m_SheathList.push_back(m_pSabitoSheath);*/
 
      //검
     //======================================================
    PATHS Path;
    FILEGROUP* pFolder;
    FILEDATA* pData;
    CSheath::ITEMDESC tItemDesc;
     
      Path = { L"Model", L"Item", L"Menu_Tanjiro", L"FBX_Converter", L"Data" };
      pFolder = pGameInstance->Find_Folder(&Path);
      pData = pFolder->FindData(L"Menu_Tanjiro_Sword");

      CSelect_Item* m_pTanjiroSword = nullptr;

      tItemDesc.pItemData = pData;
      tItemDesc.pTargetModel = m_ModelList[MODEL_TANJIRO];
      tItemDesc.pTargetTransform = m_pTransformCom;
      tItemDesc.strModelProtoName = PROTO_COMP_MENU_TANJIRO_SWORD;
      m_pTanjiroSword = static_cast<CSelect_Item*>(pGameInstance->Clone_GameObject_Add(LEVEL_SELECT, LAYER_UI, PROTO_GOB_SELECT_ITEM, &tItemDesc));
      m_SwordList.push_back(m_pTanjiroSword);
      Safe_AddRef(m_pTanjiroSword);
	
	
      Path = { L"Model", L"Item", L"Menu_Zenitsu", L"FBX_Converter", L"Data" };
      pFolder = pGameInstance->Find_Folder(&Path);
      pData = pFolder->FindData(L"Menu_Zenitsu_Sword");

      CSelect_Item* m_pZenitsuSword = nullptr;

      tItemDesc.pItemData = pData;
      tItemDesc.pTargetModel = m_ModelList[MODEL_ZENITSU];
      tItemDesc.pTargetTransform = m_pTransformCom;
      tItemDesc.strModelProtoName = PROTO_COMP_MENU_ZENITSU_SWORD;
      m_pZenitsuSword = static_cast<CSelect_Item*>(pGameInstance->Clone_GameObject_Add(LEVEL_SELECT, LAYER_UI, PROTO_GOB_SELECT_ITEM, &tItemDesc));
      m_SwordList.push_back(m_pZenitsuSword);
      Safe_AddRef(m_pZenitsuSword);
	
	
	
      Path = { L"Model", L"Item", L"Menu_Sabito", L"FBX_Converter", L"Data" };
      pFolder = pGameInstance->Find_Folder(&Path);
      pData = pFolder->FindData(L"Menu_Sabito_Sword");
	
      CSelect_Item* m_pSabitoSword = nullptr;
   
      tItemDesc.pItemData = pData;
      tItemDesc.pTargetModel = m_ModelList[MODEL_SABITO];
      tItemDesc.pTargetTransform = m_pTransformCom;
      tItemDesc.strModelProtoName = PROTO_COMP_MENU_SABITO_SWORD;
      m_pSabitoSword = static_cast<CSelect_Item*>(pGameInstance->Clone_GameObject_Add(LEVEL_SELECT, LAYER_UI, PROTO_GOB_SELECT_ITEM, &tItemDesc));
      m_SwordList.push_back(m_pSabitoSword);
      Safe_AddRef(m_pSabitoSword);
	

      Path = { L"Model", L"Item", L"Menu_Shinobu", L"FBX_Converter", L"Data" };
      pFolder = pGameInstance->Find_Folder(&Path);
      pData = pFolder->FindData(L"Menu_Shinobu_Sword");

      CSelect_Item* m_pShinobuSword = nullptr;

      tItemDesc.pItemData = pData;
      tItemDesc.pTargetModel = m_ModelList[MODEL_SHINOBU];
      tItemDesc.pTargetTransform = m_pTransformCom;
      tItemDesc.strModelProtoName = PROTO_COMP_MENU_SHINOBU_SWORD;
      m_pShinobuSword = static_cast<CSelect_Item*>(pGameInstance->Clone_GameObject_Add(LEVEL_SELECT, LAYER_UI, PROTO_GOB_SELECT_ITEM, &tItemDesc));
      m_SwordList.push_back(m_pShinobuSword);
      Safe_AddRef(m_pShinobuSword);

    RELEASE_INSTANCE(CGameInstance);
	
    return S_OK;
}

void CUI_Select_Model::Setting_Trigger()
{
    if (PrevState == CurrentState)
        return;
    PrevState = CurrentState;

    switch (CurrentState)
    {
    case Client::CUI_Select_Model::SELECT:
        m_pAnimator->Input_Trigger("Select");
        CurrentState = STATE_END;
        break; 
    case Client::CUI_Select_Model::STATE_END:
        break;
    }
}

CUI_Select_Model* CUI_Select_Model::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CUI_Select_Model* pInstance = new CUI_Select_Model(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CUI_Select_Model::Create To Failed");
#endif
    }
    return pInstance;
}

CGameObject* CUI_Select_Model::Clone(void* pArg)
{
    CUI_Select_Model* pInstance = new CUI_Select_Model(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CUI_Select_Model::Clone To Failed");
#endif
    }
    return pInstance;
}

void CUI_Select_Model::Free()
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

    for (int i = 0; i < m_SwordList.size(); ++i)
    {
        Safe_Release(m_SwordList[i]);
    }
    m_SwordList.clear();

    Safe_Release(m_pShader);
    Safe_Release(m_pRenderer);
	
    Safe_Release(m_pAllAnimModel);
	
    __super::Free();
}
