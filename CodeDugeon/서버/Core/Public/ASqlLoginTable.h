#ifndef _SERVERFRAMEWORK_CORE_PUBLIC_ASQLLOGINTABLE_H
#define _SERVERFRAMEWORK_CORE_PUBLIC_ASQLLOGINTABLE_H

#include "AMySqlTable.h"

BEGIN(Core)

class ASqlLoginTable final : public AMySqlTable {
public:
	ASqlLoginTable();
	DESTRUCTOR(ASqlLoginTable)
public:
	// ExcuteQueryMessage
	virtual _bool ExcuteQueryMessage(SHPTR<AMySqlConnector> _spMySqlConnector, 
		SQLQUERYTYPE _sqlQueryType, const _string& _strQueryData) override;
protected:
	// Create Table
	virtual _bool CreateTable(sql::Statement* _pStmt) override;
	virtual void CheckAndCreateProcedure(sql::Statement* _pStmt) override;
private:
	virtual void Free() override;

private:
	static constexpr _int METHOD_NUM{ 3 };
	static const _char* INSERT_METHOD_NAME;
	static const _char* UPDATE_METHOD_NAME;
};

END

#endif // _SERVERFRAMEWORK_CORE_PUBLIC_ASQLLOGINTABLE_H