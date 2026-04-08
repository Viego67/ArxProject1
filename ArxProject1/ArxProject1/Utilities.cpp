#include "Utilities.h"
#include "stdafx.h"
#include "tchar.h"

//
// Create a new layer or return the ObjectId if it already exists
//
// In :
// const TCHAR* layerName : layer name
// Out :
// AcDbObjectId& layerId : ObjectId of the created or existing layer

Acad::ErrorStatus createLayer(const TCHAR* layerName, AcDbObjectId& layerId)
{
	Acad::ErrorStatus es;
	AcDbLayerTable* pTable;
	layerId = AcDbObjectId::kNull;

	//1. Get the layer table from the current working database
	es = acdbHostApplicationServices()->workingDatabase()->getLayerTable(pTable, AcDb::kForRead);
	//had we opened the layer table in kForWrite mode, then below upgradeOpen() call wouldn't have been necessary
	if (es == Acad::eOk)
	{
		//2. Check to see if a layer of the same name already exists (AcDbLayerTable::getAt()). 
		//If it does, get it's object ID and return it using the second argument of our createLayer() function. 
		es = pTable->getAt(layerName, layerId, false);
		if (es != Acad::eOk)
		{
			//If layer doesn't exist
			//create a new layer table record using the layer name
			AcDbLayerTableRecord* pLrTblRec = new AcDbLayerTableRecord; //heap
			pLrTblRec->setName(layerName);

			//Since we will modify the layer table, the layer table will need to be opened for write
			if ((es = pTable->upgradeOpen()) == Acad::eOk)
			{
				es = pTable->add(layerId, pLrTblRec);
				pLrTblRec->close();
			}
			else
				delete pLrTblRec;
		}
		else
			acutPrintf(_T("Failed to - AcDbLayerTable::getAt()"));

		pTable->close();

	}
	else
		acutPrintf(_T("Failed to Get the layer table from the current working database"));
	return es;
}



// Create a new block table record and add the entities of the employee to it 
// 
// In : 
// const TCHAR* name : Name of block table record 

Acad::ErrorStatus createBlockRecord(const TCHAR* name)
{
	Acad::ErrorStatus es;
	//1. Get the block table from the current working database (AcDbBlockTable, AcDbDatabase::getBlockTable() ) 
	AcDbBlockTable* pBlockTable;
	es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTable, AcDb::kForRead);
	if (es != Acad::eOk)
		return es;

	//2. Check if the block table record already exists (AcDbBlockTable::has()). 
	//If it exists, return an error status (Acad::eDuplicateKey). Don't forget to close the block table. 
	if (pBlockTable->has(name) == Adesk::kTrue)
	{
		pBlockTable->close();
		return Acad::eDuplicateKey;
	}

	//3. Create a new block table record with the "new" operator 
	//and add it to the block table (AcDbBlockTableRecord, AcDbBlockTable::add() ). 
	//(Since we modify the block table it must be opened for write). 
	//Initialize the new block table record. 
	//Set the origin to (0,0,0) (AcDbBlockTableRecord::setOrigin (AcGePoint3d::kOrigin) ). 
	//Set its name. 
	//If the block table record was successfully added to the AutoCAD database, 
	//you can close the block table. 

	AcDbBlockTableRecord* pBTRec = new AcDbBlockTableRecord;
	pBTRec->setName(name);
	pBTRec->setOrigin(AcGePoint3d::kOrigin);
	
	if ((es = pBlockTable->upgradeOpen()) != Acad::eOk)
	{
		delete pBTRec;
		pBlockTable->close();
		return es;
	}

	es = pBlockTable->add(pBTRec);
	if (es != Acad::eOk)
	{
		pBlockTable->close();
		delete pBTRec;
		return es;
	}
	pBlockTable->close();


	//4. Create the "EMPLOYEE" entities and append them to 
	//the new block table record. 
	//(AcDbBlockTableRecord::appendAcDbEntity() ) 
	//The "EMPLOYEE" entities are three circles (AcDbCircle) 
	//and an arc (AcDbArc) with the characteristics outlined in the table below. 
	//Make the face yellow, eyes blue, and mouth red. 
	//(AcDbEntity::setColorIndex()) 

	AcDbCircle* pFace = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 1.0);
	AcDbCircle* pLEye = new AcDbCircle(AcGePoint3d(0.33, 0.25, 0), AcGeVector3d::kZAxis, 0.1);
	AcDbCircle* pREye = new AcDbCircle(AcGePoint3d(-0.33, 0.25, 0), AcGeVector3d::kZAxis, 0.1);
	
	double pi = 3.141592;
	AcDbArc* pMouth = new AcDbArc(AcGePoint3d(0, 0.5, 0), 1.0, pi + (pi * 0.3), pi + (pi * 0.7));

	pFace->setColorIndex(2); ///how to find which index corresponds to which color value????
	pLEye->setColorIndex(5);
	pREye->setColorIndex(5);
	pMouth->setColorIndex(1);

	if ((es = pBTRec->appendAcDbEntity(pFace)) == Acad::eOk)
	{
		pFace->close();
		if ((es = pBTRec->appendAcDbEntity(pLEye)) == Acad::eOk)
		{
			pLEye->close();
			if ((es = pBTRec->appendAcDbEntity(pREye)) == Acad::eOk)
			{
				pREye->close();
				if ((es = pBTRec->appendAcDbEntity(pMouth)) == Acad::eOk)
				{
					pMouth->close();
					pBTRec->close();
				}
				else
				{
					delete pMouth;
					pBTRec->erase();
					pBTRec->close();
					return es;
				}
			}
			else
			{
				delete pREye;
				delete pMouth;
				pBTRec->erase();
				pBTRec->close();
				return es;
			}
		}
		else
		{
			delete pLEye;
			delete pREye;
			delete pMouth;
			pBTRec->erase();
			pBTRec->close();
			return es;
		}
	}
	else
	{
		delete pFace;
		delete pLEye;
		delete pREye;
		delete pMouth;
		pBTRec->erase();
		pBTRec->close();
		return es;
	}


	return es;
}



