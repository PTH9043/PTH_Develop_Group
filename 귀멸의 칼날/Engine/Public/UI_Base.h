

#pragma once

#include "GameObject.h"


BEGIN(Engine)

enum UIEffect {GROW, GROW_DECREASE, RIGHT, LEFT, DOWN, REPEAT_LR, UIEFFET_NONE};
enum AlphaEffect { PLUS_ALPHA , MINUS_ALPHA , REPEAT_ALPHA, ALPHA_NONE};


struct FileInfo 
{
	//기본적인 정보를 담는다
	_float	m_fX = 0.f;
	_float	m_fY = 0.f;
	_float  m_fSizeX = 0.f;
	_float  m_fSizeY = 0.f;
	_int	m_Depth = 0;
	_tchar  m_pName[20] = L"";
	_tchar  m_TextureName[20] = L"";
	_tchar  m_PrototypeTag[30] = L"";
	_int	CurrentLevel = 0;

	//효과에 필요한 정보를 담는다
	_uint    m_State = UIEFFET_NONE;
	_float   m_Speed = 0.f;
	_float   m_Max = 0.f;
	_float   m_Min = 0.f;
	_float   m_MoveX = 0.f;
	_float   m_MoveY = 0.f;
	_float   m_CreatTime = 0.f;
	_float   m_DeadTime = 0.f;

	//알파값과 관련된 정보를 담는다
	_uint    m_AlphaState = ALPHA_NONE;
	_float   m_Alpha = 1.f;
	_float   m_AlphaCreat = 0.f;
	_float   m_AlphaDead = 0.f;

};

