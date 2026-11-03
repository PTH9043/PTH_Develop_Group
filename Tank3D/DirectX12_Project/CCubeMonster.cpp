#include "CCubeMonster.h"
#include "CControlHead.h"
#include "CGameInstance.h"
#include "CMethod.h"
#include "CTransform.h"
#include "COBBCollider.h"
#include "CShooter.h"
#include "CBullet.h"
#include "CAABBCollider.h"
#include "CCubeBuffer.h"
#include "CTank.h"
#include "CCristal.h"
#include "CRenderer.h"

shared_ptr<CActor> CCubeMonster::Clone(const VOIDDATAS& _tDatas)
{
	shared_ptr<CCubeMonster> pActor = CSmartPointerFactory<CCubeMonster>::CreateShared(*this);
	if (FAILED(pActor->NativeConstruct(_tDatas))) {
		Safe_Free(pActor);
		DEBUG_MSG("CCubeMonster::Clone To Failed");
	}
	return pActor;
}

void CCubeMonster::Free()
{
	__super::Free();
}

HRESULT CCubeMonster::NativeConstruct()
{
	return S_OK;
}

HRESULT CCubeMonster::NativeConstruct(VOIDDATAS _tDatas)
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

		if (nullptr != m_pCubeBuffer)
			m_pCubeBuffer->Set_Color(tDesc.vColor);

		shared_ptr<CTransform> pTransform = Get_Transform();
		if (nullptr != Get_Transform()) {
			pTransform->Set_Scale(tDesc.vBodyScale);
		}
	}

	{
		if (_tDatas.size() >= 1) {
			MONSTERSHOOTERDESC tMonsterDesc = CMethod::ConvertTemplate_Index<MONSTERSHOOTERDESC>(&_tDatas, 1);
			CreateMonster(tMonsterDesc);
		}
	}

	return S_OK;
}

HRESULT CCubeMonster::CreateComponent()
{
	if (FAILED(__super::CreateComponent())) {
		return E_FAIL;
	}
	CComponent::COMPDESC tDesc;
	tDesc.pActor = ThisShared<CCharacter>();
	// Create Bufafer
	{
		m_pCubeBuffer = CloneCompData<CCubeBuffer>(PROTO_COMP_CUBEBUFFER, VOIDDATAS{ &tDesc });
		Add_Component(COM_BUFFER, m_pCubeBuffer);
	}
	return S_OK;
}

HRESULT CCubeMonster::Tick(const _double& _dTimeDelta)
{
	if (FAILED(__super::Tick(_dTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCubeMonster::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);
	Get_Renderer()->Add_Render(RENDERID::RENDER_ACTOR, ThisShared<CActor>());

	return  S_OK;
}

HRESULT CCubeMonster::Render(const HDC& _hDC)
{
	if (FAILED(__super::Render(_hDC)))
		return E_FAIL;

	m_pCubeBuffer->Render(_hDC);
	return S_OK;
}

void CCubeMonster::CreateMonster(const MONSTERSHOOTERDESC& _tMonsterDesc)
{
	__super::CreateMonster(_tMonsterDesc);
}

void CCubeMonster::ControlMonster(const _double& _dTimeDelta)
{
	if (nullptr != Get_Target().lock()) {
		shared_ptr<CTransform> pTargetTR = Get_Target().lock()->Get_Transform();

		if (pTargetTR->Compute_Distance(Get_Transform()->Get_OriginPos()) <= 40.f) {
			_float3 vPos = pTargetTR->Get_Pos();
			vPos.y = Get_Transform()->Get_PosF3().y;
			Get_Transform()->LookAt(vPos.Get_Vector());

			if (false == Get_Target().lock()->Is_ShockingState()) {
				Get_Shooter()->Set_ShootDir(Get_Transform()->Get_Look());
				Get_Shooter()->Fire(ThisShared<CCharacter>(), Get_Floor().lock(), 0.f);
			}
		}
		Get_Transform()->Translate_TransNotY(GetCrilstal().lock()->Get_Transform(), _dTimeDelta, Get_MoveSpeed());
	}
	__super::ControlMonster(_dTimeDelta);
}
