
# ffmpegOut  
by rigaya  

ffmpegOut は、[ffmpeg](https://ffmpeg.org/)を使用してエンコードを行うAviutlの出力プラグインです。

## ダウンロード & 更新履歴
[こちら＞＞](https://github.com/rigaya/ffmpegOut/releases)

## 想定動作環境
Windows 8.1/10/11 (x86/x64)  
Aviutl 1.00 以降 推奨

## ffmpegOut 使用にあたっての注意事項
無保証です。自己責任で使用してください。  
ffmpegOutを使用したことによる、いかなる損害・トラブルについても責任を負いません。  

## ffmpegOut の Aviutl への導入・更新

### ダウンロード

まず、ffmpegOutを[こちら](https://github.com/rigaya/ffmpegOut/releases)からダウンロードします。

### 導入・更新

ダウンロードしたzipファイルをダブルクリックして開きます。中身はこんな感じです。

![zipファイルの中身](./data/ffmpegOut_1_00_install_02.png)

中身をすべてAviutlフォルダにコピーします。

![中身のコピー](./data/ffmpegOut_1_00_install_03.png)

更新時には、下記のように上書きするか聞いてくることがあります。

その場合には「ファイルを置き換える」を選択して上書きしてください。

![上書き](./data/ffmpegOut_1_00_install_04.png)
  

次に、ffmpeg本体をダウンロードします。ffmpeg本体は下記サイトからダウンロードすることが可能です。
- [gyan.dev](https://www.gyan.dev/ffmpeg/builds/)
- [BtbN](https://github.com/BtbN/FFmpeg-Builds/releases)

ダウンロードしたffmpeg.exeをAviutlフォルダ内にあるexe_filesフォルダの中にコピーします。

![ffmpegのコピー](./data/ffmpegOut_1_00_install_03_2.png)
  
このあとAviutlをダブルクリックして起動してください。

初回起動時に必要に応じて下の図のようにffmpegOutの使用準備をするというメッセージが出ます。環境によっては準備が不要な場合があり、その場合は表示されません。

OKをクリックすると使用準備が開始されます。

![初回起動時](./data/ffmpegOut_1_00_install_05.png)
  
  
  
下の図のように、「この不明な発行元からのアプリがデバイスに変更を加えることを許可しますか?」というメッセージが表示されますので、「はい」をクリックしてください。

![UAC](./data/ffmpegOut_1_00_install_06.png)
  
  
  
下の図のようなウィンドウが表示され、ffmpegOutの使用に必要なモジュールがインストールされます。

エラーなくインストールが完了すると下記のように表示されますので、右上の[x]ボタンでウィンドウを閉じてください。

![auo_setup](./data/ffmpegOut_1_00_install_11.png)

これで使用準備は完了です。

### 確認

ffmpegOutがAviutlに認識されているか確認します。

Aviutlの [その他] > [出力プラグイン情報]を選択します。

![出力プラグイン情報](./data/ffmpegOut_1_00_install_07.png)


ffmpegOut 1.xxが表示されていれば成功です。

![確認](./data/ffmpegOut_1_00_install_09.png)


### エンコード
[ ファイル ] > [ プラグイン出力 ] > [ ffmpegOut ] を選択し、出力ファイル名を入力して、「保存」をクリックしてください。

![プラグイン出力](./data/ffmpegOut_1_00_install_14.png)

エンコードが開始されます。

![エンコード](./data/ffmpegOut_1_00_install_10.png)

エンコードが完了するまで待ちます。お疲れ様でした。


## iniファイルによる拡張
ffmpegOut.iniを書き換えることにより、音声エンコーダやmuxerのコマンドラインを変更できます。また音声エンコーダを追加することもできます。

デフォルトの設定では不十分だと思った場合は、iniファイルの音声やmuxerのコマンドラインを調整してみてください。

## ffmpegOut の Aviutl からの削除

ffmpegOut の Aviutl から削除するには、"plugins" フォルダ内の下記ファイルとフォルダを削除してください。

- [フォルダ] ffmpegOut_stg
- [ファイル] ffmpegOut.auo
- [ファイル] ffmpegOut(.ini)
- [ファイル] auo_setup.auf

![削除](./data/ffmpegOut_1_00_uninstall_01.png)


## ソースコードについて
- MITライセンスです。

### ソースの構成
VCビルド  
文字コード: UTF-8-BOM  
改行: CRLF  
インデント: 空白x4  
