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

// MTK fusion include
#include "RfxVoidData.h"

// CC local include
#include "RmcCallControlInterface.h"
#include "RmcCallControlCommonRequestHandler.h"
#include "RmcCallControlImsRequestHandler.h"

#define RFX_LOG_TAG "RmcImsCCReqHandler"

static const int requests[] = {
    RFX_MSG_REQUEST_CONFERENCE_DIAL,                     // AT+EDCONF
    RFX_MSG_REQUEST_DIAL_WITH_SIP_URI,                   // AT+CDU
    RFX_MSG_REQUEST_VT_DIAL_WITH_SIP_URI,
    RFX_MSG_REQUEST_HOLD_CALL,                           // AT+ECCTRL
    RFX_MSG_REQUEST_RESUME_CALL,
    RFX_MSG_REQUEST_ADD_IMS_CONFERENCE_CALL_MEMBER,      // AT+ECONF
    RFX_MSG_REQUEST_REMOVE_IMS_CONFERENCE_CALL_MEMBER,
    RFX_MSG_REQUEST_SET_SRVCC_CALL_CONTEXT_TRANSFER,
    RFX_MSG_REQUEST_PULL_CALL,
    RFX_MSG_REQUEST_VIDEO_CALL_ACCEPT,
    RFX_MSG_REQUEST_IMS_ECT,
    RFX_MSG_REQUEST_ASYNC_HOLD_CALL,
    RFX_MSG_REQUEST_ASYNC_RESUME_CALL,
};

RmcCallControlImsRequestHandler::RmcCallControlImsRequestHandler(
    int slot_id, int channel_id) : RmcCallControlBaseHandler(slot_id, channel_id) {
    // register to handle request
    registerToHandleRequest(requests, sizeof(requests) / sizeof(int));
}

RmcCallControlImsRequestHandler::~RmcCallControlImsRequestHandler() {
}

void RmcCallControlImsRequestHandler::onHandleTimer() {

}

void RmcCallControlImsRequestHandler::onHandleRequest(const sp<RfxMclMessage>& msg) {
    int requestId = msg->getId();
    logD(RFX_LOG_TAG, "onHandleRequest: %s", RFX_ID_TO_STR(requestId));
    switch(requestId) {
        case RFX_MSG_REQUEST_CONFERENCE_DIAL:
            confDial(msg);
            break;
        case RFX_MSG_REQUEST_DIAL_WITH_SIP_URI:
            dialWithSipUri(msg);
            break;
        case RFX_MSG_REQUEST_VT_DIAL_WITH_SIP_URI:
            dialWithSipUri(msg, true);
            break;
        case RFX_MSG_REQUEST_HOLD_CALL:
        case RFX_MSG_REQUEST_RESUME_CALL:
            controlCall(msg, requestId);
            break;
        case RFX_MSG_REQUEST_ASYNC_HOLD_CALL:
        case RFX_MSG_REQUEST_ASYNC_RESUME_CALL:
            controlCallAsync(msg, requestId);
            break;
        case RFX_MSG_REQUEST_ADD_IMS_CONFERENCE_CALL_MEMBER:
        case RFX_MSG_REQUEST_REMOVE_IMS_CONFERENCE_CALL_MEMBER:
            modifyImsConf(msg, requestId);
            break;
        case RFX_MSG_REQUEST_VIDEO_CALL_ACCEPT:
            acceptVideoCall(msg);
            break;
        case RFX_MSG_REQUEST_IMS_ECT:
            explicitCallTransfer(msg);
            break;
        case RFX_MSG_REQUEST_PULL_CALL:
            pullCall(msg);
            break;
        case RFX_MSG_REQUEST_SET_SRVCC_CALL_CONTEXT_TRANSFER:
        default:
            break;
    }
}

void RmcCallControlImsRequestHandler::confDial(const sp<RfxMclMessage>& msg) {
    /* +EDCONF=<type>,<n>,<dial_str_1>...,<clir>
     * <type>: 0 = voice; 1 = video
     * <n>: invited members count
     * <dial_str_1>...: invited members number
     * <clir>: clir mode
     */
    char** params = (char **)msg->getData()->getData();
    char* atCmd = AT_CONF_DIAL;
    int type = atoi(params[0]);
    int numOfMembers = atoi(params[1]);
    int clir = atoi(params[numOfMembers + 2]);
    char* clirPrefix = getClirPrefix(clir);

    setVDSAuto(true, false);

    String8 cmd = String8::format("%s=%d,%d", atCmd, type, numOfMembers);

    // get number sequence
    int offSet = 2;
    for (int i = 0; i < numOfMembers; ++i) {
        char* addr = params[i + offSet];
        String8 memberNum = String8::format(",\"%s%s\"", clirPrefix, addr);
        cmd += memberNum;
    }
    handleCmdWithVoidResponse(msg, cmd);
}

void RmcCallControlImsRequestHandler::dialWithSipUri(const sp<RfxMclMessage>& msg, bool isVt) {
    /* +CDU=<action>,[<uri>]
     * <atcion>: 0 = query supported URI types; 1 = dial URI
     * <uri>: optional, dialed URI
     */
    int action = 1;
    char* sipUri = (char *)msg->getData()->getData();

    char* atCmd = AT_SIP_URI_DIAL;
    String8 cmd;

    if (strncmp(sipUri, "sip:", 4) == 0) {
        cmd = String8::format("%s=%d,\"%s\"", atCmd, action, sipUri);
    } else {
        cmd = String8::format("%s=%d,\"sip:%s\"", atCmd, action, sipUri);
    }

    if (isVt) {
        cmd += String8(",1,2");
    }
    handleCmdWithVoidResponse(msg, cmd);
}

