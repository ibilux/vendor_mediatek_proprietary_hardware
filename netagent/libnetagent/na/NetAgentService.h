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

#ifndef __NET_AGENT_SERVICE_H__
#define __NET_AGENT_SERVICE_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <cutils/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sysutils/NetlinkListener.h>
#include <list>
#include <map>
#include <dirent.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "netagent_io.h"
#ifdef __cplusplus
} // closing brace for extern "C"
#endif

#include "hardware/ccci_intf.h"
#include "netutils/ifc.h"
#include "ifcutils/ifc.h"

#include "NetAction.h"

/*****************************************************************************
 * Defines
 *****************************************************************************/
#ifndef NA_LOG_V
#define NA_LOG_V(...) \
    ((void)__android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_VERBOSE, NA_LOG_TAG, __VA_ARGS__))
#endif

#ifndef NA_LOG_D
#define NA_LOG_D(...) \
    ((void)__android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_DEBUG, NA_LOG_TAG, __VA_ARGS__))
#endif

#ifndef NA_LOG_I
#define NA_LOG_I(...) \
    ((void)__android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_INFO, NA_LOG_TAG, __VA_ARGS__))
#endif

#ifndef NA_LOG_W
#define NA_LOG_W(...) \
    ((void)__android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_WARN, NA_LOG_TAG, __VA_ARGS__))
#endif

#ifndef NA_LOG_E
#define NA_LOG_E(...) \
    ((void)__android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_ERROR, NA_LOG_TAG, __VA_ARGS__))
#endif

#define NA_INIT(a) a = netagent_io_init()
#define NA_DEINIT(a) netagent_io_deinit(a)
#define NA_CMD netagent_io_cmd_e
#define NA_ADDR_TYPE netagent_io_addr_type_e
#define NA_RAN_TYPE netagent_io_ho_ran_e
#define NA_IFST netagent_io_ifst_e
#define NA_MTU_SIZE netagent_io_mtu_e
#define NA_RA netagent_io_ra_e
#define NA_PDN_HO_INFO netagent_io_ho_info_t
#define NA_CMD_SEND(a, b) netagent_io_send(a, b)
#define NA_CMD_IFST_ALLOC(a, b, c) netagent_io_cmd_ifst_alloc(a, b, c)
#define NA_CMD_IPUPDATE_ALLOC(a, b, c, d) netagent_io_cmd_ipupdate_alloc(a, b, c, d)
#define NA_CMD_RA_ALLOC(a, b) netagent_io_cmd_ra_alloc(a, b)
#define NA_CMD_SYNC_CAPABILITY_ALLOC() netagent_io_cmd_sync_capability_alloc()
#define NA_CMD_PDNHO_ALLOC(a) netagent_io_cmd_pdnho_alloc(a)
#define NA_CMD_TYPE(a, b) netagent_io_get_cmd_type(a, b)
#define NA_CMD_RECV(a, b) b = netagent_io_recv(a)
#define NA_CMD_FREE(a) netagent_io_cmd_free(a)
#define NA_GET_IF_ID(a, b) netagent_io_get_if_id(a, b)
#define NA_GET_IFST_STATE(a, b) \
        b = (a == ENABLE) ? NETAGENT_IO_IFST_UP : NETAGENT_IO_IFST_DOWN;
#define NA_GET_MTU_SIZE(a, b) netagent_io_get_mtu_size(a, b)
#define NA_GET_ADDR_TYPE(a, b) netagent_io_get_addr_type(a, b)
#define NA_GET_ADDR_V4(a, b) netagent_io_get_addr_v4(a, b)
#define NA_GET_ADDR_V6(a, b) netagent_io_get_addr_v6(a, b)
#define NA_GET_IP_CHANGE_REASON(a, b) netagent_io_get_ip_change_reason(a, b)
#define NA_GET_PDN_HO_INFO(a, b) netagent_io_get_pdnho_info(a, b)
#define NA_FLUSH_IPSEC_POLICY(a, b) netagent_io_flush_ipsec_policy(a, b)

