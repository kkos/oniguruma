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
  int name_id;
  UChar* contents;
  const UChar* acontents;
  const UChar* acontents_end;
  const UChar* start;
  const UChar* current;
  regex_t* regex;

  in            = onig_get_callout_in_by_callout_args(args);
  name_id       = onig_get_name_id_by_callout_args(args);
  start         = onig_get_start_by_callout_args(args);
  current       = onig_get_current_by_callout_args(args);
  regex         = onig_get_regex_by_callout_args(args);

  contents = 0;
  acontents = onig_get_contents_by_callout_args(args);
  if (acontents != 0) {
    OnigEncoding enc = onig_get_encoding(regex);
    acontents_end = onig_get_contents_end_by_callout_args(args);
    contents = onigenc_strdup(enc, acontents, acontents_end);
  }

  if (name_id != ONIG_NON_NAME_ID) {
    UChar* name = onig_get_callout_name_by_name_id(name_id);
    fprintf(stdout, "name: %s\n", name);
  }
  fprintf(stdout,
          "%s %s: contents: \"%s\", start: \"%s\", current: \"%s\"\n",
          contents != 0 ? "CONTENTS" : "NAME",
          in == ONIG_CALLOUT_IN_PROGRESS ? "PROGRESS" : "RETRACTION",
          contents, start, current);

  if (contents != 0) free(contents);

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
bar(OnigCalloutArgs* args, void* user_data)
{
  int r;
  int i;
  int n;
  OnigType type;
  OnigValue val;

  fprintf(stdout, "bar called.\n");

  n = onig_get_args_num_by_callout_args(args);
  if (n < 0) {
    fprintf(stderr, "FAIL: onig_get_args_num_by_callout_args(): %d\n", n);
    return n;
  }

  for (i = 0; i < n; i++) {
    r = onig_get_arg_by_callout_args(args, i, &type, &val);
    if (r != 0) {
      fprintf(stderr, "FAIL: onig_get_arg_by_callout_args(): %d\n", r);
      return r;
    }

    fprintf(stdout, "arg[%d]: ", i);
    switch (type) {
    case ONIG_TYPE_INT:
      fprintf(stdout, "%d\n", val.i);
      break;
    case ONIG_TYPE_LONG:
      fprintf(stdout, "%ld\n", val.l);
      break;
    case ONIG_TYPE_CHAR:
      fprintf(stdout, "0x%06x\n", val.c);
      break;
    case ONIG_TYPE_STRING:
      fprintf(stdout, "'%s'\n", val.s.start);
      break;
    default:
      /* Never come here. But escape warning. */
      break;
    };
  }

  return ONIG_CALLOUT_SUCCESS;
}

static int
test(OnigEncoding enc, char* in_pattern, char* in_str)
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
	ONIG_OPTION_DEFAULT, enc, ONIG_SYNTAX_DEFAULT, &einfo);
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
  OnigType arg_types[4];
  OnigValue opt_defaults[4];
  OnigEncoding enc;

  enc = ONIG_ENCODING_UTF8;
  use_encs[0] = enc;

  r = onig_initialize(use_encs, sizeof(use_encs)/sizeof(use_encs[0]));
  if (r != ONIG_NORMAL) return -1;

  /* monitor on */
  r = onig_setup_builtin_monitors_by_ascii_encoded_name();
  if (r != ONIG_NORMAL) return -1;

  name = (UChar* )"foo";
  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,
                                name, name + strlen((char* )name),
                                ONIG_CALLOUT_IN_BOTH, foo, 0, 0, 0, 0, 0);
  if (id < 0) {
    fprintf(stderr, "ERROR: fail to set callout of name: %s\n", name);
    //return -1;
  }

  name = (UChar* )"bar";
  arg_types[0] = ONIG_TYPE_INT;
  arg_types[1] = ONIG_TYPE_STRING;
  arg_types[2] = ONIG_TYPE_CHAR;
  opt_defaults[0].s.start = (UChar* )"I am a option argument's default value.";
  opt_defaults[0].s.end   = opt_defaults[0].s.start +
                                strlen((char* )opt_defaults[0].s.start);
  opt_defaults[1].c = 0x4422;

  id = onig_set_callout_of_name(enc, ONIG_CALLOUT_TYPE_SINGLE,
                                name, name + strlen((char* )name),
                                ONIG_CALLOUT_IN_PROGRESS, bar, 0,
                                3, arg_types, 2, opt_defaults);
  if (id < 0) {
    fprintf(stderr, "ERROR: fail to set callout of name: %s\n", name);
    //return -1;
  }

  (void)onig_set_callout_of_contents(progress_callout_func);
  (void)onig_set_retraction_callout_of_contents(retraction_callout_func);

  // callout of contents
  test(enc, "a+(?{foo bar baz...}+)$", "aaab");
  test(enc, "(?{{!{}#$%&'()=-~^|[_]`@*:+;<>?/.\\,}}[symbols])c", "abc");
  test(enc, "\\A(...)(?{{{booooooooooooo{{ooo}}ooooooooooz}}}-)", "aaab");
  test(enc, "\\A(?!a(?{prec-read-not}[xxx]+)b)", "ac");
  test(enc, "(?<!a(?{look-behind-not}+)c)c", "abc");


  // callout of name
  test(enc, "\\A(*foo)abc", "abc");
  test(enc, "abc(*SUCCESS)", "abcabc");
  test(enc, "abc(?:(*FAIL)|$)", "abcabc");
  test(enc, "abc(?:(*ABORT)|$)", "abcabc");
  test(enc, "ab(*foo{})(*FAIL)", "abc");
  test(enc, "abc(d|(*ERROR{-999}))", "abc");
  test(enc, "ab(*bar{372,I am a bar's argument,あ})c(*FAIL)", "abc");
  test(enc, "ab(*bar{1234567890})", "abc");
  test(enc, "(?:a(*MAX{2})|b)*", "abbabbabbabb");
  test(enc, "(?:(*MAX{2})a|b)*", "abbabbabbabb");
  test(enc, "(?:(*MAX{1})a|b)*", "bbbbbabbbbbabbbbb");
  test(enc, "(?:(*MAX{3})a|(*MAX{4})b)*", "bbbaabbab");

  /* monitor test */
  //test(enc, "(?:(*MON)(*MAX{3})a(*MON{foo})|(*MAX{4})b)*", "bbbaabbab");
  test(enc, "(?:(*MON{+})(*FAIL)|.{,3}(*MON[X])k)", "abcdefghijk");

  onig_end();
  return 0;
}
