#ifndef __RACETYPES_H
#define __RACETYPES_H

//#include "../conf/zsconfig.h"
#include "constants.h"

/* Race type strings go in here */
typedef struct _racetype {
	char *race;
	char *newleader;
	char *update;
	char *race_halfway;
	char *norace_halfway;
} RACETYPE;

static const struct _racetype __racetype_rar = {
	rar_announce_race_type,
	rar_announce_newleader_type,
	rar_announce_update_type,
	rar_announce_norace_halfway_type,
	rar_announce_race_halfway_type
};

static const struct _racetype __racetype_other = {
	other_announce_race_type,
	other_announce_newleader_type,
	other_announce_update_type,
	other_announce_norace_halfway_type,
	other_announce_race_halfway_type,
};

static const struct _racetype __racetype_audio_cbr = {
	audio_announce_race_type,
	audio_announce_newleader_type,
	audio_announce_cbr_update_type,
	audio_announce_norace_halfway_type,
	audio_announce_race_halfway_type
};

static const struct _racetype __racetype_audio_vbr = {
	audio_announce_race_type,
	audio_announce_newleader_type,
	audio_announce_vbr_update_type,
	audio_announce_norace_halfway_type,
	audio_announce_race_halfway_type
};

static const struct _racetype __racetype_video = {
	video_announce_race_type,
	video_announce_newleader_type,
	video_announce_update_type,
	video_announce_norace_halfway_type,
	video_announce_race_halfway_type
};

static const struct _racetype __racetype_zip = {
	zip_announce_race_type,
	zip_announce_newleader_type,
	zip_announce_update_type,
	zip_announce_norace_halfway_type,
	zip_announce_race_halfway_type
};

void set_rtype_msg(struct _racetype *, const struct _racetype *);

#endif /* __RACETYPES_H */

