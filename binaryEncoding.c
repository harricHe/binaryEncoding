/*--------------------------------------------------------------------
 *
 *  binaryEncoding.c
 *
 *  Copyright (C) 2011 yongs2 <http://github.com/yongs2/binaryEncoding>
 *  All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *--------------------------------------------------------------------
 */
#include "binaryEncoding.h"

/** 이진 데이터를 분석하기 위해 버퍼 초기화
 */
HBINARY binaryInit(int nSize)
{
	HBINARY hBinary = NULL;
	
	hBinary = (HBINARY)malloc(sizeof(stBinary));
	if(hBinary == NULL)
	{
		return NULL;
	}
	
	hBinary->nPos = 0;
	hBinary->nSize = nSize;
	hBinary->pstData = (u_int8_t *)malloc(sizeof(u_int8_t)*nSize);
	if(hBinary->pstData == NULL)
	{
		free(hBinary);
		return NULL;
	}
	memset(hBinary->pstData, 0, sizeof(u_int8_t)*nSize);
	
	return hBinary;
}

/** 이진 데이터를 분석하기 위해 할당되었던 버퍼 해제
 */
int binaryRelease(HBINARY hBinary)
{
	if(hBinary == NULL)
	{
		return -1;
	}
	
	if(hBinary->pstData != NULL)
	{
		free(hBinary->pstData);
		hBinary->pstData = NULL;
	}
	
	free(hBinary);
	return 0;
}

/** 이진 데이터를 인코딩/디코딩하기 전에 위치 설정
 */
int binarySetPos(HBINARY hBinary, int nPos)
{
	if(hBinary == NULL)
	{
		return -1;
	}
	
	hBinary->nPos = nPos;
	return 0;
}

/** 이진 데이터로 인코딩
 */
int binaryEncoding(HBINARY hBinary, int nBits, int nValue)
{
	int			*pnDest32 = NULL, nMask32 = 0xFFFFFFFF;
	short int	*pnDest16 = NULL, nMask16 = 0xFFFF;
	char 		*pnDest8 = NULL, nMask8 = 0xFF;
	int			nIdx = 0, nPos = 0, nBitSize = 0;
	
	if(hBinary == NULL)
		return -1;
	
	nIdx = hBinary->nPos / (sizeof(u_int8_t)*8);
	nPos = hBinary->nPos % (sizeof(u_int8_t)*8);
	if(((nPos+nBits) > 16) && ((nPos+nBits) <= 32))
	{	// 32 bits = 4 bytes => int
		nBitSize = sizeof(int)*8;
		pnDest32 = (int *)(hBinary->pstData + nIdx);
		*pnDest32 = htonl(*pnDest32);
		nMask32 = (int)(0xFFFFFFFF >> (nBitSize - nBits));
		*pnDest32 |= (nValue & nMask32) << (nBitSize - (nPos+nBits));
		*pnDest32 = ntohl(*pnDest32);
		printf("%s,32, Pos=%3d(%d), Data[%02d]=0x%02X, Bits=%d, Shft=%d, Mask=0x%02x, Val=0x%x\n", __func__,
			hBinary->nPos, nPos, nIdx, hBinary->pstData[nIdx], nBits, ( nBitSize - (nPos+nBits)), nMask32, nValue);
		hBinary->nPos += nBits;
	}
	else if(((nPos+nBits) > 8) && ((nPos+nBits) <= 16))
	{	// 16 bits = 2 bytes => short int
		nBitSize = sizeof(short int)*8;
		pnDest16 = (short int *)(hBinary->pstData + nIdx);
		*pnDest16 = htons(*pnDest16);
		nMask16 = (short int)(0xFFFF >> (nBitSize - nBits));
		*pnDest16 |= (nValue & nMask16) << (nBitSize - (nPos+nBits));
		*pnDest16 = ntohs(*pnDest16);
		printf("%s,16, Pos=%3d(%d), Data[%02d]=0x%02X%02X (0x%04x), Bits=%d, Shft=%d, Mask=0x%02x, Val=0x%x\n", __func__,
			hBinary->nPos, nPos, nIdx, hBinary->pstData[nIdx], hBinary->pstData[nIdx+1], *pnDest16, nBits, ( nBitSize - (nPos+nBits)), nMask16, nValue);
		hBinary->nPos += nBits;
	}
	else if((nPos+nBits) <= 8)
	{	// 8 bits = 1 bytes => char
		nBitSize = sizeof(char)*8;
		pnDest8 = (char *)(hBinary->pstData + nIdx);
		nMask8 = (char )(0xFF >> (nBitSize - nBits));
		*pnDest8 |= (nValue & nMask8) << (nBitSize - (nPos+nBits));
		printf("%s,08, Pos=%3d(%d), Data[%02d]=0x%02X (0x%02x), Bits=%d, Shft=%d, Mask=0x%02x, Val=0x%x\n", __func__,
			hBinary->nPos, nPos, nIdx, hBinary->pstData[nIdx], *pnDest8, nBits, ( nBitSize - (nPos+nBits)), (char)nMask8, nValue);
		hBinary->nPos += nBits;
	}
	
	return 0;
}

/** 이진 데이터로 디코딩
 */
