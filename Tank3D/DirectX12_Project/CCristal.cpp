#include "CCristal.h"
#include "CCubeBuffer.h"
#include "CRenderer.h"
#include "CMethod.h"
#include "CTransform.h"
#include "CGameScene.h"

shared_ptr<CActor> CCristal::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CCristal> pActor = CSmartPointerFactory<CCristal>::CreateShared(*this);
	if (FAILED(pActor->NativeConstruct(_tDatas))) {
		Safe_Free(pActor);
		DEBUG_MSG("CCristal::Clone To Failed");
	}
	return pActor;
}

void CCristal::Free()
{
	__super::Free();
}

HRESULT CCristal::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct())) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CCristal::NativeConstruct(VOIDDATAS _tDatas)
{
	if (FAILED(__super::NativeConstruct(_tDatas))) {
		return E_FAIL;
	}
	// Make CubeBuffer 
	{
		CHARDESC tDesc;
		if (_tDatas.size() > 0) {
			tDesc = CMethod::ConvertTemplate_Index<CHARDESC>(&_tDatas, 0);
		}

		if (nullptr != m_pCristalBuffer)
			m_pCristalBuffer->Set_Color(tDesc.vColor);

		shared_ptr<CTransform> pTransform = Get_Transform();
		if (nullptr != Get_Transform()) {
			pTransform->Set_Scale(tDesc.vBodyScale);
		}

		UpdateColliderScale();
	}
	Set_Active(true);
	return S_OK;
}

HRESULT CCristal::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CCharacter>();
	// Create Bufafer
	{
		m_pCristalBuffer = CloneCompData<CCubeBuffer>(PROTO_COMP_CUBEBUFFER, VOIDDATAS{ &tDesc });
		Add_Component(COM_BUFFER, m_pCristalBuffer);
	}
	return S_OK;
}

HRESULT CCristal::Tick(const _double& _dTimeDelta)
{
	return __super::Tick(_dTimeDelta);
}

HRESULT CCristal::Late_Tick(const _double& _dTimeDelta)
{
	Get_Renderer()->Add_Render(RENDERID::RENDER_ACTOR, ThisShared<CActor>());
	return S_OK;
}

HRESULT CCristal::Render(const HDC& _hDC)
{
	if (FAILED(__super::Render(_hDC)))
		return E_FAIL;

	m_pCristalBuffer->Render(_hDC);
	return S_OK;
}

void CCristal::Damaged(const shared_ptr<CCharacter>& _pChar)
{
	m_iHealth -= (int)(_pChar->Get_Damage());
	m_pGameScene.lock()->DeleteMonster(_pChar);
}
