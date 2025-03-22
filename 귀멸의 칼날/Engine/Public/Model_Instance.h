#pragma once
#include "Component.h"

BEGIN(Engine)
class CMeshContainer_Instance;
class CQuadTree;
class CAnimation_Instance;
class CHierarchyNode_Instance;
class CTexture;

class ENGINE_DLL CModel_Instance : public CComponent
{
public:
	typedef struct tagModelInstanceDesc
	{
		_uint iModelInstanceCount = 0;
		// 그룹 수
		_uint iGroupNumber = 0;
	}MODEL_INSTNACE_DESC;
public:
	enum TYPE { TYPE_ANIM, TYPE_NONANIM, TYPE_END };
protected:
	// Mesh Container
	typedef vector<CMeshContainer_Instance*>		MESHCONTAINERS;
	typedef vector<MODELMATRIALDESC>					MATRIALS;
	typedef vector<CHierarchyNode_Instance*>		HIERARCHYNODES;
	typedef vector<CAnimation_Instance*>					ANIMATIONS;

	typedef vector<ANIMATIONS>									ANIMATIONGROUP;
	typedef vector< HIERARCHYNODES>						HIERARCHYNODESGROUP;
public:
	const _uint& Get_NumMeshContainers() const { return m_iNumMeshContainers; }
	FILEGROUP* Get_FileGroup() const { return m_pFileGroup; }
	FILEDATA* Get_FileData() const { return m_pFileData; }

	MESHCONTAINERS Get_MeshContainers() { return m_vecMeshContainers; }
	CHierarchyNode_Instance* Get_HierarchyNodes_Proto(const _char* _pHierarchyNodeNames);
	HIERARCHYNODES   Get_HierachyNodeList(const _uint& _iGroupID);
	CHierarchyNode_Instance* Get_HierarchyNodes_Clone(const _uint& _iGroupID, const _char* _pHierarchyNodeNames);

	const _uint& Get_GroupNum() const { return m_iGroupNumber; }
protected:
	explicit CModel_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CModel_Instance(const CModel_Instance& _rhs);
	virtual ~CModel_Instance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup, 
		FILEDATA* _pFileData,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, FILEGROUP* _pFileGroup,
		const _wstring& _strName,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT NativeConstruct_Prototype(const TYPE& _eModelType, PATHS* _pPaths,
		const _wstring& _strName,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
public:
	// Update Animation
	void Update_Animation(const _double& _dTimeDelta);
	HRESULT Update_Value(TRANSFORMVECTOR Transforms, const _uint& _iIndex = 0);
	HRESULT Update_Value(F4POSVECTOR f4PosVector, const _uint& _iIndex = 0);
	HRESULT Update_Value(MATRIXES _Matrix, const _uint& _iIndex = 0);

	HRESULT Bind_SRV(class CShader* _pShader, const char* _pConstantName,
		const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType);
	_bool Check_Texture(const _uint& _iMeshContainerIndex, const ETextureType& _eTextureType);
	HRESULT Render(const _uint& _iMeshContainersIndex,
		CShader* _pShader, const  _uint& _iPassIndex);
	HRESULT Render_NonAnim(const _uint& _iMeshContainersIndex, 
		CShader* _pShader, const _uint& _iPassIndex);
	HRESULT	Bind_BoneTexture(_float4x4* BoneData, CShader* _pShader, _uint iSize);


	// 애니메이션 변경
	void Set_Animation(const _uint& _iIndex, const _uint& _iAnimIndex);
	void Set_Animation(const _uint& _iIndex, const char* _pName);
	void Change_Animation(const _uint& _iIndex, const _char* _pName);
	void Change_SameAnimation(const _uint& _iIndex, const _char* _pName);
	void Change_SameAnimationSup(const _uint& _iIndex, const _char* _pName);
	void Repeat_Animation(const _uint& _iIndex, const _char* _pName);
	void Set_NextAnimation(const _uint& _iIndex, const _char* _pName);
protected:
	// Create MeshContainers
	HRESULT Create_MeshConstainers(const MODELDESC& _pData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT Create_MeshConstainers_Anim(const ANIMMODELDESC& _pData, _fmatrix _mPivotMatrix = XMMatrixIdentity());
	HRESULT Create_Materials(const COMMODELDATA& _pData);
	HRESULT Create_HierachyNodes(const vector<HNODEDESC>& _tDescList);
	HRESULT Create_Animation(const vector<ANIMDESC>& _tDescList);
	HRESULT	Create_BoneTexture();

protected:
	// 로드 함수 (바이너리 로드 용)
	HRESULT Load_Data_NonAnim(MODELDESC& _tModelDesc);
	HRESULT Load_Data_Anim(ANIMMODELDESC& _tModelDesc);

protected:
	MATRIALS						m_vecMaterials;
	_uint							m_iNumMatrials = 0;
protected:
	HIERARCHYNODES					m_vecHierachyNodes;
	ANIMATIONS						m_vecAnimations;
protected:
	HIERARCHYNODESGROUP				m_vecHierarchyGroup;
	ANIMATIONGROUP					m_vecAnimationGroup;
protected:
	vector<CAnimation_Instance*>		m_CurrentAnimationGroup;
	vector<CAnimation_Instance*>		m_NextAnimationGroup;
	vector<CAnimation_Instance*>		m_PrevAnimationGroup;
	vector<_uint>						m_CurIndexGroup;
private:
	// Change Animation
	_bool							m_bChangeAnimation = false;
protected:
	// 메쉬
	MESHCONTAINERS					m_vecMeshContainers;
	_uint							m_iNumMeshContainers = 0;
protected:
	// Model Name
	FILEGROUP*						m_pFileGroup = nullptr;
	FILEDATA*						m_pFileData = nullptr;
	_float4x4						m_PivotMatrix;
protected: /* Instanace */
	// Model Instance Count
	_uint							m_iModelInstanceCount = 0;
	TYPE							m_eModelType = TYPE_END;
protected:
	// Group Number
	_uint							m_iGroupNumber= 0;
private:
	// BoneMatrix
	ID3D11Texture2D*				m_pBoneTexture = nullptr;
	ID3D11ShaderResourceView*		m_pBoneSRV = nullptr;

public:
	static CModel_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, FILEGROUP* _pFileGroup,
		FILEDATA* _pFileData,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	static CModel_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, FILEGROUP* _pFileGroup,
		const _wstring& _strName,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
	static CModel_Instance* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const TYPE& _eModelType, PATHS* _pPaths,
		const _wstring& _strName,  _fmatrix _mPivotMatrix = XMMatrixIdentity());
public:
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END