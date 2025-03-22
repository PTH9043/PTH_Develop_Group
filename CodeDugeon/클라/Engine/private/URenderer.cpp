#include "EngineDefine.h"
#include "URenderer.h"
#include "UPipeLine.h"
#include "USceneManager.h"
#include "UShader.h"
#include "UGameInstance.h"
#include "URenderTargetManager.h"
#include "UShaderConstantBuffer.h"
#include "UTransform.h"
#include "UGraphicDevice.h"
#include "UVIBufferRect.h"
#include "URenderTargetGroup.h"
#include "UTransform.h"
#include "UDefferedCamera.h"
#include "USmallDefferedCamera.h"
#include "UShadowCamera.h"
#include "URenderTargetManager.h"
#include "UCharacter.h"
#include "UGlobalConstantBuffer.h"
#include "UGpuCommand.h"
#include "UPawn.h"
#include "UMethod.h"
#include "UCommand.h"


URenderer::URenderer(CSHPTRREF <UDevice> _spDevice, CSHPTRREF<UCommand> _spCommand,
    CSHPTRREF<UGraphicDevice> _spGraphicDevice, CSHPTRREF<UPipeLine> _spPipeLine,
    CSHPTRREF<USceneManager> _spSceneManager, CSHPTRREF<URenderTargetManager> _spRenderTargetManager,
    CSHPTRREF<UComputeManager> _spComputeManager) :
    UComponent(_spDevice),
    m_spTransformConstantBuffer{ nullptr },
    m_fGrobalDeltaTime{0},
    m_spDefferedCamera{ nullptr },
    m_spSmallDefferedCamera{ nullptr },
    m_spShadowCamera{ nullptr },
    m_stFinalRenderTransformParam{},
    m_stSmallRenderTransformParam{},
    m_spGraphicDevice{ _spGraphicDevice },
    m_spPipeLine{ _spPipeLine }, m_spSceneManager{ _spSceneManager },
    m_spRenderTargetManager{ _spRenderTargetManager },
    m_spComputeManager{ _spComputeManager },
    m_spVIBufferPlane{ nullptr },
    m_sNonAlphaBlendIndex{ 0 },
    m_bTurnShader{0},
    m_spTurnShaderConstantBuffer{nullptr},
    m_spCastingCommand{ static_pointer_cast<UCommand>(_spGraphicDevice->GetGpuCommand()) }
#ifdef _USE_DEBUGGING
    , m_stRenderDebugging{}
#endif
{
}

void URenderer::Free()
{
}

HRESULT URenderer::NativeConstruct()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
  /*  {
        spGameInstance->GetGlobalConstantBuffer(GLOBAL_GLOBALDATA, m_spGlobalBuffer);
        spGameInstance->CopyToMaterialShaderParam(REF_OUT m_stGlobalParam);
    }*/
    // Rendering
    {
        m_spVIBufferPlane = static_pointer_cast<UVIBufferRect>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERRECT));
    }
    {
        // Add Shader 
        {
            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_FINALDEFFEREDSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_FINALDEFFEREDSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_BLENDDEFFEREDSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_BLENDDEFFEREDSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_HDRSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_HDRSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_HDRTWOSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_HDRTWOSHADER))));
           
            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_GRAYSCALESHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_GRAYSCALESHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_VERTICALBLURSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_VERTICALBLURSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_HORIZONTALBLURSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_HORIZONTALBLURSHADER))));
            
            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_BLOOMSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_BLOOMSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_DOWNSAMPLINGSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_DOWNSAMPLINGSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_DOWNSAMPLINGTWOSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_DOWNSAMPLINGTWOSHADER))));
               
            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_UPSAMPLINGSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_UPSAMPLINGSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_UPSAMPLINGGRAYSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_UPSAMPLINGGRAYSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_MOTIONBLURSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_MOTIONBLURSHADER))));

            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_MIDDLERENDERSCREENSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_MIDDLERENDERSCREENSHADER))));
#ifdef _USE_DEBUGGING
            m_ShaderObjects.insert(std::pair<_wstring, SHPTR<UShader>>(PROTO_RES_DEBUG2DTARGETSHADER, static_pointer_cast<UShader>(
                spGameInstance->CloneResource(PROTO_RES_DEBUG2DTARGETSHADER))));
