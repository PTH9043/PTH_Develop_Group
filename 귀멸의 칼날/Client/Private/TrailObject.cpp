#include "TrailObject.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "GameInstance.h"

CTrailObject::CTrailObject(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CPawn(_pDevice, _pContext)
{
	m_StartMatrix = CMethod::XMMatrixIdentity_P();
	m_EndMatrix = CMethod::XMMatrixIdentity_P();
}

CTrailObject::CTrailObject(const CTrailObject& _rhs) 
	: CPawn(_rhs), m_StartMatrix(_rhs.m_StartMatrix), m_EndMatrix(_rhs.m_EndMatrix)
{
}

HRESULT CTrailObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CTrailObject::NativeConstruct_Clone(void* pArg)
{
	if (FAILED(__super::NativeConstruct_Clone(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Bones(pArg)))
		return E_FAIL;

	m_strChangeTextureName = m_strTrailTextureName;
	return S_OK;
}

void CTrailObject::Tick(const _double& _dTimeDelta)
{
	__super::Tick(_dTimeDelta);
}

void CTrailObject::Late_Tick(const _double& _dTimeDelta)
{
	__super::Late_Tick(_dTimeDelta);

	Update_Transform();
	if (nullptr != m_pVIBuffer_Trail)
		m_pVIBuffer_Trail->Update(_dTimeDelta, m_pTargetTransform->Get_WorldFloat4x4(), true, true);

	if (false == m_bActive)
		return;

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
}

HRESULT CTrailObject::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	{
		if (nullptr != m_pShaderCom)
			m_pShaderCom->Begin(0);

		if (nullptr != m_pVIBuffer_Trail)
			m_pVIBuffer_Trail->Render();
	}

	return S_OK;
}

HRESULT CTrailObject::RenderLight()
{
	return S_OK;
}

void CTrailObject::Extern_ImGuiClass()
{
	if (nullptr == m_pVIBuffer_Trail)
		return;

	CVIBuffer_Trail::TRAILBUFFERDESC* pDesc = nullptr;
	m_pVIBuffer_Trail->Change_TrailBufferDesc(pDesc);
	if (nullptr == pDesc)
		return;

	ImGui::DragFloat("Trail_StartValue", &pDesc->fStartValue, -0.001f, 1000.f);
	ImGui::DragFloat("Trail_EndValue", &pDesc->fEndValue, -0.001f, 1000.f);
}

void CTrailObject::Change_Texture(const _wstring& _strChangeTexture)
{
	m_strChangeTextureName = _strChangeTexture;
}

void CTrailObject::Return_Texture()
{
	m_strChangeTextureName = m_strTrailTextureName;
}

void CTrailObject::Update_TrailMatrix(const _float& _fStartValue, const _float& _fEndValue)
{
	CVIBuffer_Trail::TRAILBUFFERDESC* pDesc = nullptr;
	m_pVIBuffer_Trail->Change_TrailBufferDesc(pDesc);
	if (nullptr == pDesc)
		return;

	m_StartMatrix = CMethod::XMMatrixStoreFloat4x4_P(XMMatrixTranslation(0.f, 0.f, _fStartValue));
	m_EndMatrix = CMethod::XMMatrixStoreFloat4x4_P(XMMatrixTranslation(0.f, 0.f, _fEndValue));

	pDesc->pStartMatrix = &m_StartMatrix;
	pDesc->pEndMatrix = &m_EndMatrix;
	pDesc->fStartValue = _fStartValue;
	pDesc->fEndValue = _fEndValue;
}

HRESULT CTrailObject::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
	{
		// /*L"T_e_Skl_Wa_Slash001C"*/L"T_e_Plc_P1002_Atk_Trail001_C"
		SETUP_TEXTURE_SRV_NAME(m_pShaderCom, m_pTrailTextureCom, "g_DiffuseTexture", m_strChangeTextureName.c_str());
		SETUP_TEXTURE_SRV_NAME(m_pShaderCom, m_pNoiseTextureCom, "g_NoiseTexture", L"T_e_Skl_Fi_Birth_Noise00");
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CTrailObject::SetUp_Components()
{
	if (FAILED(__super::SetUp_Components()))
		return E_FAIL;

	/* For.Shader */
	if (FAILED(__super::Add_Component(COM_SHADER, LEVEL_STATIC, PROTO_COMP_SHADER_VTXTRAIL, (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CVIBuffer_Trail::TRAILBUFFERDESC tDesc;
	m_StartMatrix = CMethod::XMMatrixStoreFloat4x4_P(XMMatrixTranslation(0.f, 0.f, 100.f));
	tDesc.pStartMatrix = &m_StartMatrix;
	m_EndMatrix = CMethod::XMMatrixStoreFloat4x4_P(XMMatrixTranslation(0.f, 0.f, 60.f));
	tDesc.pEndMatrix = &m_EndMatrix;
	tDesc.PivotMatrix = CMethod::XMMatrixStoreFloat4x4_P(XMMatrixScaling(0.1f, 0.1f, 0.1f));
	tDesc.fStartValue = 100.f; 
	tDesc.fEndValue = 100.f;

	CVIBuffer_Trail::TRAILBUFFERSAVES tSaves;
	tSaves.iVerticesNum = 120;
	tSaves.tValue = tDesc;
	
	/* For.Buffer */
	if (FAILED(__super::Add_Component(COM_VIBUFFER, LEVEL_STATIC, PROTO_COMP_VIBUFFER_TRAIL, (CComponent**)&m_pVIBuffer_Trail, &tSaves)))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	{
		m_pTrailTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component(g_iCurrentLevel, PROTO_COMP_TEXTURE_TRAIL));
		m_pNoiseTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component(g_iCurrentLevel, PROTO_COMP_TEXTURE_NOISE));
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CTrailObject::SetUp_Bones(void* _pData)
{
	RETURN_IFNOT(nullptr == _pData, E_FAIL);

	TRAILOBJ tDesc = *static_cast<TRAILOBJ*>(_pData);
	// Return
	{
		RETURN_IFNOT(nullptr == tDesc.pTargetModel , E_FAIL);
		RETURN_IFNOT(nullptr == tDesc.pTargetTrasform, E_FAIL);
	}

	m_pTargetModel = tDesc.pTargetModel;
	m_pTargetTransform = tDesc.pTargetTrasform;
	Safe_AddRef(m_pTargetModel);
	Safe_AddRef(m_pTargetTransform);

	m_pStartBone = m_pTargetModel->Get_HierarchyNodes(tDesc.strStartBoneName.c_str());
	m_pEndBone = m_pTargetModel->Get_HierarchyNodes(tDesc.strFinishBoneName.c_str());
	Safe_AddRef(m_pStartBone);
	Safe_AddRef(m_pEndBone);
	// Return
	{
		RETURN_IFNOT(nullptr == m_pStartBone, E_FAIL);
		RETURN_IFNOT(nullptr == m_pTargetTransform, E_FAIL);
	}

	return S_OK;
}

void CTrailObject::Update_Transform()
{
	if (nullptr == m_pStartBone || nullptr == m_pEndBone || nullptr == m_pTransformCom || nullptr == m_pColliderCom || nullptr == m_pModelCom)
		return;

	// Start
	{
		_float4x4 mWorld;
		XMStoreFloat4x4(&mWorld,
			
			m_pModelCom->Get_PivotMatrix() * m_pStartBone->Get_CombinedTransformationMatrix() *
			m_pTargetModel->Get_PivotMatrix() * m_pTargetTransform->Get_WorldMatrix());

		m_StartMatrix = mWorld;
	}
	// End
	{
		_float4x4 mWorld;
		XMStoreFloat4x4(&mWorld,
			m_pModelCom->Get_PivotMatrix() *
			m_pEndBone->Get_CombinedTransformationMatrix() *
			m_pTargetModel->Get_PivotMatrix());

		m_EndMatrix = mWorld;
	}
}

CTrailObject* CTrailObject::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CTrailObject* pInstance = new CTrailObject(_pDevice, _pContext);
	CHECK_CREATE_PROTOTYPE_NOTVALUE(pInstance, "CTrailObject::Create To Failed");
	return pInstance;
}

CGameObject* CTrailObject::Clone(void* pArg)
{
	CTrailObject* pInstance = new CTrailObject(*this);
	CHECK_CREATE_CLONE(pInstance, pArg, "CTrailObject::Clone To Failed");
	return pInstance;
}

void CTrailObject::Free()
{
	Safe_Release(m_pStartBone);
	Safe_Release(m_pEndBone);
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pVIBuffer_Trail);
	Safe_Release(m_pNoiseTextureCom);
	Safe_Release(m_pTrailTextureCom);

	__super::Free();
}
