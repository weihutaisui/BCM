/*
 *<:copyright-BRCM:2013:proprietary:epon 
 *
 *   Copyright (c) 2013 Broadcom 
 *   All Rights Reserved
 *
 * This program is the proprietary software of Broadcom and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in
 * an Authorized License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and Broadcom
 * expressly reserves all rights in and to the Software and all intellectual
 * property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 * NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 * BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 *    constitutes the valuable trade secrets of Broadcom, and you shall use
 *    all reasonable efforts to protect the confidentiality thereof, and to
 *    use this information only in connection with your use of Broadcom
 *    integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
 *    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
 *    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 *    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
 *    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
 *    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
 *    PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 *    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 *    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
 *    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 *    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
 *    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
 *    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
 *    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
 *    LIMITED REMEDY.
:>
 */
////////////////////////////////////////////////////////////////////////////////
/// \file Stream.c
/// \brief API for reading/writing packed bytes streams
///
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "Teknovus.h"
#include "Stream.h"


////////////////////////////////////////////////////////////////////////////////
/// \brief  Read a byte from a packed stream
///
/// \param strm     Stream to read from
///
/// \return
/// Byte read
////////////////////////////////////////////////////////////////////////////////
U8 StreamReadU8(Stream* strm)
    {
    U8 byte = StreamPeekU8(strm);
    strm->cur++;
    return byte;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Read a word from a packed stream
///
/// \param strm     Stream to read from
///
/// \return
/// Word read
////////////////////////////////////////////////////////////////////////////////
U16 StreamReadU16(Stream* strm)
    {
    U16 word = StreamPeekU16(strm);
    strm->cur += sizeof(U16);
    return word;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Read a dword from a packed stream
///
/// \param strm     Stream to read from
///
/// \return
/// Dword read
////////////////////////////////////////////////////////////////////////////////
U32 StreamReadU32(Stream* strm)
    {
    U32 dword = StreamPeekU32(strm);
    strm->cur += sizeof(U32);
    return dword;
    }


#if defined(_ARC)
////////////////////////////////////////////////////////////////////////////////
/// \brief  Read a qword from a packed stream
///
/// \param strm     Stream to read from
///
/// \return
/// Qword read
////////////////////////////////////////////////////////////////////////////////
U64 StreamReadU64(Stream* strm)
    {
    U64 qword = StreamPeekU64(strm);
    strm->cur += sizeof(U64);
    return qword;
    }
#endif


////////////////////////////////////////////////////////////////////////////////
/// \brief  Read a byte from a packed stream without advancing
///
/// \param strm     Stream to read from
///
/// \return
/// Byte read
////////////////////////////////////////////////////////////////////////////////
U8 StreamPeekU8(const Stream* strm)
    {
    return *strm->cur;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Read a word from a packed stream without advancing
///
/// \param strm     Stream to read from
///
/// \return
/// Word read
////////////////////////////////////////////////////////////////////////////////
U16 StreamPeekU16(const Stream* strm)
    {
    return OAM_NTOHS(*(U16*)strm->cur);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Read a dword from a packed stream without advancing
///
/// \param strm     Stream to read from
///
/// \return
/// Dword read
////////////////////////////////////////////////////////////////////////////////
U32 StreamPeekU32(const Stream* strm)
    {
    return OAM_NTOHL(*(U32*)strm->cur);
    }


#if defined(_ARC)
////////////////////////////////////////////////////////////////////////////////
/// \brief  Read a qword from a packed stream without advancing
///
/// \param strm     Stream to read from
///
/// \return
/// Qword read
////////////////////////////////////////////////////////////////////////////////
U64 StreamPeekU64(const Stream* strm)
    {
    return OAM_NTOHLL(*(U64 *)strm->cur);
    }
#endif


////////////////////////////////////////////////////////////////////////////////
void StreamWriteBytes(Stream* strm, const U8 * bytes, U16 numBytes)
    {
    U16 FAST i;

    for (i = 0; i < numBytes; i++)
        {
        StreamWriteU8(strm, bytes[i]);
        }
    }


////////////////////////////////////////////////////////////////////////////////
void StreamCopy(Stream* strm, const U8 * src, U16 numBytes)
    {
    memcpy(strm->cur, src, numBytes);
    StreamSkip(strm, numBytes);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Write a byte to a packed stream
///
/// \param strm     Stream to write to
/// \param byte     Byte to write
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void StreamWriteU8(Stream* strm, U8 byte)
    {
    *strm->cur = byte;
    strm->cur++;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Write a word to a packed stream
///
/// \param strm     Stream to write to
/// \param word     Word to write
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void StreamWriteU16(Stream* strm, U16 word)
    {
    *(U16*)strm->cur = OAM_HTONS(word);
    strm->cur += sizeof(U16);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Write a dword to a packed stream
///
/// \param strm     Stream to write to
/// \param dword    Dword to write
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void StreamWriteU32(Stream* strm, U32 dword)
    {
    *(U32*)strm->cur = OAM_HTONL(dword);
    strm->cur += sizeof(U32);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Write a qword to a packed stream
///
/// \param strm     Stream to write to
/// \param qword    Qword to write
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void StreamWriteU64(Stream* strm, U64 qword)
    {
	U64 qword_n = OAM_HTONLL(qword);
	memcpy(strm->cur, &qword_n, sizeof(U64));
    strm->cur += sizeof(U64);
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Skip bytes in a stream
///
/// \param strm         Stream to skip
/// \param byteCount    Number of bytes to skip
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void StreamSkip(Stream BULK* strm, U32 byteCount)
    {
    strm->cur += byteCount;
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Rewind a stream
///
/// \param strm         Stream to rewind
/// \param byteCount    Number of bytes to Rewind
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void StreamRewind(Stream BULK* strm, U32 byteCount)
    {
    strm->cur -= byteCount;
    }

////////////////////////////////////////////////////////////////////////////////
/// \brief  Return length of stream in bytes (from start to current location)
///
/// \param strm         Stream to get byte length for
///
/// \return
/// Count in bytes of the stream
////////////////////////////////////////////////////////////////////////////////
U32 StreamLengthInBytes(const Stream* strm)
    {
    return (U32)PointerDiff(strm->cur, strm->start);
    }


////////////////////////////////////////////////////////////////////////////////
/// \brief  Initialize a stream
///
/// \param strm     Stream to init
/// \param start    Pointer to beginning of stream
///
/// \return
/// None
////////////////////////////////////////////////////////////////////////////////
void StreamInit(Stream* strm, U8* start)
    {
    strm->start = start;
    strm->cur = start;
    }


// end of Stream.c
