#include "Defines.h"
#include "CPathFinder.h"

CPathFinder::CPathFinder()
{
}

CPathFinder::~CPathFinder()
{
	for_each(m_tAStarDesc.vecTileList.begin(), m_tAStarDesc.vecTileList.end(), [&](auto& iter) { 	Safe_Delete(iter); });
}

HRESULT CPathFinder::ReadyPathFinder(std::array<std::array<_byte, W_WIDTH>, W_HEIGHT>& _pTileInfo, const _uint& _iTileWidth, const _uint& _iTileHeight)
{
	// NumVertices X Y
	m_tAStarDesc.iNumVerticesX = _iTileWidth;
	m_tAStarDesc.iNumVerticesY = _iTileHeight;
	// TileList
	m_tAStarDesc.vecTileList.reserve(_iTileHeight * _iTileWidth);
	m_tAStarDesc.vecAdjTileList.reserve(_iTileHeight * _iTileWidth);

	m_CheckClose.resize(_iTileHeight * _iTileWidth);
	m_CheckOpen.resize(_iTileHeight * _iTileWidth);
	m_Distatnce.resize(_iTileHeight * _iTileWidth);

	_float2 vPos{};
	// Ready Tile
	for (_uint i = 0; i < m_tAStarDesc.iNumVerticesY; ++i)
	{
		_float2 Pos = vPos;
		for (_uint j = 0; j < m_tAStarDesc.iNumVerticesX; ++j)
		{
			_uint iIndex = m_tAStarDesc.iNumVerticesX * i + j;
			TILE* pTile = new TILE();
			pTile->iIndex = iIndex;
			pTile->vPosition = _float2{ (float)j, (float)i };
			pTile->bType = _pTileInfo[j][i];
			m_tAStarDesc.vecTileList.push_back(pTile);
			Pos.x += static_cast<_float>(i);
		}
		vPos.y += static_cast<_float>(i);
	}
	// ADJ
	for (_uint i = 0; i < m_tAStarDesc.iNumVerticesY; ++i) {
		for (_uint j = 0; j < m_tAStarDesc.iNumVerticesX; ++j) {

			int iIndex = i * m_tAStarDesc.iNumVerticesX + j;
			m_tAStarDesc.vecAdjTileList.push_back(VECTOR<TILE*>());
			m_tAStarDesc.vecAdjTileList[iIndex].reserve(8);
			// 타일 오른쪽  끝 연결 안해준다.
			if (0 == (iIndex % m_tAStarDesc.iNumVerticesX))
				continue;

			// 타일 왼쪽 끝 연결 안해준다. 
			// 인덱스 / 타일 X 개수
			if ((m_tAStarDesc.iNumVerticesX - 1) == (iIndex % m_tAStarDesc.iNumVerticesX))
				continue;

			// Tile들 연결
			// 인덱스 / 타일 X 개수
			// Up, pLeft, pRight, pDown 검사
			TILE* pUp = CheckDirectionTile(m_tAStarDesc.vecTileList, iIndex + m_tAStarDesc.iNumVerticesX );
			TILE* pLeft = CheckDirectionTile(m_tAStarDesc.vecTileList, iIndex - 1);
			TILE* pRight = CheckDirectionTile(m_tAStarDesc.vecTileList,iIndex + 1);
			TILE* pDown = CheckDirectionTile(m_tAStarDesc.vecTileList, iIndex - m_tAStarDesc.iNumVerticesX);
			// Left Up, Right Up, Left Down, Right Down 검사

			// TileList
			TILE* pTileList[4] = { pUp, pLeft, pRight, pDown, /*pLeftUp, pRightUp, pLeftDown, pRightDown*/ };
			for (_uint k = 0; k < 4; ++k)
			{
				if (nullptr != pTileList[k])
				{
					bool bSame = false;
					for (auto& iter : m_tAStarDesc.vecAdjTileList[iIndex])
					{
						if (iter->iIndex == pTileList[k]->iIndex)
						{
							bSame = true;
							break;
						}
					}
					if (false == bSame)
					{
						m_tAStarDesc.vecAdjTileList[iIndex].push_back(pTileList[k]);
					}
				}
			}
		}
	}
	return S_OK;
}

void CPathFinder::StartAStar(const _uint _iStartX, const _uint _iStartY, const _uint _iTargetX, const _uint _iTargetY)
{
	ReleaseData();

	_uint iSize = (_uint)m_tAStarDesc.vecTileList.size();
	_uint iCurPosIndex = GetIndex(_iStartX, _iStartY);
	_uint iTargetPosIndex = GetIndex(_iTargetX, _iTargetY);

	// 인덱스가 범위 넘어 갔을 때
	if (iCurPosIndex >= iSize || iTargetPosIndex >= iSize)
		return;
	// 찾았으면 바로 리턴
	if (iCurPosIndex == iTargetPosIndex)
		return ;

	m_pCurTile = (m_tAStarDesc.vecTileList)[iCurPosIndex];
	m_pGoalTile = (m_tAStarDesc.vecTileList)[iTargetPosIndex];

	m_BestList.clear();

	if (0 != m_pGoalTile->bType)
		return;

	while (true)
	{
		if (0 >= m_tAStarDesc.vecAdjTileList[m_pCurTile->iIndex].size() || m_pCurTile->bType != 0)
		{
			++iCurPosIndex;
			// 인덱스가 범위 넘어 갔을 때
			if (iCurPosIndex >= iSize)
				break;
			// 찾았으면 바로 리턴
			if (iCurPosIndex == iTargetPosIndex)
				break;
			m_pCurTile = (m_tAStarDesc.vecTileList)[iCurPosIndex];
		}
		else
			break;
	}

	while (true)
	{
		if (0 >= m_tAStarDesc.vecAdjTileList[m_pGoalTile->iIndex].size())
		{
			++iTargetPosIndex;
			// 인덱스가 범위 넘어 갔을 때
			if (iTargetPosIndex >= iSize)
				break;
			m_pGoalTile = (m_tAStarDesc.vecTileList)[iTargetPosIndex];
		}
		else
			break;
	}
	MakeRoutine(m_pCurTile, m_pGoalTile);
}

