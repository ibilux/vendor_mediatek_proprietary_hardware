/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CcuDrv"

#include <stdio.h>
#include <stdlib.h>

#include <utils/Errors.h>
#include <utils/Mutex.h>    // For android::Mutex.
#include <cutils/log.h>
#include <cutils/properties.h>  // For property_get().
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/mman-proprietary.h>
#include <cutils/atomic.h>

#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <sys/resource.h>

#include <sys/mman.h>
#include <ion/ion.h>                    // Android standard ION api
#include <linux/ion_drv.h>              // define for ion_mm_data_t
#include <libion_mtk/include/ion.h>     // interface for mtk ion
#include <m4u_lib_port.h>

#include "ccu_reg.h"
/* kernel files */
#include "ccu_ext_interface.h"
#include "ccu_mailbox_extif.h"
#include "ccu_drv.h"

//#include <mtkcam/def/common.h>
#include <string.h>
#include <ccu_udrv_stddef.h>
#include <ccu_udrv.h>

#undef   DBG_LOG_TAG                        // Decide a Log TAG for current file.
#define  DBG_LOG_TAG        LOG_TAG
#include "ccu_log.h"                        // Note: DBG_LOG_TAG/LEVEL will be used in header file, so header must be included after definition.
DECLARE_DBG_LOG_VARIABLE(ccu_drv);

#define EVEREST_EP_MARK_CODE

class CcuDbgTimer
{
protected:
    char const*const    mpszName;
    mutable MINT32      mIdx;
    MINT32 const        mi4StartUs;
    mutable MINT32      mi4LastUs;

public:
    CcuDbgTimer(char const*const pszTitle)
        : mpszName(pszTitle)
        , mIdx(0)
        , mi4StartUs(getUs())
        , mi4LastUs(getUs())
    {
    }

    inline MINT32 getUs() const
    {
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000000 + tv.tv_usec;
    }

    inline MBOOL ProfilingPrint(char const*const pszInfo = "") const
    {
        MINT32 const i4EndUs = getUs();
        if  (0==mIdx)
        {
            LOG_INF("[%s] %s:(%d-th) ===> [start-->now: %.06f ms]", mpszName, pszInfo, mIdx++, (float)(i4EndUs-mi4StartUs)/1000);
        }
        else
        {
            LOG_INF("[%s] %s:(%d-th) ===> [start-->now: %.06f ms] [last-->now: %.06f ms]", mpszName, pszInfo, mIdx++, (float)(i4EndUs-mi4StartUs)/1000, (float)(i4EndUs-mi4LastUs)/1000);
        }
        mi4LastUs = i4EndUs;

        //sleep(4); //wait 1 sec for AE stable

        return  MTRUE;
    }
};

MINT32              CcuDrv::m_Fd = -1;
void*               CcuDrv::m_pCcuRegMap = NULL;
android::Mutex      CcuDrv::RegRWMacro;


