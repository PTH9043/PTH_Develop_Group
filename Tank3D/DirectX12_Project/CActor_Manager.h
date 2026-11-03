#pragma once
#include "CBase.h"

class CActor;
class CActorGroup;

class CActor_Manager : public CBase
{
public:
	typedef unordered_map<_wstring, shared_ptr<CActor>>					PROTOTYPES;
	typedef unordered_map<_wstring, shared_ptr<CActorGroup>>		CLONES;
protected:
	explicit CActor_Manager() = default;
	virtual ~CActor_Manager() = default;

public:
	virtual void Free() override;
public:
	HRESULT NativeConstruct() { return S_OK; }
private:
	shared_ptr<CActor> Find_Actor(const _wstring& _strPrototypeNames);
public:
	// TIck Late Tick
	void Tick(const _double& _dTimeDelta);
	void Late_Tick(const _double& _dTimeDelta);
	// Add Prototype
	void Add_Prototypes(const _wstring& _strName, const shared_ptr<CActor>& _pActor);
	void Clone_Actor(const _wstring& _strProtoNames, const _wstring& _strCloneLayer, const VOIDDATAS& _tDatas ={});
	shared_ptr<CActor> Clone_Actor_Add(const _wstring& _strProtoNames, const _wstring& _strCloneLayer, const VOIDDATAS& _tDatas = {});
	void Add_DeleteActor(const _wstring& _strLayer, const shared_ptr<CActor>& _pActor);
	// Get Actor List
	list<shared_ptr<CActor>> Get_ActorList(const _wstring& _strCloneLayer);
	void Clear();
private:
	PROTOTYPES	m_Prototypes{};
	CLONES				m_Clones{};


};

