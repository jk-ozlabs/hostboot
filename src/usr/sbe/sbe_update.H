/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/sbe/sbe_update.H $                                    */
/*                                                                        */
/* IBM CONFIDENTIAL                                                       */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2013                   */
/*                                                                        */
/* p1                                                                     */
/*                                                                        */
/* Object Code Only (OCO) source materials                                */
/* Licensed Internal Code Source Materials                                */
/* IBM HostBoot Licensed Internal Code                                    */
/*                                                                        */
/* The source code for this program is not published or otherwise         */
/* divested of its trade secrets, irrespective of what has been           */
/* deposited with the U.S. Copyright Office.                              */
/*                                                                        */
/* Origin: 30                                                             */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#ifndef __SBE_SBE_UPDATE_H
#define __SBE_SBE_UPDATE_H


#include <stdint.h>
#include <builtins.h>
#include <errl/errlentry.H>
#include <pnor/pnorif.H>
#include <vmmconst.h>
#include <targeting/common/targetservice.H>
#include <i2c/eepromif.H>

namespace SBE
{
    /******************************************/
    /*  Constants                             */
    /******************************************/
    // Size of CRC used for version checking
    const size_t   SBE_DATA_CRC_SIZE      = 4; // 4  bytes (32 bits)

    // Using only first 20 bytes of 64-byte sbe_image_version
    const size_t   SBE_MVPD_SHORT_IMAGE_VERSION_SIZE = 20;

    // Version of Struct stored in SEEPROM
    const uint32_t SBE_SEEPROM_STRUCT_VERSION = 0x00000001;

    // Used for locations of SBE_Version and SBE Image on a SEEPROM
    const uint64_t SBE_IMAGE_SEEPROM_ADDRESS    = 0x400; // 1KB
    const uint64_t SBE_VERSION_SEEPROM_ADDRESS  = 0x300; // 1KB - 256B

    //Used to read SBE Boot Side from processor
    const uint64_t SBE_VITAL_REG_0x0005001C = 0x005001C;
    const uint64_t SBE_BOOT_SELECT_MASK = 0x0080000000000000;

    // PNOR SBE and SBEC Partition constants
    const uint32_t MAX_SBE_ENTRIES = 9;
    const uint32_t SBETOC_EYECATCH = 0x53424500;  //'SBE\0'
    const uint64_t NONSECURE_VER_EYECATCH = 0x56455253494F4E00; //'VERSION\0'
    const uint32_t SUPPORTED_TOC_VER = 0x00000001;

    // MVPD SB Keyword contants
    const size_t  MVPD_SB_RECORD_SIZE = 49;
    const uint8_t PERMANENT_FLAG_MASK          = 0x80;
    // PERMANENT FLAG = 0 -> indicates 0 is permanent.
    const uint8_t SEEPROM_0_PERMANENT_VALUE       = 0x00;
    const uint8_t SEEPROM_1_PERMANENT_VALUE       = 0x80;
    // @todo RTC 47033 -  add support for bit 1 (0x40): temp boot side


    // Situation constants -- bits numbered from left to right
    const uint8_t SITUATION_ALL_BITS_MASK = 0xE0;
    // Bit 0: current side is permanent (=0) or temp (=1)
    const uint8_t SITUATION_CUR_IS_PERM   = 0x00;
    const uint8_t SITUATION_CUR_IS_TEMP   = 0x80;

    // Bit 1:  current_side clean (=0) or dirty (=1)
    const uint8_t SITUATION_CUR_IS_CLEAN  = 0x00;
    const uint8_t SITUATION_CUR_IS_DIRTY  = 0x40;

    // Bit 2:  alt side clean (=0) or dirty (=1)
    const uint8_t SITUATION_ALT_IS_CLEAN  = 0x00;
    const uint8_t SITUATION_ALT_IS_DIRTY  = 0x20;


    /******************************************/
    /*  Enums                                 */
    /******************************************/
    // Used for MVPD function
    enum opType_t
    {
        MVPDOP_UNKNOWN,   // Invalid
        MVPDOP_READ,      // Read version data from MVPD
        MVPDOP_WRITE,     // Write version data to MVPD
    };

    // Used to keep track of perm/temp, and cur/alt
    enum sbeSeepromSide_t
    {
        SBE_SEEPROM0,         // corresponds to EEPROM::SBE_PRIMARY
        SBE_SEEPROM1,         // corresponts to EEPROM::SBE_BACKUP
        SBE_SEEPROM_INVALID,
    };

    // Actions can be combined
    enum sbeUpdateActions_t
    {
        CLEAR_ACTIONS          = 0x00000000,
        DO_UPDATE              = 0x00000001,
        IPL_RESTART            = 0x00000002,
        CALLOUT_SBE_CODE       = 0x00000004,
        UNSUPPORTED_SITUATION  = 0x00000008,
    };


