﻿// -----------------------------------------------------------------------------------------
// ffmpegOut by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2012-2017 rigaya
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

#ifndef _AUO_OPTIONS_H_
#define _AUO_OPTIONS_H_

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <vector>

#include "auo.h"
#include "auo_settings.h"

//エンコードモード
enum {
    ENC_RC_CRF = 0,
    ENC_RC_BITRATE,
    ENC_RC_QP,
};

//QPの最大値
const int ENC_QP_MAX_8BIT  = 69;
const int ENC_QP_MAX_10BIT = 81;

//差がこのくらいなら等しいとみなす(オプション用なのでこのくらいで十分)
const float EPS_FLOAT = 1.0e-4f;

#if ENCODER_X265
static const int AUO_KEYINT_MAX_AUTO = 0;
#elif ENCODER_X264 || ENCODER_SVTAV1
static const int AUO_KEYINT_MAX_AUTO = -1;
#endif

//マクロブロックタイプの一般的なオプション
enum {
    MB_PARTITION_NONE = 0x00000000,
    MB_PARTITION_P8x8 = 0x00000001,
    MB_PARTITION_B8x8 = 0x00000002,
    MB_PARTITION_P4x4 = 0x00000004,
    MB_PARTITION_I8x8 = 0x00000008,
    MB_PARTITION_I4x4 = 0x00000010,
    MB_PARTITION_ALL  = 0x0000001F,
};

enum {
    OUT_CSP_NV12,
    OUT_CSP_YUY2,
    OUT_CSP_YUV444,
    OUT_CSP_P010,
    OUT_CSP_YUV444_16,
    OUT_CSP_RGB,
    OUT_CSP_RGBA,
    OUT_CSP_NV16,
    OUT_CSP_YUV422,
    OUT_CSP_YUV400,
};

enum {
    YC48_COLMAT_CONV_AUTO,
    YC48_COLMAT_CONV_NONE,
    YC48_COLMAT_CONV_BT709,
};

//x264のinput-cspとして使用するもの
//OUT_CSP_NV12, OUT_CSP_YUV444, OUT_CSP_RGB に合わせる
static const char * const specify_csp[] = {
    "nv12",    //OUT_CSP_NV12
    "yuyv422", //OUT_CSP_YUY2
    "yuv444p", //OUT_CSP_YUV444
    "p010le",
    "yuv444p16le",
    "bgr24",   //OUT_CSP_RGB
    "bgra"     //OUT_CSP_RGBA
};
//文字列を引数にとるオプションの引数リスト
//OUT_CSP_NV12, OUT_CSP_YUV444, OUT_CSP_RGB に合わせる
const ENC_OPTION_STR list_output_csp[] = {
    { "nv12",        AUO_MES_UNKNOWN, L"yuv420" },
    { "yuyv422",     AUO_MES_UNKNOWN, L"yuv422" },
    { "yuv444p",     AUO_MES_UNKNOWN, L"yuv444" },
    { "p010le",      AUO_MES_UNKNOWN, L"yuv420(16bit)" },
    { "yuv444p16le", AUO_MES_UNKNOWN, L"yuv444(16bit)" },
    { "bgr24",       AUO_MES_UNKNOWN, L"rgb"  },
    { "bgra",        AUO_MES_UNKNOWN, L"rgba"  },
    { NULL,          AUO_MES_UNKNOWN, NULL }
};

static bool csp_highbit_depth(int output_csp) {
    static const bool list[] = {
        false, false, false,
        true, true,
        false,
        false,
        false /*dummy*/
    };
    static_assert(_countof(list) == _countof(list_output_csp), "list size does not match.");
    return list[output_csp];
}

