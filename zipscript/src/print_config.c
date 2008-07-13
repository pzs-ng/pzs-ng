#include <stdio.h>

#include "constants.h"
#include "print_config.h"

#include "../conf/zsconfig.h"
#include "zsconfig.defaults.h"

#include "errors.h"
/* Hardcore preprocessor hackery, thanks to runehol/#scene.no. */
#define expand(x) x
#define stringize(x) expand(#x)
#define stringify(x) stringize(x)
void print_nondefault_config(void)
{
#ifndef BAD_CRC_is_defaulted
printf("#define BAD_CRC                                 %s\n", stringify(BAD_CRC));
#endif
#ifndef BAD_ZIP_is_defaulted
printf("#define BAD_ZIP                                 %s\n", stringify(BAD_ZIP));
#endif
#ifndef BANNED_BITRATE_is_defaulted
printf("#define BANNED_BITRATE                          %s\n", stringify(BANNED_BITRATE));
#endif
#ifndef BANNED_FILE_is_defaulted
printf("#define BANNED_FILE                             %s\n", stringify(BANNED_FILE));
#endif
#ifndef BANNED_GENRE_is_defaulted
printf("#define BANNED_GENRE                            %s\n", stringify(BANNED_GENRE));
#endif
#ifndef BANNED_YEAR_is_defaulted
printf("#define BANNED_YEAR                             %s\n", stringify(BANNED_YEAR));
#endif
#ifndef DOUBLE_SFV_is_defaulted
printf("#define DOUBLE_SFV                              %s\n", stringify(DOUBLE_SFV));
#endif
#ifndef DUPE_NFO_is_defaulted
printf("#define DUPE_NFO                                %s\n", stringify(DUPE_NFO));
#endif
#ifndef EMPTY_FILE_is_defaulted
printf("#define EMPTY_FILE                              %s\n", stringify(EMPTY_FILE));
#endif
#ifndef EMPTY_SFV_is_defaulted
printf("#define EMPTY_SFV                               %s\n", stringify(EMPTY_SFV));
#endif
#ifndef GROUPFILE_is_defaulted
printf("#define GROUPFILE                               %s\n", stringify(GROUPFILE));
#endif
#ifndef NOT_IN_SFV_is_defaulted
printf("#define NOT_IN_SFV                              %s\n", stringify(NOT_IN_SFV));
#endif
#ifndef PASSWDFILE_is_defaulted
printf("#define PASSWDFILE                              %s\n", stringify(PASSWDFILE));
#endif
#ifndef SFV_FIRST_is_defaulted
printf("#define SFV_FIRST                               %s\n", stringify(SFV_FIRST));
#endif
#ifndef SPEEDTEST_is_defaulted
printf("#define SPEEDTEST                               %s\n", stringify(SPEEDTEST));
#endif
#ifndef UNKNOWN_FILE_is_defaulted
printf("#define UNKNOWN_FILE                            %s\n", stringify(UNKNOWN_FILE));
#endif
#ifndef ZIP_NFO_is_defaulted
printf("#define ZIP_NFO                                 %s\n", stringify(ZIP_NFO));
#endif
#ifndef accept_script_is_defaulted
printf("#define accept_script                           %s\n", stringify(accept_script));
#endif
#ifndef affil_script_is_defaulted
printf("#define affil_script                            %s\n", stringify(affil_script));
#endif
#ifndef allow_dir_chown_in_ng_chown_is_defaulted
printf("#define allow_dir_chown_in_ng_chown             %s\n", (allow_dir_chown_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef allow_error2_in_unzip_is_defaulted
printf("#define allow_error2_in_unzip                   %s\n", (allow_error2_in_unzip == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef allow_file_resume_is_defaulted
printf("#define allow_file_resume                       %s\n", (allow_file_resume == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef allow_files_chown_in_ng_chown_is_defaulted
printf("#define allow_files_chown_in_ng_chown           %s\n", (allow_files_chown_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef allow_files_not_in_sfv_is_defaulted
printf("#define allow_files_not_in_sfv                  %s\n", (allow_files_not_in_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef allow_gid_change_in_ng_chown_is_defaulted
printf("#define allow_gid_change_in_ng_chown            %s\n", (allow_gid_change_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef allow_slash_in_sfv_is_defaulted
printf("#define allow_slash_in_sfv                      %s\n", (allow_slash_in_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef allow_uid_change_in_ng_chown_is_defaulted
printf("#define allow_uid_change_in_ng_chown            %s\n", (allow_uid_change_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef allowed_constant_bitrates_is_defaulted
printf("#define allowed_constant_bitrates               %s\n", stringify(allowed_constant_bitrates));
#endif
#ifndef allowed_genres_is_defaulted
printf("#define allowed_genres                          %s\n", stringify(allowed_genres));
#endif
#ifndef allowed_types_is_defaulted
printf("#define allowed_types                           %s\n", stringify(allowed_types));
#endif
#ifndef allowed_types_exemption_dirs_is_defaulted
printf("#define allowed_types_exemption_dirs            %s\n", stringify(allowed_types_exemption_dirs));
#endif
#ifndef allowed_years_is_defaulted
printf("#define allowed_years                           %s\n", stringify(allowed_years));
#endif
#ifndef always_scan_audio_syms_is_defaulted
printf("#define always_scan_audio_syms                  %s\n", (always_scan_audio_syms == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef announce_norace_is_defaulted
printf("#define announce_norace                         %s\n", (announce_norace == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_allowed_genre_check_is_defaulted
printf("#define audio_allowed_genre_check               %s\n", (audio_allowed_genre_check == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_artist_noid3_is_defaulted
printf("#define audio_artist_noid3                      %s\n", (audio_artist_noid3 == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_artist_nosub_is_defaulted
printf("#define audio_artist_nosub                      %s\n", (audio_artist_nosub == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_artist_path_is_defaulted
printf("#define audio_artist_path                       %s\n", stringify(audio_artist_path));
#endif
#ifndef audio_artist_sort_is_defaulted
printf("#define audio_artist_sort                       %s\n", (audio_artist_sort == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_banned_genre_check_is_defaulted
printf("#define audio_banned_genre_check                %s\n", (audio_banned_genre_check == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_cbr_check_is_defaulted
printf("#define audio_cbr_check                         %s\n", (audio_cbr_check == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_cbr_warn_is_defaulted
printf("#define audio_cbr_warn                          %s\n", (audio_cbr_warn == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_completebar_is_defaulted
printf("#define audio_completebar                       %s\n", stringify(audio_completebar));
#endif
#ifndef audio_genre_path_is_defaulted
printf("#define audio_genre_path                        %s\n", stringify(audio_genre_path));
#endif
#ifndef audio_genre_sort_is_defaulted
printf("#define audio_genre_sort                        %s\n", (audio_genre_sort == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_genre_warn_is_defaulted
printf("#define audio_genre_warn                        %s\n", (audio_genre_warn == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_group_path_is_defaulted
printf("#define audio_group_path                        %s\n", stringify(audio_group_path));
#endif
#ifndef audio_group_sort_is_defaulted
printf("#define audio_group_sort                        %s\n", (audio_group_sort == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_ignored_languages_is_defaulted
printf("#define audio_ignored_languages                 %s\n", stringify(audio_ignored_languages));
#endif
#ifndef audio_language_path_is_defaulted
printf("#define audio_language_path                     %s\n", stringify(audio_language_path));
#endif
#ifndef audio_language_sort_is_defaulted
printf("#define audio_language_sort                     %s\n", (audio_language_sort == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_nocheck_dirs_is_defaulted
printf("#define audio_nocheck_dirs                      %s\n", stringify(audio_nocheck_dirs));
#endif
#ifndef audio_nosort_dirs_is_defaulted
printf("#define audio_nosort_dirs                       %s\n", stringify(audio_nosort_dirs));
#endif
#ifndef audio_year_check_is_defaulted
printf("#define audio_year_check                        %s\n", (audio_year_check == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_year_path_is_defaulted
printf("#define audio_year_path                         %s\n", stringify(audio_year_path));
#endif
#ifndef audio_year_sort_is_defaulted
printf("#define audio_year_sort                         %s\n", (audio_year_sort == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef audio_year_warn_is_defaulted
printf("#define audio_year_warn                         %s\n", (audio_year_warn == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef banned_filelist_is_defaulted
printf("#define banned_filelist                         %s\n", stringify(banned_filelist));
#endif
#ifndef banned_genres_is_defaulted
printf("#define banned_genres                           %s\n", stringify(banned_genres));
#endif
#ifndef benchmark_mode_is_defaulted
printf("#define benchmark_mode                          %s\n", (benchmark_mode == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef change_spaces_to_underscore_in_ng_chown_is_defaulted
printf("#define change_spaces_to_underscore_in_ng_chown %s\n", (change_spaces_to_underscore_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef charbar_filled_is_defaulted
printf("#define charbar_filled                          %s\n", stringify(charbar_filled));
#endif
#ifndef charbar_missing_is_defaulted
printf("#define charbar_missing                         %s\n", stringify(charbar_missing));
#endif
#ifndef check_for_banned_files_is_defaulted
printf("#define check_for_banned_files                  %s\n", (check_for_banned_files == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef check_for_missing_nfo_dirs_is_defaulted
printf("#define check_for_missing_nfo_dirs              %s\n", stringify(check_for_missing_nfo_dirs));
#endif
#ifndef check_for_missing_sample_dirs_is_defaulted
printf("#define check_for_missing_sample_dirs           %s\n", stringify(check_for_missing_sample_dirs));
#endif
#ifndef chmod_completebar_is_defaulted
printf("#define chmod_completebar                       %s\n", (chmod_completebar == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef cleanupdirs_is_defaulted
printf("#define cleanupdirs                             %s\n", stringify(cleanupdirs));
#endif
#ifndef cleanupdirs_dated_is_defaulted
printf("#define cleanupdirs_dated                       %s\n", stringify(cleanupdirs_dated));
#endif
#ifndef combine_path_is_defaulted
printf("#define combine_path                            %s\n", (combine_path == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef complete_script_is_defaulted
printf("#define complete_script                         %s\n", stringify(complete_script));
#endif
#ifndef create_incomplete_links_in_group_dirs_is_defaulted
printf("#define create_incomplete_links_in_group_dirs   %s\n", (create_incomplete_links_in_group_dirs == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef create_m3u_is_defaulted
printf("#define create_m3u                              %s\n", (create_m3u == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef create_missing_files_is_defaulted
printf("#define create_missing_files                    %s\n", (create_missing_files == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef create_missing_sample_link_is_defaulted
printf("#define create_missing_sample_link              %s\n", (create_missing_sample_link == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef create_missing_sfv_is_defaulted
printf("#define create_missing_sfv                      %s\n", (create_missing_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef create_missing_sfv_link_is_defaulted
printf("#define create_missing_sfv_link                 %s\n", (create_missing_sfv_link == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef custom_group_dirs_complete_message_is_defaulted
printf("#define custom_group_dirs_complete_message      %s\n", stringify(custom_group_dirs_complete_message));
#endif
#ifndef days_back_cleanup_is_defaulted
printf("#define days_back_cleanup                       %s\n", stringify(days_back_cleanup));
#endif
#ifndef debug_altlog_is_defaulted
printf("#define debug_altlog                            %s\n", (debug_altlog == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef debug_announce_is_defaulted
printf("#define debug_announce                          %s\n", (debug_announce == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef debug_mode_is_defaulted
printf("#define debug_mode                              %s\n", (debug_mode == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef del_banned_release_is_defaulted
printf("#define del_banned_release                      %s\n", (del_banned_release == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef del_completebar_is_defaulted
printf("#define del_completebar                         %s\n", stringify(del_completebar));
#endif
#ifndef del_progressmeter_is_defaulted
printf("#define del_progressmeter                       %s\n", stringify(del_progressmeter));
#endif
#ifndef del_progressmeter_mp3_is_defaulted
printf("#define del_progressmeter_mp3                   %s\n", stringify(del_progressmeter_mp3));
#endif
#ifndef delete_old_link_is_defaulted
printf("#define delete_old_link                         %s\n", (delete_old_link == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef deny_double_nfo_is_defaulted
printf("#define deny_double_nfo                         %s\n", (deny_double_nfo == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef deny_double_sfv_is_defaulted
printf("#define deny_double_sfv                         %s\n", (deny_double_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef deny_nfo_upload_in_zip_is_defaulted
printf("#define deny_nfo_upload_in_zip                  %s\n", (deny_nfo_upload_in_zip == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef deny_resume_sfv_is_defaulted
printf("#define deny_resume_sfv                         %s\n", (deny_resume_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_audio_complete_is_defaulted
printf("#define disable_audio_complete                  %s\n", (disable_audio_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_audio_halfway_is_defaulted
printf("#define disable_audio_halfway                   %s\n", (disable_audio_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_audio_newleader_is_defaulted
printf("#define disable_audio_newleader                 %s\n", (disable_audio_newleader == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_audio_norace_complete_is_defaulted
printf("#define disable_audio_norace_complete           %s\n", (disable_audio_norace_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_audio_norace_halfway_is_defaulted
printf("#define disable_audio_norace_halfway            %s\n", (disable_audio_norace_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_audio_race_is_defaulted
printf("#define disable_audio_race                      %s\n", (disable_audio_race == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_audio_sfv_is_defaulted
printf("#define disable_audio_sfv                       %s\n", (disable_audio_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_audio_update_is_defaulted
printf("#define disable_audio_update                    %s\n", (disable_audio_update == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_other_complete_is_defaulted
printf("#define disable_other_complete                  %s\n", (disable_other_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_other_halfway_is_defaulted
printf("#define disable_other_halfway                   %s\n", (disable_other_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_other_newleader_is_defaulted
printf("#define disable_other_newleader                 %s\n", (disable_other_newleader == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_other_norace_complete_is_defaulted
printf("#define disable_other_norace_complete           %s\n", (disable_other_norace_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_other_norace_halfway_is_defaulted
printf("#define disable_other_norace_halfway            %s\n", (disable_other_norace_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_other_race_is_defaulted
printf("#define disable_other_race                      %s\n", (disable_other_race == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_other_sfv_is_defaulted
printf("#define disable_other_sfv                       %s\n", (disable_other_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_other_update_is_defaulted
printf("#define disable_other_update                    %s\n", (disable_other_update == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_rar_complete_is_defaulted
printf("#define disable_rar_complete                    %s\n", (disable_rar_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_rar_halfway_is_defaulted
printf("#define disable_rar_halfway                     %s\n", (disable_rar_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_rar_newleader_is_defaulted
printf("#define disable_rar_newleader                   %s\n", (disable_rar_newleader == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_rar_norace_complete_is_defaulted
printf("#define disable_rar_norace_complete             %s\n", (disable_rar_norace_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_rar_norace_halfway_is_defaulted
printf("#define disable_rar_norace_halfway              %s\n", (disable_rar_norace_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_rar_race_is_defaulted
printf("#define disable_rar_race                        %s\n", (disable_rar_race == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_rar_sfv_is_defaulted
printf("#define disable_rar_sfv                         %s\n", (disable_rar_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_rar_update_is_defaulted
printf("#define disable_rar_update                      %s\n", (disable_rar_update == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_video_complete_is_defaulted
printf("#define disable_video_complete                  %s\n", (disable_video_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_video_halfway_is_defaulted
printf("#define disable_video_halfway                   %s\n", (disable_video_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_video_newleader_is_defaulted
printf("#define disable_video_newleader                 %s\n", (disable_video_newleader == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_video_norace_complete_is_defaulted
printf("#define disable_video_norace_complete           %s\n", (disable_video_norace_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_video_norace_halfway_is_defaulted
printf("#define disable_video_norace_halfway            %s\n", (disable_video_norace_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_video_race_is_defaulted
printf("#define disable_video_race                      %s\n", (disable_video_race == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_video_sfv_is_defaulted
printf("#define disable_video_sfv                       %s\n", (disable_video_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_video_update_is_defaulted
printf("#define disable_video_update                    %s\n", (disable_video_update == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_zip_complete_is_defaulted
printf("#define disable_zip_complete                    %s\n", (disable_zip_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_zip_halfway_is_defaulted
printf("#define disable_zip_halfway                     %s\n", (disable_zip_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_zip_newleader_is_defaulted
printf("#define disable_zip_newleader                   %s\n", (disable_zip_newleader == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_zip_norace_complete_is_defaulted
printf("#define disable_zip_norace_complete             %s\n", (disable_zip_norace_complete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_zip_norace_halfway_is_defaulted
printf("#define disable_zip_norace_halfway              %s\n", (disable_zip_norace_halfway == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_zip_race_is_defaulted
printf("#define disable_zip_race                        %s\n", (disable_zip_race == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_zip_sfv_is_defaulted
printf("#define disable_zip_sfv                         %s\n", (disable_zip_sfv == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef disable_zip_update_is_defaulted
printf("#define disable_zip_update                      %s\n", (disable_zip_update == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef dupepath_is_defaulted
printf("#define dupepath                                %s\n", stringify(dupepath));
#endif
#ifndef enable_accept_script_is_defaulted
printf("#define enable_accept_script                    %s\n", (enable_accept_script == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef enable_affil_script_is_defaulted
printf("#define enable_affil_script                     %s\n", (enable_affil_script == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef enable_complete_script_is_defaulted
printf("#define enable_complete_script                  %s\n", (enable_complete_script == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef enable_files_ahead_is_defaulted
printf("#define enable_files_ahead                      %s\n", (enable_files_ahead == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef enable_mp3_script_is_defaulted
printf("#define enable_mp3_script                       %s\n", (enable_mp3_script == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef enable_nfo_script_is_defaulted
printf("#define enable_nfo_script                       %s\n", (enable_nfo_script == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef enable_sample_script_is_defaulted
printf("#define enable_sample_script                    %s\n", (enable_sample_script == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef enable_unduper_script_is_defaulted
printf("#define enable_unduper_script                   %s\n", (enable_unduper_script == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef exclude_non_sfv_dirs_is_defaulted
printf("#define exclude_non_sfv_dirs                    %s\n", (exclude_non_sfv_dirs == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef extract_nfo_is_defaulted
printf("#define extract_nfo                             %s\n", (extract_nfo == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef force_sfv_first_is_defaulted
printf("#define force_sfv_first                         %s\n", (force_sfv_first == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef get_competitor_list_is_defaulted
printf("#define get_competitor_list                     %s\n", (get_competitor_list == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef get_user_stats_is_defaulted
printf("#define get_user_stats                          %s\n", (get_user_stats == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef gl_sections_is_defaulted
printf("#define gl_sections                             %s\n", stringify(gl_sections));
#endif
#ifndef gl_userfiles_is_defaulted
printf("#define gl_userfiles                            %s\n", stringify(gl_userfiles));
#endif
#ifndef group_dirs_is_defaulted
printf("#define group_dirs                              %s\n", stringify(group_dirs));
#endif
#ifndef hide_affil_gname_is_defaulted
printf("#define hide_affil_gname                        %s\n", stringify(hide_affil_gname));
#endif
#ifndef hide_affil_groups_is_defaulted
printf("#define hide_affil_groups                       %s\n", stringify(hide_affil_groups));
#endif
#ifndef hide_affil_uname_is_defaulted
printf("#define hide_affil_uname                        %s\n", stringify(hide_affil_uname));
#endif
#ifndef hide_affil_users_is_defaulted
printf("#define hide_affil_users                        %s\n", stringify(hide_affil_users));
#endif
#ifndef hide_gname_is_defaulted
printf("#define hide_gname                              %s\n", stringify(hide_gname));
#endif
#ifndef hide_group_uploaders_is_defaulted
printf("#define hide_group_uploaders                    %s\n", (hide_group_uploaders == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef hide_uname_is_defaulted
printf("#define hide_uname                              %s\n", stringify(hide_uname));
#endif
#ifndef ignore_lock_timeout_is_defaulted
printf("#define ignore_lock_timeout                     %s\n", (ignore_lock_timeout == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef ignore_zero_size_is_defaulted
printf("#define ignore_zero_size                        %s\n", (ignore_zero_size == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef ignore_zero_sized_on_rescan_is_defaulted
printf("#define ignore_zero_sized_on_rescan             %s\n", (ignore_zero_sized_on_rescan == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef ignored_types_is_defaulted
printf("#define ignored_types                           %s\n", stringify(ignored_types));
#endif
#ifndef incomplete_base_nfo_indicator_is_defaulted
printf("#define incomplete_base_nfo_indicator           %s\n", stringify(incomplete_base_nfo_indicator));
#endif
#ifndef incomplete_base_sfv_indicator_is_defaulted
printf("#define incomplete_base_sfv_indicator           %s\n", stringify(incomplete_base_sfv_indicator));
#endif
#ifndef incomplete_cd_indicator_is_defaulted
printf("#define incomplete_cd_indicator                 %s\n", stringify(incomplete_cd_indicator));
#endif
#ifndef incomplete_generic1_base_nfo_indicator_is_defaulted
printf("#define incomplete_generic1_base_nfo_indicator  %s\n", stringify(incomplete_generic1_base_nfo_indicator));
#endif
#ifndef incomplete_generic1_base_sfv_indicator_is_defaulted
printf("#define incomplete_generic1_base_sfv_indicator  %s\n", stringify(incomplete_generic1_base_sfv_indicator));
#endif
#ifndef incomplete_generic1_cd_indicator_is_defaulted
printf("#define incomplete_generic1_cd_indicator        %s\n", stringify(incomplete_generic1_cd_indicator));
#endif
#ifndef incomplete_generic1_indicator_is_defaulted
printf("#define incomplete_generic1_indicator           %s\n", stringify(incomplete_generic1_indicator));
#endif
#ifndef incomplete_generic1_nfo_indicator_is_defaulted
printf("#define incomplete_generic1_nfo_indicator       %s\n", stringify(incomplete_generic1_nfo_indicator));
#endif
#ifndef incomplete_generic1_path_is_defaulted
printf("#define incomplete_generic1_path                %s\n", stringify(incomplete_generic1_path));
#endif
#ifndef incomplete_generic1_sfv_indicator_is_defaulted
printf("#define incomplete_generic1_sfv_indicator       %s\n", stringify(incomplete_generic1_sfv_indicator));
#endif
#ifndef incomplete_generic2_base_nfo_indicator_is_defaulted
printf("#define incomplete_generic2_base_nfo_indicator  %s\n", stringify(incomplete_generic2_base_nfo_indicator));
#endif
#ifndef incomplete_generic2_base_sfv_indicator_is_defaulted
printf("#define incomplete_generic2_base_sfv_indicator  %s\n", stringify(incomplete_generic2_base_sfv_indicator));
#endif
#ifndef incomplete_generic2_cd_indicator_is_defaulted
printf("#define incomplete_generic2_cd_indicator        %s\n", stringify(incomplete_generic2_cd_indicator));
#endif
#ifndef incomplete_generic2_indicator_is_defaulted
printf("#define incomplete_generic2_indicator           %s\n", stringify(incomplete_generic2_indicator));
#endif
#ifndef incomplete_generic2_nfo_indicator_is_defaulted
printf("#define incomplete_generic2_nfo_indicator       %s\n", stringify(incomplete_generic2_nfo_indicator));
#endif
#ifndef incomplete_generic2_path_is_defaulted
printf("#define incomplete_generic2_path                %s\n", stringify(incomplete_generic2_path));
#endif
#ifndef incomplete_generic2_sfv_indicator_is_defaulted
printf("#define incomplete_generic2_sfv_indicator       %s\n", stringify(incomplete_generic2_sfv_indicator));
#endif
#ifndef incomplete_indicator_is_defaulted
printf("#define incomplete_indicator                    %s\n", stringify(incomplete_indicator));
#endif
#ifndef incomplete_nfo_indicator_is_defaulted
printf("#define incomplete_nfo_indicator                %s\n", stringify(incomplete_nfo_indicator));
#endif
#ifndef incomplete_sfv_indicator_is_defaulted
printf("#define incomplete_sfv_indicator                %s\n", stringify(incomplete_sfv_indicator));
#endif
#ifndef incompleteislink_is_defaulted
printf("#define incompleteislink                        %s\n", stringify(incompleteislink));
#endif
#ifndef lock_optimize_is_defaulted
printf("#define lock_optimize                           %s\n", stringify(lock_optimize));
#endif
#ifndef log_is_defaulted
printf("#define log                                     %s\n", stringify(log));
#endif
#ifndef mark_empty_dirs_as_incomplete_on_rescan_is_defaulted
printf("#define mark_empty_dirs_as_incomplete_on_rescan %s\n", (mark_empty_dirs_as_incomplete_on_rescan == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef mark_file_as_bad_is_defaulted
printf("#define mark_file_as_bad                        %s\n", (mark_file_as_bad == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef max_groups_in_top_is_defaulted
printf("#define max_groups_in_top                       %s\n", stringify(max_groups_in_top));
#endif
#ifndef max_seconds_wait_for_lock_is_defaulted
printf("#define max_seconds_wait_for_lock               %s\n", stringify(max_seconds_wait_for_lock));
#endif
#ifndef max_users_in_top_is_defaulted
printf("#define max_users_in_top                        %s\n", stringify(max_users_in_top));
#endif
#ifndef message_file_name_is_defaulted
printf("#define message_file_name                       %s\n", stringify(message_file_name));
#endif
#ifndef message_footer_is_defaulted
printf("#define message_footer                          %s\n", stringify(message_footer));
#endif
#ifndef message_group_body_is_defaulted
printf("#define message_group_body                      %s\n", stringify(message_group_body));
#endif
#ifndef message_group_footer_is_defaulted
printf("#define message_group_footer                    %s\n", stringify(message_group_footer));
#endif
#ifndef message_group_header_is_defaulted
printf("#define message_group_header                    %s\n", stringify(message_group_header));
#endif
#ifndef message_header_is_defaulted
printf("#define message_header                          %s\n", stringify(message_header));
#endif
#ifndef message_mp3_is_defaulted
printf("#define message_mp3                             %s\n", stringify(message_mp3));
#endif
#ifndef message_user_body_is_defaulted
printf("#define message_user_body                       %s\n", stringify(message_user_body));
#endif
#ifndef message_user_footer_is_defaulted
printf("#define message_user_footer                     %s\n", stringify(message_user_footer));
#endif
#ifndef message_user_header_is_defaulted
printf("#define message_user_header                     %s\n", stringify(message_user_header));
#endif
#ifndef min_halfway_files_is_defaulted
printf("#define min_halfway_files                       %s\n", stringify(min_halfway_files));
#endif
#ifndef min_halfway_size_is_defaulted
printf("#define min_halfway_size                        %s\n", stringify(min_halfway_size));
#endif
#ifndef min_newleader_files_is_defaulted
printf("#define min_newleader_files                     %s\n", stringify(min_newleader_files));
#endif
#ifndef min_newleader_size_is_defaulted
printf("#define min_newleader_size                      %s\n", stringify(min_newleader_size));
#endif
#ifndef min_update_files_is_defaulted
printf("#define min_update_files                        %s\n", stringify(min_update_files));
#endif
#ifndef min_update_size_is_defaulted
printf("#define min_update_size                         %s\n", stringify(min_update_size));
#endif
#ifndef mp3_script_is_defaulted
printf("#define mp3_script                              %s\n", stringify(mp3_script));
#endif
#ifndef mp3_script_cookies_is_defaulted
printf("#define mp3_script_cookies                      %s\n", stringify(mp3_script_cookies));
#endif
#ifndef newleader_files_ahead_is_defaulted
printf("#define newleader_files_ahead                   %s\n", stringify(newleader_files_ahead));
#endif
#ifndef nfo_script_is_defaulted
printf("#define nfo_script                              %s\n", stringify(nfo_script));
#endif
#ifndef nocheck_dirs_is_defaulted
printf("#define nocheck_dirs                            %s\n", stringify(nocheck_dirs));
#endif
#ifndef noforce_sfv_first_dirs_is_defaulted
printf("#define noforce_sfv_first_dirs                  %s\n", stringify(noforce_sfv_first_dirs));
#endif
#ifndef other_completebar_is_defaulted
printf("#define other_completebar                       %s\n", stringify(other_completebar));
#endif
#ifndef program_gid_is_defaulted
printf("#define program_gid                             %s\n", stringify(program_gid));
#endif
#ifndef program_uid_is_defaulted
printf("#define program_uid                             %s\n", stringify(program_uid));
#endif
#ifndef progressmeter_is_defaulted
printf("#define progressmeter                           %s\n", stringify(progressmeter));
#endif
#ifndef progressmeter_mp3_is_defaulted
printf("#define progressmeter_mp3                       %s\n", stringify(progressmeter_mp3));
#endif
#ifndef rar_completebar_is_defaulted
printf("#define rar_completebar                         %s\n", stringify(rar_completebar));
#endif
#ifndef realtime_group_body_is_defaulted
printf("#define realtime_group_body                     %s\n", stringify(realtime_group_body));
#endif
#ifndef realtime_group_footer_is_defaulted
printf("#define realtime_group_footer                   %s\n", stringify(realtime_group_footer));
#endif
#ifndef realtime_group_header_is_defaulted
printf("#define realtime_group_header                   %s\n", stringify(realtime_group_header));
#endif
#ifndef realtime_mp3_info_is_defaulted
printf("#define realtime_mp3_info                       %s\n", stringify(realtime_mp3_info));
#endif
#ifndef realtime_user_body_is_defaulted
printf("#define realtime_user_body                      %s\n", stringify(realtime_user_body));
#endif
#ifndef realtime_user_footer_is_defaulted
printf("#define realtime_user_footer                    %s\n", stringify(realtime_user_footer));
#endif
#ifndef realtime_user_header_is_defaulted
printf("#define realtime_user_header                    %s\n", stringify(realtime_user_header));
#endif
#ifndef remove_dot_debug_on_delete_is_defaulted
printf("#define remove_dot_debug_on_delete              %s\n", (remove_dot_debug_on_delete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef remove_dot_files_on_delete_is_defaulted
printf("#define remove_dot_files_on_delete              %s\n", (remove_dot_files_on_delete == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef rescan_chdir_flags_is_defaulted
printf("#define rescan_chdir_flags                      %s\n", stringify(rescan_chdir_flags));
#endif
#ifndef rescan_default_to_quick_is_defaulted
printf("#define rescan_default_to_quick                 %s\n", (rescan_default_to_quick == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef rescan_nocheck_dirs_allowed_is_defaulted
printf("#define rescan_nocheck_dirs_allowed             %s\n", (rescan_nocheck_dirs_allowed == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef sample_list_is_defaulted
printf("#define sample_list                             %s\n", stringify(sample_list));
#endif
#ifndef sample_script_is_defaulted
printf("#define sample_script                           %s\n", stringify(sample_script));
#endif
#ifndef sfv_calc_single_fname_is_defaulted
printf("#define sfv_calc_single_fname                   %s\n", (sfv_calc_single_fname == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef sfv_cleanup_is_defaulted
printf("#define sfv_cleanup                             %s\n", (sfv_cleanup == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef sfv_cleanup_crlf_is_defaulted
printf("#define sfv_cleanup_crlf                        %s\n", (sfv_cleanup_crlf == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef sfv_cleanup_lowercase_is_defaulted
printf("#define sfv_cleanup_lowercase                   %s\n", (sfv_cleanup_lowercase == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef sfv_dirs_is_defaulted
printf("#define sfv_dirs                                %s\n", stringify(sfv_dirs));
#endif
#ifndef sfv_dupecheck_is_defaulted
printf("#define sfv_dupecheck                           %s\n", (sfv_dupecheck == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef sfv_lenient_is_defaulted
printf("#define sfv_lenient                             %s\n", (sfv_lenient == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef short_sitename_is_defaulted
printf("#define short_sitename                          %s\n", stringify(short_sitename));
#endif
#ifndef show_group_info_is_defaulted
printf("#define show_group_info                         %s\n", (show_group_info == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef show_stats_from_pos2_only_is_defaulted
printf("#define show_stats_from_pos2_only               %s\n", (show_stats_from_pos2_only == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef show_user_info_is_defaulted
printf("#define show_user_info                          %s\n", (show_user_info == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef show_users_in_group_dirs_is_defaulted
printf("#define show_users_in_group_dirs                %s\n", (show_users_in_group_dirs == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef sitepath_dir_is_defaulted
printf("#define sitepath_dir                            %s\n", stringify(sitepath_dir));
#endif
#ifndef sleep_on_bad_is_defaulted
printf("#define sleep_on_bad                            %s\n", stringify(sleep_on_bad));
#endif
#ifndef spaces_to_dots_is_defaulted
printf("#define spaces_to_dots                          %s\n", (spaces_to_dots == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef speedtest_delfile_is_defaulted
printf("#define speedtest_delfile                       %s\n", (speedtest_delfile == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef speedtest_dirs_is_defaulted
printf("#define speedtest_dirs                          %s\n", stringify(speedtest_dirs));
#endif
#ifndef status_bar_type_is_defaulted
printf("#define status_bar_type                         %s\n", stringify(status_bar_type));
#endif
#ifndef storage_is_defaulted
printf("#define storage                                 %s\n", stringify(storage));
#endif
#ifndef strict_path_match_is_defaulted
printf("#define strict_path_match                       %s\n", (strict_path_match == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef strict_sfv_check_is_defaulted
printf("#define strict_sfv_check                        %s\n", (strict_sfv_check == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef subdir_list_is_defaulted
printf("#define subdir_list                             %s\n", stringify(subdir_list));
#endif
#ifndef test_for_password_is_defaulted
printf("#define test_for_password                       %s\n", (test_for_password == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef unduper_script_is_defaulted
printf("#define unduper_script                          %s\n", stringify(unduper_script));
#endif
#ifndef unzip_bin_is_defaulted
printf("#define unzip_bin                               %s\n", stringify(unzip_bin));
#endif
#ifndef use_group_dirs_as_affil_list_is_defaulted
printf("#define use_group_dirs_as_affil_list            %s\n", (use_group_dirs_as_affil_list == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef use_partial_on_noforce_is_defaulted
printf("#define use_partial_on_noforce                  %s\n", (use_partial_on_noforce == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef userellink_is_defaulted
printf("#define userellink                              %s\n", stringify(userellink));
#endif
#ifndef video_completebar_is_defaulted
printf("#define video_completebar                       %s\n", stringify(video_completebar));
#endif
#ifndef video_types_is_defaulted
printf("#define video_types                             %s\n", stringify(video_types));
#endif
#ifndef write_complete_message_is_defaulted
printf("#define write_complete_message                  %s\n", (write_complete_message == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef write_complete_message_in_group_dirs_is_defaulted
printf("#define write_complete_message_in_group_dirs    %s\n", (write_complete_message_in_group_dirs == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef zip_bin_is_defaulted
printf("#define zip_bin                                 %s\n", stringify(zip_bin));
#endif
#ifndef zip_clean_is_defaulted
printf("#define zip_clean                               %s\n", (zip_clean == FALSE ? "FALSE" : "TRUE"));
#endif
#ifndef zip_completebar_is_defaulted
printf("#define zip_completebar                         %s\n", stringify(zip_completebar));
#endif
#ifndef zip_dirs_is_defaulted
printf("#define zip_dirs                                %s\n", stringify(zip_dirs));
#endif
#ifndef zipscript_SFV_ok_is_defaulted
printf("#define zipscript_SFV_ok                        %s\n", stringify(zipscript_SFV_ok));
#endif
#ifndef zipscript_SFV_skip_is_defaulted
printf("#define zipscript_SFV_skip                      %s\n", stringify(zipscript_SFV_skip));
#endif
#ifndef zipscript_any_ok_is_defaulted
printf("#define zipscript_any_ok                        %s\n", stringify(zipscript_any_ok));
#endif
#ifndef zipscript_footer_error_is_defaulted
printf("#define zipscript_footer_error                  %s\n", stringify(zipscript_footer_error));
#endif
#ifndef zipscript_footer_ok_is_defaulted
printf("#define zipscript_footer_ok                     %s\n", stringify(zipscript_footer_ok));
#endif
#ifndef zipscript_footer_skip_is_defaulted
printf("#define zipscript_footer_skip                   %s\n", stringify(zipscript_footer_skip));
#endif
#ifndef zipscript_footer_unknown_is_defaulted
printf("#define zipscript_footer_unknown                %s\n", stringify(zipscript_footer_unknown));
#endif
#ifndef zipscript_header_is_defaulted
printf("#define zipscript_header                        %s\n", stringify(zipscript_header));
#endif
#ifndef zipscript_sfv_ok_is_defaulted
printf("#define zipscript_sfv_ok                        %s\n", stringify(zipscript_sfv_ok));
#endif
#ifndef zipscript_zip_ok_is_defaulted
printf("#define zipscript_zip_ok                        %s\n", stringify(zipscript_zip_ok));
#endif
#ifndef zsinternal_checks_completed_is_defaulted
printf("#define zsinternal_checks_completed             %s\n", stringify(zsinternal_checks_completed));
#endif
#ifdef USING_GLFTPD
printf("  Compiled for glftpd!\n");
#else
printf("  Compiled for generic ftpd!\n");
#endif
}

void print_full_config(void)
{
printf("#define BAD_CRC                                 %s\n", stringify(BAD_CRC));
printf("#define BAD_ZIP                                 %s\n", stringify(BAD_ZIP));
printf("#define BANNED_BITRATE                          %s\n", stringify(BANNED_BITRATE));
printf("#define BANNED_FILE                             %s\n", stringify(BANNED_FILE));
printf("#define BANNED_GENRE                            %s\n", stringify(BANNED_GENRE));
printf("#define BANNED_YEAR                             %s\n", stringify(BANNED_YEAR));
printf("#define DOUBLE_SFV                              %s\n", stringify(DOUBLE_SFV));
printf("#define DUPE_NFO                                %s\n", stringify(DUPE_NFO));
printf("#define EMPTY_FILE                              %s\n", stringify(EMPTY_FILE));
printf("#define EMPTY_SFV                               %s\n", stringify(EMPTY_SFV));
printf("#define GROUPFILE                               %s\n", stringify(GROUPFILE));
printf("#define NOT_IN_SFV                              %s\n", stringify(NOT_IN_SFV));
printf("#define PASSWDFILE                              %s\n", stringify(PASSWDFILE));
printf("#define SFV_FIRST                               %s\n", stringify(SFV_FIRST));
printf("#define SPEEDTEST                               %s\n", stringify(SPEEDTEST));
printf("#define UNKNOWN_FILE                            %s\n", stringify(UNKNOWN_FILE));
printf("#define ZIP_NFO                                 %s\n", stringify(ZIP_NFO));
printf("#define accept_script                           %s\n", stringify(accept_script));
printf("#define affil_script                            %s\n", stringify(affil_script));
printf("#define allow_dir_chown_in_ng_chown             %s\n", (allow_dir_chown_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
printf("#define allow_error2_in_unzip                   %s\n", (allow_error2_in_unzip == FALSE ? "FALSE" : "TRUE"));
printf("#define allow_file_resume                       %s\n", (allow_file_resume == FALSE ? "FALSE" : "TRUE"));
printf("#define allow_files_chown_in_ng_chown           %s\n", (allow_files_chown_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
printf("#define allow_files_not_in_sfv                  %s\n", (allow_files_not_in_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define allow_gid_change_in_ng_chown            %s\n", (allow_gid_change_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
printf("#define allow_slash_in_sfv                      %s\n", (allow_slash_in_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define allow_uid_change_in_ng_chown            %s\n", (allow_uid_change_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
printf("#define allowed_constant_bitrates               %s\n", stringify(allowed_constant_bitrates));
printf("#define allowed_genres                          %s\n", stringify(allowed_genres));
printf("#define allowed_types                           %s\n", stringify(allowed_types));
printf("#define allowed_types_exemption_dirs            %s\n", stringify(allowed_types_exemption_dirs));
printf("#define allowed_years                           %s\n", stringify(allowed_years));
printf("#define always_scan_audio_syms                  %s\n", (always_scan_audio_syms == FALSE ? "FALSE" : "TRUE"));
printf("#define announce_norace                         %s\n", (announce_norace == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_allowed_genre_check               %s\n", (audio_allowed_genre_check == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_artist_noid3                      %s\n", (audio_artist_noid3 == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_artist_nosub                      %s\n", (audio_artist_nosub == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_artist_path                       %s\n", stringify(audio_artist_path));
printf("#define audio_artist_sort                       %s\n", (audio_artist_sort == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_banned_genre_check                %s\n", (audio_banned_genre_check == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_cbr_check                         %s\n", (audio_cbr_check == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_cbr_warn                          %s\n", (audio_cbr_warn == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_completebar                       %s\n", stringify(audio_completebar));
printf("#define audio_genre_path                        %s\n", stringify(audio_genre_path));
printf("#define audio_genre_sort                        %s\n", (audio_genre_sort == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_genre_warn                        %s\n", (audio_genre_warn == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_group_path                        %s\n", stringify(audio_group_path));
printf("#define audio_group_sort                        %s\n", (audio_group_sort == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_ignored_languages                 %s\n", stringify(audio_ignored_languages));
printf("#define audio_language_path                     %s\n", stringify(audio_language_path));
printf("#define audio_language_sort                     %s\n", (audio_language_sort == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_nocheck_dirs                      %s\n", stringify(audio_nocheck_dirs));
printf("#define audio_nosort_dirs                       %s\n", stringify(audio_nosort_dirs));
printf("#define audio_year_check                        %s\n", (audio_year_check == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_year_path                         %s\n", stringify(audio_year_path));
printf("#define audio_year_sort                         %s\n", (audio_year_sort == FALSE ? "FALSE" : "TRUE"));
printf("#define audio_year_warn                         %s\n", (audio_year_warn == FALSE ? "FALSE" : "TRUE"));
printf("#define banned_filelist                         %s\n", stringify(banned_filelist));
printf("#define banned_genres                           %s\n", stringify(banned_genres));
printf("#define benchmark_mode                          %s\n", (benchmark_mode == FALSE ? "FALSE" : "TRUE"));
printf("#define change_spaces_to_underscore_in_ng_chown %s\n", (change_spaces_to_underscore_in_ng_chown == FALSE ? "FALSE" : "TRUE"));
printf("#define charbar_filled                          %s\n", stringify(charbar_filled));
printf("#define charbar_missing                         %s\n", stringify(charbar_missing));
printf("#define check_for_banned_files                  %s\n", (check_for_banned_files == FALSE ? "FALSE" : "TRUE"));
printf("#define check_for_missing_nfo_dirs              %s\n", stringify(check_for_missing_nfo_dirs));
printf("#define check_for_missing_sample_dirs           %s\n", stringify(check_for_missing_sample_dirs));
printf("#define chmod_completebar                       %s\n", (chmod_completebar == FALSE ? "FALSE" : "TRUE"));
printf("#define cleanupdirs                             %s\n", stringify(cleanupdirs));
printf("#define cleanupdirs_dated                       %s\n", stringify(cleanupdirs_dated));
printf("#define combine_path                            %s\n", (combine_path == FALSE ? "FALSE" : "TRUE"));
printf("#define complete_script                         %s\n", stringify(complete_script));
printf("#define create_incomplete_links_in_group_dirs   %s\n", (create_incomplete_links_in_group_dirs == FALSE ? "FALSE" : "TRUE"));
printf("#define create_m3u                              %s\n", (create_m3u == FALSE ? "FALSE" : "TRUE"));
printf("#define create_missing_files                    %s\n", (create_missing_files == FALSE ? "FALSE" : "TRUE"));
printf("#define create_missing_sample_link              %s\n", (create_missing_sample_link == FALSE ? "FALSE" : "TRUE"));
printf("#define create_missing_sfv                      %s\n", (create_missing_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define create_missing_sfv_link                 %s\n", (create_missing_sfv_link == FALSE ? "FALSE" : "TRUE"));
printf("#define custom_group_dirs_complete_message      %s\n", stringify(custom_group_dirs_complete_message));
printf("#define days_back_cleanup                       %s\n", stringify(days_back_cleanup));
printf("#define debug_altlog                            %s\n", (debug_altlog == FALSE ? "FALSE" : "TRUE"));
printf("#define debug_announce                          %s\n", (debug_announce == FALSE ? "FALSE" : "TRUE"));
printf("#define debug_mode                              %s\n", (debug_mode == FALSE ? "FALSE" : "TRUE"));
printf("#define del_banned_release                      %s\n", (del_banned_release == FALSE ? "FALSE" : "TRUE"));
printf("#define del_completebar                         %s\n", stringify(del_completebar));
printf("#define del_progressmeter                       %s\n", stringify(del_progressmeter));
printf("#define del_progressmeter_mp3                   %s\n", stringify(del_progressmeter_mp3));
printf("#define delete_old_link                         %s\n", (delete_old_link == FALSE ? "FALSE" : "TRUE"));
printf("#define deny_double_nfo                         %s\n", (deny_double_nfo == FALSE ? "FALSE" : "TRUE"));
printf("#define deny_double_sfv                         %s\n", (deny_double_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define deny_nfo_upload_in_zip                  %s\n", (deny_nfo_upload_in_zip == FALSE ? "FALSE" : "TRUE"));
printf("#define deny_resume_sfv                         %s\n", (deny_resume_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_audio_complete                  %s\n", (disable_audio_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_audio_halfway                   %s\n", (disable_audio_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_audio_newleader                 %s\n", (disable_audio_newleader == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_audio_norace_complete           %s\n", (disable_audio_norace_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_audio_norace_halfway            %s\n", (disable_audio_norace_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_audio_race                      %s\n", (disable_audio_race == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_audio_sfv                       %s\n", (disable_audio_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_audio_update                    %s\n", (disable_audio_update == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_other_complete                  %s\n", (disable_other_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_other_halfway                   %s\n", (disable_other_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_other_newleader                 %s\n", (disable_other_newleader == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_other_norace_complete           %s\n", (disable_other_norace_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_other_norace_halfway            %s\n", (disable_other_norace_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_other_race                      %s\n", (disable_other_race == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_other_sfv                       %s\n", (disable_other_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_other_update                    %s\n", (disable_other_update == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_rar_complete                    %s\n", (disable_rar_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_rar_halfway                     %s\n", (disable_rar_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_rar_newleader                   %s\n", (disable_rar_newleader == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_rar_norace_complete             %s\n", (disable_rar_norace_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_rar_norace_halfway              %s\n", (disable_rar_norace_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_rar_race                        %s\n", (disable_rar_race == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_rar_sfv                         %s\n", (disable_rar_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_rar_update                      %s\n", (disable_rar_update == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_video_complete                  %s\n", (disable_video_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_video_halfway                   %s\n", (disable_video_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_video_newleader                 %s\n", (disable_video_newleader == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_video_norace_complete           %s\n", (disable_video_norace_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_video_norace_halfway            %s\n", (disable_video_norace_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_video_race                      %s\n", (disable_video_race == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_video_sfv                       %s\n", (disable_video_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_video_update                    %s\n", (disable_video_update == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_zip_complete                    %s\n", (disable_zip_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_zip_halfway                     %s\n", (disable_zip_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_zip_newleader                   %s\n", (disable_zip_newleader == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_zip_norace_complete             %s\n", (disable_zip_norace_complete == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_zip_norace_halfway              %s\n", (disable_zip_norace_halfway == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_zip_race                        %s\n", (disable_zip_race == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_zip_sfv                         %s\n", (disable_zip_sfv == FALSE ? "FALSE" : "TRUE"));
printf("#define disable_zip_update                      %s\n", (disable_zip_update == FALSE ? "FALSE" : "TRUE"));
printf("#define dupepath                                %s\n", stringify(dupepath));
printf("#define enable_accept_script                    %s\n", (enable_accept_script == FALSE ? "FALSE" : "TRUE"));
printf("#define enable_affil_script                     %s\n", (enable_affil_script == FALSE ? "FALSE" : "TRUE"));
printf("#define enable_complete_script                  %s\n", (enable_complete_script == FALSE ? "FALSE" : "TRUE"));
printf("#define enable_files_ahead                      %s\n", (enable_files_ahead == FALSE ? "FALSE" : "TRUE"));
printf("#define enable_mp3_script                       %s\n", (enable_mp3_script == FALSE ? "FALSE" : "TRUE"));
printf("#define enable_nfo_script                       %s\n", (enable_nfo_script == FALSE ? "FALSE" : "TRUE"));
printf("#define enable_sample_script                    %s\n", (enable_sample_script == FALSE ? "FALSE" : "TRUE"));
printf("#define enable_unduper_script                   %s\n", (enable_unduper_script == FALSE ? "FALSE" : "TRUE"));
printf("#define exclude_non_sfv_dirs                    %s\n", (exclude_non_sfv_dirs == FALSE ? "FALSE" : "TRUE"));
printf("#define extract_nfo                             %s\n", (extract_nfo == FALSE ? "FALSE" : "TRUE"));
printf("#define force_sfv_first                         %s\n", (force_sfv_first == FALSE ? "FALSE" : "TRUE"));
printf("#define get_competitor_list                     %s\n", (get_competitor_list == FALSE ? "FALSE" : "TRUE"));
printf("#define get_user_stats                          %s\n", (get_user_stats == FALSE ? "FALSE" : "TRUE"));
printf("#define gl_sections                             %s\n", stringify(gl_sections));
printf("#define gl_userfiles                            %s\n", stringify(gl_userfiles));
printf("#define group_dirs                              %s\n", stringify(group_dirs));
printf("#define hide_affil_gname                        %s\n", stringify(hide_affil_gname));
printf("#define hide_affil_groups                       %s\n", stringify(hide_affil_groups));
printf("#define hide_affil_uname                        %s\n", stringify(hide_affil_uname));
printf("#define hide_affil_users                        %s\n", stringify(hide_affil_users));
printf("#define hide_gname                              %s\n", stringify(hide_gname));
printf("#define hide_group_uploaders                    %s\n", (hide_group_uploaders == FALSE ? "FALSE" : "TRUE"));
printf("#define hide_uname                              %s\n", stringify(hide_uname));
printf("#define ignore_lock_timeout                     %s\n", (ignore_lock_timeout == FALSE ? "FALSE" : "TRUE"));
printf("#define ignore_zero_size                        %s\n", (ignore_zero_size == FALSE ? "FALSE" : "TRUE"));
printf("#define ignore_zero_sized_on_rescan             %s\n", (ignore_zero_sized_on_rescan == FALSE ? "FALSE" : "TRUE"));
printf("#define ignored_types                           %s\n", stringify(ignored_types));
printf("#define incomplete_base_nfo_indicator           %s\n", stringify(incomplete_base_nfo_indicator));
printf("#define incomplete_base_sfv_indicator           %s\n", stringify(incomplete_base_sfv_indicator));
printf("#define incomplete_cd_indicator                 %s\n", stringify(incomplete_cd_indicator));
printf("#define incomplete_generic1_base_nfo_indicator  %s\n", stringify(incomplete_generic1_base_nfo_indicator));
printf("#define incomplete_generic1_base_sfv_indicator  %s\n", stringify(incomplete_generic1_base_sfv_indicator));
printf("#define incomplete_generic1_cd_indicator        %s\n", stringify(incomplete_generic1_cd_indicator));
printf("#define incomplete_generic1_indicator           %s\n", stringify(incomplete_generic1_indicator));
printf("#define incomplete_generic1_nfo_indicator       %s\n", stringify(incomplete_generic1_nfo_indicator));
printf("#define incomplete_generic1_path                %s\n", stringify(incomplete_generic1_path));
printf("#define incomplete_generic1_sfv_indicator       %s\n", stringify(incomplete_generic1_sfv_indicator));
printf("#define incomplete_generic2_base_nfo_indicator  %s\n", stringify(incomplete_generic2_base_nfo_indicator));
printf("#define incomplete_generic2_base_sfv_indicator  %s\n", stringify(incomplete_generic2_base_sfv_indicator));
printf("#define incomplete_generic2_cd_indicator        %s\n", stringify(incomplete_generic2_cd_indicator));
printf("#define incomplete_generic2_indicator           %s\n", stringify(incomplete_generic2_indicator));
printf("#define incomplete_generic2_nfo_indicator       %s\n", stringify(incomplete_generic2_nfo_indicator));
printf("#define incomplete_generic2_path                %s\n", stringify(incomplete_generic2_path));
printf("#define incomplete_generic2_sfv_indicator       %s\n", stringify(incomplete_generic2_sfv_indicator));
printf("#define incomplete_indicator                    %s\n", stringify(incomplete_indicator));
printf("#define incomplete_nfo_indicator                %s\n", stringify(incomplete_nfo_indicator));
printf("#define incomplete_sfv_indicator                %s\n", stringify(incomplete_sfv_indicator));
printf("#define incompleteislink                        %s\n", stringify(incompleteislink));
printf("#define lock_optimize                           %s\n", stringify(lock_optimize));
printf("#define log                                     %s\n", stringify(log));
printf("#define mark_empty_dirs_as_incomplete_on_rescan %s\n", (mark_empty_dirs_as_incomplete_on_rescan == FALSE ? "FALSE" : "TRUE"));
printf("#define mark_file_as_bad                        %s\n", (mark_file_as_bad == FALSE ? "FALSE" : "TRUE"));
printf("#define max_groups_in_top                       %s\n", stringify(max_groups_in_top));
printf("#define max_seconds_wait_for_lock               %s\n", stringify(max_seconds_wait_for_lock));
printf("#define max_users_in_top                        %s\n", stringify(max_users_in_top));
printf("#define message_file_name                       %s\n", stringify(message_file_name));
printf("#define message_footer                          %s\n", stringify(message_footer));
printf("#define message_group_body                      %s\n", stringify(message_group_body));
printf("#define message_group_footer                    %s\n", stringify(message_group_footer));
printf("#define message_group_header                    %s\n", stringify(message_group_header));
printf("#define message_header                          %s\n", stringify(message_header));
printf("#define message_mp3                             %s\n", stringify(message_mp3));
printf("#define message_user_body                       %s\n", stringify(message_user_body));
printf("#define message_user_footer                     %s\n", stringify(message_user_footer));
printf("#define message_user_header                     %s\n", stringify(message_user_header));
printf("#define min_halfway_files                       %s\n", stringify(min_halfway_files));
printf("#define min_halfway_size                        %s\n", stringify(min_halfway_size));
printf("#define min_newleader_files                     %s\n", stringify(min_newleader_files));
printf("#define min_newleader_size                      %s\n", stringify(min_newleader_size));
printf("#define min_update_files                        %s\n", stringify(min_update_files));
printf("#define min_update_size                         %s\n", stringify(min_update_size));
printf("#define mp3_script                              %s\n", stringify(mp3_script));
printf("#define mp3_script_cookies                      %s\n", stringify(mp3_script_cookies));
printf("#define newleader_files_ahead                   %s\n", stringify(newleader_files_ahead));
printf("#define nfo_script                              %s\n", stringify(nfo_script));
printf("#define nocheck_dirs                            %s\n", stringify(nocheck_dirs));
printf("#define noforce_sfv_first_dirs                  %s\n", stringify(noforce_sfv_first_dirs));
printf("#define other_completebar                       %s\n", stringify(other_completebar));
printf("#define program_gid                             %s\n", stringify(program_gid));
printf("#define program_uid                             %s\n", stringify(program_uid));
printf("#define progressmeter                           %s\n", stringify(progressmeter));
printf("#define progressmeter_mp3                       %s\n", stringify(progressmeter_mp3));
printf("#define rar_completebar                         %s\n", stringify(rar_completebar));
printf("#define realtime_group_body                     %s\n", stringify(realtime_group_body));
printf("#define realtime_group_footer                   %s\n", stringify(realtime_group_footer));
printf("#define realtime_group_header                   %s\n", stringify(realtime_group_header));
printf("#define realtime_mp3_info                       %s\n", stringify(realtime_mp3_info));
printf("#define realtime_user_body                      %s\n", stringify(realtime_user_body));
printf("#define realtime_user_footer                    %s\n", stringify(realtime_user_footer));
printf("#define realtime_user_header                    %s\n", stringify(realtime_user_header));
printf("#define remove_dot_debug_on_delete              %s\n", (remove_dot_debug_on_delete == FALSE ? "FALSE" : "TRUE"));
printf("#define remove_dot_files_on_delete              %s\n", (remove_dot_files_on_delete == FALSE ? "FALSE" : "TRUE"));
printf("#define rescan_chdir_flags                      %s\n", stringify(rescan_chdir_flags));
printf("#define rescan_default_to_quick                 %s\n", (rescan_default_to_quick == FALSE ? "FALSE" : "TRUE"));
printf("#define rescan_nocheck_dirs_allowed             %s\n", (rescan_nocheck_dirs_allowed == FALSE ? "FALSE" : "TRUE"));
printf("#define sample_list                             %s\n", stringify(sample_list));
printf("#define sample_script                           %s\n", stringify(sample_script));
printf("#define sfv_calc_single_fname                   %s\n", (sfv_calc_single_fname == FALSE ? "FALSE" : "TRUE"));
printf("#define sfv_cleanup                             %s\n", (sfv_cleanup == FALSE ? "FALSE" : "TRUE"));
printf("#define sfv_cleanup_crlf                        %s\n", (sfv_cleanup_crlf == FALSE ? "FALSE" : "TRUE"));
printf("#define sfv_cleanup_lowercase                   %s\n", (sfv_cleanup_lowercase == FALSE ? "FALSE" : "TRUE"));
printf("#define sfv_dirs                                %s\n", stringify(sfv_dirs));
printf("#define sfv_dupecheck                           %s\n", (sfv_dupecheck == FALSE ? "FALSE" : "TRUE"));
printf("#define sfv_lenient                             %s\n", (sfv_lenient == FALSE ? "FALSE" : "TRUE"));
printf("#define short_sitename                          %s\n", stringify(short_sitename));
printf("#define show_group_info                         %s\n", (show_group_info == FALSE ? "FALSE" : "TRUE"));
printf("#define show_stats_from_pos2_only               %s\n", (show_stats_from_pos2_only == FALSE ? "FALSE" : "TRUE"));
printf("#define show_user_info                          %s\n", (show_user_info == FALSE ? "FALSE" : "TRUE"));
printf("#define show_users_in_group_dirs                %s\n", (show_users_in_group_dirs == FALSE ? "FALSE" : "TRUE"));
printf("#define sitepath_dir                            %s\n", stringify(sitepath_dir));
printf("#define sleep_on_bad                            %s\n", stringify(sleep_on_bad));
printf("#define spaces_to_dots                          %s\n", (spaces_to_dots == FALSE ? "FALSE" : "TRUE"));
printf("#define speedtest_delfile                       %s\n", (speedtest_delfile == FALSE ? "FALSE" : "TRUE"));
printf("#define speedtest_dirs                          %s\n", stringify(speedtest_dirs));
printf("#define status_bar_type                         %s\n", stringify(status_bar_type));
printf("#define storage                                 %s\n", stringify(storage));
printf("#define strict_path_match                       %s\n", (strict_path_match == FALSE ? "FALSE" : "TRUE"));
printf("#define strict_sfv_check                        %s\n", (strict_sfv_check == FALSE ? "FALSE" : "TRUE"));
printf("#define subdir_list                             %s\n", stringify(subdir_list));
printf("#define test_for_password                       %s\n", (test_for_password == FALSE ? "FALSE" : "TRUE"));
printf("#define unduper_script                          %s\n", stringify(unduper_script));
printf("#define unzip_bin                               %s\n", stringify(unzip_bin));
printf("#define use_group_dirs_as_affil_list            %s\n", (use_group_dirs_as_affil_list == FALSE ? "FALSE" : "TRUE"));
printf("#define use_partial_on_noforce                  %s\n", (use_partial_on_noforce == FALSE ? "FALSE" : "TRUE"));
printf("#define userellink                              %s\n", stringify(userellink));
printf("#define video_completebar                       %s\n", stringify(video_completebar));
printf("#define video_types                             %s\n", stringify(video_types));
printf("#define write_complete_message                  %s\n", (write_complete_message == FALSE ? "FALSE" : "TRUE"));
printf("#define write_complete_message_in_group_dirs    %s\n", (write_complete_message_in_group_dirs == FALSE ? "FALSE" : "TRUE"));
printf("#define zip_bin                                 %s\n", stringify(zip_bin));
printf("#define zip_clean                               %s\n", (zip_clean == FALSE ? "FALSE" : "TRUE"));
printf("#define zip_completebar                         %s\n", stringify(zip_completebar));
printf("#define zip_dirs                                %s\n", stringify(zip_dirs));
printf("#define zipscript_SFV_ok                        %s\n", stringify(zipscript_SFV_ok));
printf("#define zipscript_SFV_skip                      %s\n", stringify(zipscript_SFV_skip));
printf("#define zipscript_any_ok                        %s\n", stringify(zipscript_any_ok));
printf("#define zipscript_footer_error                  %s\n", stringify(zipscript_footer_error));
printf("#define zipscript_footer_ok                     %s\n", stringify(zipscript_footer_ok));
printf("#define zipscript_footer_skip                   %s\n", stringify(zipscript_footer_skip));
printf("#define zipscript_footer_unknown                %s\n", stringify(zipscript_footer_unknown));
printf("#define zipscript_header                        %s\n", stringify(zipscript_header));
printf("#define zipscript_sfv_ok                        %s\n", stringify(zipscript_sfv_ok));
printf("#define zipscript_zip_ok                        %s\n", stringify(zipscript_zip_ok));
printf("#define zsinternal_checks_completed             %s\n", stringify(zsinternal_checks_completed));
#ifdef USING_GLFTPD
printf("  Compiled for glftpd!\n");
#else
printf("  Compiled for generic ftpd!\n");
#endif
}
