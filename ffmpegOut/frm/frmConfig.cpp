﻿// -----------------------------------------------------------------------------------------
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

//以下warning C4100を黙らせる
//C4100 : 引数は関数の本体部で 1 度も参照されません。
#pragma warning( push )
#pragma warning( disable: 4100 )

#include "auo_version.h"
#include "auo_frm.h"
#include "auo_faw2aac.h"
#include "frmConfig.h"
#include "frmSaveNewStg.h"
#include "frmOtherSettings.h"
#include "frmBitrateCalculator.h"

using namespace ffmpegOut;

/// -------------------------------------------------
///     設定画面の表示
/// -------------------------------------------------
[STAThreadAttribute]
void ShowfrmConfig(CONF_GUIEX *conf, const SYSTEM_DATA *sys_dat) {
    if (!sys_dat->exstg->s_local.disable_visual_styles)
        System::Windows::Forms::Application::EnableVisualStyles();
    System::IO::Directory::SetCurrentDirectory(String(sys_dat->aviutl_dir).ToString());
    frmConfig frmConf(conf, sys_dat);
    frmConf.ShowDialog();
}

/// -------------------------------------------------
///     frmSaveNewStg 関数
/// -------------------------------------------------
System::Boolean frmSaveNewStg::checkStgFileName(String^ stgName) {
    String^ fileName;
    if (stgName->Length == 0)
        return false;

    if (!ValidiateFileName(stgName)) {
        MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_INVALID_CHAR), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
        return false;
    }
    if (String::Compare(Path::GetExtension(stgName), L".stg", true))
        stgName += L".stg";
    if (File::Exists(fileName = Path::Combine(fsnCXFolderBrowser->GetSelectedFolder(), stgName)))
        if (MessageBox::Show(stgName + LOAD_CLI_STRING(AUO_CONFIG_ALREADY_EXISTS), LOAD_CLI_STRING(AUO_CONFIG_OVERWRITE_CHECK), MessageBoxButtons::YesNo, MessageBoxIcon::Question)
            != System::Windows::Forms::DialogResult::Yes)
            return false;
    StgFileName = fileName;
    return true;
}

System::Void frmSaveNewStg::setStgDir(String^ _stgDir) {
    StgDir = _stgDir;
    fsnCXFolderBrowser->SetRootDirAndReload(StgDir);
}


/// -------------------------------------------------
///     frmBitrateCalculator 関数
/// -------------------------------------------------
System::Void frmBitrateCalculator::Init(int VideoBitrate, int AudioBitrate, bool BTVBEnable, bool BTABEnable, int ab_max, const AuoTheme themeTo, const DarkenWindowStgReader *dwStg) {
    guiEx_settings exStg(true);
    exStg.load_fbc();
    enable_events = false;
    dwStgReader = dwStg;
    CheckTheme(themeTo);
    fbcTXSize->Text = exStg.s_fbc.initial_size.ToString("F2");
    fbcChangeTimeSetMode(exStg.s_fbc.calc_time_from_frame != 0);
    fbcRBCalcRate->Checked = exStg.s_fbc.calc_bitrate != 0;
    fbcRBCalcSize->Checked = !fbcRBCalcRate->Checked;
    fbcTXMovieFrameRate->Text = Convert::ToString(exStg.s_fbc.last_fps);
    fbcNUMovieFrames->Value = exStg.s_fbc.last_frame_num;
    fbcNULengthHour->Value = Convert::ToDecimal((int)exStg.s_fbc.last_time_in_sec / 3600);
    fbcNULengthMin->Value = Convert::ToDecimal((int)(exStg.s_fbc.last_time_in_sec % 3600) / 60);
    fbcNULengthSec->Value =  Convert::ToDecimal((int)exStg.s_fbc.last_time_in_sec % 60);
    SetBTVBEnabled(BTVBEnable);
    SetBTABEnabled(BTABEnable, ab_max);
    SetNUVideoBitrate(VideoBitrate);
    SetNUAudioBitrate(AudioBitrate);
    enable_events = true;
}
System::Void frmBitrateCalculator::CheckTheme(const AuoTheme themeTo) {
    //変更の必要がなければ終了
    if (themeTo == themeMode) return;

    //一度ウィンドウの再描画を完全に抑止する
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
    SetAllColor(this, themeTo, this->GetType(), dwStgReader);
    SetAllMouseMove(this, themeTo);
    //一度ウィンドウの再描画を再開し、強制的に再描画させる
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
    this->Refresh();
    themeMode = themeTo;
}
System::Void frmBitrateCalculator::frmBitrateCalculator_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
    guiEx_settings exStg(true);
    exStg.load_fbc();
    exStg.s_fbc.calc_bitrate = fbcRBCalcRate->Checked;
    exStg.s_fbc.calc_time_from_frame = fbcPNMovieFrames->Visible;
    exStg.s_fbc.last_fps = Convert::ToDouble(fbcTXMovieFrameRate->Text);
    exStg.s_fbc.last_frame_num = Convert::ToInt32(fbcNUMovieFrames->Value);
    exStg.s_fbc.last_time_in_sec = Convert::ToInt32(fbcNULengthHour->Value) * 3600
                                 + Convert::ToInt32(fbcNULengthMin->Value) * 60
                                 + Convert::ToInt32(fbcNULengthSec->Value);
    if (fbcRBCalcRate->Checked)
        exStg.s_fbc.initial_size = Convert::ToDouble(fbcTXSize->Text);
    exStg.save_fbc();
    frmConfig^ fcg = dynamic_cast<frmConfig^>(this->Owner);
    if (fcg != nullptr)
        fcg->InformfbcClosed();
}
System::Void frmBitrateCalculator::fbcRBCalcRate_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    if (fbcRBCalcRate->Checked && Convert::ToDouble(fbcTXSize->Text) <= 0.0) {
        guiEx_settings exStg(true);
        exStg.load_fbc();
        fbcTXSize->Text = exStg.s_fbc.initial_size.ToString("F2");
    }
}
System::Void frmBitrateCalculator::fbcBTABApply_Click(System::Object^  sender, System::EventArgs^  e) {
    frmConfig^ fcg = dynamic_cast<frmConfig^>(this->Owner);
    if (fcg != nullptr)
        fcg->SetAudioBitrate((int)fbcNUBitrateAudio->Value);
}
System::Void frmBitrateCalculator::fbcMouseEnter_SetColor(System::Object^  sender, System::EventArgs^  e) {
    fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Hot, dwStgReader);
}
System::Void frmBitrateCalculator::fbcMouseLeave_SetColor(System::Object^  sender, System::EventArgs^  e) {
    fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Normal, dwStgReader);
}
System::Void frmBitrateCalculator::SetAllMouseMove(Control ^top, const AuoTheme themeTo) {
    if (themeTo == themeMode) return;
    System::Type^ type = top->GetType();
    if (type == CheckBox::typeid) {
        top->MouseEnter += gcnew System::EventHandler(this, &frmBitrateCalculator::fbcMouseEnter_SetColor);
        top->MouseLeave += gcnew System::EventHandler(this, &frmBitrateCalculator::fbcMouseLeave_SetColor);
    }
    for (int i = 0; i < top->Controls->Count; i++) {
        SetAllMouseMove(top->Controls[i], themeTo);
    }
}

/// -------------------------------------------------
///     frmConfig 関数  (frmBitrateCalculator関連)
/// -------------------------------------------------
System::Void frmConfig::CloseBitrateCalc() {
    frmBitrateCalculator::Instance::get()->Close();
}
System::Void frmConfig::fcgTSBBitrateCalc_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    if (fcgTSBBitrateCalc->Checked) {
        bool videoBitrateMode = false;

        frmBitrateCalculator::Instance::get()->Init(
            0,
            (fcgNUAudioBitrate->Visible) ? (int)fcgNUAudioBitrate->Value : 0,
            videoBitrateMode,
            fcgNUAudioBitrate->Visible,
            (int)fcgNUAudioBitrate->Maximum,
            themeMode,
            dwStgReader
            );
        frmBitrateCalculator::Instance::get()->Owner = this;
        frmBitrateCalculator::Instance::get()->Show();
    } else {
        frmBitrateCalculator::Instance::get()->Close();
    }
}
System::Void frmConfig::SetfbcBTABEnable(bool enable, int max) {
    frmBitrateCalculator::Instance::get()->SetBTABEnabled(fcgNUAudioBitrate->Visible, max);
}
System::Void frmConfig::SetAudioBitrate(int bitrate) {
    SetNUValue(fcgNUAudioBitrate, bitrate);
}
System::Void frmConfig::InformfbcClosed() {
    fcgTSBBitrateCalc->Checked = false;
}


/// -------------------------------------------------
///     frmConfig 関数
/// -------------------------------------------------
/////////////   LocalStg関連  //////////////////////
System::Void frmConfig::LoadLocalStg() {
    guiEx_settings *_ex_stg = sys_dat->exstg;
    _ex_stg->load_encode_stg();
    LocalStg.ffmpegOutExeName = String(_ex_stg->s_enc.filename).ToString();
    LocalStg.ffmpegOutPath    = String(_ex_stg->s_enc.fullpath).ToString();
    LocalStg.CustomTmpDir    = String(_ex_stg->s_local.custom_tmp_dir).ToString();
    LocalStg.CustomAudTmpDir = String(_ex_stg->s_local.custom_audio_tmp_dir).ToString();
    LocalStg.CustomMP4TmpDir = String(_ex_stg->s_local.custom_mp4box_tmp_dir).ToString();
    LocalStg.LastAppDir      = String(_ex_stg->s_local.app_dir).ToString();
    LocalStg.LastBatDir      = String(_ex_stg->s_local.bat_dir).ToString();
    LocalStg.MP4MuxerExeName = String(_ex_stg->s_mux[MUXER_MP4].filename).ToString();
    LocalStg.MP4MuxerPath    = String(_ex_stg->s_mux[MUXER_MP4].fullpath).ToString();
    LocalStg.MKVMuxerExeName = String(_ex_stg->s_mux[MUXER_MKV].filename).ToString();
    LocalStg.MKVMuxerPath    = String(_ex_stg->s_mux[MUXER_MKV].fullpath).ToString();
    LocalStg.TC2MP4ExeName   = String(_ex_stg->s_mux[MUXER_TC2MP4].filename).ToString();
    LocalStg.TC2MP4Path      = String(_ex_stg->s_mux[MUXER_TC2MP4].fullpath).ToString();
    LocalStg.MP4RawExeName   = String(_ex_stg->s_mux[MUXER_MP4_RAW].filename).ToString();
    LocalStg.MP4RawPath      = String(_ex_stg->s_mux[MUXER_MP4_RAW].fullpath).ToString();

    LocalStg.audEncName->Clear();
    LocalStg.audEncExeName->Clear();
    LocalStg.audEncPath->Clear();
    for (int i = 0; i < _ex_stg->s_aud_ext_count; i++) {
        LocalStg.audEncName->Add(String(_ex_stg->s_aud_ext[i].dispname).ToString());
        LocalStg.audEncExeName->Add(String(_ex_stg->s_aud_ext[i].filename).ToString());
        LocalStg.audEncPath->Add(String(_ex_stg->s_aud_ext[i].fullpath).ToString());
    }
}

