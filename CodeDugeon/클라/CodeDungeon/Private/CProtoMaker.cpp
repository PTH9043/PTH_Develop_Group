#include "ClientDefines.h"
#include "CProtoMaker.h"
#include "UGameInstance.h"
#include "CMainCamera.h"
#include "CRooms.h"
#include "UAnimModel.h"
#include "CUserWarriorAnimController.h"
#include "CNetworkWarriorPlayerController.h"
#include "CMummyAnimController.h"
#include "CSarcophagusAnimController.h"
#include "CItemChestAnimController.h"
#include "UAnimModel.h"
#include "CWarriorPlayer.h"
#include "CMummy.h"
#include "CTorch.h"
#include "UTexGroup.h"
#include "CSword.h"
#include "CSarcophagus.h"
#include "CIronBars.h"
#include "CItemChest.h"
#include "CMinotaur.h"
#include "CMinotaurAnimController.h"
#include "CImageUI.h"
#include "CButtonUI.h"
#include "CLoadingUI.h"
#include "CHpBarUI.h"
#include "CHarlequinn.h"
#include "CHarlequinnAnimController.h"
#include "CShuriken.h"
#include "CShurikenThrowing.h"
#include "CAnubis.h"
#include "CAnubisAnimController.h"
#include "CAnubisStaff.h"
#include "CMimic.h"
#include "CMimicAnimController.h"
#include "CStatue.h"
#include "CCoreAnubis.h"
#include "CCoreHarlequinn.h"
#include "CCoreMinotaur.h"
#include "CNetworkAnubisAnimController.h"
#include "CNetworkHarlequinnAnimController.h"
#include "CNetworkItemChestAnimController.h"
#include "CNetworkMimicAnimController.h"
#include "CNetworkMinotaurAnimController.h"
#include "CNetworkMummyAnimController.h"
#include "CFreeModeCamera.h"

