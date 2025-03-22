#include "ToolDefines.h"
#include "TShowAnimModelObject.h"
#include "UAnimModel.h"
#include "UTransform.h"
#include "UShader.h"
#include "UGameInstance.h"
#include "UCollider.h"

TShowAnimModelObject::TShowAnimModelObject(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType) :
	UPawn(_spDevice, _wstrLayer, _eCloneType),
	m_spModel{nullptr}

{
}

TShowAnimModelObject::TShowAnimModelObject(const TShowAnimModelObject& _rhs) :
	UPawn(_rhs),
	m_spModel{ nullptr }
{
}

void TShowAnimModelObject::Free()
{
}

HRESULT TShowAnimModelObject::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT TShowAnimModelObject::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);
	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	UCollider::COLLIDERDESC tDesc;
	tDesc.vTranslation = _float3(0.f, 0.f, 0.f);
	tDesc.vScale = _float3(1.f, 1.f, 1.f);
	SHPTR<UCollider> Collider = static_pointer_cast<UCollider>(spGameInstance->CloneComp(PROTO_COMP_OBBCOLLIDER, { &tDesc }));

	_wstring mainColliderTag = L"Main";
	AddColliderInContainer(mainColliderTag, Collider);
	AddShader(PROTO_RES_ANIMMODELSHADER, RES_SHADER);
	AddShadowShader(PROTO_RES_SHADOWANIMSHADER, RES_SHADER);
	AddOutlineShader(PROTO_RES_ANIMDEPTHRECORDSHADER, RES_SHADER);
	AddNorPosShader(PROTO_RES_ANIMNORPOSSHADER, RES_SHADER);
	GetTransform()->SetScale(_float3(0.05f, 0.05f, 0.05f));
	return S_OK;
}

void TShowAnimModelObject::TickActive(const _double& _dTimeDelta)
{
	if (nullptr != m_spModel)
		m_spModel->TickAnimation(_dTimeDelta);
}

void TShowAnimModelObject::LateTickActive(const _double& _dTimeDelta)
{
	if (nullptr != m_spModel)
	{
		AddRenderGroup(RI_NONALPHA_MIDDLE);
		AddShadowRenderGroup(RI_SHADOW);
		AddOutlineRenderGroup(RI_DEPTHRECORD);
		AddNorPosRenderGroup(RI_NORPOS);
	}
}

HRESULT TShowAnimModelObject::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	if (nullptr != m_spModel)
	{
		__super::RenderActive(_spCommand, _spTableDescriptor);

		for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
		{
			// Bind Transform 
			GetTransform()->BindTransformData(GetShader());

			m_spModel->BindTexture(i, SRV_REGISTER::T0, TEXTYPE::TextureType_DIFFUSE, GetShader());
			m_spModel->BindTexture(i, SRV_REGISTER::T1, TEXTYPE::TextureType_NORMALS, GetShader());
			// Render
			m_spModel->Render(i, GetShader(), _spCommand);
		}
	}
	return S_OK;
}
HRESULT TShowAnimModelObject::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	if (nullptr != m_spModel)
	{
		__super::RenderShadowActive(_spCommand, _spTableDescriptor);

		for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
		{
			// Bind Transform 
			GetTransform()->BindTransformData(GetShadowShader());

			m_spModel->BindTexture(i, SRV_REGISTER::T0, TEXTYPE::TextureType_DIFFUSE, GetShadowShader());
			m_spModel->BindTexture(i, SRV_REGISTER::T1, TEXTYPE::TextureType_NORMALS, GetShadowShader());
			// Render
			m_spModel->Render(i, GetShadowShader(), _spCommand);
		}
	}
	return S_OK;
}
HRESULT TShowAnimModelObject::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	if (nullptr != m_spModel)
	{
		__super::RenderOutlineActive(_spCommand, _spTableDescriptor, true);

		for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
		{
			// Bind Transform 
			GetTransform()->BindTransformData(GetOutlineShader());

			// Render
			m_spModel->Render(i, GetOutlineShader(), _spCommand);
		}
	}
	if (nullptr != m_spModel)
	{
		__super::RenderOutlineActive(_spCommand, _spTableDescriptor, false);

		for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
		{			
			// Bind Transform 
			GetTransform()->BindTransformData(GetNorPosShader());

			// Render
			m_spModel->Render(i, GetNorPosShader(), _spCommand);
		}
	}
	return S_OK;
}
void TShowAnimModelObject::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}