System::Boolean frmConfig::CheckLocalStg() {
    bool error = false;
    String^ err = "";
    //ffmpegのチェック
    if (LocalStg.ffmpegOutPath->Length > 0
        && !File::Exists(LocalStg.ffmpegOutPath)) {
        error = true;
        err += LOAD_CLI_STRING(AUO_CONFIG_VID_ENC_NOT_EXIST) + L"\n [ " + LocalStg.ffmpegOutPath + L" ]\n";
    }
    //音声エンコーダのチェック (実行ファイル名がない場合はチェックしない)
    if (LocalStg.audEncExeName[fcgCXAudioEncoder->SelectedIndex]->Length) {
        String^ AudioEncoderPath = LocalStg.audEncPath[fcgCXAudioEncoder->SelectedIndex];
        if (AudioEncoderPath->Length > 0
            && !File::Exists(AudioEncoderPath)
            && (fcgCXAudioEncoder->SelectedIndex != sys_dat->exstg->get_faw_index(fcgCBAudioUseInternal->Checked)) ) {
            //音声実行ファイルがない かつ
            //選択された音声がfawでない または fawであってもfaw2aacがない
            if (error) err += L"\n\n";
            error = true;
            err += LOAD_CLI_STRING(AUO_CONFIG_AUD_ENC_NOT_EXIST) + L"\n [ " + AudioEncoderPath + L" ]\n";
        }
    }
    //FAWのチェック
    if (fcgCBFAWCheck->Checked) {
        if (sys_dat->exstg->get_faw_index(fcgCBAudioUseInternal->Checked) == FAW_INDEX_ERROR) {
            if (error) err += L"\n\n";
            error = true;
            err += LOAD_CLI_STRING(AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI1) + L"\n"
                +  LOAD_CLI_STRING(AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI2) + L"\n"
                +  LOAD_CLI_STRING(AUO_CONFIG_FAW_STG_NOT_FOUND_IN_INI3);
        }
    }
    if (error)
        MessageBox::Show(this, err, LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
    return error;
}

System::Void frmConfig::SaveLocalStg() {
    guiEx_settings *_ex_stg = sys_dat->exstg;
    _ex_stg->load_encode_stg();
    GetWCHARfromString(_ex_stg->s_enc.fullpath,                _countof(_ex_stg->s_enc.fullpath),                LocalStg.ffmpegOutPath);
    GetWCHARfromString(_ex_stg->s_local.custom_tmp_dir,        _countof(_ex_stg->s_local.custom_tmp_dir),        LocalStg.CustomTmpDir);
    GetWCHARfromString(_ex_stg->s_local.custom_mp4box_tmp_dir, _countof(_ex_stg->s_local.custom_mp4box_tmp_dir), LocalStg.CustomMP4TmpDir);
    GetWCHARfromString(_ex_stg->s_local.custom_audio_tmp_dir,  _countof(_ex_stg->s_local.custom_audio_tmp_dir),  LocalStg.CustomAudTmpDir);
    GetWCHARfromString(_ex_stg->s_local.app_dir,               _countof(_ex_stg->s_local.app_dir),               LocalStg.LastAppDir);
    GetWCHARfromString(_ex_stg->s_local.bat_dir,               _countof(_ex_stg->s_local.bat_dir),               LocalStg.LastBatDir);
    GetWCHARfromString(_ex_stg->s_mux[MUXER_MP4].fullpath,     _countof(_ex_stg->s_mux[MUXER_MP4].fullpath),     LocalStg.MP4MuxerPath);
    GetWCHARfromString(_ex_stg->s_mux[MUXER_MKV].fullpath,     _countof(_ex_stg->s_mux[MUXER_MKV].fullpath),     LocalStg.MKVMuxerPath);
    GetWCHARfromString(_ex_stg->s_mux[MUXER_TC2MP4].fullpath,  _countof(_ex_stg->s_mux[MUXER_TC2MP4].fullpath),  LocalStg.TC2MP4Path);
    GetWCHARfromString(_ex_stg->s_mux[MUXER_MP4_RAW].fullpath, _countof(_ex_stg->s_mux[MUXER_MP4_RAW].fullpath), LocalStg.MP4RawPath);
    for (int i = 0; i < _ex_stg->s_aud_ext_count; i++)
        GetWCHARfromString(_ex_stg->s_aud_ext[i].fullpath,     _countof(_ex_stg->s_aud_ext[i].fullpath),             LocalStg.audEncPath[i]);
    _ex_stg->save_local();
}

System::Void frmConfig::SetLocalStg() {
    fcgLBVideoEncoderPath->Text   = System::String(ENCODER_NAME).ToString() + L".exe" + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgTXVideoEncoderPath->Text   = LocalStg.ffmpegOutPath;
    fcgTXMP4MuxerPath->Text       = LocalStg.MP4MuxerPath;
    fcgTXMKVMuxerPath->Text       = LocalStg.MKVMuxerPath;
    fcgTXTC2MP4Path->Text         = LocalStg.TC2MP4Path;
    fcgTXMPGMuxerPath->Text       = LocalStg.MPGMuxerPath;
    fcgTXMP4RawPath->Text         = LocalStg.MP4RawPath;
    fcgTXCustomAudioTempDir->Text = LocalStg.CustomAudTmpDir;
    fcgTXCustomTempDir->Text      = LocalStg.CustomTmpDir;
    fcgTXMP4BoxTempDir->Text      = LocalStg.CustomMP4TmpDir;
    fcgLBMP4MuxerPath->Text       = LocalStg.MP4MuxerExeName + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgLBMKVMuxerPath->Text       = LocalStg.MKVMuxerExeName + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgLBTC2MP4Path->Text         = LocalStg.TC2MP4ExeName   + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgLBMPGMuxerPath->Text       = LocalStg.MPGMuxerExeName + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
    fcgLBMP4RawPath->Text         = LocalStg.MP4RawExeName   + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);


    fcgTXVideoEncoderPath->SelectionStart   = fcgTXVideoEncoderPath->Text->Length;
    fcgTXMP4MuxerPath->SelectionStart       = fcgTXMP4MuxerPath->Text->Length;
    fcgTXTC2MP4Path->SelectionStart         = fcgTXTC2MP4Path->Text->Length;
    fcgTXMKVMuxerPath->SelectionStart       = fcgTXMKVMuxerPath->Text->Length;
    fcgTXMPGMuxerPath->SelectionStart       = fcgTXMPGMuxerPath->Text->Length;
    fcgTXMP4RawPath->SelectionStart         = fcgTXMP4RawPath->Text->Length;
}

//////////////       その他イベント処理   ////////////////////////
System::Void frmConfig::ActivateToolTip(bool Enable) {
    fcgTTEx->Active = Enable;
}

System::Void frmConfig::fcgTSBOtherSettings_Click(System::Object^  sender, System::EventArgs^  e) {
    frmOtherSettings::Instance::get()->stgDir = String(sys_dat->exstg->s_local.stg_dir).ToString();
    frmOtherSettings::Instance::get()->SetTheme(themeMode, dwStgReader);
    frmOtherSettings::Instance::get()->ShowDialog();
    TCHAR buf[MAX_PATH_LEN];
    GetWCHARfromString(buf, _countof(buf), frmOtherSettings::Instance::get()->stgDir);
    if (_tcsicmp(buf, sys_dat->exstg->s_local.stg_dir)) {
        //変更があったら保存する
        _tcscpy_s(sys_dat->exstg->s_local.stg_dir, buf);
        sys_dat->exstg->save_local();
        InitStgFileList();
    }
    //再読み込み
    guiEx_settings stg;
    stg.load_encode_stg();
    log_reload_settings();
    sys_dat->exstg->s_local.get_relative_path = stg.s_local.get_relative_path;
    SetStgEscKey(stg.s_local.enable_stg_esc_key != 0);
    ActivateToolTip(stg.s_local.disable_tooltip_help == FALSE);
    if (str_has_char(stg.s_local.conf_font.name))
        SetFontFamilyToForm(this, gcnew FontFamily(String(stg.s_local.conf_font.name).ToString()), this->Font->FontFamily);
}
System::Boolean frmConfig::EnableSettingsNoteChange(bool Enable) {
    if (fcgTSTSettingsNotes->Visible == Enable &&
        fcgTSLSettingsNotes->Visible == !Enable)
        return true;
    if (CountStringBytes(fcgTSTSettingsNotes->Text) > fcgTSTSettingsNotes->MaxLength - 1) {
        MessageBox::Show(this, LOAD_CLI_STRING(AUO_CONFIG_TEXT_LIMIT_LENGTH), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
        fcgTSTSettingsNotes->Focus();
        fcgTSTSettingsNotes->SelectionStart = fcgTSTSettingsNotes->Text->Length;
        return false;
    }
    fcgTSTSettingsNotes->Visible = Enable;
    fcgTSLSettingsNotes->Visible = !Enable;
    if (Enable) {
        fcgTSTSettingsNotes->Text = fcgTSLSettingsNotes->Text;
        fcgTSTSettingsNotes->Focus();
        bool isDefaultNote = fcgTSLSettingsNotes->Overflow != ToolStripItemOverflow::Never;
        fcgTSTSettingsNotes->Select((isDefaultNote) ? 0 : fcgTSTSettingsNotes->Text->Length, fcgTSTSettingsNotes->Text->Length);
    } else {
        SetfcgTSLSettingsNotes(fcgTSTSettingsNotes->Text);
        CheckOtherChanges(nullptr, nullptr);
    }
    return true;
}


///////////////////  メモ関連  ///////////////////////////////////////////////
System::Void frmConfig::fcgTSLSettingsNotes_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
    EnableSettingsNoteChange(true);
}
System::Void frmConfig::fcgTSTSettingsNotes_Leave(System::Object^  sender, System::EventArgs^  e) {
    EnableSettingsNoteChange(false);
}
System::Void frmConfig::fcgTSTSettingsNotes_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
    if (e->KeyCode == Keys::Return)
        EnableSettingsNoteChange(false);
}
System::Void frmConfig::fcgTSTSettingsNotes_TextChanged(System::Object^  sender, System::EventArgs^  e) {
    SetfcgTSLSettingsNotes(fcgTSTSettingsNotes->Text);
    CheckOtherChanges(nullptr, nullptr);
}

