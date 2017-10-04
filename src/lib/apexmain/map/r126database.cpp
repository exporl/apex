/******************************************************************************
 * Copyright (C) 2008  Tom Francart <tom.francart@med.kuleuven.be>            *
 *                                                                            *
 * This file is part of APEX 3.                                               *
 *                                                                            *
 * APEX 3 is free software: you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * APEX 3 is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with APEX 3.  If not, see <http://www.gnu.org/licenses/>.            *
 *****************************************************************************/

#ifdef R126

#include "r126database.h"
#include "r126nucleusmaprecord.h"
#include "r126patientrecord.h"

#include "exceptions.h"
#include <ATLComTime.h> //COLEDateTime
#include <objbase.h>    //COM

#import "C:\Program files\Common Files\System\Ado\msado15.dll" rename("EOF", "ADOEOF")

// FIXME maybe better to open connection in ctor and close it in dtor

namespace r126
{

namespace
{ // SCREEN
void sf_DisplayError(const QString ac_sError)
{
    /* gui::SimpleDialogBox* p = new gui::SimpleDialogBox(
     QMessageBox::Critical,
       "Error", ac_sError );
     p->exec(); */
}

void sf_DisplayMessage(const QString ac_sMessage)
{
    /*gui::SimpleDialogBox* p = new gui::SimpleDialogBox(
    QMessageBox::Information,
      "", ac_sMessage );
    p->exec(); */
}
}

R126DataBase::R126DataBase() : m_pConnectionSettings(new R126DataBaseSettings())
{
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (hr != S_OK) // init COM
    {
        // only happens first time for some reason
        // but is not a problem..
        if (hr != RPC_E_CHANGED_MODE)
            throw(ApexStringException(
                "CoInitializeEx failed - cannot connect to database"));
    }
}

R126DataBase::~R126DataBase()
{
    sm_bPatientsRecorded = false;
    delete m_pConnectionSettings;
    CoUninitialize(); // uninit COM..
}

bool R126DataBase::sm_bPatientsRecorded = false;

void R126DataBase::mf_SetDataBaseSettings(const eR126VersionNumber ac_eVersion,
                                          const QString &ac_sServer)
{
    m_pConnectionSettings->mf_Set(ac_eVersion, ac_sServer);
}

const bool R126DataBase::mf_bTestConnection() const
{
    HRESULT hr;
    try {
        // Create the database-connection and recordset objects
        ADODB::_ConnectionPtr connection;
        hr = connection.CreateInstance(__uuidof(ADODB::Connection));
        if (FAILED(hr)) {
            throw _com_error(hr);
        }

        // Set up the connection
        connection->CursorLocation = ADODB::adUseClient;
        _bstr_t sConn(m_pConnectionSettings->mf_sGetConnectionString().ascii());
        connection->Open(sConn, L"", L"", ADODB::adConnectUnspecified);

        // Clean-Up
        connection->Close();
    } catch (_com_error &e) {
        QString sMess(e.Description().operator char *());
        sf_DisplayError("Could not Connect to Database: " + sMess);
        return false;
    } catch (...) {
        return false;
    }
    sf_DisplayMessage("Connection OK");
    return true;
}

const R126DataBaseSettings *R126DataBase::mf_pGetDataBaseSettings() const
{
    return m_pConnectionSettings;
}

std::vector<R126PatientRecord *> &R126DataBase::mf_FillPatientRecords()
{
    if (!sm_bPatientsRecorded) {
        // do data exchange
        HRESULT hr;
        try {
            // Create the database-connection and recordset objects
            ADODB::_ConnectionPtr connection;
            hr = connection.CreateInstance(__uuidof(ADODB::Connection));
            if (FAILED(hr)) {
                throw _com_error(hr);
            }

            ADODB::_RecordsetPtr PatientRecordSet;
            hr = PatientRecordSet.CreateInstance(__uuidof(ADODB::Recordset));
            if (FAILED(hr)) {
                throw _com_error(hr);
            }

            // Set up the connection
            connection->CursorLocation = ADODB::adUseClient;
            _bstr_t sConn(
                m_pConnectionSettings->mf_sGetConnectionString().ascii());
            connection->Open(sConn, L"", L"", ADODB::adConnectUnspecified);

            // Select all patients in the database
            // Old SELECT PatientGUID, Surname, Firstname, Gender, DateOfBirth
            // from Patient order by Patient.Surname",
            PatientRecordSet->Open(
                "SELECT Patient.PatientGUID, Patient.Surname, "
                "Patient.Firstname, Patient.Gender, Patient.DateOfBirth, "
                "Implant.DateOfImplantation, Implant.ImplantType from Patient, "
                "Implant where Implant.PatientGUID = Patient.PatientGUID order "
                "by Patient.Surname",
                connection.GetInterfacePtr(), ADODB::adOpenForwardOnly,
                ADODB::adLockReadOnly, ADODB::adCmdText);

            while (!PatientRecordSet->ADOEOF) {
                _variant_t var;
                R126PatientRecord *pRec = new R126PatientRecord();

                // Fill pRec from database
                var = PatientRecordSet->Fields->GetItem(L"Surname")->GetValue();
                pRec->m_sSurName =
                    QString(static_cast<char *>(_bstr_t(var.bstrVal)));

                var =
                    PatientRecordSet->Fields->GetItem(L"Firstname")->GetValue();
                pRec->m_sFirstName =
                    QString(static_cast<char *>(_bstr_t(var.bstrVal)));

                var = PatientRecordSet->Fields->GetItem(L"PatientGUID")
                          ->GetValue();
                pRec->m_sGUID =
                    QString(static_cast<char *>(_bstr_t(var.bstrVal)));

                var = PatientRecordSet->Fields->GetItem(L"DateOfBirth")
                          ->GetValue();
                COleDateTime datetemp(var.date);
                pRec->m_sBirth = QString(datetemp.Format().GetBuffer());

                var = PatientRecordSet->Fields->GetItem(L"Gender")->GetValue();
                QString sex(static_cast<char *>(_bstr_t(var.bstrVal)));
                if (sex == "LRD_Gender_NotSpecified") {
                    pRec->m_sSex = "/";
                } else if (sex == "LRD_Gender_Male") {
                    pRec->m_sSex = "M";
                } else if (sex == "LRD_Gender_Female") {
                    pRec->m_sSex = "F";
                }

                var = PatientRecordSet->Fields->GetItem(L"DateOfImplantation")
                          ->GetValue();
                datetemp = var.date;
                pRec->m_sImplantDate = QString(datetemp.Format().GetBuffer());

                var = PatientRecordSet->Fields->GetItem(L"ImplantType")
                          ->GetValue();
                QString sImplantType(static_cast<char *>(_bstr_t(var.bstrVal)));
                if (sImplantType.left(16) == "LRD_ImplantType_") {
                    pRec->m_sImplant = sImplantType.mid(16);
                } else {
                    pRec->m_sImplant = "Unknown";
                }

                m_aPatients.push_back(pRec);
                PatientRecordSet->MoveNext();
            }

            PatientRecordSet->Close();
            connection->Close();
        } catch (_com_error &e) {
            QString sMess(e.Description().operator char *());
            sf_DisplayError("Could not Connect to Database: " + sMess);
            return m_aPatients;
        } catch (...) {
            Q_ASSERT(0);
        }
        sm_bPatientsRecorded = true;
    }
    return m_aPatients;
}

std::vector<R126NucleusMAPRecord *> &
R126DataBase::mf_FillNucleusMapsInfo(const QString &ac_sPatientGUID)
{
    // remove any previous records
    std::vector<R126NucleusMAPRecord *>::iterator it = m_aMaps.begin();
    while (it != m_aMaps.end()) {
        delete *it;
        it++;
    }

    m_aMaps.clear();

    // do data exchange
    HRESULT hr;
    try {
        // Create the database-connection and recordset objects
        ADODB::_ConnectionPtr connection;
        hr = connection.CreateInstance(__uuidof(ADODB::Connection));
        if (FAILED(hr)) {
            throw _com_error(hr);
        }

        ADODB::_RecordsetPtr MAPRecordSet;
        hr = MAPRecordSet.CreateInstance(__uuidof(ADODB::Recordset));
        if (FAILED(hr)) {
            throw _com_error(hr);
        }

        // Set up the connection
        connection->CursorLocation = ADODB::adUseClient;
        _bstr_t sConn(m_pConnectionSettings->mf_sGetConnectionString().ascii());
        connection->Open(sConn, L"", L"", ADODB::adConnectUnspecified);

        // Select a patient in the database
        QString sqlstring = "SELECT * from MAP where PatientGUID = '" +
                            ac_sPatientGUID + "' order by CreationDateTime";

        MAPRecordSet->Open((_variant_t)sqlstring, connection.GetInterfacePtr(),
                           ADODB::adOpenForwardOnly, ADODB::adLockReadOnly,
                           ADODB::adCmdText);

        while (!MAPRecordSet->ADOEOF) {
            _variant_t var;
            R126NucleusMAPRecord *pRec = new R126NucleusMAPRecord();

            var = MAPRecordSet->Fields->GetItem(L"MAPNumber")->GetValue();
            pRec->m_nMAPNumber = var.intVal;

            var =
                MAPRecordSet->Fields->GetItem(L"CreationDateTime")->GetValue();
            COleDateTime datetemp(var.date);
            pRec->m_sMAPDate = QString(datetemp.Format().GetBuffer());

            var = MAPRecordSet->Fields->GetItem(L"MAPGUID")->GetValue();
            pRec->m_sGUID = QString(static_cast<char *>(_bstr_t(var.bstrVal)));

            var = MAPRecordSet->Fields->GetItem(L"Maxima")->GetValue();
            pRec->m_nMaxima = var.intVal;

            var = MAPRecordSet->Fields->GetItem(L"StimulationMode")->GetValue();
            QString sStimulationMode(static_cast<char *>(_bstr_t(var.bstrVal)));
            if (sStimulationMode.left(23) == "LRD_MapStimulationMode_") {
                pRec->m_sStimulationMode = sStimulationMode.mid(23);
            } else {
                pRec->m_sStimulationMode = "Unknown";
            }

            var = MAPRecordSet->Fields->GetItem(L"StimulationRate")->GetValue();
            pRec->m_nStimulationRate = var.intVal;

            var = MAPRecordSet->Fields->GetItem(L"TotalStimulationRate")
                      ->GetValue();
            // pRec->m_nMaxima = var.intVal;
            pRec->m_nTotalRate = var.intVal; // [Tom]

            var = MAPRecordSet->Fields->GetItem(L"Strategy")->GetValue();
            QString sStrategy(static_cast<char *>(_bstr_t(var.bstrVal)));
            if (sStrategy.left(16) == "LRD_MapStrategy_") {
                pRec->m_sStrategy = sStrategy.mid(16);
            } else {
                pRec->m_sStrategy = "Unknown";
            }

            m_aMaps.push_back(pRec);
            MAPRecordSet->MoveNext();
        }

        MAPRecordSet->Close();
        connection->Close();
    } catch (_com_error &e) {
        e.Description();
        Q_ASSERT(0);
    } catch (...) {
        Q_ASSERT(0);
    }
    return m_aMaps;
}

R126NucleusMAPRecord *
R126DataBase::mf_pGetNucleusMap(const QString &ac_sMapGUID)
{
    R126NucleusMAPRecord *pRec = new R126NucleusMAPRecord();

    // do data exchange
    HRESULT hr;
    try {
        // Create the database-connection and recordset objects
        ADODB::_ConnectionPtr connection;
        hr = connection.CreateInstance(__uuidof(ADODB::Connection));
        if (FAILED(hr)) {
            throw _com_error(hr);
        }

        ADODB::_RecordsetPtr MAPRecordSet;
        hr = MAPRecordSet.CreateInstance(__uuidof(ADODB::Recordset));
        if (FAILED(hr)) {
            throw _com_error(hr);
        }

        ADODB::_RecordsetPtr ChannelRecordSet;
        hr = ChannelRecordSet.CreateInstance(__uuidof(ADODB::Recordset));
        if (FAILED(hr)) {
            throw _com_error(hr);
        }

        // Set up the connection
        connection->CursorLocation = ADODB::adUseClient;
        _bstr_t sConn(m_pConnectionSettings->mf_sGetConnectionString().ascii());
        connection->Open(sConn, L"", L"", ADODB::adConnectUnspecified);

        // Select the MAP from the database
        QString sqlstring =
            "SELECT MAP.MAPNumber, MAP.CreationDateTime, MAP.MAPGUID"
            ", MAP.InterPhaseGap, MAP.StimulationRate, "
            "MAP.TotalStimulationRate "
            ", Patient.Surname, Patient.Firstname "
            "from MAP, Patient "
            "where Patient.PatientGUID=MAP.PatientGUID and MAP.MAPGUID='" +
            ac_sMapGUID + "'";

        MAPRecordSet->Open((_variant_t)sqlstring, connection.GetInterfacePtr(),
                           ADODB::adOpenForwardOnly, ADODB::adLockReadOnly,
                           ADODB::adCmdText);

        // Check if only one MAP was found
        _variant_t var;
        var = MAPRecordSet->RecordCount;
        if (var.intVal != 1)
            throw(102);

        // fill record with map data
        var = MAPRecordSet->Fields->GetItem(L"MAPNumber")->GetValue();
        pRec->m_nMAPNumber = var.intVal;
        var = MAPRecordSet->Fields->GetItem(L"MAPGUID")->GetValue();
        pRec->m_sGUID = QString(static_cast<char *>(_bstr_t(var.bstrVal)));
        var = MAPRecordSet->Fields->GetItem(L"Surname")->GetValue();
        pRec->m_sSurname = QString(static_cast<char *>(_bstr_t(var.bstrVal)));
        var = MAPRecordSet->Fields->GetItem(L"Firstname")->GetValue();
        pRec->m_sFirstname = QString(static_cast<char *>(_bstr_t(var.bstrVal)));
        var = MAPRecordSet->Fields->GetItem(L"InterPhaseGap")->GetValue();
        pRec->m_nInterPhaseGap = var.intVal;
        var =
            MAPRecordSet->Fields->GetItem(L"TotalStimulationRate")->GetValue();
        pRec->m_nTotalRate = /*(int) 1e6/*/ var.intVal;

        // Get the physical channels
        sqlstring =
            "SELECT * from PhysicalChannel where PhysicalChannel.MAPGUID='" +
            ac_sMapGUID + "' order by ChannelNumber DESC";
        // !!! R126 orders channels from base to apex (here we turn around this
        // ordering !!)

        ChannelRecordSet->Open(
            (_variant_t)sqlstring, connection.GetInterfacePtr(),
            ADODB::adOpenForwardOnly, ADODB::adLockReadOnly, ADODB::adCmdText);

        // fill record with channel data
        int i = 0;
        while (!ChannelRecordSet->ADOEOF) {
            Q_ASSERT(i < sc_nChannels); //!!
            _variant_t var;

            var = ChannelRecordSet->Fields->GetItem(L"ActiveElectrode")
                      ->GetValue();
            pRec->m_naElectrodes[i] = var.intVal;

            var = ChannelRecordSet->Fields->GetItem(L"StimulationMode")
                      ->GetValue();
            if (i == 0)
                pRec->m_sStimulationMode =
                    QString(static_cast<char *>(_bstr_t(var.bstrVal)));
            else if (pRec->m_sStimulationMode !=
                     static_cast<char *>(_bstr_t(var.bstrVal)))
                throw(104);

            var = ChannelRecordSet->Fields->GetItem(L"ChannelEnabled")
                      ->GetValue();
            pRec->m_baChannelEnabled[i] = var.boolVal;

            var = ChannelRecordSet->Fields->GetItem(L"TLevel")->GetValue();
            pRec->m_naTLevels[i] = var.intVal;

            var = ChannelRecordSet->Fields->GetItem(L"CLevel")->GetValue();
            pRec->m_naCLevels[i] = var.intVal;

            var = ChannelRecordSet->Fields->GetItem(L"PulseWidth")->GetValue();
            if (i == 0)
                pRec->m_nMinPHaseWidth = var.intVal;
            else if (pRec->m_nMinPHaseWidth != var.intVal)
                throw(103);

            i++;
            ChannelRecordSet->MoveNext();
        }
        pRec->m_nChannels = i;

        MAPRecordSet->Close();
        ChannelRecordSet->Close();
        connection->Close();
    } catch (_com_error &e) {
        e.Description();
        Q_ASSERT(0);
        delete pRec;
        return 0;
    } catch (...) {
        delete pRec;
        return 0;
    }
    return pRec;
}
}

#endif //#ifdef R126
