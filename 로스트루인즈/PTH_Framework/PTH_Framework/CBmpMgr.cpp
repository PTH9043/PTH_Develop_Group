#include "pch.h"
#include "CBmpMgr.h"

CBmpMgr::CBmpMgr()
{
	
}

CBmpMgr::CBmpMgr(CGameMgr*& pGameMgr) : CManager(pGameMgr)
{

}

CBmpMgr::~CBmpMgr()
{
	Release();
}

void CBmpMgr::Initialize()
{
	// Load 
	// ======================= Intro =======================
	// ======================= Map1 =======================
	Insert_Bmp(L"Resources\\Map\\Map1\\Map1.bmp", L"Map1");
	Insert_Bmp(L"Resources\\Map\\Map1\\Floor1.bmp", L"MAP1_FLOOR1");
	Insert_Bmp(L"Resources\\Map\\Map1\\Floor2.bmp", L"MAP1_FLOOR2");
	Insert_Bmp(L"Resources\\Map\\Map1\\Wall1.bmp", L"MAP1_WALL1");
	// ======================= Map2 =======================
	Insert_Bmp(L"Resources\\Map\\Map2\\Map2.bmp", L"Map2");
	Insert_Bmp(L"Resources\\Map\\Map2\\Floor1.bmp", L"MAP2_FLOOR1");
	Insert_Bmp(L"Resources\\Map\\Map2\\Wall1.bmp", L"MAP2_WALL1");
	// ======================= Map3 =======================
	Insert_Bmp(L"Resources\\Map\\Map3\\Map3.bmp", L"Map3");
	Insert_Bmp(L"Resources\\Map\\Map3\\Floor1.bmp", L"MAP3_FLOOR1");
	Insert_Bmp(L"Resources\\Map\\Map3\\Floor2.bmp", L"MAP3_FLOOR2");
	Insert_Bmp(L"Resources\\Map\\Map3\\Floor3.bmp", L"MAP3_FLOOR3");
	Insert_Bmp(L"Resources\\Map\\Map3\\Wall1.bmp", L"MAP3_WALL1");
	// ======================= Map4 =======================
	Insert_Bmp(L"Resources\\Map\\Map4\\Map4.bmp", L"Map4");
	Insert_Bmp(L"Resources\\Map\\Map4\\Floor1.bmp", L"MAP4_FLOOR1");
	Insert_Bmp(L"Resources\\Map\\Map4\\Floor2.bmp", L"MAP4_FLOOR2");
	Insert_Bmp(L"Resources\\Map\\Map4\\Floor3.bmp", L"MAP4_FLOOR3");
	Insert_Bmp(L"Resources\\Map\\Map4\\Wall1.bmp", L"MAP4_WALL1");
	// Load 
// ======================= BossMap1 =======================
	Insert_Bmp(L"Resources\\Map\\BossMap1\\BossMap1.bmp", L"BOSSMAP1");
	Insert_Bmp(L"Resources\\Map\\BossMap1\\Floor1.bmp", L"BM1_FLOOR1");
	Insert_Bmp(L"Resources\\Map\\BossMap1\\Floor2.bmp", L"BM1_FLOOR2");
	Insert_Bmp(L"Resources\\Map\\BossMap1\\Floor3.bmp", L"BM1_FLOOR3");
	Insert_Bmp(L"Resources\\Map\\BossMap1\\Wall1.bmp", L"BM1_WALL1");
	Insert_Bmp(L"Resources\\Map\\BossMap1\\Wall2.bmp", L"BM1_WALL2");
	Init_Player_Info();
	Init_Monster_Info();
	Init_UI_Info();
	Init_Item_Info();
	Init_Effect_Info();
}

int CBmpMgr::Update()
{
	return 0;
}

void CBmpMgr::Release()
{
	for (auto& iter : m_mapBit)
	{
		DELETE_MEMORY(iter.second);
	}
	m_mapBit.clear();
}

void CBmpMgr::Insert_Bmp(const TCHAR* pFilePath, const TCHAR * pFileName)
{
	for (auto& iter : m_mapBit)
	{
		if (!lstrcmp(iter.first, pFileName))
		{
			return;
		}
	}
	CMyBmp* pMybmp = new CMyBmp();
	pMybmp->Load_Bmp(pFilePath);
	m_mapBit.emplace(pair<const TCHAR*, CMyBmp*>(pFileName, pMybmp));
}

