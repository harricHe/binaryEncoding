/*--------------------------------------------------------------------
 *
 *  testmain.c
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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "binaryEncoding.h"

typedef enum
{
	VOLS_RECTANGULAR = 0,	// 00 rectangular 
	VOLS_BINARY,			// 01 binary 
	VOLS_BINARY_ONLY,		// 10 binary only 
	VOLS_GRAYSCALE,			// 11 grayscale 
	VOLS_MAX
} eVideoObjectLayerShape_t;

typedef enum
{
	SPRITE_NOT_USED = 0,	// 0 00 sprite not used 
	SPRITE_STATIC,			// 1 01 static (Basic/Low Latency) 
	SPRITE_GMC,				// ? 10 GMC (Global Motion Compensation) 
	SPRITE_RESERVED,		// ? 11 Reserved 
	SPRITE_MAX
} eSpriteEnable_t;

typedef enum
{
	ARI_FORBIDDEN = 0,		// 0000  Forbidden 
	ARI_SQUARE = 1,			// 0001 1:1 (Square) 
	ARI_12_11,				// 0010 12:11 (625-type for 4:3 picture) 
	ARI_10_11,				// 0011 10:11 (525-type for 4:3 picture) 
	ARI_16_11,				// 0100 16:11 (625-type stretched for 16:9 picture) 
	ARI_40_33,				// 0101 40:33 (525-type stretched for 16:9 picture) 
	ARI_RESERVED_0,			// 0110-1110 Reserved 
	ARI_RESERVED_1,
	ARI_EXTENDED_PAR = 0xF,	// 1111 extended PAR 
	ARI_MAX
} eAspectRatioInfo_t;

/** 이진 데이터로 인코딩 테스트
 */
