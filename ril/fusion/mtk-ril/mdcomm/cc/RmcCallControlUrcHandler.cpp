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
#include <string>
#include "RmcCallControlUrcHandler.h"
#include "RfxCdmaInfoRecData.h"
#include "RfxCdmaWaitingCallData.h"
#include "RfxCrssNotificationData.h"
#include "RfxSuppServNotificationData.h"
#include "RfxNwServiceState.h"
#include "RfxMessageId.h"
#include "RfxIntsData.h"
#include "RfxRilUtils.h"
#include "RfxStringData.h"
#include "RfxStringsData.h"
#include "RfxVoidData.h"
#include <telephony/mtk_ril.h>

#define RFX_LOG_TAG "RmcCCUrcHandler"

#define DISPLAY_TAG_BLANK            0x80
#define DISPLAY_TAG_SKIP             0x81

// register handler to channel
RFX_IMPLEMENT_HANDLER_CLASS(RmcCallControlUrcHandler, RIL_CMD_PROXY_URC);

RmcCallControlUrcHandler::RmcCallControlUrcHandler(int slot_id, int channel_id) :
        RmcCallControlBaseHandler(slot_id, channel_id) {
     const char* urc[] = {
        "+ECPI",
        "+CRING",
        "RING",
        "+ESPEECH",
        "+EAIC",
        "+ECIPH",
        "+CCWA",
        "+CLIP",
        "+CDIP",
        "+COLP",
        "+CNAP",
        "+EVOCD",
        "+EVTSTATUS",
        "+CSSI",
        "+CSSU",

        /// C2K specific start
        "NO CARRIER",
        "+REDIRNUM",
        "+LINECON",
        "+CEXTD",
        "+CFNM",
        "+CIEV: 102",
        "+CIEV: 13",
        /// C2K specific end
    };

    registerToHandleURC(urc, sizeof(urc)/sizeof(char *));
}

RmcCallControlUrcHandler::~RmcCallControlUrcHandler() {
}

void RmcCallControlUrcHandler::onHandleUrc(const sp<RfxMclMessage>& msg) {
    char *urc = msg->getRawUrc()->getLine();
    logD(RFX_LOG_TAG, "[onHandleUrc]%s", urc);
    if(strstr(urc, "+ECPI") != NULL) {
        handleCallProgressIndicationMessage(msg);
    } else if ((strstr(urc, "+CRING") != NULL) ||
               (strstr(urc, "RING") != NULL)) {
        handleRingMessage(msg);
    } else if (strstr(urc, "+ESPEECH") != NULL) {
        // AP can only look up ibt flag in +ECPI message to know if play in-band tone,
        // so donot need to handle +ESPEECH anymore.
    } else if (strstr(urc, "+EAIC") != NULL) {
        handleIncomingCallIndicationMessage(msg);
    } else if (strstr(urc, "+ECIPH") != NULL) {
        handleCipherIndicationMessage(msg);
    } else if (strstr(urc, "+CCWA") != NULL) {
        if (isCdmaRat()) {
            handleCdmaCallWaitingMessage(msg);
        } else {
            handleCrssNotification(msg, CRSS_CALL_WAITING);
        }
    } else if (strstr(urc, "+CLIP")) {
        if (isCdmaRat()) {
            handleCdmaCallingPartyNumberInfoMessage(msg);
        } else {
            handleCrssNotification(msg, CRSS_CALLING_LINE_ID_PREST);
        }
    } else if (strstr(urc, "+CDIP")) {
        handleCrssNotification(msg, CRSS_CALLED_LINE_ID_PREST);
    } else if (strstr(urc, "+COLP")) {
        handleCrssNotification(msg, CRSS_CONNECTED_LINE_ID_PREST);
    } else if (strstr(urc, "+CNAP")) {
        handleCnapMessage(msg);
    } else if (strstr(urc, "+EVOCD")) {
        handleSpeechCodecInfo(msg);
    } else if (strstr(urc, "+EVTSTATUS")) {
        handleVtStatusInfo(msg);
    } else if (strstr(urc, "+CSSI")) {
        handleSuppSvcNotification(msg, SUPP_SVC_CSSI);
    } else if (strstr(urc, "+CSSU")) {
        handleSuppSvcNotification(msg, SUPP_SVC_CSSU);
    } else
    /// C2K specific start
    if (strstr(urc, "NO CARRIER") != NULL) {
        handleNoCarrierMessage();
    } else if (strstr(urc, "+REDIRNUM") != NULL) {
        handleRedirectingNumberInfoMessage(msg);
    } else if (strstr(urc, "+LINECON") != NULL) {
        handleLineControlInfoMessage(msg);
    } else if (strstr(urc, "+CEXTD") != NULL) {
        handleExtendedDisplayInfoMessage(msg);
    } else if (strstr(urc, "+CFNM") != NULL) {
        handleDisplayAndSignalsInfoMessage(msg);
    } else if (strstr(urc, "+CIEV: 102") != NULL || strstr(urc, "+CIEV: 13") != NULL) {
        handleCallControlStatusMessage(msg);
    }
    /// C2K specific end
}

