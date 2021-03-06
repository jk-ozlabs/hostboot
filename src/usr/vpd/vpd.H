/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/vpd/vpd.H $                                           */
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
#ifndef __VPD_H
#define __VPD_H

#include <pnor/pnorif.H>
#include <vpd/vpd_if.H>

namespace VPD
{

/**
 * @brief This structure is used to transfer common information needed
 *      for reading the address from the PNOR RP.
 */
struct pnorInformation
{
    uint64_t segmentSize;
    uint64_t maxSegments;
    PNOR::SectionId pnorSection;
};

/**
 * @brief VPD Message Types
 */
enum VPD_MSG_TYPE
{
    VPD_WRITE_DIMM    = 0x00C1, //< DIMM SPD
    VPD_WRITE_PROC    = 0x00C2, //< Processor MVPD
    VPD_WRITE_MEMBUF  = 0x00C3, //< Centaur FRU VPD
    VPD_WRITE_PLANAR  = 0x00C4, //< Planar VPD
};

/**
 * @brief Message definition for VPD Write Request
 *
 * - data0 :
 *   - [16] VPD Record Number
 *   - [32] 4-byte ASCII String for record name
 *          'XXXX'=Entire VPD section from PNOR
 *   - [16] 2-byte ASCII String for keyword or offset into SPD
 *          'XX'=Entire VPD record
 * - data1 :
 *   - [64] Size of binary data in bytes
 * - extra data : Binary VPD Data
 */
union VpdWriteMsg_t
{
    uint64_t data0;
    struct
    {
        uint16_t rec_num;     //< VPD_REC_NUM
        char record[4];       //< ASCII Record Name
        union
        {
            char keyword[2];  //< ASCII Keyword (for IBM VPD)
            uint16_t offset;  //< Offset into record in bytes (for DIMM SPD)
        };
    } PACKED;
};


/**
 * @brief This function will query the PNOR RP to get the offset of the
 *      section requested.  It will then set global variables to save this
 *      value away for later use.  This function only needs to be called once.
 *
 * @param[in] i_pnorInfo - Structure of common PNOR information needed to
 *      query for the sections address.
 *
 * @param[in/out] io_cachedAddr - This parameter is a gloval variable in the
 *      calling code.  It is the address that is obtained from the PNOR
 *      RP.
 *
 * @param[in] i_mutex - The Mutex from the calling code.  It is locked when
 *      the globals are modified.
 *
 * @return errlHndl_t - NULL if successful, otherwise a pointer to the
 *      Error log.
*/
errlHndl_t getPnorAddr ( pnorInformation & i_pnorInfo,
                         uint64_t &io_cachedAddr,
                         mutex_t * i_mutex );

/**
 * @brief This function will read the PNOR at the correct offset and number of
 *      bytes for the keyword requested.
 *
 * @param[in] i_byteAddr - The offset to access in the PNOR.
 *
 * @param[in] i_numBytes - Number of bytes to read.
 *
 * @param[out] o_data - The data buffer that the read data will be placed
 *      into.
 *
 * @param[in] i_target - The chip target to access the data for.
 *
 * @param[in] i_pnorInfo - Information about the PNOR section that we need to
 *      know to make the request.
 *
 * @param[in/out] io_cachedAddr - The address offset to the data chunk in
 *      PNOR.
 *
 * @param[in] i_mutex - The mutex to lock/unlock while setting io_isAddrCached
 *      and io_cachedAddr.  It is assumed that those parameters are global
 *      variables in the code where they reside.
 *
 * @return errlHndl_t - NULL if successful, otherwise a pointer to the error
 *      log.
 */
errlHndl_t readPNOR ( uint64_t i_byteAddr,
                      size_t i_numBytes,
                      void * o_data,
                      TARGETING::Target * i_target,
                      pnorInformation & i_pnorInfo,
                      uint64_t &io_cachedAddr,
                      mutex_t* i_mutex );

/**
 * @brief This function will write the PNOR at the correct offset and number
 *      of bytes for the keyword requested.
 *
 * @param[in] i_byteAddr - The offset to access in the PNOR.
 *
 * @param[in] i_numBytes - The number of bytes to write.
 *
 * @param[in] i_data - The data buffer of the data to be written.
 *
 * @param[in] i_target - The chip target to access the data for.
 *
 * @param[in] i_pnorInfo - Information about the PNOR section that we need to
 *      know to make the request.
 *
 * @param[in/out] io_cachedAddr - The address offset to the data chunk in
 *      PNOR.
 *
 * @param[in] i_mutex - The mutex to lock/unlock while setting io_isAddrCached
 *      and io_cachedAddr.  It is assumed that those parameters are global
 *      variables in the code where they reside.
 *
 * @return errlHndl_t - NULL if successful, otherwise a pointer to the error
 *      log.
 */
errlHndl_t writePNOR ( uint64_t i_byteAddr,
                       size_t i_numBytes,
                       void * i_data,
                       TARGETING::Target * i_target,
                       pnorInformation & i_pnorInfo,
                       uint64_t &io_cachedAddr,
                       mutex_t* i_mutex );

/**
 * @brief This function handles sending the mailbox message to the Fsp to
 *      notify of updates to the data.
 *
 * @param[in] i_numbytes - Number of bytes to read.
 *
 * @param[in] i_data - The data buffer that will return the data read.
 *
 * @param[in] i_target - The target to access.
 *
 * @param[in] i_type - The type of VPD being written.
 *
 * @param[in] i_record - The rec_num + record/keyword.
 *
 * @return errlHndl_t - NULL if successful, otherwise a pointer to the error
 *      log.
 */
errlHndl_t sendMboxWriteMsg ( size_t i_numBytes,
                              void * i_data,
                              TARGETING::Target * i_target,
                              VPD_MSG_TYPE i_type,
                              VpdWriteMsg_t& i_record );

/**
 * @brief This function determines the VPD source (PNOR/SEEPROM)
 *      from configuration and target state information
 *
 * @param[in] i_target - Target device.
 *
 * @param[in] i_rwPnorEnabled - Config value specifying
 *      whether PNOR reads/writes are enabled for this VPD type
 *
 * @param[in] i_rwHwEnabled - Config value specifying
 *      whether SEEPROM reads/writes are enabled for this VPD type
 *
 * @param[in] i_location - The requested VPD source location
 *      (PNOR/SEEPROM) from the caller
 *
 * @param[out] o_source - The resolved VPD source to be accessed
 *
 * @return bool - True if bad config detected otherwise False
 */
bool resolveVpdSource( TARGETING::Target * i_target,
                       bool i_rwPnorEnabled,
                       bool i_rwHwEnabled,
                       vpdCmdTarget i_vpdCmdTarget,
                       vpdCmdTarget& o_vpdSource );

/**
 * @brief This function sets the correct record/keyword combinations
 *        for the part and serial numbers.
 * @param[in] i_target - target to get the data for
 * @param[in/out] io_record - record for target
 * @param[in/out] io_partKeyword - the part number keyword
 * @param[in/out] io_serialKeyword - the serial number keyword
 *
 * @return errlHndl_t - NULL if successful, otherwise a pointer to the
 *                error log.
 */
errlHndl_t getPnAndSnRecordAndKeywords( TARGETING::Target * i_target,
                                                 TARGETING::TYPE i_type,
                                                 vpdRecord & io_record,
                                                 vpdKeyword & io_partKeyword,
                                                 vpdKeyword & io_serialKeyword);

}; //end VPD namespace

#endif
