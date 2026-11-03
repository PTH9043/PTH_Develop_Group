#include "CFloor.h"
#include "CFloorBuffer.h"
#include "CMethod.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CAABBCollider.h"

shared_ptr<CActor> CFloor::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CFloor> pActor = CSmartPointerFactory<CFloor>::CreateShared(*this);
	if (FAILED(pActor->NativeConstruct(_tDatas))) {
		Safe_Free(pActor);
		DEBUG_MSG("CFloor::Clone To Failed");
	}
	return pActor;
}

void CFloor::Free() {
	__super::Free();
}

HRESULT CFloor::NativeConstruct()
{
	return S_OK;
}

HRESULT CFloor::NativeConstruct(VOIDDATAS _tDatas)
{
	FLOORDESC tDesc;
	if (_tDatas.size() > 0) {
		tDesc = CMethod::ConvertTemplate_Index<FLOORDESC>(&_tDatas, 0);
		m_fWidth = tDesc.fWidth;
		m_fDepth = tDesc.fDepth;
		m_iSubject = tDesc.iSubjects;
	}

	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}

	if (nullptr != m_pFloorBuffer)
		m_pFloorBuffer->Set_Color(tDesc.vColor);

	shared_ptr<CTransform> pTransform = Get_Transform();
	if (nullptr != Get_Transform()) {
		pTransform->Set_Scale(tDesc.vBodyScale);
		pTransform->Set_Pos(_float3(0.f, -0.5f, 0.f).Get_Vector());
		m_pCollider->Set_Scale(_float3(m_fWidth , 0.001f, m_fDepth  ));
		m_pRegionCollider->Set_Scale(_float3(m_fWidth, 100.f, m_fDepth));
	}
	Set_Active(true);
	return S_OK;
}

HRESULT CFloor::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CFloor>();
	// Create Bufafer
	{
		CFloorBuffer::FLOORBUFFERDESC tFloorBufferDesc;
		tFloorBufferDesc.fWidth = m_fWidth;
		tFloorBufferDesc.fDepth = m_fDepth;
		tFloorBufferDesc.iSubjects = m_iSubject;
		m_pFloorBuffer = CloneCompData<CFloorBuffer>(PROTO_COMP_FLOORBUFFER, VOIDDATAS{ &tDesc, &tFloorBufferDesc });
		Add_Component(COM_BUFFER, m_pFloorBuffer);
	}
	{
		CCollider::COLLDESC tCollDesc;
		tCollDesc.vPosition = _float3(0.f, 0 -10.f, 0.f);
		tCollDesc.vScale = _float3(0.f, 0.f, 0.f);
		m_pCollider = CloneCompData< CAABBCollider>(PROTO_COMP_ABBCOLLIDER, VOIDDATAS{ &tDesc, &tCollDesc });
		Add_Component(COM_COLLIDER, m_pCollider);
	}
	{
		CCollider::COLLDESC tCollDesc;
		tCollDesc.vPosition = _float3(0.f, 0 - 10.f, 0.f);
		tCollDesc.vScale = _float3(0.f, 0.f, 0.f);
		m_pRegionCollider = CloneCompData< CAABBCollider>(PROTO_COMP_ABBCOLLIDER, VOIDDATAS{ &tDesc, &tCollDesc });
		Add_Component(COM_REGIONCOLLIDER, m_pCollider);
	}
	return S_OK;
}

HRESULT CFloor::Tick(const _double& _dTimeDelta) {
	return __super::Tick(_dTimeDelta);
}

HRESULT CFloor::Late_Tick(const _double& _dTimeDelta)
{
	RETURN_CHECK_FAILED(__super::Late_Tick(_dTimeDelta), E_FAIL);

	Get_Renderer()->Add_Render(RENDER_ACTOR, ThisShared<CActor>());
#ifdef _DEBUG
	m_pCollider->Add_RendererDebugActor(Get_Renderer());
#endif
	return S_OK;
}

HRESULT CFloor::Render(const HDC& _hDC)
{
	m_pFloorBuffer->Render(_hDC);
	return S_OK;
}
