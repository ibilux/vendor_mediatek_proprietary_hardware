/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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
#include "RtcImsController.h"
#include "RfxRilUtils.h"
#include "RfxStatusDefs.h"
#include "nw/RtcRatSwitchController.h"
#include "rfx_properties.h"

/// M: add for op09 default volte setting @{
#define PROPERTY_3G_SIM          "persist.radio.simswitch"
#define PROPERTY_VOLTE_STATE     "persist.radio.volte_state"
#define PROPERTY_VOLTE_ENABLE    "persist.mtk.volte.enable"
#define IMS_CARD_TYPE_INVALID          -1
#define IMS_CARD_TYPE_NONE             0
#define READ_ICCID_RETRY_TIME      500
#define READ_ICCID_MAX_RETRY_COUNT 12

static const char PROPERTY_ICCID_SIM[4][25] = {
    "ril.iccid.sim1",
    "ril.iccid.sim2",
    "ril.iccid.sim3",
    "ril.iccid.sim4"
};
static const char PROPERTY_LAST_ICCID_SIM[4][29] = {
    "persist.radio.lastsim1_iccid",
    "persist.radio.lastsim2_iccid",
    "persist.radio.lastsim3_iccid",
    "persist.radio.lastsim4_iccid"
};
bool RtcImsController::sInitDone = false;
char RtcImsController::sLastBootIccId[4][21] = {{0}, {0}, {0}, {0}};
int RtcImsController::sLastBootVolteState = 0;
/// @}

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

#define RFX_LOG_TAG "RtcIms"

RFX_IMPLEMENT_CLASS("RtcImsController", RtcImsController, RfxController);

// register request to RfxData
/// M: add for op09 volte setting @{
RFX_REGISTER_DATA_TO_URC_ID(RfxIntsData, RIL_MSG_UNSOL_VOLTE_SETTING);
/// @}

RtcImsController::RtcImsController() :
    mNoIccidTimerHandle(NULL),
    mNoIccidRetryCount(0),
    mMainSlotId(0),
    mIsBootUp(true),
    mIsSimSwitch(false),
    mIsImsDisabling(false) {
}

RtcImsController::~RtcImsController() {
}

void RtcImsController::onInit() {
    // Required: invoke super class implementation
    RfxController::onInit();
    logD(RFX_LOG_TAG, "onInit()");

    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RADIO_STATE,
        RfxStatusChangeCallback(this, &RtcImsController::onRadioStateChanged));
    getNonSlotScopeStatusManager()->registerStatusChanged(RFX_STATUS_KEY_MAIN_CAPABILITY_SLOT,
        RfxStatusChangeCallback(this, &RtcImsController::onMainCapabilitySlotChanged));

    /// M: add for op09 default volte setting @{
    initOp09Ims();
    /// @}

    const int request_id_list[] = {
        RFX_MSG_REQUEST_IMS_REGISTRATION_STATE,
        /// M: add for op09 default volte setting @{
        RFX_MSG_REQUEST_SET_IMSCFG,
        /// @}
        RFX_MSG_REQUEST_SET_WFC_PROFILE,
        RFX_MSG_REQUEST_IMS_BEARER_ACTIVATION_DONE,
        RFX_MSG_REQUEST_IMS_BEARER_DEACTIVATION_DONE,
        RFX_MSG_REQUEST_RUN_GBA,
    };

    const int urc_id_list[] = {
        RFX_MSG_UNSOL_IMS_DISABLE_START,
        RFX_MSG_UNSOL_IMS_ENABLE_START,
        RFX_MSG_UNSOL_IMS_DISABLE_DONE,
        RFX_MSG_UNSOL_IMS_ENABLE_DONE,
        RFX_MSG_UNSOL_IMS_REGISTRATION_INFO,
        RFX_MSG_UNSOL_IMS_DEREG_DONE,
        RFX_MSG_UNSOL_GET_PROVISION_DONE,
        RFX_MSG_UNSOL_ON_USSI,
        RFX_MSG_UNSOL_ON_XUI,
        RFX_MSG_URC_IMS_BEARER_ACTIVATION,
        RFX_MSG_URC_IMS_BEARER_DEACTIVATION,
        RFX_MSG_URC_IMS_BEARER_INIT,
        RFX_MSG_UNSOL_IMS_SUPPORT_ECC,
        RFX_MSG_UNSOL_IMS_MULTIIMS_COUNT,
    };

    // register request & URC id list
    // NOTE. one id can only be registered by one controller
    registerToHandleRequest(request_id_list, sizeof(request_id_list)/sizeof(const int));
    registerToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(const int));
}

