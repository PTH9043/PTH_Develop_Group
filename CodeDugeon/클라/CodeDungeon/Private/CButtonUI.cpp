#include "ClientDefines.h"
#include "CButtonUI.h"
#include "UTexGroup.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "URectTransform.h"
#include "UVIBufferRect.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"

CButtonUI::CButtonUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType) :
	UUserInterface(_spDevice, _wstrLayer, _eCloneType)
{
}

CButtonUI::CButtonUI(const CButtonUI& _rhs) :
	UUserInterface(_rhs)
{
}

HRESULT CButtonUI::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CButtonUI::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	AddShader(GetUIDesc()._shaderName);
	m_spUITextureGroup = std::static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_UITEXTUREGROUP));
	
	m_spButtonUIBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::BUTTONUI,
		static_cast<_uint>(sizeof(BUTTONDESC)));

	return S_OK;
}

void CButtonUI::TickActive(const _double& _dTimeDelta)
{
	if (_buttonDesc.ifPressed) {
		_buttonDesc._durationTime += _dTimeDelta;
	}
	else {
		_buttonDesc._durationTime = 0;
	}
}

void CButtonUI::LateTickActive(const _double& _dTimeDelta)
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

HRESULT CButtonUI::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	GetTransform()->BindTransformData(GetShader(), 1);
	if (GetUIDesc().strImgName == L"Enter") {
		m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T0, L"EnterDown");
		m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T1, L"EnterUp");
	}
	else if (GetUIDesc().strImgName == L"Exit") {
		m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T0, L"ExitDown");
		m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T1, L"ExitUp");
	}

	GetShader()->BindCBVBuffer(m_spButtonUIBuffer, &_buttonDesc, sizeof(BUTTONDESC));
	GetVIBufferRect()->Render(GetShader(), _spCommand);
	

	return S_OK;
}

HRESULT CButtonUI::ReadyRectTransform(const VOIDDATAS& _vecDatas)
{
	return __super::ReadyRectTransform(_vecDatas);
}

void CButtonUI::Free()
{
}

HRESULT CButtonUI::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return E_NOTIMPL;
}

HRESULT CButtonUI::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return E_NOTIMPL;
}

void CButtonUI::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
