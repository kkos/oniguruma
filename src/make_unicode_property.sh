#!/bin/sh

NAME=unicode_property_data
TMP1=gperf1.tmp
TMP2=gperf2.tmp
GPERF=/usr/bin/gperf

GPERF_OPT='-T -C -c -t -j1 -L ANSI-C --ignore-case --pic -Q unicode_prop_name_pool'
POOL_CAST='s/\(int *\)\(size_t *\)&\(\(struct +unicode_prop_name_pool_t *\* *\) *0\)->unicode_prop_name_pool_str([^,]+)/pool_offset(\1)/g'

./make_unicode_property_data.py > ${NAME}.gperf
./make_unicode_property_data.py -posix > ${NAME}_posix.gperf

${GPERF} ${GPERF_OPT} -N unicode_lookup_property_name --output-file ${TMP1} ${NAME}.gperf
sed -e 's/^#line.*$//g' ${TMP1} | sed -r "${POOL_CAST}" > ${NAME}.c
${GPERF} ${GPERF_OPT} -N unicode_lookup_property_name --output-file ${TMP2} ${NAME}_posix.gperf
sed -e 's/^#line.*$//g' ${TMP2} | sed -r "${POOL_CAST}" > ${NAME}_posix.c

rm -f ${NAME}.gperf ${NAME}_posix.gperf ${TMP1} ${TMP2}

exit 0