//////////////////// 追加コマンド関連 /////////////////////////////////////////
System::Void frmConfig::fcgCXCmdExInsert_FontChanged(System::Object^  sender, System::EventArgs^  e) {
    InitCXCmdExInsert();
}
System::Void frmConfig::fcgCXCmdExInsert_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
    String^ insertStr;
    if (       0 == fcgCXCmdExInsert->SelectedIndex) {
        //何もしない
    } else if (1 == fcgCXCmdExInsert->SelectedIndex) {
        //WinXPにおいて、OpenFileDialogはCurrentDirctoryを勝手に変更しやがるので、
        //一度保存し、あとから再適用する
        String^ CurrentDir = Directory::GetCurrentDirectory();
        OpenFileDialog^ ofd = gcnew OpenFileDialog();
        ofd->FileName = L"";
        ofd->Multiselect = false;
        ofd->Filter = L"(*.*)|*.*";
        bool ret = (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK);
        if (ret) {
            if (sys_dat->exstg->s_local.get_relative_path)
                ofd->FileName = GetRelativePath(ofd->FileName, CurrentDir);
            insertStr = ofd->FileName;
        }
        Directory::SetCurrentDirectory(CurrentDir);
    } else {
        insertStr = String(REPLACE_STRINGS_LIST[fcgCXCmdExInsert->SelectedIndex-2].string).ToString();
    }
    if (insertStr != nullptr && insertStr->Length > 0) {
        int current_selection = fcgTXCmdEx->SelectionStart;
        fcgTXCmdEx->Text = fcgTXCmdEx->Text->Insert(fcgTXCmdEx->SelectionStart, insertStr);
        fcgTXCmdEx->SelectionStart = current_selection + insertStr->Length; //たまに変なところへ行くので念のため必要
        fcgTXCmdEx->Focus();
    }
    fcgCXCmdExInsert->SelectedIndex = 0;
}

System::Void frmConfig::AdjustCXDropDownWidth(ComboBox^ CX) {
    System::Drawing::Graphics^ ds = CX->CreateGraphics();
    float maxwidth = 0.0;
    for (int i = 0; i < CX->Items->Count; i++)
        maxwidth = std::max(maxwidth, ds->MeasureString(CX->Items[i]->ToString(), CX->Font).Width);
    CX->DropDownWidth = (int)(maxwidth + 0.5);
    delete ds;
}

System::Void frmConfig::InitCXCmdExInsert() {
    fcgCXCmdExInsert->BeginUpdate();
    fcgCXCmdExInsert->Items->Clear();
    fcgCXCmdExInsert->Items->Add(LOAD_CLI_STRING(AUO_CONFIG_INSERT_STRING));
    fcgCXCmdExInsert->Items->Add(LOAD_CLI_STRING(AUO_CONFIG_FILE_FULL_PATH));
    System::Drawing::Graphics^ ds = fcgCXCmdExInsert->CreateGraphics();
    float max_width_of_string = 0;
    for (int i = 0; REPLACE_STRINGS_LIST[i].desc; i++)
        max_width_of_string = std::max(max_width_of_string, ds->MeasureString(String(REPLACE_STRINGS_LIST[i].string).ToString() + L" … ", fcgCXCmdExInsert->Font).Width);
    for (int i = 0; REPLACE_STRINGS_LIST[i].desc; i++) {
        String^ AppenStr = String(REPLACE_STRINGS_LIST[i].string).ToString();
        const int length_of_string = AppenStr->Length;
        AppenStr += L" … ";
        for (float current_width = 0.0; current_width < max_width_of_string; AppenStr = AppenStr->Insert(length_of_string, L" "))
            current_width = ds->MeasureString(AppenStr, fcgCXCmdExInsert->Font).Width;
        String^ descStr = LOAD_CLI_STRING(REPLACE_STRINGS_LIST[i].mes);
        if (descStr->Length == 0) {
            descStr = String(REPLACE_STRINGS_LIST[i].desc).ToString();
        }
        AppenStr += descStr;
        fcgCXCmdExInsert->Items->Add(AppenStr);
    }
    delete ds;
    fcgCXCmdExInsert->SelectedIndex = 0;
    AdjustCXDropDownWidth(fcgCXCmdExInsert);
    fcgCXCmdExInsert->EndUpdate();
}

/////////////    音声設定関連の関数    ///////////////
System::Void frmConfig::fcgCBAudio2pass_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    if (fcgCBAudio2pass->Checked) {
        fcgCBAudioUsePipe->Checked = false;
        fcgCBAudioUsePipe->Enabled = false;
    } else if (CurrentPipeEnabled) {
        fcgCBAudioUsePipe->Checked = true;
        fcgCBAudioUsePipe->Enabled = true;
    }
}

System::Void frmConfig::fcgCXAudioEncoder_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
    setAudioDisplay();
}

System::Void frmConfig::fcgCXAudioEncMode_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
    AudioEncodeModeChanged();
}

System::Void frmConfig::fcgCBAudioUseExt_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
    fcgPNAudioExt->Visible = !fcgCBAudioUseInternal->Checked;
    fcgPNAudioInternal->Visible = fcgCBAudioUseInternal->Checked;
}

System::Int32 frmConfig::GetCurrentAudioDefaultBitrate() {
    AUDIO_SETTINGS *astg = (!fcgCBAudioUseInternal->Checked) ? &sys_dat->exstg->s_aud_ext[std::max(fcgCXAudioEncoder->SelectedIndex, 0)] : &sys_dat->exstg->s_aud_int[std::max(fcgCXAudioEncoderInternal->SelectedIndex, 0)];
    const int encMode = std::max((!fcgCBAudioUseInternal->Checked) ? fcgCXAudioEncMode->SelectedIndex : fcgCXAudioEncModeInternal->SelectedIndex, 0);
    return astg->mode[encMode].bitrate_default;
}

System::Void frmConfig::setAudioDisplay() {
    int index = fcgCXAudioEncoder->SelectedIndex;
    AUDIO_SETTINGS *astg = &sys_dat->exstg->s_aud_ext[index];
    //～の指定
    if (str_has_char(astg->filename)) {
        fcgLBAudioEncoderPath->Text = String(astg->filename).ToString() + LOAD_CLI_STRING(AUO_CONFIG_SPECIFY_EXE_PATH);
        fcgTXAudioEncoderPath->Enabled = true;
        fcgTXAudioEncoderPath->Text = LocalStg.audEncPath[index];
        fcgBTAudioEncoderPath->Enabled = true;
    } else {
        //filename空文字列(wav出力時)
        fcgLBAudioEncoderPath->Text = L"";
        fcgTXAudioEncoderPath->Enabled = false;
        fcgTXAudioEncoderPath->Text = L"";
        fcgBTAudioEncoderPath->Enabled = false;
    }
    fcgTXAudioEncoderPath->SelectionStart = fcgTXAudioEncoderPath->Text->Length;
    fcgCXAudioEncMode->BeginUpdate();
    fcgCXAudioEncMode->Items->Clear();
    for (int i = 0; i < astg->mode_count; i++)
        fcgCXAudioEncMode->Items->Add(String(astg->mode[i].name).ToString());
    fcgCXAudioEncMode->EndUpdate();
    bool pipe_enabled = (astg->pipe_input && !fcgCBAudio2pass->Checked);
    CurrentPipeEnabled = pipe_enabled;
    fcgCBAudioUsePipe->Enabled = pipe_enabled;
    fcgCBAudioUsePipe->Checked = pipe_enabled;
    if (fcgCXAudioEncMode->Items->Count > 0)
        fcgCXAudioEncMode->SelectedIndex = 0;
}

System::Void frmConfig::AudioEncodeModeChanged() {
    int index = fcgCXAudioEncMode->SelectedIndex;
    AUDIO_SETTINGS *astg = &sys_dat->exstg->s_aud_ext[fcgCXAudioEncoder->SelectedIndex];
    if (astg->mode[index].bitrate) {
        fcgCXAudioEncMode->Width = fcgCXAudioEncModeSmallWidth;
        fcgLBAudioBitrate->Visible = true;
        fcgNUAudioBitrate->Visible = true;
        fcgNUAudioBitrate->Minimum = astg->mode[index].bitrate_min;
        fcgNUAudioBitrate->Maximum = astg->mode[index].bitrate_max;
        fcgNUAudioBitrate->Increment = astg->mode[index].bitrate_step;
        SetNUValue(fcgNUAudioBitrate, (conf->aud.ext.bitrate != 0) ? conf->aud.ext.bitrate : astg->mode[index].bitrate_default);
    } else {
        fcgCXAudioEncMode->Width = fcgCXAudioEncModeLargeWidth;
        fcgLBAudioBitrate->Visible = false;
        fcgNUAudioBitrate->Visible = false;
        fcgNUAudioBitrate->Minimum = 0;
        fcgNUAudioBitrate->Maximum = 1536; //音声の最大レートは1536kbps
    }
    fcgCBAudio2pass->Enabled = astg->mode[index].enc_2pass != 0;
    if (!fcgCBAudio2pass->Enabled) fcgCBAudio2pass->Checked = false;
    SetfbcBTABEnable(fcgNUAudioBitrate->Visible, (int)fcgNUAudioBitrate->Maximum);

    bool delay_cut_available = astg->mode[index].delay > 0;
    fcgLBAudioDelayCut->Visible = delay_cut_available;
    fcgCXAudioDelayCut->Visible = delay_cut_available;
    if (delay_cut_available) {
        //edtsによるカットはmuxerを使用しないffmpegOutではサポートされない
        //const bool delay_cut_edts_available = str_has_char(astg->cmd_raw) && str_has_char(sys_dat->exstg->s_mux[MUXER_MP4_RAW].delay_cmd);
        const bool delay_cut_edts_available = false;
        const int current_idx = fcgCXAudioDelayCut->SelectedIndex;
        const int items_to_set = _countof(AUDIO_DELAY_CUT_MODE) - 1 - ((delay_cut_edts_available) ? 0 : 1);
        fcgCXAudioDelayCut->BeginUpdate();
        fcgCXAudioDelayCut->Items->Clear();
        for (int i = 0; i < items_to_set; i++) {
            String^ string = nullptr;
            if (AUDIO_DELAY_CUT_MODE[i].mes != AUO_MES_UNKNOWN) {
                string = LOAD_CLI_STRING(AUDIO_DELAY_CUT_MODE[i].mes);
            }
            if (string == nullptr || string->Length == 0) {
                string = String(AUDIO_DELAY_CUT_MODE[i].desc).ToString();
            }
            fcgCXAudioDelayCut->Items->Add(string);
        }
        fcgCXAudioDelayCut->EndUpdate();
        fcgCXAudioDelayCut->SelectedIndex = (current_idx >= items_to_set) ? 0 : current_idx;
    } else {
        fcgCXAudioDelayCut->SelectedIndex = 0;
    }
}

System::Void frmConfig::fcgCXAudioEncoderInternal_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e) {
    setAudioIntDisplay();
}
System::Void frmConfig::fcgCXAudioEncModeInternal_SelectedIndexChanged(System::Object ^sender, System::EventArgs ^e) {
    AudioIntEncodeModeChanged();
}