CMyBmp * CBmpMgr::Find_Image_MyBmp(const TCHAR * pImgKey)
{
	CMyBmp* myBmp = nullptr;
	for (auto& iter : m_mapBit)
	{
		if (!lstrcmp(iter.first, pImgKey))
		{
			myBmp = iter.second;
			break;
		}
	}
	return myBmp;
}

HDC CBmpMgr::Find_Image(const TCHAR * pImgKey)
{
	CMyBmp* myBmp = nullptr;
	for (auto& iter : m_mapBit)
	{
		if (!lstrcmp(iter.first, pImgKey))
		{
			myBmp = iter.second;
			break;
		}
	}
	if (nullptr != myBmp)
		return myBmp->Get_MemDC();

	return nullptr;
}

void CBmpMgr::Init_Map_Data()
{
}

void CBmpMgr::Init_Player_Info()
{
#pragma region  PLAYER
	// Player
// ======================= IDLE =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Idle\\R.bmp", L"PL_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Idle\\L.bmp", L"PL_IDLE_L");
	// ======================= RUN =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Run\\R.bmp", L"PL_RUN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Run\\L.bmp", L"PL_RUN_L");
	// ======================= RISE =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Rise\\R.bmp", L"PL_RISE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Rise\\L.bmp", L"PL_RISE_L");
	// ======================= FALL =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Fall\\R.bmp", L"PL_FALL_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Fall\\L.bmp", L"PL_FALL_L");
	// ======================= FALL_TO_IDLE =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\FallToIdle\\R.bmp", L"PL_FALL_TO_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\FallToIdle\\L.bmp", L"PL_FALL_TO_IDLE_L");
	// ======================= ROLL_NOR =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Roll_Nor\\R.bmp", L"PL_ROLL_NOR_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Roll_Nor\\L.bmp", L"PL_ROLL_NOR_L");
	// ======================= DAMAGED =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Damaged\\R\\1.bmp", L"PL_DAMAGED_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Damaged\\L\\1.bmp", L"PL_DAMAGED_L");
	// ======================= One Hand =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\OneHand\\R.bmp", L"PL_ONEHAND_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\OneHand\\L.bmp", L"PL_ONEHAND_L");
	// ======================= BOW =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Bow\\R.bmp", L"PL_BOW_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Bow\\L.bmp", L"PL_BOW_L");
	// ======================= TWO HAND  =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\TwoHand\\R.bmp", L"PL_TWOHAND_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\TwoHand\\L.bmp", L"PL_TWOHAND_L");
	// ======================= Dagger  =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Dagger\\R.bmp", L"PL_DAGGER_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Dagger\\L.bmp", L"PL_DAGGER_L");
	// ======================= SpellCast  =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\SpellCast\\R.bmp", L"PL_SPELLCAST_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\SpellCast\\L.bmp", L"PL_SPELLCAST_L");
	// ======================= SpellFire  =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\SpellFire\\R.bmp", L"PL_SPELLFIRE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\SpellFire\\L.bmp", L"PL_SPELLFIRE_L");
	// ======================= Block  =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Block\\R.bmp", L"PL_BLOCK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Block\\L.bmp", L"PL_BLOCK_L");
	// ======================= BlockHit  =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\BlockHit\\R.bmp", L"PL_BLOCKHIT_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\BlockHit\\L.bmp", L"PL_BLOCKHIT_L");
	// ======================= Blocked =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Blocked\\R.bmp", L"PL_BLOCKED_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Blocked\\L.bmp", L"PL_BLOCKED_L");
	// ======================= Die =======================
	// R
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\1.bmp", L"PL_DIE_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\2.bmp", L"PL_DIE_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\3.bmp", L"PL_DIE_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\4.bmp", L"PL_DIE_R4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\5.bmp", L"PL_DIE_R5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\6.bmp", L"PL_DIE_R6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\7.bmp", L"PL_DIE_R7");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\8.bmp", L"PL_DIE_R8");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\R\\9.bmp", L"PL_DIE_R9");
	// L
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\1.bmp", L"PL_DIE_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\2.bmp", L"PL_DIE_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\3.bmp", L"PL_DIE_L3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\4.bmp", L"PL_DIE_L4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\5.bmp", L"PL_DIE_L5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\6.bmp", L"PL_DIE_L6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\7.bmp", L"PL_DIE_L7");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\8.bmp", L"PL_DIE_L8");
	Insert_Bmp(L"Resources\\Anim\\Character\\Player\\Stand\\Die\\L\\9.bmp", L"PL_DIE_L9");
