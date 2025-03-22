#pragma once
#include "CManager.h"
#include "CPlayer.h"

BEGIN(Client)

class CPlayer_Manager final : public CManager
{
private:
	explicit CPlayer_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPlayer_Manager() = default;

public:
	// Get Player 
	CPlayer* Get_Player() { return m_pPlayer; }
	// Spawn Pos
	const _float3& Get_PlayerSpawnPos() const { return m_v3PlayerSpawnPos; }
	void Set_PlayerSpawnPos(const _float3& _v3SpawnPos) { this->m_v3PlayerSpawnPos = _v3SpawnPos; }
public:
	HRESULT NativeConstruct();
	// Spanw Player
	void Spawn_Player(const LEVEL& _eLevel, const _float3& _v3Pos = _float3(0.f, 0.f, 0.f));
	// Delete Player
	void Delete_Player(const LEVEL& _eLevel);

	void Release_Player() { Safe_Release(m_pPlayer); m_pPlayer = nullptr; }
	// Reset Player
	void Reset_Player(const _float3& _v3Pos);
	// Set Data
	void Recieve_Data(const CPlayer::PLAYERDESC& _tPlayerDesc) { this->m_tPlayerDesc = _tPlayerDesc; }
	// Save Data
	HRESULT Save_Data(const _string& _strFolderPath);
	// Load Data 
	HRESULT Load_Data(const _string& _strFolderPath);

protected:
	// 플레이어
	CPlayer* m_pPlayer = nullptr;
	// Player Desc
	CPlayer::PLAYERDESC m_tPlayerDesc;
	// Player Spawn
	_float3	m_v3PlayerSpawnPos = _float3(0.f, 0.f, 0.f);
public:
	static CPlayer_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END