#pragma once
#include "Base.h"

BEGIN(Engine)
class CCollider;
class CModel;
class CHierarchyNode;

class ENGINE_DLL CBonesCollider final : public CBase
{
protected:
	explicit CBonesCollider();
	virtual ~CBonesCollider() = default;

public:
	CCollider* Get_Collider() { return m_pCollider; }
	CHierarchyNode* Get_HierachyNode() { return m_pHierachyNode; }
	const _bool& Is_Collision() const { return m_bIsCollision; }
	_vector Get_Direction() { return m_vMoveDirection.Get_Vector(); };

public:
	void Add_Colliders(CModel* _pModel, const _char* _pNodeNames,
		const _uint& _iLevel, const _tchar* _pTag, void* _pArg = nullptr);
	_matrix MultiplyCollider(_matrix WorldMatrix = XMMatrixIdentity());

	_bool Is_Collision(CCollider* _pCollider);
	_bool Is_Collision(const char* _pName, CCollider* _pCollider);

	_bool Compute_Distance_Sphere(CBonesCollider* _pBoneCollider, _float& fDistance);

	HRESULT Save(CModel* _pModel, FILEGROUP* _pFileGroup);
	HRESULT Load(CModel* _pModel, FILEDATA* _pFileData, const _uint& _iLevel);
#ifdef _DEBUG
	void Render();
#endif
	void Release_Data();
private:
		CCollider*						m_pCollider = nullptr;
		 CHierarchyNode*				m_pHierachyNode = nullptr;
		_bool							m_bIsCollision = true;
		_wstring						m_strTag = L"";

		_float3							m_vMoveDirection;
		_float3							m_vPrePosition;
public:
	static CBonesCollider* Create();
	virtual void Free() override;
};

END