System::Void frmConfig::setAudioIntDisplay() {
    AUDIO_SETTINGS *astg = &sys_dat->exstg->s_aud_int[fcgCXAudioEncoderInternal->SelectedIndex];
    fcgCXAudioEncModeInternal->BeginUpdate();
    fcgCXAudioEncModeInternal->Items->Clear();
    for (int i = 0; i < astg->mode_count; i++) {
        fcgCXAudioEncModeInternal->Items->Add(String(astg->mode[i].name).ToString());
    }
    fcgCXAudioEncModeInternal->EndUpdate();
    if (fcgCXAudioEncModeInternal->Items->Count > 0)
        fcgCXAudioEncModeInternal->SelectedIndex = 0;
}
System::Void frmConfig::AudioIntEncodeModeChanged() {
    const int imode = fcgCXAudioEncModeInternal->SelectedIndex;
    if (imode >= 0 && fcgCXAudioEncoderInternal->SelectedIndex >= 0) {
        AUDIO_SETTINGS *astg = &sys_dat->exstg->s_aud_int[fcgCXAudioEncoderInternal->SelectedIndex];
        if (astg->mode[imode].bitrate) {
            fcgCXAudioEncModeInternal->Width = fcgCXAudioEncModeSmallWidth;
            fcgLBAudioBitrateInternal->Visible = true;
            fcgNUAudioBitrateInternal->Visible = true;
            fcgNUAudioBitrateInternal->Minimum = astg->mode[imode].bitrate_min;
            fcgNUAudioBitrateInternal->Maximum = astg->mode[imode].bitrate_max;
            fcgNUAudioBitrateInternal->Increment = astg->mode[imode].bitrate_step;
            SetNUValue(fcgNUAudioBitrateInternal, (conf->aud.in.bitrate > 0) ? conf->aud.in.bitrate : astg->mode[imode].bitrate_default);
        } else {
            fcgCXAudioEncModeInternal->Width = fcgCXAudioEncModeLargeWidth;
            fcgLBAudioBitrateInternal->Visible = false;
            fcgNUAudioBitrateInternal->Visible = false;
            fcgNUAudioBitrateInternal->Minimum = 0;
            fcgNUAudioBitrateInternal->Maximum = 65536;
        }
    }
    SetfbcBTABEnable(fcgNUAudioBitrateInternal->Visible, (int)fcgNUAudioBitrateInternal->Maximum);
}

///////////////   設定ファイル関連   //////////////////////
System::Void frmConfig::CheckTSItemsEnabled(CONF_GUIEX *current_conf) {
    bool selected = (CheckedStgMenuItem != nullptr);
    fcgTSBSave->Enabled = (selected && memcmp(cnf_stgSelected, current_conf, sizeof(CONF_GUIEX)));
    fcgTSBDelete->Enabled = selected;
}

System::Void frmConfig::UncheckAllDropDownItem(ToolStripItem^ mItem) {
    ToolStripDropDownItem^ DropDownItem = dynamic_cast<ToolStripDropDownItem^>(mItem);
    if (DropDownItem == nullptr)
        return;
    for (int i = 0; i < DropDownItem->DropDownItems->Count; i++) {
        UncheckAllDropDownItem(DropDownItem->DropDownItems[i]);
        ToolStripMenuItem^ item = dynamic_cast<ToolStripMenuItem^>(DropDownItem->DropDownItems[i]);
        if (item != nullptr)
            item->Checked = false;
    }
}

System::Void frmConfig::CheckTSSettingsDropDownItem(ToolStripMenuItem^ mItem) {
    UncheckAllDropDownItem(fcgTSSettings);
    CheckedStgMenuItem = mItem;
    fcgTSSettings->Text = (mItem == nullptr) ? LOAD_CLI_STRING(AUO_CONFIG_PROFILE) : mItem->Text;
    if (mItem != nullptr)
        mItem->Checked = true;
    fcgTSBSave->Enabled = false;
    fcgTSBDelete->Enabled = (mItem != nullptr);
}

ToolStripMenuItem^ frmConfig::fcgTSSettingsSearchItem(String^ stgPath, ToolStripItem^ mItem) {
    if (stgPath == nullptr)
        return nullptr;
    ToolStripDropDownItem^ DropDownItem = dynamic_cast<ToolStripDropDownItem^>(mItem);
    if (DropDownItem == nullptr)
        return nullptr;
    for (int i = 0; i < DropDownItem->DropDownItems->Count; i++) {
        ToolStripMenuItem^ item = fcgTSSettingsSearchItem(stgPath, DropDownItem->DropDownItems[i]);
        if (item != nullptr)
            return item;
        item = dynamic_cast<ToolStripMenuItem^>(DropDownItem->DropDownItems[i]);
        if (item      != nullptr &&
            item->Tag != nullptr &&
            0 == String::Compare(item->Tag->ToString(), stgPath, true))
            return item;
    }
    return nullptr;
}

ToolStripMenuItem^ frmConfig::fcgTSSettingsSearchItem(String^ stgPath) {
    return fcgTSSettingsSearchItem((stgPath != nullptr && stgPath->Length > 0) ? Path::GetFullPath(stgPath) : nullptr, fcgTSSettings);
}

System::Void frmConfig::SaveToStgFile(String^ stgName) {
    size_t nameLen = CountStringBytes(stgName) + 1;
    TCHAR *stg_name = (TCHAR *)malloc(nameLen * sizeof(stg_name[0]));
    GetWCHARfromString(stg_name, (DWORD)nameLen, stgName);
    init_CONF_GUIEX(cnf_stgSelected, FALSE);
    FrmToConf(cnf_stgSelected);
    String^ stgDir = Path::GetDirectoryName(stgName);
    if (!Directory::Exists(stgDir))
        Directory::CreateDirectory(stgDir);
    int result = guiEx_config::save_guiEx_conf(cnf_stgSelected, stg_name);
    free(stg_name);
    switch (result) {
        case CONF_ERROR_FILE_OPEN:
            MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_OPEN_STG_FILE), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        case CONF_ERROR_INVALID_FILENAME:
            MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_INVALID_CHAR), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
            return;
        default:
            break;
    }
    init_CONF_GUIEX(cnf_stgSelected, FALSE);
    FrmToConf(cnf_stgSelected);
}

System::Void frmConfig::fcgTSBSave_Click(System::Object^  sender, System::EventArgs^  e) {
    if (CheckedStgMenuItem != nullptr)
        SaveToStgFile(CheckedStgMenuItem->Tag->ToString());
    CheckTSSettingsDropDownItem(CheckedStgMenuItem);
}

System::Void frmConfig::fcgTSBSaveNew_Click(System::Object^  sender, System::EventArgs^  e) {
    frmSaveNewStg::Instance::get()->setStgDir(String(sys_dat->exstg->s_local.stg_dir).ToString());
    frmSaveNewStg::Instance::get()->SetTheme(themeMode, dwStgReader);
    if (CheckedStgMenuItem != nullptr)
        frmSaveNewStg::Instance::get()->setFilename(CheckedStgMenuItem->Text);
    frmSaveNewStg::Instance::get()->ShowDialog();
    String^ stgName = frmSaveNewStg::Instance::get()->StgFileName;
    if (stgName != nullptr && stgName->Length)
        SaveToStgFile(stgName);
    RebuildStgFileDropDown(nullptr);
    CheckTSSettingsDropDownItem(fcgTSSettingsSearchItem(stgName));
}

System::Void frmConfig::DeleteStgFile(ToolStripMenuItem^ mItem) {
    if (System::Windows::Forms::DialogResult::OK ==
        MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ASK_STG_FILE_DELETE) + L"[" + mItem->Text + L"]",
        LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OKCancel, MessageBoxIcon::Exclamation))
    {
        File::Delete(mItem->Tag->ToString());
        RebuildStgFileDropDown(nullptr);
        CheckTSSettingsDropDownItem(nullptr);
        SetfcgTSLSettingsNotes(L"");
    }
}

System::Void frmConfig::fcgTSBDelete_Click(System::Object^  sender, System::EventArgs^  e) {
    DeleteStgFile(CheckedStgMenuItem);
}

System::Void frmConfig::fcgTSSettings_DropDownItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
    ToolStripMenuItem^ ClickedMenuItem = dynamic_cast<ToolStripMenuItem^>(e->ClickedItem);
    if (ClickedMenuItem == nullptr)
        return;
    if (ClickedMenuItem->Tag == nullptr || ClickedMenuItem->Tag->ToString()->Length == 0)
        return;
    CONF_GUIEX load_stg;
    TCHAR stg_path[MAX_PATH_LEN];
    GetWCHARfromString(stg_path, _countof(stg_path), ClickedMenuItem->Tag->ToString());
    if (guiEx_config::load_guiEx_conf(&load_stg, stg_path) == CONF_ERROR_FILE_OPEN) {
        if (MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_OPEN_STG_FILE) + L"\n"
                           + LOAD_CLI_STRING(AUO_CONFIG_ASK_STG_FILE_DELETE),
                           LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::YesNo, MessageBoxIcon::Error)
                           == System::Windows::Forms::DialogResult::Yes)
            DeleteStgFile(ClickedMenuItem);
        return;
    }
    ConfToFrm(&load_stg);
    CheckTSSettingsDropDownItem(ClickedMenuItem);
    memcpy(cnf_stgSelected, &load_stg, sizeof(CONF_GUIEX));
}

System::Void frmConfig::RebuildStgFileDropDown(ToolStripDropDownItem^ TS, String^ dir) {
    array<String^>^ subDirs = Directory::GetDirectories(dir);
    for (int i = 0; i < subDirs->Length; i++) {
        ToolStripMenuItem^ DDItem = gcnew ToolStripMenuItem(L"[ " + subDirs[i]->Substring(dir->Length+1) + L" ]");
        DDItem->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &frmConfig::fcgTSSettings_DropDownItemClicked);
        DDItem->ForeColor = Color::Blue;
        DDItem->Tag = nullptr;
        RebuildStgFileDropDown(DDItem, subDirs[i]);
        TS->DropDownItems->Add(DDItem);
    }
    array<String^>^ stgList = Directory::GetFiles(dir, L"*.stg");
    for (int i = 0; i < stgList->Length; i++) {
        ToolStripMenuItem^ mItem = gcnew ToolStripMenuItem(Path::GetFileNameWithoutExtension(stgList[i]));
        mItem->Tag = stgList[i];
        TS->DropDownItems->Add(mItem);
    }
}

System::Void frmConfig::RebuildStgFileDropDown(String^ stgDir) {
    fcgTSSettings->DropDownItems->Clear();
    if (stgDir != nullptr)
        CurrentStgDir = stgDir;
    if (!Directory::Exists(CurrentStgDir))
        Directory::CreateDirectory(CurrentStgDir);
    RebuildStgFileDropDown(fcgTSSettings, Path::GetFullPath(CurrentStgDir));
}

///////////////   言語ファイル関連   //////////////////////

System::Void frmConfig::CheckTSLanguageDropDownItem(ToolStripMenuItem^ mItem) {
    UncheckAllDropDownItem(fcgTSLanguage);
    fcgTSLanguage->Text = (mItem == nullptr) ? LOAD_CLI_STRING(AuofcgTSSettings) : mItem->Text;
    if (mItem != nullptr)
        mItem->Checked = true;
}
System::Void frmConfig::SetSelectedLanguage(const TCHAR *language_text) {
    for (int i = 0; i < fcgTSLanguage->DropDownItems->Count; i++) {
        ToolStripMenuItem^ item = dynamic_cast<ToolStripMenuItem^>(fcgTSLanguage->DropDownItems[i]);
        TCHAR item_text[MAX_PATH_LEN];
        GetWCHARfromString(item_text, _countof(item_text), item->Tag->ToString());
        if (_tcsncmp(item_text, language_text, _tcslen(language_text)) == 0) {
            CheckTSLanguageDropDownItem(item);
            break;
        }
    }
}

