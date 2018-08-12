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

#include <stdio.h>
#include <cutils/log.h>
#include <cutils/pmem.h>
//#include <cutils/memutil.h>

#include "jpeg_hal_dec.h"

#define xlog(...) \
        do { \
            LOGD(__VA_ARGS__); \
        } while (0)

#undef LOG_TAG
#define LOG_TAG "jpeg_test"

extern unsigned char jpg_file[];

//#define BUFFER_SIZE 739
#define BUFFER_SIZE (720*480*4)

#undef xlog
#define xlog(...) printf(__VA_ARGS__)

int main()
{
    FILE *fp;
    int src_fd;
    unsigned long index;
    unsigned char *src_va;

    unsigned int    colorBufferPA[3];
    unsigned char*  colorBufferVA[3];
    int             colorBufferFD[3];
    unsigned int    bufferSizeArray[3];    
   
    printf("go %d !!\n", __LINE__);  

   
    JpgDecComp* jpgDec = new JpgDecComp();
    printf("go %d !!\n", __LINE__);  


    if(!jpgDec->lock())
    {
        xlog("can't lock resource");
        return -1;
    }
    printf("go %d !!\n", __LINE__);  


    src_va = (unsigned char *)pmem_alloc(BUFFER_SIZE , &src_fd);
    if(src_va == NULL)
    {
        xlog("Can not allocate memory\n");
        return -1;
    }
    fp = fopen("/data/otis/test.jpg", "r");
    fread(src_va , 1 , BUFFER_SIZE , fp);
    fclose(fp);
 
#if 0
{
    // compute color buffer size for output color buffer Allocation
    for (int i=0; i<D_MAX_JPEG_HW_COMP; i++)
    {
        unsigned int rowSize;

        bufferSize = CM_CompOneColorBufferSize(&g_DecConfig, i, realScale, 0, g_DecConfig.u4ModTotalRows);
        bufferSizeArray[i] = bufferSize;
        printf("=== Comp#%d BufferSize: %d ===\n", i, bufferSize);
    }

    // Allocate output color buffer
    for (int i=0; i<D_MAX_JPEG_HW_COMP; i++)
    {
        if (bufferSizeArray[i] == 0)
            continue;
        
        colorBufferVA[i] = (unsigned char *)pmem_alloc_sync(bufferSizeArray[i], &colorBufferFD[i]);    
        if (colorBufferVA[i] == NULL)
        {
            printf("Can not allocate color buffer!!\n");
            return -1;
        }  
        colorBufferPA[i] = (unsigned int)pmem_get_phys(colorBufferFD[i]);
        printf("Output Color Buffer#%d: 0x%08x, Size: %d\n", i, colorBufferPA[i], bufferSizeArray[i]);
        memset(colorBufferVA[i], 0xCD, sizeof(char)*bufferSizeArray[i]);
    }
}
#endif

//    jpgDec->setSrcAddr(jpg_file);

    jpgDec->setSrcAddr(src_va);
    jpgDec->setSrcSize(BUFFER_SIZE);
    jpgDec->parse();

    unsigned int w = jpgDec->getJpgWidth();
    unsigned int h = jpgDec->getJpgHeight();

    xlog("file width/height : %d %d\n", w, h);

    jpgDec->setOutWidth(w);
    jpgDec->setOutHeight(h);
    jpgDec->setOutFormat(JpgDecComp::kYUV_3PLANE_Format);
    //jpgDec->setOutFormat(JpgDecComp::kRGB_565_Format);
    
    int dst_size = w * h * 2;
    dst_size = 640*480 ;
    unsigned char *dst_buffer = (unsigned char *)malloc(dst_size); 

    jpgDec->setDstAddr((unsigned char*) dst_buffer);
    jpgDec->setDstBufSize((unsigned int)dst_size);

    if(jpgDec->start())
    {
        FILE *fp;
        fp = fopen("/data/otis/ttt.raw", "w");
        int index;
        for(index = 0 ; index < dst_size ; index++)
        {
            fprintf(fp, "%c", dst_buffer[index]);
        }
        fclose(fp);
    }
    else
    {
        xlog("decode failed~~~\n");
    }

    pmem_free(src_va , BUFFER_SIZE , src_fd);
    
    jpgDec->unlock();
    delete jpgDec;
    return 0;
}