int TestBinaryEncoding(HBINARY hBinary)
{
	int nStartCode = 0x000001;
	int visual_object_sequence_start_code = 0xb0;
	int visual_object_start_code = 0xb5;
	int	profile_and_level_indication = 1;
	int is_visual_object_identifier = 0;
	int visual_object_type = 1;
	int video_object_start_code = 0x01;
	int video_object_layer_start_code = 0x20;
	
	binarySetPos(hBinary, 0);	// 인코딩 시작점 설정
	
	// 6.2.2 Visual Object Sequence and Visual Object 
	// start_code						: 32	0000 0000 0000 0000 0000 0001 .... ....
	binaryEncoding(hBinary, 24,	nStartCode);
	
	// visual_object_sequence_start_code: 32	.... .... .... .... .... .... 1011 0000
	binaryEncoding(hBinary, 8,	visual_object_sequence_start_code);
	
	// profile_and_level_indication		: 8		0000 0001
	binaryEncoding(hBinary, 8,	profile_and_level_indication);	// Simple Profile/Level 1
	
	// visual_object
	// visual_object_start_code			: 32	0000 0000 0000 0000 0000 0001 1011 0110
	binaryEncoding(hBinary, 24, nStartCode);
	binaryEncoding(hBinary, 8,	visual_object_start_code);
	
	// is_visual_object_identifier		: 1		0... .... .... ....
	binaryEncoding(hBinary, 1,	is_visual_object_identifier);
	// visual_object_type 				: 4		.000 1... .... ....	video ID (1) <= Table 6-5 -- Meaning of visual object type
	binaryEncoding(hBinary, 4,	visual_object_type);
	// video_signal_type				: 1		.... .0.. .... ....
	binaryEncoding(hBinary, 1,	0);
	// next_start_code					: ?		.... ..01			<= 5.2.4 Definition of next_start_code() function
	binaryEncoding(hBinary, 2,	1);
	
	// video_object_type == 'video ID'
	// start code prefix				: 32	0000 0000 0000 0000 0000 0001 .... ....
	binaryEncoding(hBinary, 24, nStartCode);
	
	// video_object_start_code			: 32	.... .... .... .... .... .... 0000 0001
	binaryEncoding(hBinary, 8,	video_object_start_code);
	
	// 6.2.3 video_object_layer
	// start code prefix				: 32	0000 0000 0000 0000 0000 0001 .... ....
	binaryEncoding(hBinary, 24, nStartCode);
	
	// video_object_layer_start_code	: 32	.... .... .... .... .... .... 0010 0000
	binaryEncoding(hBinary, 8,	video_object_layer_start_code);
	
	// random_accessible_vol			: 1		0... .... .... ....
	binaryEncoding(hBinary, 1,	0);
	// video_object_type_indication		: 8		.000 0000 1... ....
	binaryEncoding(hBinary, 8, 	1);
	
	// is_object_layer_identifier 		: 1		.... .... .0.. ....
	binaryEncoding(hBinary, 1,	0);
	// aspect_ratio_info 				: 4		.... .... ..00 01..
	binaryEncoding(hBinary, 4,	1);
	// vol_control_parameters			: 1		.... .... .... ..0.
	binaryEncoding(hBinary, 1,	0);
	// video_object_layer_shape 		: 2		.... .... .... ...0 0...
	binaryEncoding(hBinary, 2,	0);
	
	// marker_bit 						: 1		.1.. .... .... .... .... ....
	binaryEncoding(hBinary, 1,	1);
	// vop_time_increment_resolution 	: 16	..00 0000 0000 0110 00.. ....
	binaryEncoding(hBinary, 16,	0x18);
	
	// marker_bit						: 1		..1. .... .... .... .... ....
	binaryEncoding(hBinary, 1,	1);
	// fixed_vop_rate 					: 1		...0 .... .... .... .... ....
	binaryEncoding(hBinary, 1,	0);
	
	// marker_bit						: 1		.... 1... .... .... .... ....
	binaryEncoding(hBinary, 1,	1);
	// video_object_layer_width			: 13	.... .000 0101 1000 00.. ....
	binaryEncoding(hBinary, 13,	0x160);
	
	// marker_bit						: 1		..1. .... .... .... .... ....
	binaryEncoding(hBinary, 1,	1);
	// video_object_layer_height		: 13	...0 0001 0010 0000 .... ....
	binaryEncoding(hBinary, 13,	0x120);
	
	// marker_bit						: 1		1... .... .... .... .... ....
	binaryEncoding(hBinary, 1,	1);
	
	// interlaced						: 1		.0.. .... .... .... .... ....
	binaryEncoding(hBinary, 1,	0);
	// obmc_disable						: 1		..1. .... .... .... .... ....
	binaryEncoding(hBinary, 1,	1);
	// sprite_enable					: 1		...0 .... .... .... .... ....		video_object_layer_verid == '0001’
	binaryEncoding(hBinary, 1,	0);
	// not_8_bit						: 1		.... 0... .... .... .... ....
	binaryEncoding(hBinary, 1,	0);
	
	// quant_type						: 1		.... .0.. .... .... .... ....
	binaryEncoding(hBinary, 1,	0);
	// complexity_estimation_disable	: 1		.... ..1. .... .... .... ....
	binaryEncoding(hBinary, 1,	1);
	// estimation_method				: 2		.... ...0 0... .... .... ....		complexity_estimation_disable == 1
	binaryEncoding(hBinary, 2,	0);
	
	
	// shape_complexity_estimation_disable 	: 1	.... .... .0.. .... .... ....
	binaryEncoding(hBinary, 1,	0);
	// opaque				 			: 1		.... .... ..0. .... .... ....
	binaryEncoding(hBinary, 1,	0);
	// transparent		 				: 1		.... .... ...1 .... .... ....
	binaryEncoding(hBinary, 1,	1);
	// intra_cae						: 1		.... .... .... 1... .... ....		data_partitioned == 1
	binaryEncoding(hBinary, 1,	1);
	// inter_cae						: 1		.... .... .... .1.. .... ....
	binaryEncoding(hBinary, 1,	1);
	// no_update 						: 1		.... .... .... ..1. .... ....
	binaryEncoding(hBinary, 1,	1);
	// upsampling						: 1		.... .... .... ...1 .... ....
	binaryEncoding(hBinary, 1,	1);
	
	return 0;
}

/** 5.2.1 Definition of next_bits() function 
 * The function next_bits() permits comparison of a bit string with the next bits to be decoded in the bitstream. 
 */
int next_bits(HBINARY hBinary, int nBits)
{
	int nValue = 0;
	
	nValue = binaryDecodingStr(hBinary, nBits, "next_bits");
	return nValue;
}

/** 5.2.2 Definition of bytealigned() function 
 * The function bytealigned () returns 1 if the current position is on a byte boundary, that is the next bit in the bitstream 
 * is the first bit in a byte. Otherwise it returns 0. 
 */
