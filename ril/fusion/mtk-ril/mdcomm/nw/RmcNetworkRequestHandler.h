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

#ifndef __RMC_NETWORK_REQUEST_HANDLER_H__
#define __RMC_NETWORK_REQUEST_HANDLER_H__

#include "RmcNetworkHandler.h"
#include "RfxNeighboringCellData.h"
#include "RfxCellInfoData.h"
#include "RfxNetworkScanData.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RmcNwReqHdlr"

typedef struct {
    int registration_state;
    unsigned int lac;
    unsigned int cid;
    int radio_technology;
    int denied_reason;
    int max_simultaneous_data_call;
    int tac;
    int physical_cid;
    int eci;
    int csgid;
    int tadv;
} RIL_DATA_REG_STATE_CACHE;

/* RIL Network Enumeration */
typedef enum
{
    GSM_BAND_900    = 0x02,
    GSM_BAND_1800   = 0x08,
    GSM_BAND_1900   = 0x10,
    GSM_BAND_850    = 0x80
} GSM_BAND_ENUM;

typedef enum
{
    UMTS_BAND_I     = 0x0001,
    UMTS_BAND_II    = 0x0002,
    UMTS_BAND_III   = 0x0004,
    UMTS_BAND_IV    = 0x0008,
    UMTS_BAND_V     = 0x0010,
    UMTS_BAND_VI    = 0x0020,
    UMTS_BAND_VII   = 0x0040,
    UMTS_BAND_VIII  = 0x0080,
    UMTS_BAND_IX    = 0x0100,
    UMTS_BAND_X     = 0x0200
} UMTS_BAND_ENUM;

typedef enum
{
    BM_AUTO_MODE,
    BM_EURO_MODE,
    BM_US_MODE,
    BM_JPN_MODE,
    BM_AUS_MODE,
    BM_AUS2_MODE,
    BM_CELLULAR_MODE,
    BM_PCS_MODE,
    BM_CLASS_3,
    BM_CLASS_4,
    BM_CLASS_5,
    BM_CLASS_6,
    BM_CLASS_7,
    BM_CLASS_8,
    BM_CLASS_9,
    BM_CLASS_10,
    BM_CLASS_11,
    BM_CLASS_15,
    BM_CLASS_16,
    BM_40_BROKEN = 100,
    BM_FOR_DESENSE_RADIO_ON = 200,
    BM_FOR_DESENSE_RADIO_OFF = 201,
    BM_FOR_DESENSE_RADIO_ON_ROAMING = 202,
    BM_FOR_DESENSE_B8_OPEN = 203
} BAND_MODE;

typedef enum {
    CDMA_ROAMING_MODE_RADIO_DEFAULT = -1,
    CDMA_ROAMING_MODE_HOME = 0,
    CDMA_ROAMING_MODE_AFFILIATED = 1,
    CDMA_ROAMING_MODE_ANY = 2
} CDMA_ROAMING_MODE;

typedef struct SPNTABLE {
    char mccMnc[8];
    char spn[MAX_OPER_NAME_LENGTH+2];
} SpnTable;

#define RIL_NW_INIT_INT         {0, 0, 0, 0}

// NW property
#define PROPERTY_GSM_CURRENT_ENBR_RAT    "gsm.enbr.rat"
#define PROPERTY_GSM_CURRENT_COPS_LAC    "gsm.cops.lac"

class RmcNetworkRequestHandler : public RmcNetworkHandler {
    RFX_DECLARE_HANDLER_CLASS(RmcNetworkRequestHandler);

    public:
        RmcNetworkRequestHandler(int slot_id, int channel_id);
        virtual ~RmcNetworkRequestHandler();

        static bool isInService(int regState) {
            if (regState == 1 || regState == 5) {
                return true;
            }
            return false;
        }

    protected:
        virtual void onHandleRequest(const sp<RfxMclMessage>& msg);

        virtual void onHandleTimer();

        virtual void onHandleEvent(const sp<RfxMclMessage>& msg);

