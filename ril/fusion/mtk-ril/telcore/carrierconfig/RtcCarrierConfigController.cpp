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
#include "RtcCarrierConfigController.h"
#include "RfxRootController.h"

#include "RfxStatusDefs.h"
#include "rfx_properties.h"


/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

#define RFX_LOG_TAG "RtcCarrier"

RFX_IMPLEMENT_CLASS("RtcCarrierConfigController", RtcCarrierConfigController, RfxController);


static OperatorTable operatorTable[] = {
    // OP01
    {46000, 46000, 1},    // China
    {46002, 46002, 1},    // China
    {46004, 46004, 1},    // China
    {46007, 46007, 1},    // China
    {46008, 46008, 1},    // China
    {46070, 46070, 1},    // China
    {46096, 46096, 1},    // China Test SIM

    // OP02
    {46001, 46001, 2},    // China
    {46006, 46006, 2},    // China

    // OP03
    {20801, 20802, 3},    // France
    {20829, 20829, 3},    // France
    {20891, 20891, 3},    // France
    {34001, 34001, 3},    // French Guiana

    {21403, 21403, 3},    // Spain
    {21409, 21409, 3},    // Spain
    {21411, 21411, 3},    // Spain
    {60201, 60201, 3},    // Egypt-orange
    {26003, 26003, 3},    // Orange-Poland
    {22610, 22610, 3},    // Orange-Romania

    // OP05
    {26201, 26201, 5},    // Germany
    {26206, 26206, 5},    // Germany

    {26002, 26002, 5},    // Poland
    {26034, 26034, 5},    // Poland
    {23001, 23001, 5},    // Czech

    // OP06
    {23415, 23415, 6},    // United Kingdom
    {23427, 23427, 6},    // United Kingdom
    {23491, 23491, 6},    // United Kingdom

    {22206, 22206, 6},    // Italy
    {22210, 22210, 6},    // Italy

    {26202, 26202, 6},    // Germany
    {26204, 26204, 6},    // Germany
    {26209, 26209, 6},    // Germany

    {26801, 26801, 6},    // Portugal

    {21401, 21401, 6},    // Spain
    {21406, 21406, 6},    // Spain

    {28602, 28602, 6},    // Turkey

    {20404, 20404, 6},    // NL

    {23003, 23003, 6},    // CZ

    {65501, 65501, 6},    // ZA
    {60202, 60202, 6},    // Egypt-vodafone
    {20205, 20205, 6},    // Greece-vodafone

    // OP07
    {31030, 31030, 7},    //United States
    {310070, 310070, 7},    //United States
    {310090, 310090, 7},    //United States
    {310680, 310680, 7},    //United States
    {310170, 310170, 7},    //United States
    {310280, 310280, 7},    //United States
    {310380, 310380, 7},    //United States
    {310410, 310410, 7},    //United States
    {310560, 310560, 7},    //United States
    {310950, 310950, 7},    //United States
    {311180, 311180, 7},    //United States

    {334050, 334050, 7},    //Maxico

    // OP08
    {310230, 310230, 8},    //United States
    {31031, 31032, 8},    //United States
    {310220, 310220, 8},    //United States
    {310270, 310270, 8},    //United States
    {310210, 310210, 8},    //United States
    {310260, 310260, 8},    //United States
    {310200, 310200, 8},    //United States
    {310250, 310250, 8},    //United States
    {310160, 310160, 8},    //United States
    {310240, 310240, 8},    //United States
    {310660, 310660, 8},    //United States
    {310300, 310300, 8},    //United States
    {310330, 310330, 8},    //United States
    {310800, 310800, 8},    //United States
    {310310, 310310, 8},    //United States
    {310490, 310490, 8},    //United States
    {310580, 310580, 8},    //United States

    // OP09
    {46003, 46003, 9},    //China
    {46005, 46005, 9},    //China
    {46011, 46011, 9},    //China
    {45502, 45502, 9},    //Macao China
    {45507, 45507, 9},    //Macao China

    // OP10

    // OP11
    {23420, 23420, 11},    //United Kingdom
    {23594, 23594, 11},    //United Kingdom


    // OP12
    {31004, 31004, 12},    //United States
    {31010, 31010, 12},    //United States
    {31012, 31013, 12},    //United States
    {310590, 310590, 12},    //United States
    {310890, 310890, 12},    //United States
    {310910, 310910, 12},    //United States

    {311110, 311110, 12},    //United States
    {311270, 311289, 12},    //United States
    {311390, 311390, 12},    //United States
    {311480, 311489, 12},    //United States

    // OP15
    {23208, 23208, 15},    //Austria

    {23410, 23410, 15},    //United Kingdom

    {26207, 26207, 15},    //DE

    // OP16
    {23502, 23502, 16},    //United Kingdom
    {23430, 23434, 16},    //United Kingdom
    {23486, 23486, 16},    //United Kingdom

    // OP17
    {44010, 44010, 17},    //Japan

    {310370, 310370, 17},    //Guam
    {310470, 310470, 17},    //Guam

    // OP18
    {405840, 405840, 18},   //India
    {405854, 405874, 18},   //India
    {40587, 40587, 18},    //India

    // OP19
    {50501, 50501, 19},    //Australia
    {50511, 50511, 19},    //Australia
    {50571, 50572, 19},    //Australia

    {53004, 53004, 19},    //New Zealand

    // OP20
    {310120, 310120, 20},    //United States
    {311490, 311490, 20},    //United States

    // OP50
    {44000, 44000, 50},    //Japan
    {44020, 44020, 50},    //Japan

    // OP100
    {45400, 45400, 100},   //Hongkong China
    {45402, 45402, 100},   //Hongkong China
    {45410, 45410, 100},   //Hongkong China
    {45418, 45418, 100},   //Hongkong China

    // OP101
    {45416, 45416, 101},   //Hongkong China
    {45419, 45420, 101},   //Hongkong China
    {45429, 45429, 101},   //Hongkong China

    // OP102
    {45406, 45406, 102},   //Hongkong China

    // OP103
    {52501, 52502, 103},   //Singapore
    {52507, 52507, 103},   //Singapore

    // OP104
    {52505, 52506, 104},   //Singapore

    // OP105
    {74402, 74402, 105},   //Paraguay

    // OP106
    {45403, 45405, 106},   //Hongkong China
    {45414, 45414, 106},   //Hongkong China

    // OP107
    {64710, 64710, 107},   //Reunion
    {20809, 20811, 107},   //France

    // OP108
    {46697, 46697, 108},   //Taiwan
    {46699, 46699, 108},   //Taiwan

    // OP109
    {46692, 46692, 109},   //Taiwan

    // OP110
    {46601, 46603, 110},   //Taiwan
    {46606, 46607, 110},   //Taiwan
    {46688, 46688, 110},   //Taiwan


    // OP111
    {40401, 40401, 111},   //Vodafone India
    {40405, 40405, 111},   //Vodafone India
    {40411, 40411, 111},   //Vodafone India
    {40413, 40413, 111},   //Vodafone India
    {40415, 40415, 111},   //Vodafone India
    {40420, 40420, 111},   //Vodafone India
    {40427, 40427, 111},   //Vodafone India
    {40430, 40430, 111},   //Vodafone India
    {40443, 40443, 111},   //Vodafone India
    {40446, 40446, 111},   //Vodafone India
    {40460, 40460, 111},   //Vodafone India
    {40484, 40484, 111},   //Vodafone India
    {40486, 40486, 111},   //Vodafone India
    {40488, 40488, 111},   //Vodafone India
    {40566, 40567, 111},   //Vodafone India
    {405750, 405756, 111},   //Vodafone India


    // OP112
    {73404, 73404, 112},   //Venezuela
    {36251, 36251, 112},   //Former Netherlands Antilles
    {34003, 34003, 112},   //French Antilles
    {33402, 33402, 112},   //TelCel Maxico
    {334020, 334020, 112},   //TelCel Maxico

    // OP113
    {25099, 25099, 113},   //Venezuela

    // OP114
    {45002, 45002, 114},   //South Korea
    {45004, 45004, 114},   //South Korea

    // OP115
    {45005, 45005, 115},   //South Korea

    // OP116
    {45006, 45006, 116},   //South Korea

    // OP117
    {51009, 51009, 117},   //Indonesia
    {51028, 51028, 117},   //Indonesia

    // OP118
    {502152, 502152, 118},   //Malaysia

    // OP119
    {37203, 37203, 119},   //Haiti
    {62140, 62140, 119},   //Natcom - Nigeria

    // OP120
    {732101, 732101, 120},   //Colombia

    {71610, 71610, 120},    //Claro Peru
    {72405, 72405, 120},    //Claro Brazil

    {722310, 722310, 120},   //Argentina ¡V Claro
    {722320, 722320, 120},   //Argentina ¡V Claro
    {722330, 722330, 120},   //Argentina ¡V Claro

    // OP121
    {302610, 302610, 121},   //Canada
    {302640, 302640, 121},   //Canada
    {302690, 302690, 121},   //Canada

    // OP122
    {52001, 52001, 122},   //Thailand
    {52003, 52003, 122},   //Thailand
    {52023, 52023, 122},   //Thailand

    // OP124
    {46605, 46605, 124},   //Taiwan

    // OP125
    {52005, 52005, 125},   //Thailand

    // OP126
    {28603, 28603, 126},   //Turkey

    // OP127
    {25002, 25002, 127},   //Russian-Megafon
    {25011, 25011, 127},   //Russian-Megafon
    {25030, 25030, 127},   //Russian

    // OP128
    {24412, 24412, 128},   //Finland

    // OP129
    {44007, 44008, 129},   // Japan
    {44050, 44056, 129},   // Japan
    {44070, 44079, 129},   // Japan
    {44088, 44089, 129},   // Japan
    {44170, 44170, 129},   // Japan

    // OP130
    {22201, 22201, 130},   // Italy
    {72402, 72404, 130},   // Tim - Brazil

    // OP131
    {52000, 52000, 131},   // TH
    {52004, 52004, 131},   // TH

    // OP132
    {71606, 71606, 132},   // Peru
    {732123, 732123, 132},   // Movistar-columbia
    {72207, 72207, 132},   // Movistar - Argentina
    {72406, 72406, 132},   // Vivo-Brazil
    {72410, 72411, 132},   // Vivo-Brazil
    {72423, 72423, 132},   // Vivo-Brazil

    {334030, 334030, 132},   // Maxico
    {73002, 73002, 132},   // Chile

    // OP133
    {42403, 42403, 133},   // DU

    // OP134
    {24405, 24405, 134},   // Elisa-Finland

    // OP135
    {25001, 25001, 135},   // MTS

    // OP136
    {71617, 71617, 136},   // Peru-Entel

    // OP137
    {25020, 25020, 137},   // Tele2

    // OP139
    {20820, 20820, 139},   // Bouygues

    // OP141
    {65507, 65507, 141},   // Cell C South Africa

    // OP143
    {28601, 28601, 143},   // Turkcell

    // OP144
    {64011, 64011, 144},   // Tanzania - Smile
    {64133, 64133, 144},   // Uganda - Smile
    {62127, 62127, 144},   // Nigeria - Smile

    // OP145
    {310150, 310150, 145},   // Cricket-USA-HTC/TCL

    // OP146
    {60203, 60203, 146},   // Etisalat - Egypt
    {42402, 42402, 146},   // Etisalat - UAE

    // OP147
    {40402, 40403, 147},   // Airtel - India
    {40410, 40410, 147},   // Airtel - India
    {40416, 40416, 147},   // Airtel - India
    {40431, 40431, 147},   // Airtel - India
    {40440, 40440, 147},   // Airtel - India
    {40445, 40445, 147},   // Airtel - India
    {40449, 40449, 147},   // Airtel - India
    {40470, 40470, 147},   // Airtel - India
    {40490, 40490, 147},   // Airtel - India
    {40492, 40498, 147},   // Airtel - India
    {40551, 40556, 147},   // Airtel - India

    // OP149
    {45412, 45412, 149},   // CMHK - Sony

    // OP150
    {22801, 22801, 150},   // Swisscom Schweiz AG
    {29501, 29501, 150},   // Swisscom Schweiz AG

    // OP151
    {52503, 52503, 151},   // M1 Singapore

    // OP152
    {50502, 50502, 152},   // Optus Australia

    // OP153
    {50503, 50503, 153},   // VHA Australia
    {50506, 50506, 153},   // VHA Australia(Hutchison 3G Australia Pty 3TELSTRA)

    // OP155
    {50216, 50216, 155},   // Digi-Malaysia
    {21603, 21603, 155},   // Digi-Hungary

    // OP156
    {23802, 23802, 156},   // Telenor - Denmark

    // OP157
    {302220, 302221, 157},   // Telus - Canada

    // OP158
    {42004, 42004, 158},   // Zain - Saudi Arabia

    // OP159
    {42001, 42001, 159},   // STC - Saudi Arabia

    // OP160
    {502153, 502153, 160},   // Web - Malaysia

    // OP161
    {26006, 26006, 161},   // Poland - Sony
    {26098, 26098, 161},   // Poland - Sony

    //OP163
    {41302, 41302, 163},   // Dialog - Sri-Lanka

    //OP186
    {405845, 405853, 186},   // Idea - India

    // OP1001
    {262080, 262080, 1001},   //Ericsson IODT

    // OP1002
    {23112, 23112, 1002},   // Nokia IODT

    // OP1003
    {24099, 24099, 1003},    //MTK_HQLAB-Ericsson

    // OP1004
    {46020, 46020, 1004},    //Huawei IODT
};