void RmcCallControlUrcHandler::handleCallProgressIndicationMessage(const sp<RfxMclMessage>& msg) {
    /* +ECPI:<call_id>, <msg_type>, <is_ibt>, <is_tch>, <dir>, <call_mode>, <number>, <type>, "<pau>", [<cause>]
     *
     * if msg_type = DISCONNECT_MSG or ALL_CALLS_DISC_MSG,
     * +ECPI:<call_id>, <msg_type>, <is_ibt>, <is_tch>,,,"",,"",<cause>
     *
     * if others,
     * +ECPI:<call_id>, <msg_type>, <is_ibt>, <is_tch>, <dir>, <call_mode>[, <number>, <toa>], ""
     */
    int maxLen = 10;
    int minLen = 9; // 1 optional data
    int ret;
    char *data[maxLen];
    RfxAtLine* line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    for (int i = 0; i < maxLen; ++i) {
        data[i] = line->atTokNextstr(&ret);
        if (ret < 0 && i < minLen) {
            logE(RFX_LOG_TAG, "ECPI: Invalid parameters");
            return;
        }
    }

    int msgType = atoi(data[1]);
    handleECPI(data);

    // notify CALL_INFO_INDICATION to TCL both for IMS or CS call, will be dispatched by TCL
    if (shoudNotifyCallInfo(msgType)) {
        //logD(RFX_LOG_TAG, "Send RFX_MSG_UNSOL_CALL_INFO_INDICATION for ECPI");
        sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CALL_INFO_INDICATION,
                    m_slot_id, RfxStringsData(data, maxLen));
        responseToTelCore(urc);
    }

    //Move to TCL
    // notify CALL_STATE_CHANGE just for CS call
    /*
    if (shouldNotifyCallStateChanged(msgType)) {
        logD(RFX_LOG_TAG, "Send RFX_MSG_UNSOL_RESPONSE_CALL_STATE_CHANGED for ECPI");
        sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_RESPONSE_CALL_STATE_CHANGED,
                    m_slot_id, RfxVoidData());
        responseToTelCore(urc);
    }
    */
}

void RmcCallControlUrcHandler::sendRingbackToneNotification(int isStart) {
    int response[1] = { 0 };

    //logD(RFX_LOG_TAG, "Stop ringback tone.");
    mIsRingBackTonePlaying = isStart;
    response[0] = mIsRingBackTonePlaying;
    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_RINGBACK_TONE,
            m_slot_id, RfxIntsData(response, 1));
    responseToTelCore(urc);
}

void RmcCallControlUrcHandler::handleRingMessage(const sp<RfxMclMessage>& msg) {
    bool isReplaceVtCall = getMclStatusManager()->getBoolValue(RFX_STATUS_KEY_REPLACE_VT_CALL, false);

    if (strStartsWith(msg->getRawUrc()->getLine(), "+CRING: VIDEO")) {
        // The flag is set in requestReplaceVtCall()
        if (!isReplaceVtCall) {
            sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_VT_RING_INFO,
                    m_slot_id, RfxVoidData());
            responseToTelCore(urc);
            //logD(RFX_LOG_TAG, "[handleRingMessage]Send RFX_MSG_UNSOL_VT_RING_INFO");
        }
    } else {
        if (!isReplaceVtCall) {
            sp<RfxMclMessage> urc_state = RfxMclMessage::obtainUrc(
                    RFX_MSG_UNSOL_RESPONSE_CALL_STATE_CHANGED, m_slot_id, RfxVoidData());
            responseToTelCore(urc_state);

            sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CALL_RING,
                    m_slot_id, RfxVoidData());
            responseToTelCore(urc);
            //logD(RFX_LOG_TAG, "[handleRingMessage]Send RFX_MSG_UNSOL_CALL_RING");
        }
    }
    // To notify SS module that CRING is received
    sendEvent(RFX_MSG_EVENT_URC_CRING_NOTIFY, RfxVoidData(), RIL_CMD_PROXY_1, m_slot_id);
}

void RmcCallControlUrcHandler::handleIncomingCallIndicationMessage(const sp<RfxMclMessage>& msg) {
    int i, ret;
    char *data[7];
    RfxAtLine* line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    for (i = 0; i < 7; i++) {
        data[i] = (char *)"";
    }

    for (i = 0; i < 5; i++) {
        data[i] = line->atTokNextstr(&ret);
        if (ret < 0) { return; }
    }

    /* Todo
    /// Enhance for DSDS behavior, reject SIM2 call while SIM1 active
    if(!incInCallNumber(rid)) {
        RejectInCallParam *param = (RejectInCallParam *)malloc(sizeof(RejectInCallParam));
        param->rid = rid;
        // EAIC: <callId>, <number>, <type>, <call mode>, <seq no>
        param->callId = data[0];
        param->seqNo = data[4];
        RIL_requestProxyTimedCallback(rejectInComingCall, param, &TIMEVAL_0,
                    getRILChannelCtx(RIL_CC, rid)->id, "rejectIncomingCall");
        return;
    }
    */

    /* Check if contains forwarding address */
    if (line->atTokHasmore()) {
        data[5] = line->atTokNextstr(&ret);
        logD(RFX_LOG_TAG, "contains forwarding address");
    }

    /* Check if contains virtual line number */
    if (line->atTokHasmore()) {
        data[6] = line->atTokNextstr(&ret);
        logD(RFX_LOG_TAG, "data[6] = %s", data[6]);
    }

    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_INCOMING_CALL_INDICATION,
            m_slot_id, RfxStringsData(data, 7));
    responseToTelCore(urc);
    //logD(RFX_LOG_TAG, "Send RFX_MSG_UNSOL_INCOMING_CALL_INDICATION");

    return;
}

