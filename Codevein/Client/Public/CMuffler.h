#pragma once
#include "CEquipment.h"

BEGIN(Client)

class CMuffler : public CEquipment
{
public:
	typedef struct tagMufflerDesc
	{
		ITEMDESC				tItemDesc;
		EQUIPDESC			tEquipDesc;
	}MUFFLERDESC;
protected:
	explicit CMuffler(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CMuffler(const CMuffler& _rhs);
	virtual ~CMuffler() = default;
public:
	virtual const char* Get_Name() { return "Muffler"; }
	virtual const _tchar* Get_Proto() { return PROTO_GOB_MUFFLER; }
	virtual const _tchar* Get_Layer() { return LAYER_WEAPON; }

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;
	// Find HierachyNode
	virtual HRESULT Find_HierackyNode(CGameObject* _pGameObject, const string& _str = "") override;

protected:
	// 장착당할 노드
	CHierarchyNode* m_pEquipMatrixNode = nullptr;
	// Node의 Hieracky Node
	CHierarchyNode* m_pBindMatrixNode = nullptr;
	// Bind Pos Node
	CHierarchyNode* m_pBindPosNode = nullptr;
	// Target Transform 
	CTransform* m_pTargetTransform = nullptr;
	// Target Model
	CModel* m_pTargetModel = nullptr;
public:
	static CMuffler* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END