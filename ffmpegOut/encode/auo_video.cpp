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

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#pragma comment(lib, "user32.lib") //WaitforInputIdle
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <limits.h>
#include <mmsystem.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <vector>
#include <set>

#include "output.h"
#include "vphelp_client.h"

#pragma warning( push )
#pragma warning( disable: 4127 )
#include "afs_client.h"
#pragma warning( pop )

#include "auo.h"
#include "auo_frm.h"
#include "auo_pipe.h"
#include "auo_error.h"
#include "auo_conf.h"
#include "auo_util.h"
#include "auo_convert.h"
#include "auo_system.h"
#include "auo_version.h"
#include "auo_chapter.h"
#include "auo_mes.h"
#include "auo_options.h"

#include "auo_encode.h"
#include "auo_video.h"
#include "auo_audio_parallel.h"
#include "cpu_info.h"

typedef struct video_output_thread_t {
    CONVERT_CF_DATA *pixel_data;
    FILE *f_out;
    BOOL abort;
    HANDLE thread;
    HANDLE he_out_start;
    HANDLE he_out_fin;
    int repeat;
} video_output_thread_t;

static const char * specify_input_csp(int output_csp) {
    return specify_csp[output_csp];
}

int get_aviutl_color_format(int use_highbit, int output_csp) {
    //Aviutlからの入力に使用するフォーマット
    switch (output_csp) {
        case OUT_CSP_P010:
        case OUT_CSP_YUV444:
        case OUT_CSP_YUV444_16:
            return CF_YC48;
        case OUT_CSP_RGB:
            return CF_RGB;
        case OUT_CSP_RGBA:
            return CF_RGBA;
        case OUT_CSP_NV12:
        case OUT_CSP_NV16:
        case OUT_CSP_YUY2:
        default:
            return (use_highbit) ? CF_YC48 : CF_YUY2;
    }
}

static int calc_input_frame_size(int width, int height, int color_format, int& buf_size) {
    width = (color_format == CF_RGB) ? (width+3) & ~3 : (color_format == CF_RGBA) ? width : (width+1) & ~1;
    //widthが割り切れない場合、多めにアクセスが発生するので、そのぶんを確保しておく
    const DWORD pixel_size = COLORFORMATS[color_format].size;
    const DWORD simd_check = get_availableSIMD();
    const DWORD align_size = (simd_check & AUO_SIMD_SSE2) ? ((simd_check & AUO_SIMD_AVX2) ? 64 : 32) : 1;
#define ALIGN_NEXT(i, align) (((i) + (align-1)) & (~(align-1))) //alignは2の累乗(1,2,4,8,16,32...)
    buf_size = ALIGN_NEXT(width * height * pixel_size + (ALIGN_NEXT(width, align_size / pixel_size) - width) * 2 * pixel_size, align_size);
#undef ALIGN_NEXT
    return width * height * pixel_size;
}

BOOL setup_afsvideo(const OUTPUT_INFO *oip, const SYSTEM_DATA *sys_dat, CONF_GUIEX *conf, PRM_ENC *pe) {
    //すでに初期化してある または 必要ない
    if (pe->afs_init || pe->video_out_type == VIDEO_OUTPUT_DISABLED || !conf->vid.afs)
        return TRUE;

    const int color_format = get_aviutl_color_format(conf->enc.use_highbit_depth ? 16 : 8, conf->enc.output_csp);
    int buf_size;
    const int frame_size = calc_input_frame_size(oip->w, oip->h, color_format, buf_size);
    //Aviutl(自動フィールドシフト)からの映像入力
    if (afs_vbuf_setup((OUTPUT_INFO *)oip, conf->vid.afs, frame_size, buf_size, COLORFORMATS[color_format].FOURCC)) {
        pe->afs_init = TRUE;
        return TRUE;
    } else if (conf->vid.afs && sys_dat->exstg->s_local.auto_afs_disable) {
        afs_vbuf_release(); //一度解放
        warning_auto_afs_disable();
        conf->vid.afs = FALSE;
        //再度使用するmuxerをチェックする
        pe->muxer_to_be_used = check_muxer_to_be_used(conf, pe->video_out_type, (oip->flag & OUTPUT_INFO_FLAG_AUDIO) != 0);
        return TRUE;
    }
    //エラー
    error_afs_setup(conf->vid.afs, sys_dat->exstg->s_local.auto_afs_disable);
    return FALSE;
}

void close_afsvideo(PRM_ENC *pe) {
    if (!pe->afs_init || pe->video_out_type == VIDEO_OUTPUT_DISABLED)
        return;

    afs_vbuf_release();

    pe->afs_init = FALSE;
}

