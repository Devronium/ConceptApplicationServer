/*
 * FPMs-TCTS SOFTWARE LIBRARY
 *
 * File    : mbrola.cpp
 * Purpose : Mbrola interface with the MBROLA synthesizer DLL
 * Author  : Alain Ruelle
 * Email   : ruelle@multitel.be
 *
 * 31/08/98 : New Interface, using functions instead of Object
 *
 * Copyright (c) 1997 Faculte Polytechnique de Mons (TCTS lab)
 * All rights reserved.
 */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "mbrola.h"

HINSTANCE   hinstDllMBR;
PROCIC      init_MBR;
PROCICCC    init_rename_MBR;
PROCIC      write_MBR;
PROCIV      flush_MBR;
PROCISI     read_MBR;
PROCIPVIAT  readType_MBR;
PROCVV      close_MBR;
PROCVV      reset_MBR;
PROCIV      lastError_MBR;
PROCVCI     lastErrorStr_MBR;
PROCVI      setNoError_MBR;
PROCIV      getNoError_MBR;
PROCVI      setFreq_MBR;
PROCIV      getFreq_MBR;
PROCVF      setVolumeRatio_MBR;
PROCFV      getVolumeRatio_MBR;
PROCVCI     getVersion_MBR;
PROCICII    getDatabaseInfo_MBR;
PROCPPAR    setParser_MBR;
PROCPPHOCF  init_Phone;
PROCVPPHO   reset_Phone;
PROCVPPHO   close_Phone;
PROCVPPHOFF appendf0_Phone;

BOOL load_MBR() {
    if (!(hinstDllMBR = LoadLibrary("mbrola.dll")))
        return FALSE;
    init_MBR            = (PROCIC)GetProcAddress(hinstDllMBR, "init_MBR");
    init_rename_MBR     = (PROCICCC)GetProcAddress(hinstDllMBR, "init_rename_MBR");
    write_MBR           = (PROCIC)GetProcAddress(hinstDllMBR, "write_MBR");
    flush_MBR           = (PROCIV)GetProcAddress(hinstDllMBR, "flush_MBR");
    read_MBR            = (PROCISI)GetProcAddress(hinstDllMBR, "read_MBR");
    readType_MBR        = (PROCIPVIAT)GetProcAddress(hinstDllMBR, "readtype_MBR");
    close_MBR           = (PROCVV)GetProcAddress(hinstDllMBR, "close_MBR");
    reset_MBR           = (PROCVV)GetProcAddress(hinstDllMBR, "reset_MBR");
    lastError_MBR       = (PROCIV)GetProcAddress(hinstDllMBR, "lastError_MBR");
    lastErrorStr_MBR    = (PROCVCI)GetProcAddress(hinstDllMBR, "lastErrorStr_MBR");
    setNoError_MBR      = (PROCVI)GetProcAddress(hinstDllMBR, "setNoError_MBR");
    getNoError_MBR      = (PROCIV)GetProcAddress(hinstDllMBR, "getNoError_MBR");
    setFreq_MBR         = (PROCVI)GetProcAddress(hinstDllMBR, "setFreq_MBR");
    getFreq_MBR         = (PROCIV)GetProcAddress(hinstDllMBR, "getFreq_MBR");
    setVolumeRatio_MBR  = (PROCVF)GetProcAddress(hinstDllMBR, "setVolumeRatio_MBR");
    getVolumeRatio_MBR  = (PROCFV)GetProcAddress(hinstDllMBR, "getVolumeRatio_MBR");
    getVersion_MBR      = (PROCVCI)GetProcAddress(hinstDllMBR, "getVersion_MBR");
    getDatabaseInfo_MBR = (PROCICII)GetProcAddress(hinstDllMBR, "getDatabaseInfo_MBR");
    setParser_MBR       = (PROCPPAR)GetProcAddress(hinstDllMBR, "setParser_MBR");
    init_Phone          = (PROCPPHOCF)GetProcAddress(hinstDllMBR, "init_Phone");
    reset_Phone         = (PROCVPPHO)GetProcAddress(hinstDllMBR, "reset_Phone");
    close_Phone         = (PROCVPPHO)GetProcAddress(hinstDllMBR, "close_Phone");
    appendf0_Phone      = (PROCVPPHOFF)GetProcAddress(hinstDllMBR, "appendf0_Phone");

    if ((!init_MBR) ||
        (!init_rename_MBR) ||
        (!write_MBR) ||
        (!flush_MBR) ||
        (!read_MBR) ||
        (!readType_MBR) ||
        (!close_MBR) ||
        (!reset_MBR) ||
        (!lastError_MBR) ||
        (!lastErrorStr_MBR) ||
        (!setNoError_MBR) ||
        (!getNoError_MBR) ||
        (!setFreq_MBR) ||
        (!getFreq_MBR) ||
        (!setVolumeRatio_MBR) ||
        (!getVolumeRatio_MBR) ||
        (!getVersion_MBR) ||
        (!getDatabaseInfo_MBR) ||
        (!setParser_MBR) ||
        (!init_Phone) ||
        (!reset_Phone) ||
        (!close_Phone) ||
        (!appendf0_Phone)) {
        FreeLibrary(hinstDllMBR);
        hinstDllMBR = NULL;
        return FALSE;
    }
    return TRUE;
}

void unload_MBR() {
    if (hinstDllMBR) {
        FreeLibrary(hinstDllMBR);
        hinstDllMBR = NULL;
    }
}
