#include "CCubePawn.h"
#include "CCubeBuffer.h"
#include "CMethod.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "COBBCollider.h"

shared_ptr<CActor> CCubePawn::Clone(const VOIDDATAS& _tDatas) {
	return nullptr;
}

void CCubePawn::Free() {
	Safe_Free(m_pCubeBuffer);
	__super::Free();
}

HRESULT CCubePawn::NativeConstruct()
{
	return S_OK;
}

HRESULT CCubePawn::NativeConstruct(VOIDDATAS _tDatas)
{
	CUBEPAWNDESC tDesc;
	if (_tDatas.size() > 0) {
		tDesc = CMethod::ConvertTemplate_Index<CUBEPAWNDESC>(&_tDatas, 0);
	}

	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	if (nullptr != m_pCubeBuffer)
		m_pCubeBuffer->Set_Color(tDesc.vColor);

	shared_ptr<CTransform> pTransform = Get_Transform();
	if (nullptr != Get_Transform()) {
		pTransform->Set_Scale(tDesc.vBodyScale);
	}

	Set_Active(true);
	return S_OK;
}

HRESULT CCubePawn::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CCubePawn>();
	// Create Bufafer
	{
		m_pCubeBuffer = CloneCompData<CCubeBuffer>(PROTO_COMP_CUBEBUFFER, VOIDDATAS{ &tDesc });
		Add_Component(COM_BUFFER, m_pCubeBuffer);
	}
	return S_OK;
}

HRESULT CCubePawn::Tick(const _double& _dTimeDelta) {
	
	return __super::Tick(_dTimeDelta);
}

HRESULT CCubePawn::Late_Tick(const _double& _dTimeDelta)
{
	RETURN_CHECK_FAILED(__super::Late_Tick(_dTimeDelta), E_FAIL);

	Get_Renderer()->Add_Render(RENDER_ACTOR, ThisShared<CActor>());
	return S_OK;
}

HRESULT CCubePawn::Render(const HDC& _hDC)
{
	m_pCubeBuffer->Render(_hDC);
	return S_OK;
}