int binaryDecoding(HBINARY hBinary, int nBits)
{
	int			nRet32 = 0, *pnDest32 = NULL, nMask32 = 0xFFFFFFFF;
	short int	nRet16 = 0, *pnDest16 = NULL, nMask16 = 0xFFFF;
	char 		nRet8 = 0, *pnDest8 = NULL, nMask8 = 0xFF;
	int			nIdx = 0, nPos = 0, nBitSize = 0;
	
	if(hBinary == NULL)
		return -1;
	
	nIdx = hBinary->nPos / (sizeof(u_int8_t)*8);
	nPos = hBinary->nPos % (sizeof(u_int8_t)*8);
	if(((nPos+nBits) > 16) && ((nPos+nBits) <= 32))
	{	// 32 bits = 4 bytes => int
		nBitSize = sizeof(int)*8;
		pnDest32 = (int *)(hBinary->pstData + nIdx);
		nRet32 = htonl(*pnDest32);
		nMask32 = (int)(0xFFFFFFFF >> (nBitSize - nBits));
		nRet32 = (nRet32 >> (nBitSize - (nPos+nBits)) & nMask32);
		//printf("%s,32, Pos=%3d(%d), Data[%02d]=0x%02X%02X (0x%02x), Bits=%d, Shft=%d, Mask=0x%02x, Val=0x%x\n", __func__,
		//	hBinary->nPos, nPos, nIdx, hBinary->pstData[nIdx], hBinary->pstData[nIdx+1], *pnDest32, nBits, ( nBitSize - (nPos+nBits)), (char)nMask8, nRet32);
		hBinary->nPos += nBits;
		return nRet32;
	}
	else if(((nPos+nBits) > 8) && ((nPos+nBits) <= 16))
	{
		nBitSize = sizeof(short int)*8;
		pnDest16 = (short int *)(hBinary->pstData + nIdx);
		nRet16 = htons(*pnDest16);
		nMask16 = (short int)(0xFFFF >> (nBitSize - nBits));
		nRet16 = (nRet16 >> (nBitSize - (nPos+nBits)) & nMask16);
		//printf("%s,16, Pos=%3d(%d), Data[%02d]=0x%02X%02x (0x%02x), Bits=%d, Shft=%d, Mask=0x%02x, Val=0x%x\n", __func__,
		//	hBinary->nPos, nPos, nIdx, hBinary->pstData[nIdx], hBinary->pstData[nIdx+1], *pnDest16, nBits, ( nBitSize - (nPos+nBits)), (char)nMask8, nRet16);
		hBinary->nPos += nBits;
		return nRet16;
	}
	else if((nPos+nBits) <= 8)
	{
		nBitSize = sizeof(char)*8;
		pnDest8 = (char *)(hBinary->pstData + nIdx);
		nMask8 = (char)(0xFF >> (nBitSize - nBits));
		nRet8 = (*pnDest8 >> (nBitSize - (nPos+nBits)) & nMask8);
		//printf("%s,08, Pos=%3d(%d), Data[%02d]=0x%02X (0x%02x), Bits=%d, Shft=%d, Mask=0x%02x, Val=0x%x\n", __func__,
		//	hBinary->nPos, nPos, nIdx, hBinary->pstData[nIdx], *pnDest8, nBits, ( nBitSize - (nPos+nBits)), (char)nMask8, nRet8);
		hBinary->nPos += nBits;
		return nRet8;
	}
	
	return 0;
}

int binaryDecodingStr(HBINARY hBinary, int nBits, char *pszObject)
{
	int 		nValue = 0;
	int			nIdx = 0, nPos = 0;
	
	nIdx = hBinary->nPos / (sizeof(u_int8_t)*8);
	nPos = hBinary->nPos % (sizeof(u_int8_t)*8);
	
	nValue = binaryDecoding(hBinary, nBits);
	printf("%s(%3d),D[%2d]=0x%02X %40s (%2d bits) = %3d (0x%08x)\n", __func__, hBinary->nPos, nIdx, hBinary->pstData[nIdx], pszObject, nBits, nValue, nValue);
	return nValue;
}

/** 이진 데이터로 화면에 출력
 */
int binaryPrint(HBINARY hBinary)
{
	int			nIdx = 0, nPos = 0;
	int			nMaxIdx = 0;
	
	if(hBinary == NULL)
		return -1;
	
	nMaxIdx = hBinary->nPos / (sizeof(u_int8_t)*8) + 1;
	nPos = hBinary->nPos % (sizeof(u_int8_t)*8);
	
	printf("---------------------------------------------------------\n");
	for(nIdx=0; nIdx < nMaxIdx; nIdx++)
	{
		printf("%02X ", hBinary->pstData[nIdx]);
		if(((nIdx % 16) == 15) && (nIdx != (nMaxIdx-1)))
		{
			printf("\n");
		}
	} // end of for
	printf("\n---------------------------------------------------------\n");
	
	return 0;
}

/** 분석하기 위한 데이터를 설정
 */
int binarySetData(HBINARY hBinary, u_int8_t *pstData, int nLen)
{
	if(hBinary == NULL)
		return -1;
	
	if((pstData != NULL) && (nLen >= hBinary->nSize))
		return -1;
	
	hBinary->nPos = 0;
	hBinary->nSize = nLen;
	memcpy(hBinary->pstData, pstData, nLen);
	
	return nLen;
}
