/*
 * count.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oniguruma.h"

#define ulen(enc, p) onigenc_str_bytelen_null(enc, (UChar* )p)

static int
test(OnigEncoding enc, char* in_pattern, char* in_str)
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

  r = onig_new(&reg, pattern, pattern + ulen(enc, pattern),
	ONIG_OPTION_DEFAULT, enc, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r, &einfo);
    fprintf(stderr, "COMPILE ERROR: %d: %s\n", r, s);
    return -1;
  }

  region = onig_region_new();
  mp     = onig_new_match_param(reg);

  end   = str + ulen(enc, str);
  start = str;
  range = end;
  r = onig_search_with_param(reg, str, end, start, range, region,
                             ONIG_OPTION_NONE, mp);
  if (r >= 0) {
    int slot;
    OnigValue val;
    char* tag;
    int tag_len;

    fprintf(stdout, "match at %d\n", r);

  show_count:
    if (enc == ONIG_ENCODING_UTF16_BE) {
      tag = "\000x\000\000";
    }
    else if (enc == ONIG_ENCODING_UTF16_LE) {
      tag = "x\000\000\000";
    }
    else {
      tag = "x";
    }
    tag_len = ulen(enc, tag);

    slot = 0;
    r = onig_get_callout_data_by_tag(reg, mp, (UChar* )tag, (UChar* )tag + tag_len,
                                     slot, 0, &val);
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
  OnigEncoding encs[3];
  OnigType arg_types[4];
  OnigValue opt_defaults[4];

  encs[0] = ONIG_ENCODING_UTF8;
  encs[1] = ONIG_ENCODING_UTF16_BE;
  encs[2] = ONIG_ENCODING_UTF16_LE;

  r = onig_initialize(encs, sizeof(encs)/sizeof(encs[0]));
  if (r != ONIG_NORMAL) {
    fprintf(stderr, "FAIL: onig_initialize(): %d\n", r);
    return -1;
  }

  test(encs[0], "abc(.(*COUNT[x]))*(*FAIL)", "abcdefg");
  test(encs[0], "abc(.(*COUNT[_any_]))*(.(*COUNT[x]))*d", "abcdefg");
  test(encs[0], "abc(.(*FAIL_COUNT[x]))*f", "abcdefg");

  test(encs[1], "\000a\000b\000c\000(\000.\000(\000*\000C\000O\000U\000N\000T\000[\000x\000]\000)\000)\000*\000(\000*\000F\000A\000I\000L\000)\000\000", "\000a\000b\000c\000d\000e\000f\000g\000\000");

  test(encs[2], "a\000b\000c\000(\000.\000(\000*\000C\000O\000U\000N\000T\000[\000x\000]\000)\000)\000*\000(\000*\000F\000A\000I\000L\000)\000\000\000", "a\000b\000c\000d\000e\000f\000g\000\000\000");

  onig_end();
  return 0;
}
