#pragma once

#pragma region COMP

#define PROTO_COMP_RENDERER TEXT("Prototype_Component_Renderer")
#define PROTO_COMP_VIBUFFER_RECT L"Prototype_Component_VIBuffer_Rect"
#define PROTO_COMP_VIBUFFER_TERRAIN L"Prototype_Component_VIBuffer_Terrain"
#define PROTO_COMP_VIBUFFER_CUBE L"Prototype_Component_VIBuffer_Cube"
#define PROTO_COMP_VIBUFFER_MODEL L"Prototype_Component_VIBuffer_Model"
#define PROTO_COMP_VIBUFFER_EFFECTINSTANCE L"Prototype_Component_VIBuffer_Effect_Instance"
#define PROTO_COMP_VIBUFFER_TRAIL L"Prototype_Component_VIBuffer_Trail"

// 네비게이션
#define PROTO_COMP_NAVIGATION_UROKODAKI L"Prototype_Component_Navigation01"
#define PROTO_COMP_NAVIGATION_TAMAYO L"Prototype_Component_Navigation06"
// 스토리 네비게이션
#define PROTO_COMP_NAVIGATION_STORY L"Prototype_Component_Navigation02"
// 몬스터 대전 네비게이션 
#define PROTO_COMP_NAVIGATION_STORY_ONI1 L"Prototype_Component_Navigation02_Oni1"
#define PROTO_COMP_NAVIGATION_STORY_ONI2 L"Prototype_Component_Navigation02_Oni2"
#define PROTO_COMP_NAVIGATION_STORY_ONI3 L"Prototype_Component_Navigation02_Oni3"
#define PROTO_COMP_NAVIGATION_STORY_ONI4 L"Prototype_Component_Navigation02_Oni4"
// 스토리 손가락오니 네비게이션
#define PROTO_COMP_NAVIGATION_STORY_TEONI L"Prototype_Component_Navigation02_TeOni"
// 라탄 네비게이션 
#define PROTO_COMP_NAVIGATION_RATTAN L"Prototype_Component_Navigation04"


#define PROTO_COMP_AABBCOLLIDER L"Prototype_Component_AABBCollider"
#define PROTO_COMP_OBBCOLLIDER L"Prototype_Component_OBBCollider"
#define PROTO_COMP_SPHERECOLLIDER L"Prototype_Component_SPHERECollider"

#define PROTO_COMP_SHADER_VTXTEX L"Prototype_Component_Shader_VTXTEX"
#define PROTO_COMP_SHADER_VTXTRAIL L"Prototype_Component_Shader_VTXTRAIL"
#define PROTO_COMP_SHADER_VTXNORTEX L"Prototype_Component_Shader_VTXNORTEX"
#define PROTO_COMP_SHADER_VTXCUBE L"Prototype_Component_Shader_VTTCUBETEX"
#define PROTO_COMP_SHADER_VTXMODEL L"Prototype_Component_Shader_VTXMODEL"
#define PROTO_COMP_SHADER_VTXANIMMODEL L"Prototype_Component_Shader_VTXANIMMODEL"
#define PROTO_COMP_SHADER_VTXPOINTINSTNACE L"Prototype_Component_Shader_VTXPOINTINSTANCE"
#define PROTO_COMP_SHADER_VTXRECTINSTNACE L"Prototype_Component_Shader_VTXRECTINSTANCE"
#define PROTO_COMP_SHADER_VTXANIMMODELINSTANCE L"Prototype_Component_Shader_VTXANIMMODELINSTANCE"
#define PROTO_COMP_SHADER_VTXMODELINSTANCE L"Prototype_Component_Shader_VTXMODELINSTANCE"
#define PROTO_COMP_SHADER_VTXTEX_EFFECT L"Prototype_Component_Shader_VTXTEX_EFFECT"
#define PROTO_COMP_SHADER_VTXMODEL_EFFECT L"Prototype_Component_Shader_VTXMODEL_EFFECT"
#define PROTO_COMP_SHADER_UI L"Prototype_Component_Shader_UI"

#define PROTO_COMP_TRANSFORM L"Prototype_Component_Transform"

