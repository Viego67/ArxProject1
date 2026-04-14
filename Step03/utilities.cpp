#include "StdAfx.h"
#include "utilities.h"
Acad::ErrorStatus createLayer(const TCHAR* layerName, AcDbObjectId& layerId)
{
    Acad::ErrorStatus es;

    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

    AcDbLayerTable* pLayerTable;
    es = pDb->getLayerTable(pLayerTable, AcDb::kForRead);
    if (es != Acad::eOk)
        return es;

    if (pLayerTable->has(layerName))
    {
        pLayerTable->getAt(layerName, layerId);
        pLayerTable->close();
        return Acad::eOk;
    }

    pLayerTable->upgradeOpen(); 

    AcDbLayerTableRecord* pLayer = new AcDbLayerTableRecord;
    pLayer->setName(layerName);

    es = pLayerTable->add(layerId, pLayer);

    pLayer->close();
    pLayerTable->close();

    return es;
}

Acad::ErrorStatus createBlockRecord(const TCHAR* name)
{
    Acad::ErrorStatus es;

    AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

    AcDbBlockTable* pBlockTable;
    es = pDb->getBlockTable(pBlockTable, AcDb::kForRead);
    if (es != Acad::eOk)
        return es;

    if (pBlockTable->has(name))
    {
        pBlockTable->close();
        return Acad::eDuplicateKey;
    }

    pBlockTable->upgradeOpen();

    AcDbBlockTableRecord* pBlock = new AcDbBlockTableRecord;
    pBlock->setName(name);
    pBlock->setOrigin(AcGePoint3d(0, 0, 0));

    es = pBlockTable->add(pBlock);

    pBlock->close();
    pBlockTable->close();

    return es;
}