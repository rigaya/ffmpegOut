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

#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") 

#include "auo.h"
#include "auo_frm.h"
#include "auo_util.h"
#include "auo_error.h"
#include "auo_version.h"
#include "auo_conf.h"
#include "auo_system.h"

#include "auo_video.h"
#include "auo_audio.h"
#include "auo_faw2aac.h"
#include "auo_mux.h"
#include "auo_encode.h"
#include "auo_runbat.h"
#include "auo_mes.h"

#if AVIUTL_TARGET_VER == 2
#include "logger2.h"
#endif

static void make_outfilename_and_set_to_oipsavefile(OUTPUT_INFO *oip, aviutlchar *outfilename, DWORD nSize, const CONF_GUIEX *conf_out);

//---------------------------------------------------------------------
//        出力プラグイン内部変数
//---------------------------------------------------------------------

static HMODULE g_dll_module = NULL;
static CONF_GUIEX g_conf = { 0 };
static SYSTEM_DATA g_sys_dat = { 0 };
static char       g_auo_filefilter[1024] = { 0 };
static aviutlchar g_auo_filefilter2[1024] = { 0 };
static aviutlchar g_auo_fullname[1024] = { 0 };
static aviutlchar g_auo_version_info[1024] = { 0 };
AuoMessages g_auo_mes;

bool func_output2( OUTPUT_INFO *oip );
bool func_config2(HWND hwnd, HINSTANCE dll_hinst);

static const aviutlchar *func_get_config_text() {
    return g_auo_version_info;
}

//---------------------------------------------------------------------
//        出力プラグイン構造体定義
//---------------------------------------------------------------------
OUTPUT_PLUGIN_TABLE output_plugin_table = {
    NULL,                         // フラグ
#if AVIUTL_TARGET_VER == 1
    AUO_FULL_NAME,                // プラグインの名前
    AUO_EXT_FILTER,               // 出力ファイルのフィルタ
    AUO_VERSION_INFO,             // プラグインの情報
    func_init,                    // DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
    func_exit,                    // DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
    func_output,                  // 出力時に呼ばれる関数へのポインタ
    func_config,                  // 出力設定のダイアログを要求された時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
    func_config_get,              // 出力設定データを取得する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
    func_config_set,              // 出力設定データを設定する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
#else
    AUO_FULL_NAME_W,              // プラグインの名前
    AUO_EXT_FILTER_W,             // 出力ファイルのフィルタ
    AUO_VERSION_INFO_W,           // プラグインの情報
    func_output2,                  // 出力時に呼ばれる関数へのポインタ
    func_config2,                  // 出力設定のダイアログを要求された時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
    func_get_config_text
#endif
};


//---------------------------------------------------------------------
//        出力プラグイン構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C OUTPUT_PLUGIN_TABLE __declspec(dllexport) * __stdcall GetOutputPluginTable( void ) {
    init_SYSTEM_DATA(&g_sys_dat);
    overwrite_aviutl_ini_auo_info();
    return &output_plugin_table;
}

#if AVIUTL_TARGET_VER == 2
EXTERN_C void __declspec(dllexport) InitializeLogger(LOG_HANDLE *logger) {
    set_aviutl2_logger(logger);
}
#endif