// ground
#define PROTO_COMP_MODEL_UROKODAKIIWA_GROUND TEXT("N3_Ground01")
#define PROTO_COMP_MODEL_STORY_GROUND TEXT("storyN6")
#define PROTO_COMP_MODEL_RATTAN_GROUND TEXT("RattanGround")
#define PROTO_COMP_MODEL_TEONI_GROUND TEXT("TeOni_Ground2")
#define PROTO_COMP_MODEL_TAMAYO_GROUND TEXT("sinN1")
#define PROTO_COMP_MODEL_ASAKUSA_GROUND TEXT("asakusa1")

// instance model
#define PROTO_COMP_INSTANCEMODEL_TREE01 TEXT("SM_cmn_mount_Coniferous_01a")
#define PROTO_COMP_INSTANCEMODEL_TREE02 TEXT("SM_cmn_mount_Coniferous_02a")

#define PROTO_COMP_INSTANCEMODEL_BUSH01 TEXT("SM_cmn_mount_Bush_01a")
#define PROTO_COMP_INSTANCEMODEL_BUSH02 TEXT("SM_cmn_mount_Bush_02a")
#define PROTO_COMP_INSTANCEMODEL_GRASSSTRIP TEXT("SM_cmn_mount_GrassStrip_01a")
#define PROTO_COMP_INSTANCEMODEL_GRASS TEXT("SM_cmn_mount_Grass_01a")
#define PROTO_COMP_INSTANCEMODEL_SMALLTREE01 TEXT("SM_cmn_mount_SmallTree_01a")

#define PROTO_COMP_INSTANCEMODEL_CLIFF02 TEXT("SM_a_001_Cliff_02a")
#define PROTO_COMP_INSTANCEMODEL_BROKENTREE01A TEXT("SM_cmn_mount_BrokenTree_01a")
#define PROTO_COMP_INSTANCEMODEL_BROKENTREE01B TEXT("SM_cmn_mount_BrokenTree_01b")
#define PROTO_COMP_INSTANCEMODEL_BROKENTREE03A TEXT("SM_cmn_mount_BrokenTree_03a")
#define PROTO_COMP_INSTANCEMODEL_BUSH03A TEXT("SM_cmn_mount_Bush_03a")
#define PROTO_COMP_INSTANCEMODEL_BUSH03B TEXT("SM_cmn_mount_Bush_03b")
#define PROTO_COMP_INSTANCEMODEL_BUSH03C TEXT("SM_cmn_mount_Bush_03c")
#define PROTO_COMP_INSTANCEMODEL_BUSH04 TEXT("SM_cmn_mount_Bush_04a")
#define PROTO_COMP_INSTANCEMODEL_BUSH05 TEXT("SM_cmn_mount_Bush_05a")
#define PROTO_COMP_INSTANCEMODEL_BUSH06 TEXT("SM_cmn_mount_Bush_06a")
#define PROTO_COMP_INSTANCEMODEL_BUSH10 TEXT("SM_cmn_mount_Bush_10a")
#define PROTO_COMP_INSTANCEMODEL_BUSH12 TEXT("SM_cmn_mount_Bush_12a")
#define PROTO_COMP_INSTANCEMODEL_MOUNTROCK01 TEXT("SM_cmn_mount_Rock_01a")
#define PROTO_COMP_INSTANCEMODEL_MOUNTROCK02 TEXT("SM_cmn_mount_Rock_02a")

// new
#define PROTO_COMP_INSTANCEMODEL_BROKENTREE02B TEXT("SM_cmn_mount_BrokenTree_02b")
#define PROTO_COMP_INSTANCEMODEL_BROKENTREE05A TEXT("SM_cmn_mount_BrokenTree_05a")
#define PROTO_COMP_INSTANCEMODEL_CLIFF03 TEXT("SM_a_001_Cliff_03a")
#define PROTO_COMP_INSTANCEMODEL_CLIFF10 TEXT("SM_a_001_Cliff_10a")
#define PROTO_COMP_INSTANCEMODEL_CLIFFJUMP TEXT("SM_a_001_CliffJump_01a")
#define PROTO_COMP_INSTANCEMODEL_GRASS01B TEXT("SM_cmn_mount_Grass_01b")
#define PROTO_COMP_INSTANCEMODEL_GRASS02A TEXT("SM_cmn_mount_Grass_02a")
#define PROTO_COMP_INSTANCEMODEL_GRASS03A TEXT("SM_cmn_mount_Grass_03a")
#define PROTO_COMP_INSTANCEMODEL_DRYTREE TEXT("SM_cmn_mount_DryTree_01a")