void RtcImsController::onDeinit() {
    logD(RFX_LOG_TAG, "onDeinit");
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_RADIO_STATE,
        RfxStatusChangeCallback(this, &RtcImsController::onRadioStateChanged));
    getNonSlotScopeStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_MAIN_CAPABILITY_SLOT,
        RfxStatusChangeCallback(this, &RtcImsController::onMainCapabilitySlotChanged));
    /// M: add for op09 default volte setting @{
    deinitOp09Ims();
    /// @}
    RfxController::onDeinit();
}

bool RtcImsController::onHandleRequest(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    //logD(RFX_LOG_TAG, "onHandleRequest - %s", RFX_ID_TO_STR(msg_id));
    switch (msg_id) {
    case RFX_MSG_REQUEST_IMS_REGISTRATION_STATE:
        handleImsRegStateRequest(message);
        break;
    /// M: add for op09 volte setting @{
    case RFX_MSG_REQUEST_SET_IMSCFG:
        handleSetImsConfigRequest(message);
        break;
    /// @}
    case RFX_MSG_REQUEST_SET_WFC_PROFILE:
        requestToMcl(message);
        break;
    case RFX_MSG_REQUEST_IMS_BEARER_ACTIVATION_DONE:
    case RFX_MSG_REQUEST_IMS_BEARER_DEACTIVATION_DONE:
        requestToMcl(message);
        break;
    case RFX_MSG_REQUEST_RUN_GBA:
        requestToMcl(message);
        break;
    default:
        break;
    }
    return true;
}

bool RtcImsController::onHandleUrc(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    //logD(RFX_LOG_TAG, "onHandleUrc - %s", RFX_ID_TO_STR(msg_id));
    switch (msg_id) {
        case RFX_MSG_UNSOL_IMS_DISABLE_DONE:
            handleImsDisableDoneUrc(message);
            break;
        default:
            break;
    }
    responseToRilj(message);
    return true;
}

bool RtcImsController::onHandleResponse(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    //logD(RFX_LOG_TAG, "onHandleResponse - %s", RFX_ID_TO_STR(msg_id));
    switch (msg_id) {
    case RFX_MSG_REQUEST_IMS_REGISTRATION_STATE:
        handleImsRegStateResponse(message);
        break;
    /// M: add for op09 volte setting @{
    case RFX_MSG_REQUEST_SET_IMSCFG:
        handleSetImsConfigResponse(message);
        break;
    /// @}
    case RFX_MSG_REQUEST_IMS_BEARER_ACTIVATION_DONE:
    case RFX_MSG_REQUEST_IMS_BEARER_DEACTIVATION_DONE:
    case RFX_MSG_REQUEST_SET_WFC_PROFILE:
    case RFX_MSG_REQUEST_RUN_GBA:
        responseToRilj(message);
        break;
    default:
        logD(RFX_LOG_TAG, "unknown request, ignore!");
        return false;
    }
    return true;
}

void RtcImsController::handleImsRegStateRequest(const sp<RfxMessage>& message) {
    requestToMcl(message);
}

void RtcImsController::handleImsRegStateResponse(const sp<RfxMessage>& message) {
    int isImsReg = ((int *)message->getData()->getData())[0];
    int rilDataRadioTech = 0;
    PsRatFamily ratFamily = PS_RAT_FAMILY_UNKNOWN;
    // Check SMSoIMS debug flag first
    char smsformat[RFX_PROPERTY_VALUE_MAX] = {0};
    rfx_property_get("persist.radio.smsformat.test", smsformat, "");

    if ((strlen(smsformat) == 4) && (strncmp(smsformat, "3gpp", 4) == 0)) {
        ((int *)message->getData()->getData())[1] = PS_RAT_FAMILY_GSM;
    } else if ((strlen(smsformat) == 5) &&  (strncmp(smsformat, "3gpp2", 5) == 0)) {
        ((int *)message->getData()->getData())[1] = PS_RAT_FAMILY_CDMA;
    } else {
        RfxNwServiceState defaultServiceState (0, 0, 0 ,0);
        RfxNwServiceState serviceState = getStatusManager()
            ->getServiceStateValue(RFX_STATUS_KEY_SERVICE_STATE, defaultServiceState);
        rilDataRadioTech = serviceState.getRilDataRadioTech();

        RtcRatSwitchController* ratSwitchController = (RtcRatSwitchController *) findController(
                getSlotId(), RFX_OBJ_CLASS_INFO(RtcRatSwitchController));
        ratFamily = ratSwitchController->getPsRatFamily(rilDataRadioTech);

        if (ratFamily == PS_RAT_FAMILY_IWLAN) {
            // treat IWLAN as 3GPP
            ((int *)message->getData()->getData())[1] = PS_RAT_FAMILY_GSM;
        } else {
            //PS_RAT_FAMILY_UNKNOWN = 0,
            //PS_RAT_FAMILY_GSM = 1,
            //PS_RAT_FAMILY_CDMA = 2,
            ((int *)message->getData()->getData())[1] = ratFamily;
        }
    }

    int format = ((int *)message->getData()->getData())[1];

    logD(RFX_LOG_TAG, "rilDataRadioTech=%d, smsformat=%s, ratFamily=%d, isImsReg=%d, format=%d",
            rilDataRadioTech, smsformat, ratFamily, isImsReg, format);

    responseToRilj(message);
}