void CPathFinder::AddBestList(concurrency::concurrent_queue<TILE*>& _queue)
{
	{
		_queue.clear();
		for (auto iter = m_BestList.rbegin(); iter != m_BestList.rend(); ++iter) {
			_queue.push((*iter));
		}
	}
}

void CPathFinder::MakeRoutine(TILE*& _pStart, TILE*& _pEnd)
{
	TILE* pTile = _pStart;
	// Find Size만큼 찾음
	//for(_uint i = 0; i < FIND_SIZE; ++i)
	while (true)
	{
		// Open List가 비어있지 않으면, 방문한 노드 보지 못하게 제거
		if (false == m_OpenList.empty())
		{
			m_OpenList.pop_front();
		}
		// 타일을 방문했다면, 다시 방문 하지 않게 추가
		m_CheckClose[pTile->iIndex] = true;
		// 각 Tile을 검사
		for (auto& CheckTile : m_tAStarDesc.vecAdjTileList[pTile->iIndex])
		{
			if (MAP_BLOCK == m_CheckOpen[CheckTile->iIndex])
				continue;
			// End == Check Tile
			if (_pEnd == CheckTile)
			{
				CheckTile->pParents = pTile;
				// Make Best List 
				MakeBestRoutin(m_pCurTile, m_pGoalTile);
				return;
			}
			_float2 _vDir = CheckTile->vPosition - pTile->vPosition;
			_float fCost = CalculateDistCost(_pStart->vPosition, _pEnd->vPosition, CheckTile->vPosition, pTile->vPosition);
			m_OpenList.emplace_front(CheckTile);
			m_CheckOpen[CheckTile->iIndex] = true;
			CheckTile->pParents = pTile;
			m_Distatnce[CheckTile->iIndex] = fCost;
		}
		// Distance 검사
		VECTOR<_float>& vecDistance = m_Distatnce;

		m_OpenList.sort([&vecDistance](TILE*& t1, TILE*& t2)->bool {
			return vecDistance[t1->iIndex] < vecDistance[t2->iIndex];
			});
		if (0 < m_OpenList.size())
			pTile = m_OpenList.front();
		else
			break;
	}
}

void CPathFinder::MakeBestRoutin(TILE*& _pStart, TILE*& _pEnd)
{
	m_BestList.emplace_back(_pEnd);
	TILE* pTile = _pEnd;
	while (true)
	{
		if (nullptr == pTile)
		{
			break;
		}
		pTile = pTile->pParents;
		if (pTile == _pStart)
			break;
		m_BestList.emplace_back(pTile);
	}
}

TILE* CPathFinder::CheckDirectionTile(VECTOR<TILE*>& _pTileList, const int& _iDirIndex)
{
	if (0 > _iDirIndex || (int)(m_tAStarDesc.iNumVerticesX * m_tAStarDesc.iNumVerticesY) <= _iDirIndex)
		return nullptr;

	if (MAP_BLOCK  == _pTileList[_iDirIndex]->bType)
		return nullptr;

	return _pTileList[_iDirIndex];
}

_uint CPathFinder::GetIndex(const _uint _x, const _uint _y)
{
	_uint		iIndex = _x * m_tAStarDesc.iNumVerticesX + _y;
	return iIndex;
}

_float CPathFinder::CalculateDistCost(const _float2& _v3Start, const _float2& _v3End, const _float2& _v3Visited, const _float2& _v3Cur)
{
	// 시작지에서 방문지까지 거리
	_float2 vCostF = _v3Visited - _v3Start;
	// 방문지에서 목표까지 거리
	_float2 vCostG = _v3Visited - _v3End;
	// 방문지에서 현재 있는 타일까지 거리
	_float2 vCostT = _v3Visited - _v3Cur;
	return vCostF.LengthSquared() + vCostG.LengthSquared() + vCostT.LengthSquared();
}

void CPathFinder::ReleaseData()
{	
	// 방문 노드
	ZeroMemory(m_CheckOpen.data(), m_CheckOpen.size());
	ZeroMemory(m_CheckClose.data(), m_CheckClose.size());
	// 방문할 리스트
	m_OpenList.clear();
	// 최상의 길 리스트
	m_BestList.clear();
	// 프레임 단위로 찾기 위한 변
}
