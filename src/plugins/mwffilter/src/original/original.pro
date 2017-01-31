BASEDIR = ../../../../..
CLEBS += fftw3 sndfile jack

include ($$BASEDIR/clebs.pri)

TARGET = original

SOURCES +=                                              \
	tp.c                                            \
	tp_noproc.c                                     \
	tp_srtbatch.c                                   \
	tp_test.c                                       \
	tpfile.c                                        \
	tpfilter.c                                      \
	tpfilter_freq.c                                 \
	tpjack.c                                        \
	tpresample.c                                    \
	tpsrt_auxialry.c                                \

HEADERS +=                                              \
	sista_defs.h                                    \
	tp.h                                            \
	tp_jackcb.h                                     \
	tp_noproc.h                                     \
	tp_srtbatch.h                                   \
	tp_test.h                                       \
	tpfile.h                                        \
	tpfilter.h                                      \
	tpjack.h                                        \
	tpresample.h                                    \
	tpsrt_auxiliary.h                               \
