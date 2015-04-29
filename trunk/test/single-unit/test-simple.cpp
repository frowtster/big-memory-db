#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <string>

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

	// create table1
	tab1 = Table::CreateTable("table1", 10);

	tab1->AddRow( "key1", "value1" );
	tab1->AddRow( "key2", "value2" );

	// create table2
	tab2 = Table::CreateTable("table2", 20);

	tab2->AddRow( "key3", "value3" );
	tab2->AddRow( "key4", "value4" );

	Table::Dump();

	// get row
	assert( !strcmp(tab1->GetRow( "key1" ), "value1" ) );
	assert( !strcmp(tab1->GetRow( "key2" ), "value2" ) );
	assert( !strcmp(tab2->GetRow( "key3" ), "value3" ) );
	assert( !strcmp(tab2->GetRow( "key4" ), "value4" ) );
	
	// update row
	tab1->UpdateRow( "key2", "value5" );
	assert( !strcmp(tab1->GetRow( "key2" ), "value5" ) );

	// del row
	tab1->DelRow( "key3" );

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