    /******************************************/
    /*  Structs                               */
    /******************************************/

    /**
     * @brief Struct containing version information stored on SBE SEEPROMs
     *
     * NOTE: For ECC purposes, this must be 8-byte aligned,
     *       so pad data if necessary
     *       struct_version 1: size = 4+64+4 = 72 (aligned)
     */
    struct sbeSeepromVersionInfo_t
    {
        uint32_t  struct_version;
        uint8_t   image_version[SBE_IMAGE_VERSION_SIZE];
        uint8_t   data_crc[SBE_DATA_CRC_SIZE];
    } PACKED;


    /**
     * @brief Struct of individual SBE entry in SBE and SBEC
     *        Table of Contents in PNOR partitions
     */
    struct sbeTocEntry_t
    {
        uint32_t ec;          // Chip EC, right aligned
        uint32_t offset;      // Offset within partition, in bytes
        uint32_t size;        // Size of image, in bytes
    } PACKED;

    /**
     * @brief Layout of Table of Contents at beginning of SBE and
     *        SBEC Partitions in PNOR
     */
    struct sbeToc_t
    {
        uint32_t eyeCatch;                      // Expected to contain 'SBE\0'
        uint32_t tocVersion;                    // Version of SBE TOC
        sbeTocEntry_t entries[MAX_SBE_ENTRIES]; // Image entries
    } PACKED;

    /**
     * @brief Layout of SB keyword in MVPD
     */
    struct mvpdSbKeyword_t
    {
        uint8_t flags;                 // 1 byte for various flags

        uint8_t seeprom_0_data_crc[SBE_DATA_CRC_SIZE];
        uint8_t seeprom_0_short_version[SBE_MVPD_SHORT_IMAGE_VERSION_SIZE];

        uint8_t seeprom_1_data_crc[SBE_DATA_CRC_SIZE];
        uint8_t seeprom_1_short_version[SBE_MVPD_SHORT_IMAGE_VERSION_SIZE];
    } PACKED;


    /**
     * @brief Contains the SBE state for a given target
     */
    struct sbeTargetState_t
    {

        // Target Information
        TARGETING::Target*       target;
        bool                     target_is_master;
        uint8_t                  ec;

        // Current Target SBE State Information
        sbe_image_version_t      pnorVersion;
        mvpdSbKeyword_t          mvpdSbKeyword;

        sbeSeepromVersionInfo_t  seeprom_0_ver;
        sbeSeepromVersionInfo_t  seeprom_1_ver;

        bool                     seeprom_0_ver_ECC_fail;
        bool                     seeprom_1_ver_ECC_fail;

        sbeSeepromSide_t         cur_seeprom_side; // aka 'booted' side
        sbeSeepromSide_t         alt_seeprom_side;
        sbeSeepromSide_t         permanent_seeprom_side;

        // Customized Image Information for this Target
        size_t                   customizedImage_size;
        uint32_t                 customizedImage_crc;

        // Update Fields and New Fields to be Written to the Target
        sbeUpdateActions_t            update_actions;
        EEPROM::eeprom_chip_types_t   seeprom_side_to_update;

        sbeSeepromVersionInfo_t       new_seeprom_ver;
        bool                          new_readBack_check;

    };



    /******************************************/
    /*  Functions -- High Level Functions     */
    /******************************************/

    /**
     * @brief Collects SBE Information for a specifc target
     *
     * @param[io/out] io_sbeState   Struct containing SBE State of the target
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t getSbeInfoState(sbeTargetState_t& io_sbeState);

    /**
     * @brief Analyze and Determine Update Actions for a specific target
     *
     * @param[io/out] io_sbeState   Struct containing SBE State of the target
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t getTargetUpdateActions(sbeTargetState_t& io_sbeState);

    /**
     * @brief Performs the Update Actions for a specific target
     *
     * @param[io/out] io_sbeState   Struct containing SBE State of the target
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t performUpdateActions(sbeTargetState_t& io_sbeState);


    /******************************************/
    /*  Functions -- System Access            */
    /******************************************/

