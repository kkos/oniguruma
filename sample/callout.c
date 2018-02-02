/*
 * callout.c
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "oniguruma.h"

static int
callout_func(OnigCalloutArgs* args, void* user_data)
{
  int len;
  UChar* content;

  len = args->content_end - args->content;
  content = strndup(args->content, len);

  fprintf(stdout, "CALLOUT: content: \"%s\", start: \"%s\", current: \"%s\"\n",
          content, args->start, args->current);
  fflush(stdout);
  free(content);

  return ONIG_CALLOUT_SUCCESS;
}

extern int main(int argc, char* argv[])
{
  int r;
  unsigned char *start, *range, *end;
  regex_t* reg;
  OnigErrorInfo einfo;
  OnigRegion *region;
  OnigMatchParams mparams;

  static UChar* pattern = (UChar* )"a+(?{foo bar baz...})$";
  static UChar* str     = (UChar* )"aaab";

  OnigEncoding use_encs[] = { ONIG_ENCODING_ASCII };
  onig_initialize(use_encs, sizeof(use_encs)/sizeof(use_encs[0]));

  r = onig_new(&reg, pattern, pattern + strlen((char* )pattern),
	ONIG_OPTION_DEFAULT, ONIG_ENCODING_ASCII, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(s, r, &einfo);
    fprintf(stderr, "COMPILE ERROR: %d: %s\n", r, s);
    return -1;
  }

  region = onig_region_new();

  onig_initialize_match_params(&mparams);
  mparams.callout = callout_func;

  end   = str + strlen((char* )str);
  start = str;
  range = end;
  r = onig_search_with_params(reg, str, end, start, range, region,
                              ONIG_OPTION_NONE, &mparams);
  if (r >= 0) {
    int i;

    fprintf(stderr, "match at %d\n", r);
    for (i = 0; i < region->num_regs; i++) {
      fprintf(stderr, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
  }
  else if (r == ONIG_MISMATCH) {
    fprintf(stderr, "search fail\n");
  }
  else { /* error */
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(s, r);
    fprintf(stderr, "SEARCH ERROR: %d: %s\n", r, s);
    return -1;
  }

  onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
  onig_free(reg);
  onig_end();
  return 0;
}