RtcCarrierConfigController::RtcCarrierConfigController() {
    for (int i = 0; i < MAX_SIM_COUNT; i++) {
        mCurrentOperatorId[i] = -1;
    }
}

RtcCarrierConfigController::~RtcCarrierConfigController() {
}

void RtcCarrierConfigController::onInit() {
    // Required: invoke super class implementation
    RfxController::onInit();

    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_UICC_GSM_NUMERIC,
            RfxStatusChangeCallback(this, &RtcCarrierConfigController::onUiccMccMncChanged));
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_UICC_CDMA_NUMERIC,
            RfxStatusChangeCallback(this, &RtcCarrierConfigController::onUiccMccMncChanged));
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_CARD_TYPE,
            RfxStatusChangeCallback(this, &RtcCarrierConfigController::onCardTypeChanged));

    /* Reset operator id when boot up */
    updateOpIdProperty(0);
}

void RtcCarrierConfigController::onDeinit() {
    logD(RFX_LOG_TAG, "onDeinit");
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_UICC_GSM_NUMERIC,
        RfxStatusChangeCallback(this, &RtcCarrierConfigController::onUiccMccMncChanged));
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_UICC_CDMA_NUMERIC,
        RfxStatusChangeCallback(this, &RtcCarrierConfigController::onUiccMccMncChanged));
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_CARD_TYPE,
        RfxStatusChangeCallback(this, &RtcCarrierConfigController::onCardTypeChanged));
    RfxController::onDeinit();
}


