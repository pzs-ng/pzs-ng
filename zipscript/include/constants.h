#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

#define VERSION		"0.7.3 BETA3 (PRE)"

#define F_IGNORED		254
#define F_BAD			255
#define F_NFO			253
#define F_DELETED		0
#define F_CHECKED		1
#define F_NOTCHECKED	2

#define TRUE			1
#define FALSE			0

#define DISABLED		NULL

#define FILE_MAX		256

#ifndef O_SYNC
# define O_SYNC O_FSYNC
#endif

#define deny_double_msg		"{%U} {%G} {%r} {%n}"
#define audio_cbr_warn_msg	"{%U} {%G} {%X}"
#define audio_year_warn_msg	"{%U} {%G} {%Y}"
#define audio_genre_warn_msg	"{%U} {%G} {%w}"

#define pre_stats		"dummy_pre_stats"
#define between_stats		"dummy_between_stats"
#define post_stats		"dummy_post_stats"
#define winner			"dummy_winner"
#define loser			"dummy_loser"

#define user_top		"{%n} {%-9u} {%-9g} {%5.1m} {%2f} {%5.1p} {%5.0s} {%2W}"
#define user_info		"{%u} {%g} {%.1m} {%f} {%.1p} {%.0s}"
#define group_top		"{%n} {%-9g} {%.1m} {%f} {%.1p} {%5.0s}"
#define group_info		"{%g} {%.1m} {%-2f} {%.1p} {%.0s}"

/* Special reformat cookies */
#define fastestfile		"{%u %g} {%.3F}" /* Put in %l cookie */
#define slowestfile		"{%u %g} {%.3S}" /* Put in %L cookie */
#define racersmsg		"{%u(%g)}" /* Put in %R cookie */
#define audio_vbr		"{%X} {%z} {%Q} {VBR} {%I}" /* Put in %j cookie */
#define audio_cbr		"{%X} {%z} {%Q} {CBR}"    /* Put in %j cookie */

/* Audio files - currently used only for mp3 (mpeg2 layer3), but mpeg1 & mpeg2.5 + layer1 & 2 can be added if needed */
#define audio_race		"{%U} {%G} %R {%r} {%.0S}"
#define audio_sfv		"{%r} {%f}"
#define audio_update		"{%U} {%G} {%.0S} {%f} {%w} {%Y} %j"
#define audio_halfway		"{%r} {%C0} {%c0}"
#define audio_newleader		"{%C0} {%r}"
#define audio_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L"
#define audio_norace_halfway	"{%r} {%C0} {%c0}"
#define audio_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L"

/* Video files - supports mpeg1/mpeg2 and various avi formats */
#define video_race		"{%U} {%G} %R {%r} {%.0S}"
#define video_sfv		"{%r} {%f}"
#define video_update		"{%U} {%G} {%.0S} {%.1e}"
#define video_halfway		"{%r} {%C0} {%c0}"
#define video_newleader		"{%C0} {%r}"
#define video_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L"
#define video_norace_halfway	"{%r} {%C0} {%c0}"
#define video_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L"

/* RAR - allows usage of compression mode cookie (%Z) for rarred releases */
#define rar_race		"{%U} {%G} %R {%r} {%.0S}"
#define rar_sfv			"{%r} {%f}"
#define rar_update		"{%U} {%G} {%.0S} {%.1e}"
#define rar_halfway		"{%r} {%C0} {%c0}"
#define rar_newleader		"{%C0} {%r}"
#define rar_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L {%Z}"
#define rar_norace_halfway	"{%r} {%C0} {%c0}"
#define rar_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L {%Z}"

/* Rest of the stuff inside sfv */
#define other_race		"{%U} {%G} %R {%r} {%.0S}"
#define other_sfv		"{%r} {%f}"
#define other_update		"{%U} {%G} {%.0S} {%.1e}"
#define other_halfway		"{%r} {%C0} {%c0}"
#define other_newleader		"{%C0} {%r}"
#define other_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L"
#define other_norace_halfway	"{%r} {%C0} {%c0}"
#define other_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L"

/* Zipped files */
#define zip_race		"{%U} {%G} %R {%r} {%.0S}"
#define zip_update		"{%U} {%G} {%.0S} {%.1e} {%f}"
#define zip_halfway		"{%r} {%C0} {%c0}"
#define zip_newleader		"{%C0} {%r}"
#define zip_complete		"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L {%T}"
#define zip_norace_halfway	"{%r} {%C0} {%c0}"
#define zip_norace_complete	"{%r} {%.0m} {%f} {%.0A} {%d} {%u} {%g} {%U} {%G} %l %L {%T}"


/* Output of racestats binary */
#define stats_line		"{%r} {%F} {%f} {%u} {%g} {%C0}"

/* Output of postdel binary */
#define incompletemsg		"{%U} {%G} {%r}"

#endif