static AUO_RESULT tcfile_out(int *jitter, int frame_n, double fps, BOOL afs, const PRM_ENC *pe) {
    AUO_RESULT ret = AUO_RESULT_SUCCESS;
    char auotcfile[MAX_PATH_LEN];
    FILE *tcfile = NULL;

    if (afs)
        fps *= 4; //afsなら4倍精度
    const double tm_multi = 1000.0 / fps;

    //ファイル名作成
    apply_appendix(auotcfile, _countof(auotcfile), pe->temp_filename, pe->append.tc);

    if (NULL != fopen_s(&tcfile, auotcfile, "wb")) {
        ret |= AUO_RESULT_ERROR; warning_auo_tcfile_failed();
    } else {
        fprintf(tcfile, "# timecode format v2\r\n");
        if (afs) {
            int time_additional_frame = 0;
            //オーディオディレイカットのために映像フレームを追加したらその分を考慮したタイムコードを出力する
            if (pe->delay_cut_additional_vframe) {
                //24fpsと30fpsどちらに近いかを考慮する
                const int multi_for_additional_vframe = 4 + !!fps_after_afs_is_24fps(frame_n, pe);
                for (int i = 0; i < pe->delay_cut_additional_vframe; i++)
                    fprintf(tcfile, "%.6lf\r\n", i * multi_for_additional_vframe * tm_multi);

                time_additional_frame = pe->delay_cut_additional_vframe * multi_for_additional_vframe;
            }
            for (int i = 0; i < frame_n; i++)
                if (jitter[i] != DROP_FRAME_FLAG)
                    fprintf(tcfile, "%.6lf\r\n", (i * 4 + jitter[i] + time_additional_frame) * tm_multi);
        } else {
            frame_n += pe->delay_cut_additional_vframe;
            for (int i = 0; i < frame_n; i++)
                fprintf(tcfile, "%.6lf\r\n", i * tm_multi);
        }
        fclose(tcfile);
    }
    return ret;
}

//cmdexのうち、guiから発行されるオプションとの衝突をチェックして、読み取られなかったコマンドを追加する
static void append_cmdex(char *cmd, size_t nSize, const char *cmdex) {
    const size_t cmd_len = strlen(cmd);

    sprintf_s(cmd + cmd_len, nSize - cmd_len, " %s", cmdex);

    //改行のチェックのみ行う
    replace_cmd_CRLF_to_Space(cmd + cmd_len + 1, nSize - cmd_len - 1);
}

static void build_full_cmd(char *cmd, size_t nSize, const CONF_GUIEX *conf, const OUTPUT_INFO *oip, const PRM_ENC *pe, const SYSTEM_DATA *sys_dat, const char *input) {
    CONF_GUIEX prm;
    memcpy(&prm, conf, sizeof(CONF_GUIEX));
    //共通置換を実行
    cmd_replace(prm.vid.cmdex, sizeof(prm.vid.cmdex), pe, sys_dat, conf, oip);
    cmd_replace(prm.vid.incmd, sizeof(prm.vid.incmd), pe, sys_dat, conf, oip);
    //コマンドライン作成
    strcpy_s(cmd, nSize, " -y -f rawvideo");
    //解像度情報追加(-s)
    if (strcmp(input, PIPE_FN) == NULL)
        sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -s %dx%d", oip->w, oip->h);
    //rawの形式情報追加
    sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -pix_fmt %s", specify_input_csp(prm.enc.output_csp));
    //fps
    int gcd = get_gcd(oip->rate, oip->scale);
    sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -r %d/%d", oip->rate / gcd, oip->scale / gcd);
    //入力追加オプション
    if (strlen(prm.vid.incmd) > 0) sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " %s", prm.vid.incmd);
    //入力ファイル
    sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -i \"%s\"", input);
    //音声入力
    if ((oip->flag & OUTPUT_INFO_FLAG_AUDIO) && conf->enc.audio_input) {
        if (!(conf->enc.use_auto_npass && pe->current_x264_pass == 1)) {
            if (conf->aud.use_internal) {
                if_valid_wait_for_single_object(pe->aud_parallel.he_vid_start, INFINITE);
                for (int i_aud = 0; i_aud < pe->aud_count; i_aud++) {
                    char pipename[MAX_PATH_LEN];
                    get_audio_pipe_name(pipename, _countof(pipename), i_aud);
                    sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -i \"%s\"", pipename);
                }
                if (pe->aud_count > 0) {
                    const CONF_AUDIO_BASE *cnf_aud = &conf->aud.in;
                    const AUDIO_SETTINGS *aud_stg = &sys_dat->exstg->s_aud_int[cnf_aud->encoder];
                    if (sys_dat->exstg->is_faw(aud_stg)) {
                        sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -c:a copy");
                    } else if (strcmp(aud_stg->codec, "custom") != 0) { // custom 選択時は特に何も指定しない
                        sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -c:a %s", aud_stg->codec);
                        if (aud_stg->mode[cnf_aud->enc_mode].bitrate) {
                            sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -b:a %dk", cnf_aud->bitrate);
                        }
                    }
                }
            } else {
                char tmp[MAX_PATH_LEN];
                get_aud_filename(tmp, _countof(tmp), pe, 0);
                sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -i \"%s\"", tmp);
            }
        }
    }
    //コマンドライン追加
    append_cmdex(cmd, nSize, prm.vid.cmdex);
    /////////  vframesを指定すると音声の最後の数秒が切れる場合があるようなので、vfrmaesは指定しない /////////
    //1pass目でafsでない、-vframesがなければ-vframesを指定
    //if ((!prm.vid.afs) && strstr(cmd, "-vframes") == NULL)
    //	sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -vframes %d", oip->n - pe->drop_count);
    //自動2pass
    if (conf->enc.use_auto_npass)
        sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " -pass %d", pe->current_x264_pass);
    //出力ファイル
    sprintf_s(cmd + strlen(cmd), nSize - strlen(cmd), " \"%s\"", pe->temp_filename);
}

