#include "ClientDefines.h"
#include "CHpBarUI.h"
#include "UTexGroup.h"
#include "UGameInstance.h"
#include "UMethod.h"
#include "URectTransform.h"
#include "UVIBufferRect.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"

CHpBarUI::CHpBarUI(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType) :
	UUserInterface(_spDevice, _wstrLayer, _eCloneType)
{
}

CHpBarUI::CHpBarUI(const CHpBarUI& _rhs) :
	UUserInterface(_rhs)
{
}

HRESULT CHpBarUI::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CHpBarUI::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	AddShader(GetUIDesc()._shaderName);
	m_spUITextureGroup = std::static_pointer_cast<UTexGroup>(spGameInstance->CloneResource(PROTO_RES_UITEXTUREGROUP));

	m_spHpBarUIBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::BUTTONUI,
		static_cast<_uint>(sizeof(HPDESC)));

	return S_OK;
}

void CHpBarUI::TickActive(const _double& _dTimeDelta)
{

}

void CHpBarUI::LateTickActive(const _double& _dTimeDelta)
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

HRESULT CHpBarUI::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderActive(_spCommand, _spTableDescriptor);

	GetTransform()->BindTransformData(GetShader(), 1);
	if (GetUIDesc().strImgName == L"Player") {
		m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T0, L"PlayerHp");
	}
	else if (GetUIDesc().strImgName == L"Boss") {
		m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T0, L"BossHp");
	}
	
	
	m_spUITextureGroup->SetUpTextureName(GetShader(), SRV_REGISTER::T1, L"noise01");
	GetShader()->BindCBVBuffer(m_spHpBarUIBuffer, &_HpDesc, sizeof(HPDESC));
	GetVIBufferRect()->Render(GetShader(), _spCommand);


	return S_OK;
}

HRESULT CHpBarUI::ReadyRectTransform(const VOIDDATAS& _vecDatas)
{
	return __super::ReadyRectTransform(_vecDatas);
}

void CHpBarUI::Free()
{
}

void CHpBarUI::SetMaxHp(_float _MaxHp)
{
	_HpDesc._MaxHp = _MaxHp;
}

void CHpBarUI::SetCurHp(_float _CurHp)
{
	_HpDesc._CurHp = _CurHp;
}

HRESULT CHpBarUI::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	return E_NOTIMPL;
}

HRESULT CHpBarUI::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	return E_NOTIMPL;
}

void CHpBarUI::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