HRESULT CProtoMaker::CreateProtoData(CSHPTRREF<UGameInstance> _spGameInstance, CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UCommand> _spCommand)
{
	_spGameInstance->AddPrototype(PROTO_ACTOR_MAINCAMERA, CreateConstructorToNative<CMainCamera>(_spDevice, LAYER_CAM, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_ROOM, CreateConstructorToNative<CRooms>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_TORCH, CreateConstructorToNative<CTorch>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_IRONBARS, CreateConstructorToNative<CIronBars>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_WARRIORPLAYER, CreateConstructorToNative<CWarriorPlayer>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_MUMMY, CreateConstructorToNative<CMummy>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_SARCOPHAGUSLYING, CreateConstructorToNative<CSarcophagus>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_SARCOPHAGUSSTANDING, CreateConstructorToNative<CSarcophagus>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_LONGSWORD, CreateConstructorToNative< CSword>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_CHEST, CreateConstructorToNative< CItemChest>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_MINOTAUR, CreateConstructorToNative< CMinotaur>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_IMAGEUI, CreateConstructorToNative<CImageUI>(_spDevice, LAYER_UI, CLONETYPE::CLONE_ONCE));
	_spGameInstance->AddPrototype(PROTO_ACTOR_BUTTONUI, CreateConstructorToNative<CButtonUI>(_spDevice, LAYER_UI, CLONETYPE::CLONE_ONCE));
	_spGameInstance->AddPrototype(PROTO_ACTOR_LOADINGUI, CreateConstructorToNative<CLoadingUI>(_spDevice, LAYER_UI, CLONETYPE::CLONE_ONCE));
	_spGameInstance->AddPrototype(PROTO_ACTOR_HPBARUI, CreateConstructorToNative<CHpBarUI>(_spDevice, LAYER_UI, CLONETYPE::CLONE_ONCE));
	_spGameInstance->AddPrototype(PROTO_ACTOR_HARLEQUINN, CreateConstructorToNative< CHarlequinn>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_SHURIKEN, CreateConstructorToNative< CShuriken>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_SHURIKENTHROWING, CreateConstructorToNative< CShurikenThrowing>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_ANUBIS, CreateConstructorToNative< CAnubis>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_ANUBISSTAFF, CreateConstructorToNative< CAnubisStaff>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_MIMIC, CreateConstructorToNative< CMimic>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_STATUE, CreateConstructorToNative<CStatue>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_ANUBISCORE, CreateConstructorToNative<CCoreAnubis>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_HARLEQUINNCORE, CreateConstructorToNative<CCoreHarlequinn>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_MINOTAURCORE, CreateConstructorToNative<CCoreMinotaur>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_ACTOR_FREEMODECAMERA,CreateConstructorToNative<CFreeModeCamera>(_spDevice, LAYER_DEFAULT, CLONETYPE::CLONE_STATIC));
	_spGameInstance->AddPrototype(PROTO_RES_PARTICLETEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Particle", true));

	_spGameInstance->AddPrototype(PROTO_RES_FIRECOLORTEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Fire\\Color", true));

	_spGameInstance->AddPrototype(PROTO_RES_TRAILTEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Trail", true));
	
	_spGameInstance->AddPrototype(PROTO_RES_GUARDTEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Guard", true));
	
	_spGameInstance->AddPrototype(PROTO_RES_MATTEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Mat", true));
	
	_spGameInstance->AddPrototype(PROTO_RES_BLOODTEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Blood", true));

	_spGameInstance->AddPrototype(PROTO_RES_FIRENOISETEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Fire\\Noise", true));

	_spGameInstance->AddPrototype(PROTO_RES_FIREALPHATEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Fire\\Alpha", true));

	_spGameInstance->AddPrototype(PROTO_RES_UITEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\UI\\Demo", true));

	_spGameInstance->AddPrototype(PROTO_RES_DISSOLVETEXTUREGROUP, CLONETYPE::CLONE_STATIC,
		CreateConstructorNative<UTexGroup>(_spDevice, L"..\\..\\Resource\\Dissolve", true));

	_spGameInstance->AddPrototype(PROTO_COMP_USERWARRIORANIMCONTROLLER, CreateConstructorToNative<CUserWarriorAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_NETWORKWARRIORANIMCONTROLLER, CreateConstructorNative<CNetworkWarriorPlayerController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_MUMMYANIMCONTROLLER, CreateConstructorToNative<CMummyAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_SARCOPHAGUSANIMCONTROLLER, CreateConstructorToNative<CSarcophagusAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_CHESTANIMCONTROLLER, CreateConstructorToNative<CItemChestAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_MINOTAURANIMCONTROLLER, CreateConstructorToNative<CMinotaurAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_HARLEQUINNANIMCONTROLLER, CreateConstructorToNative<CHarlequinnAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_ANUBISANIMCONTROLLER, CreateConstructorToNative<CAnubisAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_MIMICANIMCONTROLLER, CreateConstructorToNative<CMimicAnimController>(_spDevice));


	_spGameInstance->AddPrototype(PROTO_COMP_NETWORKMUMMYANIMCONTROLLER, CreateConstructorToNative<CNetworkMummyAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_NETWORKCHESTANIMCONTROLLER, CreateConstructorToNative<CNetworkItemChestAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_NETWORKMINOTAURANIMCONTROLLER, CreateConstructorToNative<CNetworkMinotaurAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_NETWORKHARLEQUINNANIMCONTROLLER, CreateConstructorToNative<CNetworkHarlequinnAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_NETWORKANUBISANIMCONTROLLER, CreateConstructorToNative<CNetworkAnubisAnimController>(_spDevice));
	_spGameInstance->AddPrototype(PROTO_COMP_NETWORKMIMICANIMCONTROLLER, CreateConstructorToNative<CNetworkMimicAnimController>(_spDevice));


	_spGameInstance->CreateAudioSystemAndRegister(SOUNDTYPE::SOUND_BACKGROUND, L"..\\..\\Resource\\Sound\\BackgroundSound");
	_spGameInstance->CreateAudioSystemAndRegister(SOUNDTYPE::SOUND_GAME, L"..\\..\\Resource\\Sound\\InGameSound");
	_spGameInstance->CreateAudioSystemAndRegister(SOUNDTYPE::SOUND_MUMMY, L"..\\..\\Resource\\Sound\\Mummy");
	_spGameInstance->CreateAudioSystemAndRegister(SOUNDTYPE::SOUND_PLAYER, L"..\\..\\Resource\\Sound\\Player");
	_spGameInstance->CreateAudioSystemAndRegister(SOUNDTYPE::SOUND_HARLEQUINN, L"..\\..\\Resource\\Sound\\Harle");
	_spGameInstance->CreateAudioSystemAndRegister(SOUNDTYPE::SOUND_ANUBIS, L"..\\..\\Resource\\Sound\\Anubis");
	_spGameInstance->CreateAudioSystemAndRegister(SOUNDTYPE::SOUND_MINOTAUR, L"..\\..\\Resource\\Sound\\Miono");
	_spGameInstance->CreateAudioSystemAndRegister(SOUNDTYPE::SOUND_MIMIC, L"..\\..\\Resource\\Sound\\mimic");

	_spGameInstance->FontCreate(FONT_NANUMSQUARE_ACBOLD, L"..\\..\\Resource\\Font\\NanumSquare.spritefont");
	return S_OK;
}

HRESULT CProtoMaker::CreateMainSceneProtoData(CSHPTRREF<UGameInstance> _spGameInstance, CSHPTRREF<UDevice> _spDevice, CSHPTRREF<UCommand> _spCommand)
{
	_float4x4 Matrix = _float4x4::CreateScale(1.0f) /** _float4x4::CreateRotationY(DirectX::XMConvertToRadians(180.f))*/;

	Matrix = _float4x4::CreateScale(10.f);
	_spGameInstance->AddPrototype(PROTO_RES_FEMAILPLAYERANIMMODEL,CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\Player\\Convert\\F_Human_FBX.bin", Matrix));

	Matrix = _float4x4::CreateScale(0.1f) * _float4x4::CreateRotationY(DirectX::XMConvertToRadians(180));
	_spGameInstance->AddPrototype(PROTO_RES_MUMMYANIMMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\Mummy\\Convert\\Mummy_DEMO_1_FBX.bin", Matrix));

	Matrix = _float4x4::CreateScale(0.1f);
	_spGameInstance->AddPrototype(PROTO_RES_MINOTAURANIMMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\MinoTaur\\Convert\\minotaur_FBX.bin", Matrix));

	Matrix = _float4x4::CreateScale(0.1f);
	_spGameInstance->AddPrototype(PROTO_RES_HARLEQUINNANIMMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\Harlequin1\\Convert\\Harlequin1_FBX.bin", Matrix));

	Matrix = _float4x4::CreateScale(0.1f);
	_spGameInstance->AddPrototype(PROTO_RES_ANUBISANIMMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\Anubis\\Convert\\Anubis_FBX.bin", Matrix));

	Matrix = _float4x4::CreateScale(0.1f) * _float4x4::CreateRotationY(DirectX::XMConvertToRadians(180));
	_spGameInstance->AddPrototype(PROTO_RES_SARCOPHAGUSLYINGANIMMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\Sarcophagus\\Convert\\SarcophagusLaying_FBX.bin", Matrix));

	Matrix = _float4x4::CreateScale(0.1f) * _float4x4::CreateRotationY(DirectX::XMConvertToRadians(180));
	_spGameInstance->AddPrototype(PROTO_RES_SARCOPHAGUSSTANDINGANIMMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\Sarcophagus\\Convert\\SarcophagusStanding_FBX.bin", Matrix));

	Matrix = _float4x4::CreateScale(0.1f);
	_spGameInstance->AddPrototype(PROTO_RES_SHURIKENMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UModel>(
		_spDevice, L"..\\..\\Resource\\Model\\Item\\Equip\\Shuriken\\Convert\\Shuriken_FBX.bin"));

	Matrix = _float4x4::CreateScale(0.1f);
	_spGameInstance->AddPrototype(PROTO_RES_LONGSWORDMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UModel>(
		_spDevice, L"..\\..\\Resource\\Model\\Item\\Equip\\Sword\\Convert\\sword_FBX.bin"));

	Matrix = _float4x4::CreateScale(0.1f);
	_spGameInstance->AddPrototype(PROTO_RES_ANUBISSTAFFMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UModel>(
		_spDevice, L"..\\..\\Resource\\Model\\Item\\Equip\\AnubisHook\\Convert\\Anubis_Staff_FBX.bin"));


	Matrix = _float4x4::CreateScale(0.1f);
	_spGameInstance->AddPrototype(PROTO_RES_MIMICANIMMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\Mimic\\Convert\\Mimic_FBX.bin", Matrix));


	Matrix = _float4x4::CreateScale(1.0f);
	_spGameInstance->AddPrototype(PROTO_RES_CHESTANIMMODEL, CLONETYPE::CLONE_STATIC, CreateConstructorNative<UAnimModel>(
		_spDevice, L"..\\..\\Resource\\AnimModel\\Chest\\Convert\\Chest 1_FBX.bin", Matrix));



	return S_OK;
}
