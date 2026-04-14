// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- ADSKEmployeeDetails.cpp : Implementation of ADSKEmployeeDetails
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKEmployeeDetails.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKEmployeeDetails::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
    ADSKEmployeeDetails, AcDbObject,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation, ADSKEMPLOYEEDETAILS,
    ADSKSTEP05APP
    | Product Desc : A description for your object
    | Company : Your company name
    | WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKEmployeeDetails::ADSKEmployeeDetails() : AcDbObject()
{
    m_ID = 0;
    m_cube = 0;
    m_firstName = NULL;
    m_lastName = NULL;
}

ADSKEmployeeDetails::~ADSKEmployeeDetails()
{
    if (m_firstName) {
        free(m_firstName);
        m_firstName = NULL;
    }

    if (m_lastName) {
        free(m_lastName);
        m_lastName = NULL;
    }
}

//-----------------------------------------------------------------------------
// Access / modify methods

Acad::ErrorStatus ADSKEmployeeDetails::setID(const Adesk::Int32 ID)
{
    assertWriteEnabled();
    m_ID = ID;
    return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::iD(Adesk::Int32 & ID)
{
    assertReadEnabled();
    ID = m_ID;
    return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::setCube(const Adesk::Int32 cube)
{
    assertWriteEnabled();
    m_cube = cube;
    return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::cube(Adesk::Int32 & cube)
{
    assertReadEnabled();
    cube = m_cube;
    return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::setFirstName(const TCHAR * firstName)
{
    assertWriteEnabled();

    if (m_firstName) {
        free(m_firstName);
        m_firstName = NULL;
    }

    if (firstName)
        m_firstName = _tcsdup(firstName);

    return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::firstName(TCHAR * &firstName)
{
    assertReadEnabled();
    firstName = (m_firstName ? _tcsdup(m_firstName) : NULL);
    return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::setLastName(const TCHAR * lastName)
{
    assertWriteEnabled();

    if (m_lastName) {
        free(m_lastName);
        m_lastName = NULL;
    }

    if (lastName)
        m_lastName = _tcsdup(lastName);

    return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployeeDetails::lastName(TCHAR * &lastName)
{
    assertReadEnabled();
    lastName = (m_lastName ? _tcsdup(m_lastName) : NULL);
    return Acad::eOk;
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol

Acad::ErrorStatus ADSKEmployeeDetails::dwgOutFields(AcDbDwgFiler * pFiler) const
{
    assertReadEnabled();

    //----- Save parent class information first.
    Acad::ErrorStatus es = AcDbObject::dwgOutFields(pFiler);
    if (es != Acad::eOk)
        return es;

    //----- Object version number needs to be saved first
    if ((es = pFiler->writeUInt32(ADSKEmployeeDetails::kCurrentVersionNumber)) != Acad::eOk)
        return es;

    //----- Output params
    if ((es = pFiler->writeInt32(m_ID)) != Acad::eOk)
        return es;

    if ((es = pFiler->writeInt32(m_cube)) != Acad::eOk)
        return es;

    if ((es = pFiler->writeString(m_firstName ? m_firstName : _T(""))) != Acad::eOk)
        return es;

    if ((es = pFiler->writeString(m_lastName ? m_lastName : _T(""))) != Acad::eOk)
        return es;

    return pFiler->filerStatus();
}

Acad::ErrorStatus ADSKEmployeeDetails::dwgInFields(AcDbDwgFiler * pFiler)
{
    assertWriteEnabled();

    //----- Read parent class information first.
    Acad::ErrorStatus es = AcDbObject::dwgInFields(pFiler);
    if (es != Acad::eOk)
        return es;

    //----- Object version number needs to be read first
    Adesk::UInt32 version = 0;
    if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
        return es;

    if (version > ADSKEmployeeDetails::kCurrentVersionNumber)
        return Acad::eMakeMeProxy;

    //if (version < ADSKEmployeeDetails::kCurrentVersionNumber)
    //    return Acad::eMakeMeProxy;

    if (m_firstName) {
        free(m_firstName);
        m_firstName = NULL;
    }

    if (m_lastName) {
        free(m_lastName);
        m_lastName = NULL;
    }

    if ((es = pFiler->readInt32(&m_ID)) != Acad::eOk)
        return es;

    if ((es = pFiler->readInt32(&m_cube)) != Acad::eOk)
        return es;

    if ((es = pFiler->readString(&m_firstName)) != Acad::eOk)
        return es;

    if ((es = pFiler->readString(&m_lastName)) != Acad::eOk)
        return es;

    return pFiler->filerStatus();
}

//- Dxf Filing protocol

Acad::ErrorStatus ADSKEmployeeDetails::dxfOutFields(AcDbDxfFiler* pFiler) const
{
    assertReadEnabled();

    Acad::ErrorStatus es = AcDbObject::dxfOutFields(pFiler);
    if (es != Acad::eOk)
        return es;

    es = pFiler->writeItem(AcDb::kDxfSubclass, _RXST("ADSKEmployeeDetails"));
    if (es != Acad::eOk)
        return es;

    if ((es = pFiler->writeUInt32(AcDb::kDxfInt32, ADSKEmployeeDetails::kCurrentVersionNumber)) != Acad::eOk)
        return es;

    if ((es = pFiler->writeItem(91, m_ID)) != Acad::eOk)
        return es;

    if ((es = pFiler->writeItem(90, m_cube)) != Acad::eOk)
        return es;

    if ((es = pFiler->writeItem(301, m_firstName ? m_firstName : _T(""))) != Acad::eOk)
        return es;

    if ((es = pFiler->writeItem(300, m_lastName ? m_lastName : _T(""))) != Acad::eOk)
        return es;

    return pFiler->filerStatus();
}

Acad::ErrorStatus ADSKEmployeeDetails::dxfInFields(AcDbDxfFiler * pFiler)
{
    assertWriteEnabled();

    //----- Read parent class information first.
    Acad::ErrorStatus es = AcDbObject::dxfInFields(pFiler);
    if (es != Acad::eOk || !pFiler->atSubclassData(_RXST("ADSKEmployeeDetails")))
        return pFiler->filerStatus();

    //----- Object version number needs to be read first
    struct resbuf rb;
    pFiler->readItem(&rb);

    if (rb.restype != AcDb::kDxfInt32) {
        pFiler->pushBackItem();
        pFiler->setError(
            Acad::eInvalidDxfCode,
            _RXST("\nError: expected group code %d (version #)"),
            AcDb::kDxfInt32
        );
        return pFiler->filerStatus();
    }

    Adesk::UInt32 version = (Adesk::UInt32)rb.resval.rlong;
    if (version > ADSKEmployeeDetails::kCurrentVersionNumber)
        return Acad::eMakeMeProxy;

    //if (version < ADSKEmployeeDetails::kCurrentVersionNumber)
    //    return Acad::eMakeMeProxy;

    if (m_firstName) {
        free(m_firstName);
        m_firstName = NULL;
    }

    if (m_lastName) {
        free(m_lastName);
        m_lastName = NULL;
    }

    //----- Read params in non order dependant manner
    while (es == Acad::eOk && (es = pFiler->readResBuf(&rb)) == Acad::eOk) {
        switch (rb.restype) {
        case 91:
            m_ID = rb.resval.rlong;
            break;

        case 90:
            m_cube = rb.resval.rlong;
            break;

        case 301:
            if (rb.resval.rstring)
                m_firstName = _tcsdup(rb.resval.rstring);
            break;

        case 300:
            if (rb.resval.rstring)
                m_lastName = _tcsdup(rb.resval.rstring);
            break;

        default:
            //----- An unrecognized group. Push it back so that the subclass can read it again.
            pFiler->pushBackItem();
            es = Acad::eEndOfFile;
            break;
        }
    }

    //----- At this point the es variable must contain eEndOfFile
    if (es != Acad::eEndOfFile)
        return Acad::eInvalidResBuf;

    return pFiler->filerStatus();
}