int bytealigned(HBINARY hBinary)
{
	int			nIdx = 0, nPos = 0;
	
	if(hBinary == NULL)
		return -1;
	
	nIdx = hBinary->nPos / (sizeof(u_int8_t)*8);
	nPos = hBinary->nPos % (sizeof(u_int8_t)*8);
	
	if(nPos == 0)
		return 1;
	else
		return 0;
}

/** 5.2.3 Definition of nextbits_bytealigned() function 
 * The function nextbits_bytealigned() returns a bit string starting from the next byte aligned position. This permits 
 * comparison of a bit string with the next byte aligned bits to be decoded in the bitstream. If the current location in the 
 * bitstream is already byte aligned and the 8 bits following the current location are ‘01111111’, the bits subsequent to 
 * these 8 bits are returned. The current location in the bitstream is not changed by this function. 
 */
int nextbits_bytealigned(HBINARY hBinary)
{
	int			nIdx = 0, nPos = 0;
	int			nBits = 0;
	int			nValue = 0;
	
	if(hBinary == NULL)
		return -1;
	
	nIdx = hBinary->nPos / (sizeof(u_int8_t)*8);
	nPos = hBinary->nPos % (sizeof(u_int8_t)*8);
	
	nBits = 8 - nPos;
	nValue = binaryDecoding(hBinary, nBits);
	// 위치는 원래대로 복귀
	hBinary->nPos -= nBits;
	
	return nValue;
}

/** 5.2.4 Definition of next_start_code() function 
 * The next_start_code() function removes any zero bit and a string of 0 to 7 ‘1’ bits used for stuffing and locates the 
 * next start code. 
 */
int next_start_code(HBINARY hBinary)
{
	int nBits = 1;
	int nValue = 0;
	
	nValue = binaryDecodingStr(hBinary, nBits, "next_start_code.zero_bit");
	while(!bytealigned(hBinary))
	{
		nValue = binaryDecodingStr(hBinary, nBits, "next_start_code.one_bit");
	}
	return 0;
}

/** 이진 데이터로 디코딩 테스트
 */
