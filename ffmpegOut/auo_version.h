// -----------------------------------------------------------------------------------------
// x264guiEx/x265guiEx/svtAV1guiEx/ffmpegOut/QSVEnc/NVEnc/VCEEnc by rigaya
// -----------------------------------------------------------------------------------------
// The MIT License
//
// Copyright (c) 2010-2022 rigaya
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

#ifndef _AUO_VERSION_H_
#define _AUO_VERSION_H_

#define AUO_VERSION          0,1,5,0
#define AUO_VERSION_STR      "1.05"
#define AUO_VERSION_STR_W   L"1.05"
#define AUO_NAME_R            ffmpegOut
#define AUO_NAME_WITHOUT_EXT "ffmpegOut"
#define AUO_NAME_WITHOUT_EXT_W L"ffmpegOut"
#define AUO_NAME             "ffmpegOut.auo"
#define AUO_NAME_W          L"ffmpegOut.auo"
#define AUO_FULL_NAME        "ffmpeg 出力 "
#define AUO_FULL_NAME_W     L"ffmpeg 出力 "
#define AUO_VERSION_NAME     "ffmpeg 出力 " AUO_VERSION_STR
#define AUO_VERSION_INFO     "ffmpeg 出力 " AUO_VERSION_STR " by rigaya"
#define AUO_EXT_FILTER       "All Support Formats (*.*)\0*\0"


#define ENCODER_X264 0
#define ENCODER_X265 0
#define ENCODER_SVTAV1 0
#define ENCODER_FFMPEG 1
#define ENABLE_AMP (ENCODER_X264 != 0)
#define ENABLE_TCFILE_IN (ENCODER_X264 == 0)

#ifdef DEBUG
#define VER_DEBUG   VS_FF_DEBUG
#define VER_PRIVATE VS_FF_PRIVATEBUILD
#else
#define VER_DEBUG   0
#define VER_PRIVATE 0
#endif

#define VER_STR_COMMENTS         AUO_FULL_NAME
#define VER_STR_COMPANYNAME      ""
#define VER_STR_FILEDESCRIPTION  AUO_FULL_NAME
#define VER_FILEVERSION          AUO_VERSION
#define VER_STR_FILEVERSION      AUO_VERSION_STR
#define VER_STR_INTERNALNAME     AUO_FULL_NAME
#define VER_STR_ORIGINALFILENAME AUO_NAME
#define VER_STR_LEGALCOPYRIGHT   AUO_FULL_NAME " by rigaya"
#define VER_STR_PRODUCTNAME      AUO_NAME_WITHOUT_EXT
#define VER_PRODUCTVERSION       VER_FILEVERSION
#define VER_STR_PRODUCTVERSION   VER_STR_FILEVERSION

#endif //_AUO_VERSION_H_
