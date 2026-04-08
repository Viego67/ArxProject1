// (C) Copyright 2002-2012 by Autodesk, Inc. 
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
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep04App : public AcRxArxApp {

public:
	CStep04App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
	// The ACED_ARXCOMMAND_ENTRY_AUTO macro can be applied to any static member 
	// function of the CStep04App class.
	// The function should take no arguments and return nothing.
	//
	// NOTE: ACED_ARXCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid and
	// have arguments to define context and command mechanism.
	
	// ACED_ARXCOMMAND_ENTRY_AUTO(classname, group, globCmd, locCmd, cmdFlags, UIContext)
	// ACED_ARXCOMMAND_ENTRYBYID_AUTO(classname, group, globCmd, locCmdId, cmdFlags, UIContext)
	// only differs that it creates a localized name using a string in the resource file
	//   locCmdId - resource ID for localized command

	// Modal Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL)
	static void ADSKMyGroupMyCommand () {
		// Put your command code here

	}

	// Modal Command with pickfirst selection
	// ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET)
	static void ADSKMyGroupMyPickFirst () {
		ads_name result ;
		int iRet =acedSSGet (ACRX_T("_I"), NULL, NULL, NULL, result) ;
		if ( iRet == RTNORM )
		{
			// There are selected entities
			// Put your command using pickfirst set code here
		}
		else
		{
			// There are no selected entities
			// Put your command code here
		}
	}

	// Application Session Command with localized name
	// ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION)
	static void ADSKMyGroupMySessionCmd () {
		// Put your command code here
	}

	// The ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO macros can be applied to any static member 
	// function of the CStep04App class.
	// The function may or may not take arguments and have to return RTNORM, RTERROR, RTCAN, RTFAIL, RTREJ to AutoCAD, but use
	// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal to return
	// a value to the Lisp interpreter.
	//
	// NOTE: ACED_ADSFUNCTION_ENTRY_AUTO / ACED_ADSCOMMAND_ENTRY_AUTO has overloads where you can provide resourceid.
	
	//- ACED_ADSFUNCTION_ENTRY_AUTO(classname, name, regFunc) - this example
	//- ACED_ADSSYMBOL_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file
	//- ACED_ADSCOMMAND_ENTRY_AUTO(classname, name, regFunc) - a Lisp command (prefix C:)
	//- ACED_ADSCOMMAND_ENTRYBYID_AUTO(classname, name, nameId, regFunc) - only differs that it creates a localized name using a string in the resource file

	// Lisp Function is similar to ARX Command but it creates a lisp 
	// callable function. Many return types are supported not just string
	// or integer.
	// ACED_ADSFUNCTION_ENTRY_AUTO(CStep04App, MyLispFunction, false)

    static void AdskStep04_ADDENTRY(void)
    {
        TCHAR name[133];
        if (acedGetString(0, _T("\nEnter employee name: "), name) != RTNORM)
            return;

        Acad::ErrorStatus es;
        AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

        AcDbDictionary* pNOD = nullptr;
        es = pDb->getNamedObjectsDictionary(pNOD, AcDb::kForRead);
        if (es != Acad::eOk)
            return;

        AcDbDictionary* pEmpDict = nullptr;

        if (pNOD->getAt(_T("ADSK_EMPLOYEE_DICTIONARY"), (AcDbObject*&)pEmpDict, AcDb::kForRead) != Acad::eOk)
        {
            pNOD->upgradeOpen();

            pEmpDict = new AcDbDictionary;
            AcDbObjectId dictId;
            es = pNOD->setAt(_T("ADSK_EMPLOYEE_DICTIONARY"), pEmpDict, dictId);

            pNOD->close();

            if (es != Acad::eOk)
            {
                delete pEmpDict;
                return;
            }
        }
        else
        {
            pNOD->close();
        }

        if (pEmpDict->has(name))
        {
            acutPrintf(_T("\nEmployee already exists."));
            pEmpDict->close();
            return;
        }

        pEmpDict->upgradeOpen();

        AcDbXrecord* pXrec = new AcDbXrecord;
        AcDbObjectId xrecId;
        es = pEmpDict->setAt(name, pXrec, xrecId);

        if (es == Acad::eOk)
            acutPrintf(_T("\nEmployee added: %s"), name);

        pXrec->close();
        pEmpDict->close();
    }

    static void AdskStep04_LISTENTRIES(void)
    {
        Acad::ErrorStatus es;
        AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

        AcDbDictionary* pNOD = nullptr;
        es = pDb->getNamedObjectsDictionary(pNOD, AcDb::kForRead);
        if (es != Acad::eOk)
            return;

        AcDbDictionary* pEmpDict = nullptr;
        es = pNOD->getAt(_T("ADSK_EMPLOYEE_DICTIONARY"), (AcDbObject*&)pEmpDict, AcDb::kForRead);
        pNOD->close();

        if (es != Acad::eOk)
        {
            acutPrintf(_T("\nEmployee dictionary not found."));
            return;
        }

        AcDbDictionaryIterator* pIter = pEmpDict->newIterator();
        acutPrintf(_T("\nEmployees:"));

        for (; !pIter->done(); pIter->next())
        {
            acutPrintf(_T("\n - %s"), pIter->name());
        }

        delete pIter;
        pEmpDict->close();
    }

    static void AdskStep04_REMOVEENTRY(void)
    {
        TCHAR name[133];
        if (acedGetString(0, _T("\nEnter employee name to remove: "), name) != RTNORM)
            return;

        Acad::ErrorStatus es;
        AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

        AcDbDictionary* pNOD = nullptr;
        es = pDb->getNamedObjectsDictionary(pNOD, AcDb::kForRead);
        if (es != Acad::eOk)
            return;

        AcDbDictionary* pEmpDict = nullptr;
        es = pNOD->getAt(_T("ADSK_EMPLOYEE_DICTIONARY"), (AcDbObject*&)pEmpDict, AcDb::kForWrite);
        pNOD->close();

        if (es != Acad::eOk)
        {
            acutPrintf(_T("\nEmployee dictionary not found."));
            return;
        }

        AcDbObjectId objId;
        es = pEmpDict->getAt(name, objId);
        if (es != Acad::eOk)
        {
            acutPrintf(_T("\nEmployee not found: %s"), name);
            pEmpDict->close();
            return;
        }

        AcDbObject* pObj = nullptr;
        es = acdbOpenObject(pObj, objId, AcDb::kForWrite);
        if (es == Acad::eOk)
        {
            pObj->erase();
            pObj->close();
            acutPrintf(_T("\nEmployee removed: %s"), name);
        }

        pEmpDict->close();
    }

	static int ads_MyLispFunction () {
		//struct resbuf *args =acedGetArgs () ;
		
		// Put your command code here

		//acutRelRb (args) ;
		
		// Return a value to the AutoCAD Lisp Interpreter
		// acedRetNil, acedRetT, acedRetVoid, acedRetInt, acedRetReal, acedRetStr, acedRetPoint, acedRetName, acedRetList, acedRetVal

		return (RTNORM) ;
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep04App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AdskStep04, _ADDENTRY, ADDENTRY, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AdskStep04, _LISTENTRIES, LISTENTRIES, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, AdskStep04, _REMOVEENTRY, REMOVEENTRY, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKMyGroup, MyCommand, MyCommandLocal, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKMyGroup, MyPickFirst, MyPickFirstLocal, ACRX_CMD_MODAL | ACRX_CMD_USEPICKSET, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKMyGroup, MySessionCmd, MySessionCmdLocal, ACRX_CMD_MODAL | ACRX_CMD_SESSION, NULL)
ACED_ADSSYMBOL_ENTRY_AUTO(CStep04App, MyLispFunction, false)