System::Void frmConfig::SaveSelectedLanguage(const TCHAR *language_text) {
    sys_dat->exstg->set_and_save_lang(language_text);
}

System::Void frmConfig::fcgTSLanguage_DropDownItemClicked(System::Object^  sender, System::Windows::Forms::ToolStripItemClickedEventArgs^  e) {
    ToolStripMenuItem^ ClickedMenuItem = dynamic_cast<ToolStripMenuItem^>(e->ClickedItem);
    if (ClickedMenuItem == nullptr)
        return;
    if (ClickedMenuItem->Tag == nullptr || ClickedMenuItem->Tag->ToString()->Length == 0)
        return;

    TCHAR language_text[MAX_PATH_LEN];
    GetWCHARfromString(language_text, _countof(language_text), ClickedMenuItem->Tag->ToString());
    SaveSelectedLanguage(language_text);
    load_lng(language_text);
    overwrite_aviutl_ini_auo_info();
    LoadLangText();
    CheckTSLanguageDropDownItem(ClickedMenuItem);
}

System::Void frmConfig::InitLangList() {
    if (list_lng != nullptr) {
        delete list_lng;
    }
#define ENABLE_LNG_FILE_DETECT 1
#if ENABLE_LNG_FILE_DETECT
    auto lnglist = find_lng_files();
    list_lng = new std::vector<tstring>();
    for (const auto& lang : lnglist) {
        list_lng->push_back(lang);
    }
#endif

    fcgTSLanguage->DropDownItems->Clear();

    for (const auto& auo_lang : list_auo_languages) {
        String^ label = String(auo_lang.code).ToString() + L" (" + String(auo_lang.name).ToString() + L")";
        ToolStripMenuItem^ mItem = gcnew ToolStripMenuItem(label);
        mItem->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &frmConfig::fcgTSLanguage_DropDownItemClicked);
        mItem->Tag = String(auo_lang.code).ToString();
        fcgTSLanguage->DropDownItems->Add(mItem);
    }
#if ENABLE_LNG_FILE_DETECT
    for (size_t i = 0; i < list_lng->size(); i++) {
        auto filename = String(PathFindFileName((*list_lng)[i].c_str())).ToString();
        ToolStripMenuItem^ mItem = gcnew ToolStripMenuItem(filename);
        mItem->DropDownItemClicked += gcnew System::Windows::Forms::ToolStripItemClickedEventHandler(this, &frmConfig::fcgTSLanguage_DropDownItemClicked);
        mItem->Tag = filename;
        fcgTSLanguage->DropDownItems->Add(mItem);
    }
#endif
    SetSelectedLanguage(sys_dat->exstg->get_lang());
}

//////////////   初期化関連     ////////////////
System::Void frmConfig::InitData(CONF_GUIEX *set_config, const SYSTEM_DATA *system_data) {
    if (set_config->header.size_all != CONF_INITIALIZED) {
        //初期化されていなければ初期化する
        init_CONF_GUIEX(set_config, FALSE);
    }
    conf = set_config;
    sys_dat = system_data;
}

System::Void frmConfig::InitComboBox() {
    //コンボボックスに値を設定する
    setComboBox(fcgCXAudioTempDir,   audtempdir_desc);
    setComboBox(fcgCXMP4BoxTempDir,  mp4boxtempdir_desc);
    setComboBox(fcgCXOutputCsp,      list_output_csp);
    setComboBox(fcgCXTempDir,        tempdir_desc);
    setComboBox(fcgCXInterlaced,     interlaced_desc);

    setComboBox(fcgCXAudioEncTiming, audio_enc_timing_desc);
    setComboBox(fcgCXAudioDelayCut,  AUDIO_DELAY_CUT_MODE);

    InitCXCmdExInsert();

    setMuxerCmdExNames(fcgCXMP4CmdEx, MUXER_MP4);
    setMuxerCmdExNames(fcgCXMKVCmdEx, MUXER_MKV);
    setAudioEncoderNames();

    setPriorityList(fcgCXffmpegOutPriority);
    setPriorityList(fcgCXMuxPriority);
    setPriorityList(fcgCXAudioPriority);
}

System::Void frmConfig::SetTXMaxLen(TextBox^ TX, int max_len) {
    TX->MaxLength = max_len;
    TX->Validating += gcnew System::ComponentModel::CancelEventHandler(this, &frmConfig::TX_LimitbyBytes);
}

System::Void frmConfig::SetTXMaxLenAll() {
    //MaxLengthに最大文字数をセットし、それをもとにバイト数計算を行うイベントをセットする。
    SetTXMaxLen(fcgTXCmdEx,                _countof(conf->vid.cmdex) - 1);
    SetTXMaxLen(fcgTXInCmd,                _countof(conf->enc.incmd) - 1);
    SetTXMaxLen(fcgTXVideoEncoderPath,     _countof(sys_dat->exstg->s_enc.fullpath) - 1);
    SetTXMaxLen(fcgTXAudioEncoderPath,     _countof(sys_dat->exstg->s_aud_ext[0].fullpath) - 1);
    SetTXMaxLen(fcgTXMP4MuxerPath,         _countof(sys_dat->exstg->s_mux[MUXER_MP4].fullpath) - 1);
    SetTXMaxLen(fcgTXMKVMuxerPath,         _countof(sys_dat->exstg->s_mux[MUXER_MKV].fullpath) - 1);
    SetTXMaxLen(fcgTXTC2MP4Path,           _countof(sys_dat->exstg->s_mux[MUXER_TC2MP4].fullpath) - 1);
    SetTXMaxLen(fcgTXMP4RawPath,           _countof(sys_dat->exstg->s_mux[MUXER_MP4_RAW].fullpath) - 1);
    SetTXMaxLen(fcgTXCustomTempDir,        _countof(sys_dat->exstg->s_local.custom_tmp_dir) - 1);
    SetTXMaxLen(fcgTXCustomAudioTempDir,   _countof(sys_dat->exstg->s_local.custom_audio_tmp_dir) - 1);
    SetTXMaxLen(fcgTXMP4BoxTempDir,        _countof(sys_dat->exstg->s_local.custom_mp4box_tmp_dir) - 1);
    SetTXMaxLen(fcgTXBatBeforePath,        _countof(conf->oth.batfile.before_process) - 1);
    SetTXMaxLen(fcgTXBatAfterPath,         _countof(conf->oth.batfile.after_process) - 1);

    SetTXMaxLen(fcgTXOutputExt,            _countof(conf->enc.outext) - 1);

    fcgTSTSettingsNotes->MaxLength     =   _countof(conf->oth.notes) - 1;
}

System::Void frmConfig::InitStgFileList() {
    RebuildStgFileDropDown(String(sys_dat->exstg->s_local.stg_dir).ToString());
    stgChanged = false;
    CheckTSSettingsDropDownItem(nullptr);
}

System::Void frmConfig::fcgChangeMuxerVisible(System::Object^  sender, System::EventArgs^  e) {
    //tc2mp4のチェック
    const bool enable_tc2mp4_muxer = (0 != str_has_char(sys_dat->exstg->s_mux[MUXER_TC2MP4].base_cmd));
    fcgTXTC2MP4Path->Visible = enable_tc2mp4_muxer;
    fcgLBTC2MP4Path->Visible = enable_tc2mp4_muxer;
    fcgBTTC2MP4Path->Visible = enable_tc2mp4_muxer;
    //mp4 rawのチェック
    const bool enable_mp4raw_muxer = (0 != str_has_char(sys_dat->exstg->s_mux[MUXER_MP4_RAW].base_cmd));
    fcgTXMP4RawPath->Visible = enable_mp4raw_muxer;
    fcgLBMP4RawPath->Visible = enable_mp4raw_muxer;
    fcgBTMP4RawPath->Visible = enable_mp4raw_muxer;
    //一時フォルダのチェック
    const bool enable_mp4_tmp = (0 != str_has_char(sys_dat->exstg->s_mux[MUXER_MP4].tmp_cmd));
    fcgCXMP4BoxTempDir->Visible = enable_mp4_tmp;
    fcgLBMP4BoxTempDir->Visible = enable_mp4_tmp;
    fcgTXMP4BoxTempDir->Visible = enable_mp4_tmp;
    fcgBTMP4BoxTempDir->Visible = enable_mp4_tmp;
    //Apple Chapterのチェック
    bool enable_mp4_apple_cmdex = false;
    for (int i = 0; i < sys_dat->exstg->s_mux[MUXER_MP4].ex_count; i++)
        enable_mp4_apple_cmdex |= (0 != str_has_char(sys_dat->exstg->s_mux[MUXER_MP4].ex_cmd[i].cmd_apple));
    fcgCBMP4MuxApple->Visible = enable_mp4_apple_cmdex;

    //位置の調整
    static const int HEIGHT = 31;
    fcgLBTC2MP4Path->Location = Point(fcgLBTC2MP4Path->Location.X, fcgLBMP4MuxerPath->Location.Y + HEIGHT * enable_tc2mp4_muxer);
    fcgTXTC2MP4Path->Location = Point(fcgTXTC2MP4Path->Location.X, fcgTXMP4MuxerPath->Location.Y + HEIGHT * enable_tc2mp4_muxer);
    fcgBTTC2MP4Path->Location = Point(fcgBTTC2MP4Path->Location.X, fcgBTMP4MuxerPath->Location.Y + HEIGHT * enable_tc2mp4_muxer);
    fcgLBMP4RawPath->Location = Point(fcgLBMP4RawPath->Location.X, fcgLBTC2MP4Path->Location.Y   + HEIGHT * enable_mp4raw_muxer);
    fcgTXMP4RawPath->Location = Point(fcgTXMP4RawPath->Location.X, fcgTXTC2MP4Path->Location.Y   + HEIGHT * enable_mp4raw_muxer);
    fcgBTMP4RawPath->Location = Point(fcgBTMP4RawPath->Location.X, fcgBTTC2MP4Path->Location.Y   + HEIGHT * enable_mp4raw_muxer);

    fcgtabControlMux->Visible = false;
}

System::Void frmConfig::SetStgEscKey(bool Enable) {
    if (this->KeyPreview == Enable)
        return;
    this->KeyPreview = Enable;
    if (Enable)
        this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &frmConfig::frmConfig_KeyDown);
    else
        this->KeyDown -= gcnew System::Windows::Forms::KeyEventHandler(this, &frmConfig::frmConfig_KeyDown);
}

