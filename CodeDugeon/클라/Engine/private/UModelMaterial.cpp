#include "EngineDefine.h"
#include "UModelMaterial.h"
#include "UMethod.h"
#include "UShader.h"
#include "UTexGroup.h"
#include "UGameInstance.h"

// Material Draw Number 
 _int									UModelMaterial::s_AllMaterialDrawLength;
// 넘버링을 주기 위한 컨테이너
 VECTOR<_int>				UModelMaterial::s_RemainMaterialDrawNumber;

UModelMaterial::UModelMaterial(CSHPTRREF<UDevice> _spDevice, const DESC& _MaterialDesc) :
	UComponent(_spDevice), m_MaterialInfo{ _MaterialDesc.MaterialInfo },
	m_MaterialTexContainer{ _MaterialDesc.MaterialTexContainer }, m_BindTextureContainer{}, m_iMaterialIndex{0}
{
}

UModelMaterial::UModelMaterial(const UModelMaterial& _rhs) : 
	UComponent(_rhs), m_MaterialInfo{ _rhs.m_MaterialInfo },
	m_MaterialTexContainer{ _rhs.m_MaterialTexContainer }, m_BindTextureContainer{}, m_iMaterialIndex{ 0 }
{ 
}

void UModelMaterial::Free()
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->RemoveModelMaterial(m_iMaterialIndex);

	s_RemainMaterialDrawNumber.push_back(m_iMaterialIndex);
}

HRESULT UModelMaterial::NativeConstruct()
{
	// 만약 남아있는 머터리얼 DrawNumber가 남아있으면, Remain Numbering부터 찾아온다. 
	if (s_RemainMaterialDrawNumber.size() > 0)
	{
		m_iMaterialIndex = s_RemainMaterialDrawNumber.front();
		s_RemainMaterialDrawNumber.erase(s_RemainMaterialDrawNumber.begin());
	}
	else
	{
		m_iMaterialIndex = s_AllMaterialDrawLength;
		s_AllMaterialDrawLength++;
	}

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	spGameInstance->AddModelMaterial(m_iMaterialIndex, ThisShared<UModelMaterial>());

	return __super::NativeConstruct();
}

HRESULT UModelMaterial::NativeConstructClone(const VOIDDATAS& _stDatas)
{
	return __super::NativeConstructClone(_stDatas);
}

void UModelMaterial::BindTexture(CSHPTRREF<UShader> _spShader, const SRV_REGISTER _srvRegister, TEXTYPE _TexType, _int _TextureOrder)
{
	assert(nullptr != _spShader);
	RETURN_CHECK(nullptr == m_MaterialTexContainer[_TexType], ;);
	_spShader->BindSRVBuffer(_srvRegister, m_MaterialTexContainer[_TexType]->GetTexture(_TextureOrder));
}

void UModelMaterial::BindTexture(CSHPTRREF<UShader> _spShader, const SRV_REGISTER _srvRegister, TEXTYPE _TexType, const _wstring& _wstrTextureName)
{
	assert(nullptr != _spShader);
	_spShader->BindSRVBuffer(_srvRegister, m_MaterialTexContainer[_TexType]->GetTexture(_wstrTextureName));
}

void UModelMaterial::RegisterBindTextureContainer(const SRV_REGISTER _srvRegister, TEXTYPE _TexType, _int _TextureOrder)
{
	assert(nullptr != m_MaterialTexContainer[_TexType]);
	m_BindTextureContainer.emplace(MakePair(_srvRegister, m_MaterialTexContainer[_TexType]->GetTexture(_TextureOrder)));
}

void UModelMaterial::RegisterBindTextureContainer(const SRV_REGISTER _srvRegister, TEXTYPE _TexType, const _wstring& _wstrTextureName)
{
	assert(nullptr != m_MaterialTexContainer[_TexType]);
	m_BindTextureContainer.emplace(MakePair(_srvRegister, m_MaterialTexContainer[_TexType]->GetTexture(_wstrTextureName)));
}

