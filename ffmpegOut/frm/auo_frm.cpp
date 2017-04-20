﻿//  -----------------------------------------------------------------------------------------
//   ffmpeg / avconv 出力 by rigaya
//  -----------------------------------------------------------------------------------------
//   ソースコードについて
//   ・無保証です。
//   ・本ソースコードを使用したことによるいかなる損害・トラブルについてrigayaは責任を負いません。
//   以上に了解して頂ける場合、本ソースコードの使用、複製、改変、再頒布を行って頂いて構いません。
//  -----------------------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include "auo_frm.h"
#include "auo_util.h"

const int NEW_LINE_THRESHOLD = 125;
const int MAKE_NEW_LINE_THRESHOLD = 140;

static inline int check_log_type(char *mes) {
    if (strstr(mes, "warning")) return LOG_WARNING;
    if (strstr(mes, "error")) return LOG_ERROR;
    return LOG_INFO;
}

static inline void add_line_to_cache(LOG_CACHE *cache_line, const char *mes) {
    if (cache_line->idx >= cache_line->max_line) {
        //メモリ不足なら再確保
        if (NULL != (cache_line->lines = (char **)realloc(cache_line->lines, sizeof(cache_line->lines[0]) * cache_line->max_line * 2))) {
            memset(&cache_line->lines[cache_line->max_line], 0, sizeof(cache_line->lines[0]) * cache_line->max_line);
            cache_line->max_line *= 2;
        }
    }
    if (cache_line->lines) {
        //一行のデータを格納
        const int line_len = strlen(mes) + 1;
        char *line_ptr = (char *)malloc(line_len * sizeof(line_ptr[0]));
        memcpy(line_ptr, mes, line_len);
        cache_line->lines[cache_line->idx] = line_ptr;
        cache_line->idx++;
    }
}

void release_log_cache(LOG_CACHE *log_cache) {
    if (log_cache && log_cache->lines) {
        for (int i = 0; i < log_cache->idx; i++)
            if (log_cache->lines[i]) free(log_cache->lines[i]);
        free(log_cache->lines);
        log_cache->lines = NULL;
        log_cache->idx = 0;
    }
}

//LOG_CACHEの初期化、成功->0, 失敗->1
int init_log_cache(LOG_CACHE *log_cache) {
    release_log_cache(log_cache);
    log_cache->idx = 0;
    log_cache->max_line = 64;
    return NULL == (log_cache->lines = (char **)calloc(log_cache->max_line, sizeof(log_cache->lines[0])));
}

//長すぎたら適当に折り返す
static int write_log_enc_mes_line(char *const mes, LOG_CACHE *cache_line) {
    const int mes_len = strlen(mes);
    const int mes_type = check_log_type(mes);
    char *const fin = mes + mes_len;
    char *const prefix_ptr = strstr(mes, "]: ");
    const int prefix_len = (prefix_ptr) ? prefix_ptr - mes + strlen("]: ") : 0;
    char *p = mes, *q = NULL;
    BOOL flag_continue = FALSE;
    do {
        const int threshold = NEW_LINE_THRESHOLD - (p != mes) * prefix_len;
        flag_continue = mes_len >= MAKE_NEW_LINE_THRESHOLD
            && (p + threshold) < fin
            && (q = strrchr(p, ' ', threshold)) != NULL;
        if (flag_continue) *q = '\0';
        if (p != mes)
            for (char *const prefix_adjust = p - prefix_len; p > prefix_adjust; p--)
                *(p-1) = ' ';
        (cache_line) ? add_line_to_cache(cache_line, p) : write_log_line(mes_type, p, true);
        p=q+1;
    } while (flag_continue);
    return mes_len;
}

