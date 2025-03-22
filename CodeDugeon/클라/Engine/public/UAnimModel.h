#pragma once
#include "UModel.h"

BEGIN(Engine)
class UShaderConstantBuffer;
class UAnimation;
class UTexture;
class UTransform;
class UPawn;
class UMeshFilter;
class UCollider;
/*
@ Date: 2024-02-04, Writer: 박태현
@ Explain
- 모델의 메쉬, 뼈 정보, 모델의 텍스쳐 정보들을 VECTOR 컨테이너들로 담고 있는 클래스이다.
애니메이션 정보도 들고 있어서 애니메이션을 실행할 수 있다.
*/
class  UAnimModel : public UModel{
	using ANIMEVENTCOLLIDERCONTAINER = UNORMAP<_int, SHPTR<UCollider>>;
	using ANIMATIONS = VECTOR<SHPTR<UAnimation>>;
	using SETUPBONEMATRIXES = ARRAY<_float4x4, MAX_BONE_SIZE>;
	using ANIMSTRINGS = UNORMAP<_wstring, _uint>;
public:
	UAnimModel(CSHPTRREF<UDevice> _spDevice);
	UAnimModel(const UAnimModel& _rhs);
	DESTRUCTOR(UAnimModel)
public:
	CLONE_MACRO(UAnimModel, "UAnimModel::Clone To Failed")
	virtual void Free() override;
	virtual HRESULT NativeConstruct() override;
	using UModel::NativeConstruct;
	// 해당 PATH로 모델 로드
	HRESULT NativeConstruct(const _wstring& _wstrPath, const _float4x4& _PivotMatrix);
	// 해당 폴더와 파일 데이터로 로드
	HRESULT NativeConstruct(CSHPTRREF<FILEGROUP> _spFileGroup, CSHPTRREF<FILEDATA> _spFileData, const _float4x4& _PivotMatrix);
	// PATH를 입력하고 파일 이름을 입력하면 찾아서 온다. 
	HRESULT NativeConstruct(const PATHS& _vecPaths, const _wstring& _wstrFileName, const _float4x4& _PivotMatrix);
	// 모델의 폴더 이름, 파일 이름을 입력하면 찾아서 모델 로드
	HRESULT NativeConstruct(const _wstring& _wstrModelFolder, const _wstring& _wstrFileName, const _float4x4& _PivotMatrix);

	virtual HRESULT NativeConstructClone(const VOIDDATAS& _vecDatas) override;
	// Tick Event 
	void TickEvent(UPawn* _pPawn, const _wstring& _wstrInputTrigger, const _double& _TimeDelta);
	// Tick Event 
	void TickEventToRatio(UPawn* _pPawn, const _wstring& _wstrInputTrigger, const _double& _dRatio, const _double& _TimeDelta);
	// 애니메이션을 timedelta의 흐름에 따라서 제어하는 함수 (TimeDelta는 현재 게임에서 흐르는 시간)
	void TickAnimation(const _double& _dTimeDelta);
	// 애니메이션을 timeAcc에 따라서 제어하는 함수 (TimAcc는 애니메이션 제어도)
	void UpdateCurAnimationToRatio(const _double& _dRatio);
	// 애니메이션을 Tick하면서 Event도 Tick하는 함수
	void TickAnimChangeTransform(CSHPTRREF<UTransform> _spTransform, const _double& _dTimeDelta);
	// 현재 애니메이션을 TimAcc 값에 따라서 제어하는 함수 Evnet도 TimeAcc에 따라 제어한다. 
	void TickAnimToTimeAccChangeTransform(CSHPTRREF<UTransform> _spTransform, const _double& _dTimeDelta, const _double& _TimeAc);
	// RootBone 노드의 Transform을 적용하는 함수
	void ApplyRootBoneTransform(CSHPTRREF<UTransform> _spTransform);
	// RootBone 노드의 Transform을 적용하는 함수
	void ApplyRootBoneMatrix(_float4x4& _matrix);
	// 애니메이션을 렌더하는 함수
	virtual HRESULT Render(const _uint _iMeshIndex, CSHPTRREF<UShader> _spShader, CSHPTRREF<UCommand> _spCommand) override;
	// Set Animation
	void SetAnimation(const _uint& _iAnimIndex);
	void SetAnimation(const _wstring& _wstrAnimName);
	void SetAnimation(const _uint& _iAnimIndex, const _double& _dNextTimeAcc);
	void SetAnimation(const _wstring& _wstrAnimName, const _double& _dNextTimeAcc);
	// Change Animation
	void ChangeAnimation(const _uint& _iAnimIndex);
	void ChangeAnimation(const _wstring& _wstrAnimName);
	// Change Animation, Apply Next Time Acc 
	void ChangeAnimation(const _uint& _iAnimIndex, const _double& _dNextTimeAcc);
	void ChangeAnimation(const _wstring& _wstrAnimName, const _double& _dNextTimeAcc);
	void OnShowOriginAnimation();
	void OnAdjustTransformToAnimation();
	void ResetCurAnimEvent();