//tamayo instance model
#define PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM01 TEXT("SM_d_106_CherryBlossom_01a")
#define PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM02 TEXT("SM_d_106_CherryBlossom_02a")
#define PROTO_COMP_INSTANCEMODEL_CHERRYBLOSSOM03 TEXT("SM_b_011_CherryBlossom_03a")
#define PROTO_COMP_INSTANCEMODEL_PLANT TEXT("SM_b_011_Plant_01a")
#define PROTO_COMP_INSTANCEMODEL_SMALLTREE03 TEXT("SM_cmn_mount_SmallTree_03a")
#define PROTO_COMP_INSTANCEMODEL_FAKEBG TEXT("SM_d_106_FakeBG_01a")
#define PROTO_COMP_INSTANCEMODEL_FAR01 TEXT("SM_d_106_Far_01a")
#define PROTO_COMP_INSTANCEMODEL_STREETLIGHT TEXT("SM_d_106_StreetLight_01a")
#define PROTO_COMP_INSTANCEMODEL_106BUSH01 TEXT("SM_d_106_mount_Bush_01a")
#define PROTO_COMP_INSTANCEMODEL_106BUSH02 TEXT("SM_d_106_mount_Bush_02a")

//urokodaki instance model 
#define PROTO_COMP_INSTANCEMODEL_BRANCH TEXT("SM_d_102_Branch_01a")
#define PROTO_COMP_INSTANCEMODEL_102BUSH01 TEXT("SM_d_102_Bush_01a")
#define PROTO_COMP_INSTANCEMODEL_102BUSH02 TEXT("SM_d_102_Bush_02a")
#define PROTO_COMP_INSTANCEMODEL_102BUSH03 TEXT("SM_d_102_Bush_03a")
#define PROTO_COMP_INSTANCEMODEL_102TREE01 TEXT("SM_d_102_Coniferous_01a")
#define PROTO_COMP_INSTANCEMODEL_102TREE02 TEXT("SM_d_102_Coniferous_02a")
#define PROTO_COMP_INSTANCEMODEL_102TREE03 TEXT("SM_d_102_Coniferous_03a")
#define PROTO_COMP_INSTANCEMODEL_102GRASS01 TEXT("SM_d_102_Grass_04a")
#define PROTO_COMP_INSTANCEMODEL_102GRASS02 TEXT("SM_d_102_Grass_05a")

#define PROTO_COMP_INSTANCEMODEL_PILLAR TEXT("SM_a_001_Pillar_01a")
#define PROTO_COMP_INSTANCEMODEL_WISTERIA01A TEXT("SM_a_001_Wisteria_01a")
#define PROTO_COMP_INSTANCEMODEL_WISTERIA01B TEXT("SM_a_001_Wisteria_01b")
#define PROTO_COMP_INSTANCEMODEL_WISTERIA02A TEXT("SM_a_001_Wisteria_02a")
#define PROTO_COMP_INSTANCEMODEL_WISTERIA02B TEXT("SM_a_001_Wisteria_02b")
#define PROTO_COMP_INSTANCEMODEL_LAVENDER TEXT("SM_cmn_mount_Lavender_01a")

#define PROTO_COMP_MODEL_BUILDING TEXT("SM_a_003_Building07_N")

#define PROTO_COMP_MODEL_BROKENROCK TEXT("SM_d_102_BrokenRock_01a")

// sky 
#define PROTO_COMP_MODEL_UROKODAKIIWA_SKY TEXT("SM_a_001_Urokodaki_Sky2")
#define PROTO_COMP_MODEL_HOKUSEIVS_SKY TEXT("SM_c_028_Sky_01a")
#define PROTO_COMP_MODEL_STORY_SKY TEXT("SM_a_005_natagumo_Sky")
#define PROTO_COMP_MODEL_RATTAN_SKY TEXT("SM_a_001_Urokodaki_Sky")

// map
#define PROTO_COMP_MODEL_MAPLIGHT L"Proto_Component_Model_MapLight"
#define PROTO_COMP_FILTER_TEXTURE L"Prototype_Component_Filter_Texture"

