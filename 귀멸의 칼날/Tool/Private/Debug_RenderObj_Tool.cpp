#include "Debug_RenderObj_Tool.h"
#include "GameInstance.h"

CDebug_RenderObj_Tool::CDebug_RenderObj_Tool(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CDebug_RenderObj_Tool::CDebug_RenderObj_Tool(const CDebug_RenderObj_Tool& _rhs)
	: CGameObject(_rhs)
{
}

HRESULT CDebug_RenderObj_Tool::NativeConstruct_Prototype()
{
	if (FAILED(__super::Add_Component(COM_RENDERER, LEVEL_STATIC, PROTO_COMP_RENDERER, (CComponent**)&m_pRenderCom)))
		return E_FAIL;

	CRenderer::DEBUGRENDER* pDesc = m_pRenderCom->Get_DebugRender();
	if (nullptr != pDesc)
	{
		pDesc->bFog = false;
		pDesc->bShadow = false;
		pDesc->bOutline = false;
		pDesc->bEdge = false;
		pDesc->fShadowDepth = 10.f;
	}

	return S_OK;
}

HRESULT CDebug_RenderObj_Tool::NativeConstruct_Clone(void* pArg)
{
	return S_OK;
}

void CDebug_RenderObj_Tool::Tick(const _double& _dTimeDelta)
{

}

void CDebug_RenderObj_Tool::Late_Tick(const _double& _dTimeDelta)
{
}

HRESULT CDebug_RenderObj_Tool::Render()
{
	return S_OK;
}

HRESULT CDebug_RenderObj_Tool::RenderLight()
{
	return S_OK;
}

void CDebug_RenderObj_Tool::Extern_ImGuiClass()
{
	CRenderer::DEBUGRENDER* pDesc = m_pRenderCom->Get_DebugRender();
	if (nullptr == pDesc)
		return;

	ImGui::Text("SSAO");

	ImGui::Text("HDR");
	ImGui::DragFloat("MiddleGrey", &pDesc->fMiddleGrey, 0.01f);
	ImGui::DragFloat("LumWhiteSqr", &pDesc->fLumWhiteSqr, 0.01f);
	ImGui::DragFloat("fLScalePlusValue", &pDesc->fLScalePlusValue, 0.01f, 0.f);

	ImGui::Text("Shadow");
	ImGui::DragFloat("ShadowDepth", &pDesc->fShadowDepth, 0.01f, 0.f, 100.f);
	ImGui::DragFloat4("ShadowColor", &pDesc->vShadowColor.x, 0.01f, 0.f, 1.f);

	ImGui::Text("Blur");
	ImGui::DragFloat("Blur", &pDesc->fBlur);

	ImGui::Text("Fog");
	ImGui::DragFloat("fogEnd", &pDesc->fogEnd, 1.f, 10.f);
	ImGui::DragFloat("fogStart", &pDesc->fogStart, 1.f, 1.f);
	ImGui::DragFloat("fogExp", &pDesc->fogExp, 0.001f, 0.001f);
	ImGui::DragFloat("fTime", &pDesc->fTime);
	ImGui::DragFloat4("fogColor", &pDesc->fogColor.x, 0.01f, 0.f, 1.f);
	ImGui::DragInt("fogType", &pDesc->fogType, 1, 0, 2);

	ImGui::Text("Distortion");
	ImGui::DragFloat2("UvFlowValue", &pDesc->v2UvFlowValue.x, 1.f);
	ImGui::DragFloat("MultipleTimeValue", &pDesc->fMultipleTimeValue, 0.01f);
	ImGui::DragFloat("DistortionDivineValue", &pDesc->fDistortionDivineValue, 0.01f);

	ImGui::Text("MotionBlur");
	ImGui::DragInt("NumBlurValue", &pDesc->iNumBlurValue, 1);
	ImGui::DragInt("DivineVelocityValue", &pDesc->iDivineVelocityValue, 1);

	ImGui::Text("Rim");
	ImGui::DragFloat("rimPower", &pDesc->fRimLightPower, 1.f, 0.f);
	ImGui::DragFloat("rimMin", &pDesc->fRimMin, 0.01f, 0.f, 1.f);
	ImGui::DragFloat("rimMax", &pDesc->fRimMax, 0.01f, 0.f, 1.f);

	ImGui::Text("Outline");
	ImGui::DragFloat("minSeparation", &pDesc->minSeparation);
	ImGui::DragFloat("maxSeparation", &pDesc->maxSeparation);
	ImGui::DragFloat("minDistance", &pDesc->minDistance);
	ImGui::DragFloat("maxDistance", &pDesc->maxDistance);
	ImGui::DragInt("outlineSize", &pDesc->outlineSize, 1, 0);
	ImGui::DragFloat3("colorModifier", &pDesc->colorModifier.x, 0.001f, 0.f, 1.f);

	ImGui::Text("Debug Com");
	ImGui::Checkbox("DEBUG_MAP", &pDesc->bDebug[CRenderer::DEBUG_MAP]); ImGui::SameLine();
	ImGui::Checkbox("DEBUG_PLAYER", &pDesc->bDebug[CRenderer::DEBUG_PLAYER]); ImGui::SameLine();
	ImGui::Checkbox("DEBUG_ETC", &pDesc->bDebug[CRenderer::DEBUG_ETC]);

	ImGui::Text("Checkbox");
	ImGui::Checkbox("FadeOut", &pDesc->bFadeInOut);
	ImGui::Checkbox("Shade", &pDesc->bShade);
	ImGui::Checkbox("Shadow", &pDesc->bShadow);
	ImGui::Checkbox("HDR", &pDesc->bHDR);
	ImGui::Checkbox("Rimlight", &pDesc->bRimlight);
	ImGui::Checkbox("Outline", &pDesc->bOutline);
	ImGui::Checkbox("Fog", &pDesc->bFog);
	ImGui::Checkbox("Effect", &pDesc->bEffect);
	ImGui::Checkbox("MotionBlur", &pDesc->bMotionBlur);
	ImGui::Checkbox("Shake", &pDesc->bShake);
	ImGui::Checkbox("DebugCom", &pDesc->bDebugCom);
	ImGui::Checkbox("DebugMRT", &pDesc->bDebugMRT);
}

HRESULT CDebug_RenderObj_Tool::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CDebug_RenderObj_Tool::SetUp_Components()
{
	return S_OK;
}

CDebug_RenderObj_Tool* CDebug_RenderObj_Tool::Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext)
{
	CDebug_RenderObj_Tool* pInstance = new CDebug_RenderObj_Tool(_pDevice, _pContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CDebug_RenderObj_Tool::Create To Failed");
#endif
	}
	return pInstance;
}

CGameObject* CDebug_RenderObj_Tool::Clone(void* _pArg)
{
	CDebug_RenderObj_Tool* pInstance = new CDebug_RenderObj_Tool(*this);
	if (FAILED(pInstance->NativeConstruct_Clone(_pArg)))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CDebug_RenderObj_Tool::Clone To Failed");
#endif
	}
	return pInstance;
}

void CDebug_RenderObj_Tool::Free()
{
	Safe_Release(m_pRenderCom);
	__super::Free();
}
