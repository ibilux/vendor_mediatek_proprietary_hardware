/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef ANDROID_PERFSERVICE_DEC_H
#define ANDROID_PERFSERVICE_DEC_H

#include "perfservice.h"

using namespace android;

#if 0
struct predefined_scn perf_set[] = {
    // *proj, scenarios, screen_off_action, scn_valid
    { PROJ_ALL, SCN_APP_SWITCH,      0, 1},
    { PROJ_ALL, SCN_PACKAGE_SWITCH,  0, 1},
    { PROJ_ALL, SCN_GAME_LAUNCH,     0, 1},
    { PROJ_ALL, SCN_APP_LAUNCH,      0, 1},
    { PROJ_ALL, SCN_APP_ROTATE,      0, 1},
    { PROJ_ALL, SCN_APP_TOUCH,       0, 1},
    { PROJ_ALL, SCN_SW_FRAME_UPDATE, 0, 1},
    { PROJ_ALL, SCN_GAMING,          0, 1},
    { PROJ_ALL, SCN_SW_LEVEL_BOOST,  0, 0},
    { PROJ_ALL, SCN_SPORTS_MODE,     1, 1},
    { PROJ_ALL, SCN_GALLERY_BOOST,   0, 1},
    { PROJ_ALL, SCN_PROCESS_CREATE,  0, 1},
    { PROJ_ALL, SCN_NONE,            0, 0},
    { NULL,     0, 0, 0},
};
#endif

struct predefined_app perf_app[] = {
    /* example
    * {CMD_SET_BLAH_BLAH, "com.android.blah.blah"         , 0, 0, 0, 0},
    */
    {-1, "", "", 0, 0, 0, 0},
};

#endif // ANDROID_PERFSERVICE_DEC_H