System::Void frmConfig::AdjustLocation() {
    //デスクトップ領域(タスクバー等除く)
    System::Drawing::Rectangle screen = System::Windows::Forms::Screen::GetWorkingArea(this);
    //現在のデスクトップ領域の座標
    Point CurrentDesktopLocation = this->DesktopLocation::get();
    //チェック開始
    bool ChangeLocation = false;
    if (CurrentDesktopLocation.X + this->Size.Width > screen.Width) {
        ChangeLocation = true;
        CurrentDesktopLocation.X = clamp(screen.X - this->Size.Width, 4, CurrentDesktopLocation.X);
    }
    if (CurrentDesktopLocation.Y + this->Size.Height > screen.Height) {
        ChangeLocation = true;
        CurrentDesktopLocation.Y = clamp(screen.Y - this->Size.Height, 4, CurrentDesktopLocation.Y);
    }
    if (ChangeLocation) {
        this->StartPosition = FormStartPosition::Manual;
        this->DesktopLocation::set(CurrentDesktopLocation);
    }
}

System::Void frmConfig::InitForm() {
    //UIテーマ切り替え
    CheckTheme();
    //言語設定ファイルのロード
    InitLangList();
    //設定ファイル集の初期化
    InitStgFileList();
    //言語表示
    LoadLangText();
    //パラメータセット
    ConfToFrm(conf);
    //イベントセット
    SetTXMaxLenAll(); //テキストボックスの最大文字数
    SetAllCheckChangedEvents(this); //変更の確認,ついでにNUの
    //フォームの変更可不可を更新
    fcgChangeMuxerVisible(nullptr, nullptr);
    fcgCBAudioUseExt_CheckedChanged(nullptr, nullptr);

    EnableSettingsNoteChange(false);
    ExeTXPathLeave();
    //表示位置の調整
    AdjustLocation();
    //キー設定
    SetStgEscKey(sys_dat->exstg->s_local.enable_stg_esc_key != 0);
    //フォントの設定
    if (str_has_char(sys_dat->exstg->s_local.conf_font.name))
        SetFontFamilyToForm(this, gcnew FontFamily(String(sys_dat->exstg->s_local.conf_font.name).ToString()), this->Font->FontFamily);
}

System::Void frmConfig::LoadLangText() {
    //一度ウィンドウの再描画を完全に抑止する
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
    //空白時にグレーで入れる文字列を言語変更のため一度空白に戻す
    ExeTXPathEnter();
    //言語更新開始
    LOAD_CLI_TEXT(fcgtabPageExSettings);
    LOAD_CLI_TEXT(fcgCBAuoTcfileout);
    LOAD_CLI_TEXT(fcgCBAudioInput);
    LOAD_CLI_TEXT(fcgCB2passEnc);
    LOAD_CLI_TEXT(fcgLBOutputExt);
    LOAD_CLI_TEXT(fcgLBOutputCsp);
    LOAD_CLI_TEXT(fcgLBInterlaced);
    LOAD_CLI_TEXT(fcgBTVideoEncoderPath);
    LOAD_CLI_TEXT(fcgLBVideoEncoderPath);
    LOAD_CLI_TEXT(fcgLBTempDir);
    LOAD_CLI_TEXT(fcgBTCustomTempDir);
    LOAD_CLI_TEXT(fcggroupBoxCmdEx);
    LOAD_CLI_TEXT(fcgLBInCmd);
    LOAD_CLI_TEXT(fcgLBffmpegOutPriority);
    LOAD_CLI_TEXT(fcggroupBoxAudio);
    LOAD_CLI_TEXT(fcgCBAudioUseInternal);
    LOAD_CLI_TEXT(fcgCBFAWCheck);
    LOAD_CLI_TEXT(fcgLBAudioBitrateInternal);
    LOAD_CLI_TEXT(fcgLBAudioEncModeInternal);
    LOAD_CLI_TEXT(fcgLBAudioDelayCut);
    LOAD_CLI_TEXT(fcgBTCustomAudioTempDir);
    LOAD_CLI_TEXT(fcgCBAudioUsePipe);
    LOAD_CLI_TEXT(fcgLBAudioBitrate);
    LOAD_CLI_TEXT(fcgLBAudioTemp);
    LOAD_CLI_TEXT(fcgBTAudioEncoderPath);
    LOAD_CLI_TEXT(fcgCBAudio2pass);
    LOAD_CLI_TEXT(fcgLBAudioEncoder);
    LOAD_CLI_TEXT(fcgLBAudioPriority);
    LOAD_CLI_TEXT(fcgLBAudioEncoderPath);
    LOAD_CLI_TEXT(fcgLBAudioEncMode);
    LOAD_CLI_TEXT(fcgCBAudioEncTiming);
    LOAD_CLI_TEXT(fcgtoolStripSettings);
    LOAD_CLI_TEXT(fcgTSBSave);
    LOAD_CLI_TEXT(fcgTSBSaveNew);
    LOAD_CLI_TEXT(fcgTSBDelete);
    LOAD_CLI_TEXT(fcgTSSettings);
    LOAD_CLI_TEXT(fcgTSExeFileshelp);
    LOAD_CLI_TEXT(fcgTSBCMDOnly);
    LOAD_CLI_TEXT(fcgTSBBitrateCalc);
    LOAD_CLI_TEXT(fcgTSBOtherSettings);
    LOAD_CLI_TEXT(fcgTSLSettingsNotes);
    LOAD_CLI_TEXT(fcgTSTSettingsNotes);
    LOAD_CLI_TEXT(fcgtabPageMP4);
    LOAD_CLI_TEXT(fcgBTMP4RawPath);
    LOAD_CLI_TEXT(fcgLBMP4RawPath);
    LOAD_CLI_TEXT(fcgCBMP4MuxApple);
    LOAD_CLI_TEXT(fcgBTMP4BoxTempDir);
    LOAD_CLI_TEXT(fcgLBMP4BoxTempDir);
    LOAD_CLI_TEXT(fcgBTTC2MP4Path);
    LOAD_CLI_TEXT(fcgBTMP4MuxerPath);
    LOAD_CLI_TEXT(fcgLBTC2MP4Path);
    LOAD_CLI_TEXT(fcgLBMP4MuxerPath);
    LOAD_CLI_TEXT(fcgLBMP4CmdEx);
    LOAD_CLI_TEXT(fcgCBMP4MuxerExt);
    LOAD_CLI_TEXT(fcgtabPageMKV);
    LOAD_CLI_TEXT(fcgBTMKVMuxerPath);
    LOAD_CLI_TEXT(fcgLBMKVMuxerPath);
    LOAD_CLI_TEXT(fcgLBMKVMuxerCmdEx);
    LOAD_CLI_TEXT(fcgCBMKVMuxerExt);
    LOAD_CLI_TEXT(fcgtabPageMPG);
    LOAD_CLI_TEXT(fcgBTMPGMuxerPath);
    LOAD_CLI_TEXT(fcgLBMPGMuxerPath);
    LOAD_CLI_TEXT(fcgLBMPGMuxerCmdEx);
    LOAD_CLI_TEXT(fcgCBMPGMuxerExt);
    LOAD_CLI_TEXT(fcgtabPageMux);
    LOAD_CLI_TEXT(fcgLBMuxPriority);
    LOAD_CLI_TEXT(fcgCBMuxMinimize);
    LOAD_CLI_TEXT(fcgtabPageBat);
    LOAD_CLI_TEXT(fcgLBBatAfterString);
    LOAD_CLI_TEXT(fcgLBBatBeforeString);
    LOAD_CLI_TEXT(fcgBTBatBeforePath);
    LOAD_CLI_TEXT(fcgLBBatBeforePath);
    LOAD_CLI_TEXT(fcgCBWaitForBatBefore);
    LOAD_CLI_TEXT(fcgCBRunBatBefore);
    LOAD_CLI_TEXT(fcgBTBatAfterPath);
    LOAD_CLI_TEXT(fcgLBBatAfterPath);
    LOAD_CLI_TEXT(fcgCBWaitForBatAfter);
    LOAD_CLI_TEXT(fcgCBRunBatAfter);
    LOAD_CLI_TEXT(fcgLBguiExBlog);
    LOAD_CLI_TEXT(fcgBTCancel);
    LOAD_CLI_TEXT(fcgBTOK);
    LOAD_CLI_TEXT(fcgBTDefault);
    LOAD_CLI_TEXT(fcgLBVersionDate);
    LOAD_CLI_TEXT(fcgLBVersion);

    //ローカル設定のロード(ini変更を反映)
    LoadLocalStg();
    //ローカル設定の反映
    SetLocalStg();
    //コンボボックスの値を設定
    InitComboBox();
    //ツールチップ
    SetHelpToolTips();
    ActivateToolTip(sys_dat->exstg->s_local.disable_tooltip_help == FALSE);
    { //タイトル表示,バージョン情報,コンパイル日時
        auto auo_full_name = g_auo_mes.get(AUO_GUIEX_FULL_NAME);
        if (auo_full_name == nullptr || wcslen(auo_full_name) == 0) auo_full_name = AUO_FULL_NAME_W;
        this->Text = String(auo_full_name).ToString();
        fcgLBVersion->Text = String(auo_full_name).ToString() + L" " + String(AUO_VERSION_STR_W).ToString();
        fcgLBVersionDate->Text = L"build " + String(__DATE__).ToString() + L" " + String(__TIME__).ToString();
    }
    //空白時にグレーで入れる文字列を言語に即して復活させる
    ExeTXPathLeave();
    //一度ウィンドウの再描画を再開し、強制的に再描画させる
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
    this->Refresh();
}