static void set_pixel_data(CONVERT_CF_DATA *pixel_data, const CONF_GUIEX *conf, int w, int h) {
    const int byte_per_pixel = (conf->enc.use_highbit_depth) ? sizeof(short) : sizeof(BYTE);
    ZeroMemory(pixel_data, sizeof(CONVERT_CF_DATA));
    switch (conf->enc.output_csp) {
        case OUT_CSP_NV16: //nv16 (YUV422)
            pixel_data->count = 2;
            pixel_data->size[0] = w * h * byte_per_pixel;
            pixel_data->size[1] = pixel_data->size[0];
            break;
        case OUT_CSP_YUY2: //yuy2 (YUV422)
            pixel_data->count = 1;
            pixel_data->size[0] = w * h * byte_per_pixel * 2;
            break;
        case OUT_CSP_YUV444: //i444 (YUV444 planar)
        case OUT_CSP_YUV444_16:
            pixel_data->count = 3;
            pixel_data->size[0] = w * h * byte_per_pixel;
            pixel_data->size[1] = pixel_data->size[0];
            pixel_data->size[2] = pixel_data->size[0];
            break;
        case OUT_CSP_RGB: //RGB packed
            pixel_data->count = 1;
            pixel_data->size[0] = w * h * 3 * sizeof(BYTE); //8bit only
            break;
        case OUT_CSP_RGBA: //RGBA packed
            pixel_data->count = 1;
            pixel_data->size[0] = w * h * 4 * sizeof(BYTE); //8bit only
            break;
        case OUT_CSP_NV12: //nv12 (YUV420)
        case OUT_CSP_P010:
        default:
            pixel_data->count = 2;
            pixel_data->size[0] = w * h * byte_per_pixel;
            pixel_data->size[1] = pixel_data->size[0] / 2;
            break;
    }
    //サイズの総和計算
    for (int i = 0; i < pixel_data->count; i++)
        pixel_data->total_size += pixel_data->size[i];
}

static inline void check_enc_priority(HANDLE h_aviutl, HANDLE h_x264, DWORD priority) {
    if (priority == AVIUTLSYNC_PRIORITY_CLASS)
        priority = GetPriorityClass(h_aviutl);
    SetPriorityClass(h_x264, priority);
}

//並列処理時に音声データを取得する
AUO_RESULT aud_parallel_task(const OUTPUT_INFO *oip, PRM_ENC *pe, BOOL use_internal) {
    AUO_RESULT ret = AUO_RESULT_SUCCESS;
    AUD_PARALLEL_ENC *aud_p = &pe->aud_parallel; //長いんで省略したいだけ
    if (aud_p->th_aud && video_is_last_pass(pe)) {
        //---   排他ブロック 開始  ---> 音声スレッドが止まっていなければならない
        if (aud_p->he_vid_start && WaitForSingleObject(aud_p->he_vid_start, (use_internal) ? 0 : INFINITE) == WAIT_OBJECT_0) {
            if (aud_p->he_vid_start && aud_p->get_length) {
                DWORD required_buf_size = aud_p->get_length * (DWORD)oip->audio_size;
                if (aud_p->buf_max_size < required_buf_size) {
                    //メモリ不足なら再確保
                    if (aud_p->buffer) free(aud_p->buffer);
                    aud_p->buf_max_size = required_buf_size;
                    if (NULL == (aud_p->buffer = malloc(aud_p->buf_max_size)))
                        aud_p->buf_max_size = 0; //ここのmallocエラーは次の分岐でAUO_RESULT_ERRORに設定
                }
                void *data_ptr = NULL;
                if (NULL == aud_p->buffer ||
                    NULL == (data_ptr = oip->func_get_audio(aud_p->start, aud_p->get_length, &aud_p->get_length))) {
                    ret = AUO_RESULT_ERROR; //mallocエラーかget_audioのエラー
                } else {
                    //自前のバッファにコピーしてdata_ptrが破棄されても良いようにする
                    memcpy(aud_p->buffer, data_ptr, aud_p->get_length * oip->audio_size);
                }
                //すでにTRUEなら変更しないようにする
                aud_p->abort |= oip->func_is_abort();
            }
            flush_audio_log();
            if_valid_set_event(aud_p->he_aud_start);
            //---   排他ブロック 終了  ---> 音声スレッドを開始
        }
        //内蔵エンコーダを使う場合、エンコーダが終了していたら意味がないので終了する
        if (use_internal && pe->h_p_videnc && WaitForSingleObject(pe->h_p_videnc, 0) != WAIT_TIMEOUT) {
            aud_p->abort |= TRUE;
        }
    }
    return ret;
}

