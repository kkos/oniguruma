/*
 * base.c  contributed by Mark Griffin
 * Copyright (c) 2019-2021  K.Kosako
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

#define PARSE_DEPTH_LIMIT               8
#define MAX_SUBEXP_CALL_NEST_LEVEL      8
#define SUBEXP_CALL_LIMIT            1000
#define BASE_RETRY_LIMIT            20000
#define BASE_LENGTH                  2048
#define MATCH_STACK_LIMIT        10000000
#define MAX_REM_SIZE              1048576
#define MAX_SLOW_REM_SIZE            1024
#define SLOW_RETRY_LIMIT             2000
#define SLOW_SUBEXP_CALL_LIMIT        100
#define MAX_SLOW_BACKWARD_REM_SIZE    200

//#define EXEC_PRINT_INTERVAL      500000
//#define DUMP_DATA_INTERVAL       100000
//#define STAT_PATH                "fuzzer.stat_log"

#define OPTIONS_AT_COMPILE   (ONIG_OPTION_IGNORECASE | ONIG_OPTION_EXTEND | ONIG_OPTION_MULTILINE | ONIG_OPTION_SINGLELINE | ONIG_OPTION_FIND_LONGEST | ONIG_OPTION_FIND_NOT_EMPTY | ONIG_OPTION_NEGATE_SINGLELINE | ONIG_OPTION_DONT_CAPTURE_GROUP | ONIG_OPTION_CAPTURE_GROUP | ONIG_OPTION_WORD_IS_ASCII | ONIG_OPTION_DIGIT_IS_ASCII | ONIG_OPTION_SPACE_IS_ASCII | ONIG_OPTION_POSIX_IS_ASCII | ONIG_OPTION_TEXT_SEGMENT_EXTENDED_GRAPHEME_CLUSTER | ONIG_OPTION_TEXT_SEGMENT_WORD | ONIG_OPTION_IGNORECASE_IS_ASCII)

#define OPTIONS_AT_RUNTIME   (ONIG_OPTION_NOTBOL | ONIG_OPTION_NOTEOL | ONIG_OPTION_CHECK_VALIDITY_OF_STRING | ONIG_OPTION_NOT_BEGIN_STRING | ONIG_OPTION_NOT_END_STRING | ONIG_OPTION_NOT_BEGIN_POSITION)


#define ADJUST_LEN(enc, len) do {\
  int mlen = ONIGENC_MBC_MINLEN(enc);\
  if (mlen != 1) { len -= len % mlen; }\
} while (0)

typedef unsigned char uint8_t;


//#define TEST_PATTERN

#ifdef TEST_PATTERN

#if 1
unsigned char TestPattern[] = {
};
#endif

#endif /* TEST_PATTERN */

#ifdef DUMP_INPUT
static void
dump_input(unsigned char* data, size_t len)
{
  static FILE* DumpFp;
  static char end[] = { 'E', 'N', 'D' };

  if (DumpFp == 0)
    DumpFp = fopen("dump-input", "w");

  fseek(DumpFp, 0, SEEK_SET);
  fwrite(data, sizeof(unsigned char), len, DumpFp);
  fwrite(end,  sizeof(char), sizeof(end), DumpFp);
  fflush(DumpFp);
}
#endif

#ifdef DUMP_DATA_INTERVAL
static void
dump_file(char* path, unsigned char* data, size_t len)
{
  FILE* fp;

  fp = fopen(path, "w");
  fwrite(data, sizeof(unsigned char), len, fp);
  fclose(fp);
}
#endif

#ifdef STANDALONE
#include <ctype.h>

static void
dump_data(FILE* fp, unsigned char* data, int len)
{
  int i;

  fprintf(fp, "{\n");
  for (i = 0; i < len; i++) {
    unsigned char c = data[i];

    if (isprint((int )c)) {
      if (c == '\\')
        fprintf(fp, " '\\\\'");
      else
        fprintf(fp, " '%c'", c);
    }
    else {
      fprintf(fp, "0x%02x", (int )c);
    }

    if (i == len - 1) {
      fprintf(fp, "\n");
    }
    else {
      if (i % 8 == 7)
        fprintf(fp, ",\n");
      else
        fprintf(fp, ", ");
    }
  }
  fprintf(fp, "};\n");
}

#else

static void
output_current_time(FILE* fp)
{
  char d[64];
  time_t t;

  t = time(NULL);
  strftime(d, sizeof(d), "%m/%d %H:%M:%S", localtime(&t));

  fprintf(fp, "%s", d);
}

#endif

static int
progress_callout_func(OnigCalloutArgs* args, void* user_data)
{
  return ONIG_CALLOUT_SUCCESS;
}