// Animator
#define	PROTO_COMP_ANIMATOR L"Prototype_Component_Animator"

// Oni
#define PROTO_COMP_MODEL_ONI_ZAKO L"Proto_Component_Model_Oni_Zako"
#define PROTO_COMP_MODEL_ONI_ZAKO2 L"Proto_Component_Model_Oni_Zako2"
#define PROTO_COMP_MODEL_ONI_ZAKO3 L"Proto_Component_Model_Oni_Zako3"

// Boss
#define PROTO_COMP_MODEL_BOSS_TEONI L"Proto_Component_Model_Boss_TeOni"
#define PROTO_COMP_MODEL_ITEM_TEONIROCK TEXT("Rock")
#define PROTO_COMP_MODEL_ITEM_TEONITE TEXT("OniTe")

//UI용 모델
#define PROTO_COMP_MODEL_UI_MENU	 L"Proto_Component_Model_UI_Menu"
#define PROTO_COMP_MODEL_UI_TANJIRO	 L"Proto_Component_Model_UI_Tangiro"
#define PROTO_COMP_MODEL_UI_ZENITSU	 L"Proto_Component_Model_UI_Zenitsu"
#define PROTO_COMP_MODEL_UI_SABITO	 L"Proto_Component_Model_UI_Sabito"
#define PROTO_COMP_MODEL_UI_SHINOBU	 L"Proto_Component_Model_UI_Shinobu"
#define PROTO_COMP_MODEL_UI_KYOUJURO L"Proto_Component_Model_UI_Kyoujuro"
#define PROTO_COMP_MODEL_UI_AKAZA L"Proto_Component_Model_UI_Akaza"

//선택창 용 무기
#define PROTO_COMP_MENU_TANJIRO_SWORD L"Proto_Comp_Menu_Tanjiro_Sword"
#define PROTO_COMP_MENU_ZENITSU_SWORD L"Proto_Comp_Menu_Zenitsu_Sword"
#define PROTO_COMP_MENU_SABITO_SWORD L"Proto_Comp_Menu_Sabito_Sword"
#define PROTO_COMP_MENU_SHINOBU_SWORD L"Proto_Comp_Menu_Shinobu_Sword"
#define PROTO_COMP_MENU_KYOUJURO_SWORD L"Proto_Comp_Menu_Kyoujuro_Sword"
#define PROTO_COMP_MENU_SABITO_SHEATH L"Proto_Comp_Menu_Sabito_Sheath"

//동물
#define PROTO_COMP_MODEL_DEER L"Proto_Component_Model_Deer"
#define PROTO_COMP_MODEL_BUTTERFLY L"Proto_Component_Model_Butterfly"
#define PROTO_COMP_MODEL_CAT L"Proto_Component_Model_Cat"
#define PROTO_COMP_MODEL_BIRD L"Proto_Component_Model_Bird"
#define PROTO_COMP_MODEL_DOG L"Proto_Component_Model_Dog"
#define PROTO_COMP_MODEL_MOUSE L"Proto_Component_Model_Mouse"
#define PROTO_COMP_MODEL_CHICKEN L"Proto_Component_Model_Chicken"

//NPC
#define PROTO_COMP_MODEL_CANDIDATE L"Proto_Component_Model_Candidate"
#define PROTO_COMP_MODEL_AOI L"Proto_Component_Model_Aoi"
#define PROTO_COMP_MODEL_NURSE1 L"Proto_Component_Model_Nurse1"
#define PROTO_COMP_MODEL_NURSE2 L"Proto_Component_Model_Nurse2"
#define PROTO_COMP_MODEL_NURSE3 L"Proto_Component_Model_Nurse3"
#define PROTO_COMP_MODEL_SISTER_L L"Proto_Component_Model_SISTERLEFT"
#define PROTO_COMP_MODEL_SISTER_R L"Proto_Component_Model_SISTERRIGHT"
#define PROTO_COMP_MODEL_NPC_ZENITSU L"Proto_Component_Model_NPC_Zenitsu"
#define PROTO_COMP_MODEL_NPC_KANAWO L"Proto_Component_Model_NPC_Kanawo"
#define PROTO_COMP_MODEL_NPC_GENYA L"Proto_Component_Model_NPC_Genya"
#define PROTO_COMP_MODEL_NPC_SIT_CANDIDATE L"Proto_Component_Model_NPC_Sit_Candidate"
#define PROTO_COMP_MODEL_NPC_DEAD_CANDIDATE L"Proto_Component_Model_NPC_Dead_Candidate"

