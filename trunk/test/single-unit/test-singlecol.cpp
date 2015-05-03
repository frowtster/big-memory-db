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
	Table *tab1, *tab2, *tab3;
	char value1[100];
	long value2;

	// create table1
	tab1 = Table::CreateTable("table1", COLUMN_TYPE_SINGLE );
	tab1->SetColumn( VALUE_TYPE_STRING, 10 );

	tab1->AddRow( "key1", "value1" );
	tab1->AddRow( "key2", "value2" );

	// create table2
	tab2 = Table::CreateTable("table2", COLUMN_TYPE_SINGLE );
	tab2->SetColumn( VALUE_TYPE_STRING, 20 );

	tab2->AddRow( "key3", "value3" );
	tab2->AddRow( "key4", "value4" );

	// create table3
	tab3 = Table::CreateTable("table3", COLUMN_TYPE_SINGLE );
	tab3->SetColumn( VALUE_TYPE_NUMBER, 10 );

	tab3->AddRow( "key5", 123 );
	tab3->AddRow( "key6", 234 );

	Table::Dump();

	// get row
	tab1->GetRow( "key1", value1 );
	assert( !strcmp( value1, "value1" ) );
	tab1->GetRow( "key2", value1 );
	assert( !strcmp( value1, "value2" ) );
	tab2->GetRow( "key3", value1 );
	assert( !strcmp( value1, "value3" ) );
	tab2->GetRow( "key4", value1 );
	assert( !strcmp( value1, "value4" ) );
	tab3->GetRow( "key5", &value2 );
	assert( value2 == 123 );
	tab3->GetRow( "key6", &value2 );
	assert( value2 == 234 );
	
	// update row
	tab1->UpdateRow( "key2", "value5" );
	tab1->GetRow( "key2", value1 );
	assert( !strcmp( value1, "value5" ) );

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