void RtcImsController::onRadioStateChanged(RfxStatusKeyEnum key,
        RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RFX_UNUSED(old_value);
    RFX_UNUSED(value);
    //RIL_RadioState radioState = (RIL_RadioState) value.asInt();

    //logD(RFX_LOG_TAG, "onRadioStateChanged, radioState=%d", radioState);
}

bool RtcImsController::responseToRilj(const sp<RfxMessage>& msg) {
    return RfxController::responseToRilj(msg);
}

bool RtcImsController::onCheckIfRejectMessage(
        const sp<RfxMessage>& message, bool isModemPowerOff, int radioState) {

    char prop_value[RFX_PROPERTY_VALUE_MAX] = {0};
    rfx_property_get("persist.mtk_wfc_support", prop_value, "0");
    int isWfcSupport = atoi(prop_value);

    //logD(RFX_LOG_TAG, "onCheckIfRejectMessage, id = %d, isModemPowerOff = %d, radioState = %d, msg = %s",
            //message->getId(), isModemPowerOff, radioState, idToString(message->getId()));

    // The RIL Request should be bypass anyway
    if (isWfcSupport == 1 && !isModemPowerOff) {
        //default dont need to log false case since this value will not cause this msg to be rejected
        //logD(RFX_LOG_TAG, "onCheckIfRejectMessage ret=false: isWfcSupport=%d, isModemPowerOff=%d", isWfcSupport, isModemPowerOff);
        return false;
    }

    int msgId = message->getId();
    if((radioState == (int)RADIO_STATE_UNAVAILABLE ||
        radioState == (int)RADIO_STATE_OFF) &&
            (msgId == RFX_MSG_REQUEST_SET_IMSCFG ||
             msgId == RFX_MSG_REQUEST_IMS_BEARER_ACTIVATION_DONE ||
             msgId == RFX_MSG_REQUEST_IMS_BEARER_DEACTIVATION_DONE ||
             msgId == RFX_MSG_REQUEST_RUN_GBA ||
             msgId == RFX_MSG_REQUEST_SET_WFC_PROFILE)) {
        //default dont need to log false case since this value will not cause this msg to be rejected
        //logD(RFX_LOG_TAG, "onCheckIfRejectMessage ret=false: radioState=%d, msgId=%d", radioState, msgId);
        return false;
    }
    //others will print in RfxController::onCheckIfRejectMessage
    return RfxController::onCheckIfRejectMessage(message, isModemPowerOff, radioState);
}

