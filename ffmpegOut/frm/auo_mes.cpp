﻿// -----------------------------------------------------------------------------------------
// x264guiEx by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2022 rigaya
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// --------------------------------------------------------------------------------------------


#include "auo_mes.h"
#include "auo_util.h"
#include <string>
#include <filesystem>


//grep -E '\".+\"' auo_error.cpp | sed 's/^.*"\(.*\)".*$/\1/'
//grep 'Text = L"' frmOtherSettings.h | sed -r 's/.+->([a-zA-Z0-9]+)->Text = L"(.+)".+/Auo\1=\2/g'

static const std::array<std::pair<const char *, AuoMes>, AUO_SECTION_FIN> AUO_MES_SECTIONS_STR = {
#define AUO_SECTION(x) std::make_pair( #x, x ## _SECTION_START )
	AUO_SECTION(AUO_GUIEX),
	AUO_SECTION(AUO_ERR),
	AUO_SECTION(AUO_AUDIO),
	AUO_SECTION(AUO_ENCODE),
	AUO_SECTION(AUO_FAW2AAC),
	AUO_SECTION(AUO_CONF),
	AUO_SECTION(AUO_MUX),
	AUO_SECTION(AUO_BAT),
	AUO_SECTION(AUO_VIDEO),
	AUO_SECTION(AUO_OPTION),
	AUO_SECTION(AUO_CONFIG),
	AUO_SECTION(AUO_CONFIG_FCG),
	AUO_SECTION(AUO_CONFIG_CX),
	AUO_SECTION(AUO_CONFIG_CX_REPLACE),
	AUO_SECTION(AUO_CONFIG_TT),
	AUO_SECTION(AUO_AUTO_SAVE_LOG),
	AUO_SECTION(AUO_BITRATE_CALC),
	AUO_SECTION(AUO_LOG),
	AUO_SECTION(AUO_SET_TRANSPARENCY),
	AUO_SECTION(AUO_SET_LOG_COLOR),
	AUO_SECTION(AUO_SAVE_NEW_STG),
	AUO_SECTION(AUO_OTHER_SETTINGS)
#undef AUO_SECTION
};

