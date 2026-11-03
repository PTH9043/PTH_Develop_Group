#include "CPawn.h"
#include "CRenderer.h"

void CPawn::Free()
{
	Safe_Free(m_pRenderer);
	__super::Free();
}

HRESULT CPawn::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct())) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPawn::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPawn::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}

	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CPawn>();
	m_pRenderer = CloneCompData<CRenderer>(PROTO_COMP_RENDER, VOIDDATAS{ &tDesc });
	Add_Component(COM_RENDERER, m_pRenderer);
	return S_OK;
}

HRESULT CPawn::Tick(const _double& _dTimeDelta)
{
	return __super::Tick(_dTimeDelta);
}

HRESULT CPawn::Late_Tick(const _double& _dTimeDelta)
{
	return __super::Late_Tick(_dTimeDelta);
}

HRESULT CPawn::Render(const HDC& _hDC)
{
	return __super::Render(_hDC);
}