    private:
        void resetVoiceRegStateCache(RIL_VOICE_REG_STATE_CACHE *voiceCache, RIL_CACHE_GROUP source);
        void resetDataRegStateCache(RIL_DATA_REG_STATE_CACHE *dataCache);
        void updateVoiceRegStateCache(RIL_DATA_REG_STATE_CACHE *voice, int source, int count);
        void updateDataRegStateCache(RIL_DATA_REG_STATE_CACHE *data, int source, int count);
        void combineVoiceRegState(const sp<RfxMclMessage>& msg);
        int convertToAndroidRegState(unsigned int uiRegState);
        void updateCellularPsState();
        void combineDataRegState(const sp<RfxMclMessage>& msg);
        void requestSignalStrength(const sp<RfxMclMessage>& msg);
        void requestVoiceRegistrationState(const sp<RfxMclMessage>& msg);
        int requestVoiceRegistrationStateCdma(const sp<RfxMclMessage>& msg);
        void printVoiceCache(RIL_VOICE_REG_STATE_CACHE cache);
        void printDataCache();
        void requestDataRegistrationState(const sp<RfxMclMessage>& msg);
        void requestDataRegistrationStateGsm();
        void requestOperator(const sp<RfxMclMessage>& msg);
        void requestQueryNetworkSelectionMode(const sp<RfxMclMessage>& msg);
        void requestQueryAvailableNetworks(const sp<RfxMclMessage>& msg);
        void requestQueryAvailableNetworksWithAct(const sp<RfxMclMessage>& msg);
        void requestSetNetworkSelectionAutomatic(const sp<RfxMclMessage>& msg);
        void requestSetNetworkSelectionManual(const sp<RfxMclMessage>& msg);
        void requestSetNetworkSelectionManualWithAct(const sp<RfxMclMessage>& msg);
        void requestSetBandMode(const sp<RfxMclMessage>& msg);
        void requestQueryAvailableBandMode(const sp<RfxMclMessage>& msg);
        void requestGetNeighboringCellIds(const sp<RfxMclMessage>& msg);
        void requestSetLocationUpdates(const sp<RfxMclMessage>& msg);
        void requestGetCellInfoList(const sp<RfxMclMessage>& msg);
        void requestSetCellInfoListRate(const sp<RfxMclMessage>& msg);
        void requestGetPOLCapability(const sp<RfxMclMessage>& msg);
        void requestGetPOLList(const sp<RfxMclMessage>& msg);
        void requestSetPOLEntry(const sp<RfxMclMessage>& msg);
        void requestSetCdmaRoamingPreference(const sp<RfxMclMessage>& msg);
        void requestQueryCdmaRoamingPreference(const sp<RfxMclMessage>& msg);
        void requestGetFemtocellList(const sp<RfxMclMessage>& msg);
        void requestAbortFemtocellList(const sp<RfxMclMessage>& msg);
        void requestSelectFemtocell(const sp<RfxMclMessage>& msg);
        void requestScreenState(const sp<RfxMclMessage>& msg);
        void requestSetUnsolicitedResponseFilter(const sp<RfxMclMessage>& msg);
        void requestQueryFemtoCellSystemSelectionMode(const sp<RfxMclMessage>& msg);
        void requestSetFemtoCellSystemSelectionMode(const sp<RfxMclMessage>& msg);
        void requestAntennaConf(const sp<RfxMclMessage>& msg);
        void requestAntennaInfo(const sp<RfxMclMessage>& msg);
        void requestSetServiceState(const sp<RfxMclMessage>& msg);
        void requestSetPseudoCellMode(const sp<RfxMclMessage>& msg);
        void requestGetPseudoCellInfo(const sp<RfxMclMessage>& msg);
        void updatePseudoCellMode();
        void updateSignalStrength();
        void setUnsolResponseFilterSignalStrength(bool enable);
        void setUnsolResponseFilterNetworkState(bool enable);
        void updateServiceStateValue();
        void requestQueryCurrentBandMode();

        void triggerPollNetworkState();
        void isFemtoCell(int regState, int cid, int act);
        int isEnableModulationReport();
        void updateFemtoCellInfo();
        int getPLMNNameFromNumeric(char *numeric, char *longname, char *shortname, int max_length);
        int getEonsNamesFromNumericCode(char *code, unsigned int lac, char *longname, char *shortname, int max_length);
        int getOperatorNamesFromNumericCode(char *code, char *longname, char *shortname, int max_length);
        int getOperatorNamesFromNumericCode(char *code, unsigned int lac, char *longname, char *shortname, int max_length);
        void handleConfirmRatBegin(const sp<RfxMclMessage>& msg);
        void handlePsNetworkStateEvent(const sp<RfxMclMessage>& msg);
        void setRoamingEnable(const sp<RfxMclMessage>& msg);
        void getRoamingEnable(const sp<RfxMclMessage>& msg);
        void requestStartNetworkScan(const sp<RfxMclMessage>& msg);
        void requestStopNetworkScan(const sp<RfxMclMessage>& msg);

        int isDisable2G();

        RIL_DATA_REG_STATE_CACHE data_reg_state_cache;

    protected:
        int m_slot_id;
        int m_channel_id;
        int mPlmn_list_format;
        int m_emergency_only;
        int m_csgListOngoing = 0;
        int m_csgListAbort =0;

        //Add for band de-sense feature.
        int bands[4] = RIL_NW_INIT_INT;
        // MTK_TC1_FEATURE for Antenna Testing start
        int antennaTestingType = 0;
        // MTK_TC1_FEATURE for Antenna Testing end
};

#endif
