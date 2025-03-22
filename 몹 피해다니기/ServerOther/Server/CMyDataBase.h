#pragma once
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>

enum DBEVENT {
    DBEVENT_INSERT_DATA, DBEVNET_CHANGE_DATA, 
    DBEVENT_GETDATA
};

struct SQLDATA {
    // 결과 출력
    SQLINTEGER ID;
    SQLCHAR Name[BUF_SIZE];
    SQLINTEGER Level;
    SQLINTEGER Job;
    SQLINTEGER     hp;
    SQLINTEGER    mp;
    SQLINTEGER    exp;
    SQLINTEGER    atk;
    SQLINTEGER    speed;
    SQLINTEGER    luck;
    SQLINTEGER x;
    SQLINTEGER y;
};

class CMyDataBase  {
private:
    // Handle
	SQLHANDLE                   m_OpenHandle{ NULL };
	SQLHANDLE                   m_ConnectHandle{ NULL };
    SQLHANDLE                   m_OrderHandle{ NULL };
    // SQL Data
    SQLDATA                         m_tSqlData{};

    const SQLWCHAR*       m_pDataBaseName{ L"CharacterData" };

    concurrency::concurrent_unordered_set<int> m_LoginList{};
    mutex                               m_DataBaseLock{};
public:
    CMyDataBase() = default;
    ~CMyDataBase();
public:
    // Ready DataBase
    HRESULT ReadyDataBase();

    void InsertProcedureData(const ID& _iID, const char* _pName, const int _x, const int _y, const CHARDATA& _tCharData);
    void DeleteProcedureData(const ID& _iID);
    void UpdateProcedureData(const ID& _iID, const char* _pName, const int _x, const int _y, const CHARDATA& _tCharData);
    void GetProcedureData(const ID& _iID, SQLDATA& _tData);
    bool IsFindData(const ID& _iID, SQLDATA& _tData);
    void EraseLoginData(const ID& _iID);
    void DeleteAllData();
private:
    void UpdateSqlData(const ID& _iID, const char* _pName, const int _x, const int _y, const CHARDATA& _tCharData);

    void ShowError(const _uint _handleType, const SQLHANDLE& _handle);
    void FreeOrderHandle();
};
