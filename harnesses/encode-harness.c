/*
 * encode-harness.c
 * contributed by Mark Griffin
 */
#include <stdio.h>
#include "oniguruma.h"

#include <stdlib.h>
#include <string.h>

#define PARSE_DEPTH_LIMIT   120
#define RETRY_LIMIT        3500

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
     char* apattern, char* apattern_end, char* astr, UChar* end)
{
  int r;
  regex_t* reg;
  OnigErrorInfo einfo;
  UChar* pattern = (UChar* )apattern;
  UChar* str     = (UChar* )astr;
  UChar* pattern_end = (UChar* )apattern_end;

  onig_initialize(&enc, 1);
  onig_set_retry_limit_in_match(RETRY_LIMIT);
  onig_set_parse_depth_limit(PARSE_DEPTH_LIMIT);

  r = onig_new(&reg, pattern, pattern_end,
               options, enc, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r, &einfo);
    fprintf(stdout, "ERROR: %s\n", s);
    onig_end();
    return -1;
  }

  if (onigenc_is_valid_mbc_string(enc, str, end) != 0) {
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
  size_t remaining_size;
  unsigned char *data;

  // pull off one byte to switch off
#if !defined(UTF16_BE) && !defined(UTF16_LE)
  OnigEncodingType *encodings[] = {
	  ONIG_ENCODING_SJIS,
	  ONIG_ENCODING_EUC_JP,
	  ONIG_ENCODING_CP1251,
	  ONIG_ENCODING_ISO_8859_1,
	  ONIG_ENCODING_UTF8,
    ONIG_ENCODING_KOI8_R,
    ONIG_ENCODING_BIG5,
    ONIG_ENCODING_GB18030,
    ONIG_ENCODING_EUC_TW
  };

  unsigned char encoding_choice;
#endif

  remaining_size = Size;
  data = (unsigned char* )(Data);

#if !defined(UTF16_BE) && !defined(UTF16_LE)
  encoding_choice = data[0];
#endif

  data++;
  remaining_size--;

  // copy first PATTERN_SIZE bytes off to be the pattern
  unsigned char *pattern = (unsigned char *)malloc(PATTERN_SIZE+4);
  memset(pattern, 0, PATTERN_SIZE+4);
  memcpy(pattern, data, PATTERN_SIZE);
  pattern_end = pattern + PATTERN_SIZE;
  data += PATTERN_SIZE;
  remaining_size -= PATTERN_SIZE;

#if defined(UTF16_BE) || defined(UTF16_LE)
  if (remaining_size % 2 == 1)
    remaining_size--;
#endif

  unsigned char *str = (unsigned char*)malloc(remaining_size+4);
  memset(str, 0, remaining_size+4);
  memcpy(str, data, remaining_size);
  str_null_end = str + remaining_size;

  int r;
  OnigEncodingType *enc;

#ifdef UTF16_BE
  enc = ONIG_ENCODING_UTF16_BE;
#else
#ifdef UTF16_LE
  enc = ONIG_ENCODING_UTF16_LE;
#else
  int num_encodings = sizeof(encodings)/sizeof(encodings[0]);
  enc = encodings[encoding_choice % num_encodings];
#endif
#endif

  r = exec(enc, ONIG_OPTION_NONE, (char *)pattern, (char *)pattern_end,
           (char *)str, str_null_end);

  free(pattern);
  free(str);

  return r;
}

#ifdef WITH_READ_MAIN

#include <unistd.h>

extern int main(int argc, char* argv[])
{
  size_t n;
  uint8_t Data[10000];

  n = read(0, Data, sizeof(Data));
  fprintf(stdout, "n: %ld\n", n);
  LLVMFuzzerTestOneInput(Data, n);

  return 0;
}
#endif /* WITH_READ_MAIN */