int TestBinaryDecoding(HBINARY hBinary)
{
	int nValue = 0;
	int visual_object_type = 0;
	int video_object_layer_shape = 0;
	int video_object_layer_verid = 0;
	int sprite_enable = 0;
	
	binarySetPos(hBinary, 0);	// 디코딩 시작점 설정
	
	// start_code						: 24
	nValue = binaryDecodingStr(hBinary, 24, "start_code");
	// visual_object_sequence_start_code: 8
	nValue = binaryDecodingStr(hBinary, 8, "visual_object_sequence_start_code");
	// profile_and_level_indication		: 8
	nValue = binaryDecodingStr(hBinary, 8, "profile_and_level_indication");
	// start_code						: 24
	nValue = binaryDecodingStr(hBinary, 24, "start_code");
	// visual_object_start_code			: 8
	nValue = binaryDecodingStr(hBinary, 8, "visual_object_start_code");
	// is_visual_object_identifier		: 1
	nValue = binaryDecodingStr(hBinary, 1, "is_visual_object_identifier");
	if(nValue == 1)
	{
		// visual_object_verid			: 4
		nValue = binaryDecodingStr(hBinary, 4, "visual_object_verid");
		// visual_object_priority		: 3
		nValue = binaryDecodingStr(hBinary, 3, "visual_object_priority");
	}
	// visual_object_type				: 4
	visual_object_type = binaryDecodingStr(hBinary, 4, "visual_object_type");
	if((visual_object_type == 1) || (visual_object_type == 2))
	{	// Video-ID or still texture ID
		// video_signal_type()
		// video_signal_type			: 1
		nValue = binaryDecodingStr(hBinary, 1, "video_signal_type");
		if(nValue == 1)
		{
			// video_format				: 3
			nValue = binaryDecodingStr(hBinary, 3, "video_format");
			// video_range				: 1
			nValue = binaryDecodingStr(hBinary, 1, "video_range");
			// colour_description		: 1
			nValue = binaryDecodingStr(hBinary, 1, "colour_description");
			if(nValue == 1)
			{   
				// colour_primaries		: 8
				nValue = binaryDecodingStr(hBinary, 8, "colour_primaries");
				// transfer_characteristics	: 8
				nValue = binaryDecodingStr(hBinary, 8, "transfer_characteristics");
				// matrix_coefficients		: 8
				nValue = binaryDecodingStr(hBinary, 8, "matrix_coefficients");
			}
		}
	}
	next_start_code(hBinary);
	if(visual_object_type == 1)
	{
		// start_code						: 24
		nValue = binaryDecodingStr(hBinary, 24, "start_code");
		// video_object_start_code			: 8
		nValue = binaryDecodingStr(hBinary, 8, "video_object_start_code");
		// VideoObjectLayer()
		// start_code						: 24
		nValue = binaryDecodingStr(hBinary, 24, "start_code");
		// video_object_layer_start_code	: 8
		nValue = binaryDecodingStr(hBinary, 8, "video_object_layer_start_code");
		// random_accessible_vol			: 1
		nValue = binaryDecodingStr(hBinary, 1, "random_accessible_vol");
		// video_object_type_indication		: 8
		nValue = binaryDecodingStr(hBinary, 8, "video_object_type_indication");
		// is_object_layer_identifier		: 1
		nValue = binaryDecodingStr(hBinary, 1, "is_object_layer_identifier");
		if(nValue == 1)
		{
			// video_object_layer_verid 	: 4
			video_object_layer_verid = binaryDecodingStr(hBinary, 4, "video_object_layer_verid");
			// video_object_layer_priority	: 3
			nValue = binaryDecodingStr(hBinary, 3, "video_object_layer_priority");
		}
		// aspect_ratio_info 				: 4
		nValue = binaryDecodingStr(hBinary, 4, "aspect_ratio_info");
		if(nValue == ARI_EXTENDED_PAR)
		{	// aspect_ratio_info == "extended_PAR")
			// par_width					: 8
			nValue = binaryDecodingStr(hBinary, 8, "par_width");
			// par_height					: 8
			nValue = binaryDecodingStr(hBinary, 8, "par_height");
		}
		// vol_control_parameters			: 1
		nValue = binaryDecodingStr(hBinary, 1, "vol_control_parameters");
		if(nValue == 1)
		{
		}
		// video_object_layer_shape			: 2
		video_object_layer_shape = binaryDecodingStr(hBinary, 2, "video_object_layer_shape");
		// marker_bit						: 1
		nValue = binaryDecodingStr(hBinary, 1, "marker_bit");
		// vop_time_increment_resolution	: 16
		nValue = binaryDecodingStr(hBinary, 16, "vop_time_increment_resolution");
		// marker_bit						: 1
		nValue = binaryDecodingStr(hBinary, 1, "marker_bit");
		// fixed_vop_rate					: 1
		nValue = binaryDecodingStr(hBinary, 1, "fixed_vop_rate");
		if(nValue == 1)
		{
			// fixed_vop_time_increment		: 1~16
			nValue = binaryDecodingStr(hBinary, 1, "fixed_vop_time_increment");
		}
		if(video_object_layer_shape != VOLS_BINARY_ONLY)
		{
			if(video_object_layer_shape == VOLS_RECTANGULAR)
			{ 
				// marker_bit						: 1
				nValue = binaryDecodingStr(hBinary, 1, "marker_bit");
				// video_object_layer_width			: 13
				nValue = binaryDecodingStr(hBinary, 13, "video_object_layer_width");
				// marker_bit						: 1
				nValue = binaryDecodingStr(hBinary, 1, "marker_bit");
				// video_object_layer_height		: 13
				nValue = binaryDecodingStr(hBinary, 13, "video_object_layer_height");
				// marker_bit						: 1
				nValue = binaryDecodingStr(hBinary, 1, "marker_bit");
			}
			// interlaced					: 1
			nValue = binaryDecodingStr(hBinary, 1, "interlaced");
			// obmc_disable					: 1
			nValue = binaryDecodingStr(hBinary, 1, "obmc_disable");
			
			if(video_object_layer_verid == 1)
			{	// sprite_enable					: 1
				sprite_enable = binaryDecodingStr(hBinary, 1, "sprite_enable");
			}
			else
			{	// sprite_enable					: 2
				sprite_enable = binaryDecodingStr(hBinary, 2, "sprite_enable");
			}
			if(sprite_enable == SPRITE_STATIC || sprite_enable == SPRITE_GMC)
			{
				if(sprite_enable != SPRITE_GMC)
				{
				}
			}
			if(video_object_layer_verid != 1 && video_object_layer_shape != VOLS_RECTANGULAR) 
			{
				// sadct_disable 					: 1
				nValue = binaryDecodingStr(hBinary, 1, "sadct_disable");
			}
			// not_8_bit					: 1
			nValue = binaryDecodingStr(hBinary, 1, "not_8_bit");
			if(nValue == 1)
			{
				// quant_precision			: 4
				nValue = binaryDecodingStr(hBinary, 4, "quant_precision");
				// bits_per_pixel			: 4
				nValue = binaryDecodingStr(hBinary, 4, "bits_per_pixel");
			}
			if(video_object_layer_shape == VOLS_GRAYSCALE)
			{
				// no_gray_quant_update		: 1
				nValue = binaryDecodingStr(hBinary, 1, "no_gray_quant_update");
				// composition_method		: 1
				nValue = binaryDecodingStr(hBinary, 1, "composition_method");
				// linear_composition		: 1
				nValue = binaryDecodingStr(hBinary, 1, "linear_composition");
			}
			// quant_type					: 1
			nValue = binaryDecodingStr(hBinary, 1, "quant_type");
			if(nValue == 1)
			{
				// load_intra_quant_mat 	: 1
				nValue = binaryDecodingStr(hBinary, 1, "load_intra_quant_mat");
				// load_nonintra_quant_mat	: 1
				nValue = binaryDecodingStr(hBinary, 1, "load_nonintra_quant_mat");
			}
			if(video_object_layer_verid != 1)
			{
				// quarter_sample			: 1
				nValue = binaryDecodingStr(hBinary, 1, "quarter_sample");
			}
			// complexity_estimation_disable	: 1
			nValue = binaryDecodingStr(hBinary, 1, "complexity_estimation_disable");
			if(nValue == 0)
			{
				// define_vop_complexity_estimation_header()
				// estimation_method			: 2
				nValue = binaryDecodingStr(hBinary, 2, "estimation_method");
				if((nValue == 0) || (nValue == 1))
				{
					// shape_complexity_estimation_disable	: 1
					nValue = binaryDecodingStr(hBinary, 1, "shape_complexity_estimation_disable");
					// exture_complexity_estimation_set_1_disable	: 1
					nValue = binaryDecodingStr(hBinary, 1, "exture_complexity_estimation_set_1_disable");
					// marker_bit									: 1
					nValue = binaryDecodingStr(hBinary, 1, "marker_bit");
					// texture_complexity_estimation_set_2_disable	: 1
					nValue = binaryDecodingStr(hBinary, 1, "texture_complexity_estimation_set_2_disable");
					// motion_compensation_complexity_disable		: 1
					nValue = binaryDecodingStr(hBinary, 1, "motion_compensation_complexity_disable");
					// marker_bit									: 1
					nValue = binaryDecodingStr(hBinary, 1, "marker_bit");
				}
			}
			// resync_marker_disable		: 1
			nValue = binaryDecodingStr(hBinary, 1, "resync_marker_disable");
			// data_partitioned				: 1
			nValue = binaryDecodingStr(hBinary, 1, "data_partitioned");
			if(nValue == 1)
			{
				// reversible_vlc			: 1
				nValue = binaryDecodingStr(hBinary, 1, "reversible_vlc");
			}
			if(video_object_layer_verid != 1)
			{
			}
			// scalability					: 1
			nValue = binaryDecodingStr(hBinary, 1, "scalability");
			if(nValue == 1)
			{
			}
		}
		else
		{
			if(video_object_layer_verid != 0001)
			{
				// scalability				: 1
				nValue = binaryDecodingStr(hBinary, 1, "scalability");
				if(nValue == 1)
				{
				}
			}
			// resync_marker_disable		: 1
			nValue = binaryDecodingStr(hBinary, 1, "resync_marker_disable");
		}
		next_start_code(hBinary);
	}
	
	return 0;
}

int main()
{
	HBINARY hBinary = NULL;
	
	hBinary = binaryInit(50);
	if(hBinary == NULL)
		return -1;
	
	TestBinaryEncoding(hBinary);
	binaryPrint(hBinary);
	
	TestBinaryDecoding(hBinary);
	
	binaryRelease(hBinary);
	
	return 0;
}