//音声処理をどんどん回して終了させる
static AUO_RESULT finish_aud_parallel_task(const OUTPUT_INFO *oip, PRM_ENC *pe, BOOL use_internal, AUO_RESULT vid_ret) {
    //エラーが発生していたら音声出力ループをとめる
    pe->aud_parallel.abort |= (vid_ret != AUO_RESULT_SUCCESS);
    if (pe->aud_parallel.th_aud && (video_is_last_pass(pe) || pe->aud_parallel.abort)) {
        write_log_auo_line(LOG_INFO, g_auo_mes.get(AUO_VIDEO_AUDIO_PROC_WAIT));
        set_window_title(g_auo_mes.get(AUO_VIDEO_AUDIO_PROC_WAIT), PROGRESSBAR_MARQUEE);
        while (pe->aud_parallel.he_vid_start)
            vid_ret |= aud_parallel_task(oip, pe, use_internal);
        set_window_title(g_auo_mes.get(AUO_GUIEX_FULL_NAME), PROGRESSBAR_DISABLED);
    }
    return vid_ret;
}

//並列処理スレッドの終了を待ち、終了コードを回収する
static AUO_RESULT exit_audio_parallel_control(const OUTPUT_INFO *oip, PRM_ENC *pe, BOOL use_internal, AUO_RESULT vid_ret) {
    vid_ret |= finish_aud_parallel_task(oip, pe, use_internal, vid_ret); //wav出力を完了させる
    if (!video_is_last_pass(pe) && !pe->aud_parallel.abort) return AUO_RESULT_SUCCESS;
    release_audio_parallel_events(pe);
    if (pe->aud_parallel.buffer) free(pe->aud_parallel.buffer);
    if (pe->aud_parallel.th_aud) {
        //音声エンコードを完了させる
        //2passエンコードとかだと音声エンコーダの終了を待機する必要あり
        BOOL wait_for_audio = FALSE;
        while (WaitForSingleObject(pe->aud_parallel.th_aud, LOG_UPDATE_INTERVAL) == WAIT_TIMEOUT) {
            if (!wait_for_audio) {
                set_window_title(g_auo_mes.get(AUO_VIDEO_AUDIO_PROC_WAIT), PROGRESSBAR_MARQUEE);
                wait_for_audio = !wait_for_audio;
            }
            pe->aud_parallel.abort |= oip->func_is_abort();
            log_process_events();
        }
        flush_audio_log();
        if (wait_for_audio)
            set_window_title(g_auo_mes.get(AUO_GUIEX_FULL_NAME), PROGRESSBAR_DISABLED);

        DWORD exit_code = 0;
        //GetExitCodeThreadの返り値がNULLならエラー
        vid_ret |= (NULL == GetExitCodeThread(pe->aud_parallel.th_aud, &exit_code)) ? AUO_RESULT_ERROR : exit_code;
        CloseHandle(pe->aud_parallel.th_aud);
    }
    //初期化 (重要!!!)
    ZeroMemory(&pe->aud_parallel, sizeof(pe->aud_parallel));
    return vid_ret;
}

//auo_pipe.cppのread_from_pipeの特別版
static int ReadLogEnc(PIPE_SET *pipes, int total_drop, int current_frames) {
    DWORD pipe_read = 0;
    if (!PeekNamedPipe(pipes->stdErr.h_read, NULL, 0, NULL, &pipe_read, NULL))
        return -1;
    if (pipe_read) {
        ReadFile(pipes->stdErr.h_read, pipes->read_buf + pipes->buf_len, sizeof(pipes->read_buf) - pipes->buf_len - 1, &pipe_read, NULL);
        pipes->buf_len += pipe_read;
        write_log_enc_mes(pipes->read_buf, &pipes->buf_len, total_drop, current_frames, NULL);
    } else {
        log_process_events();
    }
    return pipe_read;
}