#pragma endregion PLAYER
}

void CBmpMgr::Init_Monster_Info()
{
	// ========= Monster =========
#pragma region  CASTLE
	// ======== = Boss - Castle ======== =
	// ======================= IDLE =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\IDLE\\R.bmp", L"CASTLE_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\IDLE\\L.bmp", L"CASTLE_IDLE_L");
	// ======================= RUN =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\RUN\\R.bmp", L"CASTLE_RUN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\RUN\\L.bmp", L"CASTLE_RUN_L");
	// ======================= ATTACK =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\1.bmp", L"CASTLE_ATTACK_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\2.bmp", L"CASTLE_ATTACK_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\3.bmp", L"CASTLE_ATTACK_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\4.bmp", L"CASTLE_ATTACK_R4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\5.bmp", L"CASTLE_ATTACK_R5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\6.bmp", L"CASTLE_ATTACK_R6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\7.bmp", L"CASTLE_ATTACK_R7");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\8.bmp", L"CASTLE_ATTACK_R8");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\9.bmp", L"CASTLE_ATTACK_R9");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\R\\10.bmp", L"CASTLE_ATTACK_R10");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\1.bmp", L"CASTLE_ATTACK_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\2.bmp", L"CASTLE_ATTACK_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\3.bmp", L"CASTLE_ATTACK_L3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\4.bmp", L"CASTLE_ATTACK_L4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\5.bmp", L"CASTLE_ATTACK_L5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\6.bmp", L"CASTLE_ATTACK_L6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\7.bmp", L"CASTLE_ATTACK_L7");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\8.bmp", L"CASTLE_ATTACK_L8");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\9.bmp", L"CASTLE_ATTACK_L9");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\ATTACK\\L\\10.bmp", L"CASTLE_ATTACK_L10");
	// =======================Dash ATTACK =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\R\\1.bmp", L"CASTLE_DASHATK_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\R\\2.bmp", L"CASTLE_DASHATK_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\R\\3.bmp", L"CASTLE_DASHATK_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\R\\4.bmp", L"CASTLE_DASHATK_R4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\R\\5.bmp", L"CASTLE_DASHATK_R5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\R\\6.bmp", L"CASTLE_DASHATK_R6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\R\\7.bmp", L"CASTLE_DASHATK_R7");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\R\\8.bmp", L"CASTLE_DASHATK_R8");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\L\\1.bmp", L"CASTLE_DASHATK_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\L\\2.bmp", L"CASTLE_DASHATK_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\L\\3.bmp", L"CASTLE_DASHATK_L3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\L\\4.bmp", L"CASTLE_DASHATK_L4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\L\\5.bmp", L"CASTLE_DASHATK_L5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\L\\6.bmp", L"CASTLE_DASHATK_L6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\L\\7.bmp", L"CASTLE_DASHATK_L7");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASHATTACK\\L\\8.bmp", L"CASTLE_DASHATK_L8");
	// =======================Dash =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASH\\R\\1.bmp", L"CASTLE_DASH_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DASH\\L\\1.bmp", L"CASTLE_DASH_L1");
	// =======================RUNSTOP =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\RUNSTOP\\R\\1.bmp", L"CASTLE_RUNSTOP_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\RUNSTOP\\R\\2.bmp", L"CASTLE_RUNSTOP_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\RUNSTOP\\R\\3.bmp", L"CASTLE_RUNSTOP_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\RUNSTOP\\L\\1.bmp", L"CASTLE_RUNSTOP_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\RUNSTOP\\L\\2.bmp", L"CASTLE_RUNSTOP_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\RUNSTOP\\L\\3.bmp", L"CASTLE_RUNSTOP_L3");
	// ======================= DAMAGED =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\HIT\\R\\1.bmp", L"CASTLE_HIT_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\HIT\\L\\1.bmp", L"CASTLE_HIT_L1");
	// ======================= DIE =======================
	// R
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\R\\1.bmp", L"CASTLE_DEAD_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\R\\2.bmp", L"CASTLE_DEAD_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\R\\3.bmp", L"CASTLE_DEAD_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\R\\4.bmp", L"CASTLE_DEAD_R4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\R\\5.bmp", L"CASTLE_DEAD_R5");
	// L
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\L\\1.bmp", L"CASTLE_DEAD_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\L\\2.bmp", L"CASTLE_DEAD_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\L\\3.bmp", L"CASTLE_DEAD_L3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\L\\4.bmp", L"CASTLE_DEAD_L4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE\\L\\5.bmp", L"CASTLE_DEAD_L5");
	// ======================= PHASE 2=======================
// R
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\R\\1.bmp", L"CASTLE_PHASE2_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\R\\2.bmp", L"CASTLE_PHASE2_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\R\\3.bmp", L"CASTLE_PHASE2_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\R\\4.bmp", L"CASTLE_PHASE2_R4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\R\\5.bmp", L"CASTLE_PHASE2_R5");
	// L
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\L\\1.bmp", L"CASTLE_PHASE2_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\L\\2.bmp", L"CASTLE_PHASE2_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\L\\3.bmp", L"CASTLE_PHASE2_L3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\L\\4.bmp", L"CASTLE_PHASE2_L4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\PHASE2\\L\\5.bmp", L"CASTLE_PHASE2_L5");
	// ======================= WING =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\WING\\L.bmp", L"CASTLE_WING");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\WING\\R.bmp", L"CASTLE_WING");
	// ======================= P2_IDLE =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\IDLE_P2\\R.bmp", L"CASTLE_P2_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\IDLE_P2\\L.bmp", L"CASTLE_P2_IDLE_L");
	// ======================= P2_CAST =======================
	// L
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\R\\1.bmp", L"CASTLE_P2_CAST_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\R\\2.bmp", L"CASTLE_P2_CAST_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\R\\3.bmp", L"CASTLE_P2_CAST_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\R\\4.bmp", L"CASTLE_P2_CAST_R4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\R\\5.bmp", L"CASTLE_P2_CAST_R5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\R\\6.bmp", L"CASTLE_P2_CAST_R6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\R\\7.bmp", L"CASTLE_P2_CAST_R7");
	// R
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\L\\1.bmp", L"CASTLE_P2_CAST_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\L\\2.bmp", L"CASTLE_P2_CAST_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\L\\3.bmp", L"CASTLE_P2_CAST_L3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\L\\4.bmp", L"CASTLE_P2_CAST_L4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\L\\5.bmp", L"CASTLE_P2_CAST_L5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\L\\6.bmp", L"CASTLE_P2_CAST_L6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST\\L\\7.bmp", L"CASTLE_P2_CAST_L7");
	// ======================= P2_CAST_FRONT =======================
// L
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\R\\1.bmp", L"CASTLE_P2_CAST_F_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\R\\2.bmp", L"CASTLE_P2_CAST_F_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\R\\3.bmp", L"CASTLE_P2_CAST_F_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\R\\4.bmp", L"CASTLE_P2_CAST_F_R4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\R\\5.bmp", L"CASTLE_P2_CAST_F_R5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\R\\6.bmp", L"CASTLE_P2_CAST_F_R6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\R\\7.bmp", L"CASTLE_P2_CAST_F_R7");
	// R
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\L\\1.bmp", L"CASTLE_P2_CAST_F_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\L\\2.bmp", L"CASTLE_P2_CAST_F_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\L\\3.bmp", L"CASTLE_P2_CAST_F_L3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\L\\4.bmp", L"CASTLE_P2_CAST_F_L4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\L\\5.bmp", L"CASTLE_P2_CAST_F_L5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\L\\6.bmp", L"CASTLE_P2_CAST_F_L6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\CAST_FRONT\\L\\7.bmp", L"CASTLE_P2_CAST_F_L7");
	// ======================= P2_DIE_2 =======================
	// R
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\1.bmp", L"CASTLE_DIE_P2_R1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\2.bmp", L"CASTLE_DIE_P2_R2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\3.bmp", L"CASTLE_DIE_P2_R3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\4.bmp", L"CASTLE_DIE_P2_R4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\5.bmp", L"CASTLE_DIE_P2_R5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\6.bmp", L"CASTLE_DIE_P2_R6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\7.bmp", L"CASTLE_DIE_P2_R7");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\8.bmp", L"CASTLE_DIE_P2_R8");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\9.bmp", L"CASTLE_DIE_P2_R9");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\10.bmp", L"CASTLE_DIE_P2_R10");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\11.bmp", L"CASTLE_DIE_P2_R11");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\R\\12.bmp", L"CASTLE_DIE_P2_R12");
	// L
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\1.bmp", L"CASTLE_DIE_P2_L1");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\2.bmp", L"CASTLE_DIE_P2_L2");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\3.bmp", L"CASTLE_DIE_P2_L3");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\4.bmp", L"CASTLE_DIE_P2_L4");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\5.bmp", L"CASTLE_DIE_P2_L5");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\6.bmp", L"CASTLE_DIE_P2_L6");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\7.bmp", L"CASTLE_DIE_P2_L7");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\8.bmp", L"CASTLE_DIE_P2_L8");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\9.bmp", L"CASTLE_DIE_P2_L9");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\10.bmp", L"CASTLE_DIE_P2_L10");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\11.bmp", L"CASTLE_DIE_P2_L11");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DIE_P2\\L\\12.bmp", L"CASTLE_DIE_P2_L12");
	// ======================= BLOCK =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\BLOCK\\R\\1.bmp", L"CASTLE_BLOCK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\BLOCK\\L\\1.bmp", L"CASTLE_BLOCK_L");
	// ======================= BLOCKHIT =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\BLOCK_HIT\\R\\2.bmp", L"CASTLE_BLOCK_HIT_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\BLOCK_HIT\\L\\2.bmp", L"CASTLE_BLOCK_HIT_L");
	// ======================= BLOCKHIT =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\BLOCKED\\R\\2.bmp", L"CASTLE_BLOCKED_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\BLOCKED\\L\\2.bmp", L"CASTLE_BLOCKED_L");
	// ======================= DAMAGED =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DAMAGED_P2\\R.bmp", L"CASTLE_HIT_P2_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Boss\\Castle\\DAMAGED_P2\\L.bmp", L"CASTLE_HIT_P2_L");
