/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/hwpf/hwp/pstates/pstates/lab_pstates.c $              */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2013,2015                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
// $Id: lab_pstates.c,v 1.10 2015/03/12 18:06:46 stillgs Exp $

/// \file lab_pstates.c
/// \brief Lab-only (as opposed to product-procedure) support for Pstates.
///
/// Lab-only Pstate support is separated from generic Pstate support to reduce
/// the size of OCC product firmware images.

#include "ssx.h"
// jwy #include "ppc32.h"
#include "lab_pstates.h"
// jwy #include "pmc_register_addresses.h"
// jwy #include "pmc_firmware_registers.h"
// jwy #include "pcbs_register_addresses.h"
// jwy #include "pcbs_firmware_registers.h"

/// Convert a voltage in microvolts to a VRM-11 VID code, rounding the implied
/// voltage as required.
///
/// \param v_uv Voltage in micro-volts
///
/// \param round \a round >= 0 indicate round voltage up, while \a round < 0
/// implies round voltage down
///
/// \param vrm11_vid A pointer to the location of the final VID code. This
/// location is updated even if the final VID code is invalid.
///
/// \bug Confirm if the 1.6125V offset is still valid for PgP

// Recall that VRM11 is inverted; rounding a VID code up rounds down the
// voltage. 

int
vuv2vrm11(uint32_t v_uv, int round, uint8_t *vrm11_vid)
{
    int32_t offset, vid;

    offset = VRM11_BASE_UV - v_uv;
    vid = offset / VRM11_STEP_UV;

    if (((offset % VRM11_STEP_UV) != 0) && (round < 0)) {
        vid++;
    }

    *vrm11_vid = vid;
    return vid11_validate(vid);
}


/// Convert a VRM-11 VID code to a voltage in microvolts

int
vrm112vuv(uint8_t vrm11_vid, uint32_t *v_uv)
{
    *v_uv= VRM11_BASE_UV - (vrm11_vid * VRM11_STEP_UV);
    return vid11_validate(vrm11_vid);
}


/// Convert a voltage in microvolts to an internal VID code, rounding the
/// implied voltage as required.
///
/// \param v_uv Voltage in micro-volts
///
/// \param round \a round >= 0 indicate round voltage up, while \a round < 0
/// implies round voltage down
///
/// \param ivid A pointer to the location of the final VID code.  This
/// location is updated even the event of errors.
///
/// \retval 0 Success
///
/// \retval -IVID_INVALID_VOLTAGE If \a v_uv can not be converted to a legal
/// IVID encoding. 

int
vuv2ivid(uint32_t v_uv, int round, uint8_t *ivid)
{
    int rc;
    int32_t offset, vid;

    offset = v_uv - IVID_BASE_UV;
    vid = offset / IVID_STEP_UV;

    if (((offset % IVID_STEP_UV) != 0) && (round >= 0)) {
	    vid++;
    }
    
    *ivid = vid;
    if ((vid < 0) || (vid > 0x7f)) {
        rc = -IVID_INVALID_VOLTAGE;
    } else {
        rc = 0;
    }
    return rc;
}
    
/// Convert an iVID code to a voltage in microvolts

int
ivid2vuv(uint8_t ivid, uint32_t *v_uv)
{
    if (ivid > 0x7f) {
    return -IVID_INVALID_VOLTAGE;
    } else {
    *v_uv= IVID_BASE_UV + (ivid * IVID_STEP_UV);
    return 0;
    }
}

/// Format a voltage in microvolts as 10 microvolts into a user-supplied
/// string..  The string \a s must be able to store at least
/// FORMAT_10UV_STRLEN characters.

int
sprintf_10uv(char *s, uint32_t v_uv)
{
    return sprintf(s, "%d.%05d", v_uv / 1000000, (v_uv % 1000000) / 10);
}

#ifdef FAPIECMD

/// Format a voltage in microvolts as 10 microvolts to a stream.

