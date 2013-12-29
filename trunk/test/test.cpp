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

using namespace std;

int gUseSwap;
Log gLog;
map<string, Table*> Table::mTableMap;

int test_std()
{
	Table *tab1, *tab2;
	ColumnInfo colinfo;
	Row row;

	// create table1
	colinfo.Clear();
	colinfo.AddColumn("col1",1,1);
	colinfo.AddColumn("col2",2,2);
	colinfo.AddColumn("col3",4,5);
	tab1 = Table::CreateTable("table1", &colinfo);
	colinfo.Clear();

	row.AddVal("col1", "1");
	row.AddVal("col2", "12");
	row.AddVal("col3", "432");
	tab1->AddRow( &row );
	row.Clear();
	row.AddVal("col1", "2");
	row.AddVal("col2", "23");
	row.AddVal("col3", "234");
	tab1->AddRow( &row );

	// create table2
	colinfo.AddColumn("col2-1",1,2);
	colinfo.AddColumn("col2-2",4,5);
	tab2 = Table::CreateTable("table2", &colinfo);
	colinfo.Clear();

	row.Clear();
	row.AddVal("col2-1", "1");
	row.AddVal("col2-2", "45");
	tab2->AddRow( &row );
	row.Clear();
	row.AddVal("col2-1", "2");
	row.AddVal("col2-2", "56");
	tab2->AddRow( &row );

	Table::Dump();

	// get row
	assert( !strcmp(tab1->GetRow( "1", "col1" ), "1" ) );
	assert( !strcmp(tab1->GetRow( "1", "col2" ), "12" ) );
	assert( !strcmp(tab1->GetRow( "1", "col3" ), "432" ) );
	assert( !strcmp(tab1->GetRow( "2", "col2" ), "23" ) );
	assert( !strcmp(tab1->GetRow( "2", "col3" ), "234" ) );
	assert( !strcmp(tab2->GetRow( "1", "col2-2" ), "45" ) );
	assert( !strcmp(tab2->GetRow( "2", "col2-2" ), "56" ) );
	
	// update row
	tab1->UpdateRow( "1", "col3", "456" );
	assert( !strcmp(tab1->GetRow( "1", "col3" ), "456" ) );

	// del row
	tab1->DelRow( "2" );

	Table::Dump();

	Table::DeleteTable("table1");
	Table::DeleteTable("table2");
	return 0;
}

int test_timeout()
{
	Table *tab1;
	ColumnInfo colinfo;
	Row row;

	// create table1
	colinfo.AddColumn("col1",1,1);
	colinfo.AddColumn("col2",2,2);
	colinfo.AddColumn("col3",4,5);
	tab1 = Table::CreateTable("table1", &colinfo);
	colinfo.Clear();

	row.AddVal("col1", "1");
	row.AddVal("col2", "12");
	row.AddVal("col3", "432");
	tab1->AddRow( &row, 10 );
	row.Clear();
	Table::Dump();
	assert( !strcmp(tab1->GetRow( "1", "col2" ), "12" ) );
	sleep(11);
	assert( tab1->GetRow( "1", "col2" ) == NULL );
	Table::DeleteTable("table1");
	return 0;
}

int test_backup()
{
	Table *tab1;
	ColumnInfo colinfo;
	Row row;
	char filename[256];

	// create table1
	colinfo.AddColumn("col1",1,1);
	colinfo.AddColumn("col2",2,2);
	colinfo.AddColumn("col3",4,5);
	tab1 = Table::CreateTable("table1", &colinfo);
	colinfo.Clear();

	row.AddVal("col1", "1");
	row.AddVal("col2", "12");
	row.AddVal("col3", "432");
	tab1->AddRow( &row );
	row.Clear();
	row.AddVal("col1", "2");
	row.AddVal("col2", "23");
	row.AddVal("col3", "234");
	tab1->AddRow( &row, 10 );
	row.Clear();
	row.AddVal("col1", "3");
	row.AddVal("col2", "34");
	row.AddVal("col3", "456");
	tab1->AddRow( &row, 20 );

	Table::Backup("table1", filename);
	gLog.log( "filename %s", filename );

	Table::DeleteTable("table1");
	tab1 = Table::GetTable("table1");
	assert( tab1 == NULL );

	sleep(11);
	Table::Restore( filename );

	tab1 = Table::GetTable("table1");
	assert( tab1 != NULL );
	assert( !strcmp(tab1->GetRow( "1", "col2" ), "12" ) );
	assert( !strcmp(tab1->GetRow( "1", "col3" ), "432" ) );
	assert( tab1->GetRow( "2", "col2" ) == NULL );
	assert( tab1->GetRow( "2", "col3" ) == NULL );
	assert( !strcmp(tab1->GetRow( "3", "col2" ), "34" ) );
	assert( !strcmp(tab1->GetRow( "3", "col3" ), "456" ) );

	Table::DeleteTable("table1");
	return 0;
}

int main() 
{
	printf("Version : %s\n", BMDB_VERSION);

	gLog.init( "log", "test", Log::REOPEN_DD, Log::LEVEL_TRACE );

	TimeoutThread::CreateInstance();

	test_std();
	test_timeout();
	test_backup();

	return 0;
}

