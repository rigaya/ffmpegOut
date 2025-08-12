// -----------------------------------------------------------------------------------------
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
#include <string>
#include <limits>

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
    OUT_CSP_RGBA_16,
};

enum {
    YC48_COLMAT_CONV_AUTO,
    YC48_COLMAT_CONV_NONE,
    YC48_COLMAT_CONV_BT709,
};

//x264のinput-cspとして使用するもの
//OUT_CSP_NV12, OUT_CSP_YUV444, OUT_CSP_RGB に合わせる
static const TCHAR * const specify_csp[] = {
    _T("nv12"),    //OUT_CSP_NV12
    _T("yuyv422"), //OUT_CSP_YUY2
    _T("yuv444p"), //OUT_CSP_YUV444
    _T("p010le"),
    _T("yuv444p16le"),
    _T("bgr24"),   //OUT_CSP_RGB
    _T("bgra")     //OUT_CSP_RGBA
};
//文字列を引数にとるオプションの引数リスト
//OUT_CSP_NV12, OUT_CSP_YUV444, OUT_CSP_RGB に合わせる
const ENC_OPTION_STR list_output_csp[] = {
    { _T("nv12"),        AUO_MES_UNKNOWN, L"yuv420" },
    { _T("yuyv422"),     AUO_MES_UNKNOWN, L"yuv422" },
    { _T("yuv444p"),     AUO_MES_UNKNOWN, L"yuv444" },
    { _T("p010le"),      AUO_MES_UNKNOWN, L"yuv420(16bit)" },
    { _T("yuv444p16le"), AUO_MES_UNKNOWN, L"yuv444(16bit)" },
    { _T("bgr24"),       AUO_MES_UNKNOWN, L"rgb"  },
    { _T("bgra"),        AUO_MES_UNKNOWN, L"rgba"  },
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

#endif //_AUO_OPTIONS_H_
