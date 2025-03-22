#include "Animal.h"
#include "GameInstance.h"
#include "Animation.h"
#include "HierarchyNode.h"
#include "Player.h"

CAnimal::CAnimal(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPawn(_pDevice, _pContext)
{
}

CAnimal::CAnimal(const CAnimal& _rhs)
	: CPawn(_rhs)
{
}

HRESULT CAnimal::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimal::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;
    
    m_iType = *(_int*)pArg;
	
  //  SetUp_Components();
	
    Change_Model(m_iType);
    Setting_Positon(m_iType);

	return S_OK;
}

void CAnimal::Tick(const _double& _dTimeDelta)
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
	
    Setting_Trigger(_dTimeDelta);
    Change_State();

    RELEASE_INSTANCE(CGameInstance);
}

void CAnimal::Late_Tick(const _double& _dTimeDelta)
{
    __super::Late_Tick(_dTimeDelta);

    if (true == CGameInstance::Get_Instance()->Is_Rendering(m_pTransformCom->Get_Pos(), 30.f))
    {
        if (nullptr != m_pRendererCom)
        {
            m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
        }
    }
}

HRESULT CAnimal::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;
	
    if (nullptr != m_pModel)
    {
        _uint iNumMeshContainers = m_pModel->Get_NumMeshContainers();

        for (_uint i = 0; i < iNumMeshContainers; ++i)
        {
            if (FAILED(m_pModel->Bind_SRV(m_pShaderCom, "g_DiffuseTexture", i, TEXTYPE::TextureType_DIFFUSE)))
                return E_FAIL;

            m_pModel->Bind_SRV(m_pShaderCom, "g_OutLineTexture", i, TextureType_SPECULAR);

            m_pModel->Render(i, m_pShaderCom, 0);
        }
    }
    
    return S_OK;
}

void CAnimal::Extern_ImGuiClass()
{
    _float3 vPos;
    XMStoreFloat3(&vPos, m_pTransformCom->Get_Pos());

    _float fRotate;

    if (ImGui::DragFloat3("Pos", &vPos.x, 0.1f, -10000.f, 10000.f))
        m_pTransformCom->Set_PosF3(vPos);

    if (ImGui::DragFloat("Rotate", &fRotate, 0.1f, -300.f, 300.f))
    m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), fRotate);
	
    
    //if (ImGui::DragFloat3("Lentgh", &m_Length.x, 0.1f, -400.f, 400.f));
}



void CAnimal::Change_Model(_uint Type)
{
    switch (Type)
    {
    case Client::CAnimal::ANIMAL_DEER:
        m_pModel = m_ModelList[ANIMAL_DEER];
        m_pAnimator = m_AnimatorList[ANIMAL_DEER];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;

    case Client::CAnimal::ANIMAL_BUTTERFLY:
        m_pModel = m_ModelList[ANIMAL_BUTTERFLY];
        m_pAnimator = m_AnimatorList[ANIMAL_BUTTERFLY];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;

    case Client::CAnimal::ANIMAL_BUTTERFLY2:
        m_pModel = m_ModelList[ANIMAL_BUTTERFLY];
        m_pAnimator = m_AnimatorList[ANIMAL_BUTTERFLY];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;

    case Client::CAnimal::ANIMAL_BUTTERFLY3:
        m_pModel = m_ModelList[ANIMAL_BUTTERFLY];
        m_pAnimator = m_AnimatorList[ANIMAL_BUTTERFLY];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;

    case Client::CAnimal::ANIMAL_CAT:
        m_pModel = m_ModelList[ANIMAL_CAT];
        m_pAnimator = m_AnimatorList[ANIMAL_CAT];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;

    case Client::CAnimal::ANIMAL_BIRD:
        m_pModel = m_ModelList[ANIMAL_BIRD];
        m_pAnimator = m_AnimatorList[ANIMAL_BIRD];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;

    case Client::CAnimal::ANIMAL_DOG:
        m_pModel = m_ModelList[ANIMAL_DOG];
        m_pAnimator = m_AnimatorList[ANIMAL_DOG];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;

    case Client::CAnimal::ANIMAL_MOUSE:
        m_pModel = m_ModelList[ANIMAL_MOUSE];
        m_pAnimator = m_AnimatorList[ANIMAL_MOUSE];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;

    case Client::CAnimal::ANIMAL_CHICKEN:
        m_pModel = m_ModelList[ANIMAL_CHICKEN];
        m_pAnimator = m_AnimatorList[ANIMAL_CHICKEN];
        Safe_AddRef(m_pModel);
        Safe_AddRef(m_pAnimator);
        break;
		
    }
}