void RmcCallControlUrcHandler::handleCipherIndicationMessage(const sp<RfxMclMessage>& msg) {
    int ret, i;
    int urcParamNum = 4;
    char *data[urcParamNum];
    RfxAtLine* line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret >= 0) {
        for (i = 0; i < urcParamNum; i++) {
            data[i] = line->atTokNextstr(&ret);
            if (ret < 0) {
                logE(RFX_LOG_TAG, "There is something wrong with the +ECIPH");
                return;
            }
        }
    }

    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CIPHER_INDICATION,
            m_slot_id, RfxStringsData(data, 4));
    responseToTelCore(urc);
    //logD(RFX_LOG_TAG, "Send RFX_MSG_UNSOL_CIPHER_INDICATION");
}

void RmcCallControlUrcHandler::handleCnapMessage(const sp<RfxMclMessage>& msg) {
    int urcParamNum = 2;
    int ret;
    char *data[urcParamNum];
    RfxAtLine* line = msg->getRawUrc();

    /**
     * CNAP presentaion from the network
     * +CNAP: <name>[,<CNI validity>] to the TE.
     *
     *   <name> : GSM 7bit encode
     *
     *   <CNI validity>: integer type
     *   0 CNI valid
     *   1 CNI has been withheld by the originator.
     *   2 CNI is not available due to interworking problems or limitations of originating network.
     */

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    for (int i = 0; i < urcParamNum; i++) {
        data[i] = line->atTokNextstr(&ret);
        if (ret < 0) {
            logE(RFX_LOG_TAG, "CNAP: Invalid parameters");
            return;
        }
    }

    sendEvent(RFX_MSG_EVENT_CNAP_UPDATE, RfxStringData(data[0], strlen(data[0])),
        RIL_CMD_PROXY_2, m_slot_id);
    //logD(RFX_LOG_TAG, "Send RFX_MSG_EVENT_CNAP_UPDATE");

    return;
}

void RmcCallControlUrcHandler::handleSpeechCodecInfo(const sp<RfxMclMessage>& msg) {
    int ret;
    int response[1] = { 0 };
    RfxAtLine* line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    response[0] = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_SPEECH_CODEC_INFO,
            m_slot_id, RfxIntsData(response, 1));
    responseToTelCore(urc);
    //logD(RFX_LOG_TAG, "Send RFX_MSG_UNSOL_SPEECH_CODEC_INFO type=%d", response[0]);
}

void RmcCallControlUrcHandler::handleCrssNotification(const sp<RfxMclMessage>& msg, int code) {
    int ret;
    char *data = NULL;
    RfxAtLine* line = msg->getRawUrc();
    RIL_CrssNotification crssNotify;
    int  toa = 0;

    memset(&crssNotify, 0, sizeof(RIL_CrssNotification));
    crssNotify.code = code;

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    /* Get number */
    crssNotify.number = line->atTokNextstr(&ret);
    if (ret < 0) {
        logD(RFX_LOG_TAG, "CRSS: number fail!");
        return;
    }

    /// M: CC: For 3G VT only @{
#ifdef MTK_VT3G324M_SUPPORT
    if (code == 0) {
        /* Skip type */
        line->atTokNextint(&ret);
        if (ret < 0) {
            logD(RFX_LOG_TAG, "CRSS: type fail!");
            return;
        }
        /* Get class */
        crssNotify.type = line->atTokNextint(&ret);
        if(ret < 0)
        {
            logD(RFX_LOG_TAG, "CRSS: class fail!");
        }
    } else {
#endif
    /// @}

        /* Get type */
        crssNotify.type = line->atTokNextint(&ret);
        if (ret < 0) {
            logD(RFX_LOG_TAG, "CRSS: type fail!");
            return;
        }

        if (line->atTokHasmore()) {
            /*skip subaddr*/
            line->atTokNextstr(&ret);
            if (ret < 0) {
                logD(RFX_LOG_TAG, "CRSS: subaddr fail!");
            }

            /* Get sa type */
            line->atTokNextint(&ret);
            if (ret < 0) {
                logD(RFX_LOG_TAG, "CRSS: sa type fail!");
            }

            if (line->atTokHasmore()) {
                /* Get alphaid */
                crssNotify.alphaid = line->atTokNextstr(&ret);
                if (ret < 0) {
                    logD(RFX_LOG_TAG, "CRSS: alphaid fail!");
                }

                /* Get cli_validity */
                crssNotify.cli_validity = line->atTokNextint(&ret);
                if (ret < 0) {
                    logD(RFX_LOG_TAG, "CRSS: cli validity fail!");
                }

                /*
                   Google AOSP only supports 0~3, so re-map 4 or later value to 2(UNKNOWN)
                */
                if (crssNotify.cli_validity > 3) {
                    crssNotify.cli_validity = 2;
                }
                logD(RFX_LOG_TAG, "CRSS: cli_validity = %d", crssNotify.cli_validity);
            }
        }

    /// M: CC: For 3G VT only @{
#ifdef MTK_VT3G324M_SUPPORT
    }
#endif

    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CRSS_NOTIFICATION, m_slot_id,
            RfxCrssNotificationData((void *)&crssNotify, sizeof(RIL_CrssNotification)));
    responseToTelCore(urc);
}