//---------------------------------------------------------------------
//        出力プラグイン出力関数
//---------------------------------------------------------------------
//
    //int        flag;            //    フラグ
    //                        //    OUTPUT_INFO_FLAG_VIDEO    : 画像データあり
    //                        //    OUTPUT_INFO_FLAG_AUDIO    : 音声データあり
    //                        //    OUTPUT_INFO_FLAG_BATCH    : バッチ出力中
    //int        w,h;            //    縦横サイズ
    //int        rate,scale;        //    フレームレート
    //int        n;                //    フレーム数
    //int        size;            //    １フレームのバイト数
    //int        audio_rate;        //    音声サンプリングレート
    //int        audio_ch;        //    音声チャンネル数
    //int        audio_n;        //    音声サンプリング数
    //int        audio_size;        //    音声１サンプルのバイト数
    //LPSTR    savefile;        //    セーブファイル名へのポインタ
    //void    *(*func_get_video)( int frame );
    //                        //    DIB形式(RGB24bit)の画像データへのポインタを取得します。
    //                        //    frame    : フレーム番号
    //                        //    戻り値    : データへのポインタ
    //                        //              画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効
    //void    *(*func_get_audio)( int start,int length,int *readed );
    //                        //    16bitPCM形式の音声データへのポインタを取得します。
    //                        //    start    : 開始サンプル番号
    //                        //    length    : 読み込むサンプル数
    //                        //    readed    : 読み込まれたサンプル数
    //                        //    戻り値    : データへのポインタ
    //                        //              音声データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効
    //BOOL    (*func_is_abort)( void );
    //                        //    中断するか調べます。
    //                        //    戻り値    : TRUEなら中断
    //BOOL    (*func_rest_time_disp)( int now,int total );
    //                        //    残り時間を表示させます。
    //                        //    now        : 処理しているフレーム番号
    //                        //    total    : 処理する総フレーム数
    //                        //    戻り値    : TRUEなら成功
    //int        (*func_get_flag)( int frame );
    //                        //    フラグを取得します。
    //                        //    frame    : フレーム番号
    //                        //    戻り値    : フラグ
    //                        //  OUTPUT_INFO_FRAME_FLAG_KEYFRAME        : キーフレーム推奨
    //                        //  OUTPUT_INFO_FRAME_FLAG_COPYFRAME    : コピーフレーム推奨
    //BOOL    (*func_update_preview)( void );
    //                        //    プレビュー画面を更新します。
    //                        //    最後にfunc_get_videoで読み込まれたフレームが表示されます。
    //                        //    戻り値    : TRUEなら成功
    //void    *(*func_get_video_ex)( int frame,DWORD format );
    //                        //    DIB形式の画像データを取得します。
    //                        //    frame    : フレーム番号
    //                        //    format    : 画像フォーマット( NULL = RGB24bit / 'Y''U''Y''2' = YUY2 / 'Y''C''4''8' = PIXEL_YC )
    //                        //              ※PIXEL_YC形式 は YUY2フィルタモードでは使用出来ません。
    //                        //    戻り値    : データへのポインタ
    //                        //              画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効

BOOL func_init() {
    return TRUE;
}

BOOL func_exit() {
    delete_SYSTEM_DATA(&g_sys_dat);
    return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        g_dll_module = hModule;
        if (AVIUTL_TARGET_VER == 2) {
            func_init();
        }
        break;
    case DLL_PROCESS_DETACH:
        if (AVIUTL_TARGET_VER == 2) {
			if (lpReserved != nullptr) break;
            func_exit();
        }
        break;
    }
    return TRUE;
}

