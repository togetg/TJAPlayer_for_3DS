[日本語](README.md)
<div align="center">
    <img src="https://user-images.githubusercontent.com/18244518/120987722-d49e8300-c7b8-11eb-9d13-f5c92804dae2.png" alt="banner">
</div>

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

![1](https://user-images.githubusercontent.com/18244518/121764654-886c8d80-cb80-11eb-9ab0-db90cbe2c989.png)　![2](https://user-images.githubusercontent.com/18244518/121764656-8dc9d800-cb80-11eb-8c21-be6b99fc4b0e.png)
![3](https://user-images.githubusercontent.com/18244518/121764658-90c4c880-cb80-11eb-8ee2-af9dc31e5ea7.png)　![4](https://user-images.githubusercontent.com/18244518/121764661-94584f80-cb80-11eb-96b6-31237baea857.png)
![5](https://user-images.githubusercontent.com/18244518/121764663-97ebd680-cb80-11eb-9ab7-c24bc7f5f8bb.png)

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