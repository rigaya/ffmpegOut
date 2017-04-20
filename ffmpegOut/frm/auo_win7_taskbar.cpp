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

#include <Windows.h>
// Windows SDK 7.1の<Shobjidl.h>が呼ばれるようにすること
#include <Shobjidl.h>
#pragma comment(lib, "ole32.lib")
#include "auo_util.h"
#include "auo_frm.h"

static ITaskbarList3 *g_pTaskbarList = NULL;
static BOOL enabled = false;
static const int MAX_PROGRESS = 1000;

void taskbar_progress_enable(BOOL _enable) {
    enabled = _enable;
}

void taskbar_progress_init() {
    if (!check_OS_Win7orLater() || CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pTaskbarList)) != S_OK)
        g_pTaskbarList = NULL;
}

void taskbar_progress_start(HWND hWnd, int mode) {
    if (g_pTaskbarList) {
        g_pTaskbarList->SetProgressValue(hWnd, 0, MAX_PROGRESS);
        g_pTaskbarList->SetProgressState(hWnd, (!enabled || mode == PROGRESSBAR_DISABLED) ? TBPF_NOPROGRESS : ((mode == PROGRESSBAR_MARQUEE) ? TBPF_INDETERMINATE : TBPF_NORMAL));
    }
}

void taskbar_progress_paused(HWND hWnd) {
    if (g_pTaskbarList && enabled)
        g_pTaskbarList->SetProgressState(hWnd, TBPF_PAUSED);
}

void taskbar_setprogress(HWND hWnd, double progress) {
    if (g_pTaskbarList) {
        g_pTaskbarList->SetProgressValue(hWnd, (enabled) ? (int)(MAX_PROGRESS * progress + 0.5) : 0, MAX_PROGRESS);
        g_pTaskbarList->SetProgressState(hWnd, (enabled) ? TBPF_NORMAL : TBPF_NOPROGRESS);
    }
}