void RmcCallControlUrcHandler::handleVtStatusInfo(const sp<RfxMclMessage>& msg) {
    int ret;
    int response[1] = { 0 };
    RfxAtLine* line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    response[0] = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    //logD(RFX_LOG_TAG, "VT status info = %d", response[0]);

    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_VT_STATUS_INFO,
            m_slot_id, RfxIntsData(response, 1));
    responseToTelCore(urc);
}

void RmcCallControlUrcHandler::handleSuppSvcNotification(const sp<RfxMclMessage>& msg,
        int notiType) {
    RIL_SuppSvcNotification svcNotify;
    int ret;
    RfxAtLine* line = msg->getRawUrc();

    /**
     * +CSSN=[<n>[,<m>]]
     * +CSSN?  +CSSN: <n>,<m>
     * +CSSN=? +CSSN: (list of supported <n>s),(list of supported <m>s)
     */
    /**
     * When <n>=1 and a supplementary service notification is received
     * after a mobile originated call setup, intermediate result code
     * +CSSI: <code1>[,<index>] is sent to TE before any other MO call
     * setup result codes presented in the present document or in V.25ter [14].
     * When several different <code1>s are received from the network,
     * each of them shall have its own +CSSI result code.
     * <code1> (it is manufacturer specific, which of these codes are supported):
     * 0   unconditional call forwarding is active
     * 1   some of the conditional call forwardings are active
     * 2   call has been forwarded
     * 3   call is waiting
     * 4   this is a CUG call (also <index> present)
     * 5   outgoing calls are barred
     * 6   incoming calls are barred
     * 7   CLIR suppression rejected
     * 8   call has been deflected
     * <index>: refer "Closed user group +CCUG"
     */
     /**
      * When <m>=1 and a supplementary service notification is received
      * during a mobile terminated call setup or during a call, or when
      * a forward check supplementary service notification is received,
      * unsolicited result code +CSSU: <code2>[,<index>[,<number>,<type>[,<subaddr>,<satype>]]]
      * is sent to TE. In case of MT call setup, result code is sent after every +CLIP result code
      * (refer command "Calling line identification presentation +CLIP")
      * and when several different <code2>s are received from the network,
      * each of them shall have its own +CSSU result code.
      * <code2> (it is manufacturer specific, which of these codes are supported):
      * 0   this is a forwarded call (MT call setup)
      * 1   this is a CUG call (also <index> present) (MT call setup)
      * 2   call has been put on hold (during a voice call)
      * 3   call has been retrieved (during a voice call)
      * 4   multiparty call entered (during a voice call)
      * 5   call on hold has been released (this is not a SS notification) (during a voice call)
      * 6   forward check SS message received (can be received whenever)
      * 7   call is being connected (alerting) with the remote party in alerting state in explicit call transfer operation (during a voice call)
      * 8   call has been connected with the other remote party in explicit call transfer operation (also number and subaddress parameters may be present) (during a voice call or MT call setup)
      * 9   this is a deflected call (MT call setup)
      * 10  additional incoming call forwarded
      * 11  MT is a forwarded call (CF)
      * 12  MT is a forwarded call (CFU)
      * 13  MT is a forwarded call (CFC)
      * 14  MT is a forwarded call (CFB)
      * 15  MT is a forwarded call (CFNRy)
      * 16  MT is a forwarded call (CFNRc)
      * <number>: string type phone number of format specified by <type>
      * <type>: type of address octet in integer format (refer GSM 04.08 [8] subclause 10.5.4.7)
      */

    memset(&svcNotify, 0, sizeof(RIL_SuppSvcNotification));
    svcNotify.notificationType = notiType;

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    /* Get code */
    svcNotify.code = line->atTokNextint(&ret);
    if (ret < 0) {
        logD(RFX_LOG_TAG, "SUPP notification: Incorrect code.");
        return;
    }

    if (notiType == SUPP_SVC_CSSU && hasImsCall(m_slot_id)) {
        // code 2 = remote hold, code 3 = remote resume
        // IMS use ECPI 135/136, CSSU:2, 3 should be ignore.
        if (svcNotify.code == 2 || svcNotify.code == 3) {
            logD(RFX_LOG_TAG, "SUPP notificatioin: IMS ignore CSSU - remote hold / resume");
            return;
        }
    }

/*#ifdef MTK_BSP_PACKAGE
    if ((svcNotify.code >= 11) && (svcNotify.code <= 16)) {
        LOGD("convert proprietary CSSU <code2> for MT forwarded call to 0");
        svcNotify.code = 0;
    }
#endif*/

    if (line->atTokHasmore()) {
        /* Get index */
        svcNotify.index = line->atTokNextint(&ret);
        if (ret < 0) {
            logD(RFX_LOG_TAG, "SUPP notification: Incorrect index.");
        }
    }

    if (notiType == SUPP_SVC_CSSU) {
        if (line->atTokHasmore()) {
            /* Get number */
            svcNotify.number = line->atTokNextstr(&ret);
            if (ret < 0) {
                logD(RFX_LOG_TAG, "SUPP notification: Incorrect number.");
            }
        }

        if (line->atTokHasmore()) {
            svcNotify.type = line->atTokNextint(&ret);
            if (ret < 0) {
                logD(RFX_LOG_TAG, "SUPP notification: Incorrect type.");
            }
        }
    }

    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_SUPP_SVC_NOTIFICATION, m_slot_id,
            RfxSuppServNotificationData((void *)&svcNotify, sizeof(RIL_SuppSvcNotification)));
    responseToTelCore(urc);
}