#define INVALID_AID -1
#define INVALID_INTERFACE_ID -1
#define MAX_IPV4_ADDRESS_LENGTH 17
//xxx.xxx.xxx.xxx
#define MAX_IPV6_ADDRESS_LENGTH 65
//xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
//xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx.xxx
#define MAX_MTU_SIZE_LENGTH 5
#define IPV6_PREFIX "FE80:0000:0000:0000:"
#define NULL_IPV6_ADDRESS "0::0"
#define INVALID_IPV6_PREFIX_LENGTH -1
#define TRANSACTION_ID_OFFSET 100
#define INVALID_TRANS_INTF_ID -1
#define IPV4_REFIX_LENGTH 32
#define IPV6_REFIX_LENGTH 64


#define FREEIF(data)    \
if (data != NULL) {     \
    free(data);         \
    data = NULL;        \
}

#define _IN6_IS_ULA(a)  \
    ((((a)->s6_addr[0] & 0xff) == 0xfc) || (((a)->s6_addr[0] & 0xff) == 0xfd))
/*****************************************************************************
 * Class NetAgentService
 *****************************************************************************/
class NetlinkEventHandler;

typedef enum {
    // Request is from Data Domain Manager(DDM) which is used for NW-IF operation.
    REQUEST_TYPE_DDM = 0,
    // Request is from Netlink Event Handler which is used for Netlink operation.
    REQUEST_TYPE_NETLINK = 1,
    // Request is from NetAgent which is used to control network state currently.
    REQUEST_TYPE_NETAGENT = 2,
} REQUEST_TYPE;

typedef struct NetAgentReqInfo {
    struct NetAgentReqInfo *pNext;
    void *pNetAgentCmdObj;
    NA_CMD cmdType;
    REQUEST_TYPE reqType;
} NetAgentReqInfo;

typedef enum {
    DISABLE = 0,
    ENABLE = 1,
    UPDATE = 2
} STATUS;

typedef struct {
    int state;
    int interfaceId;
} SetNetworkTransmitState;

typedef struct {
    NA_CMD cmd;
    sp<NetActionBase> action;
    union {
        SetNetworkTransmitState snts;
    } parameter;
} NetEventReqInfo;

typedef struct {
    int interfaceId;
    int cid;
    NA_ADDR_TYPE addrType;
    char addressV4[MAX_IPV4_ADDRESS_LENGTH];
    char addressV6[MAX_IPV6_ADDRESS_LENGTH];
} NetAgentPdnInfo;

class NetAgentService {
    public:
        NetAgentService();
        virtual ~NetAgentService();
        static bool createNetAgentService();

    public:
        static const char* getCcmniInterfaceName();
        static const char* getCcmniInterfaceName(int rid);
        void enqueueReqInfo(void* obj, REQUEST_TYPE reqType);
        static const char *cmdToString(NA_CMD cmd);
        static const char *addrTypeToString(NA_ADDR_TYPE addrType);
        static const char *reqTypeToString(REQUEST_TYPE reqType);
        static const char *ranTypeToString(NA_RAN_TYPE ranType);
        static const char *hoStateToString(int state);
        static const char *hoResultToString(int result);
        static NetAgentService* getInstance();
        void setNetworkTransmitState(int state, int transIntfId, const sp<NetActionBase>& action);
        int getTransIntfId(int interfaceId);
        // Test mode start.
        void setTransactionInterfaceId(int transIntfId);
        void removeTransactionInterfaceId(int transIntfId);
        void removeAllTransactionInterfaceId();
        // Test mode end.

