#include "CLogoScene.h"
#include "CGameInstance.h"

#include "CText.h"
#include "CTextButton.h"
#include "CRectTransform.h"
#include "CGameScene.h"

void CLogoScene::Free()
{
}

HRESULT CLogoScene::NativeConstruct(){
	return S_OK;
}

HRESULT CLogoScene::LoadSceneData()
{
	shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
	// Title
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(_float(WINDOW_HALF_WIDTH) - 180.f, (_float)(WINDOW_HALF_HEIGHT) - 160.f);
		tRectDesc.v2Size = _float2(80.f, 40.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 0.f, 0.f, 0.f };

		m_pTitleText = static_pointer_cast<CText>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXT,
			LAYER_UI, VOIDDATAS{&tRectDesc , &tTextDesc }));
		m_pTitleText->SetText(L"탱크 게임!");
	}
	// Start Button 
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(_float(WINDOW_HALF_WIDTH) - 90.f, (_float)(WINDOW_HALF_HEIGHT) + 20.f);
		tRectDesc.v2Size = _float2(40.f, 20.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 100.f, 0.f, 100.f };

		CText::TEXTDESC tSelectDesc;
		tSelectDesc.tInfo = RGBINFO{ 100.f, 255.f, 100.f };

		m_pStartButton = static_pointer_cast<CTextButton>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXTBUTTON,
			LAYER_UI, VOIDDATAS{ &tRectDesc , &tTextDesc, &tSelectDesc }));
		m_pStartButton->SetText(L"게임 시작");
	}
	// End Button
	{
		CUserInterface::UIDESC tRectDesc;
		tRectDesc.v2Pos = _float2(_float(WINDOW_HALF_WIDTH) - 90.f, (_float)(WINDOW_HALF_HEIGHT)+100.f);
		tRectDesc.v2Size = _float2(40.f, 20.f);

		CText::TEXTDESC tTextDesc;
		tTextDesc.tInfo = RGBINFO{ 255.f, 0.f, 255.f };

		CText::TEXTDESC tSelectDesc;
		tSelectDesc.tInfo = RGBINFO{ 100.f, 255.f, 100.f };

		m_pEndButton = static_pointer_cast<CTextButton>(pGameInstance->Clone_Actor_Add(PROTO_ACTOR_UI_TEXTBUTTON,
			LAYER_UI, VOIDDATAS{ &tRectDesc , &tTextDesc, &tSelectDesc }));
		m_pEndButton->SetText(L"게임 종료");
	}

	return S_OK;
}

void CLogoScene::Tick(const _double& _dTimeDelta)
{
	if (true == m_pStartButton->IsClickedTextButton()) {
		shared_ptr<CGameInstance> pGameInstance = GET_INSTNACE(CGameInstance);
		pGameInstance->RegisterScene(CreateNew<CGameScene>());
	}
	else if (true == m_pEndButton->IsClickedTextButton()) {
		PostQuitMessage(0);
	}
}

void CLogoScene::Late_Tick(const _double& _dTimeDelta)
{
}
