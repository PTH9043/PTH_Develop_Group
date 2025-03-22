#include "EngineDefine.h"
#include "UScene.h"
#include "ULight.h" 
#include "ULoader.h"
#include "UShader.h"
#include "UVIBufferRect.h"
#include "UShaderConstantBuffer.h"
#include "UGameInstance.h"
#include "UTransform.h"
#include "URenderTargetGroup.h"
#include "UGpuCommand.h"

UScene::UScene(CSHPTRREF<UDevice> _spDevice, const _ushort _iSceneID)
    : UObject(_spDevice),
    m_spLoader{ nullptr },
    m_sSceneID{ _iSceneID },
    m_LightGroup{},
    m_AllLightContainer{},
    MAX_LIGHT_NUMS{ MAX_LIGHTS },
    CUR_LIGHT_NUMS{ 0 },
    m_stLightParams{},
    m_stLightParamVector{}
{
}

void UScene::Free()
{
}

HRESULT UScene::NativeConstruct()
{
    if (FAILED(__super::NativeConstruct()))
        return E_FAIL;

    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    // Direction 
    //m_LightShaders.insert(MakePair(LIGHTTYPE::TYPE_DIRECTIONAL, static_pointer_cast<UShader>(
    //    spGameInstance->CloneResource(PROTO_RES_LIGHTDIRECTIONSHADER)
    //)));
    //SHPTR<UShader> spShader = static_pointer_cast<UShader>(spGameInstance->CloneResource(PROTO_RES_LIGHTPOINTSHADER));
    //// Point
    //m_LightShaders.insert(MakePair(LIGHTTYPE::TYPE_POINT, spShader));
    //// Spot
    //m_LightShaders.insert(MakePair(LIGHTTYPE::TYPE_SPOT, spShader));
    //// Rect
    m_spVIBufferPlane = static_pointer_cast<UVIBufferRect>(spGameInstance->CloneResource(PROTO_RES_VIBUFFERRECT));
   
    m_LightingShader= static_pointer_cast<UShader>(spGameInstance->CloneResource(PROTO_RES_LIGHTPOINTSHADER));

    m_spLightConstantBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::LIGHTCONTROL, GetTypeSize<LIGHTPARAMS>());

    return S_OK;
}

void UScene::RenderLights()
{
    SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
    SHPTR<URenderTargetGroup> spGroup{ spGameInstance->FindRenderTargetGroup(RTGROUPID::NONALPHA_DEFFERED) };
    SHPTR<URenderTargetGroup> spShadowDepthGroup{ spGameInstance->FindRenderTargetGroup(RTGROUPID::SHADOW_MAP) };
    SHPTR<UCommand> spCmdList = static_pointer_cast<UCommand>(spGameInstance->GetGpuCommand());
    SHPTR<UTableDescriptor> spTableDescriptor= spGameInstance->GetTableDescriptor();
    RETURN_CHECK(nullptr == spGroup, ;);
    RETURN_CHECK(nullptr == spShadowDepthGroup, ;);

    //for (const std::pair<LIGHTTYPE, SHPTR<UShader>>& LightPair : m_LightShaders)
    //{
    //    const auto& LightIterator = m_LightGroup.find(LightPair.first);

    //    if (m_LightGroup.end() == LightIterator)
    //        continue;
    //    {
    //        //       SMART_LOCK LL{ UGameInstance::GetResourceMutex() };
    //        LightPair.second->SettingPipeLineState(spCmdList);
    //        LightPair.second->SetTableDescriptor(spTableDescriptor);
    //    }

    //    // Bind Transform Param
    //    LightPair.second->BindSRVBuffer(SRV_REGISTER::T0, spGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_NORMAL_DEFFERED));
    //    LightPair.second->BindSRVBuffer(SRV_REGISTER::T1, spGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
    //    LightPair.second->BindSRVBuffer(SRV_REGISTER::T2, spGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_POSITION_DEFFERED));
    //    LightPair.second->BindSRVBuffer(SRV_REGISTER::T3, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::SHADOW_DEPTH_FOURBYFOUR));

    //    // Draw Light
    //    //04-09 ¼öÁ¤
    //    for (CSHPTRREF<ULight> spLight : LightIterator->second)
    //    {
    //        spLight->Render(spCmdList, m_spVIBufferPlane, LightPair.second);
    //    }
    //}

    {
        m_LightingShader->SettingPipeLineState(spCmdList);
        m_LightingShader->SetTableDescriptor(spTableDescriptor);
       
        m_LightingShader->BindSRVBuffer(SRV_REGISTER::T0, spGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_NORMAL_DEFFERED));
        m_LightingShader->BindSRVBuffer(SRV_REGISTER::T1, spGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_DEPTH_DEFFERED));
        m_LightingShader->BindSRVBuffer(SRV_REGISTER::T2, spGroup->GetRenderTargetTexture(RTOBJID::NONALPHA_POSITION_DEFFERED));
        m_LightingShader->BindSRVBuffer(SRV_REGISTER::T3, spShadowDepthGroup->GetRenderTargetTexture(RTOBJID::SHADOW_DEPTH_FOURBYFOUR));


    }
    m_stLightParams.nLights = CUR_LIGHT_NUMS;
    for (int i = 0; i < CUR_LIGHT_NUMS; i++) {
        m_stLightParams.tLightInfos[i] = m_AllLightContainer[i]->GetLightInfo();
        m_stLightParamVector[i]= m_AllLightContainer[i]->GetLightInfo();
    }
  
    m_LightingShader->BindCBVBuffer(m_spLightConstantBuffer, &m_stLightParams, GetTypeSize<LIGHTPARAMS>());

    m_spVIBufferPlane->Render(m_LightingShader, spCmdList);
}

