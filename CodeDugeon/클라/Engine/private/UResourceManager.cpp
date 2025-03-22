#include "EngineDefine.h"
#include "UResourceManager.h"
#include "UResource.h"
#include "UCommand.h"

UResourceManager::UResourceManager() 
{
}

void UResourceManager::Free()
{
    m_PipeLineContainer.clear();
    for (auto& iter : m_arrPrototypes)
        iter.clear();
}

HRESULT UResourceManager::AddPrototype(const _wstring& _wstrPrototype, const CLONETYPE& _eCloneType, CSHPTRREF<UResource> _spResource)
{
    RETURN_CHECK(nullptr == _spResource, E_FAIL);

    _ubyte bCloneType = (_ubyte)_eCloneType;
    RETURN_CHECK(nullptr != FindPrototype(bCloneType, _wstrPrototype), E_FAIL);

    m_arrPrototypes[bCloneType].insert(std::pair<_wstring, SHPTR<UResource>>(_wstrPrototype, _spResource));

    return S_OK;
}

SHPTR<UResource> UResourceManager::CloneResource(const _wstring& _wstrPrototype, const VOIDDATAS& _stDatas)
{
    SHPTR<UResource> pResource = FindPrototype((_ubyte)CLONETYPE::CLONE_STATIC, _wstrPrototype);
    if (nullptr == pResource)
    {
        pResource = FindPrototype((_ubyte)CLONETYPE::CLONE_ONCE, _wstrPrototype);
        RETURN_CHECK(nullptr == pResource, nullptr);
    }
    return static_pointer_cast<UResource>(pResource->Clone(_stDatas));
}

void UResourceManager::FindResourceProtoTag(CSHPTRREF<UResource> _spResource, _wstring& _wstrResourceTag)
{
    RETURN_CHECK(nullptr == _spResource, ;);
    for (auto& iter : m_arrPrototypes)
    {
        for (auto& value : iter)
        {
            if (value.second == _spResource)
            {
                _wstrResourceTag = value.first;
                return;
            }
        }
    }
}

HRESULT UResourceManager::ClearOnceTypeData()
{
    _ubyte byOnce = (_ubyte)CLONETYPE::CLONE_ONCE;
    m_arrPrototypes[byOnce].clear();
    return S_OK;
}

void UResourceManager::AddPipeLineState(const _wstring& _wstrName, const ComPtr<Dx12PipelineState>& _cpPipeLineState)
{
    const auto& iter = m_PipeLineContainer.find(_wstrName);
    RETURN_CHECK(iter != m_PipeLineContainer.end(), ;);

    m_PipeLineContainer.insert(std::pair<_wstring, ComPtr<Dx12PipelineState>>(_wstrName, _cpPipeLineState));
}

void UResourceManager::SettingPipeLineState(const _wstring& _wstrName, CSHPTRREF<UCommand> _spCommand)
{
    const auto& iter = m_PipeLineContainer.find(_wstrName);
    assert(iter != m_PipeLineContainer.end());
    _spCommand->GetGpuCmdList()->SetPipelineState(iter->second.Get());
}

SHPTR<UResource> UResourceManager::FindPrototype(const _ubyte _bCloneType, const _wstring& _strPrototype)
{
    const auto& iter = m_arrPrototypes[_bCloneType].find(_strPrototype);
    RETURN_CHECK(iter == m_arrPrototypes[_bCloneType].end(), nullptr);
    return iter->second;
}
