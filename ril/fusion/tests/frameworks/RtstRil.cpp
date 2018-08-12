/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
/*****************************************************************************
 * Include
 *****************************************************************************/
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wformat"
#pragma GCC diagnostic ignored "-Wwritable-strings"
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wmacro-redefined"
//#include "../../libril/ril.cpp"
#include <cutils/sockets.h>
#include <cutils/jstring.h>
#include <telephony/record_stream.h>
#include <utils/Log.h>
#include <utils/SystemClock.h>
#include <pthread.h>
#include <binder/Parcel.h>
#include <cutils/jstring.h>
#include <sys/types.h>
#include <sys/limits.h>
#include <sys/system_properties.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <sys/un.h>
#include <assert.h>
#include <cutils/properties.h>
#include "RtstGRil.h"
#include "RfxMessageId.h"
#include <telephony/mtk_ril.h>
#include <telephony/librilutilsmtk.h>
#include "RfxDefs.h"
#include "RtstParcelUtils.h"

/*****************************************************************************
 * Name Space
 *****************************************************************************/
//using android::s_commands;
//using android::s_mtk_commands;
//using android::s_unsolResponses;
//using android::s_mtk_unsolResponses;
//using android::CommandInfo;
//using android::UnsolResponseInfo;

#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))

/*****************************************************************************
 * Static Variable
 *****************************************************************************/
namespace android {

static CommandInfo s_commands[] = {
#include "ril_commands_parcel.h"
};

static CommandInfo s_mtk_commands[] = {
#include "mtk_ril_commands_parcel.h"

};

static UnsolResponseInfo s_unsolResponses[] = {
#include "ril_unsol_commands_parcel.h"
};

static UnsolResponseInfo s_mtk_unsolResponses[] = {
#include "mtk_ril_unsol_commands_parcel.h"
};

static CommandInfo sClientCommands[] = {
//    #include "client/ril_client_commands.h"
};

static UnsolResponseInfo sClientUnsolCommands[] = {
//    #include "client/ril_client_unsol_commands.h"
};
}

using android::s_commands;
using android::s_mtk_commands;
using android::s_unsolResponses;
using android::s_mtk_unsolResponses;
using android::sClientCommands;
using android::sClientUnsolCommands;

/*****************************************************************************
 * External Function
 *****************************************************************************/
extern "C" CommandInfo *RIL_getCommandInfo(int requestId) {

    if (requestId >= RIL_REQUEST_VENDOR_BASE) {
        for (int i = 0; i < (int)NUM_ELEMS(s_mtk_commands); i++) {
            if (requestId == s_mtk_commands[i].requestNumber) {
                return &(s_mtk_commands[i]);
            }
        }
    } else {
        for (int i = 0; i < (int)NUM_ELEMS(s_commands); i++) {
            if (requestId == s_commands[i].requestNumber) {
                return &(s_commands[i]);
            }
        }
    }

    int size = sizeof(sClientCommands) / sizeof(sClientCommands[0]);
    for (unsigned int i = 0; i < size; i++) {
        if (requestId == sClientCommands[i].requestNumber) {
            return &(sClientCommands[i]);
        }
    }
    return NULL;
}

extern "C" UnsolResponseInfo *RIL_getUrcInfo(int urc) {

    if (urc >= RIL_UNSOL_VENDOR_BASE) {
        for (int i = 0; i < (int)NUM_ELEMS(s_mtk_unsolResponses); i++) {
            if (urc == s_mtk_unsolResponses[i].requestNumber) {
                return &(s_mtk_unsolResponses[i]);
            }
        }
    } else {
        for (int i = 0; i < (int)NUM_ELEMS(s_unsolResponses); i++) {
            if (urc == s_unsolResponses[i].requestNumber) {
                return &(s_unsolResponses[i]);
            }
        }
    }

    int size = sizeof(sClientUnsolCommands) / sizeof(sClientUnsolCommands[0]);
    for (unsigned int i = 0; i < size; i++) {
        if (urc == sClientUnsolCommands[i].requestNumber) {
            return &(sClientUnsolCommands[i]);
        }
    }

    return NULL;
}