// sed -n '81,859p' auo_mes.h | tr -d \\r | awk '{print $1}' | grep -v ^$ | grep -v ^// | grep -v _SECTION_FIN,$ | sed 's/,$//g' | sed 's/[^ ]\+/"&"/g' | sed 's/$/,/g'
static const char * AUO_MES_ID_NAME_STR[] = {
"AUO_MES_UNKNOWN",
"AUO_GUIEX_SECTION_START",
"AUO_GUIEX_LANG",
"AUO_GUIEX_FULL_NAME",
"AUO_GUIEX_TOTAL_TIME",
"AUO_GUIEX_TIME_HOUR",
"AUO_GUIEX_TIME_MIN",
"AUO_GUIEX_TIME_SEC",
"AUO_GUIEX_ALL_SUPPORT_FORMATS",
"AUO_GUIEX_ERROR",
"AUO_GUIEX_DEFAULT",
"AUO_ERR_SECTION_START",
"AUO_ERR_CONF_NOT_INIT0",
"AUO_ERR_CONF_NOT_INIT1",
"AUO_ERR_CONF_NOT_INIT2",
"AUO_ERR_CONF_NOT_INIT3",
"AUO_ERR_FAILED_GET_TEMP_PATH",
"AUO_ERR_UNABLE_OPEM_TEMP_FILE",
"AUO_ERR_NO_TEMP_ROOT",
"AUO_ERR_NO_AUD_TEMP_ROOT",
"AUO_ERR_FILENAME_TOO_LONG",
"AUO_ERR_SAVDIR_DO_NOT_EXIST1",
"AUO_ERR_SAVDIR_DO_NOT_EXIST2",
"AUO_ERR_SAVDIR_DO_NOT_EXIST3",
"AUO_ERR_SAVDIR_DO_NOT_EXIST4",
"AUO_ERR_SAVDIR_DO_NOT_EXIST5",
"AUO_ERR_SAVDIR_DO_NOT_EXIST6",
"AUO_ERR_FILE_ALREADY_OPENED1",
"AUO_ERR_FILE_ALREADY_OPENED2",
"AUO_WARN_NO_AUO_CHECK_FILEOPEN1",
"AUO_WARN_NO_AUO_CHECK_FILEOPEN2",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR1",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR2",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_SPECIAL_CHAR1",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_SPECIAL_CHAR2",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_FILE",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_FOLDER",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_ACCESS_DENIED1",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_ACCESS_DENIED2",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_ACCESS_DENIED3",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_ACCESS_DENIED4",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_ACCESS_DENIED5",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_ACCESS_DENIED6",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_ACCESS_DENIED7",
"AUO_ERR_FAILED_TO_OPEN_TEMP_FILE_DIR_OTHER",
"AUO_ERR_NOTHING_TO_OUTPUT",
"AUO_ERR_OUTPUT_ZERO_FRAMES1",
"AUO_ERR_OUTPUT_ZERO_FRAMES2",
"AUO_ERR_AMP_BITRATE_CONFLICT1",
"AUO_ERR_AMP_BITRATE_CONFLICT2",
"AUO_ERR_AMP_BITRATE_CONFLICT3",
"AUO_ERR_AMP_BITRATE_CONFLICT4",
"AUO_ERR_AMP_AFS_AUDIO_DELAY_CONFLICT1",
"AUO_ERR_AMP_AFS_AUDIO_DELAY_CONFLICT2",
"AUO_ERR_AFS_AUDIO_DELAY_CONFLICT",
"AUO_ERR_AMP_TARGET_LIMIT_NAME_BITRATE",
"AUO_ERR_AMP_TARGET_LIMIT_NAME_FILESIZE",
"AUO_ERR_AMP_TARGET_LIMIT_NAME_BITRATE_FILESIZE",
"AUO_ERR_AMP_DO_AUD_ENC_FIRST",
"AUO_ERR_AMP_AUD_TOO_BIG1",
"AUO_ERR_AMP_AUD_TOO_BIG2",
"AUO_ERR_AMP_TARGET_BITRATE_TOO_SMALL1",
"AUO_ERR_AMP_TARGET_BITRATE_TOO_SMALL2",
"AUO_ERR_AMP_CHANGE_BITRATE_TOO_BIG",
"AUO_ERR_AMP_CHANGE_BITRATE_TOO_SMALL",
"AUO_ERR_AMP_CHANGE_BITRATE_FROM_TO",
"AUO_ERR_AMP_CHANGE_BITRATE_AUTO",
"AUO_ERR_INVALID_RESOLUTION",
"AUO_ERR_INVALID_RESOLUTION_WIDTH",
"AUO_ERR_INVALID_RESOLUTION_HEIGHT",
"AUO_ERR_LOG_LINE_CACHE",
"AUO_ERR_TC2MP4_AFS_NOT_SUPPORTED1",
"AUO_ERR_TC2MP4_AFS_NOT_SUPPORTED2",
"AUO_ERR_TC2MP4_AFS_NOT_SUPPORTED3",
"AUO_ERR_NO_EXE_FILE1",
"AUO_ERR_NO_EXE_FILE2",
"AUO_ERR_NO_EXE_FILE3",
"AUO_ERR_USE_DEFAULT_AUDIO_ENCODER",
"AUO_ERR_INFO_USE_EXE_FOUND",
"AUO_ERR_INVALID_INI_FILE",
"AUO_ERR_UNSUPPORTED_AUDIO_FORMAT_BY_MUXER1",
"AUO_ERR_UNSUPPORTED_AUDIO_FORMAT_BY_MUXER2",
"AUO_ERR_UNSUPPORTED_AUDIO_FORMAT_BY_MUXER3",
"AUO_ERR_MP4_MUXER_ERROR",
"AUO_ERR_AFS_AUTO_DISABLE1",
"AUO_ERR_AFS_AUTO_DISABLE2",
"AUO_ERR_AFS_AUTO_DISABLE3",
"AUO_ERR_AFS_AUTO_DISABLE4",
"AUO_ERR_AFS_AUTO_DISABLE5",
"AUO_ERR_AFS_SETUP1",
"AUO_ERR_AFS_SETUP2",
"AUO_ERR_AFS_SETUP3",
"AUO_ERR_AFS_SETUP4",
"AUO_ERR_AFS_SETUP5",
"AUO_ERR_AFS_SETUP6",
"AUO_ERR_OPEN_PIPE",
"AUO_ERR_GET_PIPE_HANDLE",
"AUO_ERR_RUN_PROCESS",
"AUO_ERR_VIDEO_OUTPUT_THREAD_START",
"AUO_ERR_VIDEO_CREATE_PARAM_MEM",
"AUO_ERR_VIDEO_CREATE_EVENT",
"AUO_ERR_VIDEO_WAIT_EVENT",
"AUO_ERR_VIDEO_SET_EVENT",
"AUO_ERR_VIDEO_OPEN_SHARED_INPUT_BUF",
"AUO_ERR_VIDEO_GET_CONV_FUNC",
"AUO_ERR_QPFILE_FAILED",
"AUO_ERR_TCFILE_FAILED",
"AUO_ERR_MALLOC_PIXEL_DATA",
"AUO_ERR_MALLOC_TC",
"AUO_ERR_MALLOC_8BIT",
"AUO_ERR_AFS_INTERLACE_STG",
"AUO_ERR_X264_MP4_OUTPUT_NOT_SUPPORTED1",
"AUO_ERR_X264_MP4_OUTPUT_NOT_SUPPORTED2",
"AUO_ERR_X264_MP4_OUTPUT_NOT_SUPPORTED3",
"AUO_ERR_VIDENC_DEAD",
"AUO_ERR_VIDENC_DEAD_AND_NODISKSPACE1",
"AUO_ERR_VIDENC_DEAD_AND_NODISKSPACE2",
"AUO_ERR_VIDENC_DEAD_AND_NODISKSPACE3",
"AUO_ERR_VIDENC_DEAD_AND_NODISKSPACE4",
"AUO_ERR_VIDENC_VERSION1",
"AUO_ERR_VIDENC_VERSION2",
"AUO_ERR_VIDENC_VERSION3",
"AUO_ERR_VIDENC_VERSION4",
"AUO_ERR_AFS_GET_FRAME",
"AUO_ERR_OPEN_WAVFILE",
"AUO_ERR_NO_WAVFILE",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT1",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT2",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT3",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT4",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT5",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT6",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT7",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT8",
"AUO_ERR_AUDIO_LENGTH_DIFFERENT9",
"AUO_ERR_AUDENC_FAILED1",
"AUO_ERR_AUDENC_FAILED2",
"AUO_ERR_MUX_FAILED1",
"AUO_ERR_MUX_FAILED2",
"AUO_ERR_NO_MUX_TMP_ROOT",
"AUO_ERR_FAILED_TMP_DRIVE_SPACE",
"AUO_ERR_FAILED_MUX_DRIVE_SPACE",
"AUO_ERR_FAILED_OUT_DRIVE_SPACE",
"AUO_ERR_FAILED_GET_AUD_SIZE",
"AUO_ERR_FAILED_GET_VID_SIZE",
"AUO_ERR_NO_AUD_FILE",
"AUO_ERR_NO_VID_FILE",
"AUO_ERR_AUD_FILE_ZERO_BYTE",
"AUO_ERR_VID_FILE_ZERO_BYTE",
"AUO_ERR_TMP_NO_ENOUGH_SPACE1",
"AUO_ERR_NO_ENOUGH_SPACE_SHOW_SIZE",
"AUO_ERR_TMP_NO_ENOUGH_SPACE2",
"AUO_ERR_MUX_NO_ENOUGH_SPACE",
"AUO_ERR_OUT_NO_ENOUGH_SPACE",
"AUO_ERR_NO_ENOUGH_SPACE_SHOW_SIZE1",
"AUO_ERR_NO_ENOUGH_SPACE_SHOW_SIZE2",
"AUO_ERR_NO_ENOUGH_SPACE_SHOW_SIZE3",
"AUO_ERR_FAILED_TO_GET_DURATION_FROM_TIMECODE1",
"AUO_ERR_FAILED_TO_GET_DURATION_FROM_TIMECODE2",
"AUO_ERR_FAILED_TO_GET_DURATION_FROM_TIMECODE3",
"AUO_ERR_CHECK_MUXOUT_EXIST",
"AUO_ERR_CHECK_MUXOUT_TO_SMALL1",
"AUO_ERR_CHECK_MUXOUT_TO_SMALL2",
"AUO_ERR_CHECK_MUXOUT_GET_SIZE",
"AUO_ERR_AMP_FAILED",
"AUO_ERR_AMP_FILESIZE_OVER_LIMIT1",
"AUO_ERR_AMP_FILESIZE_OVER_LIMIT2",
"AUO_ERR_NO_AUTO_SAVE_LOG_DIR",
"AUO_ERR_ABORT",
"AUO_ERR_MUX_NO_CHAPTER_FILE",
"AUO_ERR_AMP_RESULT1",
"AUO_ERR_AMP_RESULT2",
"AUO_ERR_AMP_RESULT3",
"AUO_ERR_AMP_RESULT4",
"AUO_ERR_AMP_RESULT5",
"AUO_ERR_AMP_RESULT6",
"AUO_ERR_AMP_RESULT7",
"AUO_ERR_AMP_RESULT8",
"AUO_ERR_AMP_RESULT9",
"AUO_ERR_AMP_RESULT10",
"AUO_ERR_AMP_RESULT11",
"AUO_ERR_MUX_CHPATER_OPEN",
"AUO_ERR_MUX_CHPATER_READ",
"AUO_ERR_MUX_CHPATER_WRITE",
"AUO_ERR_MUX_CHPATER_SWAP",
"AUO_ERR_MUX_CHPATER_CP_DETECT",
"AUO_ERR_MUX_CHPATER_INIT_IMUL2",
"AUO_ERR_MUX_CHPATER_INVALID_FMT",
"AUO_ERR_MUX_CHPATER_NULL_PTR",
"AUO_ERR_MUX_CHPATER_INIT_XML_PARSE",
"AUO_ERR_MUX_CHPATER_INIT_READ_STREAM",
"AUO_ERR_MUX_CHPATER_SET_STREAM",
"AUO_ERR_MUX_CHPATER_PARSE_XML",
"AUO_ERR_MUX_CHPATER_UNKNOWN",
"AUO_ERR_CHPATER_CONVERT",
"AUO_ERR_SEL_CONVERT_FUNC",
"AUO_ERR_NO_BAT_FILE",
"AUO_ERR_MALLOC_BAT_FILE_TMP",
"AUO_ERR_OPEN_BAT_ORG",
"AUO_ERR_FAILED_OPEN_BAT_NEW",
"AUO_ERR_VIDEO_VERY_SHORT1",
"AUO_ERR_VIDEO_VERY_SHORT2",
"AUO_ERR_EXEDIT_NOT_FOUND",
"AUO_ERR_EXEDIT_OUTPUT_START",
"AUO_AUDIO_SECTION_START",
"AUO_AUDIO_FAW_INDEX_ERR",
"AUO_AUDIO_ERR_TOO_SHORT",
"AUO_AUDIO_ERR_OTHER",
"AUO_AUDIO_PROGRESS_BAR_ENCODE",
"AUO_AUDIO_PROGRESS_BAR_WAV_OUT",
"AUO_AUDIO_DELAY_CUT",
"AUO_AUDIO_START_ENCODE",
"AUO_AUDIO_CPU_USAGE",
"AUO_ENCODE_SECTION_START",
"AUO_ENCODE_AUDIO_ONLY",
"AUO_ENCODE_AUDIO_ENCODER",
"AUO_ENCODE_TMP_FOLDER",
"AUO_ENCODE_TMP_FOLDER_AUDIO",
"AUO_ENCODE_ERROR_MOVE_CHAPTER_FILE",
"AUO_ENCODE_FILE_NOT_FOUND",
"AUO_ENCODE_FILE_MOVE_FAILED",
"AUO_ENCODE_AFTER_MUX",
"AUO_ENCODE_TC_FILE",
"AUO_ENCODE_CHAPTER_FILE",
"AUO_ENCODE_CHAPTER_APPLE_FILE",
"AUO_ENCODE_STATUS_FILE",
"AUO_ENCODE_AUDIO_FILE",
"AUO_ENCODE_AMP_ADJUST_LOW_BITRATE_PRESET_KEY",
"AUO_ENCODE_AMP_ADJUST_LOW_BITRATE_PRESET",
"AUO_ENCODE_AMP_ADJUST_LOW_BITRATE_KEY",
"AUO_FAW2AAC_SECTION_START",
"AUO_FAW2AAC_NOT_FOUND",
"AUO_FAW2AAC_NOT_LOADED",
"AUO_FAW2AAC_ERR_INIT",
"AUO_FAW2AAC_RUN",
"AUO_FAW2AAC_ERR_RUN",
"AUO_MUX_SECTION_START",
"AUO_MUX_RUN_START",
"AUO_MUX_RUN_VIDEO",
"AUO_MUX_RUN_AUDIO",
"AUO_MUX_RUN_TC",
"AUO_MUX_RUN_CHAP",
"AUO_MUX_RUN_EXT_MODE",
"AUO_MUX_RUN",
"AUO_MUX_CPU_USAGE",
"AUO_CONF_SECTION_START",
"AUO_CONF_PRIORITY_AVIUTLSYNC",
"AUO_CONF_PRIORITY_HIGHER",
"AUO_CONF_PRIORITY_HIGH",
"AUO_CONF_PRIORITY_NORMAL",
"AUO_CONF_PRIORITY_LOW",
"AUO_CONF_PRIORITY_LOWER",
"AUO_CONF_PRIORITY_REALTIME",
"AUO_CONF_AUDIO_DELAY_NONE",
"AUO_CONF_AUDIO_DELAY_CUT_AUDIO",
"AUO_CONF_AUDIO_DELAY_ADD_VIDEO",
"AUO_CONF_AUDIO_DELAY_EDTS",
"AUO_CONF_LAST_OUT_STG",
"AUO_BAT_SECTION_START",
"AUO_BAT_RUN",
"AUO_VIDEO_SECTION_START",
"AUO_VIDEO_KEY_FRAME_DETECTION_START",
"AUO_VIDEO_KEY_FRAME_ABORT",
"AUO_VIDEO_KEY_FRAME_DETECT_RESULT",
"AUO_VIDEO_SET_KEYFRAME_NO_MUXER",
"AUO_VIDEO_SET_KEYFRAME_NO_CHAPTER",
"AUO_VIDEO_SET_KEYFRAME_CHAPTER_READ_ERROR",
"AUO_VIDEO_SET_KEYFRAME_RESULT",
"AUO_VIDEO_CHAPTER_AFS_ADJUST_START",
"AUO_VIDEO_CHAPTER_AFS_ADJUST_FIN",
"AUO_VIDEO_SET_KEYFRAME_NOT_DETECTED",
"AUO_VIDEO_AFS_VBV_WARN",
"AUO_VIDEO_AFS_AVIUTL_AND_VPP_CONFLICT1",
"AUO_VIDEO_AFS_AVIUTL_AND_VPP_CONFLICT2",
"AUO_VIDEO_AUDIO_PROC_WAIT",
"AUO_VIDEO_CPU_USAGE",
"AUO_VIDEO_AVIUTL_PROC_AVG_TIME",
"AUO_VIDEO_ENCODE_TIME",
"AUO_VIDEO_ENCODE",
"AUO_VIDEO_AUDIO_ENCODE",
"AUO_OPTION_SECTION_START",
"AUO_OPTION_VUI_UNDEF",
"AUO_OPTION_VUI_AUTO",
"AUO_OPTION_LEVEL_AUTO",
"AUO_CONFIG_SECTION_START",
"AUO_CONFIG_TEXT_LIMIT_LENGTH",
"AUO_CONFIG_HELP_CMD_UNSET",
"AUO_CONFIG_STATUS_FILE",
"AUO_CONFIG_ANALYSIS_FILE",
"AUO_CONFIG_TC_FILE",
"AUO_CONFIG_CHAPTER_FILE",
"AUO_CONFIG_BAT_FILE",
"AUO_CONFIG_MATRIX_FILE",
"AUO_CONFIG_PROFILE",
"AUO_CONFIG_ERR_INVALID_CHAR",
"AUO_CONFIG_ALREADY_EXISTS",
"AUO_CONFIG_OVERWRITE_CHECK",
"AUO_CONFIG_VID_ENC_NOT_EXIST",
"AUO_CONFIG_AUD_ENC_NOT_EXIST",
"AUO_CONFIG_SPECIFY_EXE_PATH",
"AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI1",
"AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI2",
"AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI3",
"AUO_CONFIG_FAW_PATH_UNSET1",
"AUO_CONFIG_FAW_PATH_UNSET2",
"AUO_CONFIG_AMP_STG_INVALID1",
"AUO_CONFIG_AMP_STG_INVALID2",
"AUO_CONFIG_AMP_STG_INVALID3",
"AUO_CONFIG_AMP_STG_INVALID4",
"AUO_CONFIG_VIDEO",
"AUO_CONFIG_COMMANDLINE",
"AUO_CONFIG_EXTENSION",
"AUO_CONFIG_ADDITIONAL_COMMANDLINE",
"AUO_CONFIG_QUALITY_HIGH",
"AUO_CONFIG_QUALITY_LOW",
"AUO_CONFIG_MODE_QP",
"AUO_CONFIG_MODE_CRF",
"AUO_CONFIG_MODE_TARGET_BITRATE",
"AUO_CONFIG_MODE_BITRATE",
"AUO_CONFIG_INSERT_STRING",
"AUO_CONFIG_FILE_FULL_PATH",
"AUO_CONFIG_ERR_OPEN_STG_FILE",
"AUO_CONFIG_ASK_STG_FILE_DELETE",
"AUO_CONFIG_TC_FILE_OUT",
"AUO_CONFIG_ERR_GET_EXE_VER",
"AUO_CONFIG_ERR_EXE_NOT_FOUND",
"AUO_CONFIG_ERR_GET_HELP",
"AUO_CONFIG_ERR_OPEN_HELP",
"AUO_CONFIG_BITRATE_AUTO",
"AUO_CONFIG_FCG_SECTION_START",
"AuofcgtabPageExSettings",
"AuofcgCBAuoTcfileout",
"AuofcgCBAudioInput",
"AuofcgCB2passEnc",
"AuofcgLBOutputExt",
"AuofcgLBOutputCsp",
"AuofcgLBInterlaced",
"AuofcgBTVideoEncoderPath",
"AuofcgLBVideoEncoderPath",
"AuofcgLBTempDir",
"AuofcgBTCustomTempDir",
"AuofcggroupBoxCmdEx",
"AuofcgLBInCmd",
"AuofcgLBffmpegOutPriority",
"AuofcggroupBoxAudio",
"AuofcgCBAudioUseInternal",
"AuofcgCBFAWCheck",
"AuofcgLBAudioBitrateInternal",
"AuofcgLBAudioEncModeInternal",
"AuofcgLBAudioDelayCut",
"AuofcgBTCustomAudioTempDir",
"AuofcgCBAudioUsePipe",
"AuofcgLBAudioBitrate",
"AuofcgLBAudioTemp",
"AuofcgBTAudioEncoderPath",
"AuofcgCBAudio2pass",
"AuofcgLBAudioEncoder",
"AuofcgLBAudioPriority",
"AuofcgLBAudioEncoderPath",
"AuofcgLBAudioEncMode",
"AuofcgCBAudioEncTiming",
"AuofcgtoolStripSettings",
"AuofcgTSBSave",
"AuofcgTSBSaveNew",
"AuofcgTSBDelete",
"AuofcgTSSettings",
"AuofcgTSExeFileshelp",
"AuofcgTSBCMDOnly",
"AuofcgTSBBitrateCalc",
"AuofcgTSBOtherSettings",
"AuofcgTSLSettingsNotes",
"AuofcgTSTSettingsNotes",
"AuofcgtabPageMP4",
"AuofcgBTMP4RawPath",
"AuofcgLBMP4RawPath",
"AuofcgCBMP4MuxApple",
"AuofcgBTMP4BoxTempDir",
"AuofcgLBMP4BoxTempDir",
"AuofcgBTTC2MP4Path",
"AuofcgBTMP4MuxerPath",
"AuofcgLBTC2MP4Path",
"AuofcgLBMP4MuxerPath",
"AuofcgLBMP4CmdEx",
"AuofcgCBMP4MuxerExt",
"AuofcgtabPageMKV",
"AuofcgBTMKVMuxerPath",
"AuofcgLBMKVMuxerPath",
"AuofcgLBMKVMuxerCmdEx",
"AuofcgCBMKVMuxerExt",
"AuofcgtabPageMPG",
"AuofcgBTMPGMuxerPath",
"AuofcgLBMPGMuxerPath",
"AuofcgLBMPGMuxerCmdEx",
"AuofcgCBMPGMuxerExt",
"AuofcgtabPageMux",
"AuofcgLBMuxPriority",
"AuofcgCBMuxMinimize",
"AuofcgtabPageBat",
"AuofcgLBBatAfterString",
"AuofcgLBBatBeforeString",
"AuofcgBTBatBeforePath",
"AuofcgLBBatBeforePath",
"AuofcgCBWaitForBatBefore",
"AuofcgCBRunBatBefore",
"AuofcgBTBatAfterPath",
"AuofcgLBBatAfterPath",
"AuofcgCBWaitForBatAfter",
"AuofcgCBRunBatAfter",
"AuofcgLBguiExBlog",
"AuofcgBTCancel",
"AuofcgBTOK",
"AuofcgBTDefault",
"AuofcgLBVersionDate",
"AuofcgLBVersion",
"AUO_CONFIG_CX_SECTION_START",
"AUO_CONFIG_CX_X264_ENCMODE_SINGLE_CBR",
"AUO_CONFIG_CX_X264_ENCMODE_SINGLE_QP",
"AUO_CONFIG_CX_X264_ENCMODE_SINGLE_CRF",
"AUO_CONFIG_CX_X264_ENCMODE_MULTI_1PASS",
"AUO_CONFIG_CX_X264_ENCMODE_MULTI_NPASS",
"AUO_CONFIG_CX_X264_ENCMODE_AMP",
"AUO_CONFIG_CX_X264_ENCMODE_CRF_WITH_CHECK",
"AUO_CONFIG_CX_ASPECT_SAR",
"AUO_CONFIG_CX_ASPECT_DAR",
"AUO_CONFIG_CX_TEMPDIR_OUTDIR",
"AUO_CONFIG_CX_TEMPDIR_SYSTEM",
"AUO_CONFIG_CX_TEMPDIR_CUSTOM",
"AUO_CONFIG_CX_AUDTEMP_DEFAULT",
"AUO_CONFIG_CX_AUDTEMP_CUSTOM",
"AUO_CONFIG_CX_MP4BOXTEMP_DEFAULT",
"AUO_CONFIG_CX_MP4BOXTEMP_CUSTOM",
"AUO_CONFIG_CX_INTERLACE_PROGRESSIVE",
"AUO_CONFIG_CX_INTERLACE_TFF",
"AUO_CONFIG_CX_INTERLACE_BFF",
"AUO_CONFIG_CX_AUD_ENC_ORDER_AFTER",
"AUO_CONFIG_CX_AUD_ENC_ORDER_BEFORE",
"AUO_CONFIG_CX_AUD_ENC_ORDER_PARALLEL",
"AUO_CONFIG_CX_USE_DEFAULT_EXE_PATH",
"AUO_CONFIG_CX_LOG_LEVEL_INFO",
"AUO_CONFIG_CX_LOG_LEVEL_MORE",
"AUO_CONFIG_CX_LOG_LEVEL_DEBUG",
"AUO_CONFIG_CX_FEATURE_CURRENT_RC",
"AUO_CONFIG_CX_FEATURE_10BIT_DEPTH",
"AUO_CONFIG_CX_FEATURE_HYPER_MODE",
"AUO_CONFIG_CX_FEATURE_INTERLACE",
"AUO_CONFIG_CX_FEATURE_VUI_INFO",
"AUO_CONFIG_CX_FEATURE_AUD",
"AUO_CONFIG_CX_FEATURE_PIC_STRUCT",
"AUO_CONFIG_CX_FEATURE_RDO",
"AUO_CONFIG_CX_FEATURE_CAVLC",
"AUO_CONFIG_CX_FEATURE_BFRAME",
"AUO_CONFIG_CX_FEATURE_ADAPTIVE_I",
"AUO_CONFIG_CX_FEATURE_ADAPTIVE_B",
"AUO_CONFIG_CX_FEATURE_WEIGHT_P",
"AUO_CONFIG_CX_FEATURE_WEIGHT_B",
"AUO_CONFIG_CX_FEATURE_FADE_DETECT",
"AUO_CONFIG_CX_FEATURE_B_PYRAMID",
"AUO_CONFIG_CX_FEATURE_B_PYRAMID_MANY_BFRAMES",
"AUO_CONFIG_CX_FEATURE_MBBRC",
"AUO_CONFIG_CX_FEATURE_EXT_BRC",
"AUO_CONFIG_CX_FEATURE_LA_DS",
"AUO_CONFIG_CX_FEATURE_QP_MINMAX",
"AUO_CONFIG_CX_FEATURE_TRELLIS",
"AUO_CONFIG_CX_FEATURE_INTRA_REFRESH",
"AUO_CONFIG_CX_FEATURE_NO_DEBLOCK",
"AUO_CONFIG_CX_FEATURE_PERMBQP",
"AUO_CONFIG_CX_FEATURE_DIRECT_BIAS_ADJUST",
"AUO_CONFIG_CX_FEATURE_GLOBAL_MOTION_ADJUST",
"AUO_CONFIG_CX_FEATURE_HEVC_SAO",
"AUO_CONFIG_CX_FEATURE_HEVC_CTU",
"AUO_CONFIG_CX_FEATURE_HEVC_TSKIP",
"AUO_CONFIG_CX_REPLACE_SECTION_START",
"AUO_CONFIG_CX_REPLACE_VID_PATH",
"AUO_CONFIG_CX_REPLACE_AUD_PATH",
"AUO_CONFIG_CX_REPLACE_TMPDIR",
"AUO_CONFIG_CX_REPLACE_TMPFILE",
"AUO_CONFIG_CX_REPLACE_TMPNAME",
"AUO_CONFIG_CX_REPLACE_SAVPATH",
"AUO_CONFIG_CX_REPLACE_SAVFILE",
"AUO_CONFIG_CX_REPLACE_SAVNAME",
"AUO_CONFIG_CX_REPLACE_SAVDIR",
"AUO_CONFIG_CX_REPLACE_AVIUTLDIR",
"AUO_CONFIG_CX_REPLACE_CHPATH",
"AUO_CONFIG_CX_REPLACE_TCPATH",
"AUO_CONFIG_CX_REPLACE_MUXOUT",
"AUO_CONFIG_CX_REPLACE_X264PATH",
"AUO_CONFIG_CX_REPLACE_X264_10PATH",
"AUO_CONFIG_CX_REPLACE_AUDENCPATH",
"AUO_CONFIG_CX_REPLACE_MP4MUXERPATH",
"AUO_CONFIG_CX_REPLACE_MKVMUXERPATH",
"AUO_CONFIG_CX_REPLACE_FPS_SCALE",
"AUO_CONFIG_CX_REPLACE_FPS_RATE",
"AUO_CONFIG_CX_REPLACE_FPS_RATE_TIMES_4",
"AUO_CONFIG_CX_REPLACE_SAR_X",
"AUO_CONFIG_CX_REPLACE_SAR_Y",
"AUO_CONFIG_CX_REPLACE_DAR_X",
"AUO_CONFIG_CX_REPLACE_DAR_Y",
"AUO_CONFIG_TT_SECTION_START",
"AuofrmTTfcgBTVideoEncoderPath",
"AuofrmTTfcgCXOutputCsp",
"AuofrmTTfcgCB2passEnc",
"AuofrmTTfcgCBAudioInput",
"AuofrmTTfcgTXOutputExt",
"AuofrmTTfcgCBAuoTcfileout",
"AuofrmTTfcgCXInterlaced",
"AuofrmTTfcgTXCmdEx",
"AuofrmTTfcgTXInCmd",
"AuofrmTTfcgCXffmpegOutPriority",
"AuofrmTTfcgCXTempDir",
"AuofrmTTfcgBTCustomTempDir",
"AuofrmTTfcgCXAudioEncoder",
"AuofrmTTfcgCBFAWCheck",
"AuofrmTTfcgBTAudioEncoderPath",
"AuofrmTTfcgCXAudioEncMode",
"AuofrmTTfcgCBAudio2pass",
"AuofrmTTfcgCBAudioUsePipe",
"AuofrmTTfcgNUAudioBitrate",
"AuofrmTTfcgCXAudioPriority",
"AuofrmTTfcgCXAudioEncTiming",
"AuofrmTTfcgCXAudioTempDir",
"AuofrmTTfcgBTCustomAudioTempDir",
"AuofrmTTfcgCBMP4MuxerExt",
"AuofrmTTfcgCXMP4CmdEx",
"AuofrmTTfcgBTMP4MuxerPath",
"AuofrmTTfcgBTTC2MP4Path",
"AuofrmTTfcgBTMP4RawPath",
"AuofrmTTfcgCXMP4BoxTempDir",
"AuofrmTTfcgBTMP4BoxTempDir",
"AuofrmTTfcgCBMKVMuxerExt",
"AuofrmTTfcgCXMKVCmdEx",
"AuofrmTTfcgBTMKVMuxerPath",
"AuofrmTTfcgCBMPGMuxerExt",
"AuofrmTTfcgCXMPGCmdEx",
"AuofrmTTfcgBTMPGMuxerPath",
"AuofrmTTfcgCXMuxPriority",
"AuofrmTTfcgCBRunBatBefore",
"AuofrmTTfcgCBRunBatAfter",
"AuofrmTTfcgCBWaitForBatBefore",
"AuofrmTTfcgCBWaitForBatAfter",
"AuofrmTTfcgBTBatBeforePath",
"AuofrmTTfcgBTBatAfterPath",
"AuofrmTTfcgBTDefault",
"AuofrmTTfcgTSBDelete",
"AuofrmTTfcgTSBOtherSettings",
"AuofrmTTfcgTSBSave",
"AuofrmTTfcgTSBSaveNew",
"AUO_AUTO_SAVE_LOG_SECTION_START",
"AuofasLBAutoSaveLog",
"AuofasBTAutoSaveLog",
"AuofasBTCancel",
"AuofasBTOK",
"AuofasMain",
"AUO_AUTO_SAVE_LOG_SAME_AS_OUTPUT",
"AUO_AUTO_SAVE_LOG_CUSTOM",
"AUO_AUTO_SAVE_LOG_EXT_FILTER",
"AUO_BITRATE_CALC_SECTION_START",
"AuofbcMain",
"Auolabel1",
"Auolabel2",
"Auolabel3",
"Auolabel4",
"AuofbcLBVideoBitrate",
"AuofbcLBAudioBitrate",
"AuofbcLBMovieSize",
"AuofbcLBVideoBitrateKbps",
"AuofbcLBAudioBitrateKbps",
"Auolabel10",
"AuofbcBTVBApply",
"AuofbcBTABApply",
"AuofbcLBMovieBitrateKbps",
"AuofbcLBMovieBitrate",
"AuofbcRBCalcRate",
"AuofbcRBCalcSize",
"AuofbcTXMovieFrameRate",
"AuofbcLBFrames",
"AuofbcLBMovieFrameRate",
"AuofbcBTChangeLengthMode",
"AUO_FBC_LENGTH_MODE_TO_TIME",
"AUO_FBC_LENGTH_MODE_TO_FRAME_NUMBER",
"AUO_LOG_SECTION_START",
"AuorichTextLog",
"AuotoolStripMenuItemFileOpen",
"AuotoolStripMenuItemFilePathOpen",
"AuoToolStripMenuItemVidEncPriority",
"AuoToolStripMenuItemEncPause",
"AuoToolStripMenuItemTransparent",
"AuotoolStripMenuItemTransparentValue",
"AuotoolStripMenuItemSetLogColor",
"AuoToolStripMenuItemStartMinimized",
"AuotoolStripMenuItemSaveLogSize",
"AuotoolStripMenuItemAutoSave",
"AuotoolStripMenuItemAutoSaveSettings",
"AuotoolStripMenuItemShowStatus",
"AuotoolStripMenuItemTaskBarProgress",
"AuotoolStripMenuItemWindowFont",
"AuostatusStripLog",
"AuotoolStripStatusCurrentTask",
"AuotoolStripStatusElapsedTime",
"AuotoolStripStatusCurrentProgress",
"AUO_LOG_ENCODE_FIN_TIME",
"AUO_LOG_ERR_SAVE_LOG",
"AUO_LOG_ERR_FILE_OPEN",
"AUO_LOG_ERR_SHOW_FILE_PATH",
"AuoFrmLogMain",
"AUO_NEW_FOLDER_NAME_SECTION_START",
"AuofnfBTOK",
"AuofnfBTCancel",
"AuofnfMain",
"AUO_NEW_FOLDER_NAME_ERR_FOLDER_PATH",
"AUO_OTHER_SETTINGS_SECTION_START",
"AuofosCBCancel",
"AuofosCBOK",
"AuofosCBAutoAFSDisable",
"AuofosCBAutoDelChap",
"AuofostabPageGeneral",
"AuofosLBDefaultAudioEncoder",
"AuofosCBAutoRefLimitByLevel",
"AuofosCBChapConvertToUTF8",
"AuofosCBKeepQPFile",
"AuofosCBRunBatMinimized",
"AuofosLBDefaultOutExt2",
"AuofosLBDefaultOutExt",
"AuofostabPageGUI",
"AuofosCBOutputMoreLog",
"AuofosCBWineCompat",
"AuofosCBGetRelativePath",
"AuofosBTSetFont",
"AuofosCBStgEscKey",
"AuofosCBDisableToolTip",
"AuofosCBDisableVisualStyles",
"AuofosCBLogDisableTransparency",
"AuofosLBDisableVisualStyles",
"AuofosCBLogStartMinimized",
"AuofosLBStgDir",
"AuofosBTStgDir",
"AuofostabPageAMP",
"AuofosCBAutoDelStats",
"AuofosGroupBoxAMPLimit",
"AuofosLBAMPLimitMarginWarning",
"AuofosBTAMPMarginMulti",
"AuofosGroupBoxAMPLimitMarginExample",
"AuofosLBAMPLMMExampleB32",
"AuofosLBAMPLMMExampleB22",
"AuofosLBAMPLMMExampleB31",
"AuofosLBAMPLMMExampleB21",
"AuofosLBAMPLMMExampleB12",
"AuofosLBAMPLMMExampleB11",
"AuofosLBAMPLMMExampleA12",
"AuofosLBAMPLMMExampleA11",
"AuofosLBAMPLimitMarginInfo",
"AuofosLBAMPLimitMarginMax",
"AuofosLBAMPLimitMarginMin",
"AuofosCBAmpKeepOldFile",
"AuofosCBPerfMonitor",
"AuofosLBLogOut",
"AuofosMain",
"AUO_OTHER_SETTINGS_AMP_MARGIN_XS",
"AUO_OTHER_SETTINGS_AMP_MARGIN_S",
"AUO_OTHER_SETTINGS_AMP_MARGIN_M",
"AUO_OTHER_SETTINGS_AMP_MARGIN_L",
"AUO_OTHER_SETTINGS_AUDIO_ENCODER_EXTERNAL",
"AUO_SAVE_NEW_STG_SECTION_START",
"AuofsnBTOK",
"AuofsnBTCancel",
"AuofsnBTNewFolder",
"AuofsnMain",
"AUO_SET_LOG_COLOR_SECTION_START",
"AuofscBTOK",
"AuofscBTCancel",
"AuofscBTDefault",
"AuofscLBColorBackground",
"AuofcsLBColorText",
"AuofcsTXColorTextInfo",
"AuofcsTXColorTextWarning",
"AuofcsTXColorTextError",
"AuofcsMain",
"AUO_SET_TRANSPARENCY_SECTION_START",
"AuofstBTDefault",
"AuofstBTOK",
"AuofstBTCancel",
"AuofstLBTransparency",
"AuofstMain",
"AUO_MESSAGE_FIN"
};

