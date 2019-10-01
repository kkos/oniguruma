/*
 * syntax-harness.c
 * contributed by Mark Griffin
 */
#include <stdio.h>
#include <string.h>
#include "oniguruma.h"

#include <stdlib.h>

#define DEFAULT_LIMIT 120
typedef unsigned char uint8_t;

extern int exec(OnigSyntaxType* syntax, char* apattern, char* astr)
{
  int r;
  unsigned char *start, *range, *end;
  regex_t* reg;
  OnigErrorInfo einfo;
  OnigRegion *region;
  UChar* pattern = (UChar* )apattern;
  UChar* str     = (UChar* )astr;

  r = onig_new(&reg, pattern, pattern + strlen((char* )pattern),
               ONIG_OPTION_DEFAULT, ONIG_ENCODING_ASCII, syntax, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r, &einfo);
    fprintf(stdout, "ERROR: %s\n", s);
    return -1;
  }

  region = onig_region_new();

  end   = str + strlen((char* )str);
  start = str;
  range = end;
  r = onig_search(reg, str, end, start, range, region, ONIG_OPTION_NONE);
  if (r >= 0) {
    int i;

    fprintf(stdout, "match at %d\n", r);
    for (i = 0; i < region->num_regs; i++) {
      fprintf(stdout, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
  }
  else if (r == ONIG_MISMATCH) {
    fprintf(stdout, "search fail\n");
  }
  else { /* error */
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r);
    fprintf(stdout, "ERROR: %s\n", s);
    onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
    onig_free(reg);
    return -1;
  }

  onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
  onig_free(reg);
  return 0;
}

#define PATTERN_SIZE 64
#define NUM_CONTROL_BYTES 1
#define MIN_STR_SIZE  1
int LLVMFuzzerTestOneInput(const uint8_t * Data, size_t Size)
{
  if (Size <= (NUM_CONTROL_BYTES + PATTERN_SIZE + MIN_STR_SIZE))
    return 0;
  if (Size > 0x1000)
    return 0;
  size_t remaining_size = Size;
  unsigned char *data = (unsigned char *)(Data);

  // pull off one byte to switch syntax choice
  unsigned char syntax_choice = data[0];
  data++;
  remaining_size--;

  // copy first PATTERN_SIZE bytes off to be the pattern
  unsigned char *pattern = (unsigned char *)malloc(PATTERN_SIZE);
  memcpy(pattern, data, PATTERN_SIZE);
  data += PATTERN_SIZE;
  remaining_size -= PATTERN_SIZE;

  unsigned char *str = (unsigned char*)malloc(remaining_size);
  memcpy(str, data, remaining_size);
  
  OnigEncoding use_encs[] = { ONIG_ENCODING_ASCII };
  onig_initialize(use_encs, sizeof(use_encs)/sizeof(use_encs[0]));

  onig_set_retry_limit_in_match(DEFAULT_LIMIT);
  onig_set_parse_depth_limit(DEFAULT_LIMIT);

  OnigSyntaxType *syntaxes[] = {
    ONIG_SYNTAX_POSIX_EXTENDED,
    ONIG_SYNTAX_EMACS,
    ONIG_SYNTAX_GREP,
    ONIG_SYNTAX_GNU_REGEX,
    ONIG_SYNTAX_JAVA,
    ONIG_SYNTAX_PERL_NG,
    ONIG_SYNTAX_RUBY,
    ONIG_SYNTAX_ONIGURUMA,
  }; 
  OnigSyntaxType *syntax = syntaxes[syntax_choice % 8];
  
  int r;
  r = exec(syntax, (char *)pattern, (char *)str);
  // r = exec(ONIG_SYNTAX_JAVA, "\\p{XDigit}\\P{XDigit}[a-c&&b-g]", "bgc");

  onig_end();

  free(pattern);
  free(str);

  return 0;
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