static unsigned __stdcall video_output_thread_func(void *prm) {
    video_output_thread_t *thread_data = reinterpret_cast<video_output_thread_t *>(prm);
    CONVERT_CF_DATA *pixel_data = thread_data->pixel_data;
    WaitForSingleObject(thread_data->he_out_start, INFINITE);
    while (false == thread_data->abort) {
        //映像データをパイプに
        for (int i = 0; i < 1 + thread_data->repeat; i++)
            for (int j = 0; j < pixel_data->count; j++)
                _fwrite_nolock((void *)pixel_data->data[j], 1, pixel_data->size[j], thread_data->f_out);

        thread_data->repeat = 0;
        SetEvent(thread_data->he_out_fin);
        WaitForSingleObject(thread_data->he_out_start, INFINITE);
    }
    return 0;
}

static int video_output_create_thread(video_output_thread_t *thread_data, CONVERT_CF_DATA *pixel_data, FILE *pipe_stdin) {
    AUO_RESULT ret = AUO_RESULT_SUCCESS;
    thread_data->abort = false;
    thread_data->pixel_data = pixel_data;
    thread_data->f_out = pipe_stdin;
    if (   NULL == (thread_data->he_out_start = (HANDLE)CreateEvent(NULL, false, false, NULL))
        || NULL == (thread_data->he_out_fin   = (HANDLE)CreateEvent(NULL, false, true,  NULL))
        || NULL == (thread_data->thread       = (HANDLE)_beginthreadex(NULL, 0, video_output_thread_func, thread_data, 0, NULL))) {
        ret = AUO_RESULT_ERROR;
    }
    return ret;
}

static void video_output_close_thread(video_output_thread_t *thread_data, AUO_RESULT ret) {
    if (thread_data->thread) {
        if (!ret)
            while (WAIT_TIMEOUT == WaitForSingleObject(thread_data->he_out_fin, LOG_UPDATE_INTERVAL))
                log_process_events();
        thread_data->abort = true;
        SetEvent(thread_data->he_out_start);
        WaitForSingleObject(thread_data->thread, INFINITE);
        CloseHandle(thread_data->thread);
        CloseHandle(thread_data->he_out_start);
        CloseHandle(thread_data->he_out_fin);
    }
    memset(thread_data, 0, sizeof(thread_data[0]));
}

static void error_videnc_failed(const PRM_ENC *pe) {
    ULARGE_INTEGER temp_drive_avail_space = { 0 };
    const uint64_t disk_warn_threshold = 4 * 1024 * 1024; //4MB
    //指定されたドライブが存在するかどうか
    char temp_root[MAX_PATH_LEN];
    if (PathGetRoot(pe->temp_filename, temp_root, _countof(temp_root))
        && PathIsDirectory(temp_root)
        && GetDiskFreeSpaceEx(temp_root, &temp_drive_avail_space, NULL, NULL)
        && temp_drive_avail_space.QuadPart <= disk_warn_threshold) {
        char driveLetter[MAX_PATH_LEN];
        strcpy_s(driveLetter, temp_root);
        if (strlen(driveLetter) > 1 && driveLetter[strlen(driveLetter) - 1] == '\\') {
            driveLetter[strlen(driveLetter) - 1] = '\0';
        }
        if (strlen(driveLetter) > 1 && driveLetter[strlen(driveLetter) - 1] == ':') {
            driveLetter[strlen(driveLetter) - 1] = '\0';
        }
        error_videnc_dead_and_nodiskspace(driveLetter, temp_drive_avail_space.QuadPart);
    } else {
        error_videnc_dead();
    }
}

static BOOL get_exedit_file_mapping(ExeditFileMapping* efm) {
    char name[256];
    wsprintf(name, "exedit_%d_%d", '01', GetCurrentProcessId());
    HANDLE hfmo = OpenFileMapping(FILE_MAP_WRITE, FALSE, name);
    if (hfmo == nullptr)
        return FALSE;

    PVOID mapping = MapViewOfFile(hfmo, FILE_MAP_WRITE, 0, 0, sizeof(ExeditFileMapping));
    if (mapping == nullptr) {
        CloseHandle(hfmo);
        return FALSE;
    }
    memcpy(efm, mapping, sizeof(ExeditFileMapping));
    UnmapViewOfFile(mapping);
    CloseHandle(hfmo);

    return TRUE;
};