#endif
        }
    }
    {
        m_stFinalRenderTransformParam.mWorldMatrix._11 = spGameInstance->GetD3DViewport().Width;
        m_stFinalRenderTransformParam.mWorldMatrix._22 = spGameInstance->GetD3DViewport().Height;
        m_stFinalRenderTransformParam.mWorldMatrix = m_stFinalRenderTransformParam.mWorldMatrix.Transpose();
        m_stSmallRenderTransformParam.mWorldMatrix._11 = spGameInstance->GetD3DViewport().Width/2;
        m_stSmallRenderTransformParam.mWorldMatrix._22 = spGameInstance->GetD3DViewport().Height/2;
        m_stSmallRenderTransformParam.mWorldMatrix = m_stSmallRenderTransformParam.mWorldMatrix.Transpose();

        {

            // Create Shadow Camera (깊이 추출용 카메라)
            UCamera::CAMDESC tDesc;
            tDesc.stCamProj = UCamera::CAMPROJ(UCamera::PROJECTION_TYPE::PERSPECTIVE, _float3(0.f, 0.f, 0.f),
                _float3(0.f, 0.f, 0.f),
                DirectX::XMConvertToRadians(60.0f), spGameInstance->GetD3DViewport().Width*4,
                spGameInstance->GetD3DViewport().Height*4,0.05f,100.f,1.f);
            tDesc.stCamValue = UCamera::CAMVALUE(5.f, DirectX::XMConvertToRadians(90.f));
            tDesc.eCamType = CAMERATYPE::SHADOWLIGHT;

            VOIDDATAS vecDatas;
            vecDatas.push_back(&tDesc);

            m_spShadowCamera = static_pointer_cast<UShadowCamera>(spGameInstance->CloneActorAdd(
                PROTO_ACTOR_SHADOWCAMERA, vecDatas));
            m_spShadowCamera->SetShadowCamViewportInfo(0, 0, static_cast<long>(spGameInstance->GetD3DViewport().Width *4),
                static_cast<long>(spGameInstance->GetD3DViewport().Height * 4), 0.0f, 1.0f);
            m_spShadowCamera->SetShadowCamRectInfo(0, 0, static_cast<long>(spGameInstance->GetD3DViewport().Width * 4),
                static_cast<long>(spGameInstance->GetD3DViewport().Height * 4));
        }
        {

            // Create Deffered Camera (디퍼드용 카메라)
            UCamera::CAMDESC tDesc;
            tDesc.stCamProj = UCamera::CAMPROJ(UCamera::PROJECTION_TYPE::ORTHOGRAPHIC, _float3(0.f, 0.f, 0.f),
                _float3(0.f, 0.f, 0.f),
                DirectX::XMConvertToRadians(60.0f), spGameInstance->GetD3DViewport().Width,
                spGameInstance->GetD3DViewport().Height);
            tDesc.stCamValue = UCamera::CAMVALUE(5.f, DirectX::XMConvertToRadians(90.f));
            tDesc.eCamType = CAMERATYPE::DEFFERED;

            VOIDDATAS vecDatas;
            vecDatas.push_back(&tDesc);

            m_spDefferedCamera = static_pointer_cast<UDefferedCamera>(spGameInstance->CloneActorAdd(
                PROTO_ACTOR_DEFFEREDCAMERA, vecDatas));
        }
        {

            // Create Small Deffered Camera (다운샘플링 용디퍼드용 카메라)
            UCamera::CAMDESC tDesc;
            tDesc.stCamProj = UCamera::CAMPROJ(UCamera::PROJECTION_TYPE::ORTHOGRAPHIC, _float3(0.f, 0.f, 0.f),
                _float3(0.f, 0.f, 0.f),
                DirectX::XMConvertToRadians(60.0f), spGameInstance->GetD3DViewport().Width,
                spGameInstance->GetD3DViewport().Height);
            tDesc.stCamValue = UCamera::CAMVALUE(5.f, DirectX::XMConvertToRadians(90.f));
            tDesc.eCamType = CAMERATYPE::SMALL_DEFFERED;

            VOIDDATAS vecDatas;
            vecDatas.push_back(&tDesc);

            m_spSmallDefferedCamera = static_pointer_cast<USmallDefferedCamera>(spGameInstance->CloneActorAdd(
                PROTO_ACTOR_SMALL_DEFFEREDCAMERA, vecDatas));
        }
        {
            m_spTurnShaderConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::TURNSHADERBOOL, static_cast<_int>(sizeof(DRAWSHADERBUFFER)));
        }
        m_spShadowCamera->GetTransform()->SetPos(_float3(-875.66, - 135.805, 852.));
        m_spShadowCamera->GetTransform()->LookAt(_float3(-629.466, -135.805,991.156)); //y를 확올리기
      
        m_stFinalRenderTransformParam.iCamIndex = m_spDefferedCamera->GetCamID();
        m_stSmallRenderTransformParam.iCamIndex = m_spSmallDefferedCamera->GetCamID();
            
        // 이미 만들어진 Shader ConstnatBuffer를 가져옴
        spGameInstance->GetPreAllocatedConstantBuffer(PREALLOCATED_TRANSFORM, m_spTransformConstantBuffer);
    }
    // Initailize 
        return S_OK;
}

HRESULT URenderer::NativeConstructClone(const VOIDDATAS& _vectDatas)
{
    return __super::NativeConstructClone(_vectDatas);
}

HRESULT URenderer::AddRenderGroup(const RENDERID _eID, CSHPTRREF<UShader> _spShader, CSHPTRREF<UPawn> _spPawn)
{
    assert(_spPawn && _spShader);
    // 쉐이더 이름 있는지 찾고
    const auto& findIter = m_arrActiveDrawRenderList[_eID].find(_spShader->GetShaderDesc().wstrShaderName);
    // 찾아서 저 쉐이더 이름이 없으면
    if (findIter == m_arrActiveDrawRenderList[_eID].end())
    {
        LIST<SHPTR<UPawn>> PawnList;
        PawnList.push_back(_spPawn);
        m_arrActiveDrawRenderList[_eID].emplace(std::pair<_wstring, LIST<SHPTR<UPawn>>>(_spShader->GetShaderDesc().wstrShaderName, PawnList));
    }
    else
    {
        findIter->second.push_back(_spPawn);
    }
    return S_OK;
}