/// M: add for op09 volte setting @{
void RtcImsController::initOp09Ims() {
    if (!RfxRilUtils::isCtVolteSupport()) {
        return;
    }

    int setValue;
    char tempstr[RFX_PROPERTY_VALUE_MAX] = { 0 };
    property_get(PROPERTY_3G_SIM, tempstr, "1");
    int mainSlotId = atoi(tempstr) - 1;

    char volteEnable[RFX_PROPERTY_VALUE_MAX] = { 0 };
    property_get(PROPERTY_VOLTE_ENABLE, volteEnable, "0");
    int volteValue = atoi(volteEnable);
    logD(RFX_LOG_TAG, "initOp09Ims: volteEnable = %d, mainSlotId = %d", volteValue, mainSlotId);

    for (int slotCheckingBit = 0; slotCheckingBit < RFX_SLOT_COUNT; slotCheckingBit++) {
        if (slotCheckingBit == mainSlotId) {
            setValue = volteValue;
        } else {
            setValue = 0;
        }
    }

    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_CARD_TYPE,
        RfxStatusChangeCallback(this, &RtcImsController::onCardTypeChanged));

    if (sInitDone == false) {
        for (int slot = 0; slot < RFX_SLOT_COUNT; slot++) {
            property_get(PROPERTY_LAST_ICCID_SIM[slot], sLastBootIccId[slot], "null");
        }
        char tempstr[RFX_PROPERTY_VALUE_MAX] = { 0 };
        property_get(PROPERTY_VOLTE_STATE, tempstr, "0");
        sLastBootVolteState = atoi(tempstr);
        logD(RFX_LOG_TAG, "initOp09Ims, sLastBootVolteState = %d", sLastBootVolteState);
        sInitDone = true;
    }
}

void RtcImsController::deinitOp09Ims() {
    if (!RfxRilUtils::isCtVolteSupport()) {
        return;
    }
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_CARD_TYPE,
        RfxStatusChangeCallback(this, &RtcImsController::onCardTypeChanged));
}

void RtcImsController::onMainCapabilitySlotChanged(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant new_value) {
    RFX_UNUSED(key);
    if (RfxRilUtils::isMultipleImsSupport() == 0)  {
        // query IMS registration status again after SIM switch for single ims
        updateImsRegStatus();
    }

    if (!RfxRilUtils::isCtVolteSupport()) {
        return;
    }
    int oldType = old_value.asInt();
    int newType = new_value.asInt();
    logD(RFX_LOG_TAG, "onMainCapabilitySlotChanged()  cap %d->%d, mMainSlotId:%d",
        oldType, newType, mMainSlotId);

    char tempstr[RFX_PROPERTY_VALUE_MAX] = { 0 };
    property_get(PROPERTY_3G_SIM, tempstr, "1");
    int mainSlotId = atoi(tempstr) - 1;
    int curSlotId = getSlotId();

    if (RfxRilUtils::isMultipleImsSupport() == 0) {
        if (mMainSlotId != mainSlotId) {
            if (!RfxRilUtils::isOp09() && mainSlotId != curSlotId) {
                // For om project, only do the check for main capability sim.
                // Because om's design only need check CT card first insert or switch,
                // other operator sim will not set default volte setting.
                mMainSlotId = mainSlotId;
                return;
            }
            char newIccIdStr[RFX_PROPERTY_VALUE_MAX] = { 0 };
            property_get(PROPERTY_ICCID_SIM[curSlotId], newIccIdStr, "");
            logD(RFX_LOG_TAG, "onMainCapabilitySlotChanged, newIccIdStr = %s",
                 givePrintableStr(newIccIdStr));
            if (strlen(newIccIdStr) == 0 || strcmp("N/A", newIccIdStr) == 0) {
                logD(RFX_LOG_TAG, "onMainCapabilitySlotChanged(), iccid not ready");
                // iccid change case, cover by onCardTypeChanged()
                // also set mMainSlotId in onCardTypeChanged()
            } else {
                mMainSlotId = mainSlotId;
                mIsSimSwitch = true;
                int card_type = getStatusManager()->getIntValue(RFX_STATUS_KEY_CARD_TYPE);
                setDefaultVolteState(curSlotId, newIccIdStr, card_type);
            }
        }
    } else {
        // Add for CT VoLTE v2.
        char curIccIdStr[RFX_PROPERTY_VALUE_MAX] = { 0 };
        property_get(PROPERTY_ICCID_SIM[curSlotId], curIccIdStr, "");
        if (strlen(curIccIdStr) == 0 || strcmp("N/A", curIccIdStr) == 0) {
            logD(RFX_LOG_TAG, "onMainCapabilitySlotChanged(), iccid not ready");
            return;
        }
        mMainSlotId = mainSlotId;
        // Only update non-main slot
        if (mMainSlotId != curSlotId) {
            // CT+CT, subordinate 4g card shall set volte on
            if (strlen(curIccIdStr) != 0 &&
                isOp09SimCard(curSlotId, curIccIdStr, 0)) {
                char peerIccIdStr[RFX_PROPERTY_VALUE_MAX] = { 0 };
                int peerSlotId = mMainSlotId;
                property_get(PROPERTY_ICCID_SIM[peerSlotId], peerIccIdStr, "");
                if (strlen(peerIccIdStr) != 0
                        && isOp09SimCard(peerSlotId, peerIccIdStr, 0)) {
                    // dual OP09 SIM, need update
                    setVolteStateProperty(curSlotId, 1);
                    setVolteStateProperty(peerSlotId, 0);
                    // send volte state to IMS FW
                    sendDefaultVolteStateUrc(curSlotId, 1);
                    sendDefaultVolteStateUrc(peerSlotId, 0);
                }
            }
        }
    }
}