/////////////         データ <-> GUI     /////////////
System::Void frmConfig::ConfToFrm(CONF_GUIEX *cnf) {
    this->SuspendLayout();

    //動画部
    SetCXIndex(fcgCXInterlaced,          cnf->enc.interlaced);
    SetCXIndex(fcgCXOutputCsp,           cnf->enc.output_csp);
    SetCXIndex(fcgCXffmpegOutPriority,    cnf->vid.priority);
    SetCXIndex(fcgCXTempDir,             cnf->oth.temp_dir);
    fcgCB2passEnc->Checked             = cnf->enc.use_auto_npass != 0;
    fcgCBAudioInput->Checked           = cnf->enc.audio_input != 0;
    fcgCBAuoTcfileout->Checked          = cnf->vid.auo_tcfile_out != 0;

    fcgTXCmdEx->Text                   = String(cnf->vid.cmdex).ToString();
    fcgTXOutputExt->Text               = String(cnf->enc.outext).ToString();
    fcgTXInCmd->Text                   = String(cnf->enc.incmd).ToString();

    //音声
    fcgCBAudioUseInternal->Checked     = cnf->aud.use_internal != 0;
    fcgCBFAWCheck->Checked             = cnf->aud.ext.faw_check != 0;
    SetCXIndex(fcgCXAudioEncoder,        cnf->aud.ext.encoder);
    fcgCBAudio2pass->Checked           = cnf->aud.ext.use_2pass != 0;
    fcgCBAudioUsePipe->Checked = (CurrentPipeEnabled && !cnf->aud.ext.use_wav);
    SetCXIndex(fcgCXAudioDelayCut,       cnf->aud.ext.delay_cut);
    SetCXIndex(fcgCXAudioEncMode,        cnf->aud.ext.enc_mode);
    SetNUValue(fcgNUAudioBitrate,       (cnf->aud.ext.bitrate != 0) ? cnf->aud.ext.bitrate : GetCurrentAudioDefaultBitrate());
    SetCXIndex(fcgCXAudioPriority,       cnf->aud.ext.priority);
    SetCXIndex(fcgCXAudioTempDir,        cnf->aud.ext.aud_temp_dir);
    SetCXIndex(fcgCXAudioEncTiming,      1);
    SetCXIndex(fcgCXAudioEncoderInternal, cnf->aud.in.encoder);
    SetCXIndex(fcgCXAudioEncModeInternal, cnf->aud.in.enc_mode);
    SetNUValue(fcgNUAudioBitrateInternal, (cnf->aud.in.bitrate != 0) ? cnf->aud.in.bitrate : GetCurrentAudioDefaultBitrate());

    //mux
    /*
    fcgCBMP4MuxerExt->Checked          = cnf->mux.disable_mp4ext == 0;
    fcgCBMP4MuxApple->Checked          = cnf->mux.apple_mode != 0;
    SetCXIndex(fcgCXMP4CmdEx,            cnf->mux.mp4_mode);
    SetCXIndex(fcgCXMP4BoxTempDir,       cnf->mux.mp4_temp_dir);
    fcgCBMKVMuxerExt->Checked          = cnf->mux.disable_mkvext == 0;
    SetCXIndex(fcgCXMKVCmdEx,            cnf->mux.mkv_mode);
    fcgCBMPGMuxerExt->Checked          = cnf->mux.disable_mpgext == 0;
    SetCXIndex(fcgCXMPGCmdEx,            cnf->mux.mpg_mode);
    fcgCBMuxMinimize->Checked          = cnf->mux.minimized != 0;
    SetCXIndex(fcgCXMuxPriority,         cnf->mux.priority);

    fcgCBRunBatBefore->Checked         =(cnf->oth.run_bat & RUN_BAT_BEFORE) != 0;
    fcgCBRunBatAfter->Checked          =(cnf->oth.run_bat & RUN_BAT_AFTER) != 0;
    fcgCBWaitForBatBefore->Checked     =(cnf->oth.dont_wait_bat_fin & RUN_BAT_BEFORE) == 0;
    fcgCBWaitForBatAfter->Checked      =(cnf->oth.dont_wait_bat_fin & RUN_BAT_AFTER) == 0;
    fcgTXBatPath->Text                 = String(cnf->oth.batfile).ToString();
    */

    SetfcgTSLSettingsNotes(cnf->oth.notes);

    //cli mode
    fcgTSBCMDOnly->Checked             = cnf->oth.disable_guicmd != 0;

    this->ResumeLayout();
    this->PerformLayout();
}

System::Void frmConfig::FrmToConf(CONF_GUIEX *cnf) {
    //拡張部
    cnf->enc.interlaced             = fcgCXInterlaced->SelectedIndex;
    cnf->enc.output_csp             = fcgCXOutputCsp->SelectedIndex;
    cnf->enc.use_highbit_depth      = csp_highbit_depth(cnf->enc.output_csp);
    cnf->enc.use_auto_npass         = fcgCB2passEnc->Checked;
    cnf->enc.audio_input            = fcgCBAudioInput->Checked;
    cnf->enc.auto_npass             = 2;
    //cnf->vid.amp_check              = NULL;
    cnf->vid.afs                    = fcgCBAuoTcfileout->Checked;
    cnf->vid.auo_tcfile_out         = fcgCBAuoTcfileout->Checked;
    cnf->vid.afs_bitrate_correction = FALSE;
    cnf->vid.check_keyframe         = FALSE;
    cnf->vid.priority               = fcgCXffmpegOutPriority->SelectedIndex;
    cnf->oth.temp_dir               = fcgCXTempDir->SelectedIndex;
    GetWCHARfromString(cnf->vid.cmdex, _countof(cnf->vid.cmdex), fcgTXCmdEx->Text);
    GetWCHARfromString(cnf->enc.incmd, _countof(cnf->enc.incmd), fcgTXInCmd->Text);

    //音声部
    cnf->aud.use_internal               = fcgCBAudioUseInternal->Checked;
    cnf->aud.ext.encoder                = fcgCXAudioEncoder->SelectedIndex;
    cnf->aud.ext.faw_check              = fcgCBFAWCheck->Checked;
    cnf->aud.ext.enc_mode               = fcgCXAudioEncMode->SelectedIndex;
    cnf->aud.ext.bitrate                = (int)fcgNUAudioBitrate->Value;
    cnf->aud.ext.use_2pass              = fcgCBAudio2pass->Checked;
    cnf->aud.ext.use_wav                = !fcgCBAudioUsePipe->Checked;
    cnf->aud.ext.delay_cut              = fcgCXAudioDelayCut->SelectedIndex;
    cnf->aud.ext.priority               = fcgCXAudioPriority->SelectedIndex;
    cnf->aud.ext.audio_encode_timing    = 1;
    cnf->aud.ext.aud_temp_dir           = fcgCXAudioTempDir->SelectedIndex;
    cnf->aud.in.encoder                 = fcgCXAudioEncoderInternal->SelectedIndex;
    cnf->aud.in.faw_check               = fcgCBFAWCheck->Checked;
    cnf->aud.in.enc_mode                = fcgCXAudioEncModeInternal->SelectedIndex;
    cnf->aud.in.bitrate                 = (int)fcgNUAudioBitrateInternal->Value;

    //mux部
    /*
    cnf->mux.disable_mp4ext         = !fcgCBMP4MuxerExt->Checked;
    cnf->mux.apple_mode             = fcgCBMP4MuxApple->Checked;
    cnf->mux.mp4_mode               = fcgCXMP4CmdEx->SelectedIndex;
    cnf->mux.mp4_temp_dir           = fcgCXMP4BoxTempDir->SelectedIndex;
    cnf->mux.disable_mkvext         = !fcgCBMKVMuxerExt->Checked;
    cnf->mux.mkv_mode               = fcgCXMKVCmdEx->SelectedIndex;
    cnf->mux.disable_mpgext         = !fcgCBMPGMuxerExt->Checked;
    cnf->mux.mpg_mode               = fcgCXMPGCmdEx->SelectedIndex;
    cnf->mux.minimized              = fcgCBMuxMinimize->Checked;
    cnf->mux.priority               = fcgCXMuxPriority->SelectedIndex;

    cnf->oth.run_bat                = RUN_BAT_NONE;
    cnf->oth.run_bat               |= (fcgCBRunBatBefore->Checked) ? RUN_BAT_BEFORE : NULL;
    cnf->oth.run_bat               |= (fcgCBRunBatAfter->Checked) ? RUN_BAT_AFTER : NULL;
    cnf->oth.dont_wait_bat_fin      = RUN_BAT_NONE;
    cnf->oth.dont_wait_bat_fin     |= (!fcgCBWaitForBatBefore->Checked) ? RUN_BAT_BEFORE : NULL;
    cnf->oth.dont_wait_bat_fin     |= (!fcgCBWaitForBatAfter->Checked) ? RUN_BAT_AFTER : NULL;
    GetWCHARfromString(cnf->oth.batfile, sizeof(cnf->oth.batfile), fcgTXBatPath->Text);
    */
    cnf->mux.disable_mp4ext         = TRUE;
    cnf->mux.disable_mkvext         = TRUE;

    GetfcgTSLSettingsNotes(cnf->oth.notes, _countof(cnf->oth.notes));

    GetWCHARfromString(cnf->enc.outext, _countof(cnf->enc.outext), fcgTXOutputExt->Text);

    //cli mode
    cnf->oth.disable_guicmd         = FALSE;
}

System::Void frmConfig::GetfcgTSLSettingsNotes(TCHAR *notes, int nSize) {
    ZeroMemory(notes, nSize);
    if (fcgTSLSettingsNotes->Overflow != ToolStripItemOverflow::Never)
        GetWCHARfromString(notes, nSize, fcgTSLSettingsNotes->Text);
}

System::Void frmConfig::SetfcgTSLSettingsNotes(const TCHAR *notes) {
    if (str_has_char(notes)) {
        fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[0][0], StgNotesColor[0][1], StgNotesColor[0][2]);
        fcgTSLSettingsNotes->Text = String(notes).ToString();
        fcgTSLSettingsNotes->Overflow = ToolStripItemOverflow::AsNeeded;
    } else {
        fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[1][0], StgNotesColor[1][1], StgNotesColor[1][2]);
        fcgTSLSettingsNotes->Text = LOAD_CLI_STRING(AuofcgTSTSettingsNotes);
        fcgTSLSettingsNotes->Overflow = ToolStripItemOverflow::Never;
    }
}

System::Void frmConfig::SetfcgTSLSettingsNotes(String^ notes) {
    if (notes->Length && fcgTSLSettingsNotes->Overflow != ToolStripItemOverflow::Never) {
        fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[0][0], StgNotesColor[0][1], StgNotesColor[0][2]);
        fcgTSLSettingsNotes->Text = notes;
        fcgTSLSettingsNotes->Overflow = ToolStripItemOverflow::AsNeeded;
    } else {
        fcgTSLSettingsNotes->ForeColor = Color::FromArgb(StgNotesColor[1][0], StgNotesColor[1][1], StgNotesColor[1][2]);
        fcgTSLSettingsNotes->Text = LOAD_CLI_STRING(AuofcgTSTSettingsNotes);
        fcgTSLSettingsNotes->Overflow = ToolStripItemOverflow::Never;
    }
}

System::Void frmConfig::SetChangedEvent(Control^ control, System::EventHandler^ _event) {
    System::Type^ ControlType = control->GetType();
    if (ControlType == NumericUpDown::typeid)
        ((NumericUpDown^)control)->ValueChanged += _event;
    else if (ControlType == ComboBox::typeid)
        ((ComboBox^)control)->SelectedIndexChanged += _event;
    else if (ControlType == CheckBox::typeid)
        ((CheckBox^)control)->CheckedChanged += _event;
    else if (ControlType == TextBox::typeid)
        ((TextBox^)control)->TextChanged += _event;
}

System::Void frmConfig::SetToolStripEvents(ToolStrip^ TS, System::Windows::Forms::MouseEventHandler^ _event) {
    for (int i = 0; i < TS->Items->Count; i++) {
        ToolStripButton^ TSB = dynamic_cast<ToolStripButton^>(TS->Items[i]);
        if (TSB != nullptr) TSB->MouseDown += _event;
    }
}

