#include "ClientDefines.h"
#include "CLoadingUI.h"
#include "UTexGroup.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "URectTransform.h"
#include "UVIBufferRect.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"

CLoadingUI::CLoadingUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType) :
	UUserInterface(_spDevice, _wstrLayer, _eCloneType)
{
}

CLoadingUI::CLoadingUI(const CLoadingUI& _rhs) :
	UUserInterface(_rhs)
{
}

HRESULT CLoadingUI::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CLoadingUI::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	AddShader(GetUIDesc()._shaderName);
	m_spUITextureGroup = std::static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_UITEXTUREGROUP));

	m_spLoadingUIBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::BUTTONUI,
		static_cast<_uint>(sizeof(LOADINGDESC)));

	return S_OK;
}

void CLoadingUI::TickActive(const _double& _dTimeDelta)
{
	if (_LoadingDesc.ifPressed) {
		_LoadingDesc._durationTime += _dTimeDelta;
	}
	else {
		_LoadingDesc._durationTime = 0;
	}

}

void CLoadingUI::LateTickActive(const _double& _dTimeDelta)
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

HRESULT CLoadingUI::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	GetTransform()->BindTransformData(GetShader(), 1);
	m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T0, GetUIDesc().strImgName);
	GetShader()->BindCBVBuffer(m_spLoadingUIBuffer, &_LoadingDesc, sizeof(LOADINGDESC));
	GetVIBufferRect()->Render(GetShader(), _spCommand);


	return S_OK;
}

HRESULT CLoadingUI::ReadyRectTransform(const VOIDDATAS& _vecDatas)
{
	return __super::ReadyRectTransform(_vecDatas);
}

void CLoadingUI::Free()
{
}

HRESULT CLoadingUI::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return E_NOTIMPL;
}

HRESULT CLoadingUI::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return E_NOTIMPL;
}

void CLoadingUI::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
