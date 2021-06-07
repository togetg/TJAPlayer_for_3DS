[English](README_en.md)

![banner](https://user-images.githubusercontent.com/18244518/120987722-d49e8300-c7b8-11eb-9d13-f5c92804dae2.png)

It's a music game of the [TJA file](https://wikiwiki.jp/jiro/%E5%A4%AA%E9%BC%93%E3%81%95%E3%82%93%E6%AC%A1%E9%83%8E#h2_content_1_8) used by Taiko Jiro on 3DS.

# Usage

1. **Convert character code of TJA file to UFT-8.**<br>I recommend that you do this task collectively with [KanjiTranslator](https://www.kashim.com/kanjitranslator/) etc.<br>If "TITLE" and "WAVE" don't contain multibyte characters such as Japanese, this work isn't necessary.
2. Create a folder named "tjafiles" in the root of the SD card of 3DS, and put in the TJA file and the sound file in it.

# Note

- **If the music is delayed, set a larger buffer size.(This will reduce the frame rate.)**<br>A music file with a bit rate of 64Kbps and a sample rate of 32000Hz can be played with a buffer size of about 8000.
- Only the Ogg file is supported for the sound file.
- If you don't hearh the notes sound and music at all, start up [DSP1](https://github.com/zoogie/DSP1/releases).

# Not supported

- Note
  - Potato roll (Replace it with Don note)
- Header
  - SONGVOL
  - SEVOL
  - STYLE
  - LIFE
  - DEMOSTART
  - TOTAL
- Command
  - BMSCROLL
  - HBSCROLL
- Other
  - TJC format
  - TJF format
  - Genre
  - Save score

# Screenshots
<details><summary>Screenshots</summary>

![1](https://user-images.githubusercontent.com/18244518/120986498-a3718300-c7b7-11eb-9036-8d9807a1b5c0.png) ![2](https://user-images.githubusercontent.com/18244518/120986505-a66c7380-c7b7-11eb-9c61-d98f752e9f2d.png)
![3](https://user-images.githubusercontent.com/18244518/120986516-a8363700-c7b7-11eb-83af-db1fe4a3de32.png) ![4](https://user-images.githubusercontent.com/18244518/120986526-a9fffa80-c7b7-11eb-99a4-0756e8dae4db.png)
![5](https://user-images.githubusercontent.com/18244518/120986545-ad938180-c7b7-11eb-89ad-bafca4c2c441.png)

</details>

# How to Build
1. Install [devkitPro](https://github.com/devkitPro/installer/releases/)

2. Install the package with the following command

`pacman -S 3ds-libogg 3ds-dev 3ds-libvorbisidec 3ds-jansson --noconfirm`

3. Download [MakeROM](https://github.com/3DSGuy/Project_CTR/releases) and [bannertool](https://github.com/Steveice10/bannertool/releases) and set the PATH.
4. Run Build.bat

# Credits
- [Robichani6-1](https://github.com/Robichani6-1) - Spanish Translation
- [ManOfNoWonder](https://github.com/ManOfNoWonder) - English Translation