void RtcImsController::onCardTypeChanged(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    // main slot status only update after switching finish, so must use property.
    char tempstr[RFX_PROPERTY_VALUE_MAX] = { 0 };
    property_get(PROPERTY_3G_SIM, tempstr, "1");
    int mainSlotId = atoi(tempstr) - 1;
    int curSlotId = getSlotId();

    if (mainSlotId != mMainSlotId) {
        mMainSlotId = mainSlotId;
        mIsSimSwitch = true;
    }
    int old_type = old_value.asInt();
    int new_type = value.asInt();
    logD(RFX_LOG_TAG, "onCardTypeChanged, old_type is %d, new_type is %d, mMainSlotId = %d",
         old_type, new_type, mMainSlotId);

    // if old_type == -1 and new_type == 0, it is bootup without sim.
    // if old_type == -1 and new_type > 0, it is bootup with sim or sim switch finish.
    // if old_type == 0 and new_type > 0, it is plug in sim.
    // if old_type == 0 and new_type == 0, it is still no sim.
    // if old_type > 0 and new_type == -1, it is sim switch start, no need deal yet.
    if (old_type == IMS_CARD_TYPE_INVALID || old_type == IMS_CARD_TYPE_NONE) {
        if (new_type == IMS_CARD_TYPE_NONE) {
            // no sim inserted case
            property_set(PROPERTY_LAST_ICCID_SIM[curSlotId], "null");
            mIsBootUp = false;
            mIsSimSwitch = false;
        } else if (new_type > 0) {
            if (!RfxRilUtils::isOp09() &&
                RfxRilUtils::isMultipleImsSupport() == 0 &&
                mMainSlotId != curSlotId) {
                // For om project, only do the check for main capability sim.
                // Because om's design only need check CT card first insert or switch,
                // other operator sim will not set default volte setting.
                return;
            }
            char newIccIdStr[RFX_PROPERTY_VALUE_MAX] = { 0 };
            property_get(PROPERTY_ICCID_SIM[curSlotId], newIccIdStr, "");
            logD(RFX_LOG_TAG, "onCardTypeChanged, newIccIdStr = %s",
                 givePrintableStr(newIccIdStr));
            if (strlen(newIccIdStr) == 0 || strcmp("N/A", newIccIdStr) == 0) {
                logD(RFX_LOG_TAG, "onCardTypeChanged, iccid not ready");
                // start timer waiting icc id loaded
                if (mNoIccidTimerHandle != NULL) {
                    RfxTimer::stop(mNoIccidTimerHandle);
                }
                mNoIccidRetryCount = 0;
                mNoIccidTimerHandle = RfxTimer::start(RfxCallback0(this,
                            &RtcImsController::onNoIccIdTimeout), ms2ns(READ_ICCID_RETRY_TIME));
            } else {
                setDefaultVolteState(curSlotId, newIccIdStr, new_type);
            }
        }
    } else {
        // if old_type > 0 and new_type == 0, it is plug out sim.
        if (new_type == IMS_CARD_TYPE_NONE) {
            property_set(PROPERTY_LAST_ICCID_SIM[curSlotId], "null");
            mIsBootUp = false;
        }

        mIsSimSwitch = false;
    }
}

void RtcImsController::onNoIccIdTimeout() {
    mNoIccidTimerHandle = NULL;
    char newIccIdStr[RFX_PROPERTY_VALUE_MAX] = { 0 };
    int slotId = getSlotId();
    property_get(PROPERTY_ICCID_SIM[slotId], newIccIdStr, "");
    logD(RFX_LOG_TAG, "onNoIccIdTimeout, newIccIdStr = %s", givePrintableStr(newIccIdStr));

    if (strlen(newIccIdStr) == 0 || strcmp("N/A", newIccIdStr) == 0) {
        // if still fail, need revise timer
        if (mNoIccidRetryCount < READ_ICCID_MAX_RETRY_COUNT &&
            mNoIccidTimerHandle == NULL) {
            mNoIccidRetryCount++;
            mNoIccidTimerHandle = RfxTimer::start(RfxCallback0(this,
                            &RtcImsController::onNoIccIdTimeout), ms2ns(READ_ICCID_RETRY_TIME));
        }
    } else {
        int newType = getStatusManager(slotId)->getIntValue(
                RFX_STATUS_KEY_CARD_TYPE, IMS_CARD_TYPE_NONE);
        setDefaultVolteState(slotId, newIccIdStr, newType);
    }
}

