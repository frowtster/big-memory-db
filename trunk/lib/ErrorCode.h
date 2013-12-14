
#pragma once

#define ERROR_OK				0		// success

// system error
#define ERROR_MEMALOCK_FAIL		101		// memory alloc fail
#define ERROR_INVALID_COMMAND	102		// invalid command

// table error
#define ERROR_NOT_SELECT_TABLE	201		// no current table. select table first.
#define ERROR_TABLE_NOT_FOUND	202		// table not found
#define ERROR_TABLE_DUP			203		// table duplicate
#define ERROR_TABLE_EXIST		204		// table exist

// row error
#define ERROR_ROW_NOT_FOUND		301		// empty row
#define ERROR_KEY_NOT_FOUND		302		// empty key
#define ERROR_COLUMN_NOT_FOUND	303		// column not found

// auth error
