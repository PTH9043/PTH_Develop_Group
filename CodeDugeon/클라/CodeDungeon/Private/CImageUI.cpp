#include "ClientDefines.h"
#include "CImageUI.h"
#include "UTexGroup.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "URectTransform.h"
#include "UVIBufferRect.h"
#include "UTransform.h"

CImageUI::CImageUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType) :
	UUserInterface(_spDevice, _wstrLayer, _eCloneType)
{
}

CImageUI::CImageUI(const CImageUI& _rhs) : 
	UUserInterface(_rhs)
{
}

HRESULT CImageUI::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CImageUI::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	AddShader(GetUIDesc()._shaderName);
	m_spUITextureGroup = std::static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_UITEXTUREGROUP));
	return S_OK;
}

void CImageUI::TickActive(const _double& _dTimeDelta)
{
}

void CImageUI::LateTickActive(const _double& _dTimeDelta)
{
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	RETURN_CHECK(spGameInstance->IsFreeModeCameraEnable() || spGameInstance->IsGamePause(), ;);

	if (nullptr != GetVIBufferRect()) {
		if (GetUIDesc().DrawOrder == L"Priority") {
			AddRenderGroup(RENDERID::RI_2DUIPRIORITY);
		}
		else if (GetUIDesc().DrawOrder == L"Middle") {
			AddRenderGroup(RENDERID::RI_2DUIMIDDLE);
		}
		else if (GetUIDesc().DrawOrder == L"Last") {
			AddRenderGroup(RENDERID::RI_2DUILAST);
		}
	}
		
}

HRESULT CImageUI::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	GetTransform()->BindTransformData(GetShader(),1);
	m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T0, GetUIDesc().strImgName);
	GetVIBufferRect()->Render(GetShader(), _spCommand);
	return S_OK;
}

HRESULT CImageUI::ReadyRectTransform(const VOIDDATAS& _vecDatas)
{
	return __super::ReadyRectTransform(_vecDatas);
}

void CImageUI::Free()
{
}

HRESULT CImageUI::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return E_NOTIMPL;
}

HRESULT CImageUI::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return E_NOTIMPL;
}

void CImageUI::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
