#pragma once
#include "GameObject_Tool.h"

BEGIN(Engine)
class CModel;
class CHierarchyNode;
class CChannel;
class CMeshContainer;
class CAnimationClip;
class CAnimator;
class CBonesCollider;
END

BEGIN(Tool)
class CAnimViewerModel final : public CGameObject_Tool
{
private:
	explicit CAnimViewerModel(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	explicit CAnimViewerModel(const CAnimViewerModel& _rhs);
	virtual ~CAnimViewerModel() = default;
	// 태현 추가
public:
	CModel* Get_Model() { return m_pModelCom; }
public:
	// 애니메이터 컴포넌트를 받아옵니다.
	CAnimator* Get_Animator() { return m_pAnimatorCom; }

	// 현재 재생되고있는 애니메이션의 이름을 받아옵니다.
	const _string& Get_CurrentAnimationName();
	// 현재 재생되고있는 애니메이션의 인덱스 번호를 받아옵니다.
	const _uint& Get_CurretnAnimationIndex();
	// 현재 재생되고있는 애니메이션의 Duration 값을 받아옵니다.
	const _double& Get_AnimationDuration();
	// 현재 재생되고있는 애니메이션의 애니메이션 재생값을 받아옵니다 ( TimeAcc )
	const _double& Get_AnimationPlayTime();
	// 현재 재생되고있는 애니메이션의 TickPerSecond를 받아옵니다
	const _double& Get_TickPerSecond();
	// 현재 재생되고있는 애니메이션의 Ratio값을 받아옵니다.
	_double Get_AnimationRatio();
	// 현재 모델의 하이어라키 노드 리스트를 받아옵니다.
	vector<CHierarchyNode*> Get_HierarchyNodeList();
	// 현재 재생되고있는 애니메이션의 ChannelList를 받아옵니다.
	vector<CChannel*> Get_ChannelList();
	// 현재 모델의 메쉬 컨테이너 리스트를 받아옵니다.
	vector<CMeshContainer*> Get_MeshContainerList();


	// 애니메이션의 Loop 여부를 반환합니다.
	const _bool& Is_Loop() const { return m_bIsLoop; }
	// 애니메이션이 플레이중인지에 대한 여부를 반환합니다.
	const _bool& Is_Play() const { return m_bIsPlay; }
	// 애니메이션의 Loop여부를 설정합니다.
	void Set_Loop(const _bool& isLoop) { m_bIsLoop = isLoop; }
	// 애니메이션의 Play여부를 설정합니다.
	void Set_Play(const _bool& isPlay) { m_bIsPlay = isPlay; }
	// 애니메이션을 Stop 합니다. ( Play를 false로 전환 , 애니메이션을 현재 재생중인 애니메이션으로 다시 교체 후 뼈값 초기화 )
	void Set_Stop();
	// 애니메이션의 진행도를 조정합니다.
	void Set_ProgressRatio(_double Ratio);	
	// 애니메이션을 한 프레임 단위로 실행시킵니다.
	void Play_OneFrame(_double TimeDelta);

	// 활성화 할 메쉬 컨테이너의 인덱스 리스트를 설정합니다.
	void Set_MeshIndexList(vector<_uint> meshIndexList) { m_MeshList = meshIndexList; }
	/// <summary>
	/// 애니메이션 클립을 추가합니다.
	/// </summary>
	/// <param name="pClip">툴에서 만든 애니메이션 클립</param>
	void Add_AnimationClip(CAnimationClip* pClip);

public:
	// 모델을 세팅합니다.
	HRESULT Set_Model(CModel* pModel);
	// 애니메이터를 세팅합니다.
	HRESULT Set_Animator(CAnimator* pAnimator);
	// 현재 애니메이션 인덱스를 변경합니다.
	HRESULT Set_Animation(_uint iAnimIndex);

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	void  Tick(const _double& _dTimeDelta) override;
	void  Late_Tick(const _double& _dTimeDelta) override;
	virtual HRESULT  Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderValue();

public:
	virtual const char* Get_Name() override;
	virtual const _tchar* Get_Proto() override;
	virtual const _tchar* Get_Layer() override;

public:
	// 태현 추가
#pragma region USE_TOOL_METHOD
	// Extern EditValue
	virtual _bool Extern_EditValue();
	// Extern Hierakey Value
	virtual _bool Extern_HierakeyView(const string& _str);
	// Create Self 
	virtual HRESULT Create_Self(CGameObject** _ppOut);
	// Delete Self
	virtual HRESULT Delete_Self();
	// Save Data 
	virtual HRESULT Save_Data();
	// Load Data 
	virtual HRESULT Load_Data(CGameObject** _ppOut, const LEVEL& _eLevel, void* _pData = nullptr);
	// Get Picking
	virtual _bool Is_Picking(_float3* _pOut = nullptr);
	// Reset Data 
	virtual void Reset_Data();
#pragma endregion USE_TOOL_METHOD

private:
	// 애니메이터 컴포넌트
	CAnimator* m_pAnimatorCom = nullptr;
	CCollider* m_pAttackColliderCom = nullptr;
private:
	// 현재 재생중인 애니메이션 인덱스
	_uint m_iCurrentAnimationIndex = 0;
	// 애니메이션 Loop 여부를 판단하는 bool 변수
	_bool m_bIsLoop = false;
	// 애니메이션의 재생 여부를 판단하는 bool 변수
	_bool m_bIsPlay = false;
	// Render에서 그릴 메쉬 인덱스 목록 ( int )
	vector<_uint> m_MeshList;
	// 테스트 모델용 애니메이션 클립 ( 애니메이션 툴에서 애니메이션 클립을 만들때 미리 효과등을 보기 위해서 존재함. )
	CAnimationClip* m_pAnimationClip = nullptr;
	// 테스트 모델용 애니메이션 클립 리스트( 애니메이션 툴에서 만들어서 바로 넘겨줌 ) -> 애니메이터 실험용
	vector<CAnimationClip*> m_AnimClips;
	// 태현 추가
	// Bones Collider
	vector<class CBonesCollider*> m_BonesCollider;
	// Select Collider
	CBonesCollider* m_pBonesSelectCollider = nullptr;
	// Str 
	_string m_strText = "";
	// Select Node
	CHierarchyNode* m_pSelectNode = nullptr;
	// Save To Find Bone
	list<CHierarchyNode*> m_SaveToFindBoneList;
	// Select Type
	_uint			m_iSelectType = 0;

	class CCollider* m_pSphereCollider = nullptr;
	class CCollider* m_pCubeCollider = nullptr;
	// Prev World Matrix
	_float4x4			m_PrevWorldMatrix;
	_bool				m_bWrapSampler = false;
public:
	static CAnimViewerModel* Create(ID3D11Device*& _pDevice, ID3D11DeviceContext*& _pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END