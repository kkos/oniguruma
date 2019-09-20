/*
 * regset-harness.c
 * Copyright (c) 2019  K.Kosako
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


#define RETRY_LIMIT    3500

typedef unsigned char uint8_t;

static OnigEncoding ENC;

static int
search(OnigRegSet* set, unsigned char* str, unsigned char* end)
{
  int r;
  int match_pos;
  unsigned char *start, *range;

  start = str;
  range = end;
  r = onig_regset_search(set, str, end, start, range,
                         ONIG_REGSET_POSITION_LEAD, ONIG_OPTION_NONE, &match_pos);
  if (r >= 0) {
#ifdef WITH_READ_MAIN
    int i;
    int match_index;
    OnigRegion* region;

    fprintf(stdout, "match at %d  (%s)\n", r, ONIGENC_NAME(ENC));
    match_index = r;
    region = onig_regset_get_region(set, match_index);
    if (region == 0) {
      fprintf(stdout, "ERROR: can't get region.\n");
      return -1;
    }

    for (i = 0; i < region->num_regs; i++) {
      fprintf(stdout, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
#endif
  }
  else if (r == ONIG_MISMATCH) {
#ifdef WITH_READ_MAIN
    fprintf(stdout, "search fail (%s)\n", ONIGENC_NAME(ENC));
#endif
  }
  else { /* error */
#ifdef WITH_READ_MAIN
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];

    onig_error_code_to_str((UChar* )s, r);
    fprintf(stdout, "ERROR: %s\n", s);
    fprintf(stdout, "  (%s)\n", ONIGENC_NAME(ENC));
#endif
    return -1;
  }

  return 0;
}

static long INPUT_COUNT;
static long EXEC_COUNT;
static long EXEC_COUNT_INTERVAL;
static long REGEX_SUCCESS_COUNT;
static long VALID_STRING_COUNT;

static int
exec(OnigEncoding enc, OnigOptionType options,
     int reg_num, UChar* pat[], UChar* pat_end[], char* astr, UChar* end)
{
  int r;
  int i;
  OnigRegSet* set;
  regex_t* reg;
  OnigErrorInfo einfo;
  UChar* str = (UChar* )astr;

  EXEC_COUNT++;
  EXEC_COUNT_INTERVAL++;

  onig_initialize(&enc, 1);
  onig_set_retry_limit_in_match(RETRY_LIMIT);

  r = onig_regset_new(&set, 0, NULL);
  if (r != 0) return -1;

  for (i = 0; i < reg_num; i++) {
    r = onig_new(&reg, pat[i], pat_end[i],
                 ONIG_OPTION_DEFAULT, ONIG_ENCODING_UTF8, ONIG_SYNTAX_DEFAULT,
                 &einfo);
    if (r != 0) {
#ifdef WITH_READ_MAIN
      char s[ONIG_MAX_ERROR_MESSAGE_LEN];

      onig_error_code_to_str((UChar* )s, r, &einfo);
      fprintf(stdout, "ERROR: %s  /%s/\n", s, pat[i]);
#endif
      onig_regset_free(set);
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

    r = onig_regset_add(set, reg);
    if (r != 0) {
      onig_regset_free(set);
      fprintf(stdout, "ERROR: onig_regset_add(): /%s/\n", pat[i]);
      return r;
    }
  }

  REGEX_SUCCESS_COUNT++;

  if (onigenc_is_valid_mbc_string(enc, str, end) != 0) {
    VALID_STRING_COUNT++;
    r = search(set, str, end);
  }

  onig_regset_free(set);
  onig_end();
  return 0;
}

#define MAX_DATA_SIZE       1024
#define MAX_PATTERN_SIZE      30
#define NUM_CONTROL_BYTES      1

#define EXEC_PRINT_INTERVAL  2000000

extern int
LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size)
{
  int r, i;
  int pattern_size;
  unsigned char *str_null_end;
  size_t remaining_size;
  unsigned char *data;
  unsigned int reg_num;
  unsigned char* pat[256];
  unsigned char* pat_end[256];
  unsigned char *alloc_pattern;
  unsigned char *p;
  int len;

  INPUT_COUNT++;

  if (Size < NUM_CONTROL_BYTES) return 0;
  if (Size > MAX_DATA_SIZE)     return 0;

  remaining_size = Size;
  data = (unsigned char* )(Data);

  reg_num = data[0];

  data++;
  remaining_size--;

  if (remaining_size < reg_num * 2) {
    reg_num = reg_num % 15;  // zero is OK.
  }

  if (reg_num == 0)
    pattern_size = 1;
  else
    pattern_size = remaining_size / (reg_num * 2);
    
  if (pattern_size > MAX_PATTERN_SIZE)
    pattern_size = MAX_PATTERN_SIZE;

  len = pattern_size * reg_num;
  if (len == 0) len = 1;
  p = alloc_pattern = (unsigned char* )malloc(len);
  for (i = 0; i < reg_num; i++) {
    pat[i] = p;
    memcpy(p, data, pattern_size);
    pat_end[i] = p + pattern_size;
    data += pattern_size;
    remaining_size -= pattern_size;
  }

  unsigned char *str = (unsigned char*)malloc(remaining_size != 0 ? remaining_size : 1);
  memcpy(str, data, remaining_size);
  str_null_end = str + remaining_size;

  //ENC = ONIG_ENCODING_UTF8;
  ENC = ONIG_ENCODING_ISO_8859_1;

  r = exec(ENC, ONIG_OPTION_NONE, reg_num, pat, pat_end, (char* )str, str_null_end);

  free(alloc_pattern);
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