static AUO_RESULT ffmpeg_out(CONF_GUIEX *conf, const OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat) {
    AUO_RESULT ret = AUO_RESULT_SUCCESS;
    PIPE_SET pipes = { 0 };
    PROCESS_INFORMATION pi_enc = { 0 };

    char enc_cmd[MAX_CMD_LEN]  = { 0 };
    char enc_args[MAX_CMD_LEN] = { 0 };
    char enc_dir[MAX_PATH_LEN] = { 0 };
    char *enc_path = sys_dat->exstg->s_local.ffmpeg_path;

    const bool afs = conf->vid.afs != 0;
    video_output_thread_t thread_data = { 0 };
    thread_data.repeat = pe->delay_cut_additional_vframe;
    CONVERT_CF_DATA pixel_data;
    set_pixel_data(&pixel_data, conf, oip->w, oip->h);

    int *jitter = NULL;
    int rp_ret;

    //x264優先度関連の初期化
    DWORD set_priority = (pe->h_p_aviutl || conf->vid.priority != AVIUTLSYNC_PRIORITY_CLASS) ? priority_table[conf->vid.priority].value : NORMAL_PRIORITY_CLASS;

    //プロセス用情報準備
    if (!PathFileExists(enc_path)) {
        ret |= AUO_RESULT_ERROR; error_no_exe_file(ENCODER_NAME_W, enc_path);
        return ret;
    }
    PathGetDirectory(enc_dir, _countof(enc_dir), enc_path);

    const int color_format = get_aviutl_color_format(conf->enc.use_highbit_depth, conf->enc.output_csp);
    const DWORD aviutl_fourcc = COLORFORMATS[color_format].FOURCC;

    //YUY2/YC48->NV12/YUV444, RGBコピー用関数
    auto convert_func_output_csp = conf->enc.output_csp;
    if (convert_func_output_csp == OUT_CSP_P010) {
        convert_func_output_csp = OUT_CSP_NV12;
    } else if (convert_func_output_csp == OUT_CSP_YUV444_16) {
        convert_func_output_csp = OUT_CSP_YUV444;
    }
    const func_convert_frame convert_frame = get_convert_func(oip->w, color_format, conf->enc.use_highbit_depth ? 16 : 8, conf->enc.interlaced, convert_func_output_csp);
    if (convert_frame == NULL) {
        ret |= AUO_RESULT_ERROR; error_select_convert_func(oip->w, oip->h, conf->enc.use_highbit_depth ? 16 : 8, conf->enc.interlaced, conf->enc.output_csp);
        return ret;
    }
    //映像バッファ用メモリ確保
    if (!malloc_pixel_data(&pixel_data, oip->w, oip->h, conf->enc.output_csp, conf->enc.use_highbit_depth ? 16 : 8)) {
        ret |= AUO_RESULT_ERROR; error_malloc_pixel_data();
        return ret;
    }

    //拡張編集のファイルマッピングを取得
    ExeditFileMapping efm = { nullptr };
    ExeditData ed = { 0 };
    if(conf->enc.output_csp == OUT_CSP_RGBA) {
        if (get_exedit_file_mapping(&efm) == FALSE) {
            ret |= AUO_RESULT_ERROR; error_get_exedit_file_mapping();
            return ret;
        }
        //拡張編集の出力開始
        if (efm.output_start(&ed) == FALSE) {
            ret |= AUO_RESULT_ERROR; error_get_exedit_output_start();
            return ret;
        }
    }

    //パイプの設定
    pipes.stdIn.mode = AUO_PIPE_ENABLE;
    pipes.stdErr.mode = AUO_PIPE_ENABLE;
    pipes.stdIn.bufferSize = pixel_data.total_size * 2;

    //コマンドライン生成
    build_full_cmd(enc_cmd, _countof(enc_cmd), conf, oip, pe, sys_dat, PIPE_FN);
    write_log_auo_line(LOG_INFO, L"ffmpeg options...");
    write_args(enc_cmd);
    sprintf_s(enc_args, _countof(enc_args), "\"%s\" %s", enc_path, enc_cmd);
    
    if ((jitter = (int *)calloc(oip->n + 1, sizeof(int))) == NULL) {
        ret |= AUO_RESULT_ERROR; error_malloc_tc();
        //Aviutl(afs)からのフレーム読み込み
    } else if (!setup_afsvideo(oip, sys_dat, conf, pe)) {
        ret |= AUO_RESULT_ERROR; //Aviutl(afs)からのフレーム読み込みに失敗
        //Aviutl(afs)からのフレーム読み込み
    } else if ((rp_ret = RunProcess(enc_args, enc_dir, &pi_enc, &pipes, (set_priority == AVIUTLSYNC_PRIORITY_CLASS) ? GetPriorityClass(pe->h_p_aviutl) : set_priority, TRUE, FALSE)) != RP_SUCCESS) {
        ret |= AUO_RESULT_ERROR; error_run_process(ENCODER_NAME_W, rp_ret);
    } else if (video_output_create_thread(&thread_data, &pixel_data, pipes.f_stdin)) {
        ret |= AUO_RESULT_ERROR; error_video_output_thread_start();
    } else {
        //全て正常
        int i = 0;
        int framen = 0;
        void *frame = NULL;
        int *next_jitter = NULL;
        bool enc_pause = false, copy_frame = false;
        BOOL drop = FALSE;

        //Aviutlの時間を取得
        PROCESS_TIME time_aviutl;
        GetProcessTime(pe->h_p_aviutl, &time_aviutl);
        pe->h_p_videnc = pi_enc.hProcess;

        //x264が待機に入るまでこちらも待機
        while (WaitForInputIdle(pi_enc.hProcess, LOG_UPDATE_INTERVAL) == WAIT_TIMEOUT)
            log_process_events();
        if (video_is_last_pass(pe) && conf->aud.use_internal)
            if_valid_set_event(pe->aud_parallel.he_aud_start);

        //ログウィンドウ側から制御を可能に
        DWORD tm_vid_enc_start = timeGetTime();
        enable_enc_control(&set_priority, &enc_pause, FALSE, FALSE, tm_vid_enc_start, oip->n);

        //------------メインループ------------
        for (framen = ed.frame_start, i = 0, next_jitter = jitter + 1, pe->drop_count = 0; (conf->enc.output_csp == OUT_CSP_RGBA ? (framen <= ed.frame_end) : (i < oip->n)); i++, framen++, next_jitter++) {
            //中断を確認
            ret |= (oip->func_is_abort()) ? AUO_RESULT_ABORT : AUO_RESULT_SUCCESS;

            //x264が実行中なら、メッセージを取得・ログウィンドウに表示
            if (ReadLogEnc(&pipes, pe->drop_count, i) < 0) {
                //勝手に死んだ...
                ret |= AUO_RESULT_ERROR; error_videnc_failed(pe);
                break;
            }

            if (!(i & 7)) {
                //Aviutlの進捗表示を更新
                oip->func_rest_time_disp(i + oip->n * (pe->current_x264_pass - 1), oip->n * pe->total_x264_pass);

                //x264優先度
                check_enc_priority(pe->h_p_aviutl, pi_enc.hProcess, set_priority);

                //音声同時処理
                if (!conf->aud.use_internal) {
                    //音声同時処理
                    ret |= aud_parallel_task(oip, pe, conf->aud.use_internal);
                }
            }

            //一時停止
            while (enc_pause & !ret) {
                Sleep(LOG_UPDATE_INTERVAL);
                ret |= (oip->func_is_abort()) ? AUO_RESULT_ABORT : AUO_RESULT_SUCCESS;
                ReadLogEnc(&pipes, pe->drop_count, i);
                log_process_events();
            }

            //標準入力への書き込み完了をチェック
            for (int itr = 0; WAIT_TIMEOUT == WaitForSingleObject(thread_data.he_out_fin, 0); itr++) {
                ret |= (oip->func_is_abort()) ? AUO_RESULT_ABORT : AUO_RESULT_SUCCESS;
                if ((itr & 63) == 63) {
                    if (ReadLogEnc(&pipes, pe->drop_count, i) < 0) {
                        //勝手に死んだ...
                        ret |= AUO_RESULT_ERROR; error_videnc_failed(pe);
                        break;
                    }
                    log_process_events();
                }
                if (conf->aud.use_internal) {
                    //音声同時処理
                    ret |= aud_parallel_task(oip, pe, conf->aud.use_internal);
                }
            }

            //中断・エラー等をチェック
            if (AUO_RESULT_SUCCESS != ret)
                break;


            if(conf->enc.output_csp == OUT_CSP_RGBA) {
                //拡張編集からフレームをもらう
                if ((frame = efm.get_image(framen)) == NULL) {
                    ret |= AUO_RESULT_ERROR; error_afs_get_frame();
                    break;
                }
            } else {
                //Aviutl(afs)からフレームをもらう
                if (NULL == (frame = ((afs) ? afs_get_video((OUTPUT_INFO *)oip, i, &drop, next_jitter) : oip->func_get_video_ex(i, aviutl_fourcc)))) {
                    ret |= AUO_RESULT_ERROR; error_afs_get_frame();
                    break;
                }
            }

            //コピーフレームフラグ処理
            copy_frame = (i && (oip->func_get_flag(i) & OUTPUT_INFO_FRAME_FLAG_COPYFRAME));

            drop |= (afs & copy_frame);

            if (!drop) {
                //コピーフレームの場合は、映像バッファの中身を更新せず、そのままパイプに流す
                if (!copy_frame)
                    convert_frame(frame, &pixel_data, oip->w, oip->h);  /// YUY2/YC48->NV12/YUV444変換, RGBコピー
                //標準入力への書き込みを開始
                SetEvent(thread_data.he_out_start);
            } else {
                *(next_jitter - 1) = DROP_FRAME_FLAG;
                pe->drop_count++;
                //次のフレームの変換を許可
                SetEvent(thread_data.he_out_fin);
            }

            // 「表示 -> セーブ中もプレビュー表示」がチェックされていると
            // func_update_preview() の呼び出しによって func_get_video_ex() の
            // 取得したバッファが書き換えられてしまうので、呼び出し位置を移動 (拡張AVI出力 plus より)
            oip->func_update_preview();
        }
        //------------メインループここまで--------------

        //書き込みスレッドを終了
        video_output_close_thread(&thread_data, ret);

        //ログウィンドウからのx264制御を無効化
        disable_enc_control();

        //パイプを閉じる
        CloseStdIn(&pipes);
        
        if(conf->enc.output_csp == OUT_CSP_RGBA)
            efm.output_end();

        if (!ret) oip->func_rest_time_disp(oip->n * pe->current_x264_pass, oip->n * pe->total_x264_pass);

        //音声の同時処理を終了させる
        ret |= finish_aud_parallel_task(oip, pe, conf->aud.use_internal, ret);

        //タイムコード出力
        if (!ret && (afs || conf->vid.auo_tcfile_out))
            tcfile_out(jitter, oip->n, (double)oip->rate / (double)oip->scale, afs, pe);

        //エンコーダ終了待機
        while (WaitForSingleObject(pi_enc.hProcess, LOG_UPDATE_INTERVAL) == WAIT_TIMEOUT)
            ReadLogEnc(&pipes, pe->drop_count, i);

        DWORD tm_vid_enc_fin = timeGetTime();

        //最後にメッセージを取得
        while (ReadLogEnc(&pipes, pe->drop_count, i) > 0);

        write_log_auo_line_fmt(LOG_INFO, L"%s: Aviutl: %.2f%% / %s: %.2f%%", g_auo_mes.get(AUO_VIDEO_CPU_USAGE), GetProcessAvgCPUUsage(pe->h_p_aviutl, &time_aviutl), ENCODER_APP_NAME_W, GetProcessAvgCPUUsage(pi_enc.hProcess));
        write_log_auo_enc_time(g_auo_mes.get(AUO_VIDEO_ENCODE_TIME), tm_vid_enc_fin - tm_vid_enc_start);
    }

    //解放処理
    if (pipes.stdErr.mode)
        CloseHandle(pipes.stdErr.h_read);
    CloseHandle(pi_enc.hProcess);
    CloseHandle(pi_enc.hThread);
    pe->h_p_videnc = NULL;

    free_pixel_data(&pixel_data);
    if (jitter) free(jitter);

    return ret;
}

