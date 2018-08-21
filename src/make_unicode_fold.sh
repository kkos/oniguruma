#!/bin/sh

GPERF=/usr/bin/gperf

TMP0=gperf0.tmp
TMP1=gperf1.tmp
TMP2=gperf2.tmp
TMP3=gperf3.tmp

GPERF_OPT='-n -C -T -c -t -j1 -L ANSI-C '

./make_unicode_fold_data.py > unicode_fold_data.c

${GPERF} ${GPERF_OPT} -F,-1,0 -N unicode_unfold_key unicode_unfold_key.gperf > ${TMP0}
./gperf_unfold_key_conv.py < ${TMP0} > unicode_unfold_key.c

${GPERF} ${GPERF_OPT} -F,-1 -N unicode_fold1_key unicode_fold1_key.gperf > ${TMP1}
./gperf_fold_key_conv.py 1 < ${TMP1} > unicode_fold1_key.c

${GPERF} ${GPERF_OPT} -F,-1 -N unicode_fold2_key unicode_fold2_key.gperf > ${TMP2}
./gperf_fold_key_conv.py 2 < ${TMP2} > unicode_fold2_key.c

${GPERF} ${GPERF_OPT} -F,-1 -N unicode_fold3_key unicode_fold3_key.gperf > ${TMP3}
./gperf_fold_key_conv.py 3 < ${TMP3} > unicode_fold3_key.c

rm -f ${TMP0} ${TMP1} ${TMP2} ${TMP3}
rm -f unicode_unfold_key.gperf unicode_fold1_key.gperf unicode_fold2_key.gperf unicode_fold3_key.gperf

exit 0
