[English](README_en.md)
<div align="center">
    <img src="https://user-images.githubusercontent.com/18244518/120987722-d49e8300-c7b8-11eb-9d13-f5c92804dae2.png" alt="banner">
</div>

太鼓さん次郎等で使われている[TJAファイル](https://wikiwiki.jp/jiro/%E5%A4%AA%E9%BC%93%E3%81%95%E3%82%93%E6%AC%A1%E9%83%8E#h2_content_1_8)を3DSでプレイします。

# 使い方

1. **TJAファイルの文字コードをUFT-8に変換します。**<br>この作業は[KanjiTranslator](https://www.kashim.com/kanjitranslator/)等を使って一括で行うことをオススメします。<br>TITLEやWAVEに日本語などのマルチバイト文字が含まれていない場合はこの作業は必要ありません。
2. 3DSのSDカードのルートに「tjafiles」というフォルダを作り、その中にTJAファイルと音源ファイルを入れてください。

# 注意

- **音楽が遅れる、音楽が途切れる時はバッファサイズを大きく設定してください。(フレームレートは低下します)**<br>ビットレートが64Kbps、サンプルレートが32000Hzの音楽ファイルは8000程度のバッファサイズで再生できます。
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

# スクリーンショット
<details><summary>スクリーンショット</summary>

![1](https://user-images.githubusercontent.com/18244518/121764654-886c8d80-cb80-11eb-9ab0-db90cbe2c989.png)　![2](https://user-images.githubusercontent.com/18244518/121764656-8dc9d800-cb80-11eb-8c21-be6b99fc4b0e.png)
![3](https://user-images.githubusercontent.com/18244518/121764658-90c4c880-cb80-11eb-8ee2-af9dc31e5ea7.png)　![4](https://user-images.githubusercontent.com/18244518/121764661-94584f80-cb80-11eb-96b6-31237baea857.png)
![5](https://user-images.githubusercontent.com/18244518/121764663-97ebd680-cb80-11eb-9ab7-c24bc7f5f8bb.png)

</details>

# ビルド
1. [devkitPro](https://github.com/devkitPro/installer/releases/)をインストール

2. 以下のコマンドでパッケージをインストールする

`pacman -S 3ds-libogg 3ds-dev 3ds-libvorbisidec 3ds-jansson --noconfirm`

3. [MakeROM](https://github.com/3DSGuy/Project_CTR/releases)と[bannertool](https://github.com/Steveice10/bannertool/releases)をダウンロードしPATHを通す
4. Build.batを起動

# クレジット
- [Robichani6-1](https://github.com/Robichani6-1) - スペイン語翻訳
- [ManOfNoWonder](https://github.com/ManOfNoWonder) - 英語翻訳
