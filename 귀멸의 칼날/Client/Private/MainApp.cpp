
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "ImGuiClass_Manager.h"
#include "Battle_Manager.h"
#include "Camera_Manager.h"
#include "CutScene_Manager.h"
#include "Level_Loading.h"

#include "Story_Tanjiro.h"
#include "Tanjiro.h"
#include "Zenitsu.h"
#include "Akaza.h"

#include "MapCollider.h"
#include "MapLight.h"
#include "MapObject.h"
#include "MapGround.h"
#include "MapGroupObject.h"

#include "MainView.h"
#include "Camera_Cinematic.h"
#include "CameraLight.h"
#include "Camera_Free.h"
#include "Camera_CutScene.h"
#include "ShaderView.h"

#include "Projectile.h"
#include "WindBall.h"

#include "UI.h"
#include "UI_HP.h"
#include "UI_Combo.h"
#include "UI_Ultimate.h"
#include "UI_Minimap.h"
#include "UI_CharSelect.h"
#include "UI_Dialogue.h"
#include "UI_Select_Model.h"
#include "UI_CharSelect.h"
#include "UI_Icon.h"
#include "Minimap_Track.h"
#include "Select_Item.h"
#include "UI_Quest.h"
#include "UI_DirMap.h"
#include "UI_DirIcon.h"
#include "UI_MiniGame.h"
#include "MiniGame_Note.h"
#include "MiniGame_Model.h"
#include "UI_Boss.h"
#include "UI_Lines.h"

#include "NPC_Item.h"
#include "Animal.h"
#include "NPC.h"

#include "TrailObject.h"
#include "Sword.h"
#include "Sheath.h"

#include "Sabito.h"
#include "Debug_RenderObj.h"

// ONI 1 2 3 4 
#include "Zako.h"
#include "Zako2.h"
#include "Zako3.h"
#include "Zako3.h"

// Boss
#include "TeOni.h"
#include "TeOni_Rock.h"
#include "TeOni_Te.h"

#include "MainView.h"

#include "BrokenRock.h"




CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance()),
	m_pImGuiManager(CImGuiClass_Manager::Get_Instance()),
	m_pBattleManager(CBattle_Manager::Get_Instance()),
	m_pCameraManager(CCamera_Manager::Get_Instance()),
	m_pCutSceneManager(CCutScene_Manager::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pImGuiManager);
	Safe_AddRef(m_pBattleManager);
	Safe_AddRef(m_pCameraManager);
	Safe_AddRef(m_pCutSceneManager);
}

