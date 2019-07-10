/*
 * encode-harness.c
 * contributed by Mark Griffin
 */
#include <stdio.h>
#include "oniguruma.h"

#include <stdlib.h>
#include <string.h>

#define DEFAULT_LIMIT 120
typedef unsigned char uint8_t;

static int
search(regex_t* reg, unsigned char* str, unsigned char* end)
{
  int r;
  unsigned char *start, *range;
  OnigRegion *region;

  region = onig_region_new();

  start = str;
  range = end;
  r = onig_search(reg, str, end, start, range, region, ONIG_OPTION_NONE);
  if (r >= 0) {
    int i;

    fprintf(stdout, "match at %d  (%s)\n", r,
            ONIGENC_NAME(onig_get_encoding(reg)));
    for (i = 0; i < region->num_regs; i++) {
      fprintf(stdout, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
  }
  else if (r == ONIG_MISMATCH) {
    fprintf(stdout, "search fail (%s)\n",
            ONIGENC_NAME(onig_get_encoding(reg)));
  }
  else { /* error */
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r);
    fprintf(stdout, "ERROR: %s\n", s);
    fprintf(stdout, "  (%s)\n", ONIGENC_NAME(onig_get_encoding(reg)));
    onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
    return -1;
  }

  onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
  return 0;
}

static int
exec(OnigEncoding enc, OnigOptionType options,
     char* apattern, char* apattern_end, char* astr, char* str_null_end)
{
  int r;
  unsigned char *end;
  regex_t* reg;
  OnigErrorInfo einfo;
  UChar* pattern = (UChar* )apattern;
  UChar* str     = (UChar* )astr;
  UChar* pattern_end = (UChar* )apattern_end;

  onig_initialize(&enc, 1);
  onig_set_retry_limit_in_match(DEFAULT_LIMIT);
  onig_set_parse_depth_limit(DEFAULT_LIMIT);

  r = onig_new(&reg, pattern, pattern_end,
               options, enc, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r, &einfo);
    fprintf(stdout, "ERROR: %s\n", s);
    onig_end();
    return -1;
  }

  if (onigenc_is_valid_mbc_string(enc, str, (UChar* )str_null_end) != 0) {
    end = str + onigenc_str_bytelen_null(enc, str);
    r = search(reg, str, end);
  }

  onig_free(reg);
  onig_end();
  return 0;
}

#define PATTERN_SIZE 32
#define NUM_CONTROL_BYTES 1
#define MIN_STR_SIZE  1
int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size)
{
  if (Size <= (NUM_CONTROL_BYTES + PATTERN_SIZE + MIN_STR_SIZE))
    return 0;
  if (Size > 0x1000)
    return 0;

  unsigned char *pattern_end;
  unsigned char *str_null_end;

  size_t remaining_size = Size;
  unsigned char *data = (unsigned char *)(Data);

  // pull off one byte to switch off
  unsigned char encoding_choice = data[0];
  data++;
  remaining_size--;

  // copy first PATTERN_SIZE bytes off to be the pattern
  unsigned char *pattern = (unsigned char *)malloc(PATTERN_SIZE+4);
  memset(pattern, 0, PATTERN_SIZE+4);
  memcpy(pattern, data, PATTERN_SIZE);
  pattern_end = pattern + PATTERN_SIZE;
  data += PATTERN_SIZE;
  remaining_size -= PATTERN_SIZE;

  unsigned char *str = (unsigned char*)malloc(remaining_size+4);
  memset(str, 0, remaining_size+4);
  memcpy(str, data, remaining_size);
  str_null_end = str + (remaining_size+4);

  int r;
  OnigEncodingType *encodings[] = {
	  ONIG_ENCODING_SJIS,
	  ONIG_ENCODING_EUC_JP,
	  ONIG_ENCODING_CP1251,
	  ONIG_ENCODING_ISO_8859_1,
	  ONIG_ENCODING_UTF8,
          ONIG_ENCODING_UTF16_BE,
          ONIG_ENCODING_KOI8_R,
          ONIG_ENCODING_BIG5
  };
  int num_encodings = sizeof(encodings)/sizeof(encodings[0]);
  OnigEncodingType *enc = encodings[encoding_choice % num_encodings];

  r = exec(enc, ONIG_OPTION_NONE, (char *)pattern, (char *)pattern_end,
           (char *)str, (char *)str_null_end);

  free(pattern);
  free(str);

  return r;
}
