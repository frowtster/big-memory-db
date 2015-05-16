
#pragma once

#define ERROR_OK				0		// success

// system error
#define ERROR_MEMALOCK_FAIL		101		// memory alloc fail
#define ERROR_INVALID_COMMAND	102		// invalid command
#define ERROR_FILE_NOT_CREATE	103		// fopen error
#define ERROR_FILE_NOT_OPEN		104		// fopen error
#define ERROR_FILE_NOT_READ		105		// fread error
#define ERROR_FILE_NOT_WRITE	106		// fwrite error
#define ERROR_FILE_FORMAT		107		// format error

// table error
#define ERROR_NOT_SELECT_TABLE	201		// no current table. select table first.
#define ERROR_TABLE_NOT_FOUND	202		// table not found
#define ERROR_TABLE_DUP			203		// table duplicate
#define ERROR_TABLE_EXIST		204		// table exist
#define ERROR_TABLE_TYPE		205		// table type error. multi/single column

// row error
#define ERROR_ROW_NOT_FOUND		301		// empty row
#define ERROR_KEY_NOT_FOUND		302		// empty key
#define ERROR_COLUMN_NOT_FOUND	303		// column not found

// value error
#define ERROR_VALUE_TYPE		401     // value type mismatch
#define ERROR_VALUE_EXIST		402     // value exist

// system error
#define ERROR_MEMORY_LACK		501		// memory shotage

// auth error
