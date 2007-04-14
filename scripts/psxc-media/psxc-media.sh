#!/bin/bash

###########################################
#
# psxc-media.sh
# =============
# A small wrapper to MediaInfo to announce sample info.
# Supported formats are divx/xvid, vcd/svcd, quicktime, mp3 -
# and probably some others.
#
# The following changes is needed on pzs-ng installs prior to r2301:
#
# Add the following to dZSbot.vars:
#	set variables(SAMPLE_AVI)                   "%pf %u_name %g_name %video_codec %video_framerate %video_bitrate %video_height %video_width %video_aspect %video_interlacement %audio_codec %audio_bitrate %audio_mode %audio_channels %audio_samplerate %audio_resolution %audio_codecprofile"
#	set variables(SAMPLE_MPEG2)                 "%pf %u_name %g_name %video_codec %video_framerate %video_bitrate %video_bitratemode %video_standard %video_height %video_width %video_aspect %video_interlacement %audio_codec %audio_bitrate %audio_mode %audio_channels %audio_samplerate %audio_resolution"
#	set variables(SAMPLE_MPEG1)                 "%pf %u_name %g_name %audio_codec %audio_bitrate %audio_mode %audio_channels %audio_samplerate %audio_resolution"
#	set variables(SAMPLE_QT)                    "%pf %u_name %g_name %video_codec %video_framerate %video_bitrate %video_height %video_width %video_aspect %video_interlacement %audio_codec %audio_bitrate %audio_mode %audio_channels %audio_samplerate %audio_resolution"
#	set variables(SAMPLE_GENERAL)               "%pf %u_name %g_name %video_codec %video_framerate %video_bitrate %video_height %video_width %video_aspect %audio_codec %audio_bitrate %audio_mode %audio_channels %audio_samplerate %audio_resolution"
#
# Add the following announce types to msgtypes(SECTION) (usually in dZSbot.conf.defaults):
#	SAMPLE_AVI SAMPLE_MPEG1 SAMPLE_MPEG2 SAMPLE_QT SAMPLE_GENERAL
#
# Add the following to dZSbot.conf:
#	set disable(SAMPLE_AVI)                     0
#	set disable(SAMPLE_MPEG1)                   0
#	set disable(SAMPLE_MPEG2)                   0
#	set disable(SAMPLE_QT)                      0
#	set disable(SAMPLE_GENERAL)                 0
#
# Add the following to your themefile:
#	announce.SAMPLE_AVI             = "[%b{sample}][%section] %b{%path} has the following specs: Video: %b{%video_codec} %b{%video_width}x%b{%video_height} pixels (aspectratio: %b{%video_aspect}) @%b{%video_framerate} - Audio: %b{%audio_codec} (%b{%audio_samplerate}/%b{%audio_channels} @ %audio_bitrate)"
#	announce.SAMPLE_MPEG2             = "[%b{sample}][%section] %b{%path} has the following specs: Video: %b{%video_codec} %b{%video_width}x%b{%video_height} pixels (aspectratio: %b{%video_aspect}) @%b{%video_framerate} - Audio: %b{%audio_codec} (%b{%audio_samplerate}/%b{%audio_channels} @ %audio_bitrate)"
#	announce.SAMPLE_QT             = "[%b{sample}][%section] %b{%path} has the following specs: Video: %b{%video_codec} %b{%video_width}x%b{%video_height} pixels (aspectratio: %b{%video_aspect}) @%b{%video_framerate} - Audio: %b{%audio_codec} (%b{%audio_samplerate}/%b{%audio_channels} @ %audio_bitrate)"
#	announce.SAMPLE_GENERAL             = "[%b{sample}][%section] %b{%path} has the following specs: Video: %b{%video_codec} %b{%video_width}x%b{%video_height} pixels (aspectratio: %b{%video_aspect}) @%b{%video_framerate} - Audio: %b{%audio_codec} (%b{%audio_samplerate}/%b{%audio_channels} @ %audio_bitrate)"
#	announce.SAMPLE_MPEG1             = "[%b{sample}][%section] %b{%path} has the following specs: Audio: %b{%audio_codec} (%b{%audio_samplerate}/%b{%audio_channels} @ %audio_bitrate)"
#
#########################################################
#
# CONFIG SECTION
# ==============

