#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <string>

#include "Defines.h"
#include "Table.h"
#include "Column.h"
#include "Row.h"
#include "Version.h"
#include "Log.h"
#include "TimeoutThread.h"
#include "TimerThread.h"

using namespace std;

int gUseSwap;
Log gLog;
map<string, Table*> Table::mTableMap;

int test_std()
{
	Table *tab1, *tab2;
	ColumnInfo colinfo;
	Row row;
	char value1[100];

	// create table1
	colinfo.Clear();
	colinfo.AddColumn("col1",1,1);
	colinfo.AddColumn("col2",2,2);
	colinfo.AddColumn("col3",4,5);
	tab1 = Table::CreateTable("table1", COLUMN_TYPE_MULTI );
	tab1->SetColumn( &colinfo );
	colinfo.Clear();

	row.AddVal("col1", "1");
	row.AddVal("col2", "12");
	row.AddVal("col3", "432");
	tab1->AddRow( "key1", &row );
	row.Clear();
	row.AddVal("col1", "2");
	row.AddVal("col2", "23");
	row.AddVal("col3", "234");
	tab1->AddRow( "key2", &row );

	// create table2
	colinfo.AddColumn("col2-1",1,2);
	colinfo.AddColumn("col2-2",4,5);
	tab2 = Table::CreateTable("table2", COLUMN_TYPE_MULTI);
	tab2->SetColumn( &colinfo);
	colinfo.Clear();

	row.Clear();
	row.AddVal("col2-1", "1");
	row.AddVal("col2-2", "45");
	tab2->AddRow( "key3", &row );
	row.Clear();
	row.AddVal("col2-1", "2");
	row.AddVal("col2-2", "56");
	tab2->AddRow( "key4", &row );

	Table::Dump();

	// get row
	tab1->GetRow( "key1", "col1", value1 );
	assert( !strcmp( value1, "1" ) );
	tab1->GetRow( "key1", "col2", value1 );
	assert( !strcmp( value1, "12" ) );
	tab1->GetRow( "key1", "col3", value1 );
	assert( !strcmp( value1, "432" ) );
	tab1->GetRow( "key2", "col2", value1 );
	assert( !strcmp( value1, "23" ) );
	tab1->GetRow( "key2", "col3", value1 );
	assert( !strcmp( value1, "234" ) );
	tab2->GetRow( "key3", "col2-2", value1 );
	assert( !strcmp( value1, "45" ) );
	tab2->GetRow( "key4", "col2-2", value1 );
	assert( !strcmp( value1, "56" ) );
	
	// update row
	tab1->UpdateRow( "key1", "col3", "456" );
	tab1->GetRow( "key1", "col3", value1 );
	assert( !strcmp( value1, "456" ) );

	// del row
	tab1->DelRow( "key2" );

	Table::Dump();

	Table::DeleteTable("table1");
	Table::DeleteTable("table2");
	return 0;
}

int main() 
{
	int ret;
	printf("Version : %s\n", BMDB_VERSION);

	gLog.init( "log", "test", Log::REOPEN_DD, Log::LEVEL_TRACE );
	gLog.log( "== Start test program! ==" );

	TimeoutThread::CreateInstance();
	TimerThread::CreateInstance();

	//while( true )
	{
		test_std();
		usleep(1000);
	}

	TimerThread::Destroy();
	TimeoutThread::Destroy();

	return 0;
}