System::Void frmConfig::TabControl_DarkDrawItem(System::Object^ sender, DrawItemEventArgs^ e) {
    //対象のTabControlを取得
    TabControl^ tab = dynamic_cast<TabControl^>(sender);
    //タブページのテキストを取得
    System::String^ txt = tab->TabPages[e->Index]->Text;

    //タブのテキストと背景を描画するためのブラシを決定する
    SolidBrush^ foreBrush = gcnew System::Drawing::SolidBrush(ColorfromInt(DEFAULT_UI_COLOR_TEXT_DARK));
    SolidBrush^ backBrush = gcnew System::Drawing::SolidBrush(ColorfromInt(DEFAULT_UI_COLOR_BASE_DARK));

    //StringFormatを作成
    StringFormat^ sf = gcnew System::Drawing::StringFormat();
    //中央に表示する
    sf->Alignment = StringAlignment::Center;
    sf->LineAlignment = StringAlignment::Center;

    //背景の描画
    e->Graphics->FillRectangle(backBrush, e->Bounds);
    //Textの描画
    e->Graphics->DrawString(txt, e->Font, foreBrush, e->Bounds, sf);
}

System::Void frmConfig::fcgMouseEnter_SetColor(System::Object^  sender, System::EventArgs^  e) {
    fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Hot, dwStgReader);
}
System::Void frmConfig::fcgMouseLeave_SetColor(System::Object^  sender, System::EventArgs^  e) {
    fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Normal, dwStgReader);
}

System::Void frmConfig::SetAllMouseMove(Control ^top, const AuoTheme themeTo) {
    if (themeTo == themeMode) return;
    System::Type^ type = top->GetType();
    if (type == CheckBox::typeid /* || isToolStripItem(type)*/) {
        top->MouseEnter += gcnew System::EventHandler(this, &frmConfig::fcgMouseEnter_SetColor);
        top->MouseLeave += gcnew System::EventHandler(this, &frmConfig::fcgMouseLeave_SetColor);
    } else if (type == ToolStrip::typeid) {
        ToolStrip^ TS = dynamic_cast<ToolStrip^>(top);
        for (int i = 0; i < TS->Items->Count; i++) {
            auto item = TS->Items[i];
            item->MouseEnter += gcnew System::EventHandler(this, &frmConfig::fcgMouseEnter_SetColor);
            item->MouseLeave += gcnew System::EventHandler(this, &frmConfig::fcgMouseLeave_SetColor);
        }
    }
    for (int i = 0; i < top->Controls->Count; i++) {
        SetAllMouseMove(top->Controls[i], themeTo);
    }
}

System::Void frmConfig::CheckTheme() {
    //DarkenWindowが使用されていれば設定をロードする
    if (dwStgReader != nullptr) delete dwStgReader;
    const auto [themeTo, dwStg] = check_current_theme(sys_dat->aviutl_dir);
    dwStgReader = dwStg;

    //変更の必要がなければ終了
    if (themeTo == themeMode) return;

    //一度ウィンドウの再描画を完全に抑止する
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
#if 1
    //tabcontrolのborderを隠す
    SwitchComboBoxBorder(fcgtabControlVideo, fcgPNHideTabControlVideo, themeMode, themeTo, dwStgReader);
    //SwitchComboBoxBorder(fcgtabControlAudio, fcgPNHideTabControlAudio, themeMode, themeTo, dwStgReader);
    //SwitchComboBoxBorder(fcgtabControlMux,   fcgPNHideTabControlMux,   themeMode, themeTo, dwStgReader);
    //上部のtoolstripborderを隠すためのパネル
    fcgPNHideToolStripBorder->Visible = themeTo == AuoTheme::DarkenWindowDark;
#endif
#if 0
    //TabControlをオーナードローする
    fcgtabControlVideo->DrawMode = TabDrawMode::OwnerDrawFixed;
    fcgtabControlVideo->DrawItem += gcnew DrawItemEventHandler(this, &frmConfig::TabControl_DarkDrawItem);

    fcgtabControlAudio->DrawMode = TabDrawMode::OwnerDrawFixed;
    fcgtabControlAudio->DrawItem += gcnew DrawItemEventHandler(this, &frmConfig::TabControl_DarkDrawItem);

    fcgtabControlMux->DrawMode = TabDrawMode::OwnerDrawFixed;
    fcgtabControlMux->DrawItem += gcnew DrawItemEventHandler(this, &frmConfig::TabControl_DarkDrawItem);
#endif
    if (themeTo != themeMode) {
        SetAllColor(this, themeTo, this->GetType(), dwStgReader);
        SetAllMouseMove(this, themeTo);
    }
    //一度ウィンドウの再描画を再開し、強制的に再描画させる
    SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
    this->Refresh();
    themeMode = themeTo;
}

System::Void frmConfig::SetAllCheckChangedEvents(Control ^top) {
    //再帰を使用してすべてのコントロールのtagを調べ、イベントをセットする
    for (int i = 0; i < top->Controls->Count; i++) {
        System::Type^ type = top->Controls[i]->GetType();
        if (type == NumericUpDown::typeid)
            top->Controls[i]->Enter += gcnew System::EventHandler(this, &frmConfig::NUSelectAll);

        if (type == Label::typeid || type == Button::typeid)
            ;
        else if (type == ToolStrip::typeid)
            SetToolStripEvents((ToolStrip^)(top->Controls[i]), gcnew System::Windows::Forms::MouseEventHandler(this, &frmConfig::fcgTSItem_MouseDown));
        else if (top->Controls[i]->Tag == nullptr)
            SetAllCheckChangedEvents(top->Controls[i]);
        else if (String::Equals(top->Controls[i]->Tag->ToString(), L"chValue"))
            SetChangedEvent(top->Controls[i], gcnew System::EventHandler(this, &frmConfig::CheckOtherChanges));
        else
            SetAllCheckChangedEvents(top->Controls[i]);
    }
}

System::Void frmConfig::SetHelpToolTips() {
#define SET_TOOL_TIP_EX(target) { fcgTTEx->SetToolTip(target, LOAD_CLI_STRING(AuofrmTT ## target)); }
    SET_TOOL_TIP_EX(fcgBTVideoEncoderPath);
    SET_TOOL_TIP_EX(fcgCXOutputCsp);
    SET_TOOL_TIP_EX(fcgCB2passEnc);
    SET_TOOL_TIP_EX(fcgCBAudioInput);
    SET_TOOL_TIP_EX(fcgTXOutputExt);
    SET_TOOL_TIP_EX(fcgCBAuoTcfileout);
    SET_TOOL_TIP_EX(fcgCXInterlaced);
    SET_TOOL_TIP_EX(fcgTXCmdEx);
    SET_TOOL_TIP_EX(fcgTXInCmd);
    SET_TOOL_TIP_EX(fcgCXffmpegOutPriority);

    //拡張
    SET_TOOL_TIP_EX(fcgCXTempDir);
    SET_TOOL_TIP_EX(fcgBTCustomTempDir);
    //音声
    SET_TOOL_TIP_EX(fcgCXAudioEncoder);
    SET_TOOL_TIP_EX(fcgCBFAWCheck);
    SET_TOOL_TIP_EX(fcgBTAudioEncoderPath);
    SET_TOOL_TIP_EX(fcgCXAudioEncMode);
    SET_TOOL_TIP_EX(fcgCBAudio2pass);
    SET_TOOL_TIP_EX(fcgCBAudioUsePipe);
    SET_TOOL_TIP_EX(fcgNUAudioBitrate);
    SET_TOOL_TIP_EX(fcgCXAudioPriority);
    SET_TOOL_TIP_EX(fcgCXAudioEncTiming);
    SET_TOOL_TIP_EX(fcgCXAudioTempDir);
    SET_TOOL_TIP_EX(fcgBTCustomAudioTempDir);
    //muxer
    SET_TOOL_TIP_EX(fcgCBMP4MuxerExt);
    SET_TOOL_TIP_EX(fcgCXMP4CmdEx);
    SET_TOOL_TIP_EX(fcgBTMP4MuxerPath);
    SET_TOOL_TIP_EX(fcgBTTC2MP4Path);
    SET_TOOL_TIP_EX(fcgBTMP4RawPath);
    SET_TOOL_TIP_EX(fcgCXMP4BoxTempDir);
    SET_TOOL_TIP_EX(fcgBTMP4BoxTempDir);
    SET_TOOL_TIP_EX(fcgCBMKVMuxerExt);
    SET_TOOL_TIP_EX(fcgCXMKVCmdEx);
    SET_TOOL_TIP_EX(fcgBTMKVMuxerPath);
    SET_TOOL_TIP_EX(fcgCBMPGMuxerExt);
    SET_TOOL_TIP_EX(fcgCXMPGCmdEx);
    SET_TOOL_TIP_EX(fcgBTMPGMuxerPath);
    SET_TOOL_TIP_EX(fcgCXMuxPriority);
    //バッチファイル実行
    SET_TOOL_TIP_EX(fcgCBRunBatBefore);
    SET_TOOL_TIP_EX(fcgCBRunBatAfter);
    SET_TOOL_TIP_EX(fcgCBWaitForBatBefore);
    SET_TOOL_TIP_EX(fcgCBWaitForBatAfter);
    SET_TOOL_TIP_EX(fcgBTBatBeforePath);
    SET_TOOL_TIP_EX(fcgBTBatAfterPath);
    //他
    SET_TOOL_TIP_EX(fcgBTDefault);
    //上部ツールストリップ
    fcgTSBDelete->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBDelete);
    fcgTSBOtherSettings->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBOtherSettings);
    fcgTSBSave->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBSave);
    fcgTSBSaveNew->ToolTipText = LOAD_CLI_STRING(AuofrmTTfcgTSBSaveNew);
}
System::Void frmConfig::ShowExehelp(String^ ExePath, String^ args) {
    if (!File::Exists(ExePath)) {
        MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_EXE_NOT_FOUND), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
    } else {
        TCHAR exe_path[MAX_PATH_LEN];
        TCHAR file_path[MAX_PATH_LEN];
        TCHAR cmd[MAX_CMD_LEN];
        GetWCHARfromString(exe_path, _countof(exe_path), ExePath);
        apply_appendix(file_path, _countof(file_path), exe_path, _T("_fullhelp.txt"));
        File::Delete(String(file_path).ToString());
        array<String^>^ arg_list = args->Split(L';');
        for (int i = 0; i < arg_list->Length; i++) {
            if (i) {
                StreamWriter^ sw;
                try {
                    sw = gcnew StreamWriter(String(file_path).ToString(), true, System::Text::Encoding::GetEncoding("shift_jis"));
                    sw->WriteLine();
                    sw->WriteLine();
                } catch (...) {
                    //ファイルオープンに失敗…初回のget_exe_message_to_fileでエラーとなるため、おそらく起こらない
                } finally {
                    if (sw != nullptr) { sw->Close(); }
                }
            }
            GetWCHARfromString(cmd, _countof(cmd), arg_list[i]);
            if (get_exe_message_to_file(exe_path, cmd, file_path, AUO_PIPE_MUXED, 5) != RP_SUCCESS) {
                File::Delete(String(file_path).ToString());
                MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_GET_HELP), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
                return;
            }
        }
        try {
            System::Diagnostics::Process::Start(String(file_path).ToString());
        } catch (...) {
            MessageBox::Show(LOAD_CLI_STRING(AUO_CONFIG_ERR_OPEN_HELP), LOAD_CLI_STRING(AUO_GUIEX_ERROR), MessageBoxButtons::OK, MessageBoxIcon::Error);
        }
    }
}

#pragma warning( pop )