/*
const X264_OPTION_STR list_aq[] = { 
    { NULL, L"none"    }, 
    { NULL, L"VAQ"     }, 
    { NULL, L"AutoVAQ" }, 
    { NULL, NULL } 
};
const X264_OPTION_STR list_weightp[] = { 
    { NULL, L"Disabled" }, 
    { NULL, L"Simple"   }, 
    { NULL, L"Smart"    }, 
    { NULL, NULL } 
};
const X264_OPTION_STR list_b_adpat[] = {
    { NULL, L"無効" },
    { NULL, L"簡易" },
    { NULL, L"完全" },
    { NULL, NULL }
};
const X264_OPTION_STR list_trellis[] = {
    { NULL, L"Disabled"        },
    { NULL, L"Final MB encode" },
    { NULL, L"All"             },
    { NULL, NULL }
};
const X264_OPTION_STR list_me[] = {
    { "dia",  L"Diamond Search (高速)" },
    { "hex",  L"Hexagonal Search"      },
    { "umh",  L"Uneven Multi-Hexagon"  },
    { "esa",  L"Exaustive Search"      },
    { "tesa", L"Hadamard ESA (低速)"   },
    { NULL, NULL }
};
const X264_OPTION_STR list_subme[] = {
    { NULL, L" 0 (fullpell only)"                },
    { NULL, L" 1 (高速)"                         },
    { NULL, L" 2"                                },
    { NULL, L" 3"                                },
    { NULL, L" 4"                                },
    { NULL, L" 5"                                },
    { NULL, L" 6 (RDO for I/P frames)"           },
    { NULL, L" 7 (RDO for all frames)"           },
    { NULL, L" 8 (RD refinement for I/P frames)" },
    { NULL, L" 9 (RD refinement for all frames)" },
    { NULL, L"10 (QP-RD)"                        },
    { NULL, L"11 (Full RD)"                      },
    { NULL, NULL }
};
const X264_OPTION_STR list_direct[] = {
    { "none",     L"none"     },
    { "spatial",  L"spatial"  },
    { "temporal", L"temporal" },
    { "auto",     L"auto"     },
    { NULL, NULL }
};
const X264_OPTION_STR list_cqm[] = {
    { "flat", NULL },
    { "jvt",  NULL },
    { NULL, NULL }
};
const X264_OPTION_STR list_input_range[] = {
    { "auto", L"auto" },
    { "pc",   L"pc" },
    { NULL, NULL }
};
const X264_OPTION_STR list_colorprim[] = {
    { "undef",     L"指定なし"  },
    { "auto",      L"自動"      },
    { "bt709",     L"bt709"     },
    { "smpte170m", L"smpte170m" },
    { "bt470m",    L"bt470m"    },
    { "bt470bg",   L"bt470bg"   },
    { "smpte240m", L"smpte240m" },
    { "film",      L"film"      },
    { NULL, NULL }
};
const X264_OPTION_STR list_transfer[] = {
    { "undef",     L"指定なし"  },
    { "auto",      L"自動"      },
    { "bt709",     L"bt709"     },
    { "smpte170m", L"smpte170m" },
    { "bt470m",    L"bt470m"    },
    { "bt470bg",   L"bt470bg"   },
    { "smpte240m", L"smpte240m" },
    { "linear",    L"linear"    },
    { "log100",    L"log100"    },
    { "log316",    L"log316"    },
    { NULL, NULL }
};
const X264_OPTION_STR list_colormatrix[] = {
    { "undef",     L"指定なし"  },
    { "auto",      L"自動"      },
    { "bt709",     L"bt709"     },
    { "smpte170m", L"smpte170m" },
    { "bt470bg",   L"bt470bg"   },
    { "smpte240m", L"smpte240m" },
    { "YCgCo",     L"YCgCo"     },
    { "fcc",       L"fcc"       },
    { "GBR",       L"GBR"       },
    { NULL, NULL }
};
const X264_OPTION_STR list_videoformat[] = {
    { "undef",     L"指定なし"  },
    { "ntsc",      L"ntsc"      },
    { "component", L"component" },
    { "pal",       L"pal"       },
    { "secam",     L"secam"     },
    { "mac",       L"mac"       },
    { NULL, NULL } 
};
const X264_OPTION_STR list_log_type[] = {
    { "none",    L"none"    },
    { "error",   L"error"   },
    { "warning", L"warning" },
    { "info",    L"info"    },
    { "debug",   L"debug"   },
    { NULL, NULL }
};
const X264_OPTION_STR list_b_pyramid[] = {
    { "none",   L"none"   },
    { "strict", L"strict" },
    { "normal", L"normal" },
    { NULL, NULL }
};
const X264_OPTION_STR list_nal_hrd[] = {
    { "none",   L"使用しない" },
    { "vbr",    L"vbr"        },
    { "cbr",    L"cbr"        },
    { NULL, NULL }
};

//色についてのオプション設定(0がデフォルトとなるよう、x264とは並び順が異なる)
//まあGUIで順番なんてどうでも…
enum {
    COLOR_MATRIX_DEFAULT   = 0,
    COLOR_MATRIX_AUTO      = 1,
    COLOR_MATRIX_HD        = 2,
    COLOR_MATRIX_SD        = 3,
};
const int COLOR_MATRIX_THRESHOLD = 720;

typedef struct {
    int x;
    int y;
} INT2;

typedef struct {
    float x;
    float y;
} FLOAT2;

//x264パラメータ構造体
//guiExで使いやすいよう、適当に
//並び順に意味はない
typedef struct {
    BOOL    use_highbit_depth;
    int     output_csp;
    int     pass;
    BOOL    slow_first_pass;
    BOOL    use_auto_npass;
    int        auto_npass;
    BOOL    nul_out;
    int     preset;
    int     tune;
    int     profile;
    int     rc_mode;
    int     bitrate;
    int     crf;
    int     qp;
    float   ip_ratio;
    float   pb_ratio;
    int     qp_min;
    int     qp_max;
    int     qp_step;
    float   qp_compress;
    int     chroma_qp_offset;
    BOOL    mbtree;
    int     rc_lookahead;
    int     vbv_bufsize;
    int     vbv_maxrate;
    int     aq_mode;
    float   aq_strength;
    FLOAT2  psy_rd;
    int     scenecut;
    int     keyint_min;
    int     keyint_max;
    BOOL    open_gop;
    BOOL    cabac;
    int     bframes;
    int     b_adapt;
    int     b_bias;
    int     b_pyramid;
    int     slice_n;
    BOOL    use_deblock;
    INT2    deblock;
    BOOL    interlaced;
    BOOL    tff;
    DWORD   mb_partition;
    BOOL    dct8x8;
    int     me;
    int     subme;
    int     me_range;
    BOOL    chroma_me;
    int     direct_mv;
    int     ref_frames;
    BOOL    mixed_ref;
    BOOL    weight_b;
    int     weight_p;
    int     noise_reduction;
    BOOL    no_fast_pskip;
    BOOL    no_dct_decimate;
    int     trellis;
    int     cqm;
    int     colormatrix;
    int     colorprim;
    int     transfer;
    int     input_range;
    INT2    sar;
    int     h264_level;
    int     videoformat;
    BOOL    aud;
    BOOL    pic_struct;
    int     nal_hrd;
    BOOL    bluray_compat;
    int     threads;
    int     sliced_threading;
    int     log_mode;
    BOOL    psnr;
    BOOL    ssim;
    BOOL    use_tcfilein;
    BOOL    use_timebase;
    INT2    timebase;
    BOOL    disable_progress;
    int     lookahead_threads;
} CONF_X264;

typedef struct {
    char *long_name;
    char *short_name;
    DWORD type;
    const X264_OPTION_STR *list;
    size_t p_offset;
} X264_OPTIONS;

enum {
    ARG_TYPE_UNKNOWN = 0,
    ARG_TYPE_LONG    = 1,
    ARG_TYPE_SHORT   = 2,
};

typedef struct {
    int arg_type;       //LONGかSHORTか
    char *option_name;  //オプション名(最初の"--"なし)
    char *value;        //オプションの値
    BOOL value_had_dQB; //前後に'"'があったか
    BOOL ret;           //構造体に読み込まれたかどうか
} CMD_ARG;

//コマンドラインの解析・生成
void set_cmd_to_conf(const char *cmd_src, CONF_X264 *conf_set);
void set_cmd_to_conf(char *cmd, CONF_X264 *conf_set, size_t cmd_len, BOOL build_not_imported_cmd);
void get_default_conf_x264(CONF_X264 *conf_set, BOOL use_highbit);
//void set_preset_to_conf(CONF_X264 *conf_set, int preset_index);
//void set_tune_to_conf(CONF_X264 *conf_set, int tune_index);
void set_profile_to_conf(CONF_X264 *conf_set, int profile_index);
void apply_presets(CONF_X264 *conf_set);
int check_profile(const CONF_X264 *conf_set);
void build_cmd_from_conf(char *cmd, size_t nSize, const CONF_X264 *conf, const void *_vid, BOOL write_all);
void set_guiEx_auto_sar(int *sar_x, int *sar_y, int width, int height);
void apply_guiEx_auto_settings(CONF_X264 *cx, int width, int height, int fps_num, int fps_den);
const X264_OPTION_STR * get_option_list(const char *option_name);
void set_ex_stg_ptr(guiEx_settings *_ex_stg);
int get_option_value(const char *cmd_src, const char *target_option_name, char *buffer, size_t nSize);
*/
#endif //_AUO_OPTIONS_H_