#pragma endregion CASTLE
#pragma region ABOMI_GREEN
	// ================ IDLE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\IDLE\\R.bmp", L"ABOMI_GREEN_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\IDLE\\L.bmp", L"ABOMI_GREEN_IDLE_L");
	// ================ RUN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\RUN\\R.bmp", L"ABOMI_GREEN_RUN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\RUN\\L.bmp", L"ABOMI_GREEN_RUN_L");
	// ================ ATTACK ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\ATTACK\\R.bmp", L"ABOMI_GREEN_ATTACK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\ATTACK\\L.bmp", L"ABOMI_GREEN_ATTACK_L");
	// ================ DAMAGED ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\DAMAGED\\R.bmp", L"ABOMI_GREEN_DAMAGED_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\DAMAGED\\L.bmp", L"ABOMI_GREEN_DAMAGED_L");
	// ================ DIE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\DIE\\R.bmp", L"ABOMI_GREEN_DIE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\DIE\\L.bmp", L"ABOMI_GREEN_DIE_L");
#pragma endregion ABOMI_GREEN
#pragma region  DEMONASSASSIN
	// ================ IDLE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\IDLE\\R.bmp", L"DEMASS_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\IDLE\\L.bmp", L"DEMASS_IDLE_L");
	// ================ RUN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\WALK\\R.bmp", L"DEMASS_WALK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\WALK\\L.bmp", L"DEMASS_WALK_L");
	// ================ RUN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\RUN\\R.bmp", L"DEMASS_RUN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\RUN\\L.bmp", L"DEMASS_RUN_L");
	// ================ ATTACK ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\ATTACK\\R.bmp", L"DEMASS_ATTACK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\ATTACK\\L.bmp", L"DEMASS_ATTACK_L");
	// ================ DAMAGED ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\DAMAGED\\R.bmp", L"DEMASS_DAMAGED_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\DAMAGED\\L.bmp", L"DEMASS_DAMAGED_L");
	// ================ DAMAGE_TAKEN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\DAMAGE_TAKEN\\R.bmp", L"DEMASS_DAMAGE_TAKEN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\DAMAGE_TAKEN\\L.bmp", L"DEMASS_DAMAGE_TAKEN_L");
	// ================ DIE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\DIE\\R.bmp", L"DEMASS_DIE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\DIE\\L.bmp", L"DEMASS_DIE_L");
