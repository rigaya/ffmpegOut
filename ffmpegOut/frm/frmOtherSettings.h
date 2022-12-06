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

#pragma once

#include "auo_version.h"
#include "auo_settings.h"
#include "auo_mes.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace AUO_NAME_R {

    /// <summary>
    /// frmOtherSettings の概要
    ///
    /// 警告: このクラスの名前を変更する場合、このクラスが依存するすべての .resx ファイルに関連付けられた
    ///          マネージ リソース コンパイラ ツールに対して 'Resource File Name' プロパティを
    ///          変更する必要があります。この変更を行わないと、
    ///          デザイナと、このフォームに関連付けられたローカライズ済みリソースとが、
    ///          正しく相互に利用できなくなります。
    /// </summary>
    public ref class frmOtherSettings : public System::Windows::Forms::Form
    {
    public:
        frmOtherSettings(void)
        {
            fos_ex_stg = new guiEx_settings(TRUE);
            InitializeComponent();
            //
            //TODO: ここにコンストラクタ コードを追加します
            //
            themeMode = AuoTheme::DefaultLight;
            dwStgReader = nullptr;
        }

    protected:
        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        ~frmOtherSettings()
        {
            if (components)
            {
                delete components;
            }
            delete fos_ex_stg;
        }
    private:
        guiEx_settings *fos_ex_stg;
        static frmOtherSettings^ _instance;
    public:
        static String^ stgDir;
        static int useLastExt;
        //static bool DisableToolTipHelp;

    protected:
    private: System::Windows::Forms::Button^  fosCBCancel;
    private: System::Windows::Forms::Button^  fosCBOK;













    private: System::Windows::Forms::FontDialog^  fosfontDialog;

    private: System::Windows::Forms::TabControl^  fosTabControl;
    private: System::Windows::Forms::TabPage^  fostabPageGeneral;
    private: System::Windows::Forms::ComboBox^  fosCXDefaultAudioEncoder;
    private: System::Windows::Forms::Label^  fosLBDefaultAudioEncoder;
    private: System::Windows::Forms::TabPage^  fostabPageGUI;
    private: System::Windows::Forms::CheckBox^  fosCBOutputMoreLog;
    private: System::Windows::Forms::CheckBox^  fosCBRunBatMinimized;
    private: System::Windows::Forms::CheckBox^  fosCBGetRelativePath;
    private: System::Windows::Forms::Button^  fosBTSetFont;
    private: System::Windows::Forms::CheckBox^  fosCBStgEscKey;
    private: System::Windows::Forms::CheckBox^  fosCBDisableToolTip;
    private: System::Windows::Forms::CheckBox^  fosCBDisableVisualStyles;
    private: System::Windows::Forms::CheckBox^  fosCBLogDisableTransparency;
    private: System::Windows::Forms::Label^  fosLBDisableVisualStyles;
    private: System::Windows::Forms::CheckBox^  fosCBLogStartMinimized;
    private: System::Windows::Forms::Panel^  fosPNHideTabPage;
    private: System::Windows::Forms::Label^  fosLBStgDir;
    private: System::Windows::Forms::Button^  fosBTStgDir;
    private: System::Windows::Forms::TextBox^  fosTXStgDir;









































    public: 

    public:
        static property frmOtherSettings^ Instance {
            frmOtherSettings^ get() {
                if (_instance == nullptr || _instance->IsDisposed)
                    _instance = gcnew frmOtherSettings();
                return _instance;
            }
        }


    private:
        /// <summary>
        /// 必要なデザイナ変数です。
        /// </summary>
        System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
        /// <summary>
        /// デザイナ サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディタで変更しないでください。
        /// </summary>
        void InitializeComponent(void)
        {
            this->fosCBCancel = (gcnew System::Windows::Forms::Button());
            this->fosCBOK = (gcnew System::Windows::Forms::Button());
            this->fosfontDialog = (gcnew System::Windows::Forms::FontDialog());
            this->fosTabControl = (gcnew System::Windows::Forms::TabControl());
            this->fostabPageGeneral = (gcnew System::Windows::Forms::TabPage());
            this->fosCXDefaultAudioEncoder = (gcnew System::Windows::Forms::ComboBox());
            this->fosLBDefaultAudioEncoder = (gcnew System::Windows::Forms::Label());
            this->fostabPageGUI = (gcnew System::Windows::Forms::TabPage());
            this->fosCBOutputMoreLog = (gcnew System::Windows::Forms::CheckBox());
            this->fosCBRunBatMinimized = (gcnew System::Windows::Forms::CheckBox());
            this->fosCBGetRelativePath = (gcnew System::Windows::Forms::CheckBox());
            this->fosBTSetFont = (gcnew System::Windows::Forms::Button());
            this->fosCBStgEscKey = (gcnew System::Windows::Forms::CheckBox());
            this->fosCBDisableToolTip = (gcnew System::Windows::Forms::CheckBox());
            this->fosCBDisableVisualStyles = (gcnew System::Windows::Forms::CheckBox());
            this->fosCBLogDisableTransparency = (gcnew System::Windows::Forms::CheckBox());
            this->fosLBDisableVisualStyles = (gcnew System::Windows::Forms::Label());
            this->fosCBLogStartMinimized = (gcnew System::Windows::Forms::CheckBox());
            this->fosPNHideTabPage = (gcnew System::Windows::Forms::Panel());
            this->fosLBStgDir = (gcnew System::Windows::Forms::Label());
            this->fosBTStgDir = (gcnew System::Windows::Forms::Button());
            this->fosTXStgDir = (gcnew System::Windows::Forms::TextBox());
            this->fosTabControl->SuspendLayout();
            this->fostabPageGeneral->SuspendLayout();
            this->fostabPageGUI->SuspendLayout();
            this->fosPNHideTabPage->SuspendLayout();
            this->SuspendLayout();
            // 
            // fosCBCancel
            // 
            this->fosCBCancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
            this->fosCBCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            this->fosCBCancel->Location = System::Drawing::Point(171, 420);
            this->fosCBCancel->Name = L"fosCBCancel";
            this->fosCBCancel->Size = System::Drawing::Size(84, 29);
            this->fosCBCancel->TabIndex = 1;
            this->fosCBCancel->Text = L"キャンセル";
            this->fosCBCancel->UseVisualStyleBackColor = true;
            this->fosCBCancel->Click += gcnew System::EventHandler(this, &frmOtherSettings::fosCBCancel_Click);
            // 
            // fosCBOK
            // 
            this->fosCBOK->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
            this->fosCBOK->Location = System::Drawing::Point(283, 420);
            this->fosCBOK->Name = L"fosCBOK";
            this->fosCBOK->Size = System::Drawing::Size(84, 29);
            this->fosCBOK->TabIndex = 2;
            this->fosCBOK->Text = L"OK";
            this->fosCBOK->UseVisualStyleBackColor = true;
            this->fosCBOK->Click += gcnew System::EventHandler(this, &frmOtherSettings::fosCBOK_Click);
            // 
            // fosfontDialog
            // 
            this->fosfontDialog->AllowVerticalFonts = false;
            this->fosfontDialog->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->fosfontDialog->FontMustExist = true;
            this->fosfontDialog->MaxSize = 9;
            this->fosfontDialog->MinSize = 9;
            this->fosfontDialog->ShowEffects = false;
            // 
            // fosTabControl
            // 
            this->fosTabControl->Controls->Add(this->fostabPageGeneral);
            this->fosTabControl->Controls->Add(this->fostabPageGUI);
            this->fosTabControl->Location = System::Drawing::Point(1, 0);
            this->fosTabControl->Name = L"fosTabControl";
            this->fosTabControl->SelectedIndex = 0;
            this->fosTabControl->Size = System::Drawing::Size(391, 414);
            this->fosTabControl->TabIndex = 26;
            // 
            // fostabPageGeneral
            // 
            this->fostabPageGeneral->Controls->Add(this->fosCXDefaultAudioEncoder);
            this->fostabPageGeneral->Controls->Add(this->fosLBDefaultAudioEncoder);
            this->fostabPageGeneral->Location = System::Drawing::Point(4, 24);
            this->fostabPageGeneral->Name = L"fostabPageGeneral";
            this->fostabPageGeneral->Padding = System::Windows::Forms::Padding(3);
            this->fostabPageGeneral->Size = System::Drawing::Size(383, 386);
            this->fostabPageGeneral->TabIndex = 0;
            this->fostabPageGeneral->Text = L"一般設定";
            this->fostabPageGeneral->UseVisualStyleBackColor = true;
            // 
            // fosCXDefaultAudioEncoder
            // 
            this->fosCXDefaultAudioEncoder->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->fosCXDefaultAudioEncoder->FormattingEnabled = true;
            this->fosCXDefaultAudioEncoder->Location = System::Drawing::Point(35, 39);
            this->fosCXDefaultAudioEncoder->Name = L"fosCXDefaultAudioEncoder";
            this->fosCXDefaultAudioEncoder->Size = System::Drawing::Size(190, 23);
            this->fosCXDefaultAudioEncoder->TabIndex = 33;
            // 
            // fosLBDefaultAudioEncoder
            // 
            this->fosLBDefaultAudioEncoder->AutoSize = true;
            this->fosLBDefaultAudioEncoder->Location = System::Drawing::Point(8, 17);
            this->fosLBDefaultAudioEncoder->Name = L"fosLBDefaultAudioEncoder";
            this->fosLBDefaultAudioEncoder->Size = System::Drawing::Size(130, 15);
            this->fosLBDefaultAudioEncoder->TabIndex = 32;
            this->fosLBDefaultAudioEncoder->Text = L"デフォルトの音声エンコーダ";
            // 
            // fostabPageGUI
            // 
            this->fostabPageGUI->Controls->Add(this->fosLBStgDir);
            this->fostabPageGUI->Controls->Add(this->fosBTStgDir);
            this->fostabPageGUI->Controls->Add(this->fosTXStgDir);
            this->fostabPageGUI->Controls->Add(this->fosCBOutputMoreLog);
            this->fostabPageGUI->Controls->Add(this->fosCBRunBatMinimized);
            this->fostabPageGUI->Controls->Add(this->fosCBGetRelativePath);
            this->fostabPageGUI->Controls->Add(this->fosBTSetFont);
            this->fostabPageGUI->Controls->Add(this->fosCBStgEscKey);
            this->fostabPageGUI->Controls->Add(this->fosCBDisableToolTip);
            this->fostabPageGUI->Controls->Add(this->fosCBDisableVisualStyles);
            this->fostabPageGUI->Controls->Add(this->fosCBLogDisableTransparency);
            this->fostabPageGUI->Controls->Add(this->fosLBDisableVisualStyles);
            this->fostabPageGUI->Controls->Add(this->fosCBLogStartMinimized);
            this->fostabPageGUI->Location = System::Drawing::Point(4, 24);
            this->fostabPageGUI->Name = L"fostabPageGUI";
            this->fostabPageGUI->Size = System::Drawing::Size(383, 386);
            this->fostabPageGUI->TabIndex = 1;
            this->fostabPageGUI->Text = L"ログ・設定画面";
            this->fostabPageGUI->UseVisualStyleBackColor = true;
            // 
            // fosCBOutputMoreLog
            // 
            this->fosCBOutputMoreLog->AutoSize = true;
            this->fosCBOutputMoreLog->Location = System::Drawing::Point(20, 289);
            this->fosCBOutputMoreLog->Name = L"fosCBOutputMoreLog";
            this->fosCBOutputMoreLog->Size = System::Drawing::Size(143, 19);
            this->fosCBOutputMoreLog->TabIndex = 32;
            this->fosCBOutputMoreLog->Text = L"音声・muxのログも表示";
            this->fosCBOutputMoreLog->UseVisualStyleBackColor = true;
            // 
            // fosCBRunBatMinimized
            // 
            this->fosCBRunBatMinimized->AutoSize = true;
            this->fosCBRunBatMinimized->Location = System::Drawing::Point(20, 257);
            this->fosCBRunBatMinimized->Name = L"fosCBRunBatMinimized";
            this->fosCBRunBatMinimized->Size = System::Drawing::Size(205, 19);
            this->fosCBRunBatMinimized->TabIndex = 30;
            this->fosCBRunBatMinimized->Text = L"エンコ前後バッチ処理を最小化で実行";
            this->fosCBRunBatMinimized->UseVisualStyleBackColor = true;
            // 
            // fosCBGetRelativePath
            // 
            this->fosCBGetRelativePath->AutoSize = true;
            this->fosCBGetRelativePath->Location = System::Drawing::Point(20, 232);
            this->fosCBGetRelativePath->Name = L"fosCBGetRelativePath";
            this->fosCBGetRelativePath->Size = System::Drawing::Size(185, 19);
            this->fosCBGetRelativePath->TabIndex = 24;
            this->fosCBGetRelativePath->Text = L"ダイアログから相対パスで取得する";
            this->fosCBGetRelativePath->UseVisualStyleBackColor = true;
            // 
            // fosBTSetFont
            // 
            this->fosBTSetFont->Location = System::Drawing::Point(241, 197);
            this->fosBTSetFont->Name = L"fosBTSetFont";
            this->fosBTSetFont->Size = System::Drawing::Size(124, 27);
            this->fosBTSetFont->TabIndex = 25;
            this->fosBTSetFont->Text = L"フォントの変更...";
            this->fosBTSetFont->UseVisualStyleBackColor = true;
            this->fosBTSetFont->Click += gcnew System::EventHandler(this, &frmOtherSettings::fosBTSetFont_Click);
            // 
            // fosCBStgEscKey
            // 
            this->fosCBStgEscKey->AutoSize = true;
            this->fosCBStgEscKey->Location = System::Drawing::Point(20, 202);
            this->fosCBStgEscKey->Name = L"fosCBStgEscKey";
            this->fosCBStgEscKey->Size = System::Drawing::Size(168, 19);
            this->fosCBStgEscKey->TabIndex = 23;
            this->fosCBStgEscKey->Text = L"設定画面でEscキーを有効化";
            this->fosCBStgEscKey->UseVisualStyleBackColor = true;
            // 
            // fosCBDisableToolTip
            // 
            this->fosCBDisableToolTip->AutoSize = true;
            this->fosCBDisableToolTip->Location = System::Drawing::Point(20, 66);
            this->fosCBDisableToolTip->Name = L"fosCBDisableToolTip";
            this->fosCBDisableToolTip->Size = System::Drawing::Size(158, 19);
            this->fosCBDisableToolTip->TabIndex = 22;
            this->fosCBDisableToolTip->Text = L"ポップアップヘルプを抑制する";
            this->fosCBDisableToolTip->UseVisualStyleBackColor = true;
            // 
            // fosCBDisableVisualStyles
            // 
            this->fosCBDisableVisualStyles->AutoSize = true;
            this->fosCBDisableVisualStyles->Location = System::Drawing::Point(20, 96);
            this->fosCBDisableVisualStyles->Name = L"fosCBDisableVisualStyles";
            this->fosCBDisableVisualStyles->Size = System::Drawing::Size(128, 19);
            this->fosCBDisableVisualStyles->TabIndex = 23;
            this->fosCBDisableVisualStyles->Text = L"視覚効果をオフにする";
            this->fosCBDisableVisualStyles->UseVisualStyleBackColor = true;
            // 
            // fosCBLogDisableTransparency
            // 
            this->fosCBLogDisableTransparency->AutoSize = true;
            this->fosCBLogDisableTransparency->Location = System::Drawing::Point(20, 172);
            this->fosCBLogDisableTransparency->Name = L"fosCBLogDisableTransparency";
            this->fosCBLogDisableTransparency->Size = System::Drawing::Size(174, 19);
            this->fosCBLogDisableTransparency->TabIndex = 22;
            this->fosCBLogDisableTransparency->Text = L"ログウィンドウの透過をオフにする";
            this->fosCBLogDisableTransparency->UseVisualStyleBackColor = true;
            // 
            // fosLBDisableVisualStyles
            // 
            this->fosLBDisableVisualStyles->AutoSize = true;
            this->fosLBDisableVisualStyles->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(128)));
            this->fosLBDisableVisualStyles->ForeColor = System::Drawing::Color::OrangeRed;
            this->fosLBDisableVisualStyles->Location = System::Drawing::Point(41, 115);
            this->fosLBDisableVisualStyles->Name = L"fosLBDisableVisualStyles";
            this->fosLBDisableVisualStyles->Size = System::Drawing::Size(161, 14);
            this->fosLBDisableVisualStyles->TabIndex = 20;
            this->fosLBDisableVisualStyles->Text = L"※反映にはAviutlの再起動が必要";
            // 
            // fosCBLogStartMinimized
            // 
            this->fosCBLogStartMinimized->AutoSize = true;
            this->fosCBLogStartMinimized->Location = System::Drawing::Point(20, 142);
            this->fosCBLogStartMinimized->Name = L"fosCBLogStartMinimized";
            this->fosCBLogStartMinimized->Size = System::Drawing::Size(184, 19);
            this->fosCBLogStartMinimized->TabIndex = 21;
            this->fosCBLogStartMinimized->Text = L"ログウィンドウを最小化で開始する";
            this->fosCBLogStartMinimized->UseVisualStyleBackColor = true;
            // 
            // fosPNHideTabPage
            // 
            this->fosPNHideTabPage->Controls->Add(this->fosTabControl);
            this->fosPNHideTabPage->Location = System::Drawing::Point(-1, -1);
            this->fosPNHideTabPage->Name = L"fosPNHideTabPage";
            this->fosPNHideTabPage->Size = System::Drawing::Size(392, 415);
            this->fosPNHideTabPage->TabIndex = 18;
            // 
            // fosLBStgDir
            // 
            this->fosLBStgDir->AutoSize = true;
            this->fosLBStgDir->Location = System::Drawing::Point(7, 7);
            this->fosLBStgDir->Name = L"fosLBStgDir";
            this->fosLBStgDir->Size = System::Drawing::Size(123, 15);
            this->fosLBStgDir->TabIndex = 34;
            this->fosLBStgDir->Text = L"設定ファイルの保存場所";
            // 
            // fosBTStgDir
            // 
            this->fosBTStgDir->Location = System::Drawing::Point(334, 27);
            this->fosBTStgDir->Name = L"fosBTStgDir";
            this->fosBTStgDir->Size = System::Drawing::Size(35, 23);
            this->fosBTStgDir->TabIndex = 35;
            this->fosBTStgDir->Text = L"...";
            this->fosBTStgDir->UseVisualStyleBackColor = true;
            this->fosBTStgDir->Click += gcnew System::EventHandler(this, &frmOtherSettings::fosBTStgDir_Click);
            // 
            // fosTXStgDir
            // 
            this->fosTXStgDir->Location = System::Drawing::Point(34, 27);
            this->fosTXStgDir->Name = L"fosTXStgDir";
            this->fosTXStgDir->Size = System::Drawing::Size(294, 23);
            this->fosTXStgDir->TabIndex = 33;
            // 
            // frmOtherSettings
            // 
            this->AcceptButton = this->fosCBOK;
            this->AutoScaleDimensions = System::Drawing::SizeF(96, 96);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Dpi;
            this->CancelButton = this->fosCBCancel;
            this->ClientSize = System::Drawing::Size(392, 458);
            this->Controls->Add(this->fosPNHideTabPage);
            this->Controls->Add(this->fosCBOK);
            this->Controls->Add(this->fosCBCancel);
            this->Font = (gcnew System::Drawing::Font(L"Meiryo UI", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->KeyPreview = true;
            this->MaximizeBox = false;
            this->Name = L"frmOtherSettings";
            this->ShowIcon = false;
            this->Text = L"frmOtherSettings";
            this->Load += gcnew System::EventHandler(this, &frmOtherSettings::frmOtherSettings_Load);
            this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &frmOtherSettings::frmOtherSettings_KeyDown);
            this->fosTabControl->ResumeLayout(false);
            this->fostabPageGeneral->ResumeLayout(false);
            this->fostabPageGeneral->PerformLayout();
            this->fostabPageGUI->ResumeLayout(false);
            this->fostabPageGUI->PerformLayout();
            this->fosPNHideTabPage->ResumeLayout(false);
            this->ResumeLayout(false);

        }