HRESULT CMainApp::NativeConstruct()
{
	g_iCurrentLevel = LEVEL_LOGO;

	if (FAILED(m_pGameInstance->Load_FirstFolder(L"..\\..\\Resources")))
		return E_FAIL;

	GRAPHICDESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWindowed = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinCX = g_iWinSizeX;
	GraphicDesc.iWinCY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	CFrustCulling::FRUSTDESC tFrustDesc = CFrustCulling::FRUSTDESC(60.f, _float(g_iWinSizeX) / g_iWinSizeY, 0.2f, 3000.f);
	if (FAILED(m_pGameInstance->NativeConstruct_FrustCull(m_pDevice, m_pContext, tFrustDesc)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Components()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(m_pImGuiManager->NativeConstruct(g_hWnd, m_pDevice, m_pContext, IMGCLASS_END)))
		return E_FAIL;

	if (FAILED(m_pCutSceneManager->NativeConstruct(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Tick(const _double& _dTimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	// 레벨에 따른 렌더러 설정
	SetUp_RendererDesc();

#ifdef _DEBUG

	m_TimeAcc += _dTimeDelta;

	CImGuiClass_Manager* pImGuiManager = GET_INSTANCE(CImGuiClass_Manager);
	{
		_bool* pStopFrame = pImGuiManager->Is_StopFrame();
		if (nullptr != pStopFrame)
		{
			if (false == *pStopFrame)
			{
				m_pGameInstance->Tick_Engine(_dTimeDelta);
				m_pImGuiManager->Tick(_dTimeDelta);
				m_pCutSceneManager->Tick(_dTimeDelta);
				m_pBattleManager->Update(_dTimeDelta);
				m_pGameInstance->LateTick_Engine(_dTimeDelta);
				m_pImGuiManager->Late_Tick(_dTimeDelta);
				m_pCutSceneManager->Late_Tick(_dTimeDelta);
			}
			else
			{
				m_pCutSceneManager->Tick(_dTimeDelta);
				m_pImGuiManager->Tick(_dTimeDelta);
				//m_pBattleManager->Update(_dTimeDelta);
				m_pCutSceneManager->Late_Tick(_dTimeDelta);
				m_pImGuiManager->Late_Tick(_dTimeDelta);
			}
		}
	}
	RELEASE_INSTANCE(CImGuiClass_Manager);
#else
	m_pGameInstance->Tick_Engine(_dTimeDelta);
	m_pImGuiManager->Tick(_dTimeDelta);
	m_pCutSceneManager->Tick(_dTimeDelta);
	m_pBattleManager->Update(_dTimeDelta);
	m_pGameInstance->LateTick_Engine(_dTimeDelta);
	m_pImGuiManager->Late_Tick(_dTimeDelta);
	m_pCutSceneManager->Late_Tick(_dTimeDelta);


#endif
}

HRESULT CMainApp::Render()
{
#ifdef _DEBUG

	CImGuiClass_Manager* pImGuiManager = GET_INSTANCE(CImGuiClass_Manager);
	{

		_bool* pStopFrame = pImGuiManager->Is_StopFrame();
		if (nullptr != pStopFrame)
		{
			if (false == *pStopFrame)
			{
				m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
				m_pGameInstance->Clear_DepthStencil_View();

				// Render all Gameobjects in every level
				m_pRenderer->Draw();
				m_pGameInstance->Render_Engine();
				m_pCutSceneManager->Render();
				m_pImGuiManager->Render();

				++m_iNumRender;
				if (m_TimeAcc >= 1.0)
				{
					wsprintf(m_szFPS, L"FPS : %d", m_iNumRender);
					m_iNumRender = 0;
					m_TimeAcc = 0.0;
				}
				m_pGameInstance->Render_Font(L"Font_Maru18", m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));


				m_pGameInstance->Present();
			}
			else
			{
				m_pCutSceneManager->Render();
				m_pImGuiManager->Render();
			}

		}
	}
	RELEASE_INSTANCE(CImGuiClass_Manager);


#else
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	// Render all Gameobjects in every level
	m_pRenderer->Draw();
	m_pGameInstance->Render_Engine();

	m_pImGuiManager->Render();
	m_pCutSceneManager->Render();
	m_pGameInstance->Present();
#endif
	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL; 

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevel))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_STORY_TANJIRO,
		CStory_Tanjiro::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_PLAYER_TANJIRO,
		CTanjiro::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_PLAYER_ZENITSU,
		CZenitsu::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_PLAYER_AKAZA,
		CAkaza::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_PLAYER_SABITO,
		CSabito::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_BOSS_TEONI,
		CTeOni::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ITEM_TEONIROCK,
		CTeOni_Rock::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ITEM_TEONITE,
		TeOni_Te::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPOBJECT_BROKENROCK,
		CBrokenRock::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPOBJECT_TEST,
		CMapObject::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CAMERA_CINEMATIC,
		CCamera_Cinematic::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CAMERA_FREE,
		CCamera_Free::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_LIGHT_CAMERA,
		CCameraLight::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CUTSCENE_CAMERA,
		CCamera_CutScene::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	// 맵 오브젝트 추가 
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPOBJECT_OBJ,
		CMapObject::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	// 맵 인스턴스 오브젝트 추가 
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPOBJECT_INSTANCE,
		CMapGroupObject::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	// 맵 땅바닥 오브젝트 추가 
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPOBJECT_GROUND,
		CMapGround::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPOBJECT_SKY,
		CMapGround::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	// 맵 라이트 테스트 추가
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MAPOBJECT_LIGHT,
		CMapLight::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	// 스토리맵 콜라이더 테스트 추가
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_STORYMAP_COLLIDER,
		CMapCollider::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT, CEffectObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_IMAGE, CImageEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_STATICMESH, CStaticMeshEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_ANIMMESH, CAnimMeshEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFECT_INSTANCE, CInstanceEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_TRAILOBJ, CTrailObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_SWORD, CSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_SHEATH, CSheath::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_PROJECTILE, CProjectile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_WATERBALL, CWindBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//UI
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI,
		CUI::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_HP,
		CUI_HP::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_COMBO,
		CUI_Combo::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_ULTIMATE,
		CUI_Ultimate::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_DIRMAP,
		CUI_DirMap::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_MINITRACK,
		CMinimap_Track::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_DIALOGUE,
		CUI_Dialogue::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_SELECTMODEL,
		CUI_Select_Model::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_CHARSELECT,
		CUI_CharSelect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_SELECT_ITEM,
		CSelect_Item::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_ICON,
		CUI_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_QUEST,
		CUI_Quest::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_DIRICON,
		CUI_DirIcon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_MINIGAME,
		CUI_MiniGame::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_NOTE,
		CMiniGame_Note::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_BOSS,
		CUI_Boss::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_LINES,
		CUI_Lines::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	//동물
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ANIMAL,
		CAnimal::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	// oni zako / 오니 자코 
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ONI_ZAKO,
		CZako::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ONI_ZAKO2,
		CZako2::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ONI_ZAKO3,
		CZako3::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	//NPC
	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_NPC,
		CNPC::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_NPC_ITEM,
		CNPC_Item::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_MINIMGAME_MODEL,
		CMiniGame_Model::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Components()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(m_pDevice, m_pContext);
	Safe_AddRef(m_pRenderer);
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_RENDERER, m_pRenderer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_UI,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_UI.hlsl"),
			VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement))))
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

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX_EFFECT,
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxTex_Effect.hlsl"),
			VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXPOINTINSTNACE,
		CShader::Create(m_pDevice, m_pContext, L"Shader_Point_Instance.hlsl",
			VTXEFFECTINSTANCE_DECLARATION::Element, VTXEFFECTINSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXRECTINSTNACE,
		CShader::Create(m_pDevice, m_pContext, L"Shader_Rect_Instance.hlsl",
			VTXRECTINSTANCE_DECLARATION::Element, VTXRECTINSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL_EFFECT,
		CShader::Create(m_pDevice, m_pContext, L"Shader_VtxModel_Effect.hlsl",
			VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODELINSTANCE,
		CShader::Create(m_pDevice, m_pContext, L"Shader_AnimModel_Instance.hlsl",
			VTXANIMMODEL_INSTANCE_DECLARATION::Element, VTXANIMMODEL_INSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODELINSTANCE,
		CShader::Create(m_pDevice, m_pContext, L"Shader_Model_Instance.hlsl",
			VTXMODEL_INSTNACE_DECLARATION::Element, VTXMODEL_INSTNACE_DECLARATION::iNumElement))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT,
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Transform*/
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_TRANSFORM,
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Effect_Instance */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_VIBUFFER_EFFECTINSTANCE,
		CVIBuffer_Effect_Instance::Create(m_pDevice, m_pContext, 1))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Effect_Instance */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_VIBUFFER_TRAIL,
		CVIBuffer_Trail::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	// 콜라이더 
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_AABBCOLLIDER,
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_OBBCOLLIDER,
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SPHERECOLLIDER,
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	//UI 텍스쳐
	PATHS Path = PATHS{ L"Texture" };
	/* For.Prototype_Component_Texture_UI_Test */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Test"),
		CTexture::Create(m_pDevice, m_pContext, &Path))))
		return E_FAIL;

	//UI 텍스쳐
	Path = PATHS{ L"Texture", L"Lines" };
	/* For.Prototype_Component_Texture_UI_Lines */
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Lines"),
		CTexture::Create(m_pDevice, m_pContext, &Path))))
		return E_FAIL;

	//UI 대사 
	Path = PATHS{ L"Dialogue" };
	/* For.Prototype_Component_Texture_UI_Dialogue*/
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Dialogue"),
		CTexture::Create(m_pDevice, m_pContext, &Path))))
		return E_FAIL;

	Path = { L"Effect", L"Filter" };
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_FILTER_TEXTURE,
		CTexture::Create(m_pDevice, m_pContext, &Path))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_ANIMATOR,
		CAnimator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//폰트
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Maru10"), TEXT("../../Resources/Data/Font/Maru10.spritefont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Maru18"), TEXT("../../Resources/Data/Font/Maru18.spritefont"))))
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