#ifdef _USE_DEBUGGING
HRESULT URenderer::AddDebugRenderGroup(const DEBUGRENDERID _eID, CSHPTRREF<UShader> _spShader, CSHPTRREF<UPawn> _spPawn)
{
    RETURN_CHECK(false == m_stRenderDebugging.isShowRenderTarget, E_FAIL);
    RETURN_CHECK(nullptr == _spPawn, E_FAIL);

    const auto& findIter = m_arrDrawDebugRenderList[_eID].find(_spShader->GetShaderDesc().wstrShaderName);
    if (findIter == m_arrDrawDebugRenderList[_eID].end())
    {
        LIST<SHPTR<UPawn>> PawnList;
        PawnList.push_back(_spPawn);
        m_arrDrawDebugRenderList[_eID].emplace(std::pair<_wstring, LIST<SHPTR<UPawn>>>(_spShader->GetShaderDesc().wstrShaderName, PawnList));
        // 쉐이더 이름별로 
        // Pawn List 
        // 
    }
    else
    {
        findIter->second.push_back(_spPawn);
    }

    return S_OK;
}
#endif

void URenderer::Tick(const _double& _dTimeDelta)
{

    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    m_fGrobalDeltaTime += static_cast<_float>(_dTimeDelta);

    

    if (m_bTurnShader.m_bTurnDie) {      
        m_bTurnShader.m_fDieTime += _dTimeDelta;
    }
    else {        
        m_bTurnShader.m_fDieTime = 0;
    }

    if (m_bTurnShader.m_bGameStart) {
        m_bTurnShader.m_fGameStartTime += _dTimeDelta;
    }

    if (m_bTurnShader.m_bTurnAbility) {
        m_bTurnShader.m_fAbilityTime += _dTimeDelta;
        if (m_bTurnShader.m_fAbilityTime > 5.f) {
            m_bTurnShader.m_bTurnAbility = false;
        }
    }
    else {
        m_bTurnShader.m_fAbilityTime = 0;
    }

    if (m_bTurnShader.m_bTurnHit) {
        m_bTurnShader.m_fHitTime += _dTimeDelta;
        if (m_bTurnShader.m_fHitTime > 3.f) {
            m_bTurnShader.m_bTurnHit = false;
        }
    }
    else {
        m_bTurnShader.m_fHitTime = 0;
    }
   
    
        /*SHPTR<UTransform> MainTransform=    spGameInstance->GetMainCameraTransform();
        m_spShadowCamera->GetTransform()->SetLook(MainTransform->GetLook());
        _float3 MainPos = spGameInstance->GetMainCamPosition()-10*MainTransform->GetLook();
        m_spShadowCamera->GetTransform()->SetPos(MainPos);*/
}

HRESULT URenderer::Render()
{
    m_spPipeLine->UpdateViewProjMatrix(m_fGrobalDeltaTime);
    m_spPipeLine->BindViewProjMatrix(m_spCastingCommand);
    BindGrobalBuffer();
    // ============== Bind Static Buffer =============
  
    // Render 
  //  RenderRTs();
    RenderPriority();
    RenderPosNormal();
    RenderShadowDepth();
    RenderNonAlphaBlend();
    RenderLights();
    RenderNonLight();
    RenderBlend();
    RenderAlphaBlend();
    RenderDistortion();
    RenderMotionBlur();
    RenderHDR();
    RenderHDRTWO();
    RenderGrayScale();
   DownSample();
   DownSample2();
   RenderHorizontalBlur();
   RenderVerticalBlur();
   UpSample();
    RenderBloom();
    RenderEnd();
    Render2DUIPriority();
    Render2DUIMiddle();
    Render2DUILast();
    Render3DUI();
    //원상복구하려면 Blur두개 키고 DownSample 2개를 꺼야함. 그리고 Upsample입력 텍스쳐를 BlurResult로
//#ifdef _USE_DEBUGGING
//    
//    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance); 
//    RenderDebug();
//    m_spRenderTargetManager->RenderDebugObjects(FrameReadyDrawLast(PROTO_RES_DEBUG2DTARGETSHADER), m_spVIBufferPlane,
//        m_spCastingCommand, SRV_REGISTER::T0);
//  /*  _float3 pos = spGameInstance->GetCurrPlayer()->GetTransform()->GetPos();
//    _float3 vec = spGameInstance->GetMainCameraTransform()->GetLook();
//
//
//    m_spShadowCamera->GetTransform()->SetPos(_float3(pos.x,pos.y+20,pos.z));
//    m_spShadowCamera->GetTransform()->SetLook(pos);*/
//    m_spShadowCamera->GetTransform()->SetPos(_float3(-883.653748, -125.2615, 847.1639));
//    m_spShadowCamera->GetTransform()->LookAt(_float3(-859.759, -154.805, 860.509)); //y를 확올리기
//
//    if (spGameInstance->GetDIKeyPressing(DIK_F1)) {
//      }
//    if (spGameInstance->GetDIKeyDown(DIK_F2)) {
//        TurnDie++;
//    }
//
//   
//    
//    if (TurnDie % 2 == 1) {
//        TurnOnDieEffect();
//    }
//    else {
//        TurnOffDieEffect();
//    }
//   
//#endif
    return S_OK;
}