#pragma endregion
    private:
        AuoTheme themeMode;
        const DarkenWindowStgReader *dwStgReader;
    private:
        System::Void LoadLangText() {
            LOAD_CLI_TEXT(fosCBCancel);
            LOAD_CLI_TEXT(fosCBOK);
            //LOAD_CLI_TEXT(fosCBAutoAFSDisable);
            //LOAD_CLI_TEXT(fosCBAutoDelChap);
            LOAD_CLI_TEXT(fostabPageGeneral);
            LOAD_CLI_TEXT(fosLBDefaultAudioEncoder);
            //LOAD_CLI_TEXT(fosCBAutoRefLimitByLevel);
            //LOAD_CLI_TEXT(fosCBChapConvertToUTF8);
            //LOAD_CLI_TEXT(fosCBKeepQPFile);
            LOAD_CLI_TEXT(fosCBRunBatMinimized);
            //LOAD_CLI_TEXT(fosLBDefaultOutExt2);
            //LOAD_CLI_TEXT(fosLBDefaultOutExt);
            LOAD_CLI_TEXT(fostabPageGUI);
            LOAD_CLI_TEXT(fosCBOutputMoreLog);
            LOAD_CLI_TEXT(fosCBGetRelativePath);
            LOAD_CLI_TEXT(fosBTSetFont);
            LOAD_CLI_TEXT(fosCBStgEscKey);
            LOAD_CLI_TEXT(fosCBDisableToolTip);
            LOAD_CLI_TEXT(fosCBDisableVisualStyles);
            LOAD_CLI_TEXT(fosCBLogDisableTransparency);
            LOAD_CLI_TEXT(fosLBDisableVisualStyles);
            LOAD_CLI_TEXT(fosCBLogStartMinimized);
            LOAD_CLI_TEXT(fosLBStgDir);
            LOAD_CLI_TEXT(fosBTStgDir);
            //LOAD_CLI_TEXT(fostabPageAMP);
            //LOAD_CLI_TEXT(fosCBAutoDelStats);
            //LOAD_CLI_TEXT(fosGroupBoxAMPLimit);
            //LOAD_CLI_TEXT(fosLBAMPLimitMarginWarning);
            //LOAD_CLI_TEXT(fosBTAMPMarginMulti);
            //LOAD_CLI_TEXT(fosGroupBoxAMPLimitMarginExample);
            //LOAD_CLI_TEXT(fosLBAMPLMMExampleB32);
            //LOAD_CLI_TEXT(fosLBAMPLMMExampleB22);
            //LOAD_CLI_TEXT(fosLBAMPLMMExampleB31);
            //LOAD_CLI_TEXT(fosLBAMPLMMExampleB21);
            //LOAD_CLI_TEXT(fosLBAMPLMMExampleB12);
            //LOAD_CLI_TEXT(fosLBAMPLMMExampleB11);
            //LOAD_CLI_TEXT(fosLBAMPLMMExampleA12);
            //LOAD_CLI_TEXT(fosLBAMPLMMExampleA11);
            //LOAD_CLI_TEXT(fosLBAMPLimitMarginInfo);
            //LOAD_CLI_TEXT(fosLBAMPLimitMarginMax);
            //LOAD_CLI_TEXT(fosLBAMPLimitMarginMin);
            //LOAD_CLI_TEXT(fosCBAmpKeepOldFile);
            //LOAD_CLI_TEXT(fosCBPerfMonitor);
            //LOAD_CLI_TEXT(fosLBLogOut);
            LOAD_CLI_MAIN_TEXT(fosMain);
        }
    private: 
        System::Void fosCBOK_Click(System::Object^  sender, System::EventArgs^  e) {
            //DisableToolTipHelp = fosCBDisableToolTip->Checked;
            //make_file_filter(NULL, 0, fosCXDefaultOutExt->SelectedIndex);
            //overwrite_aviutl_ini_file_filter(fosCXDefaultOutExt->SelectedIndex);

            stgDir = fosTXStgDir->Text;
            fos_ex_stg->load_encode_stg();
            fos_ex_stg->load_log_win();
            fos_ex_stg->s_local.disable_tooltip_help     = fosCBDisableToolTip->Checked;
            fos_ex_stg->s_local.disable_visual_styles    = fosCBDisableVisualStyles->Checked;
            fos_ex_stg->s_local.enable_stg_esc_key       = fosCBStgEscKey->Checked;
            fos_ex_stg->s_log.minimized                  = fosCBLogStartMinimized->Checked;
            fos_ex_stg->s_log.transparent                = !fosCBLogDisableTransparency->Checked;
            fos_ex_stg->s_log.log_level                  =(fosCBOutputMoreLog->Checked) ? LOG_MORE : LOG_INFO;
            fos_ex_stg->s_local.get_relative_path        = fosCBGetRelativePath->Checked;
            fos_ex_stg->s_local.run_bat_minimized        = fosCBRunBatMinimized->Checked;
            fos_ex_stg->s_local.default_audio_encoder_ext= fosCXDefaultAudioEncoder->SelectedIndex;
            fos_ex_stg->save_local();
            fos_ex_stg->save_log_win();
            this->Close();
        }
    private: 
        System::Void fosSetComboBox() {
            fosCXDefaultAudioEncoder->SuspendLayout();
            fosCXDefaultAudioEncoder->Items->Clear();
            for (int i = 0; i < fos_ex_stg->s_aud_ext_count; i++)
                fosCXDefaultAudioEncoder->Items->Add(LOAD_CLI_STRING(AUO_OTHER_SETTINGS_AUDIO_ENCODER_EXTERNAL) + L": " + String(fos_ex_stg->s_aud_ext[i].dispname).ToString());
            fosCXDefaultAudioEncoder->ResumeLayout();
        }
    private:
        System::Void frmOtherSettings_Load(System::Object^  sender, System::EventArgs^  e) {
            this->Text = String(AUO_FULL_NAME).ToString();
            fosTXStgDir->Text = stgDir;
            fos_ex_stg->load_encode_stg();
            fos_ex_stg->load_log_win();

            LoadLangText();
            fosSetComboBox();
            fosCBDisableToolTip->Checked         = fos_ex_stg->s_local.disable_tooltip_help != 0;
            fosCBDisableVisualStyles->Checked    = fos_ex_stg->s_local.disable_visual_styles != 0;
            fosCBStgEscKey->Checked              = fos_ex_stg->s_local.enable_stg_esc_key != 0;
            fosCBLogStartMinimized->Checked      = fos_ex_stg->s_log.minimized != 0;
            fosCBLogDisableTransparency->Checked = fos_ex_stg->s_log.transparent == 0;
            fosCBOutputMoreLog->Checked          = fos_ex_stg->s_log.log_level != LOG_INFO;
            fosCBGetRelativePath->Checked        = fos_ex_stg->s_local.get_relative_path != 0;
            fosCBRunBatMinimized->Checked        = fos_ex_stg->s_local.run_bat_minimized != 0;
            fosCXDefaultAudioEncoder->SelectedIndex = clamp(fos_ex_stg->s_local.default_audio_encoder_ext, 0, fosCXDefaultAudioEncoder->Items->Count);
            if (str_has_char(fos_ex_stg->s_local.conf_font.name))
                SetFontFamilyToForm(this, gcnew FontFamily(String(fos_ex_stg->s_local.conf_font.name).ToString()), this->Font->FontFamily);
        }
    private:
        System::Void fosBTStgDir_Click(System::Object^  sender, System::EventArgs^  e) {
            FolderBrowserDialog^ fbd = gcnew FolderBrowserDialog();
            if (System::IO::Directory::Exists(fosTXStgDir->Text)) {
                fbd->SelectedPath = Path::GetFullPath(fosTXStgDir->Text);
            }
            if (fbd->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                if (fosCBGetRelativePath->Checked)
                    fbd->SelectedPath = GetRelativePath(fbd->SelectedPath);
                fosTXStgDir->Text = fbd->SelectedPath;
            }
        }
    private:
        System::Void fosCBCancel_Click(System::Object^  sender, System::EventArgs^  e) {
            this->Close();
        }
    private:
        System::Void frmOtherSettings_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
            if (e->KeyCode == Keys::Escape)
                this->Close();
        }
    private:
        System::Void fosBTSetFont_Click(System::Object^  sender, System::EventArgs^  e) {
            fosfontDialog->Font = fosBTSetFont->Font;
            if (fosfontDialog->ShowDialog() != System::Windows::Forms::DialogResult::Cancel
                && String::Compare(fosfontDialog->Font->FontFamily->Name, this->Font->FontFamily->Name)) {
                guiEx_settings exstg(true);
                exstg.load_encode_stg();
                Set_AUO_FONT_INFO(&exstg.s_local.conf_font, fosfontDialog->Font, this->Font);
                exstg.s_local.conf_font.size = 0.0;
                exstg.s_local.conf_font.style = 0;
                exstg.save_local();
                SetFontFamilyToForm(this, fosfontDialog->Font->FontFamily, this->Font->FontFamily);
            }
        }
    public:
        System::Void frmOtherSettings::SetTheme(AuoTheme themeTo, const DarkenWindowStgReader *dwStg) {
            dwStgReader = dwStg;
            CheckTheme(themeTo);
        }
    private:
        System::Void frmOtherSettings::CheckTheme(const AuoTheme themeTo) {
            //変更の必要がなければ終了
            if (themeTo == themeMode) return;

            //一度ウィンドウの再描画を完全に抑止する
            SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 0, 0);
            //tabcontrolのborderを隠す
            SwitchComboBoxBorder(fosTabControl, fosPNHideTabPage, themeMode, themeTo, dwStgReader);
            SetAllColor(this, themeTo, this->GetType(), dwStgReader);
            SetAllMouseMove(this, themeTo);
            //一度ウィンドウの再描画を再開し、強制的に再描画させる
            SendMessage(reinterpret_cast<HWND>(this->Handle.ToPointer()), WM_SETREDRAW, 1, 0);
            this->Refresh();
            themeMode = themeTo;
        }
    private:
        System::Void frmOtherSettings::fosMouseEnter_SetColor(System::Object^  sender, System::EventArgs^  e) {
            fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Hot, dwStgReader);
        }
    private:
        System::Void frmOtherSettings::fosMouseLeave_SetColor(System::Object^  sender, System::EventArgs^  e) {
            fcgMouseEnterLeave_SetColor(sender, themeMode, DarkenWindowState::Normal, dwStgReader);
        }
    private:
        System::Void frmOtherSettings::SetAllMouseMove(Control ^top, const AuoTheme themeTo) {
            if (themeTo == themeMode) return;
            System::Type^ type = top->GetType();
            if (type == CheckBox::typeid) {
                top->MouseEnter += gcnew System::EventHandler(this, &frmOtherSettings::fosMouseEnter_SetColor);
                top->MouseLeave += gcnew System::EventHandler(this, &frmOtherSettings::fosMouseLeave_SetColor);
            }
            for (int i = 0; i < top->Controls->Count; i++) {
                SetAllMouseMove(top->Controls[i], themeTo);
            }
        }
};
}