# where is MediaInfo located?
mediainfo=/bin/MediaInfo.exe

#########################################################
#
# CODEPART BELOW
################
#
#########################################################

filename=$1

while read line; do
	key=$(echo "$line" | cut -d ':' -f 1 | tr -cd 'a-zA-Z0-9\#')
	string=$(echo $line | cut -d ':' -f 2- | tr -cd 'a-zA-Z0-9\ \#\.\,\/\:' | tr -s ' ')

	[[ "$key" == "Video#0" ]] && part=video
	[[ "$key" == "Audio#0" ]] && part=audio
	[[ "$key" == "General#0" ]] && part=general

	case $part in
		general)
			case $key in
				Format)
					case $string in
						*[Aa][Vv][Ii]*) family=avi ;;
						*[Mm][Pp][Ee][Gg]*2*) family=mpeg2 ;;
						*[Mm][Pp][Ee][Gg]*1*) family=mpeg1 ;;
						*[Qq][Uu][Ii][Cc][Kk][Tt][Ii][Mm][Ee]*) family=qt ;;
						*) family=general ;;
					esac
			esac
		;;
		video)
			case $key in
				Codec) v_codec=$(echo $string) ;;
				Framerate) v_framerate=$(echo $string) ;;
				Bitrate) v_bitrate=$(echo $string) ;;
				Bitratemode) v_bitratemode=$(echo $string) ;;
				Width) v_width=$(echo $string | tr -cd '0-9') ;;
				Height) v_height=$(echo $string | tr -cd '0-9') ;;
				Aspectratio) v_aspectratio=$(echo $string) ;;
				Framerate) v_framerate=$(echo $string) ;;
				Standard) v_standard=$(echo $string) ;;
				Interlacement) v_interlacement=$(echo $string) ;;
			esac
		;;
		audio)
			case $key in
				Codec) a_codec=$(echo $string) ;;
				Bitrate) a_bitrate=$(echo $string) ;;
				Bitratemode) a_bitratemode=$(echo $string) ;;
				Channels) a_channels=$(echo $string) ;;
				Samplingrate) a_samplingrate=$(echo $string) ;;
				Resolution) a_resolution=$(echo $string) ;;
				Codecprofile) a_codecprofile=$(echo $string) ;;
			esac
		;;
	esac
done < <($mediainfo $filename 2>/dev/null)
date="$(date "+%a %b %d %H:%M:%S %Y")"

case $family in
	avi)
		echo "$date SAMPLE_AVI: "$PWD" {$USER} {$GROUP} {$v_codec} {$v_framerate} {$v_bitrate} {$v_height} {$v_width} {$v_aspectratio} {$v_interlacement} {$a_codec} {$a_bitrate} {$a_bitratemode} {$a_channels} {$a_samplingrate} {$a_resolution} {$a_codecprofile}"
		;;
	mpeg2)
		echo "$date SAMPLE_MPEG2: "$PWD" {$USER} {$GROUP} {$v_codec} {$v_framerate} {$v_bitrate} {$v_bitratemode} {$v_standard} {$v_height} {$v_width} {$v_aspectratio} {$v_interlacement} {$a_codec} {$a_bitrate} {$a_bitratemode} {$a_channels} {$a_samplingrate} {$a_resolution}"
		;;
	mpeg1)
		echo "$date SAMPLE_MPEG1: "$PWD" {$USER} {$GROUP} {$a_codec} {$a_bitrate} {$a_bitratemode} {$a_channels} {$a_samplingrate} {$a_resolution}"
		;;
	qt)
		echo "$date SAMPLE_QT: "$PWD" {$USER} {$GROUP} {$v_codec} {$v_framerate} {$v_bitrate} {$v_height} {$v_width} {$v_aspectratio} {$v_interlacement} {$a_codec} {$a_bitrate} {$a_bitratemode} {$a_channels} {$a_samplingrate} {$a_resolution}"
		;;
	general)
		echo "$date SAMPLE_GENERAL: "$PWD" {$USER} {$GROUP} {$v_codec} {$v_framerate} {$v_bitrate} {$v_height} {$v_width} {$v_aspectratio} {$a_codec} {$a_bitrate} {$a_bitratemode} {$a_channels} {$a_samplingrate} {$a_resolution}"
		;;
esac