int
fprintf_10uv(FILE *stream, uint32_t v_uv)
{
    int rc;
    char s[FORMAT_10UV_STRLEN];

    rc = sprintf_10uv(s, v_uv);
    if (rc > 0) {
    rc = fputs(s, stream);
    }
    return rc;
}


/// Format a VRM-11 VID code as 10 microvolts into a user-supplied string. The
/// string \a s must be able to store at least FORMAT_10UV_STRLEN characters.

int
sprintf_vrm11(char *s, uint8_t vrm11)
{
    int rc;
    uint32_t v_uv;

    if ((rc = vrm112vuv(vrm11, &v_uv)) != 0) {
        strcpy(s, FORMAT_10UV_ERROR);
    } else {
        rc = sprintf_10uv(s, v_uv);
    }
    return rc;
}


/// Format a VRM-11 VID code as 10 microvolts to a stream.

int
fprintf_vrm11(FILE *stream, uint8_t vrm11)
{
    int rc;
    char s[FORMAT_10UV_STRLEN];

    rc = sprintf_vrm11(s, vrm11);
    if (rc > 0) {
    rc = fputs(s, stream);
    }
    return rc;
}


/// Format an IVID code as 10 microvolts into a user-supplied string. The
/// string \a s must be able to store at least FORMAT_10UV_STRLEN characters.

int
sprintf_ivid(char *s, uint8_t ivid)
{
    int rc;
    uint32_t v_uv;

    if ((rc = ivid2vuv(ivid, &v_uv)) != 0) {
    return rc;
    }
    return sprintf_10uv(s, v_uv);
}


/// Format an iVID code as 10 microvolts to a stream.

int
fprintf_ivid(FILE *stream, uint8_t ivid)
{
    int rc;
    char s[FORMAT_10UV_STRLEN];

    rc = sprintf_ivid(s, ivid);
    if (rc > 0) {
    rc = fputs(s, stream);
    }
    return rc;
}


// NB: The gpst_print() routine only needs the revle* functions when compiled
// into little-endian Linux applications, which must provide their
// implementations. 

#ifdef _BIG_ENDIAN

#define revle16(x) x
#define revle32(x) x
#define revle64(x) x

#else

uint16_t revle16(uint16_t i_x);
uint32_t revle32(uint32_t i_x);
uint64_t revle64(uint64_t i_x);

#endif


/// Print a GlobalPstateTable structure on a given stream
///
/// \param stream The output stream
///
/// \param gpst The Global Pstate Table to print

