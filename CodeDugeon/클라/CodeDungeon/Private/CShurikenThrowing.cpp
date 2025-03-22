#include "ClientDefines.h"
#include "UGameInstance.h"
#include "UCollider.h"
#include "UTransform.h"
#include "UMethod.h"
#include "CShurikenThrowing.h"
#include "UPawn.h"
#include "UParticle.h"
#include "UParticleSystem.h"
#include "UModel.h"
#include "UTrail.h"
#include "UVIBufferTrail.h"
#include "UNavigation.h"
#include "CHarlequinn.h"

CShurikenThrowing::CShurikenThrowing(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: CModelObjects(_spDevice, _wstrLayer, _eCloneType),
	m_ftraveledDistance{ 0 }, m_bisThrown{ false }, m_spTrail{nullptr}, m_spCurNavi{nullptr}
{
}

CShurikenThrowing::CShurikenThrowing(const CShurikenThrowing& _rhs)
	: CModelObjects(_rhs),
	m_ftraveledDistance{ 0 }, m_bisThrown{ false }, m_spTrail{ nullptr }, m_spCurNavi{ nullptr }
{
}

void CShurikenThrowing::Free()
{
}

HRESULT CShurikenThrowing::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CShurikenThrowing::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);

	SetModel(PROTO_RES_SHURIKENMODEL);

	SHURIKENDESC tShurikenDesc = UMethod::ConvertTemplate_Index<SHURIKENDESC>(_vecDatas, 0);
	GetTransform()->SetNewWorldMtx(tShurikenDesc._Worldm);

	UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(1.f, 1.f, 1.f);
	SHPTR<UCollider> Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));
	_wstring mainColliderTag = L"Main";
	AddColliderInContainer(mainColliderTag, Collider);
	Collider->SetScaleToFitModel(GetModel()->GetMinVertexPos(), GetModel()->GetMaxVertexPos());

	{
		UVIBufferTrail::TrailVertexCnt tDesc;
		tDesc.iMaxVertexCount = 100;
		m_spTrail = std::static_pointer_cast<UTrail>(spGameInstance->CloneActorAdd(PROTO_ACTOR_TRAIL, { &tDesc }));
		m_spTrail->SetActive(true);

		m_spTrail->SetColorTexture(L"Pink");
		m_spTrail->SetTrailShapeTexture(L"Noise_Thunder");
		m_spTrail->SetTrailNoiseTexture(L"GlowDiffuse");
	}

	{
		UNavigation::NAVDESC navDesc;
		m_spCurNavi = std::static_pointer_cast<UNavigation>(spGameInstance->CloneComp(PROTO_NAVI_INTERIOR, { &navDesc }));
		assert(nullptr != m_spCurNavi);
	}

	SetPawnType(PAWNTYPE::PAWN_PROJECTILE);

	return S_OK;
}

void CShurikenThrowing::ThrowShurikens(const _double& _dTimeDelta, _float3 _dir)
{
	if(m_bisThrown)
	{
		// 목표 지점까지의 거리 계산
		_float ActiveDistance = 100;

		// 이동 속도와 회전 속도 설정
		_float moveSpeed = 250;
		_float rotateSpeed = 720.0f; 

		// 이동 및 회전 로직 (프레임 단위로)
		if (m_ftraveledDistance < ActiveDistance)
		{
			_float3 movement = _dir * moveSpeed * _dTimeDelta;
			m_ftraveledDistance += movement.Length();

			// 수리켄 이동
			GetTransform()->TranslateDir(_dir, _dTimeDelta, moveSpeed);
			GetCurrentNavi()->FindCell(GetTransform()->GetPos());

			// 수리켄 회전 (z축 기준)
			GetTransform()->RotateTurn(_float3(1, 0, 0), rotateSpeed, _dTimeDelta);
		}
	}
}

void CShurikenThrowing::TickActive(const _double& _dTimeDelta)
{
	__super::TickActive(_dTimeDelta);
	for (auto& Containers : GetColliderContainer())
	{
		Containers.second->SetTranslate(GetModel()->GetCenterPos());
		Containers.second->SetTransform(GetTransform());
		for (auto& Containers : GetColliderContainer())
		{
			if (Containers.second) {
				SHPTR<DirectX::BoundingOrientedBox> OBB = Containers.second->GetOBB();

				_float3 plusPoint = Containers.second->GetHeightAdjustedPointFromCenter(OBB, false);
				_float3 minusPoint = Containers.second->GetHeightAdjustedPointFromCenter(OBB, true);
				_float4x4 AnimTransform = Containers.second->GetTransformMatrix();
				m_spTrail->SetRenderingTrail(m_bisThrown);
				m_spTrail->AddTrail(plusPoint, minusPoint);
			}
		}
	}


	
}


void CShurikenThrowing::LateTickActive(const _double& _dTimeDelta)
{
	__super::LateTickActive(_dTimeDelta);
	//for (auto& Colliders : GetColliderContainer())
	//	Colliders.second->AddRenderer(RENDERID::RI_NONALPHA_LAST);

	SHPTR<UCell> newCell{};
	_float3 vPosition{ GetTransform()->GetPos() };

	if (m_bisThrown)
	{
		if (false == GetCurrentNavi()->IsMove(vPosition, REF_OUT newCell))
		{	
			m_bisThrown = false;
			m_ftraveledDistance = 0;
		}
	}
}

HRESULT CShurikenThrowing::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	if (m_bisThrown)
	{
		/*SetOutline(true);*/
		__super::RenderActive(_spCommand, _spTableDescriptor);
	}
	else
		/*SetOutline(false);*/
	return S_OK;
}

HRESULT CShurikenThrowing::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	__super::RenderShadowActive(_spCommand, _spTableDescriptor);
	return S_OK;
}
HRESULT CShurikenThrowing::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	__super::RenderOutlineActive(_spCommand, _spTableDescriptor,_pass);
	return S_OK;
}

void CShurikenThrowing::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