static void set_window_title_ffmpegout(const PRM_ENC *pe) {
    wchar_t mes[256];
    swprintf_s(mes, _countof(mes), L"%s %s", ENCODER_NAME_W, g_auo_mes.get(AUO_VIDEO_ENCODE));
    if (pe->total_x264_pass > 1)
        swprintf_s(mes + wcslen(mes), _countof(mes) - wcslen(mes), L"   %d / %d pass", pe->current_x264_pass, pe->total_x264_pass);
    if (pe->aud_parallel.th_aud)
        wcscat_s(mes, _countof(mes), g_auo_mes.get(AUO_VIDEO_AUDIO_ENCODE));
    set_window_title(mes, PROGRESSBAR_CONTINUOUS);
}

static AUO_RESULT video_output_inside(CONF_GUIEX *conf, const OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat) {
    AUO_RESULT ret = AUO_RESULT_SUCCESS;
    //動画エンコードの必要がなければ終了
    if (pe->video_out_type == VIDEO_OUTPUT_DISABLED)
        return ret;

    for (; !ret && pe->current_x264_pass <= pe->total_x264_pass; pe->current_x264_pass++) {
        if (pe->current_x264_pass > 1)
            open_log_window(oip->savefile, sys_dat, pe->current_x264_pass, pe->total_x264_pass);
        set_window_title_ffmpegout(pe);
        ret |= ffmpeg_out(conf, oip, pe, sys_dat);
        set_window_title(AUO_FULL_NAME_W, PROGRESSBAR_DISABLED);
    }
    return ret;
}

AUO_RESULT video_output(CONF_GUIEX *conf, const OUTPUT_INFO *oip, PRM_ENC *pe, const SYSTEM_DATA *sys_dat) {
    return exit_audio_parallel_control(oip, pe, conf->aud.use_internal, video_output_inside(conf, oip, pe, sys_dat));
}
