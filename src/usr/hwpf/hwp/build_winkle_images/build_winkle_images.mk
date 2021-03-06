# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: src/usr/hwpf/hwp/build_winkle_images/build_winkle_images.mk $
#
# OpenPOWER HostBoot Project
#
# COPYRIGHT International Business Machines Corp. 2014
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
##      support for Targeting and fapi
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/ecmddatabuffer
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/hwpf/fapi
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/hwpf/plat
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/hwpf/hwp

## pointer to common HWP files
EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/include

##  NOTE: add the base istep dir here.
EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images
EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/mvpd_accessors

##  Include sub dirs
##  NOTE: add a new EXTRAINCDIR when you add a new HWP
##  EXAMPLE:
##  EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/<HWP_dir>
EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/p8_slw_build
EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/p8_set_pore_bar
EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/occ/occ_procedures
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/hwpf/hwp/utility_procedures
EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/p8_block_wakeup_intr
EXTRAINCDIR += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/proc_mailbox_utils

CFLAGS += -D __FAPI

##  NOTE: add new object files when you add a new HWP
OBJS += p8_image_help.o
OBJS += p8_scan_compression.o
OBJS += pore_inline_assembler.o
OBJS += p8_pba_bar_config.o
OBJS += sbe_xip_image.o
OBJS += p8_pore_table_static_data.o
OBJS += p8_pore_table_gen_api_fixed.o
OBJS += p8_pmc_deconfig_setup.o
OBJS += p8_poreslw_init.o
OBJS += p8_set_pore_bar.o
OBJS += p8_xip_customize.o
OBJS += p8_ring_identification.o
OBJS += p8_slw_build_fixed.o
OBJS += p8_image_help_base.o
OBJS += p8_pfet_init.o
OBJS += p8_pfet_control.o
OBJS += p8_block_wakeup_intr.o
OBJS += p8_mailbox_utils.o

##  NOTE: add a new directory onto the vpaths when you add a new HWP
##  EXAMPLE:
#   VPATH += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/<HWP_dir>
VPATH += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/p8_slw_build
VPATH += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/p8_set_pore_bar
VPATH += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/p8_block_wakeup_intr
VPATH += ${ROOTPATH}/src/usr/hwpf/hwp/build_winkle_images/proc_mailbox_utils