#pragma endregion DEMONASSASSIN
#pragma region DEMMONESS
	// ================ IDLE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\IDLE\\R.bmp", L"DEMESS_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\IDLE\\L.bmp", L"DEMESS_IDLE_L");
	// ================ RUN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\WALK\\R.bmp", L"DEMESS_WALK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\WALK\\L.bmp", L"DEMESS_WALK_L");
	// ================ ATTACK ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\ATTACK\\R.bmp", L"DEMESS_ATTACK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\ATTACK\\L.bmp", L"DEMESS_ATTACK_L");
	// ================ DAMAGE_TAKEN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\DAMAGE_TAKEN\\R.bmp", L"DEMESS_DAMAGE_TAKEN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\DAMAGE_TAKEN\\L.bmp", L"DEMESS_DAMAGE_TAKEN_L");
	// ================ DIE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\DIE\\R.bmp", L"DEMESS_DIE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\Demmoness\\DIE\\L.bmp", L"DEMESS_DIE_L");
#pragma endregion DEMMONESS
#pragma region  ZOMBIEGIRL
	// ================ IDLE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\ZombieGirl\\IDLE\\R.bmp", L"ZOMBIEGIRL_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\ZombieGirl\\IDLE\\L.bmp", L"ZOMBIEGIRL_IDLE_L");
	// ================ RUN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\ZombieGirl\\WALK\\R.bmp", L"ZOMBIEGIRL_WALK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\ZombieGirl\\WALK\\L.bmp", L"ZOMBIEGIRL_WALK_L");
	// ================ ATTACK ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\ZombieGirl\\ATTACK\\R.bmp", L"ZOMBIEGIRL_ATTACK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\ZombieGirl\\ATTACK\\L.bmp", L"ZOMBIEGIRL_ATTACK_L");
	// ================ DAMAGE_TAKEN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\ZombieGirl\\DAMAGE_TAKEN\\R.bmp", L"ZOMBIEGIRL_DAMAGE_TAKEN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\ZombieGirl\\DAMAGE_TAKEN\\L.bmp", L"ZOMBIEGIRL_DAMAGE_TAKEN_L");
	// ================ DIE ZombieGirl
