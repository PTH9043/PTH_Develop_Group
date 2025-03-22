#include "MainView_Tool.h"
#include "..\Public\MainApp_Tool.h"
#include "ImGuiClass_Manager_Tool.h"
#include "Camera_Tool.h"
#include "GameInstance.h"
#include "Level_Loading_Tool.h"

#include "ImGui_UI_Tool.h"
#include "UI_Tool.h"
#include "Camera_Free_Tool.h"

// jinju 
#include "MapGround_Tool.h"
#include "MapObj_Tool.h"
#include "Monster_Tool.h"
// kang
#include "AnimViewerModel.h"

// Effect
#include "EffectObject.h"
#include "ImageEffect.h"
#include "StaticMeshEffect.h"
#include "AnimMeshEffect.h"

#include "Sword_Tool.h"
#include "Test_Instance.h"
#include "Item_Tool.h"
#include "Animator.h"
#include "Camera_Cinematic_Tool.h"
#include "CutSceneModel_Tool.h"


CMainApp_Tool::CMainApp_Tool()
	: m_pGameInstance(CGameInstance::Get_Instance())
	, m_pImGuiClassManager_Tool(CImGuiClass_Manager_Tool::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pImGuiClassManager_Tool);
}

HRESULT CMainApp_Tool::Native_Construct()
{
	srand((_uint)time(NULL));

	_float3 a = _float3(0.f, 0.f, 0.f) + XMVectorSet(1.f, 1.f, 1.f, 1.f);

	GRAPHICDESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWindowed = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinCX = g_iWinSizeX;
	GraphicDesc.iWinCY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Load_FirstFolder(L"..\\..\\Resources")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	CFrustCulling::FRUSTDESC tFrustDesc = CFrustCulling::FRUSTDESC(60.f, _float(g_iWinSizeX) / g_iWinSizeY, 0.2f, 300.f);
	if (FAILED(m_pGameInstance->NativeConstruct_FrustCull(m_pDevice, m_pContext, tFrustDesc)))
		return E_FAIL;

	if (FAILED(Ready_Static_GameObj()))
		return E_FAIL;

	if (FAILED(Ready_Static_Component()))
		return E_FAIL;

	if (FAILED(m_pImGuiClassManager_Tool->NativeConstruct(g_hWnd, m_pDevice, m_pContext, IMGCLASS_END)))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_TOOL)))
		return E_FAIL;

	return S_OK;
}

void CMainApp_Tool::Tick(const _double& _dTimeDelta)
{

	if (false == g_bToolReset)
	{
		m_pGameInstance->Tick_Engine(_dTimeDelta);
		m_pImGuiClassManager_Tool->Tick(_dTimeDelta);
		m_pGameInstance->LateTick_Engine(_dTimeDelta);
		m_pImGuiClassManager_Tool->Late_Tick(_dTimeDelta);
	}
#ifdef _DEBUG
	m_dTimeAcc += _dTimeDelta;
#endif
}

HRESULT CMainApp_Tool::Render()
{
	if (false == g_bToolReset)
	{
		m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
		m_pGameInstance->Clear_DepthStencil_View();
		m_pRendererComp->Draw();
		m_pImGuiClassManager_Tool->Render();

		m_pGameInstance->Render_Engine();
		m_pGameInstance->Present();
	}
	return S_OK;
}

