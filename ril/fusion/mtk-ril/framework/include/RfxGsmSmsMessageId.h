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

#ifndef __RFX_GSM_SMS_MESSAGE_ID_HEADER__
#define __RFX_GSM_SMS_MESSAGE_ID_HEADER__

RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_SEND_SMS)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_SEND_SMS_EXPECT_MORE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_WRITE_SMS_TO_SIM)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_DELETE_SMS_ON_SIM)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_REPORT_SMS_MEMORY_STATUS)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GET_SMS_SIM_MEM_STATUS)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_SMS_ACKNOWLEDGE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GET_SMSC_ADDRESS)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_SET_SMSC_ADDRESS)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GET_SMS_PARAMS)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_SET_SMS_PARAMS)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GSM_GET_BROADCAST_LANGUAGE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GSM_SET_BROADCAST_LANGUAGE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GSM_SMS_BROADCAST_ACTIVATION)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_SET_ETWS)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_REMOVE_CB_MESSAGE)
RFX_MSG_ID_EXPN(RFX_MSG_REQUEST_GET_GSM_SMS_BROADCAST_ACTIVATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RESPONSE_NEW_SMS)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RESPONSE_NEW_BROADCAST_SMS)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RESPONSE_ETWS_NOTIFICATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_SMS_READY_NOTIFICATION)
RFX_MSG_ID_EXPN(RFX_MSG_URC_SIM_SMS_STORAGE_FULL)
RFX_MSG_ID_EXPN(RFX_MSG_URC_ME_SMS_STORAGE_FULL)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RESPONSE_NEW_SMS_ON_SIM)
RFX_MSG_ID_EXPN(RFX_MSG_URC_RESPONSE_NEW_SMS_STATUS_REPORT)
RFX_MSG_ID_EXPN(RFX_MSG_EVENT_SMS_NEW_SMS_ERR_ACK)
RFX_MSG_ID_EXPN(RFX_MSG_EVENT_SMS_NOTIFY_SMS_READY)

#endif
