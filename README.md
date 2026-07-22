# Cemu - Fork Tanzia [Texture Loading]
**A fork of Cemu made specifically for Monster Hunter 3 Ultimate (Wii U version) texture loading.**

> This project **not affiliated with nor endorsed by Cemu, Nintendo, or NGRP (by raccu).**
> This is a free and open-source (MPL-2.0) community fork of Cemu.

## What this is
This is a fork that allows you to load custom textures, including upscale textures, on Cemu. 
The fork adds **runtime custom-texture loading** - much like 3DS emulators - which allows
for custom textures to load . While it is built with MH3U in mind, it could work for other
games too. Textures are matched automatically as they load. In rare cases (e.g. swapping equipment),
a texture may briefly appear vanilla/blank for half a second before the correct replacement is applied — this
resolves itself within about half a second. No action needed. If in any case a texture does bug out you can press
"F6" (Can be rebound in hotkey settings) to manually reload the textures.

Alongside it, there's a separate tool that converts textures from **NGRP by raccu** (an HD
texture project for the *3DS* version of MH3U) into a form the Wii U version can use.
**I do not own the NGRP assets, so I cannot distribute converted textures** — you get NGRP
from its official source and run the converter yourself.
➡️ Conversion scripts: [3DS-to-Wii U](https://github.com/ForkTanzia/MH3U-3DS-textures-to-Wii-U)

## How texture loading works
- Place custom textures in `<Cemu>/load/textures/`.
- Files are matched to game textures by content hash; the fork sizes each texture from the
  replacement itself (no graphic-pack rules needed).
- Supported: DDS (BC1–BC5, with mipmaps) and PNG/TGA.
- Custom-texture loading is enabled by default in this build.
- Press F6 (can be changed in hotkey settings) to reload all textures. This is mostly for if textures bug out for any reason.

## Download & run (ready-to-use build)
1. Go to the [Releases](../../releases) page and download the latest build.
2. Extract the zip anywhere (a path **without spaces** is safest).
3. Run `Cemu.exe` and set up your game as normal.
4. Put your converted textures in `<Cemu>/load/textures/`.

## Video demo featuring custom textures
[![Custom Texture Demo](https://img.youtube.com/vi/nntnY9l1PAI/maxresdefault.jpg)](https://www.youtube.com/watch?v=nntnY9l1PAI)

## Building

For compilation instructions, see [BUILD.md](BUILD.md).

## Credits
- **Cemu** by the Cemu project — https://github.com/cemu-project/Cemu (MPL-2.0)
- **NGRP** HD texture project by **raccu** — https://www.raccu.com/releases/runtime/mh3u-3g/

## Support
The fork and all tools are free to download and build. If it's been useful, you can
optionally support my work:

[![Support me on Ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/H8L623H70U)

## License
This fork is licensed under **MPL-2.0**, same as Cemu.
Cemu is licensed under [Mozilla Public License 2.0](/LICENSE.txt). Exempt from this are all files in the dependencies directory for which the licenses of the original code apply as well as some individual files in the src folder, as specified in those file headers respectively.