HRESULT CMainApp_Tool::Open_Level(const LEVEL& _eLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading_Tool::Create(m_pDevice, m_pContext, _eLevel))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp_Tool::Ready_Static_Component()
{
	m_pRendererComp = CRenderer::Create(m_pDevice, m_pContext);
	Safe_AddRef(m_pRendererComp);
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_RENDERER, m_pRendererComp)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxTex.hlsl"),
			VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXTRAIL,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxTrail.hlsl"),
			VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXNORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxNorTex.hlsl"),
			VTXNORTEX_DECLARATION::Element, VTXNORTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXCUBE,
		CShader::Create(m_pDevice, m_pContext, L"Shader_VtxCubeTex.hlsl",
			VTXCUBETEX_DECLARATION::Element, VTXCUBETEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL,
		CShader::Create(m_pDevice, m_pContext, L"Shader_VtxModel.hlsl",
			VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL,
		CShader::Create(m_pDevice, m_pContext, L"Shader_VtxAnimModel.hlsl",
			VTXANIMMODEL_DECLARATION::Element, VTXANIMMODEL_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXPOINTINSTNACE,
		CShader::Create(m_pDevice, m_pContext, L"Shader_Point_Instance.hlsl",
			VTXEFFECTINSTANCE_DECLARATION::Element, VTXEFFECTINSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXRECTINSTNACE,
		CShader::Create(m_pDevice, m_pContext, L"Shader_Rect_Instance.hlsl",
			VTXRECTINSTANCE_DECLARATION::Element, VTXRECTINSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL_INSTANCE,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_AnimModel_Instance.hlsl"),
			VTXANIMMODEL_INSTANCE_DECLARATION::Element, VTXANIMMODEL_INSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL_INSTANCE,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Model_Instance.hlsl"),
			VTXMODEL_INSTNACE_DECLARATION::Element, VTXMODEL_INSTNACE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_UI,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_UI.hlsl"),
			VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX_EFFECT,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxTex_Effect.hlsl"),
			VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL_EFFECT,
		CShader::Create(m_pDevice, m_pContext, L"Shader_VtxModel_Effect.hlsl",
			VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement))))
		return E_FAIL;


	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT,
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Effect_Instance */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_VIBUFFER_EFFECTINSTANCE,
		CVIBuffer_Effect_Instance::Create(m_pDevice, m_pContext, 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_VIBUFFER_TRAIL,
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	PATHS pPaths = PATHS{ L"Texture" };
	/* For.Prototype_Component_Texture_UI_Test */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Test"),
		CTexture::Create(m_pDevice, m_pContext, &pPaths))))
		return E_FAIL;

	PATHS pPath = PATHS{ L"Dialogue" };
	/* For.Prototype_Component_Texture_UI_Dialogue */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Dialogue"),
		CTexture::Create(m_pDevice, m_pContext, &pPath))))
		return E_FAIL;


	pPaths = PATHS{ L"NoiseTexture" };
	/* For.Prototype_Component_Texture_UI_Test */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NoiseTextures"),
		CTexture::Create(m_pDevice, m_pContext, &pPaths))))
		return E_FAIL;

	//////
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Bazzi"), TEXT("../../Resources/Data/Font/Bazzi.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Maru36"), TEXT("../../Resources/Data/Font/Maru36.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Maru24"), TEXT("../../Resources/Data/Font/Maru24.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Maru18"), TEXT("../../Resources/Data/Font/Maru18.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Maru16"), TEXT("../../Resources/Data/Font/Maru16.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER, CCollider::Create(m_pDevice, m_pContext,
		CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER, CCollider::Create(m_pDevice, m_pContext,
		CCollider::TYPE_OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER, CCollider::Create(m_pDevice, m_pContext,
		CCollider::TYPE_SPHERE))))
		return E_FAIL;

	// AnimViewerPrototype Animator Component
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_ANIMATOR,
		CAnimator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// EffectResources
	{
		PATHS vecTexturePath = { L"Model",L"EffectModel",L"Textures" };
		if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffectDiffuse"),
			CTexture::Create(m_pDevice, m_pContext, &vecTexturePath))))
			return E_FAIL;

		vecTexturePath = { L"NoiseTexture" };
		if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffectMoise"),
			CTexture::Create(m_pDevice, m_pContext, &vecTexturePath))))
			return E_FAIL;

		vecTexturePath = { L"MaskTexture" };
		if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EffectMask"),
			CTexture::Create(m_pDevice, m_pContext, &vecTexturePath))))
			return E_FAIL;

		// 사운드 로드를 한다.
		/*PATHS Path = { L"Sound", L"Battle" };
		if (FAILED(m_pGameInstance->Load_SoundResource(&Path)))
			return E_FAIL;

		Path = { L"Sound", L"Story" };
		if (FAILED(m_pGameInstance->Load_SoundResource(&Path)))
			return E_FAIL;*/

		PATHS vecMeshPath = { L"Model",L"EffectModel",L"FBX_Converter" };
		FILEGROUP* pStaticMeshFolder = m_pGameInstance->Find_Folder(&vecMeshPath);

		for (auto& iter : pStaticMeshFolder->vecFileDataList)
		{
			_tchar* szProtoTag = new _tchar[MAX_PATH];
			lstrcpy(szProtoTag, TEXT("Prototype_Component_Model_"));
			lstrcat(szProtoTag, iter.strFileName.c_str());
			m_vecEffectProtoTag.push_back(szProtoTag);

			_matrix PivotMatrix = XMMatrixIdentity();
			if (!lstrcmp(L"PlaneCustomX", iter.strFileName.c_str()))
				PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(1.27f, 0.f, 0.f);
			else if (!lstrcmp(L"PlaneCustomY", iter.strFileName.c_str()))
				PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, 1.27f, 0.f);
			else
				PivotMatrix *= XMMatrixScaling(0.01f, 0.01f, 0.01f);

			if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, szProtoTag,
				CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, iter.strFilePath, PivotMatrix))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CMainApp_Tool::Ready_Static_GameObj()
{
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CAMERA_TOOL, CCamera_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_TOOL, CUI_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CAMERA_FREE_TOOL, CCamera_Free_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// mapviewer ��ü prototype
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPGROUND, CMapGround_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPSKY, CMapGround_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPOBJECT, CMapObj_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MONSTER, CMonster_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	// AnimViewer Prototype Animation Model
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ANIMVIEWERMODEL, CAnimViewerModel::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_SWORD, CSword_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	// Effect Object
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_TOOL, CEffectObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_IMAGE_TOOL, CImageEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_STATICMESH_TOOL, CStaticMeshEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_ANIMMESH_TOOL, CAnimMeshEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_INSTANCE_TOOL, CInstanceEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_TEST_INSTANCE, CTest_Instance::Create(m_pDevice,
		m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_TEST_ITEM, CItem_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CINEMATIC_CAMREA_TOOL,
		CCamera_Cinematic_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CUTSCENEMODEL_TOOL,
		CCutSceneModel_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CMainApp_Tool* CMainApp_Tool::Create()
{
	CMainApp_Tool* pInstance = new CMainApp_Tool();
	if (FAILED(pInstance->Native_Construct()))
	{
		Safe_Release(pInstance);
#ifdef _DEBUG
		MSG_BOX("CMainApp_Tool::Create To Failed");
#endif
	}
	return pInstance;
}

void CMainApp_Tool::Free()
{
	for (auto& iter : m_vecEffectProtoTag)
		Safe_Delete_Array(iter);
	Safe_Release(m_pRendererComp);
	/*  Device, Context */
	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);

	CImGuiClass_Manager_Tool::Destroy_Instance();
	Safe_Release(m_pImGuiClassManager_Tool);
	/* CGameInstance */
	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
}