void CAnimal::Setting_Positon(_uint Type)
{
    if (g_iCurrentLevel == LEVEL_STORY)
    {
        switch (Type)
        {
        case Client::CAnimal::ANIMAL_DEER:
            m_pTransformCom->Set_PosF3(_float3(-643.2f, 185.8f, 323.9f));
            break;

        case Client::CAnimal::ANIMAL_BUTTERFLY:
            m_pTransformCom->Set_PosF3(_float3(-474.4f, 172.6f, 326.9f));
            break;

        case Client::CAnimal::ANIMAL_CAT:
            m_pTransformCom->Set_PosF3(_float3(-1340.000f, 191.800f, 410.314f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 180.f);
            break;

        case Client::CAnimal::ANIMAL_BIRD:
            m_pTransformCom->Set_PosF3(_float3(-1031.f, 293.f, 890.1f));
            break;

        case Client::CAnimal::ANIMAL_DOG:
            m_pTransformCom->Set_PosF3(_float3(-530.6f, 174.9f, 345.f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 120.f);
            break;

        case Client::CAnimal::ANIMAL_MOUSE:
            m_pTransformCom->Set_PosF3(_float3(-522.6f, 169.6f, 332.2f));
            break;
        }
    }

    if (g_iCurrentLevel == LEVEL_RATTAN)
    {
        switch (Type)
        {
        case Client::CAnimal::ANIMAL_BIRD:
            m_pTransformCom->Set_PosF3(_float3(39.4f, 50.f, 0.2f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 270.f);
            break;
        case Client::CAnimal::ANIMAL_DOG:
            m_pTransformCom->Set_PosF3(_float3(-36.f, 8.6f, -30.2f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 270.f);
            break;
        case Client::CAnimal::ANIMAL_BUTTERFLY:
            m_pTransformCom->Set_PosF3(_float3(-24.6f, 6.4f, 17.5f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 270.f);
            break;
        case Client::CAnimal::ANIMAL_CHICKEN:
            m_pTransformCom->Set_PosF3(_float3(-2.4f, 8.6f, 16.f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 180.f);
            Run = true;
            break;
        case Client::CAnimal::ANIMAL_BUTTERFLY2:
            m_pTransformCom->Set_PosF3(_float3(9.2f, 14.4f, -10.4f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 180.f);
            break;
        case Client::CAnimal::ANIMAL_BUTTERFLY3:
            m_pTransformCom->Set_PosF3(_float3(-44.4f, 12.6f, -30.5f));
            m_pTransformCom->Rotate_Fix(XMVectorSet(0.f, 1.f, 0.f, 1.f), 200.f);
            break;
        }
    }
}

void CAnimal::Setting_Trigger(const _double& _dTimeDelta)
{
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

    CPlayer* Player = dynamic_cast<CPlayer*>(pGameInstance->Find_GameObject(g_iCurrentLevel, LAYER_STORYPLAYER, 0));
    if (nullptr == Player)
    {
        RELEASE_INSTANCE(CGameInstance);
        return;
    }
    CTransform* PlayerTransform = Player->Get_Transform();
 
    _float3 fLength = m_pTransformCom->Get_PosF3() - PlayerTransform->Get_PosF3();
    _vector vLength = XMLoadFloat3(&fLength);
    vLength = XMVector3Length(vLength);
    XMStoreFloat3(&m_Length, vLength);
	
    if (m_iType == ANIMAL_DEER)
    {

        if ((m_Length.x <= 100.f) || pGameInstance->Get_DIKeyUp(DIK_2))
        {
            CurrentState = RUN;
            Run = true;
            if (SoundPlay == false)
            {
                pGameInstance->Play_SoundW(TEXT("UI_DearRun2"), 0.3f);
                SoundPlay = true;
            }
        }
    
        if (Run == true)
        {
            m_RunTime += (_float)_dTimeDelta * 0.5f;
            _float3 Pos = m_pTransformCom->Get_PosF3();
            Pos.z += m_RunTime;
            m_pTransformCom->Set_PosF3(Pos);
            if (Pos.z >= 410.f)
            {
                Run = false;
                m_RunTime = 0.f;
            }
        }
    }

    if (m_iType == ANIMAL_CAT)
    {
       if ((m_Length.x <= 50.f ) || pGameInstance->Get_DIKeyUp(DIK_4))
        {
			CurrentState = RUN;
            Run = true;
            if (SoundPlay == false)
            {
                pGameInstance->Play_SoundW(TEXT("UI_Cat"), 0.4f);
                SoundPlay = true;
            }
        }
		
        if (Run == true)
        {
            m_RunTime += (_float)_dTimeDelta * 0.3f;
            _float3 Pos = m_pTransformCom->Get_PosF3();
            Pos.z -= m_RunTime;
            m_pTransformCom->Set_PosF3(Pos);
            if (Pos.z <= 310.f)
            {
                Run = false;
                m_RunTime = 0.f;
            }
        }
		
    }

    if (m_iType == ANIMAL_BIRD)
    {
        if (g_iCurrentLevel == LEVEL_STORY)
        {
            _float3 pos = m_pTransformCom->Get_PosF3();

            m_Angle += (_float)_dTimeDelta * 30.f;

            if (m_Angle >= 360.f)
                m_Angle = 0.f;

            pos.x = cos(XMConvertToRadians(m_Angle)) * 30.f + -1031.f;
            pos.z = sin(XMConvertToRadians(m_Angle)) * 30.f + 890.1f;

            m_pTransformCom->Set_PosF3(pos);

            _float3 lookat = m_pTransformCom->Get_PosF3();

            lookat.x += -1.f * sin(XMConvertToRadians(m_Angle)) * 30.f;
            lookat.z += cos(XMConvertToRadians(m_Angle)) * 30.f;

            _vector vlookat = XMLoadFloat3(&lookat);

            m_pTransformCom->LookAt(vlookat);
        }
        else if (g_iCurrentLevel == LEVEL_RATTAN)
        {
            _float3 pos = m_pTransformCom->Get_PosF3();

            m_Angle += (_float)_dTimeDelta * 30.f;

            if (m_Angle >= 360.f)
                m_Angle = 0.f;

            pos.x = cos(XMConvertToRadians(m_Angle)) * 30.f + 39.4f;
            pos.z = sin(XMConvertToRadians(m_Angle)) * 30.f + 0.2f;

            m_pTransformCom->Set_PosF3(pos);

            _float3 lookat = m_pTransformCom->Get_PosF3();

            lookat.x += -1.f * sin(XMConvertToRadians(m_Angle)) * 30.f;
            lookat.z += cos(XMConvertToRadians(m_Angle)) * 30.f;

            _vector vlookat = XMLoadFloat3(&lookat);

            m_pTransformCom->LookAt(vlookat);
        }
    }
    if (m_iType == ANIMAL_DOG)
    {
        if ((m_Length.x <= 20.f) || pGameInstance->Get_DIKeyUp(DIK_5))
            CurrentState = ALERT;
    }

    if (m_iType == ANIMAL_MOUSE)
    {
        _float3 pos = m_pTransformCom->Get_PosF3();

        m_Angle += (_float)_dTimeDelta * 40.f;

        if (m_Angle >= 360.f)
            m_Angle = 0.f;

        pos.x = cos(XMConvertToRadians(m_Angle)) * 20.f + -522.6f;
        pos.z = sin(XMConvertToRadians(m_Angle)) * 20.f + 332.2f;

        m_pTransformCom->Set_PosF3(pos);

        _float3 lookat = m_pTransformCom->Get_PosF3();

        lookat.x += -1.f * sin(XMConvertToRadians(m_Angle)) * 20.f;
        lookat.z += cos(XMConvertToRadians(m_Angle)) * 20.f;

        _vector vlookat = XMLoadFloat3(&lookat);

        m_pTransformCom->LookAt(vlookat);

    }

    if (m_iType == ANIMAL_CHICKEN)
    {
        Chickentime += _dTimeDelta;

        if (Chickentime >= 3)
            Run = true;

        if (Chickentime >= 6 && Run == true)
        {
            Run = false;
            Chickentime = 0.0;
        }
       
        if(Run == false)
            CurrentState = EAT;
       
        if (Run == true)
        {
            CurrentState = WALK;

            _float3 pos = m_pTransformCom->Get_PosF3();

            m_Angle += (_float)_dTimeDelta * 40.f;

            if (m_Angle >= 360.f)
                m_Angle = 0.f;

            pos.x = cos(XMConvertToRadians(m_Angle)) * 10.f + -2.4f;
            pos.z = sin(XMConvertToRadians(m_Angle)) * 10.f + 16.f;

            m_pTransformCom->Set_PosF3(pos);

            _float3 lookat = m_pTransformCom->Get_PosF3();

            lookat.x += -1.f * sin(XMConvertToRadians(m_Angle)) * 10.f;
            lookat.z += cos(XMConvertToRadians(m_Angle)) * 10.f;

            _vector vlookat = XMLoadFloat3(&lookat);

            m_pTransformCom->LookAt(vlookat);
        }
        
    }
	
    RELEASE_INSTANCE(CGameInstance);
}

void CAnimal::Change_State()
{
    
	
    if (PrevState == CurrentState)
        return;
    PrevState = CurrentState;
	
    switch (CurrentState)
    {
    case Client::CAnimal::EAT:
    {
     m_pAnimator->Input_Trigger("Eat");
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
        pGameInstance->Play_SoundW(TEXT("UI_Chicken"), 0.4f);
    RELEASE_INSTANCE(CGameInstance);
    }
        break;
    case Client::CAnimal::ALERT:
    { m_pAnimator->Input_Trigger("Alert");
    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    pGameInstance->Play_SoundW(TEXT("UI_Dog"), 0.4f);
    RELEASE_INSTANCE(CGameInstance);
    }
    break;
    case Client::CAnimal::RUN:
        m_pAnimator->Input_Trigger("Run");
        break;
    case Client::CAnimal::WALK:
        m_pAnimator->Input_Trigger("Walk");
        break;
    case Client::CAnimal::STAND:
        m_pAnimator->Input_Trigger("Stand");
        break;
    case Client::CAnimal::STATE_END:
        break;
    }
  
}

HRESULT CAnimal::SetUp_ConstantTable()
{
    if (FAILED(__super::SetUp_ConstantTable()))
        return E_FAIL;

    _bool bMotionBlur = false;
    if (FAILED(m_pShaderCom->Set_RawValue("g_ObjectMotionBlur", &bMotionBlur, sizeof(_bool))))
        return E_FAIL;

    return S_OK;
}

HRESULT CAnimal::SetUp_Components()
{
   if (FAILED(__super::SetUp_Components()))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL, (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
    
    if (g_iCurrentLevel == LEVEL_STORY)
    {
        CModel* pDeerModel = nullptr;
        pDeerModel = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_DEER, (CComponent**)&pDeerModel);
        m_ModelList.push_back(pDeerModel);

        CModel* pButterflyModel = nullptr;
        pButterflyModel = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_BUTTERFLY, (CComponent**)&pButterflyModel);
        m_ModelList.push_back(pButterflyModel);

        CModel* pCatModel = nullptr;
        pCatModel = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_CAT, (CComponent**)&pCatModel);
        m_ModelList.push_back(pCatModel);

        CModel* pBirdModel = nullptr;
        pBirdModel = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_BIRD, (CComponent**)&pBirdModel);
        m_ModelList.push_back(pBirdModel);

        CModel* pDogModel = nullptr;
        pDogModel = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_DOG, (CComponent**)&pDogModel);
        m_ModelList.push_back(pDogModel);

        CModel* pMouseModel = nullptr;
        pMouseModel = (CModel*)pGameInstance->Clone_Component(LEVEL_STORY, PROTO_COMP_MODEL_MOUSE, (CComponent**)&pMouseModel);
        m_ModelList.push_back(pMouseModel);
        //=====================================================

        CAnimator::ANIMATORDESC tDesc;
        tDesc.pModel = pDeerModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"Deer";
        CAnimator* pDeerAnimator = nullptr;
        pDeerAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pDeerAnimator->Set_Model(m_ModelList[ANIMAL_DEER]);
        m_AnimatorList.push_back(pDeerAnimator);


        tDesc.pModel = pButterflyModel;
        tDesc.strFilePath = L"Butterfly";
        CAnimator* pButterflyAnimator = nullptr;
        pButterflyAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pButterflyAnimator->Set_Model(m_ModelList[ANIMAL_BUTTERFLY]);
        m_AnimatorList.push_back(pButterflyAnimator);

        tDesc.pModel = pCatModel;
        tDesc.strFilePath = L"Cat";
        CAnimator* pCatAnimator = nullptr;
        pCatAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pCatAnimator->Set_Model(m_ModelList[ANIMAL_CAT]);
        m_AnimatorList.push_back(pCatAnimator);

        tDesc.pModel = pBirdModel;
        tDesc.strFilePath = L"Bird";
        CAnimator* pBirdAnimator = nullptr;
        pBirdAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pBirdAnimator->Set_Model(m_ModelList[ANIMAL_BIRD]);
        m_AnimatorList.push_back(pBirdAnimator);

        tDesc.pModel = pDogModel;
        tDesc.strFilePath = L"Dog";
        CAnimator* pDogAnimator = nullptr;
        pDogAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pDogAnimator->Set_Model(m_ModelList[ANIMAL_DOG]);
        m_AnimatorList.push_back(pDogAnimator);

        tDesc.pModel = pMouseModel;
        tDesc.strFilePath = L"Mouse";
        CAnimator* pMouseAnimator = nullptr;
        pMouseAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pMouseAnimator->Set_Model(m_ModelList[ANIMAL_MOUSE]);
        m_AnimatorList.push_back(pMouseAnimator);
    }

    if (g_iCurrentLevel == LEVEL_RATTAN)
    {
        CModel* pDeerModel = nullptr;
        pDeerModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_DEER, (CComponent**)&pDeerModel);
        m_ModelList.push_back(pDeerModel);

        CModel* pButterflyModel = nullptr;
        pButterflyModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_BUTTERFLY, (CComponent**)&pButterflyModel);
        m_ModelList.push_back(pButterflyModel);

        CModel* pCatModel = nullptr;
        pCatModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_CAT, (CComponent**)&pCatModel);
        m_ModelList.push_back(pCatModel);

        CModel* pBirdModel = nullptr;
        pBirdModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_BIRD, (CComponent**)&pBirdModel);
        m_ModelList.push_back(pBirdModel);

        CModel* pDogModel = nullptr;
        pDogModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_DOG, (CComponent**)&pDogModel);
        m_ModelList.push_back(pDogModel);

        CModel* pMouseModel = nullptr;
        pMouseModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_MOUSE, (CComponent**)&pMouseModel);
        m_ModelList.push_back(pMouseModel);

        CModel* pChickenModel = nullptr;
        pChickenModel = (CModel*)pGameInstance->Clone_Component(LEVEL_RATTAN, PROTO_COMP_MODEL_CHICKEN, (CComponent**)&pChickenModel);
        m_ModelList.push_back(pChickenModel);
        //=====================================================

        CAnimator::ANIMATORDESC tDesc;
        tDesc.pModel = pDeerModel;
        tDesc.pTransform = m_pTransformCom;
        tDesc.strFilePath = L"Deer";
        CAnimator* pDeerAnimator = nullptr;
        pDeerAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pDeerAnimator->Set_Model(m_ModelList[ANIMAL_DEER]);
        m_AnimatorList.push_back(pDeerAnimator);


        tDesc.pModel = pButterflyModel;
        tDesc.strFilePath = L"Butterfly";
        CAnimator* pButterflyAnimator = nullptr;
        pButterflyAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pButterflyAnimator->Set_Model(m_ModelList[ANIMAL_BUTTERFLY]);
        m_AnimatorList.push_back(pButterflyAnimator);

        tDesc.pModel = pCatModel;
        tDesc.strFilePath = L"Cat";
        CAnimator* pCatAnimator = nullptr;
        pCatAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pCatAnimator->Set_Model(m_ModelList[ANIMAL_CAT]);
        m_AnimatorList.push_back(pCatAnimator);

        tDesc.pModel = pBirdModel;
        tDesc.strFilePath = L"Bird";
        CAnimator* pBirdAnimator = nullptr;
        pBirdAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pBirdAnimator->Set_Model(m_ModelList[ANIMAL_BIRD]);
        m_AnimatorList.push_back(pBirdAnimator);

        tDesc.pModel = pDogModel;
        tDesc.strFilePath = L"Dog";
        CAnimator* pDogAnimator = nullptr;
        pDogAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pDogAnimator->Set_Model(m_ModelList[ANIMAL_DOG]);
        m_AnimatorList.push_back(pDogAnimator);

        tDesc.pModel = pMouseModel;
        tDesc.strFilePath = L"Mouse";
        CAnimator* pMouseAnimator = nullptr;
        pMouseAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pMouseAnimator->Set_Model(m_ModelList[ANIMAL_MOUSE]);
        m_AnimatorList.push_back(pMouseAnimator);

        tDesc.pModel = pChickenModel;
        tDesc.strFilePath = L"Chicken";
        CAnimator* pChickenAnimator = nullptr;
        pChickenAnimator = (CAnimator*)pGameInstance->Clone_Component(LEVEL_STATIC, PROTO_COMP_ANIMATOR, (CComponent**)&tDesc);
        pChickenAnimator->Set_Model(m_ModelList[ANIMAL_CHICKEN]);
        m_AnimatorList.push_back(pChickenAnimator);
    }
	
    RELEASE_INSTANCE(CGameInstance);
	
    return S_OK;
}

CAnimal* CAnimal::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
    CAnimal* pInstance = new CAnimal(_pDevice, _pContext);
    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CAnimal::Create To Failed");
#endif
    }
    return pInstance;
}

CGameObject* CAnimal::Clone(void* pArg)
{
    CAnimal* pInstance = new CAnimal(*this);
    if (FAILED(pInstance->NativeConstruct_Clone(pArg)))
    {
        Safe_Release(pInstance);
#ifdef _DEBUG
        MSG_BOX("CAnimal::Clone To Failed");
#endif
    }
    return pInstance;
}

void CAnimal::Free()
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

    Safe_Release(m_pAllAnimModel);

    Safe_Release(m_pModel);
    Safe_Release(m_pAnimator);

    __super::Free();
}