    /**
     * @brief Read or Write SB Keyword in MVPD
     *
     * @param[in] i_target   Target processor
     *
     * @param[in] i_op      Indicates read or write
     *
     * @param[io/out] io_sb_keyword  Pointer to MVPD SBE Keyword struct
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t getSetMVPDVersion(TARGETING::Target* i_target,
                                 opType_t i_op,
                                 mvpdSbKeyword_t& io_sb_keyword);


    /**
     * @brief Read SBE Version from PNOR
     *
     * @param[in] i_pnorImgHdrPtr Pointer to Image Header in PNOR
     *
     * @param[out] o_version      Pointer to SBE Version struct
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t readPNORVersion(void*& i_pnorImgHdrPtr,
                               sbe_image_version_t& o_version);


    /**
     * @brief Customize SBE Image for current Processor
     *
     * @param[in] i_target      Target processor to customize
     *
     * @param[in] i_sbePnorPtr  Pointer to SBE Image in PNOR
     *                          Expected that this pointer was found with
     *                          findSBEInPnor() function and is past any
     *                          image header
     *
     * @param[in] i_maxImgSize  Maximum size of customized image
     *
     * @param[out] io_imgPtr    Pointer to SBE image in memory
     *                          Memory from this pointer through
     *                          i_maxImgSize is expected to be pre-alloacted
     *
     * @param[out] o_actImgSize  Actual size of the customized image
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t procCustomizeSbeImg(TARGETING::Target* i_target,
                                   void* i_sbePnorPtr,
                                   size_t i_maxImgSize,
                                   void* io_imgPtr,
                                   size_t& o_actImgSize);

    /**
     @brief Collect data for CRC, Concatenate the data, then compute the CRC
     *
     * @param[in] i_target      Target processor
     *
     * @param[out] o_data_crc   CRC computed from concatenated collected data
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t getDataCrc(TARGETING::Target* i_target,
                          uint32_t& o_data_crc);


    /**
     * @brief Determines which Seeprom was used to boot the SBE
     *
     * @param[in] i_target      Target processor to customize
     *
     * @param[out] o_bootSide   The Seeprom the SBE booted from
     *
     * @return errlHndl_t    Error log handle on failure.
     */

    errlHndl_t getSbeBootSeeprom(TARGETING::Target* i_target,
                                 sbeSeepromSide_t& o_bootSide);


    /**
     * @brief Collects Version information from a specific SEEPROM
     *
     * @param[in] i_target      Target processor to customize
     *
     * @param[in] i_seepromSide The SEEPROM on the target that to collect
     *                          the information from
     *
     * @param[out] o_info       Struct containing the SBE Version Information
     *
     * @param[out] o_seeprom_ver_ECC_fail Reports if there was an ECC erorr on
     *                                    the collected information
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t getSeepromSideVersion(TARGETING::Target* i_target,
                                     EEPROM::eeprom_chip_types_t i_seepromSide,
                                     sbeSeepromVersionInfo_t& o_info,
                                     bool& o_seeprom_ver_ECC_fail);

    /**
     * @brief Updates a specific SEEPROM
     *
     * @param[io/out] io_sbeState   Struct containing SBE State of the target
     *                              and the information to be used for updating
     *                              the SBE State of the target
     */
    errlHndl_t updateSeepromSide(sbeTargetState_t i_sbeState);



    /******************************************/
    /*  Functions -- Helper Functions         */
    /******************************************/

    /**
     * @brief Determines the Update Actions for a specific target situation
     *
     * @param[io/out] io_sbeState   Struct containing SBE State of the target
     *                              and is updated based on the situation
     *
     * @param[in] i_system_situation SBE Situation of the target that requires
     *                               analysis to determine Update Actions
     *
     * @return    void
     */
    void decisionTreeForUpdates(sbeTargetState_t& io_sbeState,
                                uint8_t i_system_situation);

    /**
     * @brief Creates the memory space needed for SBE Image Customization
     *        by allocating and setting permissions on VMM_VADDR_SBE_UPDATE
     *        area defined in vmmconst.H
     *
     * @param[in]   void
     *
     * @return      void
     */
    errlHndl_t createSbeImageVmmSpace(void);

    /**
     * @brief Cleans up memory space needed for SBE Image Customization
     *
     * @param[in]   void
     *
     * @return      void
     */
    errlHndl_t cleanupSbeImageVmmSpace(void);

    /**
     * @brief Select best cores to include in SBE Image.
     *        The output is used to indicate to p8_xip_customize which cores
     *        should be included in the SBE image.
     *
     * @param[in] i_target   Target processor to use for core selection
     *
     * @param[in] i_maxCores  Maximum number of cores to select for inclusion
     *                        in the SBE Image
     *
     * @param[out] o_coreMask  Core Mask for the target that does not exceed
     *                         the maximum number of cores
     *                         NOTE: bits 16:31 = EX0:EX15
     *                         This value is used as input into
     *                         p8_xip_customize.C
     *
     * @return errlHndl_t    Error log handle on failure.
     */
    errlHndl_t selectBestCores(TARGETING::Target* i_target,
                               size_t i_maxCores,
                               uint32_t& o_coreMask);


    /**
     * @brief Reduces bits set in a bit mask until there is a
     *        a maximum number of bits set
     *
     * @param[in] i_mask      Starting mask to be reduced
     *
     * @param[in] i_maxBits   Maximum number of bits that the
     *                        returned Bit Mask can contain
     *
     * @return uint32_t       Bit Mask
     */
    uint32_t trimBitMask(uint32_t i_mask,
                         size_t i_maxBits);


} //end namespace SBE
#endif