#define PROTO_COMP_MODEL_PLAYER_SABITO	 L"Proto_Component_Model_Player_Sabito"
#define PROTO_COMP_MODEL_PLAYER_ZENITSU L"Proto_Component_Model_Player_Zenitsu"
#define PROTO_COMP_MODEL_PLAYER_AKAZA L"Proto_Component_Model_Player_Akaza"
#define PROTO_COMP_MODEL_STORY_TANJIRO L"Proto_Component_Model_Story_Tanjiro"
#define PROTO_COMP_MODEL_PLAYER_TANJIRO L"Proto_Component_Model_Player_Tanjiro"

// Item
#define PROTO_COMP_MODEL_ZENITSU_SWORD L"Proto_Comp_Model_ZeniTsu_Sword"
#define PROTO_COMP_MODEL_ZENITSU_SHEATH L"Proto_Comp_Model_ZenuTsu_Sheath"

#define PROTO_COMP_MODEL_TANJIRO_SWORD L"Proto_Comp_Model_Tanjiro_Sword"
#define PROTO_COMP_MODEL_TANJIRO_SHEATH L"Proto_Comp_Model_Tanjiro_Sheath"

#define PROTO_COMP_MODEL_SABITO_SWORD L"Proto_Comp_Model_Sabito_Sword"
#define PROTO_COMP_MODEL_SABITO_SHEATH L"Proto_Comp_Model_Sabito_Sheath"

#define PROTO_COMP_TEXTURE_NOISE L"Prototype_Component_Texture_Noise"
#define PROTO_COMP_TEXTURE_TRAIL L"Prototype_Component_Texture_Trail"

#define PROTO_COMP_MODEL_LANTERN_L L"Prototype_Component_Item_Lantern_Left"
#define PROTO_COMP_MODEL_LANTERN_R L"Prototype_Component_Item_Lantern_Right"

#pragma endregion COMP



#pragma region GOB

#define PROTO_GOB_EFFECT_TOOL L"Prototype_GameObject_Effect"

#define PROTO_GOB_CAMERA_TOOL L"Prototype_GameObject_Camera_Tool"

#define PROTO_GOB_UI_TOOL L"Prototype_GameObject_UI_Tool"

#define PROTO_GOB_ANIMVIEWERMODEL L"Prototype_GameObject_AnimViewerModel"

#define PROTO_GOB_STORY_TANJIRO L"Prototype_GameObject_Story_Tanjiro" 
#define PROTO_GOB_PLAYER_ZENITSU L"Prototype_GameObject_Player_Zenitsu" 
#define PROTO_GOB_PLAYER_AKAZA L"Prototype_GameObject_Player_Akaza" 
#define PROTO_GOB_PLAYER_TANJIRO L"Prototype_GameObject_Player_Tanjiro" 
#define PROTO_GOB_PLAYER_SABITO L"Prototype_GameObject_Player_Sabito"

#define PROTO_GOB_MAPOBJECT_TEST L"Proto_GameObject_MapObject_Test"

#define PROTO_GOB_CAMERA_CINEMATIC L"Proto_GameObject_CameraCinematic"
#define PROTO_GOB_CAMERA_FREE L"Proto_GameObject_CameraFree"
#define PROTO_GOB_LIGHT_CAMERA L"Proto_GameObject_LightCamera"
#define PROTO_GOB_CUTSCENE_CAMERA L"Proto_GameObject_CutSceneCamera"

// map object
#define PROTO_GOB_MAPOBJECT_OBJ L"Proto_GameObject_MapObject_Obj"
#define PROTO_GOB_MAPOBJECT_GROUND L"Proto_GameObject_MapObject_Ground"
#define PROTO_GOB_MAPOBJECT_SKY L"Proto_GameObject_MapObject_Sky"
#define PROTO_GOB_MAPOBJECT_LIGHT L"Proto_GameObject_MapLight"
#define PROTO_GOB_STORYMAP_COLLIDER L"Proto_GameObject_StoryMap_Collider"
#define PROTO_GOB_MAPOBJECT_INSTANCE L"Proto_GameObject_MapObject_Instance"
#define PROTO_GOB_MAPOBJECT_BROKENROCK L"Proto_GameObject_MapObject_BrokenRock"

