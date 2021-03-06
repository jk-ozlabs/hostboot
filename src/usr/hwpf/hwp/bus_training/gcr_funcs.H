/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/hwpf/hwp/bus_training/gcr_funcs.H $                   */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2012,2014                        */
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
// $Id: gcr_funcs.H,v 1.22 2014/03/07 13:13:29 varkeykv Exp $
// *!***************************************************************************
// *! (C) Copyright International Business Machines Corp. 1997, 1998
// *!           All Rights Reserved -- Property of IBM
// *!                   ***  ***
// *!***************************************************************************
// *! FILENAME             : gcr_funcs.H
// *! TITLE                : 
// *! DESCRIPTION          : 
// *! CONTEXT              : 
// *!
// *! OWNER  NAME          : Varghese, Varkey         Email: varkeykv@in.ibm.com
// *! BACKUP NAME          : Swaminathan, Janani      Email: jaswamin@in.ibm.com
// *!
// *!***************************************************************************
// CHANGE HISTORY: 
//------------------------------------------------------------------------------
// Version:|Author: | Date:  | Comment:
// --------|--------|--------|--------------------------------------------------
//   1.0   |jaswamin|09/13/11|
//   2.0   |varkeykv|01/12/12| Post GFW review changes
//------------------------------------------------------------------------------

#ifndef GCR_FUNCS
#define GCR_FUNCS

/* Include some system headers */
#include <list>
#include <stdint.h>
#include <fapi.H>
using namespace fapi;

#include "edi_regs.h"


enum io_interface_t {     CP_PSI,
           CP_FABRIC_X0,
           CP_FABRIC_A0, 
           CP_IOMC0_P0,  
           CP_IOMC1_P0,  
           S1_FABRIC_SX0,
           S1_FABRIC_SA0,
           CEN_DMI,
           };

// P8 chip interfaces
const uint32_t NUM_INTERFACES=21;
const char * const io_interface_name[NUM_INTERFACES] = { "CP_PSI",
                                            "CP_FABRIC_X0", 
                                            "CP_FABRIC_A0",
                                            "CP_IOMC0_P0",  
                                            "CP_IOMC1_P0",  
                                            "S1_FABRIC_SX0",
                                            "S1_FABRIC_SA0", 
                                            "CEN_DMI" };
// EDI register addresses for CP
const uint32_t ei_reg_addr_GCR_scom[NUM_INTERFACES] = { 0x00000000,
                                                  0x0401103F,
                                                  0x08010c3f, 
                                                  0x02011a3F, 
                                                  0x02011e3F,
                                                  0x03010c3f,
                                                  0x08010c3f, 
                                                  0x0201043F  };
const uint32_t ei_reg_addr_Mode_scom[NUM_INTERFACES]= { 0x00000000,
                                                  0x04011020, 
                                                  0x08010c20, 
                                                  0x02011a20, 
                                                  0x02011e20,
                                                  0x03010c20,
                                                  0x08010c20, 
                                                  0x02010420  };


// Register type
typedef enum { tx_per_lane,  tx_per_group,  tx_per_pack, tx_per_bus,  rx_per_lane,  rx_per_group,  rx_per_pack, rx_per_bus, num_register_type } register_type;

typedef enum { gcr_op_read,  gcr_op_write }  gcr_op;

     const uint32_t num_rxlanes_per_group[NUM_INTERFACES] ={1,20,24,24,24,20,24,18 };
const uint32_t num_txlanes_per_group[NUM_INTERFACES] ={1,20,24,17,17,20,24,24 };
const uint32_t num_groups_per_bus[NUM_INTERFACES] = { 1,4,1,1,1,4,1,1};

// Lane Bit Defintions
// 0x00  (lane 0),   0x01  (lane 1) , etc
const uint8_t SELECT_ALL_LANES=0x1F;          // The lane address is a 5 bit value  0b 11111  selects all lanes

// Group Bit Definitions 0x00 group 0 ,  0x01 group1 , etc
const uint8_t  RX_GROUP_BROADCAST =0x0F ;       // (Write Only) The group address is a 6 bit value to select all groups ,
const uint8_t  TX_GROUP_BROADCAST =0x2F  ;      // (Write Only) The group address is a 6 bit value to select all groups ,

// ROUTINES
//------------------------------------------------------------------------------------------------------------------------------------
// generate the 64 bit scom address for the GCR
//------------------------------------------------------------------------------------------------------------------------------------
uint64_t scom_address_64bit(uint32_t gcr_addr, uint64_t gcr_data );

//------------------------------------------------------------------------------------------------------------------------------------
// handle GCR operations - do not use directly!
// use GCR_read and GCR_write for reg access - not this function!!!!
//------------------------------------------------------------------------------------------------------------------------------------
ReturnCode doGCRop(const Target& chip_target, io_interface_t interface,
                   gcr_op read_or_write, GCR_sub_registers target_io_reg,
                   uint32_t group_address,  uint32_t lane_address,
                   ecmdDataBufferBase set_bits,  ecmdDataBufferBase clear_bits,
                   ecmdDataBufferBase &databuf_16bit, int skipCheck=0,int bypass_rmw=0);

//------------------------------------------------------------------------------------------------------------------------------------
// GCR SCOM READ  - main api for read - do not use doGCRop directly
//------------------------------------------------------------------------------------------------------------------------------------
ReturnCode  GCR_read(const Target& chip_target, io_interface_t interface,
                     GCR_sub_registers target_io_reg, uint32_t group_address,
                     uint32_t lane_address, ecmdDataBufferBase &databuf_16bit);

//------------------------------------------------------------------------------------------------------------------------------------
// GCR SCOM WRITE  - main api for write - do not use doGCRop directly
//------------------------------------------------------------------------------------------------------------------------------------
ReturnCode  GCR_write(const Target& chip_target, io_interface_t interface,
                      GCR_sub_registers target_io_reg, uint32_t group_address,
                      uint32_t lane_address,  ecmdDataBufferBase set_bits,
                      ecmdDataBufferBase clear_bits, int skipCheck=0,int bypass_rmw=0);




#endif