bool RtcCarrierConfigController::onHandleRequest(const sp<RfxMessage>& message) {
    RFX_UNUSED(message);
    return true;
}

bool RtcCarrierConfigController::onHandleUrc(const sp<RfxMessage>& message) {
    RFX_UNUSED(message);
    return true;
}

bool RtcCarrierConfigController::onHandleResponse(const sp<RfxMessage>& message) {
    RFX_UNUSED(message);
    return true;
}

bool RtcCarrierConfigController::responseToRilj(const sp<RfxMessage>& message) {
    RFX_UNUSED(message);
    return true;
}

void RtcCarrierConfigController::onUiccMccMncChanged(RfxStatusKeyEnum key,
        RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RFX_UNUSED(old_value);
    String8 mccmncStr = value.asString8();
    int mccmnc;

    mccmnc = atoi(mccmncStr.string());
    logD(RFX_LOG_TAG, "[onUiccMccMncChanged]MCC/MNC: %d", mccmnc);

    updateOpIdProperty(mccmnc);
}

void RtcCarrierConfigController::onCardTypeChanged(RfxStatusKeyEnum key,
    RfxVariant oldValue, RfxVariant newValue) {
    RFX_UNUSED(key);
    logD(RFX_LOG_TAG, "[onCardTypeChanged]oldValue: %d, newValue: %d",
        oldValue.asInt(), newValue.asInt());

    /*
      -1: init value
      0: no card
      others: sim/usim/csim/rui
    */
    int cardType = newValue.asInt();
    if (cardType <= 0) {
        updateOpIdProperty(0);
    } else {
        int mccmnc = 0;
        String8 mccmncStr;
        String8 defaultValue = String8("0");
        mccmncStr = getStatusManager()->getString8Value(RFX_STATUS_KEY_UICC_GSM_NUMERIC,
                defaultValue);
        if ((mccmncStr.string()  == NULL) || (mccmncStr == "0")) {
            mccmncStr = getStatusManager()->getString8Value(RFX_STATUS_KEY_UICC_CDMA_NUMERIC,
                    defaultValue);
        }
        if (mccmncStr.string() != NULL) {
            mccmnc = atoi(mccmncStr.string());
        }
        updateOpIdProperty(mccmnc);
    }
}