void RtcImsController::setDefaultVolteState(int slot_id, char new_iccid[], int card_type) {
    char oldIccIdStr[RFX_PROPERTY_VALUE_MAX] = { 0 };
    // Todo: multi ims need save every sim iccid
    property_get(PROPERTY_LAST_ICCID_SIM[slot_id], oldIccIdStr, "null");
    logD(RFX_LOG_TAG, "setDefaultVolteState, oldIccIdStr = %s", givePrintableStr(oldIccIdStr));

    if (RfxRilUtils::isMultipleImsSupport() == 0) {
        if (strlen(oldIccIdStr) == 0 || strcmp("null", oldIccIdStr) == 0
         || strcmp(new_iccid, oldIccIdStr) != 0
         || (RfxRilUtils::isOp09() && mIsSimSwitch)) {
            property_set(PROPERTY_LAST_ICCID_SIM[slot_id], new_iccid);

            int setValue = 1;
            if (isOp09SimCard(slot_id, new_iccid, card_type)) {
                setValue = 0;
            }

            bool isNewSim = true;
            if (mIsBootUp) {
                for (int slot = 0; slot < RFX_SLOT_COUNT; slot++) {
                    if (strcmp(new_iccid, sLastBootIccId[slot]) == 0) {
                        // For switch sim when powoff case, boot up need get before setting
                        int state = sLastBootVolteState;
                        setValue = (state >> slot) & 1;
                        isNewSim = false;
                        logD(RFX_LOG_TAG,
                            "setDefaultVolteState, change sim slot reboot, last= %d, %d",
                            state, setValue);
                        break;
                    }
                }
                mIsBootUp = false;
            }

            // only OM new non-CT SIM card no need set as default.
            // Other case need set as default.
            if (!RfxRilUtils::isOp09() &&
                !isOp09SimCard(slot_id, new_iccid, card_type) &&
                isNewSim) {
                return;
            }

            if (mIsSimSwitch && (strcmp(new_iccid, oldIccIdStr) == 0)) {
                char volteState[RFX_PROPERTY_VALUE_MAX] = { 0 };
                property_get(PROPERTY_VOLTE_STATE, volteState, "0");
                int stateValue = atoi(volteState);
                setValue = (stateValue >> slot_id) & 1;
                logD(RFX_LOG_TAG, "setDefaultVolteState, sim switch, setValue = %d", setValue);
            } else {
                setVolteStateProperty(slot_id, setValue);
            }
            mIsSimSwitch = false;

            char volteEnable[RFX_PROPERTY_VALUE_MAX] = { 0 };
            property_get(PROPERTY_VOLTE_ENABLE, volteEnable, "0");
            int enableValue = atoi(volteEnable);
            // for single ims, only set volte state and send request for main capability sim.
            if (mMainSlotId == slot_id) {
                // state diff, send volte state to IMS FW
                sendDefaultVolteStateUrc(slot_id, setValue);
            }
        }
    } else {
        if (strlen(oldIccIdStr) == 0 || strcmp("null", oldIccIdStr) == 0
         || strcmp(new_iccid, oldIccIdStr) != 0) {
            property_set(PROPERTY_LAST_ICCID_SIM[slot_id], new_iccid);

            int setValue = 1;
            if (isOp09SimCard(slot_id, new_iccid, card_type)) {
                setValue = 0;
            }

            bool isNewSim = true;
            if (mIsBootUp) {
                for (int slot = 0; slot < RFX_SLOT_COUNT; slot++) {
                    if (strcmp(new_iccid, sLastBootIccId[slot]) == 0) {
                        // For switch sim when powoff case, boot up need get before setting
                        int state = sLastBootVolteState;
                        setValue = (state >> slot) & 1;
                        isNewSim = false;
                        logD(RFX_LOG_TAG,
                            "setDefaultVolteState, change sim slot reboot, last= %d, %d",
                            state, setValue);
                        break;
                    }
                }
                mIsBootUp = false;
            }

            // only OM new non-CT SIM card no need set as default.
            // Other case need set as default.
            if (!RfxRilUtils::isOp09() &&
                !isOp09SimCard(slot_id, new_iccid, card_type) &&
                isNewSim) {
                return;
            }

            // Add for CT VoLTE v2.
            // CT+CT, subordinate 4g card shall set volte on
            int switchState = getNonSlotScopeStatusManager()->getIntValue(
                                  RFX_STATUS_KEY_CAPABILITY_SWITCH_STATE);
            int dataSim = getNonSlotScopeStatusManager()->getIntValue(
                              RFX_STATUS_KEY_DEFAULT_DATA_SIM);
            if (switchState != CAPABILITY_SWITCH_STATE_IDLE) {
                logD(RFX_LOG_TAG, "setDefaultVolteState, do switching...");
            } else if (dataSim != -1 && dataSim != mMainSlotId) {
                // For CT+CT, main sim will same to data sim.
                logD(RFX_LOG_TAG, "setDefaultVolteState, dataSim != mainSim");
            } else if (isOp09SimCard(slot_id, new_iccid, card_type)) {
                char peerIccIdStr[RFX_PROPERTY_VALUE_MAX] = { 0 };
                int peerSlotId = (slot_id == 0 ? 1 : 0);
                property_get(PROPERTY_ICCID_SIM[peerSlotId], peerIccIdStr, "");
                if (strlen(peerIccIdStr) != 0
                        && isOp09SimCard(peerSlotId, peerIccIdStr, 0)) {
                    if (mMainSlotId == slot_id) {
                        setVolteStateProperty(peerSlotId, 1);
                        // send volte state to IMS FW
                        sendDefaultVolteStateUrc(peerSlotId, 1);
                        logD(RFX_LOG_TAG, "setDefaultVolteState, peer = %d", peerSlotId);
                        setValue = 0;
                    } else {
                        setValue = 1;
                    }
                }
            }

            setVolteStateProperty(slot_id, setValue);

            // send volte state to IMS FW
            sendDefaultVolteStateUrc(slot_id, setValue);
        }
    }
}