void
gpst_print(FILE *stream, GlobalPstateTable *gpst)
{
    // Endian-corrected scalar Pstate fields

    uint32_t options;
    uint32_t pstate0_frequency_khz, frequency_step_khz;
    uint8_t entries, pstate_stepsize, vrm_stepdelay_range, vrm_stepdelay_value;
//    Pstate pmin, pvsafe, psafe;
    Pstate pvsafe, psafe;
    

    // Endian-corrected vector Pstate fields

    gpst_entry_t entry;

    // Other local variables

    int i;
    uint8_t evid_vdd, evid_vcs, evid_vdd_eff, evid_vcs_eff,
    maxreg_vdd, maxreg_vcs;
    int8_t pstate;
    char evid_vdd_str[FORMAT_10UV_STRLEN];
    char evid_vcs_str[FORMAT_10UV_STRLEN];
    char evid_vdd_eff_str[FORMAT_10UV_STRLEN];
    char evid_vcs_eff_str[FORMAT_10UV_STRLEN];
    char maxreg_vdd_str[FORMAT_10UV_STRLEN];
    char maxreg_vcs_str[FORMAT_10UV_STRLEN];

    // Get endian-corrected scalars

    options = revle32(gpst->options.options);
    pstate0_frequency_khz = revle32(gpst->pstate0_frequency_khz);
    frequency_step_khz = revle32(gpst->frequency_step_khz);
    entries = gpst->entries;
    pstate_stepsize = gpst->pstate_stepsize;
    vrm_stepdelay_range = gpst->vrm_stepdelay_range;
    vrm_stepdelay_value = gpst->vrm_stepdelay_value;
//    pmin = gpst->pmin;
    pvsafe = gpst->pvsafe;
    psafe = gpst->psafe;


    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
    fprintf(stream, "Global Pstate Table @ %p\n", gpst);
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
    fprintf(stream, "%d Entries from %+d to %+d\n",
        entries, gpst_pmin(gpst), gpst_pmax(gpst));
    fprintf(stream, "Frequency Step = %u KHz\n", frequency_step_khz);
    fprintf(stream, "Pstate 0 Frequency = %u KHz\n", pstate0_frequency_khz);
    fprintf(stream, "Pstate 0 Frequency Code (per core) :");
    for (i = 0; i < PGP_NCORES; i++) {
        if ((i != 0) && ((i % 4) == 0)) {
            fprintf(stream, "\n                                    ");
        }
        fprintf(stream, " 0x%03x", revle16(gpst->pstate0_frequency_code[i]));
    }
    fprintf(stream, "\n");
    fprintf(stream, "DPLL Fmax Bias (per core)          :");
    for (i = 0; i < PGP_NCORES; i++) {
        fprintf(stream, " %d", gpst->dpll_fmax_bias[i]);
    }
    fprintf(stream, "\n");
    fprintf(stream, "Pstate Step Size %u, VRM Range %u, VRM Delay %u\n",
            pstate_stepsize, vrm_stepdelay_range, vrm_stepdelay_value);
    fprintf(stream, "Pvsafe %d, Psafe %d\n", pvsafe, psafe);
    
    if (options == 0) {
        fprintf(stream, "No Options\n");
    } else {
        fprintf(stream, "Options 0x%08x:\n", options);
        if (options & PSTATE_NO_COPY_GPST) {
            fprintf(stream, "    PSTATE_NO_COPY_GPST\n");
        }
        if (options & PSTATE_NO_INSTALL_GPST) {
            fprintf(stream, "    PSTATE_NO_INSTALL_GPST\n");
        }
        if (options & PSTATE_NO_INSTALL_LPSA) {
            fprintf(stream, "    PSTATE_NO_INSTALL_LPSA\n");
        }
        if (options & PSTATE_NO_INSTALL_RESCLK) {
            fprintf(stream, "    PSTATE_NO_INSTALL_RESCLK\n");
        }
        if (options & PSTATE_FORCE_INITIAL_PMIN) {
            fprintf(stream, "    PSTATE_FORCE_INITIAL_PMIN\n");
        }
    }
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
    fprintf(stream, 
        "  I Pstate F(MHz) evid_vdd(V)  evid_vcs(V) evid_vdd_eff(V) evid_vcs_eff(V) maxreg_vdd(V)  maxreg_vcs(V)\n"
        "---------------------------------------------------------------------------------------------------------\n");

    for (i = gpst->entries - 1; i >= 0; i--) {

        entry.value = revle64(gpst->pstate[i].value);
    
    evid_vdd = entry.fields.evid_vdd;
    sprintf_vrm11(evid_vdd_str, evid_vdd);

    evid_vcs = entry.fields.evid_vcs;
    sprintf_vrm11(evid_vcs_str, evid_vcs);

    evid_vdd_eff = entry.fields.evid_vdd_eff;
    sprintf_ivid(evid_vdd_eff_str, evid_vdd_eff);

    evid_vcs_eff = entry.fields.evid_vcs_eff;
    sprintf_ivid(evid_vcs_eff_str, evid_vcs_eff);

    maxreg_vdd = entry.fields.maxreg_vdd;
    sprintf_ivid(maxreg_vdd_str, maxreg_vdd);

    maxreg_vcs = entry.fields.maxreg_vcs;
    sprintf_ivid(maxreg_vcs_str, maxreg_vcs);

    pstate = gpst_pmin(gpst) + i;

    fprintf(stream, 
        "%3d %+4d    "
        "%4d  "
        "0x%02x %s  "
        "0x%02x %s  "
        "0x%02x %s    "
        "0x%02x %s   "
        "0x%02x %s   "
        "0x%02x %s\n",
        i, pstate,
        (pstate0_frequency_khz + (frequency_step_khz * pstate)) / 1000,
        evid_vdd, evid_vdd_str,
        evid_vcs, evid_vcs_str,
        evid_vdd_eff, evid_vdd_eff_str,
        evid_vcs_eff, evid_vcs_eff_str,
        maxreg_vdd, maxreg_vdd_str,
        maxreg_vcs, maxreg_vcs_str);
    }
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
}


