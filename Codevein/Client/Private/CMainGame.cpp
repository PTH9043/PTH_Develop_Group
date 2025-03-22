#include<time.h>
#include "CMainGame.h"
#include "CGameInstance.h"
#include "CImGui_Manager.h"
#include "CData_Manager.h"
#include "CLevel_Loading.h"

#include "CBackground.h"
#include "CButton.h"
#include "CText.h"
#include "CTerrain.h"
#include "CCamera_Free.h"
#include "CCamera_Tool.h"
#include "CCamera_Minimap.h"
#include "CWorldObj.h"
#include "CDebugModel.h"
#include "CCubeObj.h"
#include "CSkyBox.h"

#include "CPlayer.h"
#include "CWeapon.h"

#include "CTwoHandSword.h"
#include "COneHandSword.h"
#include "CHammer.h"
#include "CHalberd.h"
#include "CGunSword.h"

#include "CGiantDevil.h"
#include "CMonkeyDevil.h"
#include "CWolfDevil.h"
#include "CUrchinDevil.h"
#include "CGiantWhiteDevil.h"
#include "CAncientHannibal.h"
#include "CGateKeeperWoman.h"

#include "CHpBar.h"
#include "CMpBar.h"

#include "CCheckPoints.h"
#include "CUI_LoadImg.h"
#include "CKnownKey_UI.h"
#include "CMoveToCheckPointsImg.h"
#include "CCheckPointInterface.h"

#include "CParticle.h"
#include "C2DEffectParticle.h"
#include "CEffectParticleGroup.h"
#include "CWOEffect.h"
#include "CMuffler.h"
#include "CCinematicCamera.h"
#include "CPotal.h"
#include "CEffect.h"
#include "CEffectGroup.h"

CMainGame::CMainGame() : 
	m_pGameInstance(CGameInstance::Get_Instance()),
	m_pImGuiManager(CImGui_Manager::Get_Instance()),
	m_pDataManager(CData_Manager::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pImGuiManager);
	Safe_AddRef(m_pDataManager);
	/*  	D3D11_SAMPLER_DESC */
}

