/*
 * callout.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "oniguruma.h"

static int
callout_body(OnigCalloutArgs* args, void* user_data)
{
  int r;
  int i;
  int n;
  int begin, end;
  int len;
  int used_num;
  int used_bytes;
  OnigCalloutIn in;
  OnigCalloutOf of;
  int name_id;
  UChar* content;
  const UChar* acontent;
  const UChar* acontent_end;
  const UChar* start;
  const UChar* current;
  regex_t* regex;

  in           = onig_get_callout_in_of_callout_args(args);
  of           = onig_get_callout_of_of_callout_args(args);
  name_id      = onig_get_name_id_of_callout_args(args);
  acontent     = onig_get_content_of_callout_args(args);
  acontent_end = onig_get_content_end_of_callout_args(args);
  start        = onig_get_start_of_callout_args(args);
  current      = onig_get_current_of_callout_args(args);
  regex        = onig_get_regex_of_callout_args(args);

  if (acontent != 0) {
    len = acontent_end - acontent;
    content = (UChar* )strndup((const char* )acontent, len);
  }
  else
    content = 0;

  if (name_id != ONIG_NO_NAME_ID) {
    UChar* name = onig_get_callout_name_from_name_id(name_id);
    fprintf(stdout, "name: %s\n", name);
  }
  fprintf(stdout,
          "%s %s: content: \"%s\", start: \"%s\", current: \"%s\"\n",
          of == ONIG_CALLOUT_OF_CODE ? "CODE" : "NAME",
          in == ONIG_CALLOUT_IN_PROGRESS ? "PROGRESS" : "RETRACTION",
          content, start, current);

  if (content != 0) free(content);

  (void )onig_get_used_stack_size_in_callout(args, &used_num, &used_bytes);
  fprintf(stdout, "stack: used_num: %d, used_bytes: %d\n", used_num, used_bytes);

  n = onig_number_of_captures(regex);
  for (i = 1; i <= n; i++) {
    r = onig_get_capture_range_in_callout(args, i, &begin, &end);
    if (r != ONIG_NORMAL) return r;

    fprintf(stdout, "capture %d: (%d-%d)\n", i, begin, end);
  }

  fflush(stdout);
  return ONIG_CALLOUT_SUCCESS;
}

static int
progress_callout_func(OnigCalloutArgs* args, void* user_data)
{
  return callout_body(args, user_data);
}

static int
retraction_callout_func(OnigCalloutArgs* args, void* user_data)
{
  return callout_body(args, user_data);
}

static int
foo(OnigCalloutArgs* args, void* user_data)
{
  return callout_body(args, user_data);
}


static int
test(char* in_pattern, char* in_str)
{
  int r;
  unsigned char *start, *range, *end;
  regex_t* reg;
  OnigErrorInfo einfo;
  OnigRegion *region;
  UChar* pattern;
  UChar* str;

  pattern = (UChar* )in_pattern;
  str = (UChar* )in_str;

  r = onig_new(&reg, pattern, pattern + strlen((char* )pattern),
	ONIG_OPTION_DEFAULT, ONIG_ENCODING_ASCII, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((UChar* )s, r, &einfo);
    fprintf(stderr, "COMPILE ERROR: %d: %s\n", r, s);
    return -1;
  }

  region = onig_region_new();

  end   = str + strlen((char* )str);
  start = str;
  range = end;
  r = onig_search(reg, str, end, start, range, region, ONIG_OPTION_NONE);
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
    onig_error_code_to_str((UChar* )s, r);
    fprintf(stderr, "SEARCH ERROR: %d: %s\n", r, s);
  }

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

  use_encs[0] = ONIG_ENCODING_UTF8;

  r = onig_initialize(use_encs, sizeof(use_encs)/sizeof(use_encs[0]));
  if (r != ONIG_NORMAL) return -r;

  r = onig_initialize_builtin_callouts();
  if (r != ONIG_NORMAL) return -r;

  name = (UChar* )"foo";
  id = onig_set_callout_of_name(name, name + strlen((char* )name),
                                ONIG_CALLOUT_IN_BOTH, foo, 0, 0, 0, 0);
  if (id < 0) {
    fprintf(stderr, "ERROR: fail to set callout of name: %s\n", name);
    //return -1;
  }

  (void)onig_set_callout_of_code(progress_callout_func);
  (void)onig_set_retraction_callout_of_code(retraction_callout_func);


  // callout of code
  test("a+(?{foo bar baz...}+)$", "aaab");
  test("(?{{!{}#$%&'()=-~^|[_]`@*:+;<>?/.\\,}})c", "abc");
  test("\\A(...)(?{{{booooooooooooo{{ooo}}ooooooooooz}}}-)", "aaab");
  test("\\A(?!a(?{prec-read-not}+)b)", "ac");
  test("(?<!a(?{look-behind-not}+)c)c", "abc");


  // callout of name
  test("\\A(*foo)abc", "abc");
  test("abc(*SUCCESS)", "abcabc");
  test("abc(?:(*FAIL)|$)", "abcabc");
  test("abc(?:(*ABORT)|$)", "abcabc");
  test("ab(*foo+{will be fail.})(*FAIL)", "abc");
  test("abc(d|(*ERROR{-999}))", "abc");

  onig_end();
  return 0;
}