void RmcCallControlImsRequestHandler::modifyImsConf(const sp<RfxMclMessage>& msg, int request) {
    /* +ECONF=<conf_call_id>,<operation>,<num>[,<joined_call_id>]
     * <conf_call_id>: host id
     * <operation>: 0 = add; 1 = remove
     * <num>: modified call number
     * [<joined_call_id>]: optional, modified call id
     */
    char **params = (char **)msg->getData()->getData();

    char *atCmd = AT_CONF_MODIFY;
    int hostId = atoi(params[0]);
    char *callNum = params[1];
    int callId = atoi(params[2]);

    int operation = (request == RFX_MSG_REQUEST_ADD_IMS_CONFERENCE_CALL_MEMBER)? 0: 1;

    String8 cmd = String8::format("%s=%d,%d,\"%s\"", atCmd, hostId, operation, callNum);
    if (callId > 0) {
        cmd = String8::format("%s,%d", cmd.string(), callId);
    }
    handleCmdWithVoidResponse(msg, cmd);
}

void RmcCallControlImsRequestHandler::controlCall(const sp<RfxMclMessage>& msg, int request) {
    /* +ECCTRL=<call_id>,<call_state>
     * <call_id>: controled call id
     * <call_state>: 131 = hold; 132 = resume
     */
    int *params = (int *)msg->getData()->getData();

    char* atCmd = AT_IMS_CALL_CTRL;
    int callId = params[0];

    int modifier = (request == RFX_MSG_REQUEST_HOLD_CALL)? 131: 132;

    String8 cmd = String8::format("%s=%d,%d", atCmd, callId, modifier);
    sp<RfxAtResponse> atResponse = atSendCommand(cmd);

    // check at cmd result, consider default as success
    RIL_Errno result = RIL_E_SUCCESS;
    if (atResponse->getError() != 0 || atResponse->getSuccess() != 1) {
        result = RIL_E_GENERIC_FAILURE;
        if (atResponse->atGetCmeError() == CME_HOLD_FAILED_CAUSED_BY_TERMINATED) {
            result = RIL_E_ERROR_IMS_HOLD_CALL_FAILED_CALL_TERMINATED;
        }
        logE(RFX_LOG_TAG, "%d failed!", msg->getId());
    }
    // report to tcl
    responseVoidDataToTcl(msg, result);
}

void RmcCallControlImsRequestHandler::acceptVideoCall(const sp<RfxMclMessage>& msg) {
    /* +EVTA=<mode>,<call id>
     * <mode>: 0 = accept as video (TXRX) (use ATA)
     *         1 = accept as audio
     *         2 = accept as RX
     *         3 = accept as TX
     * <call id>: call id
     */
    int* params = (int*)msg->getData()->getData();
    char* atCmd = AT_ANSWER_VT_FUNCTIONAL;
    int mode = params[0];
    int callId = params[1];
    String8 cmd;
    if (mode == 0) {
        atCmd = AT_ANSWER;
        cmd = String8::format("%s", atCmd);
    } else {
        cmd = String8::format("%s=%d,%d", atCmd, mode, callId);
    }
    handleCmdWithVoidResponse(msg, cmd);
}

void RmcCallControlImsRequestHandler::pullCall(const sp<RfxMclMessage>& msg) {
    /* +ECALLPULL: <uri>,<call mode>
     * <uri>: target URI
     * <call mode>: call mode
     */
    char** params = (char**)msg->getData()->getData();
    char* atCmd = AT_PULL_CALL;
    char* targetUri = params[0];
    char* callmode = params[1];

    String8 cmd = String8::format("%s=\"%s\",%s", atCmd, targetUri, callmode);
    handleCmdWithVoidResponse(msg, cmd);
}

void RmcCallControlImsRequestHandler::explicitCallTransfer(const sp<RfxMclMessage>& msg) {
    /* +ECT=<type>,<number>
     */
    char** params = (char**)msg->getData()->getData();
    char* atCmd = AT_IMS_ECT;
    char* number = params[0];
    char* type = params[1];

    String8 cmd = String8::format("%s=%s,\"%s\"", atCmd, type, number);
    handleCmdWithVoidResponse(msg, cmd);
}

void RmcCallControlImsRequestHandler::controlCallAsync(
        const sp<RfxMclMessage>& msg, int request) {
    /* +ECCTRL=<call_id>,<call_state>, 1 (unsynchronize)
     * <call_id>: controled call id
     * <call_state>: 131 = hold; 132 = resume
     */
    int *params = (int *)msg->getData()->getData();

    char* atCmd = AT_IMS_CALL_CTRL;
    int callId = params[0];

    int modifier = (request == RFX_MSG_REQUEST_ASYNC_HOLD_CALL)? 131: 132;

    String8 cmd = String8::format("%s=%d,%d,1", atCmd, callId, modifier);
    sp<RfxAtResponse> atResponse = atSendCommand(cmd);

    // check at cmd result, consider default as success
    RIL_Errno result = RIL_E_SUCCESS;
    if (atResponse->getError() != 0 || atResponse->getSuccess() != 1) {
        result = RIL_E_GENERIC_FAILURE;
        if (atResponse->atGetCmeError() == CME_ASYNC_IMS_CALL_CONTROL) {
            result = RIL_E_ERROR_ASYNC_IMS_CALL_CONTROL_WAIT_RESULT;
        }
        logE(RFX_LOG_TAG, "%d failed!", msg->getId());
    }
    // report to tcl
    responseVoidDataToTcl(msg, result);
}