BOOL func_output( OUTPUT_INFO *oip ) 
{
    AUO_RESULT ret = AUO_RESULT_SUCCESS;
    static const encode_task task[3][2] = { { video_output, audio_output }, { audio_output, video_output }, { audio_output_parallel, video_output }  };
    PRM_ENC pe = { 0 };
    CONF_GUIEX conf_out = { 0 };
    const DWORD tm_start_enc = timeGetTime();
    TCHAR default_stg_file[MAX_PATH_LEN] = { 0 };

    //データの初期化
    init_SYSTEM_DATA(&g_sys_dat);
    if (!g_sys_dat.exstg->get_init_success()) return FALSE;

    const bool conf_not_initialized = memcmp(&conf_out, &g_conf, sizeof(g_conf)) == 0;
    if (conf_not_initialized) {
        PathCombine(default_stg_file, g_sys_dat.exstg->s_local.stg_dir, CONF_LAST_OUT);
        if (!PathFileExists(default_stg_file)
            || guiEx_config::load_guiEx_conf(&g_conf, default_stg_file) != CONF_ERROR_NONE) {
            //前回出力した設定ファイルがない場合は、デフォルト設定をロード
            init_CONF_GUIEX(&g_conf, FALSE);
            memset(default_stg_file, 0, sizeof(default_stg_file));
        }
    }
    conf_out = g_conf;

    init_enc_prm(&conf_out, &pe, oip, &g_sys_dat);

    //出力拡張子の設定
    aviutlchar outfilename[MAX_PATH_LEN];
    make_outfilename_and_set_to_oipsavefile(oip, outfilename, _countof(outfilename), &conf_out);

    //ログウィンドウを開く
    open_log_window(oip, &g_sys_dat, 1, (conf_out.enc.use_auto_npass) ? conf_out.enc.auto_npass : 1);
    if (conf_not_initialized) {
        warning_conf_not_initialized(default_stg_file);
    }
    set_prevent_log_close(TRUE); //※1 start

    //各種設定を行う
    set_enc_prm(&conf_out, &pe, oip, &g_sys_dat);
    pe.h_p_aviutl = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId()); //※2 start

    //チェックを行い、エンコード可能ならエンコードを開始する
    if (check_output(&conf_out, oip, &pe, g_sys_dat.exstg)) {

        //ret |= run_bat_file(&conf_out, oip, &pe, &sys_dat, RUN_BAT_BEFORE);

        const auto audio_encode_timing = (conf_out.aud.use_internal) ? 2 : conf_out.aud.ext.audio_encode_timing;
        for (int i = 0; !ret && i < 2; i++)
            ret |= task[audio_encode_timing][i](&conf_out, oip, &pe, &g_sys_dat);

        //if (!ret) ret |= mux(&conf_out, oip, &pe, &sys_dat);

        ret |= move_temporary_files(&conf_out, &pe, &g_sys_dat, oip, ret);

        write_log_auo_enc_time(g_auo_mes.get(AUO_GUIEX_TOTAL_TIME), timeGetTime() - tm_start_enc);

    } else {
        ret |= AUO_RESULT_ERROR;
    }

    if (ret & AUO_RESULT_ABORT) info_encoding_aborted();

    warn_video_length(oip);

    CloseHandle(pe.h_p_aviutl); //※2 end
    set_prevent_log_close(FALSE); //※1 end

    const bool error_or_abort = ret & (AUO_RESULT_ERROR | AUO_RESULT_ABORT);
    auto_save_log(&conf_out, oip, &pe, &g_sys_dat, is_aviutl2() && error_or_abort); //※1 end のあとで行うこと

    //if (!error_or_abort)
     //   ret |= run_bat_file(&conf_out, oip, &pe, &g_sys_dat, RUN_BAT_AFTER_PROCESS);

    log_process_events();
    // エラーが発生しなかった場合は設定を保存
    if (ret == AUO_RESULT_SUCCESS) {
        memset(default_stg_file, 0, sizeof(default_stg_file));
        PathCombine(default_stg_file, g_sys_dat.exstg->s_local.stg_dir, CONF_LAST_OUT);
        guiEx_config::save_guiEx_conf(&conf_out, default_stg_file);
    }
    free_enc_prm(&pe);

    return (ret & AUO_RESULT_ERROR) ? FALSE : TRUE;
}

#if AVIUTL_TARGET_VER == 2
void set_window_title_override(WindowTitleOverride *window_title_override);

bool func_output2( OUTPUT_INFO *oip ) {
    WindowTitleOverride window_title_override;
    set_window_title_override(&window_title_override);
    bool ret = false;
    try {
        ret = func_output(oip) != FALSE;
    } catch (...) {
        ret = false;
    }
    set_window_title_override(nullptr);
    return ret;
}
#endif

//---------------------------------------------------------------------
//        出力プラグイン設定関数
//---------------------------------------------------------------------
//以下部分的にwarning C4100を黙らせる
//C4100 : 引数は関数の本体部で 1 度も参照されません。
#pragma warning( push )
#pragma warning( disable: 4100 )
BOOL func_config(HWND hwnd, HINSTANCE dll_hinst)
{
    init_SYSTEM_DATA(&g_sys_dat);
    if (g_sys_dat.exstg->get_init_success())
        ShowfrmConfig(&g_conf, &g_sys_dat);
    return TRUE;
}

bool func_config2(HWND hwnd, HINSTANCE dll_hinst) {
    return func_config(hwnd, dll_hinst) != FALSE;
}
#pragma warning( pop )

// 1回目、data=null,size=0で呼ばれ、そのとき返したサイズでメモリが確保されてもう一回呼ばれる
int func_config_get(void *data, int size) {
    std::string json_str = guiEx_config::conf_to_json(&g_conf, 0);
    const int json_len = (int)json_str.length();
    const int data_len = json_len + (int)strlen(CONF_NAME_JSON) + 1;
    if (data && size >= data_len) {
        memset(data, 0, size);
        strcpy_s((char *)data, size, CONF_NAME_JSON);
        strcpy_s((char *)data + strlen(CONF_NAME_JSON), size - strlen(CONF_NAME_JSON), json_str.c_str());
    }
    return data_len;
}