void URenderer::ClearRenderingData()
{
    for (auto& iter : m_arrActiveDrawRenderList)
    {
        iter.clear();
    }
#ifdef  _USE_DEBUGGING
    for (auto& iter : m_arrDrawDebugRenderList)
    {
        iter.clear();
    }
#endif //  _USE_DEBUGGING
}

void URenderer::ClearAllData()
{
    m_ShaderObjects.clear();
    m_spTransformConstantBuffer.reset();
    m_spDefferedCamera.reset();
    m_spShadowCamera.reset();

    m_spGraphicDevice.reset();
    m_spPipeLine.reset();
    m_spSceneManager.reset();
    m_spRenderTargetManager.reset();
    m_spComputeManager.reset();
    m_spVIBufferPlane.reset();

    ClearRenderingData();
}

void URenderer::RenderOtherCamera()
{
    RenderNonAlphaBlend();
    RenderLights();
    RenderNonLight();
    RenderAlphaBlend();
    RenderDistortion();
    RenderHDR();
}

void URenderer::BindDefferedTransform(SHPTR<UShader> _spShader)
{
    RETURN_CHECK(nullptr == _spShader, ;);
    _spShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
}

void URenderer::RenderRTs()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    _uint iIndex{ 0 }, iMirror{ 0 };

    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_TARGETS])
    {
        // Render
        for (auto& Mirror : iter.second)
        {
            if (0 == iIndex)
                iMirror = (_uint)(GRAPHICRENDEROBJECT_TYPE::MIRROR_1);
            else
                iMirror = (_uint)(GRAPHICRENDEROBJECT_TYPE::MIRROR_2);

           
            m_spPipeLine->BindViewProjMatrix(m_spCastingCommand);
            BindGrobalBuffer();

            Mirror->Render(m_spCastingCommand, m_spGraphicDevice->GetTableDescriptor());
          
            iIndex = (iIndex + 1) % 2;
        }
    }

    spGameInstance->ChangeRenderCamera(MAIN_CAMERA_ID);
}

void URenderer::RenderPriority()
{
   
}

void URenderer::RenderPosNormal()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    SHPTR<URenderTargetGroup> spRenderTargetGroup0{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::OUTLINE_POS_NOR_FORABILITY) };
    {
        spRenderTargetGroup0->WaitResourceToTarget(m_spCastingCommand);
        spRenderTargetGroup0->ClearRenderTargetView(m_spCastingCommand);
        spRenderTargetGroup0->OmSetRenderTargets(m_spCastingCommand);
    }

    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_NORPOS_FORABILITY])
    {
        RenderOutlineObject(iter.first, iter.second, false); //false가 NorPos true가 depth
    }
    spRenderTargetGroup0->WaitTargetToResource(m_spCastingCommand);

    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::OUTLINE_POS_NOR) };
    {
        spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
        spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
        spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    }
  
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_NORPOS])
    {
        RenderOutlineObject(iter.first, iter.second,false); //false가 NorPos true가 depth
    }
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);

    SHPTR<URenderTargetGroup> spRenderTargetGroup1{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::DEPTH_RECORD) };
    {
        spRenderTargetGroup1->WaitResourceToTarget(m_spCastingCommand);
        spRenderTargetGroup1->ClearRenderTargetView(m_spCastingCommand);
        spRenderTargetGroup1->OmSetRenderTargets(m_spCastingCommand);
    }

    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_DEPTHRECORD])
    {
        RenderOutlineObject(iter.first, iter.second, true); //false가 NorPos true가 depth
    }
    spRenderTargetGroup1->WaitTargetToResource(m_spCastingCommand);

    
}

void URenderer::RenderShadowDepth()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
  
   
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::SHADOW_MAP) };
    {
        spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
        spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
        spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    }
  spGameInstance->SetTemporaryViewPort(m_spShadowCamera->GetShadowCamViewPort(), m_spShadowCamera->GetShadowCamRect());

    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_SHADOW])
    {
        RenderShadowObject(iter.first, iter.second);
    }
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);

   spGameInstance->SetDefaultViewPort();
}

void URenderer::RenderNonAlphaBlend()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED) };
   
    
    {
        spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
        spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
        spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    }
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_NONALPHA_PRIORITY])
    {
        RenderObject(iter.first, iter.second);
    }
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_NONALPHA_FIRST])
    {
        RenderObject(iter.first, iter.second);
    }
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_NONALPHA_MIDDLE])
    {
        RenderObject(iter.first, iter.second);
    }
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_NONALPHA_LAST])
    {
        RenderObject(iter.first, iter.second);
    }
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);

}

void URenderer::RenderLights()
{
    // Render Light
    m_spSceneManager->RenderLight(m_spPipeLine, m_spRenderTargetManager, m_stFinalRenderTransformParam);
}

