#include "EngineDefine.h"
#include "UMaterialManager.h"
#include "UModelMaterial.h"

UMaterialManager::UMaterialManager() : m_isMaterialContainerInfoChange{ false }
{
}

void UMaterialManager::AddModelMaterial(const _uint _MaterialIndex, CSHPTRREF<UModelMaterial> _spModelMaterial)
{
    assert(nullptr != _spModelMaterial);
    m_MaterialContainer.emplace(MakePair(_MaterialIndex, _spModelMaterial));

    m_isMaterialContainerInfoChange = true;
}

void UMaterialManager::CopyToMaterialShaderParam(REF_IN GLOBALPARAM& _GrobalParam)
{
   /* RETURN_CHECK(false == m_isMaterialContainerInfoChange, ;);

    for (auto& iter : m_MaterialContainer)
    {
        ::memcpy(&_GrobalParam.stMaterialGrobalInfo.stModelMaterialInfoGroup[iter.first],
            &iter.second.lock()->GetMaterialInfo(), GetTypeSize<MODELMATERIALINFO>());
    }

    m_isMaterialContainerInfoChange = false;*/
}

void UMaterialManager::RemoveModelMaterial(const _uint _MaterialIndex)
{
    m_MaterialContainer.erase(_MaterialIndex);
}

void UMaterialManager::Free()
{
}
