# Cemu - Fork Tanzia [Texture Loading]
**A fork of Cemu made specifically for Monster Hunter 3 Ultimate (Wii U version) texture loading.**

> This project is **not affiliated with nor endorsed by Cemu or Nintendo.**
> This is a free and open-source (MPL-2.0) community fork of Cemu.

## What this is
This is a fork that allows you to load custom textures, including upscaled textures, on Cemu.
The fork adds **runtime custom-texture loading** — much like 3DS emulators — which allows
custom textures to be swapped in as the game runs. While it is built with MH3U in mind, it
could work for other games too.

Textures are matched automatically as they load. In rare cases (e.g. swapping equipment), a
texture may briefly appear vanilla or blank before the correct replacement is applied — this
resolves itself within about half a second, and no action is needed. If a texture does bug out,
press **F6** (rebindable in hotkey settings) to manually reload all textures.

You supply your own textures.

> ### Using third-party texture packs
> This tool works with texture packs you supply yourself. **Check the terms of whichever pack
> you use.** Many texture packs are released under terms that do not permit converting,
> repackaging, porting, or redistributing them — do not convert or redistribute another
> author's work without their permission. If you publish screenshots or videos of results made
> with someone else's pack, credit that pack's author clearly, so their work isn't mistakenly
> attributed to this tool or to the emulator fork.

## How texture loading works
- Place custom textures in `<Cemu>/load/textures/`.
- Files are matched to game textures by content hash; the fork sizes each texture from the
  replacement itself (no graphic-pack rules needed).
- Filenames follow `<hash16>_<w>x<h>_fmt<XXXX>_mip00.dds`. A dumped texture's filename can be
  reused directly as the replacement's filename.
- Supported: DDS (BC1–BC5, with mipmaps) and PNG/TGA.
- Custom-texture loading is enabled by default in this build.
- Press **F6** (rebindable in hotkey settings) to reload all textures.

## Compatibility
Works alongside resolution graphic packs. Render targets are excluded from texture replacement,
so scaling packs and this fork don't interfere with each other.

## Download & run (ready-to-use build)
1. Go to the [Releases](../../releases) page and download the latest build.
2. Extract the zip anywhere (a path **without spaces** is safest).
3. Run `Cemu.exe` and set up your game as normal.
4. Put your textures in `<Cemu>/load/textures/`.

## Building

For compilation instructions, see [BUILD.md](BUILD.md).

## Credits
- **Cemu** by the Cemu project — https://github.com/cemu-project/Cemu (MPL-2.0)

## Support
The fork and all tools are free to download and build. If it's been useful, you can
optionally support my work:

[![Support me on Ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/H8L623H70U)

## License
This fork is licensed under **MPL-2.0**, same as Cemu.
Cemu is licensed under [Mozilla Public License 2.0](/LICENSE.txt). Exempt from this are all files in the dependencies directory for which the licenses of the original code apply as well as some individual files in the src folder, as specified in those file headers respectively.