    private:
        void init();
        void startEventLoop(void);
        void startReaderLoop(void);
        static void *eventThreadStart(void *arg);
        static void *readerThreadStart(void *arg);
        void runEventLoop();
        void runReaderLoop();
        void handleEvent(NetAgentReqInfo* pReqInfo);
        NetAgentReqInfo *createNetAgentReqInfo(void* obj, REQUEST_TYPE reqType, NA_CMD cmd);
        NetAgentReqInfo *dequeueReqInfo();
        void setNwIntfDown(const char *interfaceName);
        void nwIntfIoctlInit();
        void nwIntfIoctlDeInit();
        void nwIntfSetFlags(int s, struct ifreq *ifr, int set, int clr);
        inline void nwIntfInitSockAddrIn(struct sockaddr_in *sin, const char *addr);
        void nwIntfSetAddr(int s, struct ifreq *ifr, const char *addr);
        void nwIntfSetIpv6Addr(int s, struct ifreq *ifr, const char *addr);
        void configureNetworkInterface(NetAgentReqInfo* pReqInfo, STATUS isUp);
        void configureMTUSize(NetAgentReqInfo* pReqInfo);
        void updateIpv6GlobalAddress(NetAgentReqInfo* pReqInfo);
        void notifyNoRA(NetAgentReqInfo* pReqInfo);
        void confirmInterfaceState(unsigned int interfaceId, NA_IFST state, NA_ADDR_TYPE addrType);
        void confirmIpUpdate(unsigned int interfaceId, NA_ADDR_TYPE addrType, unsigned int* addr, int ipv6PrefixLength);
        void confirmNoRA(unsigned int interfaceId, NA_RA flag);
        void syncCapabilityToModem();
        void setCapabilityToModem(NetAgentReqInfo* pReqInfo);
        void confirmPdnHandoverControl(unsigned int tranId);
        void handlePdnHandoverControl(NetAgentReqInfo* pReqInfo);
        void updatePdnHandoverAddr(NetAgentReqInfo* pReqInfo);
        NetAgentPdnInfo *recordPdnHandoverInfo(unsigned int interfaceId, NA_ADDR_TYPE addrType, char *addressV4, char *addressV6);
        NetAgentPdnInfo *getPdnHandoverInfo(unsigned int interfaceId);
        bool clearPdnHandoverInfo(unsigned int interfaceId);
        void clearIpsec(unsigned int interfaceId);
        bool isNeedNotifyIPv6RemovedToModem(unsigned int interfaceId, char* delAddr);

        void startNetlinkEventHandler(void);
        NetlinkEventHandler *setupSocket(int *sock, int netlinkFamily, int groups, int format);
        bool isIpv6Global(const char *ipv6Addr);
        int getCommand(void* obj, REQUEST_TYPE reqType, NA_CMD *cmd);
        void getIpv4Address(void *obj, char *addressV4);
        void getIpv6Address(void *obj, char *addressV6);
        void getIpv4v6Address(void *obj, char *addressV4, char *addressV6);
        int convertIpv6ToBinary(unsigned int *output, char *input);
        int convertIpv4ToString(char *output, unsigned int *input);
        int convertIpv6ToString(char *output, unsigned int *input);
        void freeNetAgentCmdObj(NetAgentReqInfo *pReqInfo);
        void configureNetworkTransmitState(NetAgentReqInfo* pReqInfo);
        void setNwTxqState(int interfaceId, int state);
        char* getNetworkInterfaceName(int interfaceId);
        bool isTransIntfIdMatched(int transIntfId);
        void configureRSTimes(int interfaceId);

    private:
        static pthread_mutex_t sInitMutex;
        static NetAgentService* sInstance;
        pthread_t mReaderThread;
        pthread_t mEventThread;

        void* m_pNetAgentIoObj;
        NetAgentReqInfo* m_pNetAgentReqInfo;
        pthread_mutex_t mDispatchMutex;
        pthread_cond_t mDispatchCond;

        int sock_fd;
        int sock6_fd;
        NetlinkEventHandler *m_pRouteHandler;
        int mRouteSock;

        static const char* CCMNI_IFNAME_CCMNI;

        std::list<int> m_lTransIntfId; // element: tran_id * 100 + interface_id
        // Hashmap to store handover PDN information, data: <tid, NetAgentPdnInfo>
        std::map<int, NetAgentPdnInfo *> m_pdnHoInfoMap;
};

#endif /* __NET_AGENT_SERVICE_H__ */
