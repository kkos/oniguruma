/*
 * encode-harness.c
 * contributed by Mark Griffin
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "oniguruma.h"


//#define PARSE_DEPTH_LIMIT   120
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
#ifdef WITH_READ_MAIN
    int i;

    fprintf(stdout, "match at %d  (%s)\n", r,
            ONIGENC_NAME(onig_get_encoding(reg)));
    for (i = 0; i < region->num_regs; i++) {
      fprintf(stdout, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
#endif
  }
  else if (r == ONIG_MISMATCH) {
#ifdef WITH_READ_MAIN
    fprintf(stdout, "search fail (%s)\n",
            ONIGENC_NAME(onig_get_encoding(reg)));
#endif
  }
  else { /* error */
#ifdef WITH_READ_MAIN
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];

    onig_error_code_to_str((UChar* )s, r);
    fprintf(stdout, "ERROR: %s\n", s);
    fprintf(stdout, "  (%s)\n", ONIGENC_NAME(onig_get_encoding(reg)));
#endif
    onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
    return -1;
  }

  onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
  return 0;
}

static long INPUT_COUNT;
static long EXEC_COUNT;
static long EXEC_COUNT_INTERVAL;
static long REGEX_SUCCESS_COUNT;
static long VALID_STRING_COUNT;

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

  EXEC_COUNT++;
  EXEC_COUNT_INTERVAL++;

  onig_initialize(&enc, 1);
  onig_set_retry_limit_in_match(RETRY_LIMIT);
  //onig_set_parse_depth_limit(PARSE_DEPTH_LIMIT);

  r = onig_new(&reg, pattern, pattern_end,
               options, enc, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r, &einfo);
#ifdef WITH_READ_MAIN
    fprintf(stdout, "ERROR: %s\n", s);
#endif
    onig_end();

    if (r == ONIGERR_PARSER_BUG ||
        r == ONIGERR_STACK_BUG  ||
        r == ONIGERR_UNDEFINED_BYTECODE ||
        r == ONIGERR_UNEXPECTED_BYTECODE) {
      return -2;
    }
    else
      return -1;
  }
  REGEX_SUCCESS_COUNT++;

  if (onigenc_is_valid_mbc_string(enc, str, end) != 0) {
    VALID_STRING_COUNT++;
    r = search(reg, str, end);
  }

  onig_free(reg);
  onig_end();
  return 0;
}

#if 0
static void
output_data(char* path, const uint8_t * data, size_t size)
{
  int fd;
  ssize_t n;

  fd = open(path, O_CREAT|O_RDWR, S_IRUSR|S_IRGRP|S_IROTH);
  if (fd == -1) {
    fprintf(stderr, "ERROR: output_data(): can't open(%s)\n", path);
    return ;
  }

  n = write(fd, (const void* )data, size);
  if (n != size) {
    fprintf(stderr, "ERROR: output_data(): n: %ld, size: %ld\n", n, size);
  }
  close(fd);
}
#endif


#define MAX_PATTERN_SIZE     100
#define NUM_CONTROL_BYTES      1

#define EXEC_PRINT_INTERVAL  20000000

int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size)
{
  INPUT_COUNT++;

  if (Size < NUM_CONTROL_BYTES) return 0;

  int pattern_size;
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

  pattern_size = remaining_size / 2;
  if (pattern_size > MAX_PATTERN_SIZE)
    pattern_size = MAX_PATTERN_SIZE;

#if defined(UTF16_BE) || defined(UTF16_LE)
  if (pattern_size % 2 == 1) pattern_size--;
#endif

  // copy first PATTERN_SIZE bytes off to be the pattern
  unsigned char *pattern = (unsigned char *)malloc(pattern_size != 0 ? pattern_size : 1);
  memcpy(pattern, data, pattern_size);
  pattern_end = pattern + pattern_size;
  data += pattern_size;
  remaining_size -= pattern_size;

#if defined(UTF16_BE) || defined(UTF16_LE)
  if (remaining_size % 2 == 1) remaining_size--;
#endif

  unsigned char *str = (unsigned char*)malloc(remaining_size != 0 ? remaining_size : 1);
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

  if (r == -2) {
    //output_data("parser-bug", Data, Size);
    exit(-2);
  }

  if (EXEC_COUNT_INTERVAL == EXEC_PRINT_INTERVAL) {
    char d[64];
    time_t t;
    float fexec, freg, fvalid;

    t = time(NULL);
    strftime(d, sizeof(d), "%m/%d %H:%M:%S", localtime(&t));

    fexec  = (float )EXEC_COUNT / INPUT_COUNT;
    freg   = (float )REGEX_SUCCESS_COUNT / INPUT_COUNT;
    fvalid = (float )VALID_STRING_COUNT / INPUT_COUNT;

    fprintf(stdout, "%s: %ld: EXEC:%.2f, REG:%.2f, VALID:%.2f\n",
            d, EXEC_COUNT, fexec, freg, fvalid);

    EXEC_COUNT_INTERVAL = 0;
  }
  return r;
}

#ifdef WITH_READ_MAIN

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