void URenderer::RenderBlend()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
  
    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLEND_DEFFERED) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    // Bind Shader 
    SHPTR<UShader> spShader = FindShader(PROTO_RES_BLENDDEFFEREDSHADER);
    spShader->SettingPipeLineState(m_spCastingCommand);
    spShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED);
        spShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::NONALPHA_DIFFUSE_DEFFERED));
        spShader->BindSRVBuffer(SRV_REGISTER::T4, spNonAlpha->GetRenderTargetTexture(RTOBJID::NONALPHA_GLOW_DEFFERED));
        spShader->BindSRVBuffer(SRV_REGISTER::T5, spNonAlpha->GetRenderTargetTexture(RTOBJID::NONALPHA_SPECULAR_DEFFERED));
    }
    {
        SHPTR<URenderTargetGroup> spLightGroup = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::LIGHTSHADE_DEFFERED);
        spShader->BindSRVBuffer(SRV_REGISTER::T1, spLightGroup->GetRenderTargetTexture(RTOBJID::LIGHTSHADE_SHADE_DEFFERED));
        spShader->BindSRVBuffer(SRV_REGISTER::T2, spLightGroup->GetRenderTargetTexture(RTOBJID::LIGHTSHADE_SPECULAR_DEFFERED));
        spShader->BindSRVBuffer(SRV_REGISTER::T3, spLightGroup->GetRenderTargetTexture(RTOBJID::LIGHTSHADE_AMBIENT_DEFFERED));
    }
   /* {
        spShader->BindSRVBuffer(SRV_REGISTER::T6, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::DEPTH_RECORD,
                RTOBJID::DEPTH_RECORD));
        spShader->BindSRVBuffer(SRV_REGISTER::T7, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::OUTLINE_POS_NOR,
                RTOBJID::OUTLINE_DEPTH_POS));
    }*/
    m_spVIBufferPlane->Render(spShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);
  
}


void URenderer::RenderNonLight()
{
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_NOLIGHT])
    {
        RenderObject(iter.first, iter.second);
    }
}

void URenderer::RenderAlphaBlend()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
     // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::ALPHA_DEFFERED) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    {
        for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_ALPHA])
        {
            RenderObject(iter.first, iter.second);
        }
    }
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);
 
}

void URenderer::RenderDistortion()
{

}

void URenderer::RenderMotionBlur()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    // Set Render Tareget
    {
        SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::MOTIONBLUR_DEFFERED) };
        spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
        spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
        spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
        // Bind Shader 
        SHPTR<UShader> spShader = FindShader(PROTO_RES_MOTIONBLURSHADER);
        spShader->SettingPipeLineState(m_spCastingCommand);
        spShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
        spShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
        {
            SHPTR<URenderTargetGroup> spBlend = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLEND_DEFFERED);
            spShader->BindSRVBuffer(SRV_REGISTER::T0, spBlend->GetRenderTargetTexture(RTOBJID::BLEND_SCREEN_DEFFERED));

            SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED);
            spShader->BindSRVBuffer(SRV_REGISTER::T1, spNonAlpha->GetRenderTargetTexture(RTOBJID::NONALPHA_VELOCITY_DEFFERED));
        }

        m_spVIBufferPlane->Render(spShader, m_spCastingCommand);
        spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);
    }

    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLEND_DEFFERED) };
    {
        spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
        spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);

        // Bind Shader 
        SHPTR<UShader> spShader = FindShader(PROTO_RES_MIDDLERENDERSCREENSHADER);
        spShader->SettingPipeLineState(m_spCastingCommand);
        spShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
        spShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
        {

            SHPTR<URenderTargetGroup> spMotionBlur = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::MOTIONBLUR_DEFFERED);
            SHPTR<URenderTargetGroup> spBlend = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLEND_DEFFERED);
            spShader->BindSRVBuffer(SRV_REGISTER::T0, spMotionBlur->GetRenderTargetTexture(RTOBJID::MOTIONBLUR_DIFFUSE_DEFFERED));
            spShader->BindSRVBuffer(SRV_REGISTER::T1, spMotionBlur->GetRenderTargetTexture(RTOBJID::MOTIONBLUR_JUDGECOLOR_DEFFERED));
        //    spShader->BindSRVBuffer(SRV_REGISTER::T2, spBlend->GetRenderTargetTexture(RTOBJID::BLEND_SCREEN_DEFFERED));
        }

        m_spVIBufferPlane->Render(spShader, m_spCastingCommand);
        spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);
    }
}

void URenderer::Render3DUI()
{
}

void URenderer::Render2DUIPriority()
{
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_2DUIPRIORITY])
    {
        RenderObject(iter.first, iter.second);
    }
}


void URenderer::Render2DUIMiddle()
{
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_2DUIMIDDLE])
    {
        RenderObject(iter.first, iter.second);
    }
}

void URenderer::Render2DUILast()
{
    for (auto& iter : m_arrActiveDrawRenderList[RENDERID::RI_2DUILAST])
    {
        RenderObject(iter.first, iter.second);
    }
}

void URenderer::RenderHDR()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::HDR) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    // Bind Shader 
    SHPTR<UShader> spShader = FindShader(PROTO_RES_HDRSHADER);
    spShader->SettingPipeLineState(m_spCastingCommand);
    spShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLEND_DEFFERED);
        spShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::BLEND_SCREEN_DEFFERED));
    }
   
    m_spVIBufferPlane->Render(spShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);
}