HRESULT CMainGame::Native_Construct()
{
	srand((_uint)time(NULL));

	GRAPHICDESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWindowed = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinCX = g_iWinSizeX;
	GraphicDesc.iWinCY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END,  g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_FirstFolder(L"..\\..\\Resource")))
		return E_FAIL;
	CFrustCulling::FRUSTDESC tFrustDesc = CFrustCulling::FRUSTDESC(60.f, _float(g_iWinSizeX) / g_iWinSizeY, 0.2f, 300.f);
	if (FAILED(m_pGameInstance->NativeConstruct_FrustCull(m_pDevice, m_pContext, tFrustDesc)))
		return E_FAIL;

	if (FAILED(m_pImGuiManager->NativeConstruct(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(m_pDataManager->NativeConstruct(m_pDevice, m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Static_Component()))
		return E_FAIL;

	if (FAILED(Ready_Static_GameObj()))
		return E_FAIL;

	m_pGameInstance->Add_Thread(Run_Thread, this);
	m_pGameInstance->Add_Thread(Run_Thread, this);
	m_pGameInstance->Add_Thread(Run_Thread, this);
	m_pGameInstance->Add_Thread(Run_Thread, this);
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

#ifdef _DEBUG
	m_pDataManager->Get_VariableMgr()->Initialize_Debug();

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Bazzi"), TEXT("..\\..\\Resource\\Font\\Bazzi.spriteFont"))))
		return E_FAIL;
#endif

	return S_OK;
}

void CMainGame::Tick(const _double & _dDeltaSeconds)
{
	m_pGameInstance->Tick_Engine(_dDeltaSeconds);
	m_pGameInstance->LateTick_Engine(_dDeltaSeconds);
	/*  ImGui Tick */
	m_pImGuiManager->Tick(_dDeltaSeconds);
#ifdef _DEBUG
	m_dTimeAcc += _dDeltaSeconds;
#endif
}

HRESULT CMainGame::Render()
{
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	m_pImGuiManager->Render();
	m_pRendererComp->Draw();
#ifdef _DEBUG
	++m_iNumRender;

	if (m_dTimeAcc >= 1.0)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iNumRender);

		m_iNumRender = 0;

		m_dTimeAcc = 0.0;
	}

	// MakeSpriteFont "폰트이름" /FontSize:32 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 출력파일이름.spritefont
	m_pGameInstance->Render_Font(TEXT("Font_Bazzi"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
#endif // _DEBUG

	m_pGameInstance->Render_Engine();
	m_pGameInstance->Present();
	return S_OK;
}

void _stdcall CMainGame::Run_Thread(void* _pArg)
{
	CMainGame* pMainGame = static_cast<CMainGame*>(_pArg);
	if (nullptr != pMainGame)
		pMainGame->Running();
}

void CMainGame::Running()
{
	if (0 == m_iThreadLoader)
	{
		++m_iThreadLoader;
		if (FAILED(Ready_Static_Model_1()))
			return;

		m_pDataManager->Plus_Init_PlusThreadCount();
	}
	else if (1 == m_iThreadLoader)
	{
		++m_iThreadLoader;
		if (FAILED(Ready_Static_Texture_1()))
			return;

		m_pDataManager->Plus_Init_PlusThreadCount();
	}
	else if (2 == m_iThreadLoader)
	{
		++m_iThreadLoader;
		if (FAILED(Ready_Static_Texture_2()))
			return;

		m_pDataManager->Plus_Init_PlusThreadCount();
	}
	else if (3 == m_iThreadLoader)
	{
		++m_iThreadLoader;
		if (FAILED(Ready_Load_Partiacle_1()))
			return;

		m_pDataManager->Plus_Init_PlusThreadCount();
	}
}

HRESULT CMainGame::Open_Level(const LEVEL& _eLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, _eLevel))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainGame::Ready_Static_Component()
{
	m_pRendererComp = CRenderer::Create(m_pDevice, m_pContext);
	Safe_AddRef(m_pRendererComp);
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_RENDERER, m_pRendererComp)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_VIBUFFER_RECT,
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_VIBUFFER_CUBE,
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
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

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_TRANSFORM, CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#endif

	if(FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_2DEFFECTPARTICLE, 
	CShader::Create(m_pDevice, m_pContext, TEXT("..\\Bin\\Shader\\Shader_2DEffectParticle.hlsl"), 
		VTXRECTINSTANCE_DECLARATION::Element, VTXRECTINSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_PARTICLE,
		CShader::Create(m_pDevice, m_pContext, TEXT("..\\Bin\\Shader\\Shader_Particle.hlsl"),
			VTXCUBENSTANCE_DECLARATION::Element, VTXCUBENSTANCE_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("..\\Bin\\Shader\\Shader_VtTex.hlsl"),
			VTXTEX_DECLARATION::Element, VTXTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXNORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("..\\Bin\\Shader\\Shader_VtxNorTex.hlsl"),
			VTXNORTEX_DECLARATION::Element, VTXNORTEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXCUBE,
		CShader::Create(m_pDevice, m_pContext, L"..\\Bin\\Shader\\Shader_VtxCubeTex.hlsl",
		VTXCUBETEX_DECLARATION::Element, VTXCUBETEX_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL,
		CShader::Create(m_pDevice, m_pContext, L"..\\Bin\\Shader\\Shader_VtxModel.hlsl",
			VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXANIMMODEL,
		CShader::Create(m_pDevice, m_pContext, L"..\\Bin\\Shader\\Shader_VtxAnimModel.hlsl",
			VTXANIMMODEL_DECLARATION::Element, VTXANIMMODEL_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_SHADER_VTXMODEL_EQUIP,
		CShader::Create(m_pDevice, m_pContext , L"..\\Bin\\Shader\\Shader_VtxModel_Equip.hlsl",
		VTXMODEL_DECLARATION::Element, VTXMODEL_DECLARATION::iNumElement))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_TEXTURE_LOADING,
		CTexture::Create(m_pDevice, m_pContext, &PATHS{ L"Game", L"Loading" }))))
		return E_FAIL;



	return S_OK;
}

HRESULT CMainGame::Ready_Static_GameObj()
{
	// Non Client GameObj
	{
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_BACKGROUND, CBackground::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_TEXT, CText::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_BUTTON, CButton::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CAMERA_FREE, CCamera_Free::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CAMERA_TOOL, CCamera_Tool::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_SKYBOX, CSkyBox::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_DEBUGMODEL, CDebugModel::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_PLAYER, CPlayer::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_TWOHANDWORD, CTwoHandSword::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ONEHANDSWORD, COneHandSword::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_HAMMER, CHammer::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if(FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_HALBERD, CHalberd::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_GUNSWROD, CGunSword::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_HPBAR, CHpBar::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_MPBAR, CMpBar::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if(FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CAMERA_MINIMAP, CCamera_Minimap::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_LOADINGIMG, 
			CUI_LoadImg::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_KNOWNKEY_UI,
			CKnownKey_UI::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_MOVETOCHECKPOINTIMG,
			CMoveToCheckPointsImg::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if(FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_UI_CHECKPOINTINTERFACE, 
			CCheckPointInterface::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MUFFLER,
			CMuffler::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CINEMATICCAM,
			CCinematicCamera::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_POTAL,
			CPotal::Create(m_pDevice, m_pContext))))
			return E_FAIL;
	}

	// Client GameObj
	{
		CGameObject* pGameObject = CTerrain::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_TERRAIN, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_TERRAIN, pGameObject)))
			return E_FAIL;

		pGameObject = CWorldObj::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_WORLDOBJ, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_WORLDOBJ, pGameObject)))
			return E_FAIL;

		pGameObject = CCheckPoints::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_CHECKPOINT, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_CHECKPOINT, pGameObject)))
			return E_FAIL;

		pGameObject = CMonkeyDevil::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_MONKEYDEVIL, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_MONKEYDEVIL, pGameObject)))
			return E_FAIL;

		pGameObject = CWolfDevil::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_WOLFDEVIL, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_WOLFDEVIL, pGameObject)))
			return E_FAIL;

		pGameObject = CUrchinDevil::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_URCHINDEVI, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_URCHINDEVI, pGameObject)))
			return E_FAIL;

		pGameObject = CGiantWhiteDevil::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_GIANTWHITEDEVIL, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_GIANTWHITEDEVIL, pGameObject)))
			return E_FAIL;

		pGameObject = CAncientHannibal::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_ANCIENTHANNIBAL, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_ANCIENTHANNIBAL, pGameObject)))
			return E_FAIL;

		pGameObject = CGateKeeperWoman::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_GATEKEEPERWOMAN, (CGameObject_Client*)pGameObject);
		if(FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_GATEKEEPERWOMAN, pGameObject)))
			return E_FAIL;

		pGameObject = CWOEffect::Create(m_pDevice, m_pContext);
		m_pImGuiManager->Add_Proto(PROTO_GOB_WOEFFECT, (CGameObject_Client*)pGameObject);
		if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_WOEFFECT, pGameObject)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainGame::Ready_Static_Model_1()
{
	BEGIN_SINGLETON(CGameInstance, pGameInstance)
	{
		_matrix TransformationMatrix = XMMatrixScaling(0.006f, 0.006f, 0.006f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
		{
			{
				PATHS Path{ L"Game", L"Character", L"Player", L"Io" };
				FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
				FILEGROUP* pFbxGroup = pFileGroup->Find_Group(L"FBX_Converter");
				CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, pFileGroup, &pFbxGroup->vecFileDataList.front(), TransformationMatrix);
				PATHS Anim{ L"Game", L"Character", L"Player", L"Io",  L"Animations_Converter" };
				FILEGROUP* pAnim = pGameInstance->Find_Folder(&Anim);
				pModel->Load_Animation(pAnim);

				pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_MODEL_PLAYER, (CComponent*)pModel);
			}
			{
				PATHS Path{ L"Game", L"Character", L"Player", L"Io" };
				FILEGROUP* pFileGroup = pGameInstance->Find_Folder(&Path);
				FILEGROUP* pFbxGroup = pFileGroup->Find_Group(L"FBX_Converter");
				CModel* pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,
					pFileGroup, &pFbxGroup->vecFileDataList[1], XMMatrixScaling(1.0f, 1.0f, 1.0f));

				pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_MODEL_PLAYERMUFFLER, (CComponent*)pModel);
			}
		}
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CMainGame::Ready_Static_Texture_1()
{
	CoInitializeEx(nullptr, 0);
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_TEXTURE_UIGROUP,
			CTexture::Create(m_pDevice, m_pContext, &PATHS{ L"Game", L"UserInterface"}))))
			return E_FAIL;
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CMainGame::Ready_Static_Texture_2()
{
	CoInitializeEx(nullptr, 0);
	BEGIN_SINGLETON(CGameInstance, pGameInstance);
	{
		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_TEXTURE_SKYBOXGROUP,
			CTexture::Create(m_pDevice, m_pContext, &PATHS{ L"Game", L"Environment", L"SkyBox"}))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_ProtoType(LEVEL_STATIC, PROTO_COMP_TEXTURE_PARTICLEGROUP,
			CTexture::Create(m_pDevice, m_pContext, &PATHS{ L"Game", L"Effect" }))))
			return E_FAIL;
	}
	END_SINGLETON(pGameInstance);
	return S_OK;
}