	SHPTR<UCollider> BringAttackCollider(_int _iColliderType);
	SHPTR<UCollider> GetAttackCollider() { return m_spAttackCollisionCollider;}
	_bool IsCollisionAttackCollider(CSHPTRREF<UCollider> _spEnemyCollider);

	void OutAnimationDataToServer(const _wstring& _wstrPath);
	void UpdateDissolveTImer(const _double& _dTimeDelta);
	void ResetDissolveTimer();
public: /* get set */
	ANIMATIONS& GetAnimations() { return m_vecAnimations; }
	const ANIMATIONPARAM& GetAnimParam() const { return m_stAnimParam; }
	const ANIMSTRINGS& GetAnimStrings() const { return m_AnimNamesGroup; }
	CSHPTRREF<UAnimation> GetCurrentAnimation() const { return m_spCurAnimation; }
	CSHPTRREF<UAnimation> GetNextAnimation() const { return m_spNextAnimation; }
	CSHPTRREF<UMeshFilter> GetMeshFilter() const { return m_spMeshFilterController; }
	const _float GetSupplyLerpValue() const { return m_fSupplyLerpValue; }
	const _int GetCurrentAnimIndex() const { return m_iCurAnimIndex; }
	const _int GetNetAnimIndex() const { return m_iNextAnimIndex; }
	_bool IsChangeAnimation() const { return m_isChangeAnim; }
	_bool IsChangedAnimation() const { return m_spNextAnimation == nullptr; }
	_bool IsNotApplyAnimPosition() const { return m_isNotApplyAnimPosition; }

	const _float4x4& GetPivotMatirx() const { return m_mPivotMatrix; }

	void SetAnimModelRim(_int _SetRim) { m_DrawRim._drawRim = _SetRim; }
	void SetAnimModelRimColor(_float3 _SetRimColor) { m_DrawRim.RimRGB = _SetRimColor; }
	void SetSupplyLerpValue(const _float _fSupplyLerpValue) { this->m_fSupplyLerpValue = _fSupplyLerpValue; }
	void SetAnimParam(const ANIMATIONPARAM& _stAnimParam) { this->m_stAnimParam = _stAnimParam; }
	void UpdateAttackData(const _bool _isCanAttackSituation, CSHPTRREF<UCollider> _spCollider);
	void NotApplyAnimPositionEnable() { this->m_isNotApplyAnimPosition = true; }

private:
	// CreateAnimation
	HRESULT CreateAnimation(const VECTOR<ANIMDESC>& _convecAnimDesc, const _wstring& _wstrPath);
	// Load To Data
	virtual void LoadToData(const _wstring& _wstrPath) override;
	void LoadAnimMeshData(std::ifstream& _ifRead, VECTOR<ANIMMESHDESC>& _convecMeshes);
	void LoadAnimationData(std::ifstream& _ifRead, VECTOR<ANIMDESC>& _convecAnimes);

	void ChangeAnimIndex(const _uint& _iAnimIndex, _uint& _iIndex);
	void ChangeAnimIndex(const _wstring& _wstrAnimName, _uint& _iIndex);
	void SettingCurAnimSituation();
	void SettingNextAnimSituation();
	HRESULT CreateShaderConstantBuffer();
private:
	ANIMATIONS												m_vecAnimations;
	ANIMSTRINGS											m_AnimNamesGroup;
	VECTOR<SETUPBONEMATRIXES>		m_vecSetupBonMatrix;
	// Animations
	SHPTR< UShaderConstantBuffer>		m_spAnimShaderConstantBuffer;
	ANIMATIONPARAM									m_stAnimParam;
	// Bone, PrevBone
	SHPTR<UShaderConstantBuffer>		m_spBoneMatrixShaderConstantBuffer;
	SHPTR<UShaderConstantBuffer>		m_spPrevBoneMatrixShaderConstantBuffer;
	SHPTR<UShaderConstantBuffer>		m_spDrawRimLightBuffer;
	//UpLoadResource
	ComPtr<Dx12Resource>						m_cpUpLoadResource;

	SHPTR<UAnimation>								m_spCurAnimation;
	SHPTR<UAnimation>								m_spNextAnimation;

	_uint															m_iCurAnimIndex;
	_uint															m_iNextAnimIndex;
	_float															m_fSupplyLerpValue;
	_bool															m_isChangeAnim;
	_float4x4													m_mPivotMatrix;


	RIMDRAW			m_DrawRim;

	SHPTR<UMeshFilter>								m_spMeshFilterController;
	ANIMEVENTCOLLIDERCONTAINER		m_AnimEventColliderContainer;
	_bool															m_isCanAttackSituation;
	_bool															m_isNotApplyAnimPosition;
	SHPTR<UCollider>									m_spAttackCollisionCollider;
};

END