//	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\DIE\\R.bmp", L"DEMASS_DIE_R");
//	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAssassin\\DIE\\L.bmp", L"DEMASS_DIE_L");
#pragma endregion DEMONASSASSIN
#pragma region  SKELETONARCHER
// ================ IDLE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\SkeletonAcher\\IDLE\\R.bmp", L"SKARCHER_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\SkeletonAcher\\IDLE\\L.bmp", L"SKARCHER_IDLE_L");
	// ================ RUN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\SkeletonAcher\\WALK\\R.bmp", L"SKARCHER_WALK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\SkeletonAcher\\WALK\\L.bmp", L"SKARCHERS_WALK_L");
	// ================ ATTACK ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\SkeletonAcher\\ATTACK\\R.bmp", L"SKARCHER_ATTACK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\SkeletonAcher\\ATTACK\\L.bmp", L"SKARCHER_ATTACK_L");
	// ================ DAMAGE_TAKEN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\SkeletonAcher\\DAMAGE_TAKEN\\R.bmp", L"SKARCHER_DAMAGE_TAKEN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\SkeletonAcher\\DAMAGE_TAKEN\\L.bmp", L"SKARCHER_DAMAGE_TAKEN_L");
#pragma endregion SKELETONARCHER
#pragma region  DEMONASSASSIN
	// ================ IDLE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\IDLE\\R.bmp", L"DEMAXE_IDLE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\IDLE\\L.bmp", L"DEMAXE_IDLE_L");
	// ================ RUN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\WALK\\R.bmp", L"DEMAXE_WALK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\WALK\\L.bmp", L"DEMAXE_WALK_L");
	// ================ ATTACK ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\ATTACK\\R.bmp", L"DEMAXE_ATTACK_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\ATTACK\\L.bmp", L"DEMAXE_ATTACK_L");
	// ================ DAMAGE_TAKEN ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\DAMAGE_TAKEN\\R.bmp", L"DEMAXE_DAMAGE_TAKEN_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\DAMAGE_TAKEN\\L.bmp", L"DEMAXE_DAMAGE_TAKEN_L");
	// ================ DIE ================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\DEAD\\R.bmp", L"DEMAXE_DIE_R");
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\DemonAxe\\DEAD\\L.bmp", L"DEMAXE_DIE_L");
#pragma endregion DEMONASSASSIN
}

