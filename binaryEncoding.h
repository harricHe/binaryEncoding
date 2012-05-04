#ifndef	__BINARYENCODING_H__
#define	__BINARYENCODING_H__
/*--------------------------------------------------------------------
 *
 *  binaryEncoding.h
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

typedef struct
{
	int			nSize;
	int			nPos;
	u_int8_t	*pstData;
} stBinary;
typedef stBinary *	HBINARY;

HBINARY binaryInit(int nSize);
int binaryRelease(HBINARY hBinary);
int binarySetPos(HBINARY hBinary, int nPos);
int binaryEncoding(HBINARY hBinary, int nBits, int nValue);
int binaryDecoding(HBINARY hBinary, int nBits);
int binaryDecodingStr(HBINARY hBinary, int nBits, char *pszObject);
int binaryPrint(HBINARY hBinary);
int binarySetData(HBINARY hBinary, u_int8_t *pstData, int nLen);

#endif	// __BINARYENCODING_H__