unsigned char jpg_file[739]=
{0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 
 0x01, 0x01, 0x00, 0x60, 0x00, 0x60, 0x00, 0x00, 0xFF, 0xDB, 0x00, 0x43, 
 0x00, 0x08, 0x06, 0x06, 0x07, 0x06, 0x05, 0x08, 0x07, 0x07, 0x07, 0x09, 
 0x09, 0x08, 0x0A, 0x0C, 0x14, 0x0D, 0x0C, 0x0B, 0x0B, 0x0C, 0x19, 0x12, 
 0x13, 0x0F, 0x14, 0x1D, 0x1A, 0x1F, 0x1E, 0x1D, 0x1A, 0x1C, 0x1C, 0x20, 
 0x24, 0x2E, 0x27, 0x20, 0x22, 0x2C, 0x23, 0x1C, 0x1C, 0x28, 0x37, 0x29, 
 0x2C, 0x30, 0x31, 0x34, 0x34, 0x34, 0x1F, 0x27, 0x39, 0x3D, 0x38, 0x32, 
 0x3C, 0x2E, 0x33, 0x34, 0x32, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x09, 0x09, 
 0x09, 0x0C, 0x0B, 0x0C, 0x18, 0x0D, 0x0D, 0x18, 0x32, 0x21, 0x1C, 0x21, 
 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 
 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 
 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 
 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 
 0x32, 0x32, 0xFF, 0xC0, 0x00, 0x11, 0x08, 0x00, 0x10, 0x00, 0x0C, 0x03, 
 0x01, 0x22, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF, 0xC4, 0x00, 
 0x1F, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x10, 0x00, 
 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 
 0x00, 0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 
 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 
 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 
 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 
 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 
 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 
 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 
 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 
 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 
 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 
 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 
 0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 
 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 
 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 
 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 
 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 
 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 
 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 
 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 
 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 
 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 
 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 
 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 
 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 
 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 
 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 
 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 
 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 
 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xDA, 0x00, 
 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00, 0xB3, 
 0x7F, 0xE2, 0x07, 0xF0, 0xEC, 0x76, 0x10, 0xAD, 0xBC, 0xD2, 0xFF, 0x00, 
 0xA2, 0xA3, 0x31, 0x8A, 0x4D, 0xA5, 0x42, 0xAA, 0x8E, 0x87, 0xAF, 0x5A, 
 0xD4, 0xBB, 0xD4, 0xCB, 0x8B, 0x79, 0x65, 0x25, 0x9A, 0x48, 0x83, 0x02, 
 0xE0, 0x67, 0x19, 0x35, 0x93, 0xE2, 0xBF, 0x0F, 0xDE, 0xEA, 0x17, 0xA1, 
 0xAD, 0x65, 0x81, 0x02, 0x44, 0x61, 0x29, 0x30, 0x23, 0x3E, 0xE0, 0xD5, 
 0x4F, 0x13, 0x5C, 0xFD, 0x96, 0xEE, 0xD2, 0x20, 0x7A, 0x5B, 0x0E, 0x9F, 
 0xEF, 0xB5, 0x78, 0x39, 0x8E, 0x17, 0x2F, 0x9E, 0x55, 0x84, 0x9D, 0x05, 
 0x1F, 0x6A, 0xFE, 0x26, 0x9E, 0xBB, 0x5F, 0x51, 0xF0, 0xBC, 0x30, 0xF8, 
 0xAC, 0xE9, 0xFB, 0x29, 0xB7, 0x37, 0xED, 0x5C, 0xD5, 0xDD, 0x97, 0xBE, 
 0xB9, 0x6C, 0xB6, 0xD9, 0x9F, 0xFF, 0xD9};