static int
search(regex_t* reg, unsigned char* str, unsigned char* end, OnigOptionType options, int backward, int sl)
{
  int r;
  unsigned char *start, *range;
  OnigRegion *region;
  unsigned int retry_limit;
  size_t len;

  region = onig_region_new();

  len = (size_t )(end - str);
  if (len < BASE_LENGTH) {
    if (sl >= 2)
      retry_limit = (unsigned int )SLOW_RETRY_LIMIT;
    else
      retry_limit = (unsigned int )BASE_RETRY_LIMIT;
  }
  else
    retry_limit = (unsigned int )(BASE_RETRY_LIMIT * BASE_LENGTH / len);

#ifdef STANDALONE
  fprintf(stdout, "retry limit: %u\n", retry_limit);
#endif

  onig_set_retry_limit_in_search(retry_limit);
  onig_set_match_stack_limit_size(MATCH_STACK_LIMIT);
  if (sl >= 2)
    onig_set_subexp_call_limit_in_search(SLOW_SUBEXP_CALL_LIMIT);
  else
    onig_set_subexp_call_limit_in_search(SUBEXP_CALL_LIMIT);

  if (backward != 0) {
    start = end;
    range = str;
  }
  else {
    start = str;
    range = end;
  }

  r = onig_search(reg, str, end, start, range, region, (options & OPTIONS_AT_RUNTIME));
  if (r >= 0) {
#ifdef STANDALONE
    int i;

    fprintf(stdout, "match at %d  (%s)\n", r,
            ONIGENC_NAME(onig_get_encoding(reg)));
    for (i = 0; i < region->num_regs; i++) {
      fprintf(stdout, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
#endif
  }
  else if (r == ONIG_MISMATCH) {
#ifdef STANDALONE
    fprintf(stdout, "search fail (%s)\n",
            ONIGENC_NAME(onig_get_encoding(reg)));
#endif
  }
  else { /* error */
#ifdef STANDALONE
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];

    onig_error_code_to_str((UChar* )s, r);
    fprintf(stdout, "ERROR: %s\n", s);
    fprintf(stdout, "  (%s)\n", ONIGENC_NAME(onig_get_encoding(reg)));
#endif
    onig_region_free(region, 1 /* 1:free self, 0:free contents only */);

    if (r == ONIGERR_STACK_BUG ||
        r == ONIGERR_UNDEFINED_BYTECODE ||
        r == ONIGERR_UNEXPECTED_BYTECODE)
      return -2;

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
exec(OnigEncoding enc, OnigOptionType options, OnigSyntaxType* syntax,
     char* apattern, char* apattern_end, char* astr, UChar* end, int backward,
     int sl)
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
  (void)onig_set_progress_callout(progress_callout_func);
#ifdef PARSE_DEPTH_LIMIT
  onig_set_parse_depth_limit(PARSE_DEPTH_LIMIT);
#endif
  onig_set_subexp_call_max_nest_level(MAX_SUBEXP_CALL_NEST_LEVEL);

  r = onig_new(&reg, pattern, pattern_end,
               (options & OPTIONS_AT_COMPILE), enc, syntax, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r, &einfo);
#ifdef STANDALONE
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

  r = search(reg, pattern, pattern_end, options, backward, sl);
  if (r == -2) return -2;

  if (onigenc_is_valid_mbc_string(enc, str, end) != 0) {
    VALID_STRING_COUNT++;
    r = search(reg, str, end, options, backward, sl);
    if (r == -2) return -2;
  }

  onig_free(reg);
  onig_end();
  return 0;
}

static int
alloc_exec(OnigEncoding enc, OnigOptionType options, OnigSyntaxType* syntax,
           int backward, int pattern_size, size_t rem_size, unsigned char *data)
{
  extern int onig_detect_can_be_slow_pattern(const UChar* pattern, const UChar* pattern_end, OnigOptionType option, OnigEncoding enc, OnigSyntaxType* syntax);

  int r;
  int sl;
  unsigned char *pattern;
  unsigned char *pattern_end;
  unsigned char *str_null_end;

#ifdef TEST_PATTERN
  pattern = (unsigned char *)malloc(sizeof(TestPattern));
  memcpy(pattern, TestPattern, sizeof(TestPattern));
  pattern_end = pattern + sizeof(TestPattern);
#else
  pattern = (unsigned char *)malloc(pattern_size != 0 ? pattern_size : 1);
  memcpy(pattern, data, pattern_size);
  pattern_end = pattern + pattern_size;
#endif

  data += pattern_size;
  rem_size -= pattern_size;

  if (rem_size > MAX_REM_SIZE) rem_size = MAX_REM_SIZE;

  sl = onig_detect_can_be_slow_pattern(pattern, pattern_end, options, enc, syntax);
#ifdef STANDALONE
  fprintf(stdout, "sl: %d\n", sl);
#endif
  if (sl > 0) {
    if (rem_size > MAX_SLOW_REM_SIZE)
      rem_size = MAX_SLOW_REM_SIZE;
  }
  if (backward != 0 && enc == ONIG_ENCODING_GB18030) {
    if (rem_size > MAX_SLOW_BACKWARD_REM_SIZE)
      rem_size = MAX_SLOW_BACKWARD_REM_SIZE;
  }

  ADJUST_LEN(enc, rem_size);
#ifdef STANDALONE
  fprintf(stdout, "rem_size: %ld\n", rem_size);
#endif

  unsigned char *str = (unsigned char*)malloc(rem_size != 0 ? rem_size : 1);
  memcpy(str, data, rem_size);
  str_null_end = str + rem_size;

  r = exec(enc, options, syntax,
           (char *)pattern, (char *)pattern_end,
           (char *)str, str_null_end, backward, sl);

  free(pattern);
  free(str);
  return r;
}

#ifdef SYNTAX_TEST
#define NUM_CONTROL_BYTES      8
#else
#define NUM_CONTROL_BYTES      7
#endif

int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size)
{
#if !defined(UTF16_BE) && !defined(UTF16_LE)
  static OnigEncoding encodings[] = {
    ONIG_ENCODING_UTF8,
    ONIG_ENCODING_UTF8,
    ONIG_ENCODING_UTF8,
    ONIG_ENCODING_UTF8,
    ONIG_ENCODING_UTF8,
    ONIG_ENCODING_UTF8,
    ONIG_ENCODING_UTF8,
    ONIG_ENCODING_UTF8,
    ONIG_ENCODING_ASCII,
    ONIG_ENCODING_EUC_JP,
    ONIG_ENCODING_EUC_TW,
    ONIG_ENCODING_EUC_KR,
    ONIG_ENCODING_EUC_CN,
    ONIG_ENCODING_SJIS,
    ONIG_ENCODING_KOI8_R,
    ONIG_ENCODING_CP1251,
    ONIG_ENCODING_BIG5,
    ONIG_ENCODING_GB18030,
    ONIG_ENCODING_UTF16_BE,
    ONIG_ENCODING_UTF16_LE,
    ONIG_ENCODING_UTF16_BE,
    ONIG_ENCODING_UTF16_LE,
    ONIG_ENCODING_UTF32_BE,
    ONIG_ENCODING_UTF32_LE,
    ONIG_ENCODING_UTF32_BE,
    ONIG_ENCODING_UTF32_LE,
    ONIG_ENCODING_ISO_8859_1,
    ONIG_ENCODING_ISO_8859_2,
    ONIG_ENCODING_ISO_8859_3,
    ONIG_ENCODING_ISO_8859_4,
    ONIG_ENCODING_ISO_8859_5,
    ONIG_ENCODING_ISO_8859_6,
    ONIG_ENCODING_ISO_8859_7,
    ONIG_ENCODING_ISO_8859_8,
    ONIG_ENCODING_ISO_8859_9,
    ONIG_ENCODING_ISO_8859_10,
    ONIG_ENCODING_ISO_8859_11,
    ONIG_ENCODING_ISO_8859_13,
    ONIG_ENCODING_ISO_8859_14,
    ONIG_ENCODING_ISO_8859_15,
    ONIG_ENCODING_ISO_8859_16
  };
  unsigned char encoding_choice;
#endif

#ifdef SYNTAX_TEST
  static OnigSyntaxType* syntaxes[] = {
    ONIG_SYNTAX_POSIX_EXTENDED,
    ONIG_SYNTAX_EMACS,
    ONIG_SYNTAX_GREP,
    ONIG_SYNTAX_GNU_REGEX,
    ONIG_SYNTAX_JAVA,
    ONIG_SYNTAX_PERL_NG,
    ONIG_SYNTAX_ONIGURUMA
  };

#ifdef STANDALONE
  static char* syntax_names[] = {
    "Posix Extended",
    "Emacs",
    "Grep",
    "GNU Regex",
    "Java",
    "Perl+NG",
    "Oniguruma"
  };
#endif

  unsigned char syntax_choice;
#endif

  int r;
  int backward;
  int pattern_size;
  size_t rem_size;
  unsigned char *data;
  unsigned char pattern_size_choice;
  OnigOptionType  options;
  OnigEncoding    enc;
  OnigSyntaxType* syntax;

#ifndef STANDALONE
  static FILE* STAT_FP;
#endif

  INPUT_COUNT++;

#ifdef DUMP_DATA_INTERVAL
  if (INPUT_COUNT % DUMP_DATA_INTERVAL == 0) {
    char path[20];
    sprintf(path, "dump-%ld", INPUT_COUNT);
    dump_file(path, (unsigned char* )Data, Size);
  }
#endif

  if (Size < NUM_CONTROL_BYTES) return 0;

  rem_size = Size;
  data = (unsigned char* )(Data);

#ifdef UTF16_BE
  enc = ONIG_ENCODING_UTF16_BE;
#else
#ifdef UTF16_LE
  enc = ONIG_ENCODING_UTF16_LE;
#else
  encoding_choice = data[0];
  data++;
  rem_size--;

  int num_encodings = sizeof(encodings)/sizeof(encodings[0]);
  enc = encodings[encoding_choice % num_encodings];
#endif
#endif

#ifdef SYNTAX_TEST
  syntax_choice = data[0];
  data++;
  rem_size--;

  int num_syntaxes = sizeof(syntaxes)/sizeof(syntaxes[0]);
  syntax = syntaxes[syntax_choice % num_syntaxes];
#else
  syntax = ONIG_SYNTAX_DEFAULT;
#endif

  if ((data[3] & 0xc0) == 0)
    options = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
  else
    options = data[0] & ONIG_OPTION_IGNORECASE;

  data++; rem_size--;
  data++; rem_size--;
  data++; rem_size--;
  data++; rem_size--;

  pattern_size_choice = data[0];
  data++; rem_size--;

  backward = (data[0] == 0xbb);
  data++; rem_size--;

  if (backward != 0) {
    options = options & ~ONIG_OPTION_FIND_LONGEST;
  }

  if (rem_size == 0)
    pattern_size = 0;
  else {
    pattern_size = (int )pattern_size_choice % rem_size;
    ADJUST_LEN(enc, pattern_size);
  }

#ifdef STANDALONE
  dump_data(stdout, data, pattern_size);
#ifdef SYNTAX_TEST
  fprintf(stdout,
          "enc: %s, syntax: %s, options: %u, pattern_size: %d, back:%d\n",
          ONIGENC_NAME(enc),
          syntax_names[syntax_choice % num_syntaxes],
          options,
          pattern_size, backward);
#else
  fprintf(stdout, "enc: %s, options: %u, pattern_size: %d, back:%d\n",
          ONIGENC_NAME(enc), options, pattern_size, backward);
#endif
#endif

#ifdef DUMP_INPUT
  dump_input((unsigned char* )Data, Size);
#endif

  r = alloc_exec(enc, options, syntax, backward, pattern_size,
                 rem_size, data);
  if (r == -2) exit(-2);

#ifndef STANDALONE
#ifdef EXEC_PRINT_INTERVAL
  if (EXEC_COUNT_INTERVAL == EXEC_PRINT_INTERVAL) {
    float fexec, freg, fvalid;

    if (STAT_FP == 0) {
#ifdef STAT_PATH
      STAT_FP = fopen(STAT_PATH, "a");
#else
      STAT_FP = stdout;
#endif
    }

    output_current_time(STAT_FP);

    if (INPUT_COUNT != 0) { // overflow check
      fexec  = (float )EXEC_COUNT / INPUT_COUNT;
      freg   = (float )REGEX_SUCCESS_COUNT / INPUT_COUNT;
      fvalid = (float )VALID_STRING_COUNT / INPUT_COUNT;

      fprintf(STAT_FP, ": %ld: EXEC:%.2f, REG:%.2f, VALID:%.2f\n",
              EXEC_COUNT, fexec, freg, fvalid);
      fflush(STAT_FP);
    }
    else {
      fprintf(STAT_FP, ": ignore (input count overflow)\n");
    }

    EXEC_COUNT_INTERVAL = 0;
  }
  else if (EXEC_COUNT == 1) {
    output_current_time(stdout);
    fprintf(stdout, ": ------------ START ------------\n");
  }
#endif
#endif

  return r;
}

#ifdef STANDALONE

#define MAX_INPUT_DATA_SIZE  4194304

extern int main(int argc, char* argv[])
{
  size_t max_size;
  size_t n;
  uint8_t Data[MAX_INPUT_DATA_SIZE];

  if (argc > 1) {
    max_size = (size_t )atoi(argv[1]);
  }
  else {
    max_size = sizeof(Data);
  }

  n = read(0, Data, max_size);
  fprintf(stdout, "read size: %ld, max_size: %ld\n", n, max_size);

  LLVMFuzzerTestOneInput(Data, n);
  return 0;
}
#endif /* STANDALONE */