/// Print a LocalPstateArray structure on a given stream
///
/// \param stream The output stream
///
/// \param lpsa The Local Pstate Array to print
///
/// \todo Replace the hex dump with a decoded Local Pstate Table + ivrm table

void
lpsa_print(FILE* stream, LocalPstateArray* lpsa)
{
    int i;
    uint8_t entries;
    uint8_t entries_div4;
    char ivid_vdd_str[FORMAT_10UV_STRLEN];
    char ivid_vcs_str[FORMAT_10UV_STRLEN]; 
    uint8_t ivid_vdd, ivid_vcs;
    lpst_entry_t   lpst_entry;
    vdsvin_entry_t vdsvin_entry;

    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
    fprintf(stream, "Local Pstate Array @ %p\n", lpsa);
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
    fprintf(stream, "%d Entries from %+d to %+d\n",
        lpsa->entries, lpst_pmin(lpsa), lpst_pmax(lpsa));
    fprintf(stream, "Step Delay Rising %u, Step Delay Falling %u\n",
            lpsa->stepdelay_rising,
            lpsa->stepdelay_lowering);
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------------------\n");    
    fprintf(stream, 
        " I  ivid_vdd(V)   ivid_vcs(V)   Core vdd  Core vcs  ECO vdd   ECO vcs   ps1 inc  ps2 inc  ps3 inc  inc step  dec step\n"
        "                                pwrratio  pwrratio  pwrratio  pwrratio                                               \n"
        "---------------------------------------------------------------------------------------------------------------------\n");
        
    entries      = lpsa->entries;
    entries_div4 = entries/4;    
    
    if ( entries % 4 != 0)
       entries_div4++;
        
    for (i = entries_div4-1  ; i >= 0; i--) {    
      lpst_entry.value = revle64(lpsa->pstate[i].value);
    
      ivid_vdd = lpst_entry.fields.ivid_vdd;
      sprintf_ivid(ivid_vdd_str, ivid_vdd);

      ivid_vcs = lpst_entry.fields.ivid_vcs;
      sprintf_ivid(ivid_vcs_str, ivid_vcs);

      fprintf(stream,
           "%2u  "
           "0x%02x %s  "
           "0x%02x %s  "
           "%-9u %-9u %-9u %-9u "
           "%-8u %-8u %-8u "
           "%-9u %-9u \n",
           i,
           ivid_vdd, ivid_vdd_str,
           ivid_vcs, ivid_vcs_str,
           (uint8_t)lpst_entry.fields.vdd_core_pwrratio,
           (uint8_t)lpst_entry.fields.vcs_core_pwrratio,
           (uint8_t)lpst_entry.fields.vdd_eco_pwrratio,
           (uint8_t)lpst_entry.fields.vcs_eco_pwrratio,
           (uint8_t)lpst_entry.fields.ps1_vid_incr,
           (uint8_t)lpst_entry.fields.ps2_vid_incr,
           (uint8_t)lpst_entry.fields.ps3_vid_incr,
           (uint8_t)lpst_entry.fields.inc_step,
           (uint8_t)lpst_entry.fields.dec_step);
    }

    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------------------\n\n");   

   fprintf(stream, 
        "--------------------------------\n");
   fprintf(stream,            
        "VDS\n");    
    fprintf(stream, 
        " I    beg_offset  end_offset    \n"
        "--------------------------------\n");     

    for (i = 15  ; i >= 0; i--) {    
      vdsvin_entry.value = revle64(lpsa->vdsvin[i].value);
    
      fprintf(stream,
           "%2u    "
           "%-10u  "
           "%-10u   \n",
           i,
           (uint8_t)vdsvin_entry.fields.ivid0,
           (uint8_t)vdsvin_entry.fields.ivid1);
    }

    fprintf(stream, 
        "--------------------------------\n\n");   
        

   fprintf(stream, 
        "-----------------------------------------------------\n");
   fprintf(stream,        
        "VIN\n");    
    fprintf(stream, 
        " I    ptef0 pfet1 pfet2 pfet3 pfet4 pfet5 pfet6 pfet7\n"
        "-----------------------------------------------------\n");     

    for (i = 63  ; i >= 0; i--) {    
      vdsvin_entry.value = revle64(lpsa->vdsvin[i].value);

      fprintf(stream,
           "%2u    "
           "%-5u %-5u %-5u %-5u %-5u %-5u %-5u %-5u\n",
           i,
           (uint8_t)vdsvin_entry.fields.pfet0, 
           (uint8_t)vdsvin_entry.fields.pfet1, 
           (uint8_t)vdsvin_entry.fields.pfet2, 
           (uint8_t)vdsvin_entry.fields.pfet3, 
           (uint8_t)vdsvin_entry.fields.pfet4, 
           (uint8_t)vdsvin_entry.fields.pfet5, 
           (uint8_t)vdsvin_entry.fields.pfet6, 
           (uint8_t)vdsvin_entry.fields.pfet7);           
    }  
    
    fprintf(stream, 
        "-----------------------------------------------------\n\n");   
}    

