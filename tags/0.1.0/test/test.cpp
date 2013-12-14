#include <stdio.h>
#include <assert.h>
#include <string>

#include "Table.h"
#include "Column.h"
#include "Row.h"
#include "Version.h"
#include "Log.h"

using namespace std;

int gUseSwap;
Log gLog;
map<string, Table*> Table::mTableMap;

int main() 
{
	printf("Version : %s\n", BMDB_VERSION);

	gLog.init( "log", "test", Log::REOPEN_DD, Log::LEVEL_TRACE );
	Table *tab1, *tab2;
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

	return 0;
}