void RmcCallControlUrcHandler::onHandleTimer() {
    // do something
}

void RmcCallControlUrcHandler::handleNoCarrierMessage() {
    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_RESPONSE_CALL_STATE_CHANGED,
            m_slot_id, RfxVoidData());
    responseToTelCore(urc);
}

// +CLIP:<number>,<type>,,,,<cli_validity>
void RmcCallControlUrcHandler::handleCdmaCallingPartyNumberInfoMessage(const sp<RfxMclMessage>& msg) {
    int ret;
    int numOfRecs = 0;
    int numberType, cliValidity;
    int dummy;
    char *number = NULL;
    RIL_CDMA_InformationRecords *cdmaInfo = (RIL_CDMA_InformationRecords *) alloca(
            sizeof(RIL_CDMA_InformationRecords));
    RFX_ASSERT(cdmaInfo != NULL);

    RfxAtLine *line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    number = line->atTokNextstr(&ret);
    if (ret < 0) { return; }

    numberType = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    dummy = line->atTokNextint(&ret);
    dummy = line->atTokNextint(&ret);
    dummy = line->atTokNextint(&ret);
    cliValidity = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    cdmaInfo->infoRec[numOfRecs].name = RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC;
    if (number != NULL && strlen(number) > 0 && strlen(number) < CDMA_NUMBER_INFO_BUFFER_LENGTH) {
        logD(RFX_LOG_TAG, "%s, number != NULL", __FUNCTION__);
        cdmaInfo->infoRec[numOfRecs].rec.number.len = strlen(number);
        strncpy(cdmaInfo->infoRec[numOfRecs].rec.number.buf, number,
                cdmaInfo->infoRec[numOfRecs].rec.number.len + 1);
    } else {
        logD(RFX_LOG_TAG, "%s, number is NULL or invalid", __FUNCTION__);
        cdmaInfo->infoRec[numOfRecs].rec.number.len = 0;
        strncpy(cdmaInfo->infoRec[numOfRecs].rec.number.buf, "", 1);
    }
    cdmaInfo->infoRec[numOfRecs].rec.number.number_type = ((char)numberType >> 4) & 0x07;
    cdmaInfo->infoRec[numOfRecs].rec.number.number_plan = ((char)numberType) & 0x07;
    switch (cliValidity) {
        case 0:
            cdmaInfo->infoRec[numOfRecs].rec.number.pi = 0;  // Presentation allowed
            break;
        case 1:
        case 2:
            cdmaInfo->infoRec[numOfRecs].rec.number.pi = 1;  // Presentation restricted
            break;
        default:
            cdmaInfo->infoRec[numOfRecs].rec.number.pi = 2;  // Number not available
            break;
    }
    cdmaInfo->infoRec[numOfRecs].rec.number.si = 0;  // User provided, not screened

    if (RfxRilUtils::isUserLoad() != 1) {
        logD(RFX_LOG_TAG, "RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC: len:%d, buf:%s, \
                number_type:%d, number_plan:%d, pi:%d, si:%d",
                cdmaInfo->infoRec[numOfRecs].rec.number.len,
                cdmaInfo->infoRec[numOfRecs].rec.number.buf,
                cdmaInfo->infoRec[numOfRecs].rec.number.number_type,
                cdmaInfo->infoRec[numOfRecs].rec.number.number_plan,
                cdmaInfo->infoRec[numOfRecs].rec.number.pi,
                cdmaInfo->infoRec[numOfRecs].rec.number.si);
    }

    numOfRecs++;
    cdmaInfo->numberOfInfoRecs = numOfRecs;
    logD(RFX_LOG_TAG, "RIL_UNSOL_CDMA_INFO_REC: numOfRecs:%d", numOfRecs);
    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CDMA_INFO_REC, m_slot_id,
            RfxCdmaInfoRecData((void *)cdmaInfo, sizeof(RIL_CDMA_InformationRecords)));
    responseToTelCore(urc);
}

