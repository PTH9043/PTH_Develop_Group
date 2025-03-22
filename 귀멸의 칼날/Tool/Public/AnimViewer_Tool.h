#pragma once
#include "ImGuiClass_Tool.h"

BEGIN(Engine)
class CModel;
class CGameObject;
class CAnimationClip;
class CAnimator;
END

BEGIN(Tool)
class CAnimViewer_Tool final : public CImGuiClass_Tool
{
public:
    // 애니메이션 클립의 이벤트 타입
    enum CLIPEVENTTYPE
    {
        // 이펙트, UI, 사운드,카메라, 트리거  등등 필요시 추가예정
        EVENTTYPE_EFFECT,			// 이펙트
        EVENTTYPE_UI,				// UI
        EVENTTYPE_SOUND,			// SOUND
        EVENTTYPE_COLLIDER,			// COLLIDER
        EVENTTYPE_WEAPONBONE,		// WEAPONBONE
        EVENTTYPE_CAMERA,			// CAMERA
        EVENTTYPE_MESH,				// 특정 메쉬 숨기기
        EVENTTYPE_SPEEDSECTION,		// 애니메이션 재생의 특정 구간동안 속도를 조절하는 이벤트
        EVENTTYPE_BONETRANSFORM,	// 애니메이션 재생의 특정 구간동안 뼈의 위치값을 조절하는 이벤트		
        EVENTTYPE_GRAVITY,			// 애니메이션 재생의 특정 구간동안 중력값을 받을지 안받을지 결정하는 이벤트		
        EVENTTYPE_TIMECHECK,		// 애니메이션에서 특정 시간을 구하기 위한 이벤트		
        EVENTTYPE_TRAIL,			// 트레일
        EVENTTYPE_END
    };

    // 정보 Sorting 용 구조체와 열거체들
    enum MyItemColumnID
    {
        // 인덱스 정보
        MyItemColumnID_Index,
        // 이름정보
        MyItemColumnID_Name,
        // Animation의 Duration
        MyItemColumnID_Duration,
        // Animation의 TimePerSecond
        MyItemColumnID_TPS,
        // Channel의 프레임 카운트
        MyItemColumnID_FrameCount,
        // 뼈의 Depth정보
        MyItemColumnID_Depth,
        // 뼈의 부모 정보
        MyItemColumnID_Parent,
        // 선택됐는지 등의 정보를 기입
        MyItemColumnID_Action,
        // 애니메이터의 값들에 사용하게 됨.
        MyItemColumnID_EventCount,
        MyItemColumnID_EnterRatio,
        MyItemColumnID_ExitRatio,
        MyItemColumnID_LerpSpeed,
        MyItemColumnID_InputRatio,
        MyItemColumnID_Trigger,
        // 트랜지션의 EnterState
        MyItemColumnID_EnterState,
        // 애니메이터의 EnterClip
        MyItemColumnID_EnterClip,
        // 애니메이터의 ExitClip
        MyItemColumnID_ExitClip,
        MyItemColumnID_StartRatio,
        MyItemColumnID_EndRatio,
        MyItemColumnID_EndTime,
        MyItemColumnID_AnimSpeed,
        MyItemColumnID_EventType,
        MyItemColumnID_AnimationType
    };

    struct MyItem
    {
        int         Index;
        char        Name[MAX_PATH];
        int         ParentIndex;
        char        ParentName[MAX_PATH];
        _float      LerpSpeed;
        _double     Duration;
        _double     Tps;
        _double     dStartRatio;
        _double     dEndRatio;
        _double     dEndTime;
        _float      fValue;
        _float      fAnimSpeed;
        _uint       iEventType;
        _bool       bIsActive;
        _float4     vEventPos;
        _double     EnterRatio;
        _double     ExitRatio;
        _double     TriggerRatio;
        _uint       FrameCount;
        _uint       Depth;
        _bool       Action;
        _bool       Loop;
        _bool       RootBone;
        _int        EventCount;
        _int        EnterState;
        char        Trigger[MAX_PATH];
        char        MainAnim[MAX_PATH];
        char        SubAnim[MAX_PATH];
        char        EnterAnim[MAX_PATH];
        char        ExitAnim[MAX_PATH];
        char        EventText[MAX_PATH];
        char        AnimationType[MAX_PATH];