// Oni
#define PROTO_GOB_ONI_ZAKO L"ZakoOni01" 
#define PROTO_GOB_ONI_ZAKO2 L"ZakoOni02" 
#define PROTO_GOB_ONI_ZAKO3 L"ZakoOni03" 

// Boss
#define PROTO_GOB_BOSS_TEONI L"TeOni"
#define PROTO_GOB_ITEM_TEONIROCK L"TeOni_Rock"
#define PROTO_GOB_ITEM_TEONITE L"TeOni_Te"

// Test Player 
#define PROTO_GOB_TESTPLAYER L"Proto_GameObject_TestPlayer"


#define PROTO_GOB_EFFECT L"Prototype_GameObject_Effect"
#define PROTO_GOB_EFFECT_IMAGE L"Prototype_GameObject_Effect_Image"
#define PROTO_GOB_EFFECT_STATICMESH L"Prototype_GameObject_Effect_StaticMesh"
#define PROTO_GOB_EFFECT_ANIMMESH L"Prototype_GameObject_Effect_AnimMesh"
#define PROTO_GOB_EFFECT_INSTANCE L"Prototype_GameObject_Effect_Instance"

#define PROTO_GOB_TRAILOBJ L"Prototype_GameObject_TrailObject" 
#define PROTO_GOB_SWORD L"Prototype_GameObject_Sword"
#define PROTO_GOB_SHEATH L"Prototype_GameObject_Sheath"
//PROJECTILE
#define PROTO_GOB_PROJECTILE L"Prototype_GameObject_Projectile"
#define PROTO_GOB_WATERBALL L"Prototype_GameObject_WaterBall"

//Animal
#define PROTO_GOB_ANIMAL L"Prototype_GameObject_Animal"

//NPC
#define PROTO_GOB_NPC L"Prototype_GameObject_NPC"

//UI
#define PROTO_GOB_UI L"Prototype_GameObject_UI"
#define PROTO_GOB_UI_HP L"Prototype_GameObject_UI_HP"
#define PROTO_GOB_UI_COMBO L"Prototype_GameObject_UI_Combo"
#define PROTO_GOB_UI_ULTIMATE L"Prototype_GameObject_UI_Ult"
#define PROTO_GOB_UI_MINIMAP L"Prototype_GameObject_UI_Mini"
#define PROTO_GOB_UI_MINITRACK L"Prototype_GameObject_Track"
#define PROTO_GOB_UI_SELECTMODEL L"Prototype_GameObject_Select_Model"
#define PROTO_GOB_UI_CHARSELECT L"Prototype_GameObject_Char"
#define PROTO_GOB_UI_DIALOGUE L"Prototype_GameObject_Dia"
#define PROTO_GOB_SELECT_ITEM L"Prototype_GameObject_Select_Item"
#define PROTO_GOB_NPC_ITEM L"Prototype_GameObject_NPC_Item"
#define PROTO_GOB_UI_ICON L"Prototype_GameObject_UI_Icon"
#define PROTO_GOB_UI_QUEST L"Prototype_GameObject_UI_Quest"
#define PROTO_GOB_UI_DIRMAP L"Prototype_GameObject_UI_Dir"
#define PROTO_GOB_UI_DIRICON L"Prototype_GameObject_UI_DirIcon"
#define PROTO_GOB_UI_MINIGAME L"Prototype_GameObject_UI_MG"
#define PROTO_GOB_UI_NOTE L"Prototype_GameObject_UI_Note"
#define PROTO_GOB_UI_MINIMGAME_MODEL L"Prototype_GameObject_UI_MiniGame_Model"
#define PROTO_GOB_UI_BOSS L"Prototype_GameObject_UI_Boss"
#define PROTO_GOB_UI_LINES L"Prototype_GameObject_UI_Lines"

#define PROTO_GOB_DEBUG_RENDEROBJ L"Prototype_GameObject_Debug_RenderObj"
#pragma endregion GOB

// 애니메이션 태그 

