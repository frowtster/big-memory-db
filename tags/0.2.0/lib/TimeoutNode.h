#pragma once

#include <time.h>
#include "TableContainer.h"
#include "Constants.h"

struct TimeoutNode {
	TableContainer *mTable;
	char mKey[COLUMN_NAME_SIZE+1];
	time_t mTime;
};