void set_reconstructed_title_mes(const char *mes, int total_drop, int current_frames) {
    double progress = 0, fps = 0, bitrate = 0;
    int i_frame = 0, total_frame = 0;
    int remain_time[3] = { 0 }, elapsed_time[3] = { 0 };
    char buffer[1024] = { 0 };
    int length = 0;
    const char *ptr = buffer;
    if ('[' == mes[0]
        && 11 >= sscanf_s(mes, "[%lf%%] %d/%d %lf %lf %d:%d:%d %d:%d:%d %n",
            &progress, &i_frame, &total_frame, &fps, &bitrate,
            &remain_time[0], &remain_time[1], &remain_time[2],
            &elapsed_time[0], &elapsed_time[1], &elapsed_time[2],
            &length)) {
        const char *qtr = mes + length;
        while (' ' == *qtr) qtr++;
        while (' ' != *qtr && '\0' != *qtr) qtr++;
        while (' ' == *qtr) qtr++;
        while (' ' != *qtr && '\0' != *qtr) qtr++;
        while (' ' == *qtr) qtr++;
        sprintf_s(buffer, _countof(buffer), "[%3.1lf%%] %d/%d frames, %.2lf fps, %.2lf kb/s, eta %d:%02d:%02d, %s %s",
            progress, i_frame, total_frame, fps, bitrate, elapsed_time[0], elapsed_time[1], elapsed_time[2], ('\0' != *qtr) ? "est.size" : "", qtr);
    } else if (3 == sscanf_s(mes, "%d %lf %lf", &i_frame, &fps, &bitrate)) {
        sprintf_s(buffer, _countof(buffer), "%d frames, %.2lf fps, %.2lf kb/s", i_frame, fps, bitrate);
    } else {
        ptr = mes;
    }
    set_window_title_enc_mes(ptr, total_drop, current_frames);
}

void write_log_enc_mes(char *const msg, DWORD *log_len, int total_drop, int current_frames) {
    char *a, *b, *mes = msg;
    char * const fin = mes + *log_len; //null文字の位置
    *fin = '\0';
    while ((a = strchr(mes, '\n')) != NULL) {
        if ((b = strrchr(mes, '\r', a - mes - 2)) != NULL)
            mes = b + 1;
        *a = '\0';
        write_log_enc_mes_line(mes, NULL);
        mes = a + 1;
    }
    if ((a = strrchr(mes, '\r', fin - mes - 1)) != NULL) {
        b = a - 1;
        while (*b == ' ' || *b == '\r')
            b--;
        *(b+1) = '\0';
        if ((b = strrchr(mes, '\r', b - mes - 2)) != NULL)
            mes = b + 1;
        if (NULL == strstr(mes, "frames")) {
            set_reconstructed_title_mes(mes, total_drop, current_frames);
        } else {
            set_window_title_enc_mes(mes, total_drop, current_frames);
        }
        mes = a + 1;
    }
    if (mes == msg && *log_len)
        mes += write_log_enc_mes_line(mes, NULL);
    memmove(msg, mes, ((*log_len = fin - mes) + 1) * sizeof(msg[0]));
}

void write_args(const char *args) {
    size_t len = strlen(args);
    char *const c = (char *)malloc((len+1)*sizeof(c[0]));
    char *const fin = c + len;
    memcpy(c, args, (len+1)*sizeof(c[0]));
    char *p = c;
    for (char *q = NULL; p + NEW_LINE_THRESHOLD < fin && (q = strrchr(p, ' ', NEW_LINE_THRESHOLD)) != NULL; p = q+1) {
        *q = '\0';
        write_log_line(LOG_INFO, p);
    }
    write_log_line(LOG_INFO, p);
    free(c);
}

void write_log_exe_mes(char *const msg, DWORD *log_len, const char *exename, LOG_CACHE *cache_line) {
    char *a, *b, *mes = msg;
    char * const fin = mes + *log_len; //null文字の位置
    char * buffer = NULL;
    DWORD buffer_len = 0;
    *fin = '\0';
    while ((a = strchr(mes, '\n')) != NULL) {
        if ((b = strrchr(mes, '\r', a - mes - 2)) != NULL)
            mes = b + 1;
        *a = '\0';
        write_log_enc_mes_line(mes, cache_line);
        mes = a + 1;
    }
    if ((a = strrchr(mes, '\r', fin - mes - 1)) != NULL) {
        b = a - 1;
        while (*b == ' ' || *b == '\r')
            b--;
        *(b+1) = '\0';
        if ((b = strrchr(mes, '\r', b - mes - 2)) != NULL)
            mes = b + 1;
        if (exename) {
            if (buffer_len == 0) buffer_len = *log_len + strlen(exename) + 3;
            if (buffer != NULL || NULL != (buffer = (char*)malloc(buffer_len * sizeof(buffer[0])))) {
                sprintf_s(buffer, buffer_len, "%s: %s", exename, mes);
                set_window_title(buffer);
            }
        }
        mes = a + 1;
    }
    if (mes == msg && *log_len)
        mes += write_log_enc_mes_line(mes, cache_line);
    memmove(msg, mes, ((*log_len = fin - mes) + 1) * sizeof(msg[0]));
    if (buffer) free(buffer);
}
