/**********************************************************************
  ascii.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2018  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "regint.h"   /* for USE_CALLOUT */

static int inited;

static int
init(void)
{
  if (inited == 0) {
#ifdef USE_CALLOUT

    int id;
    OnigType t_int;
    OnigType t_long;
    OnigEncoding enc;
    char* name;

    enc = ONIG_ENCODING_ASCII;
    t_int  = ONIG_TYPE_INT;
    t_long = ONIG_TYPE_LONG;

    name = "FAIL";        BC0_P(name, fail);
    name = "SUCCESS";     BC0_P(name, success);
    name = "ABORT";       BC0_P(name, abort);
    name = "ERROR";       BC1_P(name, error, &t_int);
    name = "COUNT";       BC0_P(name, count);
    name = "FAIL_COUNT";  BC0_R(name, count);
    name = "ONLY";        BC1_B(name, only, &t_long);

#endif /* USE_CALLOUT */

    inited = 1;
  }

  return ONIG_NORMAL;
}

static int
is_initialized(void)
{
  return inited;
}

static int
ascii_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else
    return FALSE;
}

OnigEncodingType OnigEncodingASCII = {
  onigenc_single_byte_mbc_enc_len,
  "US-ASCII",  /* name */
  1,           /* max enc length */
  1,           /* min enc length */
  onigenc_is_mbc_newline_0x0a,
  onigenc_single_byte_mbc_to_code,
  onigenc_single_byte_code_to_mbclen,
  onigenc_single_byte_code_to_mbc,
  onigenc_ascii_mbc_case_fold,
  onigenc_ascii_apply_all_case_fold,
  onigenc_ascii_get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  ascii_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match,
  init,
  is_initialized,
  onigenc_always_true_is_valid_mbc_string
};
