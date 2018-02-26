/*
 * count.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oniguruma.h"

OnigEncoding ENC = ONIG_ENCODING_UTF8;

static int
test(char* in_pattern, char* in_str)
{
  int r;
  unsigned char *start, *range, *end;
  regex_t* reg;
  OnigErrorInfo einfo;
  OnigRegion *region;
  OnigMatchParam* mp;
  UChar* pattern;
  UChar* str;

  pattern = (UChar* )in_pattern;
  str = (UChar* )in_str;

  r = onig_new(&reg, pattern, pattern + strlen((char* )pattern),
	ONIG_OPTION_DEFAULT, ENC, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r, &einfo);
    fprintf(stderr, "COMPILE ERROR: %d: %s\n", r, s);
    return -1;
  }

  region = onig_region_new();
  mp     = onig_new_match_param(reg);

  end   = str + strlen((char* )str);
  start = str;
  range = end;
  r = onig_search_with_param(reg, str, end, start, range, region,
                             ONIG_OPTION_NONE, mp);
  if (r >= 0) {
    int slot;
    OnigValue val;
    char* tag;

    fprintf(stdout, "match at %d\n", r);

  show_count:
    tag = "x";
    slot = 0;
    r = onig_get_callout_data_by_tag(reg, mp, tag, tag + strlen(tag), slot, 0, &val);
    if (r != ONIG_NORMAL) goto err;

    fprintf(stdout, "COUNT[x]: %ld\n", val.l);
  }
  else if (r == ONIG_MISMATCH) {
    fprintf(stdout, "search fail\n");
    goto show_count;
  }
  else { /* error */
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
  err:
    onig_error_code_to_str((UChar* )s, r);
    fprintf(stderr, "SEARCH ERROR: %d: %s\n", r, s);
  }

  onig_free_match_param(mp);
  onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
  onig_free(reg);
  return r;
}

extern int main(int argc, char* argv[])
{
  int r;
  int id;
  UChar* name;
  OnigEncoding use_encs[1];
  OnigType arg_types[4];
  OnigValue opt_defaults[4];

  use_encs[0] = ENC;

  r = onig_initialize(use_encs, sizeof(use_encs)/sizeof(use_encs[0]));
  if (r != ONIG_NORMAL) return -1;

  r = onig_initialize_builtin_callouts();
  if (r != ONIG_NORMAL) {
    fprintf(stderr, "onig_initialize_builtin_callouts(): %d\n", r);
    return -2;
  }

  test("abc(.(*COUNT[x]))*(*FAIL)", "abcdefg");
  test("abc(.(*COUNT[_any_]))*(.(*COUNT[x]))*d", "abcdefg");
  test("abc(.(*FAIL_COUNT[x]))*f", "abcdefg");

  onig_end();
  return 0;
}
