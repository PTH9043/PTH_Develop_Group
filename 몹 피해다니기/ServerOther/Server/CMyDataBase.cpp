#include "Defines.h"
#include "CMyDataBase.h"

CMyDataBase::~CMyDataBase()
{    // 핸들 해제
    SQLDisconnect(m_ConnectHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, m_ConnectHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, m_OpenHandle);
}

HRESULT CMyDataBase::ReadyDataBase()
{
    // ODBC 함수 리턴 코드
    SQLRETURN                   ResCode{ 0 };

    // ODBC 환경 핸들 생성
    ResCode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_OpenHandle);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, E_FAIL);

    // ODBC 버전 설정
    ResCode = SQLSetEnvAttr(m_OpenHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, E_FAIL);

    // 연결 핸들 생성
    ResCode = SQLAllocHandle(SQL_HANDLE_DBC, m_OpenHandle, &m_ConnectHandle);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, E_FAIL);

    // DataBase 연결 정보 생성
    SQLWCHAR* ConnectString = (SQLWCHAR*)L"DRIVER={SQL Server};SERVER=DESKTOP-QNJ1JTG;DATABASE=2018184040PTH;UID=;PWD=";
    ResCode = SQLDriverConnect(m_ConnectHandle, NULL, ConnectString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, E_FAIL);
   
    return S_OK;
}


void CMyDataBase::InsertProcedureData(const ID& _iID, const char* _pName, const int _x, const int _y, const CHARDATA& _tCharData)
{
    FreeOrderHandle();

    SQLRETURN                   ResCode{ 0 };
    // 프로시저 호출
    SQLWCHAR sql_query[1024] = L"EXEC dbo.InsertCharacterData  @ID=?, @Name=?, @Level=?, @Job=?, @hp=?, @mp=?, @exp=?, @atk=?, @speed=?, @luck=?, @x=?, @y=?";
    ResCode = SQLAllocHandle(SQL_HANDLE_STMT, m_ConnectHandle, &m_OrderHandle);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, ;);
    ResCode = SQLPrepare(m_OrderHandle, sql_query, SQL_NTS);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, ;);

    UpdateSqlData(_iID, _pName, _x, _y, _tCharData);

    ResCode |= SQLBindParameter(m_OrderHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.ID, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_WVARCHAR,
        sizeof(m_tSqlData.Name), 0, m_tSqlData.Name, sizeof(m_tSqlData.Name), NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.Level, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.Job, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.hp, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.mp, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 7, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.exp, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 8, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.atk, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 9, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.speed, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 10, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.luck, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 11, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.x, 0, NULL);
    ResCode |= SQLBindParameter(m_OrderHandle, 12, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.y, 0, NULL);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, ;);
    ResCode = SQLExecute(m_OrderHandle);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, ;);
}

void CMyDataBase::DeleteProcedureData(const ID& _iID)
{
    FreeOrderHandle();

    SQLRETURN    ResCode{ 0 };
    // 프로시저 호출
    SQLWCHAR sql_query[1024] = L"EXEC dbo.DeleteCharacter @ID=?";
    ResCode = SQLAllocHandle(SQL_HANDLE_STMT, m_ConnectHandle, &m_OrderHandle);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, ;);
    ResCode = SQLPrepare(m_OrderHandle, sql_query, SQL_NTS);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, ;);

    SQLINTEGER ID = _iID;
    ResCode = SQLBindParameter(m_OrderHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ID, 0, NULL);
    ResCode = SQLExecDirect(m_OrderHandle, sql_query, SQL_NTS);
    RETURN_CHECK(ResCode != SQL_SUCCESS && ResCode != SQL_SUCCESS_WITH_INFO, ;);
}

void CMyDataBase::UpdateProcedureData(const ID& _iID, const char* _pName, const int _x, const int _y, const CHARDATA& _tCharData)
{
    m_DataBaseLock.lock();
    FreeOrderHandle();
    
    SQLRETURN                   ResCode{ 0 };
    SQLWCHAR sql_query[1024] = L"EXEC UpdateCharacter @ID=?, @Name=?, @Level=?, @Job=?, @hp=?, @mp=?, @exp=?, @atk=?, @speed=?, @luck=?, @x=?, @y=?";
    ResCode = SQLAllocHandle(SQL_HANDLE_STMT, m_ConnectHandle, &m_OrderHandle);
    if (ResCode != SQL_SUCCESS) { m_DataBaseLock.unlock(); return; }
    ResCode = SQLPrepare(m_OrderHandle, sql_query, SQL_NTS);
    if (ResCode != SQL_SUCCESS) { m_DataBaseLock.unlock(); return; }

    UpdateSqlData(_iID, _pName, _x, _y, _tCharData);

    ResCode = SQLBindParameter(m_OrderHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.ID, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_WVARCHAR,
        sizeof(m_tSqlData.Name), 0, m_tSqlData.Name, sizeof(m_tSqlData.Name), NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.Level, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.Job, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 5, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.hp, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.mp, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 7, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.exp, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 8, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.atk, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 9, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.speed, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 10, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.luck, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 11, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.x, 0, NULL);
    ResCode = SQLBindParameter(m_OrderHandle, 12, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &m_tSqlData.y, 0, NULL);

    // Execute the statement
    ResCode = SQLExecute(m_OrderHandle);

    m_DataBaseLock.unlock();
}

void CMyDataBase::GetProcedureData(const ID& _iID, SQLDATA& _tData)
{
    IsFindData(_iID, _tData);
}