void RtcImsController::setVolteStateProperty(int slot_id, bool isEnable) {
    char volteState[RFX_PROPERTY_VALUE_MAX] = { 0 };
    property_get(PROPERTY_VOLTE_STATE, volteState, "0");
    int stateValue = atoi(volteState);

    if (isEnable == 1) {
        stateValue = stateValue | (1 << slot_id);
    } else {
        stateValue = stateValue & (~(1 << slot_id));
    }
    char temp[3] = {0};
    if (stateValue > 10) {
        temp[0] = '1';
        temp[1] = '0' + (stateValue - 10);
    } else {
        temp[0] = '0' + stateValue;
    }
    property_set(PROPERTY_VOLTE_STATE, temp);
    logD(RFX_LOG_TAG, "setVolteStateProperty, new volte_state = %d, %s", stateValue,
        temp);
}

bool RtcImsController::isOp09SimCard(int slot_id, char icc_id[], int card_type) {
    RFX_UNUSED(slot_id);
    RFX_UNUSED(card_type);
    bool isOp09Card = false;
    if (strncmp(icc_id, "898603", 6) == 0 ||
        strncmp(icc_id, "898611", 6) == 0 ||
        strncmp(icc_id, "8985302", 7) == 0 ||
        strncmp(icc_id, "8985307", 7) == 0) {
        isOp09Card = true;
    }
    return isOp09Card;
}

void RtcImsController::sendDefaultVolteStateUrc(int slot_id, int value) {
    // send default volte state to ims ril adapter
    int response[2];
    response[0] = value;
    response[1] = slot_id;
    sp<RfxMessage> msg = RfxMessage::obtainUrc(
            slot_id,
            RIL_MSG_UNSOL_VOLTE_SETTING,
            RfxIntsData(response, 2));
    responseToRilj(msg);
}

void RtcImsController::handleSetImsConfigRequest(const sp<RfxMessage>& message) {
    void *data = message->getData()->getData();
    int imsEnable = ((int*)data)[5];
    mIsImsDisabling = false;
    //logD(RFX_LOG_TAG, "imsEnable=%d, mIsImsDisabling=%d", imsEnable, mIsImsDisabling);

    setVolteSettingStatus(message);
    requestToMcl(message);
}