#pragma endregion GOB
#define ANIM_GROUPONI_IDLE L"SK_E0001_V00_C00.ao|(ZakoOni_BaseNut01_1) A_E0001_V00_C00_BaseNut01_1"
//#define ANIM_GROUPONI_CHASE L"SK_E0001_V00_C00.ao|(ZakoOni_BaseRun01_1) A_E0001_V00_C00_BaseRun01_1"
#define ANIM_GROUPONI_CHASE L"SK_E0001_V00_C00.ao|(ZakoOni_BaseWalkF01_1) A_E0001_V00_C00_BaseWalkF01_1"
#define ANIM_GROUPONI_COLLISIONCHASE L"SK_E0001_V00_C00.ao|(ZakoOni_BaseWalkF01_1) A_E0001_V00_C00_BaseWalkF01_1"
#define ANIM_GROUPONI_DEAD L"SK_E0001_V00_C00.ao|(ZakoOni_AtkSkl11_1) A_E0001_V00_C00_AtkSkl11_1"
#define ANIM_GROUPONI_BOUNCE L"SK_E0001_V00_C00.ao|(ZakoOni_BaseJump01_2) A_E0001_V00_C00_BaseJump01_2"
//#define ANIM_GROUPONI_PUSHED L"SK_E0001_V00_C00.ao|(ZakoOni_AtkSkl03_3) A_E0001_V00_C00_AtkSkl03_3"
//#define ANIM_GROUPONI_PUSHEDOUT L"SK_E0001_V00_C00.ao|(ZakoOni_AtkSkl02_Dmg) A_E0001_V00_C00_AtkSkl02_Dmg"
#define ANIM_GROUPONI_PUSHED L"SK_E0001_V00_C00.ao|(ZakoOni_BaseGuard01_1) A_E0001_V00_C00_BaseGuard01_1"
#define ANIM_GROUPONI_PUSHEDOUT L"SK_E0001_V00_C00.ao|(ZakoOni_AtkSkl07_1) A_E0001_V00_C00_AtkSkl07_1"
#define ANIM_GROUPONI_ROAR L"SK_E0001_V00_C00.ao|(ZakoOni_AtkBurst01) A_E0001_V00_C00_AtkBurst01"
#define ANIM_GROUPONI_TURN L"SK_E0001_V00_C00.ao|(ZakoOni_BaseWalkR01_1) A_E0001_V00_C00_BaseWalkR01_1"

#define ANIM_GROUPONIB_IDLE L"SK_E0001_V00_C10.ao|(ZakoOni_BaseNut01_1) A_E0001_V00_C00_BaseNut01_1"
#define ANIM_GROUPONIB_CHASE L"SK_E0001_V00_C10.ao|(ZakoOni_BaseWalkF01_1) A_E0001_V00_C00_BaseWalkF01_1"
#define ANIM_GROUPONIB_BOUNCE L"SK_E0001_V00_C10.ao|(ZakoOni_BaseJump01_2) A_E0001_V00_C00_BaseJump01_2"
#define ANIM_GROUPONIB_PUSHED L"SK_E0001_V00_C10.ao|(ZakoOni_AtkSkl03_3) A_E0001_V00_C00_AtkSkl03_3"
#define ANIM_GROUPONIB_PUSHEDOUT L"SK_E0001_V00_C10.ao|(ZakoOni_AtkSkl02_Dmg) A_E0001_V00_C00_AtkSkl02_Dmg"
#define ANIM_GROUPONIB_DEAD L"SK_E0001_V00_C10.ao|(ZakoOni_AtkSkl11_1) A_E0001_V00_C00_AtkSkl11_1"
#define ANIM_GROUPONIB_ROAR L"SK_E0001_V00_C10.ao|(ZakoOni_AtkBurst01) A_E0001_V00_C00_AtkBurst01"
#pragma endregion AnimationName


// 카메라 tag
#define CM_CAMERA_FREE	L"Camera_Manager_Camera_Free"
#define CM_CAMERA_LIGHT L"Camera_Manager_Camera_Light"
#define	CM_CAMERA_ONI1	L"Camera_Manager_Camera_Oni1"
#define	CM_CAMERA_ONI2	L"Camera_Manager_Camera_Oni2"
#define	CM_CAMERA_ONI3	L"Camera_Manager_Camera_Oni3"