// +REDIRNUM:<ext_bit_1>,<number_type>,<number_plan>,<ext_bit_2>,<pi>,<si>,
//           <ext_bit_3>,<redirection_reason>,<number>
void RmcCallControlUrcHandler::handleRedirectingNumberInfoMessage(const sp<RfxMclMessage>& msg) {
    int ret;
    int numOfRecs = 0;
    int ext1, numberType, numberPlan, ext2, pi, si, ext3, redirReason;
    char *number = NULL;
    RIL_CDMA_InformationRecords *cdmaInfo = (RIL_CDMA_InformationRecords *) alloca(
            sizeof(RIL_CDMA_InformationRecords));
    RFX_ASSERT(cdmaInfo != NULL);

    RfxAtLine *line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    ext1 = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    numberType = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    numberPlan = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    ext2 = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    pi = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    si = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    ext3 = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    redirReason = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    number = line->atTokNextstr(&ret);
    if (ret < 0) { return; }

    cdmaInfo->infoRec[numOfRecs].name = RIL_CDMA_REDIRECTING_NUMBER_INFO_REC;
    if (number != NULL && strlen(number) > 0 && strlen(number) < CDMA_NUMBER_INFO_BUFFER_LENGTH) {
        logD(RFX_LOG_TAG, "%s, number != NULL", __FUNCTION__);
        cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.len = strlen(number);
        strncpy(cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.buf, number,
                cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.len + 1);
    } else {
        logD(RFX_LOG_TAG, "%s, number is NULL or invalid", __FUNCTION__);
        cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.len = 0;
        strncpy(cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.buf, "", 1);
    }
    cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.number_type = (char)numberType;
    cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.number_plan = (char)numberPlan;
    cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.pi = (char)pi;
    cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.si = (char)si;
    cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingReason =
            (RIL_CDMA_RedirectingReason)redirReason;

    if (RfxRilUtils::isUserLoad() != 1) {
        logD(RFX_LOG_TAG, "RIL_CDMA_REDIRECTING_NUMBER_INFO_REC: len:%d, buf:%s, number_type:%d, \
                number_plan:%d, pi:%d, si:%d, redirectingReason:%d",
                cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.len,
                cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.buf,
                cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.number_type,
                cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.number_plan,
                cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.pi,
                cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingNumber.si,
                cdmaInfo->infoRec[numOfRecs].rec.redir.redirectingReason);
    }

    numOfRecs++;
    cdmaInfo->numberOfInfoRecs = numOfRecs;
    logD(RFX_LOG_TAG, "RIL_UNSOL_CDMA_INFO_REC: numOfRecs:%d", numOfRecs);
    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CDMA_INFO_REC, m_slot_id,
            RfxCdmaInfoRecData((void *)cdmaInfo, sizeof(RIL_CDMA_InformationRecords)));
    responseToTelCore(urc);
}

// +LINECON:<polarity_included>,<toggle_mode>,<reverse_polarity>,<power_denial_time>
void RmcCallControlUrcHandler::handleLineControlInfoMessage(const sp<RfxMclMessage>& msg) {
    int ret;
    int numOfRecs = 0;
    int polarityIncluded, toggleMode, reversePolarity, powerDenialTime;
    RIL_CDMA_InformationRecords *cdmaInfo = (RIL_CDMA_InformationRecords *) alloca(
            sizeof(RIL_CDMA_InformationRecords));
    RFX_ASSERT(cdmaInfo != NULL);

    RfxAtLine *line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    polarityIncluded = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    toggleMode = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    reversePolarity = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    powerDenialTime = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    cdmaInfo->infoRec[numOfRecs].name = RIL_CDMA_LINE_CONTROL_INFO_REC;
    cdmaInfo->infoRec[numOfRecs].rec.lineCtrl.lineCtrlPolarityIncluded = (char)polarityIncluded;
    cdmaInfo->infoRec[numOfRecs].rec.lineCtrl.lineCtrlToggle = (char)toggleMode;
    cdmaInfo->infoRec[numOfRecs].rec.lineCtrl.lineCtrlReverse = (char)reversePolarity;
    cdmaInfo->infoRec[numOfRecs].rec.lineCtrl.lineCtrlPowerDenial = (char)powerDenialTime;

    logD(RFX_LOG_TAG, "RIL_CDMA_LINE_CONTROL_INFO_REC: lineCtrlPolarityIncluded:%d, \
            lineCtrlToggle:%d, lineCtrlReverse:%d, lineCtrlPowerDenial:%d",
            cdmaInfo->infoRec[numOfRecs].rec.lineCtrl.lineCtrlPolarityIncluded,
            cdmaInfo->infoRec[numOfRecs].rec.lineCtrl.lineCtrlToggle,
            cdmaInfo->infoRec[numOfRecs].rec.lineCtrl.lineCtrlReverse,
            cdmaInfo->infoRec[numOfRecs].rec.lineCtrl.lineCtrlPowerDenial);

    numOfRecs++;
    cdmaInfo->numberOfInfoRecs = numOfRecs;
    logD(RFX_LOG_TAG, "RIL_UNSOL_CDMA_INFO_REC: numOfRecs:%d", numOfRecs);
    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CDMA_INFO_REC, m_slot_id,
            RfxCdmaInfoRecData((void *)cdmaInfo, sizeof(RIL_CDMA_InformationRecords)));
    responseToTelCore(urc);
}