int func_config_set(void *data,int size) {
    init_SYSTEM_DATA(&g_sys_dat);
    if (!g_sys_dat.exstg->get_init_success(TRUE)) {
        return NULL;
    }
    init_CONF_GUIEX(&g_conf, FALSE);
    if (size >= (int)strlen(CONF_NAME_JSON)
        && strncmp(CONF_NAME_JSON, (char *)data, strlen(CONF_NAME_JSON)) == 0) {
        std::string json_str((char *)data + strlen(CONF_NAME_JSON));
        if (guiEx_config::json_to_conf(&g_conf, json_str)) {
            g_conf.header.size_all = CONF_INITIALIZED;
            return size;
        }
    } else if (size == sizeof(CONF_GUIEX_OLD)) {
        auto json_str = guiEx_config::old_conf_to_json((CONF_GUIEX_OLD *)data);
        if (guiEx_config::json_to_conf(&g_conf, json_str)) {
            g_conf.header.size_all = CONF_INITIALIZED;
            return size;
        }
    }
    memset(data, 0, size);
    return NULL;
}


//---------------------------------------------------------------------
//        ffmpegOutのその他の関数
//---------------------------------------------------------------------

void init_SYSTEM_DATA(SYSTEM_DATA *_sys_dat) {
    if (_sys_dat->init)
        return;
    get_auo_path(_sys_dat->auo_path, _countof(_sys_dat->auo_path));
    get_aviutl_dir(_sys_dat->aviutl_dir, _countof(_sys_dat->aviutl_dir));
    _sys_dat->exstg = new guiEx_settings();
    load_lng(g_sys_dat.exstg->get_lang());
    _sys_dat->init = TRUE;
}
void delete_SYSTEM_DATA(SYSTEM_DATA *_sys_dat) {
    if (_sys_dat->init) {
        delete _sys_dat->exstg;
        _sys_dat->exstg = NULL;
    }
    _sys_dat->init = FALSE;
}
void get_default_conf(CONF_GUIEX *conf) {
    conf->mux.disable_mkvext = TRUE;
    conf->mux.disable_mp4ext = TRUE;
    conf->aud.ext.encoder = g_sys_dat.exstg->s_local.default_audio_encoder_ext;
    conf->aud.in.encoder = g_sys_dat.exstg->s_local.default_audio_encoder_in;
    conf->aud.use_internal = g_sys_dat.exstg->s_local.default_audenc_use_in;
#if ENCODER_QSVENC || ENCODER_NVENC || ENCODER_VCEENC || ENCODER_FFMPEG
    conf->mux.use_internal = TRUE;
#else
    conf->mux.use_internal = FALSE;
#endif
#if ENCODER_FFMPEG
    conf->aud.ext.audio_encode_timing = 1;
    conf->aud.in.audio_encode_timing = 2;
    conf->enc.audio_input = TRUE;
    _tcscpy_s(conf->enc.outext, _countof(conf->enc.outext), _T(".mp4"));
#endif
    if (conf->aud.in.encoder < g_sys_dat.exstg->s_aud_int_count) {
        const AUDIO_SETTINGS *aud_stg_in = &g_sys_dat.exstg->s_aud_int[conf->aud.in.encoder];
        conf->aud.in.bitrate = aud_stg_in->mode[conf->aud.in.enc_mode].bitrate_default;
    }
    if (conf->aud.ext.encoder < g_sys_dat.exstg->s_aud_ext_count) {
        const AUDIO_SETTINGS *aud_stg_ext = &g_sys_dat.exstg->s_aud_ext[conf->aud.ext.encoder];
        conf->aud.ext.bitrate = aud_stg_ext->mode[conf->aud.ext.enc_mode].bitrate_default;
    }
    conf->enc.auto_npass = 2;
}
#pragma warning( push )
#pragma warning( disable: 4100 )
void init_CONF_GUIEX(CONF_GUIEX *conf, BOOL use_highbit) {
    ZeroMemory(conf, sizeof(CONF_GUIEX));
    guiEx_config::write_conf_header(&conf->header);
    get_default_conf(conf);
    conf->header.size_all = CONF_INITIALIZED;
}
void write_log_line_fmt(int log_type_index, const wchar_t *format, ...) {
    va_list args;
    int len;
    wchar_t *buffer;
    va_start(args, format);
    len = _vscwprintf(format, args) + 1; // _vscprintf doesn't count terminating '\0'
    buffer = (wchar_t *)malloc(len * sizeof(buffer[0]));
    vswprintf_s(buffer, len, format, args);
    write_log_line(log_type_index, buffer);
    free(buffer);
}
#pragma warning( pop )
void write_log_auo_line_fmt(int log_type_index, const wchar_t *format, ...) {
    va_list args;
    int len;
    wchar_t *buffer;
    va_start(args, format);
    len = _vscwprintf(format, args) // _vscprintf doesn't count
        + 1; // terminating '\0'
    buffer = (wchar_t *)malloc(len * sizeof(buffer[0]));
    vswprintf_s(buffer, len, format, args);
    write_log_auo_line(log_type_index, buffer);
    free(buffer);
}
//エンコード時間の表示
void write_log_auo_enc_time(const wchar_t *mes, DWORD time) {
    time = ((time + 50) / 100) * 100; //四捨五入
    write_log_auo_line_fmt(LOG_INFO, L"%s : %d%s%2d%s%2d.%1d%s",
        mes,
        time / (60 * 60 * 1000), g_auo_mes.get(AUO_GUIEX_TIME_HOUR),
        (time % (60 * 60 * 1000)) / (60 * 1000), g_auo_mes.get(AUO_GUIEX_TIME_MIN),
        (time % (60 * 1000)) / 1000,
        ((time % 1000)) / 100, g_auo_mes.get(AUO_GUIEX_TIME_SEC));
}