void URenderer::RenderHDRTWO()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::HDRTWO) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    // Bind Shader 
    SHPTR<UShader> spShader = FindShader(PROTO_RES_HDRTWOSHADER);
    spShader->SettingPipeLineState(m_spCastingCommand);
    spShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLEND_DEFFERED);
        spShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::BLEND_SCREEN_DEFFERED));
    }

    m_spVIBufferPlane->Render(spShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);


}

void URenderer::RenderGrayScale()
{
    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::GRAY_SCALE) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    // Bind Shader 
    SHPTR<UShader> spShader = FindShader(PROTO_RES_GRAYSCALESHADER);
    spShader->SettingPipeLineState(m_spCastingCommand);
    spShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLEND_DEFFERED);
        spShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::BLEND_SCREEN_DEFFERED));
    }

    m_spVIBufferPlane->Render(spShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);

}

void URenderer::DownSample()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::DOWNSAMPLE) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);

    D3D12_VIEWPORT _viewPort;
    _viewPort.TopLeftX = 0;
    _viewPort.TopLeftY = 0;
    _viewPort.MinDepth = 0;
    _viewPort.MaxDepth = 1.0;
    _viewPort.Height = spGameInstance->GetD3DViewport().Height / 4.f;
    _viewPort.Width = spGameInstance->GetD3DViewport().Width / 4.f;
    D3D12_RECT _rect;
    _rect.left = 0;
    _rect.top = 0;
    _rect.right = static_cast<LONG>(spGameInstance->GetD3DViewport().Width / 4);
    _rect.bottom = static_cast<LONG>(spGameInstance->GetD3DViewport().Height / 4);

    spGameInstance->SetTemporaryViewPort(_viewPort, _rect);
    // Bind Shader 
    SHPTR<UShader> spDownSamplingShader = FindShader(PROTO_RES_DOWNSAMPLINGSHADER);
    spDownSamplingShader->SettingPipeLineState(m_spCastingCommand);
    spDownSamplingShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spDownSamplingShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED);
        spDownSamplingShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::NONALPHA_GLOW_DEFFERED));
    }

    m_spVIBufferPlane->Render(spDownSamplingShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);
    spGameInstance->SetDefaultViewPort();

}

void URenderer::DownSample2()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::DOWNSAMPLETWO) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    D3D12_VIEWPORT _viewPort;
    _viewPort.TopLeftX = 0;
    _viewPort.TopLeftY = 0;
    _viewPort.MinDepth = 0;
    _viewPort.MaxDepth = 1.0;
    _viewPort.Height = spGameInstance->GetD3DViewport().Height / 16.f;
    _viewPort.Width = spGameInstance->GetD3DViewport().Width / 16.f;
    D3D12_RECT _rect;
    _rect.left = 0;
    _rect.top = 0;
    _rect.right = static_cast<LONG>(spGameInstance->GetD3DViewport().Width / 16);
    _rect.bottom = static_cast<LONG>(spGameInstance->GetD3DViewport().Height / 16);

    spGameInstance->SetTemporaryViewPort(_viewPort, _rect);
    // Bind Shader 
    SHPTR<UShader> spDownSamplingShader = FindShader(PROTO_RES_DOWNSAMPLINGTWOSHADER);
    spDownSamplingShader->SettingPipeLineState(m_spCastingCommand);
    spDownSamplingShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spDownSamplingShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::DOWNSAMPLE);
        spDownSamplingShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::DOWNSAMPLE));
    }

    m_spVIBufferPlane->Render(spDownSamplingShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);
    spGameInstance->SetDefaultViewPort();

}

void URenderer::UpSample()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::UPSAMPLE) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    // Bind Shader 
    SHPTR<UShader> spUpSamplingShader = FindShader(PROTO_RES_UPSAMPLINGSHADER);
    spUpSamplingShader->SettingPipeLineState(m_spCastingCommand);
    spUpSamplingShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spUpSamplingShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        //SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLUR_RESULT);
        //spUpSamplingShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::BLUR_RESULT));  
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::DOWNSAMPLETWO);
        spUpSamplingShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::DOWNSAMPLETWO));
    }

    m_spVIBufferPlane->Render(spUpSamplingShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);

}


void URenderer::UpSampleGray()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::UPSAMPLEGRAY) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    // Bind Shader 
    SHPTR<UShader> spUpSamplingShader = FindShader(PROTO_RES_UPSAMPLINGGRAYSHADER);
    spUpSamplingShader->SettingPipeLineState(m_spCastingCommand);
    spUpSamplingShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spUpSamplingShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        //SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLUR_RESULT);
        //spUpSamplingShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::BLUR_RESULT));  
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLUR_RESULT);
        spUpSamplingShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::BLUR_RESULT));
    }

    m_spVIBufferPlane->Render(spUpSamplingShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);

}

