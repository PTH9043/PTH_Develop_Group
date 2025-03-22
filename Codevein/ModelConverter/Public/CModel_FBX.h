#pragma once
#include "ModelDefines.h"
#include "CMeshContainer_FBX.h"
#include "CHierarchyNode_FBX.h"
#include "CAnimation_FBX.h"
#include "CComponent.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Model)

class CModel_FBX final : public CComponent
{
public:
	
public:
	enum TYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END};
protected:
	explicit CModel_FBX(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CModel_FBX(const CModel_FBX& _rhs);
	virtual ~CModel_FBX() = default;

public:
	void Get_Data(MODELDESC& _tModelDesc);
	void Get_Data(ANIMMODELDESC& _tAnimModelDesc);
public:
	// Save NonAnimModel
	void Save_NonAnimModel();
	// Save Anim Model
	void Save_AnimModel();
public:
	const _uint& Get_NumMeshContainers() const { return m_iNumMeshContainers; }
	FILEGROUP* Get_ModelFileGroup() const { return m_pModelFileGroup; }
	FILEDATA* Get_FileData() const { return m_pFileData; }
	vector<CMeshContainer_FBX*> Get_MeshContainers() { return m_MeshContainers; }
	class CHierarchyNode_FBX* Get_HierarchyNodes(const char* pHierarchyNodeName);
	CAnimation_FBX* Get_Animation() { return m_pCurAnimation; }
	const _uint& Get_AnimationCount() const { return m_iNumAnimations; }
public:
	void Set_CurAnimIndex(const _uint& _iAnimIndex) { m_iCurrentAnimIndex = _iAnimIndex; }
	const _uint& Get_CurAnimIndex() const { return m_iCurrentAnimIndex; }
public:
	// NativeConstruct Prototype
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, const _uint& _iIndex, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
public:
	// Update Animation
	void Update_Animation(const _double& _dDeltaTime);
	HRESULT Bind_SRV(CShader* _pShader, const char* _pConstantName, 
		const _uint& _iMeshContainerIndex, const aiTextureType& _eTextureType);
	HRESULT Render(const _uint& _iMeshContainersIndex, CShader* _pShader, const  _uint& _iPassIndex);
	// Create Animation
	HRESULT Create_Animation(FILEGROUP* _pFileGroup, FILEDATA* _pFileData);

public:
	void Set_Animation(const _uint& _iAnimIndex);
	void Set_Animation(const char* _pName);
protected:
	// Create MeshContainers
	HRESULT Create_MeshConstainers(const TYPE& _eType ,const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	HRESULT Create_Materials(FILEGROUP*& _pFileGroup);
	HRESULT Create_HierachyNodes(aiNode* _pNode, class CHierarchyNode_FBX* _pParents, const _uint& _iDepths = 0);
	HRESULT Create_Animation();
protected:
	const aiScene*			m_pScene = nullptr;
	Importer						m_Importer;

protected:
	typedef vector<CMeshContainer_FBX*>	MESHCONTAINERS;
	MESHCONTAINERS		m_MeshContainers;
	_uint								m_iNumMeshContainers = 0;

protected:
	_uint								m_iNumMaterials = 0;
	typedef vector<MODELMATRIALDESC*> MATRIALS;
	MATRIALS						m_Materials;

private:
	vector<class CHierarchyNode_FBX*>			m_HierarchyNodes;
	typedef vector<class CHierarchyNode_FBX*>	HIERARCHYNODES;

private:
	_uint									m_iNumAnimations;
	vector<class CAnimation_FBX*>				m_Animations;
	typedef vector<class CAnimation_FBX*>		ANIMATIONS;

	CAnimation_FBX*				m_pCurAnimation = nullptr;
protected:
	// Model Name
	FILEGROUP*					m_pModelFileGroup = nullptr;
	FILEDATA*						m_pFileData = nullptr;

private:
	_float4x4					m_PivotMatrix;
	_uint						m_iCurrentAnimIndex = 0;
public:
	static CModel_FBX* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, 
		FILEGROUP* _pFileGroup, const _uint& _iIndex, const _fmatrix& _mPivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END