bool CMyDataBase::IsFindData(const ID& _iID, SQLDATA& _tData)
{
    if (m_LoginList.count(_iID))
        return false;

    SQLINTEGER id = static_cast<SQLINTEGER>(_iID);
    SQLRETURN                   ResCode{ 0 };
    SQLWCHAR sql_query[1024] = L"EXEC GetCharacter @ID=?";

    m_DataBaseLock.lock();

    FreeOrderHandle();

    ResCode = SQLAllocHandle(SQL_HANDLE_STMT, m_ConnectHandle, &m_OrderHandle);
    if (ResCode != SQL_SUCCESS) { m_DataBaseLock.unlock(); return false; }

    ResCode = SQLPrepare(m_OrderHandle, sql_query, SQL_NTS);
    if (ResCode != SQL_SUCCESS) { m_DataBaseLock.unlock(); return false; }

    ResCode = SQLBindParameter(m_OrderHandle, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &id, 0, NULL);
    if (ResCode != SQL_SUCCESS) { m_DataBaseLock.unlock(); return false; }

    // Execute the statement
    ResCode = SQLExecute(m_OrderHandle);
    if (ResCode != SQL_SUCCESS) { m_DataBaseLock.unlock(); return false; }

    if (ResCode == SQL_SUCCESS || ResCode == SQL_SUCCESS_WITH_INFO) {
        // Process the retrieved data
        ResCode = SQLBindCol(m_OrderHandle, 1, SQL_C_LONG, &m_tSqlData.ID, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 2, SQL_C_CHAR, m_tSqlData.Name, sizeof(m_tSqlData.Name), NULL);
        ResCode = SQLBindCol(m_OrderHandle, 3, SQL_C_LONG, &m_tSqlData.Level, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 4, SQL_C_LONG, &m_tSqlData.Job, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 5, SQL_C_LONG, &m_tSqlData.hp, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 6, SQL_C_LONG, &m_tSqlData.mp, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 7, SQL_C_LONG, &m_tSqlData.exp, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 8, SQL_C_LONG, &m_tSqlData.atk, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 9, SQL_C_LONG, &m_tSqlData.speed, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 10, SQL_C_LONG, &m_tSqlData.luck, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 11, SQL_C_LONG, &m_tSqlData.x, 0, NULL);
        ResCode = SQLBindCol(m_OrderHandle, 12, SQL_C_LONG, &m_tSqlData.y, 0, NULL);

        // Fetch the result
        ResCode = SQLFetch(m_OrderHandle);
        if (ResCode == SQL_SUCCESS) {
            _tData = m_tSqlData;
            m_LoginList.insert(_iID);
            m_DataBaseLock.unlock();
            return true;
        }
    }

    m_DataBaseLock.unlock();
    return false;
}

void CMyDataBase::EraseLoginData(const ID& _iID)
{
    {
        lock_guard<mutex> Lock{m_DataBaseLock};
        m_LoginList.unsafe_erase(_iID);
    }
}

void CMyDataBase::DeleteAllData()
{
    FreeOrderHandle();
    SQLRETURN                   ResCode{ 0 };
    SQLWCHAR sql_query[1024] = L"EXEC dbo.DeleteAllCharacterData";

    ResCode = SQLAllocHandle(SQL_HANDLE_STMT, m_ConnectHandle, &m_OrderHandle);
    RETURN_CHECK(ResCode != SQL_SUCCESS, ;);

    ResCode = SQLPrepare(m_OrderHandle, sql_query, SQL_NTS);
    RETURN_CHECK(ResCode != SQL_SUCCESS, ;);
    // Execute the statement
    ResCode = SQLExecute(m_OrderHandle);
    RETURN_CHECK(ResCode != SQL_SUCCESS, ;);
}

void CMyDataBase::UpdateSqlData(const ID& _iID, const char* _pName, const int _x, const int _y, const CHARDATA& _tCharData)
{
    m_tSqlData.ID = _iID;
    ::memcpy(m_tSqlData.Name, _pName, (size_t)BUF_SIZE);
    m_tSqlData.Level = static_cast<SQLINTEGER>(_tCharData.level);
    m_tSqlData.Job = static_cast<SQLINTEGER>(_tCharData.job);
    m_tSqlData.hp = static_cast<SQLINTEGER>(_tCharData.max_hp);
    m_tSqlData.mp = static_cast<SQLINTEGER>(_tCharData.max_mp);
    m_tSqlData.atk = static_cast<SQLINTEGER>(_tCharData.atk);
    m_tSqlData.exp = static_cast<SQLINTEGER>(_tCharData.max_exp);
    m_tSqlData.speed = static_cast<SQLINTEGER>(_tCharData.speed);
    m_tSqlData.luck = static_cast<SQLINTEGER>(_tCharData.luck);
    m_tSqlData.x = static_cast<SQLINTEGER>(_x);
    m_tSqlData.y = static_cast<SQLINTEGER>(_y);
}

void CMyDataBase::ShowError(const _uint _handleType, const SQLHANDLE& _handle)
{
    SQLWCHAR sql_state[1024];
    SQLWCHAR message[1024];
    if (SQL_SUCCESS == SQLGetDiagRec(_handleType, _handle, 1, sql_state, NULL, message, 1024, NULL)){
        MessageBoxW(nullptr, message, L"MB_OK", 0);
    }
}

void CMyDataBase::FreeOrderHandle()
{
    SQLFreeHandle(SQL_HANDLE_STMT, m_OrderHandle);
    m_OrderHandle = NULL;
}