static_assert(AUO_MESSAGE_FIN + 1 == _countof(AUO_MES_ID_NAME_STR));

#if ENCODER_X264 || ENCODER_X265 || ENCODER_SVTAV1 || ENCODER_FFMPEG
std::string str_replace(std::string str, const std::string& from, const std::string& to) {
	std::string::size_type pos = 0;
	while (pos = str.find(from, pos), pos != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
	return str;
}
#endif

AuoMesSections AuoMessages::getSectionId(const std::string& section) const {
	for (size_t i = 0; i < AUO_MES_SECTIONS_STR.size(); i++) {
		if (strcmp(AUO_MES_SECTIONS_STR[i].first, section.c_str()) == 0) {
			return (AuoMesSections)i;
		}
	}
	return AUO_SECTION_UNKNOWN;
}

AuoMes AuoMessages::getId(const AuoMesSections sectionId, const std::string& id) const {
	const AuoMes sectionOffset = (sectionId < 0) ? AUO_MES_UNKNOWN : AUO_MES_SECTIONS_STR[sectionId].second;
	for (AuoMes i = sectionOffset; i < _countof(AUO_MES_ID_NAME_STR); i = (AuoMes)(i + 1)) {
		if (strcmp(AUO_MES_ID_NAME_STR[i], id.c_str()) == 0) {
			return i;
		}
	}
	for (AuoMes i = AUO_MES_UNKNOWN; i < sectionOffset; i = (AuoMes)(i + 1)) {
		if (strcmp(AUO_MES_ID_NAME_STR[i], id.c_str()) == 0) {
			return i;
		}
	}
	return AUO_MES_UNKNOWN;
}

void AuoMessages::proc_line(AuoMesSections& sectionId, char *buffer) {
	// コメント
	if (auto ptr = strchr(buffer, ';'); ptr) *ptr = '\0';
	// section
	{
		auto sec_fin = strchr(buffer, ']');
		if (buffer[0] == '[' && sec_fin) {
			const auto section = std::string(buffer + 1, sec_fin);
			sectionId = getSectionId(section);
			return;
		}
	}
	if (auto sep = strchr(buffer, '='); sep != nullptr) {
		const auto idstr = std::string(buffer, sep);
		const auto id = getId(sectionId, idstr);
		auto mes = std::string(sep + 1);
		if (id >= 0) {
			mes = str_replace(mes, "\r", "");
			mes = str_replace(mes, "\\r\\n", "\r\n");
			mes = str_replace(mes, "\\n", "\n");
			mes = str_replace(mes, "\\r", "\r");
			mes = str_replace(mes, "\\t", "\t");
			messages[id] = char_to_wstring(mes.c_str(), CP_UTF8);
		}
	}
	return;
}

int AuoMessages::read(const std::string& filename) {
	messages.clear();
	messages.resize(AUO_MESSAGE_FIN);
	language = "";

	FILE *fp = nullptr;
	int err = fopen_s(&fp, filename.c_str(), "r");
	if (err != 0) {
		return err;
	}

	AuoMesSections sectionId = AUO_SECTION_UNKNOWN;
	char buffer[4096] = { 0 };
	while (fgets(buffer, _countof(buffer) - 1, fp) != NULL) {
		const auto len = strlen(buffer);
		if (len > 0 && buffer[len-1] == '\n') {
			buffer[len - 1] = '\0';
		}
		proc_line(sectionId, buffer);
	}
	fclose(fp); // ファイルを閉じる
	language = filename;
	return 0;
}

int AuoMessages::read(const char *lang, const char *data, const size_t size) {
	messages.clear();
	messages.resize(AUO_MESSAGE_FIN);

	std::vector<char> buffer(size + 1, '\0');
	memcpy(buffer.data(), data, size);

	AuoMesSections sectionId = AUO_SECTION_UNKNOWN;
	for (char *ptr = buffer.data(), *qtr = nullptr; (ptr = strtok_s(ptr, "\n", &qtr)) != nullptr; ptr = nullptr) {
		proc_line(sectionId, ptr);
	}
	language = (lang) ? lang : wstring_to_string(get(AUO_GUIEX_LANG));
	return 0;
}

std::string get_file_lang_code(const std::string& path) {
	//言語コードの取得
	char buffer[1024];
	GetPrivateProfileString("AUO_GUIEX", "AUO_GUIEX_LANG", "", buffer, _countof(buffer) - 1, path.c_str());
	return wstring_to_string(char_to_wstring(buffer, CP_UTF8));
}

std::vector<std::string> find_lng_files() {
	char target_dir[MAX_PATH_LEN];
	get_auo_path(target_dir, _countof(target_dir));
	PathRemoveFileSpecFixed(target_dir);

	std::vector<std::string> ret;
	try {
		for (const std::filesystem::directory_entry& x : std::filesystem::recursive_directory_iterator(target_dir)) {
			if (strncmp(x.path().filename().string().c_str(), AUO_NAME_WITHOUT_EXT, strlen(AUO_NAME_WITHOUT_EXT)) == 0) {
				if (x.path().extension() == ".lng") {
					const auto language_code = get_file_lang_code(x.path().string());
					if (language_code.length() > 0) {
						//iniファイルもセットであるか確認する
						char auo_path[MAX_PATH_LEN], ini_fileName[MAX_PATH_LEN], ini_append[64];
						sprintf_s(ini_append, ".%s.ini", language_code.c_str());
						get_auo_path(auo_path, _countof(auo_path));
						apply_appendix(ini_fileName, _countof(ini_fileName), auo_path, ini_append);
						if (PathFileExists(ini_fileName)) {
							ret.push_back(x.path().string());
						}
					}
				}
			}
		}
	} catch (...) {}
	return ret;
}
