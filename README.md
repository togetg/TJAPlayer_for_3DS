[English](README_en.md)

# TJAPlayer for 3DS

太鼓さん次郎等で使われている[TJAファイル](https://wikiwiki.jp/jiro/%E5%A4%AA%E9%BC%93%E3%81%95%E3%82%93%E6%AC%A1%E9%83%8E#h2_content_1_8)を3DSでプレイします。

# 使い方

1. **TJAファイルの文字コードをUFT-8に変換します。**<br>この作業はフリーソフト等を使って一括で行うことをオススメします。<br>TITLEやWAVEに日本語などのマルチバイト文字が含まれていない場合はこの作業は必要ありません。
2. 3DSのSDカードのルートに「tjafiles」というフォルダを作り、その中にTJAファイルと音源ファイルを入れてください。

# 注意

- **音楽が遅れる時は音源ファイルのビットレートを64Kbps、サンプルレートを32000Hzにしてください。**
- 音源ファイルはOggファイルのみ対応してます。
- ノーツ音と音源が全く聞こえない時は[DSP1](https://github.com/zoogie/DSP1/releases)を起動してください。

# 対応していない機能

- ノーツ
  - イモ連打(ドン音符に置き換え)
- ヘッダ
  - SONGVOL
  - SEVOL
  - STYLE
  - LIFE
  - DEMOSTART
  - TOTAL
- 命令
  - BMSCROLL
  - HBSCROLL
- その他
  - TJCフォーマット
  - TJFフォーマット
  - ジャンル分け
  - スコアの保存

# ビルド
1. [devkitPro](https://github.com/devkitPro/installer/releases/tag/v3.0.3)をインストール

2. 以下のコマンドでパッケージをインストーする

   `pacman -S 3ds-libogg 3ds-dev 3ds-libvorbisidec --noconfirm`

3. [MakeROM](https://github.com/3DSGuy/Project_CTR/releases)と[bannertool](https://github.com/Steveice10/bannertool/releases)をダウンロードしPATHを通す

4. Build.batを起動