class ENGINE_DLL CUI_Base abstract : public CGameObject
{

protected:
	explicit CUI_Base(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CUI_Base(const CUI_Base& _rhs);
	virtual ~CUI_Base() = default;
public:
	_float	Get_PosX()					{ return m_fX; }
	_float	Get_PosY()					{ return m_fY; }
	_float2	Get_Pos()					{ return _float2(m_fX, m_fY); }
	_float	Get_SizeX()					{ return m_fSizeX; }
	_float	Get_SizeY()					{ return m_fSizeY; }
	_float2	Get_Size()					{ return _float2(m_fSizeX, m_fSizeY); }
	_float4	Get_Pos_Size()				{ return _float4(m_fX, m_fY, m_fSizeX, m_fSizeY); }
	_int	Get_Depth()					{ return m_Depth; }
	_tchar* Get_Name()                  { return m_pName; }
	_tchar* Get_Texture()				{ return m_TextureName; }
	_tchar* Get_PrototypeTag()			{ return m_PrototypeTag; }
	
	_bool	Get_Effect()				{ return m_bEffect; }
	_float  Get_Speed()                 { return m_Speed; }
	_float  Get_MaxSize()               { return m_MaxSize; }
	_float  Get_MinSize()               { return m_MinSize; }
	_float  Get_MoveX()                 { return m_MoveX; }
	_float  Get_MoveY()					{ return m_MoveY; }
	_float  Get_CreatTime()				{ return m_CreatTime; }
	_float  Get_DeadTime()				{ return m_DeadTime; }
	_uint   Get_State()                 { return m_State; }
	
	_uint   Get_AlphaState()			{ return m_AlphaState; }
	_float  Get_Alpha()					{ return m_fAlpha; }
	_float  Get_AlphaCreate()           { return m_fAlphaCreat; }
	_float  Get_AlphaEnd()				{ return m_fAlphaEnd; }
	
	void	Set_Effect(_bool effect)    { m_bEffect = effect; }
	void	Set_PosX(_float value)		{ m_fX = value; }
	void	Set_PosY(_float value)		{ m_fY = value; }
	void	Set_Pos(_float2 value)		{ m_fX = value.x; m_fY = value.y;}
	void	Set_SizeX(_float value)		{ m_fSizeX = value; }
	void	Set_SizeY(_float value)		{ m_fSizeY = value; }
	void	Set_Size(_float2 value)		{ m_fSizeX = value.x; m_fSizeY = value.y; }
	void	Set_Pos_Size(_float4 value)	{ m_fX = value.x; m_fY = value.y; m_fSizeX = value.z; m_fSizeY = value.w; }
	void	Set_Depth(_int value)		{ m_Depth = value; }
	void	Set_Picking(_bool picking)  { m_bIsPicking = picking; }
	void    Set_Name(const _tchar* name){ lstrcpy(m_pName,name); }
	void    Set_Texture(const _tchar* texturename) { lstrcpy(m_TextureName, texturename); }
	void    Set_PrototypeTag(const _tchar* tag) { lstrcpy(m_PrototypeTag, tag); }
	void	Set_Enum_Effect(_uint state) { m_State = state; }
	void    Set_Alpha(_float alpha) { m_fAlpha = alpha; }
	void	Set_Alpha_Effect(_uint state) { m_AlphaState = state; }

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Clone(void* pArg) override;
	virtual void Tick(const _double& _dTimeDelta)  override;
	virtual void Late_Tick(const _double& _dTimeDelta)  override;
	virtual HRESULT  Render() override;

public:
	_bool	InterSect   (_int mouseX, _int mouseY);
	void    Serialize   (HANDLE hFile, _int Currentlevel, CUI_Base* uiObject);

public:
	void	Start_Effect(_double timedelta);
		
	void	Grow_Combo(_float speed, _float maxsize, _float creat, _float dead, _double timedelta);
	void	Grow_UI		(_float speed, _float maxsize, _float creat, _float dead, _double timedelta);
	void	Grow_Decrease(_bool infinite, _float speed, _float maxsize, _float minsize, _float creat, _float dead, _double timedelta);
	void    Go_Right (_float movex,_float speed, _float creat, _float dead, _double timedelta);
	void    Go_Left(_float movex, _float speed, _float creat, _float dead, _double timedelta);
	void    Go_Down(_float movey, _float speed, _float creat, _float dead, _double timedelta);
	void    Repeat_LR(_float movex, _float speed,_double timedelta);
	void    L_Diagonal(_float4 possize, _float speed, _float creat, _float dead, _bool repeat, _double timedelta);
	
	void    Plus_Alpha(_float Alpha, _float AlphaCreat, _float Alphaend, _double timedelta);
	void    Minus_Alpha(_float Alpha, _float AlphaCreat, _float Alphaend, _double timedelta);
	void    Repeat_Alpha(_float Alpha, _float AlphaCreat,_float Delaytime, _float Alphaend, _double timedelta);
	
public:
	_bool Is_MyName(_tchar* nametag);
	_bool Is_MyTexture(_tchar* texturetag);
	
protected:
	
	//효과없는 기본 
	_int				m_Depth = 0; 
	_float				m_fX = 0.f, m_fY = 0.f, m_fSizeX = 0.f, m_fSizeY = 0.f;
	_bool			    m_bIsPicking = false;	
	_bool				m_bInterSect = false;
	_tchar*             m_pName = nullptr;
	_tchar*             m_TextureName = nullptr;
	_tchar*             m_PrototypeTag = nullptr;
	
	//효과 있는 
	_uint               m_State = UIEFFET_NONE;
	_bool				m_bEffect = true;
	_bool				m_bAlpha = true;
	_float				m_Speed = 0.f;
	_float			    m_MaxSize = 0.f;
	_float				m_MinSize = 0.f;
	_float				m_MoveX = 0.f;
	_float				m_MoveY = 0.f;
	_float              m_CreatTime = 0.f;
	_float              m_DeadTime = 0.f;
	_double             fDiagonaltime = 0.f;
	_bool 			    m_brepeat = false;
	
	_float ftime = 0.f;
	_float fAlphatime = 0.f;
	_bool retry = false;

	//알파값 컨트롤 
	_uint  m_AlphaState = ALPHA_NONE;
	_float m_fAlpha = 1.0;
	_float m_fAlphaCreat = 0.f;
	_float m_fAlphaDelay = 0.f;
	_float m_fAlphaEnd = 0.f;


	
public:
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END