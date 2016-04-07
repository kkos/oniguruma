/* ANSI-C code produced by gperf version 3.0.4 */
/* Command-line: gperf -pt -T -L ANSI-C -N unicode_lookup_property_name --output-file unicode_prop.c unicode_prop.gperf  */
/* Computed positions: -k'1,3,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "unicode_prop.gperf"

#include <string.h>
#include "regenc.h"

#define TOTAL_KEYWORDS 115
#define MIN_WORD_LENGTH 1
#define MAX_WORD_LENGTH 19
#define MIN_HASH_VALUE 1
#define MAX_HASH_VALUE 251
/* maximum key range = 251, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
  static unsigned char asso_values[] =
    {
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252,  20,   5,   5, 100,   0,
      252,   0, 105,   0, 252,  85,  50,  20, 120,  65,
        0, 252,   5,  10,   0,  60, 252,  10,   0, 120,
      125, 252, 252, 252, 252, 252, 252,  60, 252,  15,
        2,  70,  47, 100,   0,  40,   7,  90,  65, 100,
       25,   0,  30,   5,  35,   7,  15,  75,   5,   5,
       15,   0, 252,   0, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252, 252, 252,
      252, 252, 252, 252, 252, 252, 252, 252
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]+2];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

#ifdef __GNUC__
__inline
#if defined __GNUC_STDC_INLINE__ || defined __GNUC_GNU_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
struct PropertyNameCtype *
unicode_lookup_property_name (register const char *str, register unsigned int len)
{
  static struct PropertyNameCtype wordlist[] =
    {
      {""},
#line 49 "unicode_prop.gperf"
      {"P",                     37},
#line 55 "unicode_prop.gperf"
      {"Po",                    43},
      {""},
#line 51 "unicode_prop.gperf"
      {"Pd",                    39},
      {""}, {""},
#line 33 "unicode_prop.gperf"
      {"Co",                    21},
#line 124 "unicode_prop.gperf"
      {"Tifinagh",             112},
#line 56 "unicode_prop.gperf"
      {"Ps",                    44},
      {""},
#line 29 "unicode_prop.gperf"
      {"C",                     17},
#line 61 "unicode_prop.gperf"
      {"So",                    49},
      {""},
#line 34 "unicode_prop.gperf"
      {"Cs",                    22},
      {""}, {""},
#line 50 "unicode_prop.gperf"
      {"Pc",                    38},
#line 93 "unicode_prop.gperf"
      {"Inherited",             81},
#line 72 "unicode_prop.gperf"
      {"Buhid",                 60},
#line 17 "unicode_prop.gperf"
      {"Graph",                  5},
#line 57 "unicode_prop.gperf"
      {"S",                     45},
#line 30 "unicode_prop.gperf"
      {"Cc",                    18},
#line 27 "unicode_prop.gperf"
      {"Any",                   15},
      {""},
#line 26 "unicode_prop.gperf"
      {"ASCII",                 14},
#line 84 "unicode_prop.gperf"
      {"Gothic",                72},
#line 58 "unicode_prop.gperf"
      {"Sc",                    46},
      {""}, {""},
#line 81 "unicode_prop.gperf"
      {"Ethiopic",              69},
#line 24 "unicode_prop.gperf"
      {"Word",                  12},
#line 32 "unicode_prop.gperf"
      {"Cn",                    20},
      {""},
#line 123 "unicode_prop.gperf"
      {"Tibetan",              111},
      {""},
#line 75 "unicode_prop.gperf"
      {"Common",                63},
#line 42 "unicode_prop.gperf"
      {"Mc",                    30},
#line 82 "unicode_prop.gperf"
      {"Georgian",              70},
      {""},
#line 83 "unicode_prop.gperf"
      {"Glagolitic",            71},
#line 41 "unicode_prop.gperf"
      {"M",                     29},
#line 54 "unicode_prop.gperf"
      {"Pi",                    42},
#line 78 "unicode_prop.gperf"
      {"Cyrillic",              66},
      {""}, {""},
#line 115 "unicode_prop.gperf"
      {"Syriac",               103},
#line 44 "unicode_prop.gperf"
      {"Mn",                    32},
#line 69 "unicode_prop.gperf"
      {"Bopomofo",              57},
#line 53 "unicode_prop.gperf"
      {"Pf",                    41},
#line 20 "unicode_prop.gperf"
      {"Punct",                  8},
      {""},
#line 38 "unicode_prop.gperf"
      {"Lo",                    26},
#line 67 "unicode_prop.gperf"
      {"Armenian",              55},
#line 31 "unicode_prop.gperf"
      {"Cf",                    19},
#line 111 "unicode_prop.gperf"
      {"Runic",                 99},
#line 66 "unicode_prop.gperf"
      {"Arabic",                54},
#line 112 "unicode_prop.gperf"
      {"Shavian",              100},
#line 98 "unicode_prop.gperf"
      {"Lao",                   86},
#line 122 "unicode_prop.gperf"
      {"Thai",                 110},
      {""},
#line 76 "unicode_prop.gperf"
      {"Coptic",                64},
#line 77 "unicode_prop.gperf"
      {"Cypriot",               65},
#line 87 "unicode_prop.gperf"
      {"Gurmukhi",              75},
      {""}, {""}, {""},
#line 39 "unicode_prop.gperf"
      {"Lt",                    27},
      {""}, {""},
#line 119 "unicode_prop.gperf"
      {"Tamil",                107},
      {""},
#line 52 "unicode_prop.gperf"
      {"Pe",                    40},
      {""}, {""}, {""}, {""},
#line 104 "unicode_prop.gperf"
      {"Myanmar",               92},
      {""}, {""},
#line 15 "unicode_prop.gperf"
      {"Cntrl",                  3},
#line 121 "unicode_prop.gperf"
      {"Thaana",               109},
#line 68 "unicode_prop.gperf"
      {"Bengali",               56},
      {""},
#line 103 "unicode_prop.gperf"
      {"Mongolian",             91},
#line 99 "unicode_prop.gperf"
      {"Latin",                 87},
      {""},
#line 114 "unicode_prop.gperf"
      {"Syloti_Nagri",         102},
      {""}, {""},
#line 18 "unicode_prop.gperf"
      {"Lower",                  6},
      {""},
#line 43 "unicode_prop.gperf"
      {"Me",                    31},
#line 101 "unicode_prop.gperf"
      {"Linear_B",              89},
      {""}, {""}, {""},
#line 70 "unicode_prop.gperf"
      {"Braille",               58},
#line 125 "unicode_prop.gperf"
      {"Ugaritic",             113},
      {""},
#line 21 "unicode_prop.gperf"
      {"Space",                  9},
#line 35 "unicode_prop.gperf"
      {"L",                     23},
#line 59 "unicode_prop.gperf"
      {"Sk",                    47},
      {""}, {""},
#line 28 "unicode_prop.gperf"
      {"Assigned",              16},
#line 120 "unicode_prop.gperf"
      {"Telugu",               108},
#line 113 "unicode_prop.gperf"
      {"Sinhala",              101},
#line 117 "unicode_prop.gperf"
      {"Tagbanwa",             105},
      {""},
#line 19 "unicode_prop.gperf"
      {"Print",                  7},
#line 23 "unicode_prop.gperf"
      {"XDigit",                11},
#line 60 "unicode_prop.gperf"
      {"Sm",                    48},
#line 86 "unicode_prop.gperf"
      {"Gujarati",              74},
      {""},
#line 14 "unicode_prop.gperf"
      {"Blank",                  2},
      {""},
#line 36 "unicode_prop.gperf"
      {"Ll",                    24},
#line 91 "unicode_prop.gperf"
      {"Hebrew",                79},
#line 73 "unicode_prop.gperf"
      {"Canadian_Aboriginal",   61},
#line 13 "unicode_prop.gperf"
      {"Alpha",                  1},
      {""},
#line 48 "unicode_prop.gperf"
      {"No",                    36},
#line 71 "unicode_prop.gperf"
      {"Buginese",              59},
#line 46 "unicode_prop.gperf"
      {"Nd",                    34},
#line 97 "unicode_prop.gperf"
      {"Khmer",                 85},
      {""},
#line 40 "unicode_prop.gperf"
      {"Lu",                    28},
      {""}, {""},
#line 100 "unicode_prop.gperf"
      {"Limbu",                 88},
      {""},
#line 110 "unicode_prop.gperf"
      {"Osmanya",               98},
      {""},
#line 65 "unicode_prop.gperf"
      {"Zs",                    53},
#line 22 "unicode_prop.gperf"
      {"Upper",                 10},
      {""},
#line 107 "unicode_prop.gperf"
      {"Old_Italic",            95},
      {""}, {""}, {""}, {""},
#line 90 "unicode_prop.gperf"
      {"Hanunoo",               78},
      {""}, {""}, {""}, {""},
#line 116 "unicode_prop.gperf"
      {"Tagalog",              104},
#line 108 "unicode_prop.gperf"
      {"Old_Persian",           96},
      {""},
#line 96 "unicode_prop.gperf"
      {"Kharoshthi",            84},
      {""},
#line 37 "unicode_prop.gperf"
      {"Lm",                    25},
      {""},
#line 102 "unicode_prop.gperf"
      {"Malayalam",             90},
#line 25 "unicode_prop.gperf"
      {"Alnum",                 13},
      {""},
#line 64 "unicode_prop.gperf"
      {"Zp",                    52},
#line 95 "unicode_prop.gperf"
      {"Katakana",              83},
      {""},
#line 16 "unicode_prop.gperf"
      {"Digit",                  4},
      {""},
#line 126 "unicode_prop.gperf"
      {"Yi",                   114},
#line 88 "unicode_prop.gperf"
      {"Han",                   76},
      {""},
#line 80 "unicode_prop.gperf"
      {"Devanagari",            68},
#line 118 "unicode_prop.gperf"
      {"Tai_Le",               106},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""},
#line 106 "unicode_prop.gperf"
      {"Ogham",                 94},
      {""}, {""}, {""}, {""},
#line 94 "unicode_prop.gperf"
      {"Kannada",               82},
#line 74 "unicode_prop.gperf"
      {"Cherokee",              62},
      {""}, {""}, {""},
#line 47 "unicode_prop.gperf"
      {"Nl",                    35},
#line 92 "unicode_prop.gperf"
      {"Hiragana",              80},
      {""}, {""}, {""},
#line 63 "unicode_prop.gperf"
      {"Zl",                    51},
      {""}, {""},
#line 85 "unicode_prop.gperf"
      {"Greek",                 73},
      {""},
#line 79 "unicode_prop.gperf"
      {"Deseret",               67},
      {""}, {""}, {""},
#line 105 "unicode_prop.gperf"
      {"New_Tai_Lue",           93},
      {""}, {""}, {""}, {""},
#line 89 "unicode_prop.gperf"
      {"Hangul",                77},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""},
#line 109 "unicode_prop.gperf"
      {"Oriya",                 97},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
      {""}, {""},
#line 45 "unicode_prop.gperf"
      {"N",                     33},
      {""}, {""}, {""}, {""}, {""},
#line 12 "unicode_prop.gperf"
      {"NEWLINE",                0},
      {""}, {""}, {""},
#line 62 "unicode_prop.gperf"
      {"Z",                     50}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