void RtcImsController::handleSetImsConfigResponse(const sp<RfxMessage>& message) {
    responseToRilj(message);
}

void RtcImsController::setVolteSettingStatus(const sp<RfxMessage>& message) {
    if (!RfxRilUtils::isCtVolteSupport()) {
        return;
    }

    void *data = message->getData()->getData();
    int value = ((int*)data)[0];
    int slot_id = getSlotId();
    logD(RFX_LOG_TAG, "setVolteSettingStatus: value = %d, slot:%d ", value, slot_id);

    int sim_state = getStatusManager(slot_id)->getIntValue(
                                    RFX_STATUS_KEY_SIM_STATE);
    if (sim_state == RFX_SIM_STATE_NOT_READY) {
        logD(RFX_LOG_TAG, "setVolteSettingStatus: skip set for invalid sim type");
    } else if ((RfxRilUtils::isMultipleImsSupport() == 0) && (slot_id != mMainSlotId)) {
        logD(RFX_LOG_TAG, "setVolteSettingStatus: skip set for it's not main sim");
    } else {
        char volteEnable[RFX_PROPERTY_VALUE_MAX] = { 0 };
        property_get(PROPERTY_VOLTE_ENABLE, volteEnable, "0");
        int enalbeValue = atoi(volteEnable);
        char volteState[RFX_PROPERTY_VALUE_MAX] = { 0 };
        property_get(PROPERTY_VOLTE_STATE, volteState, "0");
        int stateValue = atoi(volteState);
        if (RfxRilUtils::isMultipleImsSupport() == 0) {
            if (enalbeValue == 1) {
                stateValue = stateValue | (1 << slot_id);
            } else {
                stateValue = stateValue & (~(1 << slot_id));
            }
        } else {
            if (((enalbeValue >> slot_id) & 0x01) == 1) {
                stateValue = stateValue | (1 << slot_id);
            } else {
                stateValue = stateValue & (~(1 << slot_id));
            }
        }
        char temp[3] = { 0 };
        if (stateValue > 10) {
            temp[0] = '1';
            temp[1] = '0' + (stateValue - 10);
        } else {
            temp[0] = '0' + stateValue;
        }
        property_set(PROPERTY_VOLTE_STATE, temp);
        logD(RFX_LOG_TAG, "setVolteSettingStatus: state = %d, %s", stateValue, temp);
    }
}

const char* RtcImsController::givePrintableStr(const char* iccId) {
    static char iccIdToPrint[RFX_PROPERTY_VALUE_MAX] = { 0 };
    if (strlen(iccId) > 6) {
        strncpy(iccIdToPrint, iccId, 6);
        return iccIdToPrint;
    }
    return iccId;
}
/// @}

void RtcImsController::updateImsRegStatus() {
    sp<RfxMessage> msg = RfxMessage::obtainRequest(getSlotId(),
            RFX_MSG_REQUEST_SET_IMS_REGISTRATION_REPORT, RfxVoidData());
    requestToMcl(msg);
}

void RtcImsController::handleImsDisableDoneUrc(const sp<RfxMessage>& message) {
    RFX_UNUSED(message);
    mIsImsDisabling = false;
    //logD(RFX_LOG_TAG, "mIsImsDisabling = %d", mIsImsDisabling);
}

bool RtcImsController::onPreviewMessage(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    //logD(RFX_LOG_TAG, "onPreviewMessage - %s", RFX_ID_TO_STR(msg_id));
    switch (msg_id) {
    case RFX_MSG_REQUEST_SET_IMSCFG:
        // Queue the AT+EIMSCFG during IMS disabling
        if(mIsImsDisabling != false){
            logD(RFX_LOG_TAG, "onPreviewMessage - %s, mIsImsDisabling -  %d", RFX_ID_TO_STR(msg_id), mIsImsDisabling);
        }
        return (mIsImsDisabling == false);
    default:
        break;
    }
    return true;
}

bool RtcImsController::onCheckIfResumeMessage(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    logD(RFX_LOG_TAG, "onCheckIfResumeMessage - %s", RFX_ID_TO_STR(msg_id));
    switch (msg_id) {
    case RFX_MSG_REQUEST_SET_IMSCFG:
        // Resume any AT+EIMSCFG after IMS disabled
        return (mIsImsDisabling == false);
    default:
        break;
    }
    return true;
}