volatile MINT32     CcuDrvImp::m_UserCnt = 0;
CCU_DRV_RW_MODE     CcuDrvImp::m_regRWMode = CCU_DRV_R_ONLY;
android::Mutex      CcuDrvImp::m_CcuInitMutex;
#if TF_PROT
IMemDrv*            CcuDrvImp::m_pMemDrv = NULL;
#endif
char                CcuDrvImp::m_UserName[MAX_USER_NUMBER][MAX_USER_NAME_SIZE] =
{
    {"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},
    {"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},
    {"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},
    {"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},{"\0"},
};

static void printCcuLog(const char *logStr, MBOOL mustPrint=MFALSE)
{
    #define LOG_LEN_LIMIT 800
    size_t length = 0;
    const char *logPtr = logStr;
    static char logBuf[LOG_LEN_LIMIT + 1];
    int logSegCnt = 0;
    int copyLen = 0;

    length = strlen(logStr);
    logSegCnt = length / LOG_LEN_LIMIT + 1;

    //LOG_DBG("logging ccu, length: %ld\n", length);
    //LOG_DBG("logging ccu, logSegCnt: %ld\n", length);
    if(mustPrint)
        LOG_WRN("===== CCU LOG DUMP START =====\n");
    else
        LOG_DBG("===== CCU LOG DUMP START =====\n");
    for(int i=0 ; i<logSegCnt; i++)
    {
        if(i == logSegCnt-1)
        {
            copyLen = length % LOG_LEN_LIMIT;
        }
        else
        {
            copyLen = LOG_LEN_LIMIT;
        }

        //LOG_DBG("logging ccu[%d], copyLen: %d\n", i, copyLen);

        if(copyLen > 0)
        {
            //LOG_DBG("logging ccu[%d], logPtr: %p\n", logPtr);
            memcpy(logBuf, logPtr, copyLen);
            logBuf[copyLen] = '\0';
            logPtr += copyLen;
            if(mustPrint)
                LOG_WRN("\n----- DUMP SEG[%d] -----\n%s\n", i, logBuf, i);
            else
                LOG_DBG("\n----- DUMP SEG[%d] -----\n%s\n", i, logBuf, i);
        }
    }
    if(mustPrint)
        LOG_WRN("===== CCU LOG DUMP END =====\n");
    else
        LOG_DBG("===== CCU LOG DUMP END =====\n");
    #undef LOG_LEN_LIMIT
}


CcuDrvImp::CcuDrvImp()
{
    DBG_LOG_CONFIG(drv, ccu_drv);
    LOG_VRB("getpid[0x%08x],gettid[0x%08x]", getpid() ,gettid());
    m_pCcuHwRegAddr = NULL;
    pCcuMem = NULL;
}

static MINT8       *g_pLogBuf[MAX_LOG_BUF_NUM];

static CcuDrvImp    gCcuDrvObj[MAX_CCU_HW_MODULE];
CcuDrv* CcuDrvImp::createInstance(CCU_HW_MODULE module)
{
    LOG_DBG("+,module(%d)",module);
    gCcuDrvObj[module].m_HWmodule = module;
    return (CcuDrv*)&gCcuDrvObj[module];
}

#define _USER_PRINTF_CCU_LOG_

MBOOL CcuDrvImp::CloseCcuKdrv()
{
    if(this->m_Fd >= 0)
    {
        close(this->m_Fd);
        this->m_Fd = -1;
        this->m_regRWMode=CCU_DRV_R_ONLY;
    }

    return MTRUE;
}

MBOOL CcuDrvImp::OpenCcuKdrv()
{  
    this->m_Fd = open(CCU_DRV_DEV_NAME, O_RDWR);
    if (this->m_Fd < 0)    // 1st time open failed.
    {
        LOG_ERR("CCU kernel 1st open fail, errno(%d):%s.", errno, strerror(errno));
        // Try again, using "Read Only".
        this->m_Fd = open(CCU_DRV_DEV_NAME, O_RDONLY);
        if (this->m_Fd < 0) // 2nd time open failed.
        {
            LOG_ERR("CCU kernel 2nd open fail, errno(%d):%s.", errno, strerror(errno));
            return MFALSE;
        }
        else
            this->m_regRWMode=CCU_DRV_R_ONLY;
    }

    return MTRUE;
}

MBOOL CcuDrvImp::init(const char* userName, WHICH_SENSOR_T sensorDev)
{
    MBOOL Result = MTRUE;
    MUINT32 strLen;

    DBG_LOG_CONFIG(drv, ccu_drv);
    //
    android::Mutex::Autolock lock(this->m_CcuInitMutex);
    //
    LOG_INF("+,m_UserCnt(%d), curUser(%s).\n", this->m_UserCnt,userName);
    //
    strLen = strlen(userName);
    if((strLen<1)||(strLen>=MAX_USER_NAME_SIZE))
    {
        if(strLen<1)
            LOG_ERR("[Error]Plz add userName if you want to use ccu driver\n");
        else if(strLen>=MAX_USER_NAME_SIZE)
            LOG_ERR("[Error]user's userName length(%d) exceed the default length(%d)\n",strLen,MAX_USER_NAME_SIZE);
        else
            LOG_ERR("[Error]coding error, please check judgement condition\n");
        return MFALSE;
    }
    //
    if(this->m_UserCnt > 0)
    {
        if(this->m_UserCnt < MAX_USER_NUMBER){
            strcpy((char*)this->m_UserName[this->m_UserCnt],userName);
            android_atomic_inc(&this->m_UserCnt);
            LOG_INF(" - X. m_UserCnt: %d,UserName:%s", this->m_UserCnt,userName);
            return Result;
        }
        else{
            LOG_ERR("m_userCnt is over upper bound\n");
            return MFALSE;
        }
    }
    // Open ccu device
    if (!(this->OpenCcuKdrv()))
    {
        Result = MFALSE;
        goto EXIT;
    }
    else    // 1st time open success.   // Sometimes GDMA will go this path, too. e.g. File Manager -> Phone Storage -> Photo.
    {
        LOG_DBG("CCU kernel open ok, fd:%d.", this->m_Fd);
        // fd opened only once at the very 1st init

        int32_t get_Platform_info_ret;
        get_Platform_info_ret = ioctl(this->m_Fd, CCU_IOCTL_GET_PLATFORM_INFO, &m_platformInfo);
        if(get_Platform_info_ret < 0)
        {
            LOG_ERR("CCU_IOCTL_GET_PLATFORM_INFO: %d\n", get_Platform_info_ret);
            goto EXIT;
        }
        else
        {
            LOG_DBG_MUST("ccu read dt property ccu_hw_base = %x\n", m_platformInfo.ccu_hw_base);
            LOG_DBG_MUST("ccu read dt property ccu_hw_offset = %x\n", m_platformInfo.ccu_hw_offset);
            LOG_DBG_MUST("ccu read dt property ccu_pmem_base = %x\n", m_platformInfo.ccu_pmem_base);
            LOG_DBG_MUST("ccu read dt property ccu_pmem_size = %x\n", m_platformInfo.ccu_pmem_size);
            LOG_DBG_MUST("ccu read dt property ccu_dmem_base = %x\n", m_platformInfo.ccu_dmem_base);
            LOG_DBG_MUST("ccu read dt property ccu_dmem_size = %x\n", m_platformInfo.ccu_dmem_size);
            LOG_DBG_MUST("ccu read dt property ccu_dmem_offset = %x\n", m_platformInfo.ccu_dmem_offset);
            LOG_DBG_MUST("ccu read dt property ccu_log_base = %x\n", m_platformInfo.ccu_log_base);
            LOG_DBG_MUST("ccu read dt property ccu_log_size = %x\n", m_platformInfo.ccu_log_size);
            LOG_DBG_MUST("ccu read dt property ccu_hw_dump_size = %x\n", m_platformInfo.ccu_hw_dump_size);
            LOG_DBG_MUST("ccu read dt property ccu_camsys_base = %x\n", m_platformInfo.ccu_camsys_base);
            LOG_DBG_MUST("ccu read dt property ccu_camsys_size = %x\n", m_platformInfo.ccu_camsys_size);
            LOG_DBG_MUST("ccu read dt property ccu_n3d_a_base = %x\n", m_platformInfo.ccu_n3d_a_base);
            LOG_DBG_MUST("ccu read dt property ccu_n3d_a_size = %x\n", m_platformInfo.ccu_n3d_a_size);
            LOG_DBG_MUST("ccu read dt property ccu_sensor_pm_size = %x\n", m_platformInfo.ccu_sensor_pm_size);
            LOG_DBG_MUST("ccu read dt property ccu_sensor_dm_size = %x\n", m_platformInfo.ccu_sensor_dm_size);
        }


        LOG_DBG("MMAP CCU HW Addrs.");
        switch(this->m_HWmodule)
        {
            case CCU_A:
            case CCU_CAMSYS:
				/* MUST: CCU_A_BASE_HW should be 4K alignment, otherwise mmap got error(22):invalid argument!! */
                LOG_DBG_MUST("CCU_A_BASE_HW: %x", CCU_A_BASE_HW);
                gCcuDrvObj[CCU_A].m_pCcuHwRegAddr 		= (MUINT32 *) mmap(0, CCU_BASE_RANGE, (PROT_READ | PROT_WRITE | PROT_NOCACHE), MAP_SHARED, this->m_Fd, CCU_A_BASE_HW - CCU_A_BASE_HW_ALIGN_OFFST);
                gCcuDrvObj[CCU_CAMSYS].m_pCcuHwRegAddr 	= (MUINT32 *) mmap(0, CCU_BASE_RANGE, (PROT_READ | PROT_WRITE | PROT_NOCACHE), MAP_SHARED, this->m_Fd, CCU_CAMSYS_BASE_HW);
                gCcuDrvObj[CCU_PMEM].m_pCcuHwRegAddr 	= (MUINT32 *) mmap(0, CCU_PMEM_RANGE, (PROT_READ | PROT_WRITE | PROT_NOCACHE), MAP_SHARED, this->m_Fd, CCU_PMEM_BASE_HW);
                gCcuDrvObj[CCU_DMEM].m_pCcuHwRegAddr 	= (MUINT32 *) mmap(0, CCU_DMEM_RANGE, (PROT_READ | PROT_WRITE | PROT_NOCACHE), MAP_SHARED, this->m_Fd, CCU_DMEM_BASE_HW);
#if 0
                g_pLogBuf[0] = (MINT8 *) mmap(0, SIZE_1MB, (PROT_READ | PROT_WRITE | PROT_NOCACHE), MAP_SHARED, this->m_Fd, CCU_MMAP_LOG0);
                g_pLogBuf[1] = (MINT8 *) mmap(0, SIZE_1MB, (PROT_READ | PROT_WRITE | PROT_NOCACHE), MAP_SHARED, this->m_Fd, CCU_MMAP_LOG1);
                g_pLogBuf[2] = (MINT8 *) mmap(0, SIZE_1MB, (PROT_READ | PROT_WRITE | PROT_NOCACHE), MAP_SHARED, this->m_Fd, CCU_MMAP_LOG2);

                LOG_INF("mmap CCU_MMAP_LOG0:%p\n",g_pLogBuf[0]);
                LOG_INF("mmap CCU_MMAP_LOG1:%p\n",g_pLogBuf[1]);
                LOG_INF("mmap CCU_MMAP_LOG2:%p\n",g_pLogBuf[2]);
#endif
                gCcuDrvObj[CCU_A].m_pCcuHwRegAddr = gCcuDrvObj[CCU_A].m_pCcuHwRegAddr + (CCU_A_BASE_HW_ALIGN_OFFST/4);
                LOG_INF("\n");
				LOG_INF("mmap CCU_A:%p\n",      gCcuDrvObj[CCU_A].m_pCcuHwRegAddr);
				LOG_INF("mmap CCU_CAMSYS:%p\n", gCcuDrvObj[CCU_CAMSYS].m_pCcuHwRegAddr);
				LOG_INF("mmap CCU_PMEM:%p\n",   gCcuDrvObj[CCU_PMEM].m_pCcuHwRegAddr);
				LOG_INF("mmap CCU_DMEM:%p\n",   gCcuDrvObj[CCU_DMEM].m_pCcuHwRegAddr);

                break;
            default:
                LOG_ERR("#############\n");
                LOG_ERR("this hw module(%d) is unsupported\n",this->m_HWmodule);
                LOG_ERR("#############\n");
                goto EXIT;
                break;
        }

        if(gCcuDrvObj[CCU_A].m_pCcuHwRegAddr == MAP_FAILED)
        {
            LOG_ERR("CCU mmap fail , errno(%d):%s",errno, strerror(errno));
			LOG_ERR("(%x)(%x)\n",CCU_BASE_RANGE,CCU_A_BASE_HW);
            Result = MFALSE;
            goto EXIT;
        }

        this->m_regRWMode = CCU_DRV_RW_MMAP;

        LOG_DBG("Allocate CCU regmap buffer");
		//map reg map for R/W Macro, in order to calculate addr-offset by reg name
		if(this->m_pCcuRegMap == NULL){
		    this->m_pCcuRegMap = (void*)malloc(MAX_MODULE_SIZE());
		}
		else{
		    LOG_ERR("re-allocate RegMap\n");
		}
    }

    //
    LOG_DBG("+:pCcuMem\n");
    this->pCcuMem = (CcuMemImp*)CcuMemImp::createInstance();
	if ( NULL == this->pCcuMem ) {
		LOG_ERR("createInstance pCcuMem return %p", pCcuMem);
		return false;
	}
    this->pCcuMem->init("CCU_UDRV");
    LOG_DBG("-:pCcuMem\n");

    CcuAeeMgrDl::CcuAeeMgrDlInit();

    //allocate ccu buffers
    this->allocateBuffers();

	//power on
	LOG_DBG("+:CCU_IOCTL_SET_POWER\n");
	m_power.bON = 1;
	m_power.freq = 500;
	m_power.power = 15;
    //<<<<<<<<<< debug 32/64 compat check
    LOG_DBG("[IOCTL_DBG] ccu_power_t size: %d\n", sizeof(ccu_power_t));
    LOG_DBG("[IOCTL_DBG] ccu_working_buffer_t size: %d\n", sizeof(ccu_working_buffer_t));
    LOG_DBG("[IOCTL_DBG] m_power addr: %p\n", &m_power);
    LOG_DBG("[IOCTL_DBG] long size: %d\n", sizeof(long));
    LOG_DBG("[IOCTL_DBG] long long: %d\n", sizeof(long long));
    LOG_DBG("[IOCTL_DBG] char * size: %d\n", sizeof(char *));
    LOG_DBG("[IOCTL_DBG] m_power.workBuf.va_log[0]: %p\n", m_power.workBuf.va_log[0]);
    //>>>>>>>>>> debug 32/64 compat check
    if(ioctl(this->m_Fd, CCU_IOCTL_SET_POWER, &m_power) < 0)
    {
        LOG_ERR("CCU_IOCTL_SET_POWER: %d, bON(%d)\n", this->m_HWmodule, m_power.bON);
        Result = MFALSE;
        goto EXIT;
    }
    LOG_DBG("-:CCU_IOCTL_SET_POWER\n");
    //
	//set interrupt mode to read clear mode
//	LOG_DBG("+:set interrupt mode to read clear mode\n");
//	CCU_WRITE_BITS(this,EINTC_MASK, CCU_EINTC_MASK,0x000FF); //read clear mode
//    LOG_DBG("-:set interrupt mode to read clear mode\n");

	//load binary here
	LOG_DBG("+:loadCCUBin\n");
	if ( !this->loadCCUBin(sensorDev) ) {
		LOG_ERR("load binary file fail\n");
        Result = MFALSE;
        goto EXIT;
	}
    LOG_DBG("-:loadCCUBin\n");

#if defined( _USER_PRINTF_CCU_LOG_ )
    LOG_DBG("+:_USER_PRINTF_CCU_LOG_ create thread\n");
    this->createThread();
    LOG_DBG("-:_USER_PRINTF_CCU_LOG_ create thread\n");
#endif

    //usleep(10000);
	//
	LOG_DBG("+:CCU_IOCTL_SET_RUN\n");
    if(ioctl(this->m_Fd, CCU_IOCTL_SET_RUN) < 0){
        LOG_ERR("CCU_IOCTL_SET_RUN: %d\n", this->m_HWmodule);
        Result = MFALSE;
        goto EXIT;
    }
    LOG_DBG("-:CCU_IOCTL_SET_RUN\n");
	//

	strcpy((char*)this->m_UserName[this->m_UserCnt],userName);
	android_atomic_inc(&this->m_UserCnt);

EXIT:

    //
    if (!Result)    // If some init step goes wrong.
    {
        this->RealUninit();
        /*if(this->m_Fd >= 0)
        {
            // unmap to avoid memory leakage
            for(MUINT32 i=0;i<MAX_CCU_HW_MODULE;i++){
                if(gCcuDrvObj[i].m_pCcuHwRegAddr != MAP_FAILED){
                    int ret = 0;
                    switch(i){
                        case CCU_A:
                            LOG_DBG("++Init_CCU_A munmap fail: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                            gCcuDrvObj[i].m_pCcuHwRegAddr = gCcuDrvObj[i].m_pCcuHwRegAddr - (CCU_A_BASE_HW_ALIGN_OFFST/4);
                            LOG_DBG("--Init_CCU_A munmap fail: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                        case CCU_CAMSYS:
                            ret = munmap(gCcuDrvObj[i].m_pCcuHwRegAddr, CCU_BASE_RANGE);
                            if (ret < 0) {
                                LOG_ERR("Init_CCU_CAMSYS_munmap fail: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                                break;
                            }
                            gCcuDrvObj[i].m_pCcuHwRegAddr = NULL;
                            break;
						case CCU_PMEM:
							ret = munmap(gCcuDrvObj[i].m_pCcuHwRegAddr, CCU_PMEM_RANGE);
							if (ret < 0) {
								LOG_ERR("Init_CCU_PMEM_munmap fail: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
								break;
							}
							gCcuDrvObj[i].m_pCcuHwRegAddr = NULL;
							break;
						case CCU_DMEM:
							ret = munmap(gCcuDrvObj[i].m_pCcuHwRegAddr, CCU_DMEM_RANGE);
							if (ret < 0) {
								LOG_ERR("Init_CCU_DMEM_munmap fail: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
								break;
							}
							gCcuDrvObj[i].m_pCcuHwRegAddr = NULL;
							break;

						default:
                            LOG_ERR("#############\n");
                            LOG_ERR("this hw module(%d) is unsupported\n",i);
                            LOG_ERR("#############\n");
                            break;
                    }
                }
                else
                    gCcuDrvObj[i].m_pCcuHwRegAddr = NULL;
            }

            close(this->m_Fd);
            this->m_Fd = -1;
            LOG_INF("close ccu device Fd");
            
        }
        */
    }


    LOG_DBG("-,ret: %d. mInitCount:(%d),m_pCcuHwRegAddr(0x%x)\n", Result, this->m_UserCnt,gCcuDrvObj[CCU_A].m_pCcuHwRegAddr);
    return Result;
}

MBOOL CcuDrvImp::allocateBuffers()
{
    LOG_DBG("+:%s\n",__FUNCTION__);

    //allocate log buffers
    for (int i=0; i<MAX_LOG_BUF_NUM; i++ )
    {
        m_power.workBuf.sz_log[i] = SIZE_1MB;

    	this->pCcuMem->ccu_malloc( this->pCcuMem->gIonDevFD, m_power.workBuf.sz_log[i], &m_power.workBuf.fd_log[i], &m_power.workBuf.va_log[i]);
    	if ( NULL == m_power.workBuf.va_log[i] )
        {
    		LOG_ERR("ccu_malloc %p", m_power.workBuf.va_log[i]);
    		return false;
    	}

        g_pLogBuf[i] = (MINT8*)m_power.workBuf.va_log[i];
        memset((char*)g_pLogBuf[i],0x00,SIZE_1MB);
        sprintf((char*)g_pLogBuf[i],"g_pLogBuf[%d]\n",i);
    	//this->pCcuMem->mmapMVA_m4u(m_power.workBuf.va_log[i], &m_power.workBuf.mva_log[i], m_power.workBuf.sz_log[i]);
        this->pCcuMem->mmapMVA( m_power.workBuf.fd_log[i], &m_logBufIonHandle[i], &m_power.workBuf.mva_log[i]);
        if ( 0 == m_power.workBuf.mva_log[i] ) {
            LOG_ERR("mmapMVA(m_power.workBuf.mva_log[%d]) Error: %x", i , m_power.workBuf.mva_log[i]);
            return false;
        }

        LOG_DBG("CCU_LOG(%d) buffer:va(%p),mva(%x),sz(%d)\n", i, m_power.workBuf.va_log[i],m_power.workBuf.mva_log[i], m_power.workBuf.sz_log[i]);
    }

    LOG_DBG("-:%s\n",__FUNCTION__);

    return MTRUE;
}

//-----------------------------------------------------------------------------
MBOOL CcuDrvImp::shutdown()
{
    LOG_DBG_MUST("+:%s\n",__FUNCTION__);

    /*ccu_drv_DbgLogEnable_VERBOSE = true;
    ccu_drv_DbgLogEnable_DEBUG = true;
    ccu_drv_DbgLogEnable_INFO = true;
    ccu_drv_DbgLogEnable_WARN = true;
    ccu_drv_DbgLogEnable_ERROR = true;
    ccu_drv_DbgLogEnable_ASSERT = true;*/

	//power off
	// 1.send MSG_TO_CCU_SHUTDOWN command
    {
        ccu_cmd_st cmd_t = { {MSG_TO_CCU_SHUTDOWN, 0, 0}, CCU_ENG_STATUS_INVALID };
    	ccu_cmd_st cmd_r;
      	if ( !this->sendCmd( &cmd_t, &cmd_r, true ) ) {
    		LOG_ERR("cmd(%d) fail \n",cmd_t.task.msg_id);
    		return MFALSE;
    	}
    }
    LOG_DBG_MUST("shutdown cmd done.\n",__FUNCTION__);
    // 2. power off
	m_power.bON = 0;
    if(ioctl(this->m_Fd, CCU_IOCTL_SET_POWER, &m_power) < 0){
        LOG_ERR("CCU_IOCTL_SET_POWER: %d, bON(%d)\n", this->m_HWmodule, m_power.bON);
        return MFALSE;
    }

    /*ccu_drv_DbgLogEnable_VERBOSE = false;
    ccu_drv_DbgLogEnable_DEBUG = false;
    ccu_drv_DbgLogEnable_INFO = false;
    ccu_drv_DbgLogEnable_WARN = false;
    ccu_drv_DbgLogEnable_ERROR = false;
    ccu_drv_DbgLogEnable_ASSERT = false;*/

    LOG_DBG_MUST("-:%s\n\n",__FUNCTION__);

    return MTRUE;
}

MBOOL CcuDrvImp::RealUninit(const char* userName)
{
    MBOOL Result = MTRUE;
    //
    LOG_INF_MUST("+,m_UserCnt(%d),curUser(%s)\n", this->m_UserCnt, userName);
    //
#if defined( _USER_PRINTF_CCU_LOG_ )
    LOG_DBG_MUST("+:release thread\n");
    if(this->m_taskWorkerThreadReady == MTRUE)
    {
        this->destroyThread();
    }
    LOG_DBG_MUST("-:release thread\n");
#endif
    //
    for(MUINT32 i=0;i<MAX_CCU_HW_MODULE;i++){
        if(gCcuDrvObj[i].m_pCcuHwRegAddr != MAP_FAILED && gCcuDrvObj[i].m_pCcuHwRegAddr != NULL)
        {
            int ret = 0;
            switch(i){
                case CCU_A:
                    LOG_DBG_MUST("++Uinit_CCU_A munmap: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                    gCcuDrvObj[i].m_pCcuHwRegAddr = gCcuDrvObj[i].m_pCcuHwRegAddr - (CCU_A_BASE_HW_ALIGN_OFFST/4);
                    LOG_DBG_MUST("--Uinit_CCU_A munmap: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                case CCU_CAMSYS:
                    LOG_DBG_MUST("Uinit_CCU_CAMSYS munmap: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                    ret = munmap(gCcuDrvObj[i].m_pCcuHwRegAddr, CCU_BASE_RANGE);
                    if (ret < 0) {
                        LOG_ERR("Uinit_CCU_CAMSYS munmap fail: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                        break;
                    }
                    gCcuDrvObj[i].m_pCcuHwRegAddr = NULL;
                    break;
                case CCU_PMEM:
                    ret = munmap(gCcuDrvObj[i].m_pCcuHwRegAddr, CCU_PMEM_RANGE);
                    if (ret < 0) {
                        LOG_ERR("Uinit_CCU_PMEM munmap fail: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                        break;
                    }
                    gCcuDrvObj[i].m_pCcuHwRegAddr = NULL;
                    break;
                case CCU_DMEM:
                    ret = munmap(gCcuDrvObj[i].m_pCcuHwRegAddr, CCU_DMEM_RANGE);
                    if (ret < 0) {
                        LOG_ERR("Uinit_CCU_DMEM munmap fail: %p\n", gCcuDrvObj[i].m_pCcuHwRegAddr);
                        break;
                    }
                    gCcuDrvObj[i].m_pCcuHwRegAddr = NULL;
                    break;
                default:
                    LOG_ERR("#############\n");
                    LOG_ERR("this hw module(%d) is unsupported\n",i);
                    LOG_ERR("#############\n");
                    break;
            }
        }
    }

    //
    if(this->m_pCcuRegMap){
        free((MUINT32*)this->m_pCcuRegMap);
        this->m_pCcuRegMap = NULL;
    }

    //
    LOG_DBG_MUST("+:release buffer\n");
    for (int i=0; i<MAX_LOG_BUF_NUM; i++ )
    {
        // if ( false == this->pCcuMem->munmapMVA_m4u( m_power.workBuf.va_log[i], m_power.workBuf.mva_log[i], m_power.workBuf.sz_log[i] ) ) {
        //     LOG_ERR("munmapMVA_m4u fail:m4u\n");
        // }

        //
        if ( false == this->pCcuMem->munmapMVA(m_logBufIonHandle[i]))
        {
            LOG_ERR("munmapMVA fail:ion_handle_in_data\n");
        }
        if ( false == this->pCcuMem->ccu_free( this->pCcuMem->gIonDevFD, m_power.workBuf.sz_log[i], m_power.workBuf.fd_log[i], m_power.workBuf.va_log[i]) ) {
            LOG_ERR("ccu_free_buffer fail:buf_va_in_data\n");
        }

        g_pLogBuf[i] = NULL;
    }
    if ( this->pCcuMem )
    {
        this->pCcuMem->uninit("CCU_UDRV");
        this->pCcuMem->destroyInstance();
    }

    CcuAeeMgrDl::CcuAeeMgrDlUninit();
    
    //
    this->CloseCcuKdrv();

    // More than one user
    android_atomic_dec(&this->m_UserCnt);
    //
EXIT:

    if(this->m_UserCnt!= 0){
        for(MUINT32 i=0;i<MAX_USER_NUMBER;i+=8) {
            if(this->m_UserCnt > (MINT32)i) {
                LOG_INF("current user[%d]: %s, %s, %s, %s, %s, %s, %s, %s\n"    \
                ,i,this->m_UserName[i],this->m_UserName[i+1],this->m_UserName[i+2],this->m_UserName[i+3]  \
                ,this->m_UserName[i+4],this->m_UserName[i+5],this->m_UserName[i+6],this->m_UserName[i+7]);
            }
        }
    }

    LOG_INF_MUST("CcuDrv uinitial done gracefully\n");

    LOG_INF_MUST(" - X. ret: %d. m_UserCnt: %d\n", Result, this->m_UserCnt);

    return Result;
}

MBOOL CcuDrvImp::uninit(const char* userName)
{
    android::Mutex::Autolock lock(this->m_CcuInitMutex);

    return this->RealUninit(userName);
}

#define FD_CHK()({\
    MINT32 Ret=0;\
    if(this->m_Fd < 0){\
        LOG_ERR("no ccu device\n");\
        Ret = -1;\
    }\
    Ret;\
})

MBOOL CcuDrvImp::waitIrq(CCU_WAIT_IRQ_ST* pWaitIrq)
{
    MINT32 Ret = 0;
    MUINT32 OrgTimeOut;
    CCU_IRQ_CLEAR_ENUM OrgClr;
    CcuDbgTimer dbgTmr("waitIrq");
    MUINT32 Ta=0,Tb=0;
    CCU_WAIT_IRQ_STRUCT wait;

    LOG_DBG(" + E. hwmodule:0x%x,Status(0x%08x),Timeout(%d).\n",this->m_HWmodule, pWaitIrq->Status, pWaitIrq->Timeout);

    OrgTimeOut = pWaitIrq->Timeout;
    OrgClr = pWaitIrq->Clear;
    switch(this->m_HWmodule){
        case CCU_A:     wait.Type = CCU_IRQ_TYPE_INT_CCU_A_ST;
            break;
        case CCU_CAMSYS:     wait.Type = CCU_IRQ_TYPE_INT_CCU_B_ST;
            break;
        default:
            LOG_ERR("unsupported hw module:0x%x\n",this->m_HWmodule);
            return MFALSE;
            break;
    }

    memcpy(&wait.EventInfo,pWaitIrq,sizeof(CCU_WAIT_IRQ_ST));

    LOG_DBG("CCU_IOCTL_WAIT_IRQ ioctl call, arg is CCU_WAIT_IRQ_STRUCT, size: %d\n", sizeof(CCU_WAIT_IRQ_STRUCT));

    Ta=dbgTmr.getUs();
    Ret = ioctl(this->m_Fd,CCU_IOCTL_WAIT_IRQ,&wait);
    Tb=dbgTmr.getUs();

    memcpy(&pWaitIrq->TimeInfo,&wait.EventInfo.TimeInfo,sizeof(CCU_IRQ_TIME_STRUCT));
    pWaitIrq->Timeout = OrgTimeOut;
    pWaitIrq->Clear = OrgClr;


    if(Ret < 0) {
        LOG_ERR("CCU(0x%x)_WAIT_IRQ fail(%d). Wait Status(0x%08x), Timeout(%d).\n",this->m_HWmodule, Ret,  pWaitIrq->Status, pWaitIrq->Timeout);
        return MFALSE;
    }


    return MTRUE;
}

MBOOL CcuDrvImp::clearIrq(CCU_CLEAR_IRQ_ST* pClearIrq)
{
    MINT32 Ret;
    CCU_CLEAR_IRQ_STRUCT clear;
    //
    LOG_DBG(" - E. hw module:0x%x,user(%d), Status(%d)\n",this->m_HWmodule,pClearIrq->UserKey, pClearIrq->Status);
    if(FD_CHK() == -1){
        return MFALSE;
    }
    switch(this->m_HWmodule){
        case CCU_A:     clear.Type = CCU_IRQ_TYPE_INT_CCU_A_ST;
            break;
        case CCU_CAMSYS:     clear.Type = CCU_IRQ_TYPE_INT_CCU_B_ST;
            break;
        default:
            LOG_ERR("unsupported hw module:0x%x\n",this->m_HWmodule);
            return MFALSE;
            break;
    }
    //
    memcpy(&clear.EventInfo,pClearIrq,sizeof(CCU_CLEAR_IRQ_ST));
    Ret = ioctl(this->m_Fd,CCU_CLEAR_IRQ,&clear);
    if(Ret < 0)
    {
        LOG_ERR("CCU(0x%x)_CLEAR_IRQ fail(%d)\n",this->m_HWmodule,Ret);
        return MFALSE;
    }
    return MTRUE;
}

MBOOL CcuDrvImp::registerIrq(CCU_REGISTER_USERKEY_STRUCT* pRegIrq)
{
    MINT32 Ret;
    LOG_DBG(" - E. hw module:0x%x,userkey(%d), name(%s)\n",this->m_HWmodule,pRegIrq->userKey, pRegIrq->userName);
    //
    Ret = ioctl(this->m_Fd,CCU_REGISTER_IRQ_USER_KEY,pRegIrq);
    if(Ret < 0) {
        LOG_ERR("CCU_REGISTER_IRQ fail(%d). hw module:0x%x, userkey(%d), name(%s\n", Ret, this->m_HWmodule, pRegIrq->userKey, pRegIrq->userName);
        return MFALSE;
    }
    return MTRUE;
}

MUINT32 CcuDrvImp::readInfoReg(MUINT32 regNo)
{
    MINT32 Ret;

    Ret = ioctl(this->m_Fd,CCU_READ_REGISTER, (void *)(uint64_t)regNo);

    return Ret;
}

MBOOL CcuDrvImp::sendCmd( ccu_cmd_st *pCMD_T, ccu_cmd_st *pCMD_R, MBOOL bSync )
{
    //ccu_cmd_st cmd;
    //ccu_cmd_st cmd_get;

    //::memcpy(&cmd, pCMD, sizeof(ccu_cmd_st) );

    LOG_INF("CCU_IOCTL_ENQUE_COMMAND: msg(%d), in(%p), out(%p), st(%d)\n",
                pCMD_T->task.msg_id, \
                pCMD_T->task.in_data_ptr, \
                pCMD_T->task.out_data_ptr, \
                pCMD_T->status);

    if(ioctl(this->m_Fd, CCU_IOCTL_ENQUE_COMMAND, pCMD_T) < 0)
    {
        LOG_ERR("ERROR: CCU_IOCTL_ENQUE_COMMAND:%x\n",pCMD_T->task.msg_id);
        return MFALSE;
    }

	if ( bSync )
    {
		LOG_INF("CCU_IOCTL_DEQUE_COMMAND wait ack...\n");
		if(ioctl(this->m_Fd, CCU_IOCTL_DEQUE_COMMAND, pCMD_R) < 0)
		{
		    LOG_ERR("ERROR: CCU_IOCTL_DEQUE_COMMAND:%x\n",pCMD_T->task.msg_id);
		    return MFALSE;;
		}

		//LOG_INF("(%d),(%p),(%p),(%x),(%x),(%d), \n",cmd_get.task.msg_id,
		LOG_INF("CCU_IOCTL_DEQUE_COMMAND ack: msg(%d)->ack(%d), in(%p), out(%p), st(%d), \n",
		        pCMD_T->task.msg_id, \
                pCMD_R->task.msg_id, \
                pCMD_R->task.in_data_ptr, \
                pCMD_R->task.out_data_ptr, \
                //cmd_get.task.sz_in_data,
                //cmd_get.task.sz_out_data,
                pCMD_R->status);

        if(pCMD_R->status == CCU_ENG_STATUS_TIMEOUT)
        {
            LOG_WRN("ERROR: CCU_IOCTL_DEQUE_COMMAND TIMEOUT:%x\n",pCMD_T->task.msg_id);
            LOG_WRN("============ CCU TIMEOUT LOG DUMP: LOGBUF[0] =============\n");
            printCcuLog((const char*)g_pLogBuf[0], MTRUE);
            LOG_WRN("============ CCU TIMEOUT LOG DUMP: LOGBUF[1] =============\n");
            printCcuLog((const char*)g_pLogBuf[1], MTRUE);
            DumpSramLog();
            PrintReg();
            AEE_ASSERT_CCU_USER("CCU WAIT CMD(%d) ACK TIMEOUT\n", pCMD_T->task.msg_id);
        }
	}

	return true;

}

MBOOL CcuDrvImp::waitCmd( ccu_cmd_st *pCMD_R )
{
	//ccu_cmd_st cmd_get;

	LOG_INF("CCU_IOCTL_DEQUE_COMMAND\n");
	if(ioctl(this->m_Fd, CCU_IOCTL_DEQUE_COMMAND, pCMD_R) < 0)
	{
		LOG_ERR("ERROR: CCU_IOCTL_DEQUE_COMMAND:%x\n",pCMD_R->task.msg_id);
		return MFALSE;;
	}

	//::memcpy( pCMD, &cmd_get, sizeof(ccu_cmd_st) );

	//LOG_INF("(%d),(%p),(%p),(%x),(%x),(%d), \n",cmd_get.task.msg_id,
	LOG_INF("CCU_IOCTL_DEQUE_COMMAND result: (%d),(%p),(%p),(%d), \n",pCMD_R->task.msg_id, \
				  pCMD_R->task.in_data_ptr, \
				  pCMD_R->task.out_data_ptr, \
				  //cmd_get.task.sz_in_data,
				  //cmd_get.task.sz_out_data,
				  pCMD_R->status);

	return true;
}

FILE *tryOpenFile(char *path1, char *path2=NULL)
{
    FILE *pFile;
    FILE *pFile_empty;

    LOG_INF("open file, path: %s\n", path1);
    pFile = fopen(path1, "rwb");

    if(pFile == NULL)
    {
        LOG_ERR("open file fail: %d\n", pFile);
        LOG_ERR("open file path: %s\n", path1);

        if(path2 != NULL)
        {
            LOG_ERR("open file, path2: %s\n", path2);
            pFile = fopen(path2, "rwb");
            if(pFile == NULL)
            {
                LOG_ERR("open file fail_2: %d\n", pFile);
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }
    pFile_empty = pFile;

    if(fgetc(pFile_empty) == EOF)
    {
        LOG_ERR("Empty file\n");
        fclose(pFile_empty);
        return NULL;
    }
    return pFile;
}

size_t loadFileToBuffer(FILE *fp, unsigned int *buf)
{
    size_t sz;

    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    rewind(fp);
    LOG_INF("size=%d\n",sz);
    LOG_INF("read file into buffer");
    fread ( (void*) buf, sz, sizeof(char), fp );
    LOG_INF("read file done");

    return sz;
}

void clearAndLoadBinToMem(unsigned int *memBaseAddr, unsigned int *buf, unsigned int memSize, unsigned int binSize, unsigned int offset)
{
    volatile int dummy = 0;

    LOG_INF_MUST("clear MEM");
    /*Must clear mem with 4 byte aligned access, due to APMCU->CCU access path APB restriction*/
    for (unsigned int i=0;i<memSize/4;i++)
    {
        //for HW Test tool, must add a dummy operation between mem access, otherwise incurrs SIGBUS/BUS_ADRALN
        //root casue under checking...
        dummy = dummy + 1;
        *(memBaseAddr + i) = 0;
    }

    LOG_INF("load bin buffer onto MEM");
    LOG_INF("%x,%x,%x\n",buf[0],buf[1],buf[2]);

    for (unsigned int i=0;i<binSize/4;i++) {
        *(memBaseAddr + (offset/4) + i) = buf[i];
    }
}


MBOOL CcuDrvImp::loadCCUBin(WHICH_SENSOR_T sensorDev)
{
    FILE *pFile;
    size_t sz;
    unsigned int *pmem = (unsigned int*)malloc(CCU_PMEM_SIZE);
    unsigned int *dmem = (unsigned int*)malloc(CCU_DMEM_SIZE);
    unsigned int *sensorPm = (unsigned int*)malloc(SENSOR_PM_SIZE);
    unsigned int *sensorDm = (unsigned int*)malloc(SENSOR_DM_SIZE);

    char *pmPath = "/system/vendor/bin/libccu_bin.pm";
    char *dmPath = "/system/vendor/bin/libccu_bin.dm";
    char *pmPath2 = "/system/bin/libccu_bin.pm";
    char *dmPath2 = "/system/bin/libccu_bin.dm";
    char *sensorPathPrefix = "/system/vendor/bin/libccu_";

    char sensorName[100] = "";
    char sensorPmPath[100] = "";
    char sensorDmPath[100] = "";

    //========================= Get sensor name via ioctl ===========================
    this->GetSensorName(this->m_Fd, sensorDev, sensorName);
    LOG_DBG_MUST("ccu sensor name: %s\n", sensorName);

    strcat(sensorPmPath, sensorPathPrefix);
    strcat(sensorPmPath, sensorName);
    strcat(sensorPmPath, ".pm");

    strcat(sensorDmPath, sensorPathPrefix);
    strcat(sensorDmPath, sensorName);
    strcat(sensorDmPath, ".dm");

    //========================= Load PMEM binary ===========================
    LOG_INF("open PMEM file, path: %s\n", pmPath);
	pFile = tryOpenFile(pmPath, pmPath2);
    LOG_INF("open PMEM file result:%d\n", pFile);
	if (pFile == NULL) { LOG_ERR("Open ccu_bin.pm fail\n"); return MFALSE; }

    LOG_INF("read PMEM file into pmem buffer\n");
	sz = loadFileToBuffer(pFile, pmem);
    fclose(pFile);
    LOG_INF("read PMEM done\n");

    LOG_INF("clear CCU PMEM & load pmem buffer onto CCU PMEM\n");
	clearAndLoadBinToMem(gCcuDrvObj[CCU_PMEM].m_pCcuHwRegAddr, pmem, CCU_PMEM_SIZE, sz, 0);
    LOG_INF("load pmem buffer onto CCU PMEM done\n");

    //========================= Load DMEM binary ===========================
    LOG_INF("open DMEM file, path: %s\n", dmPath);
    pFile = tryOpenFile(dmPath, dmPath2);
    LOG_INF("open DMEM file result:%d\n", pFile);
    if (pFile == NULL) { LOG_ERR("Open ccu_bin.dm fail\n"); return MFALSE; }

    LOG_INF("read DMEM file into dmem buffer\n");
    sz = loadFileToBuffer(pFile, dmem);
    fclose(pFile);
    LOG_INF("read DMEM done\n");

    LOG_INF("clear CCU DMEM & load dmem buffer onto CCU DMEM\n");
    clearAndLoadBinToMem(gCcuDrvObj[CCU_DMEM].m_pCcuHwRegAddr, dmem, CCU_DMEM_SIZE, sz, CCU_DMEM_OFFSET);
    LOG_INF("load dmem buffer onto CCU DMEM done\n");

    //========================= Load Sensor PM binary ===========================
    LOG_INF("open sensorPm file, path: %s\n", sensorPmPath);
    pFile = tryOpenFile(sensorPmPath);
    LOG_INF("open sensorPm file result:%d\n", pFile);
    if (pFile == NULL) { LOG_ERR("Open sensorPm fail\n"); return MFALSE; }

    LOG_INF("read sensorPm file into sensorBin buffer\n");
    sz = loadFileToBuffer(pFile, sensorPm);
    fclose(pFile);
    LOG_INF("read sensorPm done\n");

    LOG_INF("clear CCU sensorPm & load sensorPm buffer onto CCU sensorPm\n");
    clearAndLoadBinToMem(gCcuDrvObj[CCU_PMEM].m_pCcuHwRegAddr, sensorPm, 0, sz, SENSOR_PM_OFFSET);
    LOG_INF("load sensorPm buffer onto CCU sensorPm done\n");

    //========================= Load Sensor DM binary ===========================
    LOG_INF("open sensorDm file, path: %s\n", sensorDmPath);
    pFile = tryOpenFile(sensorDmPath);
    LOG_INF("open sensorDm file result:%d\n", pFile);
    if (pFile == NULL) { LOG_ERR("Open sensorDm fail\n"); return MFALSE; }

    LOG_INF("read sensorDm file into sensorBin buffer\n");
    sz = loadFileToBuffer(pFile, sensorDm);
    fclose(pFile);
    LOG_INF("read sensorDm done\n");

    LOG_INF("clear CCU sensorDm & load sensorDm buffer onto CCU sensorDm\n");
    clearAndLoadBinToMem(gCcuDrvObj[CCU_DMEM].m_pCcuHwRegAddr, sensorDm, 0, sz, SENSOR_DM_OFFSET);
    LOG_INF("load sensorDm buffer onto CCU sensorDm done\n");

	if (pmem)
    {
		free((MUINT32*)pmem);
	}
	if (dmem)
    {
		free((MUINT32*)dmem);
	}
    if (sensorPm)
    {
        free((MUINT32*)sensorPm);
    }
    if (sensorDm)
    {
        free((MUINT32*)sensorDm);
    }
	return MTRUE;
}

MBOOL CcuDrvImp::checkSensorSupport(WHICH_SENSOR_T sensorDev)
{
    FILE *pFile;
    char *sensorPathPrefix = "/system/vendor/bin/libccu_";
    char sensorName[100] = "";
    char sensorPmPath[100] = "";
    char sensorDmPath[100] = "";

    //========================= try open CCU kdrv ===========================
    if (!(this->OpenCcuKdrv()))
    {
        LOG_ERR("OpenCcuKdrv Fail\n");
        goto FAIL_EXIT;
    }

    //========================= Get sensor name via ioctl ===========================
    this->GetSensorName(this->m_Fd, sensorDev, sensorName);
    LOG_DBG_MUST("ccu sensor name: %s\n", sensorName);

    strcat(sensorPmPath, sensorPathPrefix);
    strcat(sensorPmPath, sensorName);
    strcat(sensorPmPath, ".pm");

    strcat(sensorDmPath, sensorPathPrefix);
    strcat(sensorDmPath, sensorName);
    strcat(sensorDmPath, ".dm");

    //========================= Try Open Sensor PMEM binary ===========================
    LOG_INF("open sensorPm file, path: %s\n", sensorPmPath);
    pFile = tryOpenFile(sensorPmPath);
    LOG_INF("open sensorPm file result:%d\n", pFile);
    if (pFile == NULL) { LOG_ERR("Open sensorPm fail\n"); goto FAIL_EXIT; }

    fclose(pFile);
    LOG_INF("read sensorDm done\n");
    
    //========================= Try Open Sensor DMEM binary ===========================
    LOG_INF("open sensorDm file, path: %s\n", sensorDmPath);
    pFile = tryOpenFile(sensorDmPath);
    LOG_INF("open sensorDm file result:%d\n", pFile);
    if (pFile == NULL) { LOG_ERR("Open sensorDm fail\n"); goto FAIL_EXIT; }

    fclose(pFile);
    LOG_INF("read sensorDm done\n");

    this->CloseCcuKdrv();
    return MTRUE;

FAIL_EXIT:
    this->CloseCcuKdrv();
    return MFALSE;
}

MBOOL CcuDrvImp::GetSensorName(MINT32 kdrvFd, WHICH_SENSOR_T sensorDev, char *sensorName)
{
    #define SENSOR_NAME_MAX_LEN 32
    char sensorNames[3][SENSOR_NAME_MAX_LEN];

    if(ioctl(kdrvFd, CCU_IOCTL_GET_SENSOR_NAME, sensorNames) < 0)
    {
        LOG_ERR("ERROR: CCU_IOCTL_GET_SENSOR_NAME\n");
        goto FAIL_EXIT;
    }

    LOG_DBG_MUST("ccu main sensor name: %s\n", sensorNames[0]);
    LOG_DBG_MUST("ccu sub sensor name: %s\n", sensorNames[1]);

    switch(sensorDev)
    {
        case WHICH_SENSOR_MAIN:
        {
            memcpy(sensorName, sensorNames[0], strlen(sensorNames[0])+1);
            break;
        }

        case WHICH_SENSOR_SUB:
        {
            memcpy(sensorName, sensorNames[1], strlen(sensorNames[1])+1);
            break;
        }

        default:
        {
            LOG_ERR("GetSensorName error, invalid sensorDev: %x\n", sensorDev);
            goto FAIL_EXIT;
        }
    }
    #undef SENSOR_NAME_MAX_LEN

    return MTRUE;

FAIL_EXIT:
    return MFALSE;
}

MBOOL CcuDrvImp::setI2CChannel(CCU_I2C_CHANNEL channel)
{
    if(ioctl(this->m_Fd, CCU_IOCTL_SET_I2C_CHANNEL, &channel) < 0)
    {
        LOG_ERR("ERROR: CCU_IOCTL_SET_I2C_CHANNEL\n");
        return MFALSE;
    }

    return MTRUE;

}

MBOOL CcuDrvImp::getI2CDmaBufferAddr(void *va)
{
    if(ioctl(this->m_Fd, CCU_IOCTL_GET_I2C_DMA_BUF_ADDR, va) < 0)
    {
        LOG_ERR("ERROR: CCU_IOCTL_GET_I2C_DMA_BUF_ADDR\n");
        return MFALSE;
    }

    return MTRUE;
}


MBOOL CcuDrvImp::setI2CMode(unsigned int i2c_write_id, unsigned int transfer_len)
{
    struct ccu_i2c_arg i2c_arg = {i2c_write_id, transfer_len};

    if(ioctl(this->m_Fd, CCU_IOCTL_SET_I2C_MODE, &i2c_arg) < 0)
    {
        LOG_ERR("ERROR: CCU_IOCTL_SET_I2C_MODE:%x, %x\n",i2c_write_id, transfer_len);
        return MFALSE;
    }

    return MTRUE;
}

int32_t CcuDrvImp::getCurrentFps(int32_t *current_fps)
{
    if(ioctl(this->m_Fd, CCU_IOCTL_GET_CURRENT_FPS, current_fps) < 0)
    {
        LOG_ERR("ERROR: CCU_IOCTL_GET_CURRENT_FPS:%x\n", *current_fps);
        return MFALSE;
    }

    return MTRUE;
}


void CcuDrvImp::createThread()
{
    LOG_DBG("+\n");

    this->m_taskWorkerThreadReady = MFALSE;
    this->m_taskWorkerThreadEnd = MFALSE;

	sem_init(&mSem, 0, 0);
	pthread_create(&mThread, NULL, ApTaskWorkerThreadLoop, this);

    LOG_DBG("ApTaskWorkerThreadLoop created, wait for m_taskWorkerThreadReady\n");
    LOG_DBG("m_taskWorkerThreadReady addr: %p\n", &(this->m_taskWorkerThreadReady));
    while(this->m_taskWorkerThreadReady != MTRUE){}
    LOG_DBG("m_taskWorkerThreadReady: %d\n", this->m_taskWorkerThreadReady);

    LOG_DBG("-\n");
}

void CcuDrvImp::destroyThread()
{
    MINT32 temp = 0;

	LOG_DBG("+\n");

    this->m_taskWorkerThreadEnd = MTRUE;
    this->m_taskWorkerThreadReady = MFALSE;

    if(ioctl(this->m_Fd, CCU_IOCTL_FLUSH_LOG, &temp) < 0)
    {
        LOG_ERR("CCU_IOCTL_FLUSH_LOG\n");
    }

    //pthread_kill(threadid, SIGKILL);
	pthread_join(mThread, NULL);

	//
	LOG_DBG("-\n");
}

static int js_cnt = -1;
void CcuDrvImp::DumpCcuSram()
{
    FILE *pFile;

    pFile = fopen("sdcard/ccu_reg.bin", "wb");
    if (pFile == NULL) {
        LOG_ERR("Open ccu_reg.bin fail\n");
    }
    else
    {
        fwrite ( (void*) gCcuDrvObj[CCU_A].m_pCcuHwRegAddr, CCU_BASE_RANGE, sizeof(char), pFile );
        fclose(pFile);
    }

    pFile = fopen("sdcard/ccu_dmem.bin", "wb");
    if (pFile == NULL) {
        LOG_ERR("Open ccu_dmem.bin fail\n");
    }
    else
    {
        fwrite ( (void*) gCcuDrvObj[CCU_DMEM].m_pCcuHwRegAddr, CCU_DMEM_RANGE, sizeof(char), pFile );
        fclose(pFile);
    }
    //
    LOG_DBG("============ CCU ASSERT LOG DUMP: LOGBUF[0] =============\n");
    printCcuLog((const char*)g_pLogBuf[0], MTRUE);
    LOG_DBG("============ CCU ASSERT LOG DUMP: LOGBUF[1] =============\n");
    printCcuLog((const char*)g_pLogBuf[1], MTRUE);
}

void CcuDrvImp::PrintReg()
{
    MUINT32 *ccuCtrlBasePtr = (MUINT32 *)gCcuDrvObj[CCU_A].m_pCcuHwRegAddr;
    MUINT32 *ccuCtrlPtr = (MUINT32 *)gCcuDrvObj[CCU_A].m_pCcuHwRegAddr;

    LOG_WRN("=============== CCU REG DUMP START ===============\n");
    for(int i=0 ; i<CCU_A_DUMP_RANGE ; i += 0x10)
    {
        LOG_WRN("0x%08x: 0x%08x ,0x%08x ,0x%08x ,0x%08x\n", i, *(ccuCtrlPtr), *(ccuCtrlPtr+1), *(ccuCtrlPtr+2), *(ccuCtrlPtr+3));
        ccuCtrlPtr += 4;
    }
    LOG_WRN("-------- DMA DEBUG INFO --------\n");
    *(ccuCtrlBasePtr + OFFSET_CCU_A_DMA_DEBUG_SEL) = 0x0013;
    LOG_WRN("SET DMA_DBG_SEL 0x0013, read out DMA_DBG: %08x\n", *(ccuCtrlBasePtr + OFFSET_CCU_A_DMA_DEBUG));
    *(ccuCtrlBasePtr + OFFSET_CCU_A_DMA_DEBUG_SEL) = 0x0113;
    LOG_WRN("SET DMA_DBG_SEL 0x0113, read out DMA_DBG: %08x\n", *(ccuCtrlBasePtr + OFFSET_CCU_A_DMA_DEBUG));
    *(ccuCtrlBasePtr + OFFSET_CCU_A_DMA_DEBUG_SEL) = 0x0213;
    LOG_WRN("SET DMA_DBG_SEL 0x0213, read out DMA_DBG: %08x\n", *(ccuCtrlBasePtr + OFFSET_CCU_A_DMA_DEBUG));
    *(ccuCtrlBasePtr + OFFSET_CCU_A_DMA_DEBUG_SEL) = 0x0313;
    LOG_WRN("SET DMA_DBG_SEL 0x0313, read out DMA_DBG: %08x\n", *(ccuCtrlBasePtr + OFFSET_CCU_A_DMA_DEBUG));
    LOG_WRN("=============== CCU REG DUMP END ===============\n");
}

void CcuDrvImp::DumpSramLog()
{
    char *ccuCtrlBasePtr = (char *)gCcuDrvObj[CCU_DMEM].m_pCcuHwRegAddr;
    char *ccuLogPtr_1 = ccuCtrlBasePtr + OFFSET_CCU_INTERNAL_LOG;
    char *ccuLogPtr_2 = ccuCtrlBasePtr + OFFSET_CCU_INTERNAL_LOG + OFFSET_CCU_INTERNAL_LOG_CHUNK_SZ;

    LOG_WRN("=============== CCU INTERNAL LOG DUMP START ===============\n");
    *(ccuLogPtr_1 + OFFSET_CCU_INTERNAL_LOG_CHUNK_SZ - 1) = '\0';
    *(ccuLogPtr_2 + OFFSET_CCU_INTERNAL_LOG_CHUNK_SZ - 1) = '\0';
    LOG_WRN("---------CHUNK 1-----------\n%s", ccuLogPtr_1);
    LOG_WRN("---------CHUNK 2-----------\n%s", ccuLogPtr_2);
    LOG_WRN("=============== CCU INTERNAL LOG DUMP END ===============\n");
}


void* CcuDrvImp::ApTaskWorkerThreadLoop(void *arg)
{
    CcuDrvImp *_this = reinterpret_cast<CcuDrvImp*>(arg);
    MBOOL ret;
    CCU_WAIT_IRQ_ST _irq;

	LOG_DBG("+:\n");

    _this->m_taskWorkerThreadReady = MTRUE;
    LOG_DBG("m_taskWorkerThreadReady addr: %p\n", &(_this->m_taskWorkerThreadReady));
    LOG_DBG("m_taskWorkerThreadReady val: %d\n", _this->m_taskWorkerThreadReady);

	do
    {
        //check if ap task thread should terminate
        if ( _this->m_taskWorkerThreadEnd )
        {
            LOG_DBG("m_taskWorkerThreadEnd\n");
            break;
        }

        LOG_DBG("call waitIrq() to wait ccu interrupt...\n");

        _irq.Clear      = CCU_IRQ_CLEAR_WAIT;
        _irq.Status     = CCU_INT_ST;
        _irq.St_type    = CCU_SIGNAL_INT;
        //_irq.Timeout    = 0; //wait forever
        _irq.Timeout    = 100; //wait 100 ms
        _irq.TimeInfo.passedbySigcnt = 0x00aabb00;
        ret = _this->waitIrq(&_irq);


        if ( _this->m_taskWorkerThreadEnd )
        {
            LOG_DBG("m_taskWorkerThreadEnd\n");
            break;
        }

        if ( MFALSE == ret )
        {
            LOG_ERR("waitIrq fail\n");
        }
        else
        {
            LOG_DBG("waitIrq() done, irq_cnt(%d), irq_task(%d) \n", ++js_cnt, _irq.TimeInfo.passedbySigcnt);
            //break;
            if ( 1 == _irq.TimeInfo.passedbySigcnt )
            {
                printCcuLog((const char*)g_pLogBuf[0]);
            }
            else if ( 2 == _irq.TimeInfo.passedbySigcnt )
            {
                printCcuLog((const char*)g_pLogBuf[1]);
            }
            else if ( -1 == _irq.TimeInfo.passedbySigcnt )
            {
                LOG_WRN("============ CCU ASSERT LOG DUMP: LOGBUF[0] =============\n");
                printCcuLog((const char*)g_pLogBuf[0], MTRUE);
                LOG_WRN("============ CCU ASSERT LOG DUMP: LOGBUF[1] =============\n");
                printCcuLog((const char*)g_pLogBuf[1], MTRUE);
                _this->DumpSramLog();
                _this->PrintReg();
                AEE_ASSERT_CCU_USER("CCU ASSERT\n");
            }
            else if ( -2 == _irq.TimeInfo.passedbySigcnt )
            {
                LOG_WRN("============ CCU WARNING LOG DUMP: LOGBUF[0] =============\n");
                printCcuLog((const char*)g_pLogBuf[0], MTRUE);
                LOG_WRN("============ CCU WARNING LOG DUMP: LOGBUF[1] =============\n");
                printCcuLog((const char*)g_pLogBuf[1], MTRUE);
            }
            else if(0x00aabb00 == _irq.TimeInfo.passedbySigcnt)
            {
                LOG_DBG("no irq_task: %d\n", _irq.TimeInfo.passedbySigcnt);
            }
            else
            {
                LOG_ERR("unknow irq_task: %d\n", _irq.TimeInfo.passedbySigcnt);
            }
        }
	}while(1);

	LOG_DBG("-:\n");

	return NULL;
}

/*=======================================================================================

=======================================================================================*/
static CcuMemImp    gCcuMemObj;
//
CcuMemImp::CcuMemImp():
gIonDevFD(-1)
{
    LOG_VRB("getpid[0x%08x],gettid[0x%08x]", getpid() ,gettid());
}
//
CcuMemImp* CcuMemImp::createInstance()
{
    LOG_DBG("+:%s\n",__FUNCTION__);
    return (CcuMemImp*)&gCcuMemObj;
}
//
MBOOL CcuMemImp::init(const char* userName)
{
    MBOOL Result = MTRUE;
    LOG_DBG("+:%s/%s\n",__FUNCTION__,userName);

    this->gIonDevFD = mt_ion_open("CCU");
    if  ( 0 > this->gIonDevFD )
    {
        LOG_ERR("mt_ion_open() return %d", this->gIonDevFD);
		return false;
    }

    LOG_DBG("-:%s\n",__FUNCTION__);
    return Result;
}
//
MBOOL CcuMemImp::uninit(const char* userName)
{
    MBOOL Result = MTRUE;

    LOG_DBG("+:%s/%s\n",__FUNCTION__,userName);

	if( 0 <= this->gIonDevFD ) {
		ion_close(this->gIonDevFD);
	}

    LOG_DBG("-:%s\n",__FUNCTION__);

    return Result;
}
//
MBOOL CcuMemImp::ccu_malloc(int drv_h, int len, int *buf_share_fd, char **buf_va)
{
    ion_user_handle_t buf_handle;

    LOG_DBG("+:%s\n",__FUNCTION__);

    // allocate ion buffer handle
    if(ion_alloc_mm(drv_h, (size_t)len, 0, 0, &buf_handle))        // no alignment, non-cache
    {
        LOG_WRN("fail to get ion buffer handle (drv_h=0x%x, len=%d)", drv_h, len);
        return false;
    }
    // get ion buffer share handle
    if(ion_share(drv_h, buf_handle, buf_share_fd))
    {
        LOG_WRN("fail to get ion buffer share handle");
        if(ion_free(drv_h,buf_handle))
            LOG_WRN("ion free fail");
        return false;
    }
    // get buffer virtual address
    *buf_va = ( char *)ion_mmap(drv_h, NULL, (size_t)len, PROT_READ|PROT_WRITE, MAP_SHARED, *buf_share_fd, 0);
    if(*buf_va == NULL) {
        LOG_WRN("fail to get buffer virtual address");
    }
    LOG_DBG("alloc ion: ion_buf_handle %d, share_fd %d, va: %p \n", buf_handle, *buf_share_fd, *buf_va);
    //
    LOG_DBG("-:%s\n",__FUNCTION__);

    return (*buf_va != NULL)?MTRUE:MFALSE;

}
//
MBOOL CcuMemImp::ccu_free(int drv_h, int len,int buf_share_fd, char *buf_va)
{
    ion_user_handle_t buf_handle;

    LOG_DBG("+:%s,fd(%x),va(%p),sz(%d)\n",__FUNCTION__,buf_share_fd,buf_va,len);

    // 1. get handle of ION_IOC_SHARE from fd_data.fd
    if(ion_import(drv_h, buf_share_fd, &buf_handle))
    {
        LOG_WRN("fail to get import share buffer fd");
        return false;
    }
    LOG_VRB("import ion: ion_buf_handle %d, share_fd %d", buf_handle, buf_share_fd);
    // 2. free for IMPORT ref cnt
    if(ion_free(drv_h, buf_handle))
    {
        LOG_WRN("fail to free ion buffer (free ion_import ref cnt)");
        return false;
    }
    // 3. unmap virtual memory address
    if(ion_munmap(drv_h, (void *)buf_va, (size_t)len))
    {
        LOG_WRN("fail to get unmap virtual memory");
        return false;
    }
    // 4. close share buffer fd
    if(ion_share_close(drv_h, buf_share_fd))
    {
        LOG_WRN("fail to close share buffer fd");
        return false;
    }
    // 5. pair of ion_alloc_mm
    if(ion_free(drv_h, buf_handle))
    {
        LOG_WRN("fail to free ion buffer (free ion_alloc_mm ref cnt)");
        return false;
    }

    LOG_DBG("-:%s\n",__FUNCTION__);

    return true;

}
//
MBOOL CcuMemImp::mmapMVA( int buf_share_fd, ion_user_handle_t *p_ion_handle,unsigned int *mva )
{
    struct ion_sys_data sys_data;
    struct ion_mm_data  mm_data;
    //ion_user_handle_t   ion_handle;
    int err;

    LOG_DBG("+:%s\n",__FUNCTION__);

    //a. get handle from IonBufFd and increase handle ref count
    if(ion_import(gIonDevFD, buf_share_fd, p_ion_handle))
    {
        LOG_ERR("ion_import fail, ion_handle(0x%x)", *p_ion_handle);
        return false;
    }
    LOG_VRB("ion_import: share_fd %d, ion_handle %d", buf_share_fd, *p_ion_handle);
    //b. config buffer
    mm_data.mm_cmd = ION_MM_CONFIG_BUFFER_EXT;
    mm_data.config_buffer_param.handle      = *p_ion_handle;
    mm_data.config_buffer_param.eModuleID   = M4U_PORT_CAM_CCUG;
    mm_data.config_buffer_param.security    = 0;
    mm_data.config_buffer_param.coherent    = 1;
    mm_data.config_buffer_param.reserve_iova_start  = 0x10000000;
    mm_data.config_buffer_param.reserve_iova_end    = 0xFFFFFFFF;
    err = ion_custom_ioctl(gIonDevFD, ION_CMD_MULTIMEDIA, &mm_data);
    if(err == (-ION_ERROR_CONFIG_LOCKED))
    {
        LOG_ERR("ion_custom_ioctl Double config after map phy address");
    }
    else if(err != 0)
    {
        LOG_ERR("ion_custom_ioctl ION_CMD_MULTIMEDIA Config Buffer failed!");
    }
    //c. map physical address
    sys_data.sys_cmd = ION_SYS_GET_PHYS;
    sys_data.get_phys_param.handle = *p_ion_handle;
    sys_data.get_phys_param.phy_addr = (M4U_PORT_CAM_CCUG<<24) | ION_FLAG_GET_FIXED_PHYS;
    sys_data.get_phys_param.len = ION_FLAG_GET_FIXED_PHYS;
    if(ion_custom_ioctl(gIonDevFD, ION_CMD_SYSTEM, &sys_data))
    {
        LOG_ERR("ion_custom_ioctl get_phys_param failed!");
        return false;
    }
	//
	*mva = (unsigned int)sys_data.get_phys_param.phy_addr;

    //req_buf->plane[plane_idx].ion_handle = ion_handle;
    //req_buf->plane[plane_idx].mva        = sys_data.get_phys_param.phy_addr;
    //LOG_VRB("get_phys: buffer[%d], plane[%d], mva 0x%x", buf_idx, plane_idx, buf_n->planes[plane_idx].ptr);

    LOG_DBG("\n");
    LOG_DBG("-:%s\n",__FUNCTION__);

    return true;
}
//
MBOOL CcuMemImp::munmapMVA( ion_user_handle_t ion_handle )
{
       LOG_DBG("+:%s\n",__FUNCTION__);

        // decrease handle ref count
        if(ion_free(this->gIonDevFD, ion_handle))
        {
            LOG_ERR("ion_free fail");
            return false;
        }
        LOG_VRB("ion_free: ion_handle %d", ion_handle);

        LOG_DBG("-:%s\n",__FUNCTION__);
        return true;
}
//

int32_t CcuDrvImp::getSensorI2cSlaveAddr(int32_t *sensorI2cSlaveAddr)
{
    if(ioctl(this->m_Fd, CCU_IOCTL_GET_SENSOR_I2C_SLAVE_ADDR , sensorI2cSlaveAddr) < 0)
    {
        LOG_ERR("ERROR: CCU_IOCTL_GET_SENSOR_SLAVE_ID :%x\n", *sensorI2cSlaveAddr);
        return MFALSE;
    }

    return MTRUE;
}








