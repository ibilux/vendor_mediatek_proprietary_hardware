/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2016. All rights reserved.
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

#ifndef __RFX_EMBMS_MESSAGE_ID_HEADER__
#define __RFX_EMBMS_MESSAGE_ID_HEADER__

// between RILJ and Rtc layer
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_RTC_EMBMS_ENABLE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_RTC_EMBMS_DISABLE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_RTC_EMBMS_START_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_RTC_EMBMS_STOP_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_RTC_EMBMS_SWITCH_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_RTC_EMBMS_GET_TIME)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_RTC_EMBMS_GET_COVERAGE_STATE)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RTC_EMBMS_CELL_INFO_NOTIFICATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RTC_EMBMS_COVERAGE_STATE)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RTC_EMBMS_ACTIVE_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RTC_EMBMS_AVAILABLE_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RTC_EMBMS_SAI_LIST_NOTIFICATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RTC_EMBMS_OOS_NOTIFICATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RTC_EMBMS_MODEM_EE_NOTIFICATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RTC_EMBMS_SESSION_STATUS)

// between Rtc and Rmc layer
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_EMBMS_AT_CMD)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_EMBMS_ENABLE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_EMBMS_DISABLE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_EMBMS_START_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_EMBMS_STOP_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_EMBMS_GET_TIME)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_EMBMS_SET_E911)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_EMBMS_TRIGGER_CELL_INFO_NOTIFY)
RFX_MSG_ID_EXPN(RFX_MSG_URC_EMBMS_AT_INFO)
RFX_MSG_ID_EXPN(RFX_MSG_URC_EMBMS_START_SESSION_RESPONSE)
RFX_MSG_ID_EXPN(RFX_MSG_URC_EMBMS_CELL_INFO_NOTIFICATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_EMBMS_COVERAGE_STATE)
RFX_MSG_ID_EXPN(RFX_MSG_URC_EMBMS_ACTIVE_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_EMBMS_AVAILABLE_SESSION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_EMBMS_SAI_LIST_NOTIFICATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_EMBMS_OOS_NOTIFICATION)

// between Rtc layers
RFX_MSG_ID_EXPN(RFX_MSG_EVENT_EMBMS_INITIAL_VARIABLE)
RFX_MSG_ID_EXPN(RFX_MSG_EVENT_EMBMS_POST_NETWORK_UPDATE)
RFX_MSG_ID_EXPN(RFX_MSG_EVENT_EMBMS_POST_HVOLTE_UPDATE)
RFX_MSG_ID_EXPN(RFX_MSG_EVENT_EMBMS_POST_SAI_UPDATE)
RFX_MSG_ID_EXPN(RFX_MSG_EVENT_EMBMS_POST_SESSION_UPDATE)
#endif