#if AVIUTL_TARGET_VER == 1
static std::string aviutlchar_to_string(const aviutlchar *str) { return std::string(str); }
static std::wstring aviutlchar_to_wstring(const aviutlchar *str) { return char_to_wstring(str, CP_THREAD_ACP); }
static std::basic_string<aviutlchar> string_to_aviutlchar(const std::string &str) { return str; }
static std::basic_string<aviutlchar> wstring_to_aviutlchar(const std::wstring &str) { return wstring_to_string(str, CP_THREAD_ACP); }
#define aviutlcharcpy_s strcpy_s
#define aviutlcharlen strlen
#define aviutlchar_PathFindExtension PathFindExtensionA
#else
static std::wstring aviutlchar_to_wstring(const aviutlchar *str) { return std::wstring(str); }
static std::string aviutlchar_to_string(const aviutlchar *str) { return wstring_to_string(str, CP_THREAD_ACP); }
static std::basic_string<aviutlchar> string_to_aviutlchar(const std::string &str) { return char_to_wstring(str, CP_THREAD_ACP); }
static std::basic_string<aviutlchar> wstring_to_aviutlchar(const std::wstring &str) { return str; }
#define aviutlcharcpy_s wcscpy_s
#define aviutlcharlen wcslen
#define aviutlchar_PathFindExtension PathFindExtensionW
#endif
static void make_outfilename_and_set_to_oipsavefile(OUTPUT_INFO *oip, aviutlchar *outfilename, DWORD nSize, const CONF_GUIEX *conf_out) {
    aviutlcharcpy_s(outfilename, nSize, oip->savefile);
    if (str_has_char(conf_out->enc.outext)) {
        aviutlchar *ptr_ext = aviutlchar_PathFindExtension(outfilename);
        if (ptr_ext == NULL || aviutlchar_to_wstring(ptr_ext) != conf_out->enc.outext) ptr_ext = outfilename + aviutlcharlen(outfilename);
        aviutlcharcpy_s(ptr_ext, nSize - (int)(ptr_ext - outfilename), wstring_to_aviutlchar(conf_out->enc.outext).c_str());
    }
    oip->savefile = outfilename;
}

