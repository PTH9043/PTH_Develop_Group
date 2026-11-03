#include "CUserInterface.h"
#include "CGameInstance.h"
#include "CRectTransform.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CCubeBuffer.h"

shared_ptr<CActor> CUserInterface::Clone(const VOIDDATAS& _tDatas)
{

	return shared_ptr<CActor>();
}

void CUserInterface::Free()
{
}

HRESULT CUserInterface::NativeConstruct()
{
	return S_OK;
}

HRESULT CUserInterface::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas)))
		return E_FAIL;

	if (FAILED(CreateRectComp(_tDatas)))
		return E_FAIL;

	Set_Active(true);
	return S_OK;
}

HRESULT CUserInterface::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CUserInterface>();
	m_pRenderer = CloneCompData<CRenderer>(PROTO_COMP_RENDER, VOIDDATAS{ &tDesc });
	return S_OK;
}

HRESULT CUserInterface::CreateRectComp(const VOIDDATAS& _tDatas)
{
	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CUserInterface>();
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	CRectTransform::RECTDESC tRectDesc;
	::memcpy(&tRectDesc, _tDatas[0], sizeof(UIDESC));
	tRectDesc.pTransform = Get_Transform();
	m_pRectTransform = CloneCompData<CRectTransform>(PROTO_COMP_RECTTRANSFORM, VOIDDATAS{ &tDesc,  &tRectDesc });
	RETURN_CHECK(nullptr == m_pRectTransform, E_FAIL);

	Add_Component(COM_RECTTRANSFORM, m_pRectTransform);
	
	return S_OK;
}

HRESULT CUserInterface::Tick(const _double& _dTimeDelta)
{
	return __super::Tick(_dTimeDelta);
}

HRESULT CUserInterface::Late_Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Late_Tick(_dTimeDelta)))
		return E_FAIL;

	m_pRenderer->Add_Render(RENDERID::RENDER_UI, ThisShared<CActor>());
	return S_OK;
}

HRESULT CUserInterface::Render(const HDC& _hDC)
{
	return S_OK;
}
