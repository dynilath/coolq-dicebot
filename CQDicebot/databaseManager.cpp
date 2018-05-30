#include "stdafx.h"
#include "databaseManager.h"
#include "base64.h"
#include "QTool.h"
#include <iostream>

const char * cstr_file_db = APP_DIR "com.dynilath.coolqdicebot.nickname.db";

databaseManager * databaseManager::instance = nullptr;


sqlite3 * databaseManager::getDatabase()
{
	return (databaseManager::instance)->database;
}

databaseManager * databaseManager::getInstance()
{
	return databaseManager::instance;
}

databaseManager::databaseManager()
{
	isDatabaseReady = false;
	int i_ret_code = sqlite3_open(cstr_file_db, &database);
	if (i_ret_code == SQLITE_OK) {
		if (databaseManager::instance != nullptr) {
			free(databaseManager::instance);
			databaseManager::instance = this;
		}
		databaseManager::instance = this;
		isDatabaseReady = true;
	}
}

databaseManager::~databaseManager()
{
	if (databaseManager::instance == this) databaseManager::instance = nullptr;
	sqlite3_close(this->database);
}

int databaseManager::registerTable(std::string str_table_name, std::string str_table_sql)
{
	bool isExist;
	int i_ret_code = isTableExist(str_table_name, isExist);
	if (i_ret_code == SQLITE_OK && !isExist) {
		const char * sql_command = str_table_sql.c_str();
		char * pchar_err_message = nullptr;
		int i_ret_code_2 = sqlite3_exec(this->database, sql_command, &sqlite3_callback, (void*)&i_data_database_create, &pchar_err_message);
		if (i_ret_code_2 != SQLITE_OK)
		{
#ifdef _DEBUG
			std::cout << pchar_err_message;
#endif // _DEBUG
			isDatabaseReady = true;
			return i_ret_code_2;
		}
		else {
			isDatabaseReady = false;
			return i_ret_code_2;
		}
	}
	else return i_ret_code;
}

inline int databaseManager::isTableExist(const std::string & table_name , bool & isExist)
{
	std::string sql_command = "select count(*) from sqlite_master where type ='table' and name ='" + table_name + "'";
	char * pchar_err_message = nullptr;
	int i_count_of_table = 0;
	int ret_code = sqlite3_exec(this->database, sql_command.c_str(), &sqlite3_callback_isTableExist, (void*)&i_count_of_table, &pchar_err_message);
	if (ret_code == SQLITE_OK) {
		isExist = i_count_of_table > 0;
		return ret_code;
	}
#ifdef _DEBUG
	else {
		std::cout << sqlite3_errmsg(database);
	}
#endif
	isExist = false;
	return ret_code;
}

int databaseManager::sqlite3_callback(void * data, int argc, char ** argv, char ** azColName)
{
	int i_database_op = *((int*)data);
	switch (i_database_op)
	{
	case i_data_database_create:
		return SQLITE_OK;
	case i_data_database_update:
		return SQLITE_OK;
	default:
		return SQLITE_ABORT;
	}
}

int databaseManager::sqlite3_callback_isTableExist(void * data, int argc, char ** argv, char ** azColName)
{
	int * i_handle = (int*)data;
	if (argc == 1) {
		*i_handle = atoi(argv[0]);
	}
	return SQLITE_OK;
}