// +CEXTD:<display_tag>,<info>
void RmcCallControlUrcHandler::handleExtendedDisplayInfoMessage(const sp<RfxMclMessage>& msg) {
    int ret;
    int numOfRecs = 0;
    int displayTag;
    char *info = NULL;
    RIL_CDMA_InformationRecords *cdmaInfo = (RIL_CDMA_InformationRecords *) alloca(
            sizeof(RIL_CDMA_InformationRecords));
    RFX_ASSERT(cdmaInfo != NULL);

    RfxAtLine *line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    displayTag = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    if (displayTag == DISPLAY_TAG_BLANK || displayTag == DISPLAY_TAG_SKIP) {
        return;
    }

    info = line->atTokNextstr(&ret);
    if (ret < 0) { return; }

    if (info != NULL && strlen(info) > 0 && strlen(info) < CDMA_ALPHA_INFO_BUFFER_LENGTH
            && strcmp(info, "N/A") != 0) {
        cdmaInfo->infoRec[numOfRecs].name = RIL_CDMA_EXTENDED_DISPLAY_INFO_REC;
        cdmaInfo->infoRec[numOfRecs].rec.display.alpha_len = strlen(info);
        strncpy(cdmaInfo->infoRec[numOfRecs].rec.display.alpha_buf, info,
                cdmaInfo->infoRec[numOfRecs].rec.display.alpha_len + 1);

        if (RfxRilUtils::isUserLoad() != 1) {
            logD(RFX_LOG_TAG, "RIL_CDMA_EXTENDED_DISPLAY_INFO_REC: alpha_len:%d, alpha_buf:%s",
                    cdmaInfo->infoRec[numOfRecs].rec.display.alpha_len,
                    cdmaInfo->infoRec[numOfRecs].rec.display.alpha_buf);
        }

        numOfRecs++;
        cdmaInfo->numberOfInfoRecs = numOfRecs;
        logD(RFX_LOG_TAG, "RIL_UNSOL_CDMA_INFO_REC: numOfRecs:%d", numOfRecs);
        sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CDMA_INFO_REC, m_slot_id,
                RfxCdmaInfoRecData((void *)cdmaInfo, sizeof(RIL_CDMA_InformationRecords)));
        responseToTelCore(urc);
    }
}

// +CFNM:<display>,<signal_type>,<alert_pitch>,<signal>
void RmcCallControlUrcHandler::handleDisplayAndSignalsInfoMessage(const sp<RfxMclMessage>& msg) {
    int ret;
    int numOfRecs = 0;
    int signalType, alertPitch, signal;
    char *display = NULL;
    RIL_CDMA_InformationRecords *cdmaInfo = (RIL_CDMA_InformationRecords *) alloca(
            sizeof(RIL_CDMA_InformationRecords));
    RFX_ASSERT(cdmaInfo != NULL);

    RfxAtLine *line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    display = line->atTokNextstr(&ret);
    if (ret < 0) { return; }

    if (display != NULL && strlen(display) > 0 && strlen(display) < CDMA_ALPHA_INFO_BUFFER_LENGTH
            && strcmp(display, "N/A") != 0) {
        cdmaInfo->infoRec[numOfRecs].name = RIL_CDMA_DISPLAY_INFO_REC;
        cdmaInfo->infoRec[numOfRecs].rec.display.alpha_len = strlen(display);
        strncpy(cdmaInfo->infoRec[numOfRecs].rec.display.alpha_buf, display,
                cdmaInfo->infoRec[numOfRecs].rec.display.alpha_len + 1);

        if (RfxRilUtils::isUserLoad() != 1) {
            logD(RFX_LOG_TAG, "RIL_CDMA_DISPLAY_INFO_REC: alpha_len:%d, alpha_buf:%s",
                    cdmaInfo->infoRec[numOfRecs].rec.display.alpha_len,
                    cdmaInfo->infoRec[numOfRecs].rec.display.alpha_buf);
        }

        numOfRecs++;
    } else {
        logD(RFX_LOG_TAG, "Skip since display is invalid!");
        return;
    }

    signalType = line->atTokNextint(&ret);
    if (ret < 0) { goto update; }

    alertPitch = line->atTokNextint(&ret);
    if (ret < 0) { goto update; }

    signal = line->atTokNextint(&ret);
    if (ret < 0) { goto update; }

    cdmaInfo->infoRec[numOfRecs].name = RIL_CDMA_SIGNAL_INFO_REC;
    cdmaInfo->infoRec[numOfRecs].rec.signal.isPresent = 1;
    cdmaInfo->infoRec[numOfRecs].rec.signal.signalType = (char)signalType;
    cdmaInfo->infoRec[numOfRecs].rec.signal.alertPitch = (char)alertPitch;
    cdmaInfo->infoRec[numOfRecs].rec.signal.signal = (char)signal;

    logD(RFX_LOG_TAG, "RIL_CDMA_SIGNAL_INFO_REC: signalType:%d, alertPitch:%d, signal:%d",
            cdmaInfo->infoRec[numOfRecs].rec.signal.signalType,
            cdmaInfo->infoRec[numOfRecs].rec.signal.alertPitch,
            cdmaInfo->infoRec[numOfRecs].rec.signal.signal);

    numOfRecs++;

update:
    cdmaInfo->numberOfInfoRecs = numOfRecs;
    logD(RFX_LOG_TAG, "RIL_UNSOL_CDMA_INFO_REC: numOfRecs:%d", numOfRecs);
    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CDMA_INFO_REC, m_slot_id,
            RfxCdmaInfoRecData((void *)cdmaInfo, sizeof(RIL_CDMA_InformationRecords)));
    responseToTelCore(urc);
}