HRESULT UScene::AddLight(const LIGHTINFO& _stLightInfo)
{
    SHPTR<ULight> pLight = CreateConstructorToNative<ULight>(GetDevice(), _stLightInfo);

    const LIGHTGROUP::iterator& it = m_LightGroup.find(_stLightInfo.eLightType);
   
   

    if (CUR_LIGHT_NUMS <= MAX_LIGHT_NUMS) {
        if (it == m_LightGroup.end()) {
            LIGHTCONTAINER LightContainer{  };
            LightContainer.push_back(pLight);
            m_LightGroup.insert(std::make_pair(_stLightInfo.eLightType, LightContainer));
        }
        else {
            it->second.push_back(pLight);
        }
        m_AllLightContainer.push_back(pLight);
        m_stLightParams.tLightInfos[CUR_LIGHT_NUMS] = _stLightInfo;

        m_stLightParamVector.push_back(_stLightInfo);
        CUR_LIGHT_NUMS++;
    }
  
    return S_OK;
}

void UScene::OutLight(const LIGHTTYPE& _eLightType, const _uint _iIndex, SHPTR<ULight>& _spLight)
{
    if (_iIndex >= m_LightGroup[_eLightType].size())
        return;

    LIGHTCONTAINER::iterator it = m_LightGroup[_eLightType].begin();

    it = it + _iIndex;
    if (it != m_LightGroup[_eLightType].end())
        _spLight = (*it);
}

HRESULT UScene::ActiveLIght(const LIGHTTYPE& _eLightType, const _uint& _iIndex, const _bool& _isActive)
{
    RETURN_CHECK(_iIndex >= m_LightGroup[_eLightType].size(), E_FAIL);

    LIGHTCONTAINER::iterator it = m_LightGroup[_eLightType].begin();

    it = it + _iIndex;
    if (it != m_LightGroup[_eLightType].end())
        (*it)->SetActive(_isActive);
    return S_OK;
}

HRESULT UScene::ActiveLIght(const LIGHTTYPE& _eLightType, const _uint& _iIndex, const LIGHTACTIVE& _isActive)
{
    RETURN_CHECK(_iIndex >= m_LightGroup[_eLightType].size(), E_FAIL);

    LIGHTCONTAINER::iterator it = m_LightGroup[_eLightType].begin();

    it = it + _iIndex;
    if (it != m_LightGroup[_eLightType].end())
        (*it)->SetActive(_isActive);
    return S_OK;
}

HRESULT UScene::DeleteLight(const LIGHTTYPE& _eLightType, const _uint& _iIndex)
{
    RETURN_CHECK(_iIndex >= m_LightGroup[_eLightType].size(), E_FAIL);

    LIGHTCONTAINER::iterator it = m_LightGroup[_eLightType].begin();

    it = it + _iIndex;
    if (it != m_LightGroup[_eLightType].end())
    {
        m_LightGroup[_eLightType].erase(it);
    }
    return S_OK;
}

HRESULT UScene::ClearLight()
{
    m_LightGroup.clear();
    m_AllLightContainer.clear();
    return S_OK;
}

HRESULT UScene::StartLoadingResource(const VECTOR<UDelegate<void, _bool&>>& _vecFunctions, const _uint& _iMethodCount)
{
    m_spLoader = CreateNative<ULoader>(_vecFunctions, _iMethodCount);
    return S_OK;
}
