#pragma once
#include "Pawn.h"

BEGIN(Engine)
class CModel;
class CHierarchyNode;
class CTransform;
class CAnimator;
END

BEGIN(Client)

class CItem : public CPawn
{
public:
	typedef struct tagItemDesc
	{
		CModel*				pTargetModel = nullptr;
		CTransform*		pTargetTransform = nullptr;
		CAnimator*			pTargetAnimator = nullptr;
		_wstring				strModelProtoName = L"";
		FILEDATA*				pItemData = nullptr;
	}ITEMDESC;
	typedef enum tagItemType
	{
		ITEM_SWORD, ITEM_SHEATH, ITEM_BULLET, ITEM_WORLD, ITEM_END
	}ITEMTYPE;
protected:
	explicit CItem(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CItem(const CItem& _rhs);
	virtual ~CItem() = default;
public:
	//  Item Type
	const ITEMTYPE& Get_ItemType() const { return m_eItemType; }

	void Set_ColliderActive(const _bool& bIsActive) { m_bIsColliderActive = bIsActive; }

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT	RenderLight() override;
public:
	// Update Bind Transform
	HRESULT Update_BindTransform();
	// Get Move Direction
	_float3 Get_MoveDirection();
protected:
	// Set Up Constant
	virtual HRESULT SetUp_ConstantTable() override;
	// SetUp Components
	virtual HRESULT SetUp_Components() override;
	//  Set Up Load Data 
	virtual HRESULT SetUp_LoadData(void* _pArg);
protected:
	// Target Bone Matrix
	CHierarchyNode*			m_pTargetBoneMatrix = nullptr;
	CTransform*					m_pTargetTransform = nullptr;
	CModel*							m_pTargetModel = nullptr;
	CAnimator*						m_pTargetAnimator = nullptr;
	// Socket Matrix
	_float4x4							m_SocketMatrix;
	// Pre Socket Matrix
	_float4x4							m_PreSocketMatrix;
	// Item Type 
	ITEMTYPE							m_eItemType = ITEM_END;
	// Collider Active
	_bool								m_bIsColliderActive = false;
public:
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END