void CMainApp::SetUp_RendererDesc()
{
	if (nullptr == m_pRenderer)
		return;

	CRenderer::DEBUGRENDER* pDesc = m_pRenderer->Get_DebugRender();
	if (nullptr == pDesc)
		return;

	CBattle_Manager* pBattleInstance = GET_INSTANCE(CBattle_Manager);

	if (m_iCurLevel != g_iCurrentLevel)
	{
		if (LEVEL_SELECT == g_iCurrentLevel)
		{
			pDesc->fRimLightPower = 0.f;
			pDesc->fShadowDepth = 10.f;
			pDesc->bShadow = true;
			pDesc->bFog = false;
		}
		if (LEVEL_STORY == g_iCurrentLevel)
		{
			pDesc->fogColor = _float4(0.1f, 0.25f, 0.4f, 1.f);// _float4(0.15f, 0.25f, 0.2f, 1.f);
			pDesc->fogExp = 0.002f;
			pDesc->fogType = 2;
			pDesc->fRimLightPower = 0.f;
			pDesc->fShadowDepth = 20.f;
			pDesc->bShadow = true;

			pDesc->minSeparation = 0.f;
			pDesc->maxSeparation = 1.f;
			pDesc->minDistance = 0.f;
			pDesc->maxDistance = 100.f;

			m_pRenderer->Start_FadeInOut(0.01f, false, 0);
		}
		if (LEVEL_BATTLE == g_iCurrentLevel)
		{
			CBattle_Manager::BATTLEMAP_TYPE battleMap = pBattleInstance->Get_BattleInfo().eBattleMap;

			if (CBattle_Manager::MAP_UROKODAKI == battleMap)
			{
				pDesc->fogColor = _float4(0.25f, 0.35f, 0.5f, 1.f);
				pDesc->fogExp = 0.003f;
				pDesc->fogType = 2;
				pDesc->fShadowDepth = 1.f;
				pDesc->fRimLightPower = 3.f;
			}
			else if (CBattle_Manager::MAP_HUJIKASANE == battleMap)
			{
				pDesc->fogColor = _float4(0.05f, 0.15f, 0.4f, 1.f);
				pDesc->fogExp = 0.001f;
				pDesc->fogType = 2;
				pDesc->fShadowDepth = 10.f;
				pDesc->fRimLightPower = 0.f;
			}
			else if (CBattle_Manager::MAP_TAMAYO == battleMap)
			{
				pDesc->fogColor = _float4(0.1f, 0.05f, 0.4f, 1.f);
				pDesc->fogExp = 0.0015f;
				pDesc->fogType = 2;
				pDesc->fShadowDepth = 20.f;
				pDesc->fRimLightPower = 0.f;
			}
			pDesc->bFog = true;
			pDesc->minSeparation = 0.f;
			pDesc->maxSeparation = 0.4f;
			pDesc->minDistance = 0.f;
			pDesc->maxDistance = 10.f;
			pDesc->bShadow = true;
		}
		if (LEVEL_GAMEPLAY == g_iCurrentLevel)
		{
			pDesc->bFog = true;
			pDesc->fogColor = _float4(0.05f, 0.15f, 0.4f, 1.f);
			pDesc->fogExp = 0.001f;
			pDesc->fogType = 2;

			pDesc->bShadow = true;
			pDesc->fShadowDepth = 10.f;
			pDesc->fRimLightPower = 0.f;
			m_pRenderer->Start_FadeInOut(0.01f, true, 0);

			pDesc->minSeparation = 0.f;
			pDesc->maxSeparation = 0.4f;
			pDesc->minDistance = 0.f;
			pDesc->maxDistance = 10.f;
		}
		m_iCurLevel = g_iCurrentLevel;
	}

	if (pBattleInstance->Get_BattleInfo().bBattleStart)
	{
		pDesc->bShadow = true;
	}

	RELEASE_INSTANCE(CBattle_Manager);
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);

	Safe_Release_DV(m_pDevice);
	Safe_Release_DV(m_pContext);

	Safe_Release(m_pCutSceneManager);
	CCutScene_Manager::Destroy_Instance();

	Safe_Release(m_pCameraManager);
	CCamera_Manager::Destroy_Instance();

	Safe_Release(m_pBattleManager);
	CBattle_Manager::Destroy_Instance();

	Safe_Release(m_pImGuiManager);
	CImGuiClass_Manager::Destroy_Instance();

	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();

	for (auto& iter : m_vecEffectProtoTag)
		Safe_Delete_Array(iter);
}