void URenderer::RenderHorizontalBlur()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLUR) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    D3D12_VIEWPORT _viewPort;
    _viewPort.TopLeftX = 0;
    _viewPort.TopLeftY = 0;
    _viewPort.MinDepth = 0;
    _viewPort.MaxDepth = 1.0;
    _viewPort.Height = spGameInstance->GetD3DViewport().Height / 2.f;
    _viewPort.Width = spGameInstance->GetD3DViewport().Width / 2.f;
    D3D12_RECT _rect;
    _rect.left = 0;
    _rect.top = 0;
    _rect.right = static_cast<LONG>(spGameInstance->GetD3DViewport().Width /2);
    _rect.bottom = static_cast<LONG>(spGameInstance->GetD3DViewport().Height / 2);
  
    spGameInstance->SetTemporaryViewPort(_viewPort, _rect);

    SHPTR<UShader> spHorizontalShader = FindShader(PROTO_RES_HORIZONTALBLURSHADER);
    spHorizontalShader->SettingPipeLineState(m_spCastingCommand);
    spHorizontalShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spHorizontalShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spBlur = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::HDRTWO);
        spHorizontalShader->BindSRVBuffer(SRV_REGISTER::T0, spBlur->GetRenderTargetTexture(RTOBJID::HDRTWO));
    }

    m_spVIBufferPlane->Render(spHorizontalShader, m_spCastingCommand);

    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);

}

void URenderer::RenderVerticalBlur()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLUR_RESULT) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    // Bind Shader 
    SHPTR<UShader> spVerticalShader = FindShader(PROTO_RES_VERTICALBLURSHADER);
    spVerticalShader->SettingPipeLineState(m_spCastingCommand);
    spVerticalShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spVerticalShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spNonAlpha = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLUR);
        spVerticalShader->BindSRVBuffer(SRV_REGISTER::T0, spNonAlpha->GetRenderTargetTexture(RTOBJID::BLUR));
    }

    m_spVIBufferPlane->Render(spVerticalShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);

    spGameInstance->SetDefaultViewPort();

}

void URenderer::RenderBloom()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

    // Set Render Tareget
    SHPTR<URenderTargetGroup> spRenderTargetGroup{ m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLOOM) };
    spRenderTargetGroup->WaitResourceToTarget(m_spCastingCommand);
    spRenderTargetGroup->ClearRenderTargetView(m_spCastingCommand);
    spRenderTargetGroup->OmSetRenderTargets(m_spCastingCommand);
    // Bind Shader 
    SHPTR<UShader> spShader = FindShader(PROTO_RES_BLOOMSHADER);
    spShader->SettingPipeLineState(m_spCastingCommand);
    spShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    spShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
    {
        SHPTR<URenderTargetGroup> spHDR = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::HDR);
        spShader->BindSRVBuffer(SRV_REGISTER::T0, spHDR->GetRenderTargetTexture(RTOBJID::HDR));
    }
    {
        SHPTR<URenderTargetGroup> spUpSample = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::UPSAMPLE);
        spShader->BindSRVBuffer(SRV_REGISTER::T1, spUpSample->GetRenderTargetTexture(RTOBJID::UPSAMPLE));
    }
    {
        SHPTR<URenderTargetGroup> spBlend = m_spRenderTargetManager->FindRenderTargetGroup(RTGROUPID::BLEND_DEFFERED);
        spShader->BindSRVBuffer(SRV_REGISTER::T2, spBlend->GetRenderTargetTexture(RTOBJID::BLEND_SCREEN_DEFFERED));
    }

    m_spVIBufferPlane->Render(spShader, m_spCastingCommand);
    spRenderTargetGroup->WaitTargetToResource(m_spCastingCommand);
}



