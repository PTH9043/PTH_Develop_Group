#pragma once
#include "ImGuiClass_Tool.h"
#include "Tool_Defines.h"

BEGIN(Engine)
class CModel;
class CGameObject;
END

BEGIN(Tool)
class CMapViewer_Tool final : public CImGuiClass_Tool
{
public:
	struct vertex3 
	{
		// vertexPos : 1개의 Point 위치 xyz
		_float3 vertexPos[3];
	};
private:
	CMapViewer_Tool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMapViewer_Tool(void) = default;
public:
	static CMapViewer_Tool* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
public:
	virtual HRESULT NativeConstruct(void);
	virtual HRESULT NativeConstruct_InnerValue(void);
	virtual HRESULT Tick(const _double& _dTimeDelta);
	virtual HRESULT Late_Tick(const _double& _dTimeDelta);
	virtual HRESULT Release_InnerValue(void);
	virtual HRESULT Render(void);
public:
	virtual void	Show_Window(void);
	void			Load_File(void);
	void			Show_Window_MapTool(void);
	void			Show_Window_Navigation(void);
	void			Show_Window_MonsterTool(void);
	void			Show_Window_ColliderTool(void);
public:
	virtual void	Free(void) override;
public:
	// CMapObj_Tool, CMapGround_Tool 클래스에서 사용하는함수, 어떤 모델인지 넘겨준다 
	CModel*			Get_SelectModel(void) { return m_pSelectModel; }
	CModel*			Get_GroundModel(void) { return m_pGroundModel; }
	CModel*			Get_MonsterModel(void) { return m_pSelectMonsterModel; }
private:
	int iToolMode = 0;
	int iVIBuffer = 0;
	int iCreateMode = 0;
	int iNaviMode = 0;
	int iMonsterMode = 0;
	// 맵 오브젝트 모델들을 이름, 모델로 맵으로 가지고있음
	map<_wstring, CModel*>	m_ObjModelMap;
	// 리스트박스에서 선택한 모델
	CModel*					m_pGroundModel = nullptr;
	CModel*					m_pSelectModel = nullptr;
	// Ground / 초기화작업때 처음 만들고, 업데이트때마다 계속 삭제, 생성할것
	CGameObject*			m_pGround = nullptr;
	//  Sky / 초기화작업때 처음 만들고, 업데이트때마다 계속 삭제, 생성할것
	CGameObject*			m_pSky = nullptr;
	// Modify모드에서 피킹으로 선택한 오브젝트 
	CGameObject*			m_pModifyObj = nullptr;
	// ImGui초기작업에 필요한 구조체 
	DOCKDESC				m_tMenu;
	DOCKDESC				m_tMapToolView;
	DOCKDESC				m_tNavigationView;
	DOCKDESC				m_tColliderView;
	// 이름을 멤버로 갖고있음
	_tchar					m_GroundName[MAX_PATH] = L"";
	_tchar					m_SkyName[MAX_PATH] = L"";
	_string					m_strFindFolderFileName = "";
	// 리스트박스에서 사용, 컨테이너 도는동안 카운트를 센다 
	_uint					m_iIndex = 0;
	// 네비툴에서 사용, 삼각형 하나씩 벡터로 갖고있음
	vector<vertex3>			m_vecSemo;
	vertex3					m_vertex3;
	// 네비툴에서 사용, 삼각형 1개가 되는지 체크하는 카운트 
	_uint					m_iCount = 0;
	_uint					m_iLoadCount = 0;
	// 세팅 불변수
	_bool					m_bInitSetting = false;
	_bool					m_bLoadCheck = false;
	_bool					m_bNaviLoadCheck = false;
	// 그라운드, 맵 오브젝트에 전체 적용할 피보메트릭스 
	_matrix					m_PivotMatrix;
	vector<_tchar*>			m_vecName;
	//=================Monster============================
	// 리스트박스에서 선택한 몬스터 모델 
	CModel*					m_pSelectMonsterModel = nullptr;
	CGameObject*			m_pModifyMonster = nullptr;
	// 몬스터 오브젝트 모델들 
	map<_wstring, CModel*>	m_MonsterModelMap;
	DOCKDESC				m_tMonsterView;
	_uint					m_iMonsterIndex = 0;
	_matrix					m_MonsterPivotmatrix;
	_bool					m_bMonsterLoadCheck = false;
	vector<_tchar*>			m_vecMonsterName;

	_float					m_ColliderSize[3]{};
	_float					m_fColliderYPos = 0;
	_bool					m_bColliderCheck = false;
};
END