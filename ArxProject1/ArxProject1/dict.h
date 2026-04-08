#pragma once
#include "StdAfx.h" 


int createRecordInNOD(TCHAR * value, TCHAR * nodName);
AcStringArray listRecordsInNOD(TCHAR* nodName);
int removeRecordFromNOD(TCHAR* value, TCHAR* nodName);