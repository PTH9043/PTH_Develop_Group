#pragma once

template <class T>
using VECTOR = std::vector<T>;

template<class T>
using LIST = std::list<T>;

typedef struct tagTileDesc
{
	tagTileDesc()
		: pParents{ nullptr },
		vPosition{ 0, 0 },
		iIndex{ 0 },
		bType{ 0 }
	{}
	tagTileDesc*		pParents;
	unsigned int		iIndex;
	_float2				vPosition;
	char					bType;
}TILE;

typedef struct tagAStarDesc
{
	VECTOR<TILE*>					vecTileList;
	VECTOR<VECTOR<TILE*>>	vecAdjTileList;
	_uint									iNumVerticesX{ 0 };
	_uint									iNumVerticesY{ 0 };
}ASTARDESC;

class CPathFinder {
private:
	TILE*							m_pCurTile{ nullptr };
	TILE*							m_pGoalTile{ nullptr };
	// open List 
	VECTOR<char>			m_CheckOpen{};
	VECTOR<char>			m_CheckClose{};
	VECTOR<_float>		m_Distatnce{};
	// Tile
	LIST<TILE*>				m_OpenList{};
	LIST<TILE*>				m_BestList{};
	// AStarDesc
	ASTARDESC				m_tAStarDesc{};
	mutex							m_LockMutex{};
private:
	char							m_FindTarget{ 0 };
	char							m_Move{ 0 };
public:
	CPathFinder();
	~CPathFinder();

public:
	// Ready Path Finder
	HRESULT ReadyPathFinder(std::array<std::array<_byte, W_WIDTH>, W_HEIGHT>& _pTileInfo, const _uint& _iTileWidth, const _uint& _iTileHeight);
	// Find Pos
	void StartAStar(const _uint _iStartX, const _uint _iStartY, const _uint _iTargetX, const _uint _iTargetY);
public:
	void AddBestList(concurrency::concurrent_queue<TILE*>& _queue);
private:
	// MakeRoutine
	void MakeRoutine(TILE*& _pStart, TILE*& _pEnd);
	// Best Routine
	void MakeBestRoutin(TILE*& _pStart, TILE*& _pEnd);
	// Check DirectionTile
	TILE* CheckDirectionTile(VECTOR<TILE*>& _pTileList, const int& _iDirIndex);
	// GetIndex
	_uint GetIndex(const _uint _x, const _uint _y);
	// CalculateDistCost
	_float	CalculateDistCost(const _float2& _v3Start, const _float2& _v3End, const _float2& _v3Visited, const _float2& _v3Cur);
	// Release Data
	void ReleaseData();
};