void CBmpMgr::Init_UI_Info()
{
	// UI
// ======================= BAR_BACKGROUND =======================
	Insert_Bmp(L"Resources\\UI\\Mp\\BarBackground.bmp", L"BAR_BACKGROUND");
	// ======================= HP =======================
	Insert_Bmp(L"Resources\\UI\\Hp\\hp.bmp", L"HP_RED");
	Insert_Bmp(L"Resources\\UI\\BossHp\\hp.bmp", L"HP_RED_BOSS");
	// ======================= MP =======================
	Insert_Bmp(L"Resources\\UI\\Mp\\mp.bmp", L"MP_BLUE");
	// ======================= ACTION SLOT =======================
	Insert_Bmp(L"Resources\\UI\\Slot\\ActionSlot.bmp", L"ACT_SLOT");
	Insert_Bmp(L"Resources\\UI\\Slot\\ActionSlotMask.bmp", L"ACT_BACK");
	// ======================= ITEM SLOT =======================
	Insert_Bmp(L"Resources\\UI\\Slot\\ItemSlot.bmp", L"ITEM_SLOT");
	Insert_Bmp(L"Resources\\UI\\Slot\\ItemSlotMask.bmp", L"ITEM_BACK");
	// ======================= Inventory =======================
	Insert_Bmp(L"Resources\\UI\\Inventory\\PanelTrans.bmp", L"INVEN_MAIN");
	Insert_Bmp(L"Resources\\UI\\Inventory\\PanelTransBack.bmp", L"INVEN_BACK");
	Insert_Bmp(L"Resources\\UI\\Inventory\\InvenSlot.bmp", L"INVEN_SLOT");
	Insert_Bmp(L"Resources\\UI\\Inventory\\EQUIP_IMG.bmp", L"EQUIP_IMG");
	Insert_Bmp(L"Resources\\UI\\Inventory\\ITEM_IMG.bmp", L"ITEM_IMG");
	Insert_Bmp(L"Resources\\UI\\Inventory\\THINGS_IMG.bmp", L"THINGS_IMG");
}