template <size_t size>
void get_aviutl_ini_file(char(&ini_file)[size]) {
    TCHAR ini_file_tstr[size];
    get_aviutl_dir(ini_file_tstr, _countof(ini_file_tstr));
    PathAddBackSlashLong(ini_file_tstr);
    _tcscat_s(ini_file_tstr, _countof(ini_file_tstr), _T("aviutl.ini"));
    if (!canbe_converted_to(ini_file_tstr, CP_THREAD_ACP)) {
        // CP_THREAD_ACP = sjisに変換できない場合は、相対パスにする
        TCHAR ini_file_relative[size];
        GetRelativePathTo(ini_file_relative, _countof(ini_file_relative), ini_file_tstr, NULL);
        _tcscpy_s(ini_file_tstr, ini_file_relative);
    }
    strcpy_s(ini_file, size, tchar_to_string(ini_file_tstr, CP_THREAD_ACP).c_str());
}

void overwrite_aviutl_ini_auo_info() {
    const auto auo_full_name = std::wstring(g_auo_mes.get(AUO_GUIEX_FULL_NAME));
    if (auo_full_name.length() > 0 && (auo_full_name != aviutlchar_to_wstring(output_plugin_table.name) || aviutlcharlen(g_auo_version_info) == 0)) {
        aviutlcharcpy_s(g_auo_fullname, wstring_to_aviutlchar(auo_full_name).c_str());
        output_plugin_table.name = g_auo_fullname;
        std::wstring auo_version_info;
        if (auo_full_name != AUO_NAME_WITHOUT_EXT_W) {
            auo_version_info = std::wstring(auo_full_name) + std::wstring(L" (" AUO_NAME_WITHOUT_EXT_W L") " AUO_VERSION_STR_W);
        } else {
            auo_version_info = AUO_NAME_WITHOUT_EXT_W L" " AUO_VERSION_STR_W;
        }
        aviutlcharcpy_s(g_auo_version_info, wstring_to_aviutlchar(auo_version_info).c_str());
        output_plugin_table.information = g_auo_version_info;
#if AVIUTL_TARGET_VER == 1
        char ini_file[1024];
        get_aviutl_ini_file(ini_file);
        WritePrivateProfileStringA(AUO_NAME, "name", output_plugin_table.name, ini_file);
        WritePrivateProfileStringA(AUO_NAME, "information", output_plugin_table.information, ini_file);
#endif
    }
}

std::wstring get_last_out_stg_appendix() {
    const auto appendix = g_auo_mes.get(AUO_CONF_LAST_OUT_STG);
    return (wcslen(appendix) > 0) ? appendix : CONF_LAST_OUT;
}

const aviutlchar *get_auo_version_info() {
    return output_plugin_table.information;
}

static int getEmbeddedResource(void **data, const TCHAR *name, const TCHAR *type, HMODULE hModule) {
    *data = nullptr;
    //埋め込みデータを使用する
    if (hModule == NULL) {
        hModule = GetModuleHandle(NULL);
    }
    if (hModule == NULL) {
        return 0;
    }
    HRSRC hResource = FindResource(hModule, name, type);
    if (hResource == NULL) {
        return 0;
    }
    HGLOBAL hResourceData = LoadResource(hModule, hResource);
    if (hResourceData == NULL) {
        return 0;
    }
    *data = LockResource(hResourceData);
    return (int)SizeofResource(hModule, hResource);
}

int load_lng(const TCHAR *lang) {
    if (g_auo_mes.isLang(lang)) {
        return 0;
    }
    const TCHAR *resource = list_auo_languages[0].resouce;
    if (lang && str_has_char(lang)) {
        TCHAR auo_path[MAX_PATH_LEN];
        get_auo_path(auo_path, _countof(auo_path));
        TCHAR auo_dir[MAX_PATH_LEN];
        wcscpy_s(auo_dir, auo_path);
        PathRemoveFileSpecFixed(auo_dir);
        TCHAR lng_path[MAX_PATH_LEN];
        PathCombineLong(lng_path, _countof(lng_path), auo_dir, lang);
        if (PathFileExists(lng_path)) {
            return g_auo_mes.read(lng_path);
        }
        for (const auto& auo_lang : list_auo_languages) {
            if (wcsicmp(auo_lang.code, lang) == 0) {
                resource = auo_lang.resouce;
                break;
            }
        }
    }
    char *data = nullptr;
    int size = getEmbeddedResource((void **)&data, resource, _T("EXE_DATA"), g_dll_module);
    if (size == 0) {
        return 1;
    }
    if (g_auo_mes.read(lang, data, size)) {
        return 1;
    }
    return 0;
}
