/**
 * \file
 * \brief
 *
 * Copyright (c) 2016 Astek Corporation. All rights reserved.
 *
 * \astek_eguard_library_license_start
 *
 * \page eGuard_License
 * 
 * The source code contained within is subject to Astek's eGuard licensing
 * agreement located at: https://www.astekcorp.com/
 *
 * The eGuard product may be used in source and binary forms, with or without
 * modifications, with the following conditions:
 *
 * 1. The source code must retain the above copyright notice, this list of
 *    conditions, and the disclaimer.
 *
 * 2. Distribution of source code is not authorized.
 *
 * 3. This software may only be used in connection with an Astek eGuard
 *    Product.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE
 * DO NOT WARRANT THAT THE FUNCTIONS CONTAINED IN THE SOFTWARE WILL MEET YOUR
 * REQUIREMENTS OR THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR
 * ERROR FREE. ANY USE OF THE SOFTWARE SHALL BE MADE ENTIRELY AT THE USER'S OWN
 * RISK. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR ANY CONTRIUBUTER OF
 * INTELLECTUAL PROPERTY RIGHTS TO THE SOFTWARE PROPERTY BE LIABLE FOR ANY
 * CLAIM, OR ANY DIRECT, SPECIAL, INDIRECT, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM ANY ALLEGED INFRINGEMENT
 * OR ANY LOSS OF USE, DATA, OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, OR UNDER ANY OTHER LEGAL THEORY, ARISING OUT OF OR IN
 * CONNECTION WITH THE IMPLEMENTATION, USE, COMMERCIALIZATION, OR PERFORMANCE
 * OF THIS SOFTWARE.
 * 
 * \astek_eguard_library_license_stop
 */
#include "custom_auth_def.h"

const uint8_t g_symmetric_key[] = "ASTEK CORPORATION";
size_t  g_symmetric_key_size = 17; //Exclude last null value from string

const uint8_t tag_signer_pubkey[] = {
	0xfc, 0xc3, 0x82, 0xe7, 0x0b, 0x23, 0x0e, 0xc2, 0xae, 0xf3, 0xd0, 0x44, 0x56, 0xc6, 0xab, 0xf6, 
	0x81, 0xb1, 0xfd, 0x84, 0xb3, 0xc4, 0x21, 0xb6, 0x0e, 0x93, 0x9e, 0x06, 0x27, 0xb9, 0x17, 0x74, 
	0x0d, 0x5c, 0x93, 0xf4, 0x4f, 0x80, 0x26, 0x46, 0x1e, 0x90, 0xb2, 0xb1, 0x0c, 0xa0, 0x09, 0x97, 
	0xbb, 0x92, 0x6a, 0xe8, 0x1b, 0x7b, 0x0d, 0x47, 0x7a, 0xec, 0x3e, 0x3e, 0xd5, 0xef, 0x7c, 0x7f };
