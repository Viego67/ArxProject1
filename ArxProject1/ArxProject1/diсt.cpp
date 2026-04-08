#include "Utilities.h"
#include "stdafx.h"

int createRecordInNOD(TCHAR* value, TCHAR* nodName)
{
	AcDbDictionary* pNOD;
	if (acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNOD, AcDb::kForRead) != Acad::eOk)
	{
		return 1;
	}

	AcDbObjectId idDict;
	AcDbDictionary* pEmpDict = NULL;

	if (pNOD->getAt(nodName, idDict) == Acad::eKeyNotFound)
	{
		//create key
		if (pNOD->upgradeOpen() != Acad::eOk)
		{
			pNOD->close();
			return 2;
		}

		pEmpDict = new AcDbDictionary;

		if (pNOD->setAt(nodName, pEmpDict, idDict) != Acad::eOk)
		{
			delete pEmpDict;
			pNOD->close();
			return 3;
		}
	}
	else
	{
		AcDbObject* pEmpObj;
		if (acdbOpenAcDbObject(pEmpObj, idDict, AcDb::kForWrite) != Acad::eOk)
		{
			pNOD->close();
			return 4;
		}

		// Check if someone has else has created an entry with our name
		// that is not a dictionary. This should never happen as long as
		// I use the registered developer RDS prefix.
		if ((pEmpDict = AcDbDictionary::cast(pEmpObj)) == NULL)
		{
			
			pEmpObj->close();
			pNOD->close();
			return 5;
		}
	}
	pNOD->close();

	// 5. Check if the name of the employee is already in the "ADSK_EMPLOYEE_DICTIONARY" dictionary. 
	if (pEmpDict->getAt(value, idDict) == Acad::eOk)
	{
		pEmpDict->close();
		return 6;
	}

	//6. If the employee dictionary is not present, then create a 
	// new AcDbXrecord and add it to the "ADSK_EMPLOYEE_DICTIONARY" 
	// (AcDbDictionary::setAt()). 

	AcDbXrecord* pEmpXRec = new AcDbXrecord;
	if (pEmpDict->setAt(value, pEmpXRec, idDict) != Acad::eOk)
	{
		delete pEmpXRec;
		pEmpDict->close();
		return 7;
	}

	pEmpXRec->close();
	pEmpDict->close();
	return 0;
}

AcStringArray listRecordsInNOD(TCHAR* nodName)
{
	AcStringArray empList;
	//1. Get the Named Objects Dictionary from the current working database 
	//(AcDbDictionary, AcDbDatabase::getNamedObjectsDictionary()). 
	AcDbDictionary* pNOD;
	if (acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNOD, AcDb::kForRead) != Acad::eOk)
	{
		empList.append(_T("Failed to Get the Named Objects Dictionary from the current working database"));
		return empList;
	}

	//2. Get the "ADSK_EMPLOYEE_DICTIONARY" dictionary
	//(AcDbDictionary::getAt()).
	AcDbObjectId idObj;
	AcDbObject* pObj;

	if (pNOD->getAt(_T("ADSK_EMPLOYEE_DICTIONARY"), idObj) != Acad::eOk)
	{
		empList.append(_T("Failed to fetch ADSK_EMPLOYEE_DICTIONARY dictionary"));
		pNOD->close();
		return empList;
	}

	//Open Emp Dict in read mode***
	if (acdbOpenAcDbObject(pObj, idObj, AcDb::kForRead) != Acad::eOk)
	{
		empList.append(_T("Failed to Open Emp Dict in read mode"));
		pNOD->close();
		return empList;
	}

	// Check if someone has else has created an entry with our name
	// that is not a dictionary. This should never happen as long as
	// I use the registered developer RDS prefix. ???????????
	AcDbDictionary* pEmployeeDict;
	if ((pEmployeeDict = AcDbDictionary::cast(pObj)) == NULL) {
		empList.append(_T("\nEntry found in the NOD, but it is not a dictionary."));
		pObj->close();
		pNOD->close();
		return empList;
	}
	pNOD->close();


	//3. Iterate through the "ADSK_EMPLOYEE_DICTIONARY" and print out 
	//the dictionary keys(employee names).
	//3.1. Create a new iterator
	//(AcDbDictionary::newIterator(), AcDbDictionaryIterator).
	AcDbDictionaryIterator* pDictItr;
	if ((pDictItr = pEmployeeDict->newIterator()) != NULL)
	{
		//3.2. Iterate through the "ADSK_EMPLOYEE_DICTIONARY" 
		//(AcDbDictionaryIterator::done(), AcDbDictionaryIterator::next()).
		for (int i = 1; !pDictItr->done(); pDictItr->next(), i++)
		{
			//3.3. Print the dictionary key
			//(AcDbDictionaryIterator::name()).
			empList.append(pDictItr->name());
		}

		//4. Delete the iterator and don't forget to close opened objects. 
		delete pDictItr;
	}
	pEmployeeDict->close();
	return empList;
}

int removeRecordFromNOD(TCHAR* value, TCHAR* nodName)
{
	//2. Get the Named Objects Dictionary from the current working database
		// (AcDbDictionary, AcDbDatabase::getNamedObjectsDictionary()).
	AcDbDictionary* pNOD;
	if (acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(pNOD, AcDb::kForRead) != Acad::eOk)
	{
		return 1;
	}

	//3. Get the "ADSK_EMPLOYEE_DICTIONARY" dictionary
	// (AcDbDictionary::getAt()).
	AcDbObjectId idObj;
	AcDbObject* pObj;
	if (pNOD->getAt(nodName, idObj) != Acad::eOk)
	{
		pNOD->close();
		return 2;
	}

	//*****Open the EmpDict obj for reading
	if (acdbOpenAcDbObject(pObj, idObj, AcDb::kForRead) != Acad::eOk)
	{
		pNOD->close();
		return 3;
	}
	// ******Check if someone has else has created an entry with our name
	// that is not a dictionary. This should never happen as long as
	// I use the registered developer RDS prefix.
	AcDbDictionary* pEmployeeDict;
	if ((pEmployeeDict = AcDbDictionary::cast(pObj)) == NULL) {
		pObj->close();
		pNOD->close();
		return 4;
	}
	pNOD->close();

	//4. Get the AcDbXrecord entry with the given employee name.
	if (pEmployeeDict->getAt(value, idObj) != Acad::eOk)
	{
		pEmployeeDict->close();
		return 5;
	}
	pEmployeeDict->close();

	//5. If the employee entry exists, open it for write and then 
	// erase it(AcDbObject::erase()).
	if (acdbOpenAcDbObject(pObj, idObj, AcDb::kForWrite) != Acad::eOk)
	{
		pNOD->close();
		return 6;
	}

	pObj->erase();
	pObj->close();
	return 0;
}