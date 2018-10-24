[日本語](README.md)

# TJAPlayer for 3DS

It's a music game of the [TJA file](https://wikiwiki.jp/jiro/%E5%A4%AA%E9%BC%93%E3%81%95%E3%82%93%E6%AC%A1%E9%83%8E#h2_content_1_8) used by Taiko Jiro on 3DS.

# Usage

1. **Convert character code of TJA file to UFT-8.**<br>I recommend that you do this task collectively with software etc.<br>If "TITLE" and "WAVE" don't contain multibyte characters such as Japanese, this work isn't necessary.
2. Create a folder named "tjafiles" in the root of the SD card of 3DS, and put in the TJA file and the sound file in it.

# Note

- **When music is delayed,set the bit rate of the music file to 64 Kbps and the sample rate to 32000 Hz.**
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