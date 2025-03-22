#include "CoreDefines.h"
#include "ASqlLoginTable.h"
#include "AMySqlConnector.h"


BEGIN(Core)

const _char* ASqlLoginTable::INSERT_METHOD_NAME{ "InsertLoginTable"};
const _char* ASqlLoginTable::UPDATE_METHOD_NAME{ "UpdateLoginTable" };

ASqlLoginTable::ASqlLoginTable() :  AMySqlTable(SQLTABLETYPE::LOGIN)
{
}

_bool ASqlLoginTable::ExcuteQueryMessage(SHPTR<AMySqlConnector> _spMySqlConnector, 
	SQLQUERYTYPE _sqlQueryType, const _string& _strQueryData)
{
	switch (_sqlQueryType)
	{
	case SQLQUERYTYPE::INSERT:
	{
		// Insert Procedure 함수 실행
		std::unique_ptr<sql::PreparedStatement> Procedure(AQueryFunc::ExcuteProcedureFunc(
			_spMySqlConnector, INSERT_METHOD_NAME, METHOD_NUM));
		UpdateProcedureValue(_spMySqlConnector, std::move(Procedure));
	}
		break;
	case SQLQUERYTYPE::UPDATE:
		// Update
		std::unique_ptr<sql::PreparedStatement> Procedure(AQueryFunc::ExcuteProcedureFunc(
			_spMySqlConnector, UPDATE_METHOD_NAME, METHOD_NUM));
		UpdateProcedureValue(_spMySqlConnector, std::move(Procedure));
		break;
	}

	
	return false;;
}

_bool ASqlLoginTable::CreateTable(sql::Statement* _pStmt)
{
	assert(nullptr != _pStmt);

	_string CheckExistDatabaseQuery = CombineRawString("SHOW TABLES LIKE'", SQL_LOGINTABLE_NAME) + "'";
	std::unique_ptr<sql::ResultSet> res(_pStmt->executeQuery(CheckExistDatabaseQuery.c_str()));
	assert(res != nullptr);
	if (false == res->next())
	{
		_string CreateTableQuery = "CREATE TABLE ";
		CreateTableQuery += SQL_LOGINTABLE_NAME;  
		CreateTableQuery += " (id INT AUTO_INCREMENT PRIMARY KEY, "
												  "username VARCHAR(255) NOT NULL, "
												   "password VARCHAR(255) NOT NULL)";

		std::unique_ptr<sql::ResultSet> MakeDB(_pStmt->executeQuery(CreateTableQuery.c_str()));
		assert(MakeDB != nullptr);
		return true; 
	}
	return false;
}

void ASqlLoginTable::CheckAndCreateProcedure(sql::Statement* _pStmt)
{
	assert(true == AQueryFunc::MakeProcedureFunc(_pStmt, INSERT_METHOD_NAME,
		"CREATE PROCEDURE InsertUserWithID(IN _id INT, IN _username VARCHAR(255), IN _password VARCHAR(255)) "
		"BEGIN "
		"INSERT INTO users (id, username, password) VALUES (_id, _username, _password); "
		"END;"));
}

void ASqlLoginTable::Free()
{
}


END