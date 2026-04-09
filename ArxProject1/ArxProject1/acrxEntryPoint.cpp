#include "StdAfx.h"
#include "resource.h"
#include "ADSKEditorReactor.h"
#include "utilities.h"
#include "dict.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
ADSKEditorReactor* g_pReactor = nullptr;


//----- ObjectARX EntryPoint
class CArxProject1App : public AcRxArxApp {

public:
	CArxProject1App() : AcRxArxApp() {}

	virtual AcRx::AppRetCode On_kInitAppMsg(void* pkt) {
		AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg(pkt);
		//!!! your code here. not above "retCode"
		g_pReactor = new ADSKEditorReactor();
		g_pReactor->Attach();

		return (retCode);
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg(void* pkt) {
		//!!! your code here, not below "retCode"
		if (g_pReactor)
		{
			g_pReactor->Detach();
			delete g_pReactor;
			g_pReactor = nullptr;
		}

		AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg(pkt);
		return (retCode);
	}

	virtual void RegisterServerComponents() {
	}

	/// <summary>
	/// this command will ask the user to input an integer value 
	/// and then print that value back to the user in the AutoCAD command line.
	/// in case of wrong iniput, the command will print an error message and 
	/// will return the prompt for input again until the user enters a valid 
	/// integer or cancels the command.
	/// </summary>
	static void ADSKMyGroupLesson2_input() {
		int value;
		if (acedGetInt(_T("\nEnter an integer: "), &value) == RTNORM)
		{
			acutPrintf(_T("\nYou entered: %d"), value);
		}
	}

	/// <summary>
	/// this command will tell the user how many objects they have selected in the current selection set.
	/// </summary>
	static void ADSKMyGroupLesson2_select() {
		ads_name ss;
		if (acedSSGet(NULL, NULL, NULL, NULL, ss) == RTNORM)
		{
			Adesk::Int32 length = 0;
			acedSSLength(ss, &length);
			acutPrintf(_T("\nYou have selected %d objects"), length);
			acedSSFree(ss);
		}
	}

	/// <summary>
	/// this command will create a layer with the name "USER" 
	/// and a block record with the name "EMPLOYEE" in the current drawing.
	/// none of the block inserts are made, just the block record is created.
	/// to insert block "EMPLOYEE" into the drawing, you can use the AutoCAD command "INSERT"
	/// </summary>
	static void ADSKMyGroupLesson3_create() {
		AcDbObjectId layerId;
		if (createLayer(_T("USER"), layerId) != Acad::eOk)
		{
			acutPrintf(_T("\n Error:- Failed to create layer USER\n"));
			//return;
		}

		// This is not always needed, but a call to 'applyCurDwgLayerTableChanges()'
		// will synchronize the newly created layer table change with the 
		// rest of the current DWG database.

		//2. After the layer table changes, call the 
		//ObjectARX global function applyCurDwgLayerTableChanges().
		// This function will force AutoCAD to update itself 
		// to use any changes made to any layer table records 
		// in the drawing currently in the AutoCAD editor.
		//
		applyCurDwgLayerTableChanges();

		acutPrintf(_T("\nLayer USER successfully created.\n"));

		if (createBlockRecord(_T("EMPLOYEE")) != Acad::eOk)
			acutPrintf(_T("\nFailed to create block record EMPLOYEE\n"));
		else
			acutPrintf(_T("\nSuccessfully created block record EMPLOYEE\n"));
	}

	/// <summary>
	/// this command will change a layer of all inserted to the drawing blocks 
	/// with the name "EMPLOYEE" to the layer "USER".
	/// </summary>
	static void ADSKMyGroupLesson3_setlayer() {
		//1. Open the block table of the current working database
		Acad::ErrorStatus es;

		AcDbBlockTable* pBlockTable;

		es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
		if (es != Acad::eOk)
		{
			acutPrintf(_T("\nFailed to open the Block Table"));
			return;
		}

		//2. Get the MODEL SPACE block table record 
		//(AcDbBlockTable::getAt(), ACDB_MODEL_SPACE ) 
		AcDbBlockTableRecord* pBTRec;
		es = pBlockTable->getAt(ACDB_MODEL_SPACE, pBTRec, AcDb::kForWrite);
		if (es != Acad::eOk)
		{
			acutPrintf(_T("Failed to Get the MODEL SPACE block table record"));
			pBlockTable->close();
			return;
		}
		pBlockTable->close();

		// 3. Obtain a block table record iterator 
		// (AcDbBlockTableRecordIterator, AcDbBlockTableRecord::newIterator()) 
		// to iterate through the MODEL SPACE block table record. 

		AcDbBlockTableRecordIterator* pBTItr;
		if ((es = pBTRec->newIterator(pBTItr)) != Acad::eOk)
		{
			acutPrintf(_T("\nFailed to create block table record iterator"));
			pBTRec->close();
			return;
		}

		// 4. Iterate through the MODEL_SPACE 
		// (AcDbBlockTableRecordIterator::start(), 
		// AcDbBlockTableRecordIterator::done(), 
		// AcDbBlockTableRecordIterator::step()). 

		AcDbEntity* pEnt;
		for (pBTItr->start(); !pBTItr->done(); pBTItr->step())
		{
			//5. To open objects for read or write operations 
			// use acdbOpenObject().
			//6. Obtain the entity(AcDbBlockTableRecordIterator::getEntity()).

			if ((es = pBTItr->getEntity(pEnt, AcDb::kForRead)) != Acad::eOk)
			{
				acutPrintf(_T("\nFailed to open entity through iterator"));
				continue;
			}
			//7. Check if the entity is a block reference
			// (pEnt->isA() != AcDbBlockReference::desc()).
			if (pEnt->isA() != AcDbBlockReference::desc())
			{
				pEnt->close();
				continue;
			}

			//8. Obtain the block table record of the reference
			// (AcDbBlockReference::blockTableRecord()) and 
			// check if the block table record's name is "EMPLOYEE". 

			TCHAR* blockName;
			AcDbBlockTableRecord* pCurEntBlock;
			AcDbObjectId blockId = (AcDbBlockReference::cast(pEnt))->blockTableRecord();
			if (acdbOpenObject((AcDbObject*&)pCurEntBlock, blockId, AcDb::kForRead) == Acad::eOk)
			{
				pCurEntBlock->getName(blockName);
				if (_tcscmp(blockName, _T("EMPLOYEE")) == 0)
				{
					if (pEnt->upgradeOpen() == Acad::eOk)
					{
						//9. Change the layer(setLayer())

						pEnt->setLayer(_T("USER"));
					}
				}
				pCurEntBlock->close();
				acdbFree(blockName);
			}

			//10.Don't forget to close any objects you opened 
			//   and delete the iterator ! 
			pEnt->close();

		}
		delete pBTItr;
		pBTRec->close();
	}

	/// <summary>
	/// this command will add a new entry in the "ADSK_EMPLOYEE_DICTIONARY" dictionary 
	/// in the current drawing's Named Objects Dictionary (NOD).
	/// </summary>
	static void ADSKMyGroupLesson4_addentry() {
		TCHAR empName[100];
		if (acedGetString(0, _T("Enter Employee name: "), empName) != RTNORM)
		{
			acutPrintf(_T("Failed to get emp name. Aborted!"));
			return;
		}

		switch (createRecordInNOD(empName, _T("ADSK_EMPLOYEE_DICTIONARY")))
		{
		case 1:
			acutPrintf(_T("Failed to open NOD. Aborted!"));
			break;
		case 2:
			acutPrintf(_T("\nCannot open NOD for Write!"));
			break;
		case 3:
			acutPrintf(_T("\nCannot add our dictionary in the AutoCAD NOD!"));
			break;
		case 4:
			acutPrintf(_T("\nCannot open the Emp object for write."));
			break;
		case 5:
			acutPrintf(_T("\nEntry found in the NOD, but it is not a dictionary."));
			break;
		case 6:
			acutPrintf(_T("\nThis employee is already registered."));
			break;
		case 7:
			acutPrintf(_T("\nFailed to add the new employee in the dictionary."));
			break;
		default:
			acutPrintf(_T("\nEmployee Record Added!"));
			break;
		}
	}

	/// <summary>
	/// this command will list all the entries in the "ADSK_EMPLOYEE_DICTIONARY" dictionary 
	/// in the current drawing's Named Objects Dictionary (NOD).
	/// </summary>
	static void ADSKMyGroupLesson4_list() {
		AcStringArray res = listRecordsInNOD(_T("ADSK_EMPLOYEE_DICTIONARY"));
		for (int i = 0; i < res.length(); i++)
		{
			acutPrintf(_T("\n%s"), res[i].kACharPtr());
		}
	}

	/// <summary>
	/// this command will remove an entry from the "ADSK_EMPLOYEE_DICTIONARY" dictionary
	/// </summary>
	static void ADSKMyGroupLesson4_removeentry() {
		TCHAR nodName[100] = _T("ADSK_EMPLOYEE_DICTIONARY");
		TCHAR empName[100];
		if (acedGetString(0, _T("Enter Employee Name to delete Record: "), empName) != RTNORM)
		{
			acutPrintf(_T("Unable to get name. Abort!"));
			return;
		}

		switch (removeRecordFromNOD(empName, nodName))
		{
		case 1:
			acutPrintf(_T("Failed to get the Named Objects Dictionary from the current working database. Abort!!"));
			break;
		case 2:
			acutPrintf(_T("Failed to Get/ No existing \"%s\" dictionary. Abort!!"), nodName);
			break;
		case 3:
			acutPrintf(_T("Failed to Open the EmpDict obj for reading. Aborting!!"));
			break;
		case 4:
			acutPrintf(_T("\nEntry found in the NOD, but it is not a dictionary."));
			break;
		case 5:
			acutPrintf(_T("Employee's entry not found!"));
			break;
		case 6:
			acutPrintf(_T("Failed to Open the Emp XRecord obj for writing. Aborting!!"));
			break;
		default:
			acutPrintf(_T("Employee record for %s deleted!!"), empName);
			break;
		}
	}

	static void ADSKMyGroupLesson5_y() {
		acutPrintf(_T("\ntest"));
	}


};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CArxProject1App)

ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject1App, ADSKMyGroup, Lesson2_input, l2_input, ACRX_CMD_TRANSPARENT | ACRX_CMD_NOPAPERSPACE | ACRX_CMD_NOBEDIT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject1App, ADSKMyGroup, Lesson2_select, l2_select, ACRX_CMD_TRANSPARENT | ACRX_CMD_NOPAPERSPACE | ACRX_CMD_NOBEDIT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject1App, ADSKMyGroup, Lesson3_create, l3_create, ACRX_CMD_TRANSPARENT | ACRX_CMD_NOPAPERSPACE | ACRX_CMD_NOBEDIT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject1App, ADSKMyGroup, Lesson3_setlayer, l3_setlayer, ACRX_CMD_TRANSPARENT | ACRX_CMD_NOPAPERSPACE | ACRX_CMD_NOBEDIT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject1App, ADSKMyGroup, Lesson4_addentry, l4_addentry, ACRX_CMD_TRANSPARENT | ACRX_CMD_NOPAPERSPACE | ACRX_CMD_NOBEDIT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject1App, ADSKMyGroup, Lesson4_list, l4_list, ACRX_CMD_TRANSPARENT | ACRX_CMD_NOPAPERSPACE | ACRX_CMD_NOBEDIT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject1App, ADSKMyGroup, Lesson4_removeentry, l4_removeentry, ACRX_CMD_TRANSPARENT | ACRX_CMD_NOPAPERSPACE | ACRX_CMD_NOBEDIT, NULL)

ACED_ARXCOMMAND_ENTRY_AUTO(CArxProject1App, ADSKMyGroup, Lesson5_y, l5_y, ACRX_CMD_TRANSPARENT | ACRX_CMD_NOPAPERSPACE | ACRX_CMD_NOBEDIT, NULL)

