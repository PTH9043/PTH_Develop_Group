#include "ClientDefines.h"
#include "CModelObjects.h"
#include "UGameInstance.h"
#include "UCollider.h"
#include "UModel.h"
#include "UTransform.h"
#include "UShaderConstantBuffer.h"
#include "UShader.h"
#include "UModelMaterial.h"

CModelObjects::CModelObjects(CSHPTRREF<UDevice> _spDevice, const _wstring& _wstrLayer, const CLONETYPE& _eCloneType)
	: UPawn(_spDevice, _wstrLayer, _eCloneType),
	m_spModel{ nullptr }, m_spCollider{nullptr}, m_bDrawOutline{false}
{
}

CModelObjects::CModelObjects(const CModelObjects& _rhs) :
	UPawn(_rhs),
	m_spModel{ nullptr }, m_spCollider{ nullptr }, m_bDrawOutline{false}
{
}

void CModelObjects::Free()
{
}

HRESULT CModelObjects::NativeConstruct()
{
	return __super::NativeConstruct();
}

HRESULT CModelObjects::NativeConstructClone(const VOIDDATAS& _vecDatas)
{
	RETURN_CHECK_FAILED(__super::NativeConstructClone(_vecDatas), E_FAIL);

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spShaderTexCheckBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::MODELCHECKBUF, static_cast<_int>(sizeof(HasTex)));
	m_spShaderCheckPointBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::OUTLINESCALE, sizeof(_bool));
	{
		m_spScaleOutlineBuffer = CreateNative<UShaderConstantBuffer>(GetDevice(), CBV_REGISTER::BLOODTIMERBUFFER, sizeof(_bool));
	}
	AddShader(PROTO_RES_MODELSHADER, RES_SHADER);
	AddShadowShader(PROTO_RES_SHADOWSHADER, RES_SHADER);
	AddOutlineShader(PROTO_RES_DEPTHRECORDSHADER, RES_SHADER);
	AddNorPosShader(PROTO_RES_NORPOSSHADER, RES_SHADER);
	GetTransform()->SetScale(_float3(0.05f, 0.05f, 0.05f));
	return S_OK;
}

void CModelObjects::SetModel(const _wstring& _ProtoModelName)
{
	if (m_spModel != nullptr)
		m_spModel.reset();

	SHPTR<UGameInstance> spGameInstance = GET_INSTANCE(UGameInstance);
	m_spModel = static_pointer_cast<UModel>(spGameInstance->CloneResource(_ProtoModelName));

}

void CModelObjects::CalculateAndSetCollider()
{
}

void CModelObjects::TickActive(const _double& _dTimeDelta)
{
}

void CModelObjects::LateTickActive(const _double& _dTimeDelta)
{
	if (nullptr != m_spModel)
	{
		AddRenderGroup(RI_NONALPHA_MIDDLE);
		AddShadowRenderGroup(RI_SHADOW);
	
		if (m_bDrawOutline) {
			AddOutlineRenderGroup(RI_DEPTHRECORD);
			AddNorPosRenderGroup(RI_NORPOS);
		}		
	}
}

HRESULT CModelObjects::RenderActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	if (nullptr != m_spModel)
	{
		__super::RenderActive(_spCommand, _spTableDescriptor);

		for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
		{
			// Bind Transform 
			GetTransform()->BindTransformData(GetShader());

			m_spModel->BindTexture(i, SRV_REGISTER::T0, TEXTYPE::TextureType_DIFFUSE, GetShader());
			m_spModel->BindTexture(i, SRV_REGISTER::T1, TEXTYPE::TextureType_SPECULAR, GetShader());
			m_spModel->BindTexture(i, SRV_REGISTER::T2, TEXTYPE::TextureType_NORMALS, GetShader());
			if (m_spModel->GetMaterials()[0]->IsEmpty(TextureType_DIFFUSE)) {
				HasTex[0] = 0;
			}
			else {
				HasTex[0] = 1;
			}
			if (m_spModel->GetMaterials()[0]->IsEmpty(TextureType_SPECULAR)) {
				HasTex[1] = 0;
			}
			else {
				HasTex[1] = 1;
			}
			if (m_spModel->GetMaterials()[0]->IsEmpty(TextureType_NORMALS)) {
				HasTex[2] = 0;
			}
			else {
				HasTex[2] = 1;
			}

			GetShader()->BindCBVBuffer(m_spShaderTexCheckBuffer, &HasTex, sizeof(HasTex));
			GetShader()->BindCBVBuffer(m_spShaderCheckPointBuffer, &isCheckpoint, sizeof(_bool));
			// Render
			m_spModel->Render(i, GetShader(), _spCommand);
		}
	}
	return S_OK;
}

HRESULT CModelObjects::RenderShadowActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor)
{
	if (nullptr != m_spModel)
	{
		__super::RenderShadowActive(_spCommand, _spTableDescriptor);

		for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
		{
			// Bind Transform 
			GetTransform()->BindTransformData(GetShadowShader());

			// Render
			m_spModel->Render(i, GetShadowShader(), _spCommand);
		}
	}
	return S_OK;
}
HRESULT CModelObjects::RenderOutlineActive(CSHPTRREF<UCommand> _spCommand, CSHPTRREF<UTableDescriptor> _spTableDescriptor, _bool _pass)
{
	
		

		if (nullptr != m_spModel&&m_bDrawOutline && _pass)
		{
			__super::RenderOutlineActive(_spCommand, _spTableDescriptor, _pass);

			for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
			{
				// Bind Transform 
				GetTransform()->BindTransformData(GetOutlineShader());

				// Render
				m_spModel->Render(i, GetOutlineShader(), _spCommand);
			}
		}
		if (nullptr != m_spModel && m_bDrawOutline && !_pass)
		{
			__super::RenderOutlineActive(_spCommand, _spTableDescriptor, _pass);

			for (_uint i = 0; i < m_spModel->GetMeshContainerCnt(); ++i)
			{
				// Bind Transform 
				GetTransform()->BindTransformData(GetNorPosShader());
				GetNorPosShader()->BindCBVBuffer(m_spScaleOutlineBuffer, &m_OutlineWithScale, sizeof(_bool));

				// Render
				m_spModel->Render(i, GetNorPosShader(), _spCommand);
			}
		}
	return S_OK;
}
void CModelObjects::Collision(CSHPTRREF<UPawn> _pEnemy, const _double& _dTimeDelta)
{
}

