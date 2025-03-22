
#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
END

BEGIN(Client)

class CMapCollider : public CGameObject
{
public:
	typedef enum EMapColliderType {
		BATTLE_COLLIDER, JUMP_COLLIDER, SLIDE_COLLIDER, ENDING_COLLIDER, COLLIDER_END
	}MAPCOLLIDERTYPE;

protected:
	explicit CMapCollider(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CMapCollider(const CMapCollider& _rhs);
	virtual ~CMapCollider() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void  Tick(const _double& _dTimeDelta) override;
	virtual void Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Extern_ImGuiClass() override;

private:
	class CCharacter*			m_pStoryPlayer = nullptr;

	CRenderer*					m_pRendererCom = nullptr;

	map<_string, CCollider*>	m_MapCollider[COLLIDER_END];

private:
	CCollider*		m_TmpCollider = nullptr;
	_float3			m_vPos = _float3(0.f, 0.f, 0.f);
	_float3			m_vRot = _float3(0.f, 0.f, 0.f);
	_float3			m_vScale = _float3(0.f, 0.f, 0.f);

private:
	void			Check_Collision();
	void			Check_BattleCollider(_string _tagCollider, CCollider* _pBattleCollider);
	void			Check_JumpCollider(_string _tagCollider, CCollider* _pJumpCollider);
	void			Check_SlideCollider(_string _tagCollider, CCollider* _SlideCollider);
	void			Check_EndingCollider(_string _tagCollider, CCollider* _EndingCollider);

private:
	virtual HRESULT SetUp_Components();
	HRESULT			Add_Collider(MAPCOLLIDERTYPE eType, _string strColliderTag, _float3 vScale, _float3 vRot, _float3 vPos);
	CCollider*		Find_Collider(MAPCOLLIDERTYPE eType, _string strColliderTag);



public:
	static CMapCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END