void URenderer::RenderEnd()
{
    // Wait AlphaBlend
    // Is Alpha
    {
        m_spGraphicDevice->OmSetDefaultRenderTarget();
        SHPTR<UShader> spDefferedShader = FindShader(PROTO_RES_FINALDEFFEREDSHADER);
        spDefferedShader->SettingPipeLineState(m_spCastingCommand);
        spDefferedShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
        spDefferedShader->BindCBVBuffer(m_spTransformConstantBuffer, &m_stFinalRenderTransformParam, GetTypeSize<TRANSFORMPARAM>());
        //  Diffuse Texture 가져와서 Bind 
       
            spDefferedShader->BindSRVBuffer(SRV_REGISTER::T0, m_spRenderTargetManager->
                FindRenderTargetTexture(RTGROUPID::BLOOM,
                    RTOBJID::BLOOM));
       
       
            spDefferedShader->BindSRVBuffer(SRV_REGISTER::T6, m_spRenderTargetManager->
                FindRenderTargetTexture(RTGROUPID::GRAY_SCALE,
                    RTOBJID::GRAY_SCALE)); //죽었을때
            spDefferedShader->BindSRVBuffer(SRV_REGISTER::T7, m_spRenderTargetManager->
                FindRenderTargetTexture(RTGROUPID::BLUR_RESULT,
                    RTOBJID::BLUR_RESULT)); //능력 쓸때
            spDefferedShader->BindSRVBuffer(SRV_REGISTER::T8, m_spRenderTargetManager->
                FindRenderTargetTexture(RTGROUPID::UPSAMPLEGRAY,
                    RTOBJID::UPSAMPLEGRAY));//실명, 맞았을 때
        //UPSAMPLEGRAY쓰면 흑백 암전(아예검은색 텍스쳐여서) , GrayScale이 흑백, BLUR_RESULT가 여기에 블러
        
        //  Diffuse Texture 가져와서 Bind 
        spDefferedShader->BindSRVBuffer(SRV_REGISTER::T1, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::ALPHA_DEFFERED,
                RTOBJID::ALPHA_DIFFUSE_DEFFERED));

        spDefferedShader->BindSRVBuffer(SRV_REGISTER::T2, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::NONALPHA_DEFFERED,
                RTOBJID::NONALPHA_POSITION_DEFFERED));
        spDefferedShader->BindSRVBuffer(SRV_REGISTER::T3, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::NONALPHA_DEFFERED,
                RTOBJID::NONALPHA_GLOW_DEFFERED));
        spDefferedShader->BindSRVBuffer(SRV_REGISTER::T4, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::DEPTH_RECORD,
                RTOBJID::DEPTH_RECORD));
        spDefferedShader->BindSRVBuffer(SRV_REGISTER::T5, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::OUTLINE_POS_NOR,
                RTOBJID::OUTLINE_DEPTH_POS));
        spDefferedShader->BindSRVBuffer(SRV_REGISTER::T9, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::OUTLINE_POS_NOR_FORABILITY,
                RTOBJID::OUTLINE_DEPTH_POS_FORABILITY));
      /*  spDefferedShader->BindSRVBuffer(SRV_REGISTER::T6, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::NONALPHA_DEFFERED,
                RTOBJID::NONALPHA_SPECULAR_DEFFERED));*/
        {
            spDefferedShader->BindCBVBuffer(m_spTurnShaderConstantBuffer, &m_bTurnShader, sizeof(DRAWSHADERBUFFER));
        }
        //  Diffuse Texture 가져와서 Bind 
   /*     spDefferedShader->BindSRVBuffer(SRV_REGISTER::T2, m_spRenderTargetManager->
            FindRenderTargetTexture(RTGROUPID::UI2D_DEFFERED,
                RTOBJID::UI2D_SCREEN_DEFFERED));*/
        //  Render
        m_spVIBufferPlane->Render(spDefferedShader, m_spCastingCommand);
    }
}

#ifdef _USE_DEBUGGING
void URenderer::RenderDebug()
{

    // Render Debugging
    for (auto& Debugging : m_arrDrawDebugRenderList)
    {
        for (auto& iter : Debugging)
        {
            RenderObject(iter.first, iter.second);
        }
    }
}
#endif

SHPTR<UShader> URenderer::FindShader(const _wstring& _wstrProto)
{
    const SHADERODERS::iterator& ShaderIter = m_ShaderObjects.find(_wstrProto);
    RETURN_CHECK(m_ShaderObjects.end() == ShaderIter, nullptr);
    return ShaderIter->second;
}


void URenderer::RenderObject(const _wstring& _wstrShaderName, PAWNLIST& _PawnList)
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    // Pipelinestate setting
    spGameInstance->SettingPipeLineState(_wstrShaderName, m_spCastingCommand);
    // Shadr 필요한 Pawn끼리 list 쭉 그려
    for (auto& iter : _PawnList)
    {
        iter->Render(m_spCastingCommand, m_spGraphicDevice->GetTableDescriptor());
    }
}

void URenderer::RenderShadowObject(const _wstring& _wstrShaderName, PAWNLIST& _PawnList)
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    // Pipelinestate setting
    spGameInstance->SettingPipeLineState(_wstrShaderName, m_spCastingCommand);
    // Shadr 필요한 Pawn끼리 list 쭉 그려
    for (auto& iter : _PawnList)
    {
        iter->RenderShadow(m_spCastingCommand, m_spGraphicDevice->GetTableDescriptor());
    }
}

void URenderer::RenderOutlineObject(const _wstring& _wstrShaderName, PAWNLIST& _PawnList,_bool _pass)
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    // Pipelinestate setting
    spGameInstance->SettingPipeLineState(_wstrShaderName, m_spCastingCommand);
    // Shadr 필요한 Pawn끼리 list 쭉 그려
    for (auto& iter : _PawnList)
    {
        iter->RenderOutline(m_spCastingCommand, m_spGraphicDevice->GetTableDescriptor(), _pass);
    }
   
}


SHPTR<UShader> URenderer::FrameReadyDrawFinalRenderTarget(const _wstring& _wstrShaderName, const RTGROUPID _eGroupID)
{
    m_spRenderTargetManager->OmSetRenderTargets(m_spCastingCommand, _eGroupID);
    SHPTR<UShader> spShader = FindShader(_wstrShaderName);
    spShader->SettingPipeLineState(m_spCastingCommand);
    return spShader;
}

SHPTR<UShader> URenderer::FrameReadyDrawLast(const _wstring& _wstrShaderName)
{
    SHPTR<UShader> spShader = FindShader(_wstrShaderName);
    spShader->SettingPipeLineState(m_spCastingCommand);
    spShader->SetTableDescriptor(m_spGraphicDevice->GetTableDescriptor());
    return spShader;
}

void URenderer::BindGrobalBuffer()
{
   // SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    //spGameInstance->CopyToMaterialShaderParam(REF_OUT m_stGlobalParam);
    //// Setting Grobal Data
    //m_spGlobalBuffer->SettingGlobalData(m_spCastingCommand, &m_stGlobalParam, GetTypeSize<GLOBALPARAM>());
}