/// Print CPM Pstate Range structure on a given stream
///
/// \param stream   The output stream
///
/// \param cpmrange The CPM Pstate Range structure to print

void
cpmrange_print(FILE* stream, CpmPstateModeRanges* cpmrange)
{
    int i;
    
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
    fprintf(stream, "CPM Pstate Range @ %p\n",  cpmrange);
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    

    fprintf(stream, "Valid Number of CPM Pstate Ranges : %u\n", 
            cpmrange->validRanges);
            
    for (i = 0; i < 8; i++) {        
      fprintf(stream, "  CPM Range %d Pstate : %d\n", 
              i, cpmrange->inflectionPoint[i]);
    }
    
    fprintf(stream, "  CPM Pmax : %d\n", 
            cpmrange->pMax);    
    
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
}

/// Print a Resonant Clocking Setup structure on a given stream
///
/// \param stream The output stream
///
/// \param resclk The ResonantClockingSetup to print

void
resclk_print(FILE* stream, ResonantClockingSetup* resclk)
{
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
    fprintf(stream, "Resonant Clocking Setup @ %p\n", resclk);
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    

    fprintf(stream, "Full Clock Sector Buffer Pstate      : %d\n", 
            resclk->full_csb_ps);
    fprintf(stream, "Low Frequency Resonant Lower Pstate  : %d\n", 
            resclk->res_low_lower_ps);
    fprintf(stream, "Low Frequency Resonant Upper Pstate  : %d\n", 
            resclk->res_low_upper_ps);
    fprintf(stream, "High Frequency Resonant Lower Pstate : %d\n", 
            resclk->res_high_lower_ps);
    fprintf(stream, "High Frequency Resonant Upper Pstate : %d\n", 
            resclk->res_high_upper_ps);
    
    fprintf(stream, 
        "---------------------------------------------------------------------------------------------------------\n");    
}    


/// Print a PstateSuperStructure on a given stream
///
/// \param stream The output stream
///
/// \param pss The PstateSuperStructure to print

void
pss_print(FILE* stream, PstateSuperStructure* pss)
{
    fprintf(stream, 
        "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");    
    fprintf(stream, "PstateSuperStructure @ %p\n", pss);
    fprintf(stream, 
        "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");    
    
    gpst_print(stream, &(pss->gpst));
    lpsa_print(stream, &(pss->lpsa));
    resclk_print(stream, &(pss->resclk));

    fprintf(stream, 
        "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");    
}

    
#endif // FAPIECMD    
