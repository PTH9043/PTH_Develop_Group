#pragma once 

#define DEBUG_MACRO_TO_METHOD \
__FILE__, __LINE__, __FUNCTION__ 

#define DEBUG_MSG(Debug_Msg) \
NOT_OPEN_METHOD::Debug_Message(DEBUG_MACRO_TO_METHOD, Debug_Msg);

#define COPY_NOT(CLASS) \
CLASS(const CLASS& _rhs) = delete; \
CLASS operator =(CLASS _class) = delete;


#define DECLARE_SINGLETON(CLASS) \
COPY_NOT(CLASS) \
public: \
static shared_ptr<CLASS> Get_Instance(); \
static void Destroy_Instance(); \
private: \
static shared_ptr<CLASS> m_pInstance;


#define IMPLEMENT_SINGLETON(CLASS) \
shared_ptr<CLASS> CLASS::m_pInstance = nullptr;  \
shared_ptr<CLASS> CLASS::Get_Instance(){ \
 if(nullptr == m_pInstance) \
    m_pInstance = CSmartPointerFactory<CLASS>::CreateShared();\
  return m_pInstance; \
} \
void CLASS::Destroy_Instance(){ \
 Safe_Free(m_pInstance);  \
 m_pInstance.reset(); \
 }


#define GET_INSTNACE(CLASS)  [&](){ shared_ptr<CLASS> pInstance = CLASS::Get_Instance();  return pInstance;}();

#define SAFE_DELETE(X) if(nullptr != X) {delete X; X = nullptr;}

// SITUATION을 만족시켰으면 리턴
#define RETURN_CHECK(SITUATION, RETURN) \
if(SITUATION){  return RETURN; }

// 가운데 Values를 조건을 SITUATION을 만족 시켰으면 실행
#define RETURN_CHECK_RUNVALUES(SITUATION, VALUES, RETURN) \
if(SITUATION){ VALUES return RETURN; }

#define RETURN_CHECK_FAILED_CHECKERROR(SITUATION, RETURN) \
HRESULT result = SITUATION; if(FAILED(result)){ NOT_OPEN_METHOD::Check_Error(result, DEBUG_MACRO_TO_METHOD); return RETURN; }

#define RETURN_CHECK_FAILED(SITUATION, RETURN) \
HRESULT result = SITUATION; if(FAILED(result)){ return RETURN; }

#define RETURN_CHECK_RUNVALUES_FAILED(SITUATION, VALUES,RETURN) \
if(FAILED(SITUATION)){ VALUES return RETURN; }


#define PROTO_COMP_RENDER L"Proto_Comp_Render"
#define PROTO_COMP_CUBEBUFFER L"Proto_Comp_CubeBuffer"
#define PROTO_COMP_FLOORBUFFER L"Proto_Comp_FloorBuffer"
#define PROTO_COMP_CRISTALBUFFER L"Proto_Comp_CristalBuffer"
#define PROTO_COMP_ABBCOLLIDER L"Proto_Comp_AABBCollider"
#define PROTO_COMP_OBBCOLLIDER L"Proto_Comp_OBBCollider"
#define PROTO_COMP_RECTTRANSFORM L"Proto_Comp_RectTransform"

#define PROTO_ACTOR_MAINCAMERA L"Proto_Actor_MainCamera"
#define PROTO_ACTOR_CUBE L"Proto_Actor_Cube"
#define PROTO_ACTOR_TANK L"Proto_Actor_Tank"
#define PROTO_ACTOR_CONTROLHEAD L"Proto_Actor_ControlHead"
#define PROTO_ACTOR_POSIN L"Proto_Actor_Posin"
#define PROTO_ACTOR_FLOOR L"Proto_Actor_Floor"
#define PROTO_ACTOR_EMPTY L"Proto_Actor_Empty"
#define PROTO_ACTOR_SHOOTER L"Proto_Actor_Shooter"
#define PROTO_ACTOR_NORMALBULLET L"Proto_Actor_NormalBullet"
#define PROTO_ACTOR_EXPLOSIONBULLET L"Proto_Actor_ExplosionBullet"
#define PROTO_ACTOR_TARGETBULLET L"Proto_Actor_TargetBullet"
#define PROTO_ACTOR_CUBEMONSTER L"Proto_Actor_CubeMonster"
#define PROTO_ACTOR_UI_TEXT L"Proto_Actor_UI_Text"
#define PROTO_ACTOR_UI_TEXTBUTTON L"Proto_Actor_UI_TextButton"
#define PROTO_ACTOR_CRISTAL L"Proto_Actor_Cristal"
#define PROTO_ACTOR_NORMALMONSTER L"Proto_Actor_NormalMonster"
#define PROTO_ACTOR_BOSSMONSTER L"Proto_Actor_BossMonster"
#define PROTO_ACTOR_BLOOD L"Proto_Actor_Blood"

#define LAYER_CAM L"Layer_Cam"
#define LAYER_OBJECT L"Layer_Object"
#define LAYER_FLOOR L"Layer_Floor"
#define LAYER_DEBUG L"Layer_Debug"
#define LAYER_EMPTYOBEJCT L"Layer_EmptyObject"
#define LAYER_BULLET L"Layer_Bullet"
#define LAYER_MONSTER L"Layer_Monster"
#define LAYER_UI L"Layer_UI"
#define LAYER_PLAYER L"Layer_Player"
#define LAYER_BLOOD L"Layer_Blood"

#define COM_TRANSFORM L"Com_Transform"
#define COM_RECTTRANSFORM L"Com_RectTransform"
#define COM_COLLIDER L"Com_Collider"
#define COM_RENDERER L"Com_Renderer"
#define COM_BUFFER L"Com_Buffer"
#define COM_FIRSTCHECKCOLL L"Com_FirstCheckColl"
#define COM_REGIONCOLLIDER L"Com_RegionCollider"


#define MAIN_CAMERA L"MainCamera"
#define FREE_CAMERA L"FreeCamera"