void RtcCarrierConfigController::updateOpIdProperty(int mccmnc) {
    int operatorId = 0, i, len, slotId;
    char* opIdStr = NULL;
    len = sizeof(operatorTable) / sizeof(operatorTable[0]);

    if (mccmnc != 0) {
        for(i = 0; i < len; i++) {
            if(operatorTable[i].mccMnc_range_start <= mccmnc
                    && mccmnc <= operatorTable[i].mccMnc_range_end) {
                operatorId = operatorTable[i].opId;
                break;
            }
        }

        /*
           if MCC/MNC is not found in mapping table, write specific id for
           indicating that operator id is not matched, and let modules know
           they may need to use their own default value  or behavior.
        */
        if (operatorId == 0) {
            operatorId = NOT_MATCHED_OP_ID;
        }
    }

    slotId = getSlotId();
    if (slotId < MAX_SIM_COUNT) {
        if (mCurrentOperatorId[slotId] == operatorId) {
            return;
        }
        mCurrentOperatorId[slotId] = operatorId;
    }

    asprintf(&opIdStr, "%d", operatorId);
    logD(RFX_LOG_TAG, "[updateOpIdProperty]opIdStr: %s", opIdStr);

    if (slotId == 0) {
        String8 prop(PROP_NAME_OPERATOR_ID_SIM1);
        rfx_property_set(prop, opIdStr);
    } else if (slotId == 1) {
        String8 prop(PROP_NAME_OPERATOR_ID_SIM2);
        rfx_property_set(prop, opIdStr);
    } else if (slotId == 2) {
        String8 prop(PROP_NAME_OPERATOR_ID_SIM3);
        rfx_property_set(prop, opIdStr);
    } else if (slotId == 3) {
        String8 prop(PROP_NAME_OPERATOR_ID_SIM4);
        rfx_property_set(prop, opIdStr);
    }
    free(opIdStr);
}
