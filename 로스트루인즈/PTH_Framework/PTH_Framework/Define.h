#pragma once

#define WINCX		800
#define WINCY		600
#define PI				3.14f
#define VK_MAX	0xff
#define PURE			= 0

#define MON_DEADCOUNT_0 0
#define MON_DEADCOUNT_1 1
#define MON_DEADCOUNT_2 2

#define DEGREE_TO_RADIAN(x) {x * (180.f / PI)}
#define RADIAN_TO_DEGREE(x) {(PI / 180.f) * x}

#define PARTICLE_MAX 5000

// ====== TILE =======
// ====== TILE =======
#define SELECT_X 100
#define SELECT_Y 100

#define MAP_X 4000
#define MAP_Y 1600
// OBJ EVENT
#define		OBJ_NOEVENT					0
#define		OBJ_DEADEVENT				1

#define PL_EP_MAGIC_1 0
#define PL_EP_MAGIC_2 1
#define PL_EP_WEAPON_1 2
#define PL_EP_WEAPON_2 3

#define DEG2RAD PI / 180.f
#define RAD2DEG 180 / PI

// ID
// 0 = OBJ
// 1 = ACTOR
// 11 = PLAYER
#define ID_PLAYER 0x11000000
// 12 = MONSTER
#define ID_MONSTER 0x12000000
// 14 = ITEM
#define ID_ITEM 0x14000000
// 140 = EQUIP
// 1400 == WEAPON
// 14000 == SWORD
#define ID_WEAPON_LONGSWORD		0x14000001

#define ID_WORLDOBJ_SHOP					0x18000001

#define DELETE_MEMORY(x) if(nullptr != x) { delete x; x = nullptr; }

extern HWND g_hWnd;
extern float		g_fSound;