HRESULT CMainGame::Ready_Load_Partiacle_1()
{
	CoInitializeEx(nullptr, 0);

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_DRAGONHAND_PARTICLE,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L" ", L"..\\..\\Resource\\Json\\Particle\\Dragon_Hand_Particle.bin")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_PLAYER_MOVEEFFECT,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Player_Smoke.bin", L"..\\..\\Resource\\Json\\Particle\\Smoke_Particle.bin")))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_PLAYER_SWORLDPARTICLE,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Player_Fire_Effect.bin", L" ")))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_WOG_BLOODHEART_1,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L" ", L"..\\..\\Resource\\Json\\Particle\\Jewly_Map.bin"
		)))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_WOG_BLOODHEART_2,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L" ", L"..\\..\\Resource\\Json\\Particle\\Jewly_Map_2.bin"
		)))))
		return S_OK;


	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_BLOOD_1,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Player_Attack_Effect_1.bin", L" "
		)))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_BLOOD_2,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Player_Attack_Effect_2.bin", L" "
		)))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_BLOOD_3,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Player_Attack_Effect_3.bin", L" "
		)))))
		return S_OK;

	if(FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_JUMP_DUST_GROUP, 
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Dragon_Boss_DirtEffect.bin", L" "
		)))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_JUMP_DUST_LITTLE,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Little_Effect.bin", L" "
		)))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_DEAD,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L" ", L"..\\..\\Resource\\Json\\2DEffect\\Disorve_Effect.bin"
		)))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_FIREWALL,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\FireWall_2DParticle.bin", 
			L"..\\..\\Resource\\Json\\Particle\\FireWall.bin")))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_DRAGON_MAGIC,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Magic_Dragon.bin", L" ")))))
		return S_OK;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_POTAL,
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L" ", L"..\\..\\Resource\\Json\\Particle\\Potal_Particle.bin"
		)))))
		return S_OK;

	if(FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_ICEBOSSATTACK,
		CEffectGroup::Create_LoadFile(m_pDevice, m_pContext,
			L"..\\..\\Resource\\Json\\Effect\\Boss_Attack_2.bin"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_BOSSROAD,
		CEffectGroup::Create_LoadFile(m_pDevice, m_pContext,
			L"..\\..\\Resource\\Json\\Effect\\Boss_Road_Effect.bin"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_BOSSUP,
		CEffectGroup::Create_LoadFile(m_pDevice, m_pContext,
			L"..\\..\\Resource\\Json\\Effect\\Boss_Up_Effect.bin"))))
		return E_FAIL;
	
	if(FAILED(m_pGameInstance->Add_ProtoType(PROTO_GOB_EFFPARTICLE_GTKWOOM_MAGIC, 
		CEffectParticleGroup::Create(m_pDevice, m_pContext, &CEffectParticleGroup::EFFECTPARTICLEGROUP(
			L"..\\..\\Resource\\Json\\2DEffect\\Magic_GateKeeperWoman.bin", L" "
		)))))
		return S_OK;

	return S_OK;
}

CMainGame * CMainGame::Create()
{
	CMainGame* pMainGame = new CMainGame();
	if (FAILED(pMainGame->Native_Construct()))
	{
		Safe_Release(pMainGame);
		MSG_BOX("CMainGame::Create To Failed");
	}
	return pMainGame;
}

void CMainGame::Free()
{
 	Safe_Release(m_pRendererComp);
	/*  Device, Context */
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	/* ImGui  */
	CImGui_Manager::Destroy_Instance();
	Safe_Release(m_pImGuiManager);
	// Delete WorldManager
	m_pDataManager->Delete_PlayerManager();
	/* CGameInstance */
	m_pGameInstance->Release_Engine();
	Safe_Release(m_pGameInstance);
	/* Data Manager*/
	CData_Manager::Destroy_Instance();
	Safe_Release(m_pDataManager);
} 