        static const ImGuiTableSortSpecs* s_current_sort_specs;

        // Compare function to be used by qsort()
        static int CompareWithSortSpecs(const void* lhs, const void* rhs)
        {
            const MyItem* a = (const MyItem*)lhs;
            const MyItem* b = (const MyItem*)rhs;
            for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
            {
                // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
                // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
                const  ImGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
                int delta = 0;
                switch (sort_spec->ColumnUserID)
                {
                case MyItemColumnID_Index:          delta = (a->Index - b->Index);              break;
                case MyItemColumnID_Name:           delta = (strcmp(a->Name, b->Name));         break;
                case MyItemColumnID_EnterClip:      delta = (strcmp(a->EnterAnim, b->EnterAnim));         break;
                case MyItemColumnID_ExitClip:       delta = (strcmp(a->ExitAnim, b->ExitAnim));         break;
                case MyItemColumnID_Duration:       delta = (_int)(a->Duration - b->Duration);  break;
                case MyItemColumnID_TPS:            delta = (_int)(a->Tps - b->Tps);            break;
                case MyItemColumnID_FrameCount:     delta = (a->FrameCount - b->FrameCount);    break;
                case MyItemColumnID_Depth:          delta = (a->Depth - b->Depth);              break;
                case MyItemColumnID_Parent:         delta = (a->ParentIndex - b->ParentIndex);  break;
                case MyItemColumnID_Action:         delta = (_int)(a->Action - b->Action);      break;
                case MyItemColumnID_EnterState:     delta = (a->EnterState - b->EnterState);      break;
                case MyItemColumnID_EventCount:     delta = (a->EventCount - b->EventCount);      break;
                case MyItemColumnID_EnterRatio:     delta = (_int)(a->EnterRatio - b->EnterRatio);      break;
                case MyItemColumnID_ExitRatio:      delta = (_int)(a->ExitRatio - b->ExitRatio);      break;
                case MyItemColumnID_AnimSpeed:      delta = (_int)(a->fAnimSpeed - b->fAnimSpeed);      break;
                case MyItemColumnID_LerpSpeed:      delta = (_int)(a->LerpSpeed - b->LerpSpeed);      break;
                case MyItemColumnID_InputRatio:     delta = (_int)(a->TriggerRatio - b->TriggerRatio);      break;
                case MyItemColumnID_Trigger:        delta = (strcmp(a->Trigger, b->Trigger));       break;
                case MyItemColumnID_EventType:      delta = (_int)(a->iEventType - b->iEventType);       break;
                case MyItemColumnID_AnimationType:  delta = (strcmp(a->AnimationType, b->AnimationType));       break;
                default: IM_ASSERT(0); break;
                }
                if (delta > 0)
                    return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
                if (delta < 0)
                    return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
            }

            // qsort() is instable so always return a way to differenciate items.
            // Your own compare function may want to avoid fallback on implicit sort specs e.g. a Name compare if it wasn't already part of the sort specs.
            return (a->Index - b->Index);
        }
    };


private:
    explicit CAnimViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CAnimViewer_Tool(void) = default;
public:
    virtual HRESULT NativeConstruct(void);
    virtual HRESULT NativeConstruct_InnerValue(void);
    virtual HRESULT Tick(const _double& _dTimeDelta);
    virtual HRESULT Late_Tick(const _double& _dTimeDelta);
    virtual HRESULT Release_InnerValue(void);

private:
    virtual void Show_Window(void) override;

private:
    //	AnimViewerModel 객체의 생성 함수 ( 처음 시작 시 생성 )
    HRESULT Create_AnimObject();
    //	AnimViewerModel 객체의 모델을 교체한다.
    HRESULT Set_Model();
    //  AnimViewerModel 객체의 애니메이션을 교체한다.
    HRESULT Set_Animation();
    //  AnimationClip 하나를 Create 하여 현재 컴포넌트에 넣어준다.
    HRESULT Add_AnimationClip();
    // 등록되어있는 모든 AnimationClip을 등록한다.    
    HRESULT Add_AllAnimationClip();
    //  모델에게 Animator를 전달한다.
    HRESULT Set_Animator(_wstring _strPath);
    //  AnimationClip 하나를 Delete한다.
    HRESULT Delete_AnimationClip(_string strName);
    //  현재 애니메이션 테스트 모델이 가진 애니메이터 정보를 저장합니다.
    HRESULT Save_Animator();
    //  애니메이션 클립을 저장합니다.
    HRESULT Save_AnimationClip();
    //  애니메이터 정보를 불러옵니다.
    HRESULT Load_Animator();
    //  애니메이션 클립 정보를 불러옵니다.
    HRESULT Load_AnimationClip();
    //  툴에서 필요한 정보들을 업데이트합니다.
    HRESULT Initialize_ToolData();
    // 선택한 애니메이터의 데이터를 불러와서 현재 Animator와 Clip정보, Transition정보를 합칩니다.
    HRESULT Merge_Animator(_wstring _strPath);
    // 애니메이터에서 같은 이름을 가진 애니메이션 클립의 이벤트 정보만 가져옵니다.
    HRESULT Merge_AnimatorEvent(_wstring _strPath);
    // ================== 태현 추가 ==================
    HRESULT Attach_SwordModel();
    HRESULT Attach_ShelteModel();
private:
    // 모델을 적용할 오브젝트
    CGameObject* m_pAnimObject = nullptr;
private:
    // 애니메이션 툴 창의 정보
    WINDESC			m_tWinDesc;
    // 경로를 찾아서 가져온 바이너리 파일의 정보를 담은 리스트
    vector<FILEDATA> m_BinaryList;
    // 경로를 찾아서 가져온 애니메이터 저장데이터 리스트
    vector<FILEDATA> m_SavedAnimatorList;
    // 경로를 찾아서 만든 모델 목록
    vector<CModel*> m_ModelList;
    // 내가 선택한 애니메이션 모델 인덱스 번호 -> 아무것도 선택되지 않았다면 -1로 초기화
    _int			m_iModelIndex = -1;
    // 내가 선택한 모델의 현재 애니메이션 인덱스 번호 -> 아무것도 선택되지 않았다면 -1로 초기화
    _int			m_iAnimationIndex = -1;
    // 선택한 애니메이터 목록
    _int            m_iAnimatorIndex = -1;
    // 현재 재생되고있는 애니메이션이 갖고있는 Channel중 내가 선택한 채널 -> 아무것도 선택되지 않았다면 -1로 초기화
    _int			m_iChannelIndex = -1;
    // 내가 선택한 모델의 현재 Bone 인덱스 번호 -> 아무것도 선택되지 않았다면 -1로 초기화
    _int			m_iBoneIndex = -1;
    // 내가 선택한 모델이 현재 Render에서 그리고있는 메쉬 컨테이너 인덱스 리스트
    vector<_uint>   m_MeshContainerList;
    // 애니메이션 툴 모델이 들고있는 이벤트 목록중 내가 선택한 이벤트 인덱스
    _int            m_iEventIndex = -1;
    // 현재 선택된 트랜지션 인덱스
    _int            m_iTransitionIndex = -1;
    // Channel중 KeyFrame이 2인 경우를 제외할 때 사용하기위한 bool 변수
    _bool			m_bIsFiltered = false;
    // 리스트들 업데이트용도.
    _bool           m_bIsUpdate = false;
    // 이벤트들 업데이트 용도
    _bool           m_bIsEventUpdate = false;
    // 콜라이더 타입을 입력해서 사용할 때 담아 둘 변수
    _string         m_strAnimType = "";
    // Battle 관련 사운드 텍스트 모음
    vector<_string> m_SoundNameList;
    // 사운드 텍스트 검색 리스트
    vector<_string> m_ResearchSoundList;
private:
    /*  태현 추가*/
    class CItem_Tool*      m_pSwordTool = nullptr;
    class CItem_Tool*      m_pShelteTool = nullptr;
    // ItemModelList
    vector<CModel*>     m_ItemModelList;
    // 애니메이션 클립리스트 업데이트용 변수
    _bool           m_bIsClipUpdate = false;
    // 정보를 담을 AnimationClip
    CAnimationClip* m_pAnimationClip = nullptr;
    // 현재 등록된 Animator
    CAnimator*      m_pAnimatorCom = nullptr;
    // 현재 등록된 Animator가 갖고있는 AniamtionClip 리스트
    vector<CAnimationClip*> m_ClipList;
public:
    static CAnimViewer_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void Free(void) override;
};
END