void CBmpMgr::Init_Item_Info()
{
	// ======================= SHOP =======================
	Insert_Bmp(L"Resources\\Map\\Shop.bmp", L"SHOP");
	// ======================= RESET  =======================
	Insert_Bmp(L"Resources\\Item\\Equip\\Rotate.bmp", L"ITEM_ROTATE");
	Insert_Bmp(L"Resources\\Item\\Equip\\Reset.bmp", L"ITEM_RESET");
	// ======================= SWORD =======================
	// Juse
	Insert_Bmp(L"Resources\\Item\\Equip\\BOSS_SWORD1.bmp", L"BOSS_SWORD1");
	Insert_Bmp(L"Resources\\Item\\Equip\\BOSS_SWORD2.bmp", L"BOSS_SWORD2");
	// One Hand
	Insert_Bmp(L"Resources\\Item\\Equip\\LONGSWORD.bmp", L"LONGSWORD");
	Insert_Bmp(L"Resources\\Item\\Equip\\GRADIUS.bmp", L"GRADIUS");
	Insert_Bmp(L"Resources\\Item\\Equip\\HOLYSWORD.bmp", L"HOLYSWORD");

	// TwoHand
	Insert_Bmp(L"Resources\\Item\\Equip\\BLOODAXE.bmp", L"BLOODAXE");
	Insert_Bmp(L"Resources\\Item\\Equip\\THUNDERHAMER.bmp", L"THUNDERHAMER");
	Insert_Bmp(L"Resources\\Item\\Equip\\ZWEIHANDER.bmp", L"ZWEIHANDER");
	// Dagger
	Insert_Bmp(L"Resources\\Item\\Equip\\RUSTYDAGGER.bmp", L"RUSTYDAGGER");
	Insert_Bmp(L"Resources\\Item\\Equip\\RITUALDAGGER.bmp", L"RITUALDAGGER");
	Insert_Bmp(L"Resources\\Item\\Equip\\HOLYDAGGER.bmp", L"HOLYDAGGER");
	// Maggic
	Insert_Bmp(L"Resources\\Item\\Equip\\CHAINLIGHTNING.bmp", L"CHAINLIGHTNING");
	Insert_Bmp(L"Resources\\Item\\Equip\\CONSUMECORPSE.bmp", L"CONSUMECORPSE");
	Insert_Bmp(L"Resources\\Item\\Equip\\FIREBALL.bmp", L"FIREBALL");
	Insert_Bmp(L"Resources\\Item\\Equip\\FIREBOLT.bmp", L"FIREBOLT");
	Insert_Bmp(L"Resources\\Item\\Equip\\FREEZING.bmp", L"FREEZING");
	Insert_Bmp(L"Resources\\Item\\Equip\\SPARK.bmp", L"SPARK");
	// SHIELD
	Insert_Bmp(L"Resources\\Item\\Equip\\SHIELD.bmp", L"SHIELD");
	Insert_Bmp(L"Resources\\Item\\Equip\\SHIELD_SIDE.bmp", L"SHIELD_SIDE");
	Insert_Bmp(L"Resources\\Item\\Equip\\MAGICSHIELD.bmp", L"MAGICSHIELD");
	Insert_Bmp(L"Resources\\Item\\Equip\\MAGICSHIELD_SIDE.bmp", L"MAGICSHIELD_SIDE");
	Insert_Bmp(L"Resources\\Item\\Equip\\LIFESHIELD.bmp", L"LIFESHIELD");
	Insert_Bmp(L"Resources\\Item\\Equip\\LIEFSHIELD_SIDE.bmp", L"LIEFSHIELD_SIDE");
	// ======================= BOW  =======================
	Insert_Bmp(L"Resources\\Item\\Equip\\BOW.bmp", L"BOW");
	// ======================= ARROW  =======================
	Insert_Bmp(L"Resources\\Anim\\Character\\Monster\\AbomiGreen\\Arrow_1.bmp", L"Arrow_1");
	// ======================= DISH  =======================
	Insert_Bmp(L"Resources\\Item\\Dish\\ALTARI.bmp", L"ALTARI");
	Insert_Bmp(L"Resources\\Item\\Dish\\BREAD.bmp", L"BREAD");
	Insert_Bmp(L"Resources\\Item\\Dish\\BROCCOLI.bmp", L"BROCCOLI");
	Insert_Bmp(L"Resources\\Item\\Dish\\CARROT.bmp", L"CARROT");
	Insert_Bmp(L"Resources\\Item\\Dish\\CHEEZE.bmp", L"CHEEZE");
	Insert_Bmp(L"Resources\\Item\\Dish\\COFFEE.bmp", L"COFFEE");
	Insert_Bmp(L"Resources\\Item\\Dish\\COLA.bmp", L"COLA");
	Insert_Bmp(L"Resources\\Item\\Dish\\HAMBUGGER.bmp", L"HAMBUGGER");
	Insert_Bmp(L"Resources\\Item\\Dish\\HPPOSSION.bmp", L"HPPOSSION");
	Insert_Bmp(L"Resources\\Item\\Dish\\ICESPARK.bmp", L"ICESPARK");
	Insert_Bmp(L"Resources\\Item\\Dish\\MPPOSSION.bmp", L"MPPOSSION");
	// ======================= Door  =======================
	Insert_Bmp(L"Resources\\Anim\\WorldObj\\Door.bmp", L"DOOR");
}

void CBmpMgr::Init_Effect_Info()
{
	// ===================== BLOOD =====================
	Insert_Bmp(L"Resources\\Effect\\BLOOD3_L.bmp", L"BLOOD1_L");
	Insert_Bmp(L"Resources\\Effect\\BLOOD3_R.bmp", L"BLOOD1_R");
	Insert_Bmp(L"Resources\\Effect\\BLOOD4_L.bmp", L"BLOOD2_L");
	Insert_Bmp(L"Resources\\Effect\\BLOOD4_R.bmp", L"BLOOD2_R");
}