// +CIEV: <type>,<value>
void RmcCallControlUrcHandler::handleCallControlStatusMessage(const sp<RfxMclMessage>& msg) {
    int type = 0, value = 0, ret = 0, urcType = 0;
    RfxAtLine *line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    type = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    value = line->atTokNextint(&ret);
    if (ret < 0) { return; }

    /**
     * 102: E911 Mode Indicator
     * value = 1: E911 Mode on
     * value = 0: E911 Mode off
     */
    if (type == 102) {
        if (value == 1) {
            urcType = RFX_MSG_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE;
        } else if (value == 0) {
            urcType = RFX_MSG_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE;
            // Notify NW module
            sendEvent(RFX_MSG_EVENT_EXIT_EMERGENCY_CALLBACK_MODE, RfxVoidData(),
                    RIL_CMD_PROXY_3, m_slot_id);
        } else if (value == 2) {
            urcType = RFX_MSG_UNSOL_NO_EMERGENCY_CALLBACK_MODE;
        }
    } else if (type == 13) {
        urcType = RFX_MSG_UNSOL_CDMA_CALL_ACCEPTED;
    }
    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(urcType, m_slot_id, RfxVoidData());
    responseToTelCore(urc);
}

void RmcCallControlUrcHandler::handleCdmaCallWaitingMessage(const sp<RfxMclMessage>& msg) {
    char *number = NULL;
    int ret = 0;
    int callType = 0;
    RIL_CDMA_CallWaiting_v6 *callWaiting =
            (RIL_CDMA_CallWaiting_v6 *) alloca(sizeof(RIL_CDMA_CallWaiting_v6));
    RFX_ASSERT(callWaiting != NULL);

    memset(callWaiting, 0, sizeof(RIL_CDMA_CallWaiting_v6));
    RfxAtLine *line = msg->getRawUrc();

    line->atTokStart(&ret);
    if (ret < 0) { return; }

    number = line->atTokNextstr(&ret);
    if (ret < 0) { return; }
    callWaiting->number = (char *) alloca(strlen(number) + 1);
    RFX_ASSERT(callWaiting->number != NULL);
    strncpy(callWaiting->number, number, strlen(number) + 1);

    callType = line->atTokNextint(&ret);
    if (ret < 0) { return; }
    if (145 == callType) {
        callWaiting->number_type = 1;
    } else if (129 == callType) {
        callWaiting->number_type = 2;
    }

    if (!strcmp(callWaiting->number, "Restricted")) {
        callWaiting->numberPresentation = 1;
    } else if (!strcmp(callWaiting->number, "UNKNOWN")
            || !strcmp(callWaiting->number, "Unknown")
            || !strcmp(callWaiting->number, "Unavailable")
            || !strcmp(callWaiting->number, "NotAvailable")
            || !strcmp(callWaiting->number, "LOOPBACK CALL")) {
        callWaiting->numberPresentation = 2;
    }
    if (callWaiting->numberPresentation != 0) {
        callWaiting->number = NULL;
    }

    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(RFX_MSG_UNSOL_CDMA_CALL_WAITING, m_slot_id,
            RfxCdmaWaitingCallData((void *)callWaiting, sizeof(RIL_CDMA_CallWaiting_v6)));
    responseToTelCore(urc);
}
/// C2K specific end

void RmcCallControlUrcHandler::handleECPI(char** data) {
    int callId = atoi(data[0]);
    int msgType = atoi(data[1]);
    int isIbt = atoi(data[2]);
    int callMode = atoi(data[5]); // 0:cs, 10:vt, 20:volte, 21:vilte, 22:volte conf

    switch (msgType) {
        case 2:
            if (isIbt == 0) {
                sendRingbackToneNotification(START_RING_BACK_TONE);
            }
            break;
        case 4:
            if (isIbt == 1) {
                if (mIsRingBackTonePlaying) {
                    sendRingbackToneNotification(STOP_RING_BACK_TONE);
                }
            }
            break;
        case 6:
            if (mIsRingBackTonePlaying) {
                sendRingbackToneNotification(STOP_RING_BACK_TONE);
            }
            break;
        case 130:
            break;
        case 133:
            if ((callId >= 1) && (callId <= MAX_GSMCALL_CONNECTIONS)) {
                sendEvent(RFX_MSG_EVENT_CLEAR_CLCCNAME, RfxIntsData((void*)&callId, sizeof(callId)),
                    RIL_CMD_PROXY_2, m_slot_id);
                //logD(RFX_LOG_TAG, "Send RFX_MSG_EVENT_CLEAR_CLCCNAME");

                // To notify SS module that ECPI133 is received
                sendEvent(RFX_MSG_EVENT_URC_ECPI133_NOTIFY,
                        RfxIntsData((void*) &callId, sizeof(callId)), RIL_CMD_PROXY_1, m_slot_id);
            }

            if (mIsRingBackTonePlaying) {
                sendRingbackToneNotification(STOP_RING_BACK_TONE);
            }
            break;
        default:
            break;
    }
}

bool RmcCallControlUrcHandler::shoudNotifyCallInfo(int msgType) {
    int supportedMsg[9] = {0, 2, 6, 130, 131, 132, 133, 135, 136};
    for (int i = 0; i < 9; ++i) {
        if (supportedMsg[i] == msgType) {
            return true;
        }
    }
    return false;
}

bool RmcCallControlUrcHandler::shouldNotifyCallStateChanged(int msgType) {
    int supportedMsg[8] = {0, 2, 6, 130, 131, 132, 133, 254};
    for (int i = 0; i < 8; ++i) {
        if (supportedMsg[i] == msgType) {
            return true;
        }
    }
    return false;
}
