# MH3U 3DS Textures to Wii U

Convert Monster Hunter 3 Ultimate **3DS** textures into textures the **Wii U** version can load
through [Cemu-Fork-Tanzia](https://github.com/ForkTanzia/Cemu-Fork-Tanzia-Texture-Loading-).

These scripts work with textures you have extracted yourself from copies of the game you own.
They map each Wii U texture to its 3DS counterpart and transcode the result into the format and
naming the fork expects.

> **Not affiliated with or endorsed by Cemu or Nintendo.**
> This repository contains **only conversion scripts** — no game files and no texture art.
> Note that UI textures are not converted, as the 3DS and Wii U UI layouts differ.

> ### Using third-party texture packs
> This tool works with texture packs you supply yourself. **Check the terms of whichever pack
> you use.** Many texture packs are released under terms that do not permit converting,
> repackaging, porting, or redistributing them — do not convert another author's work without
> their permission. If you publish screenshots or videos of results made with someone else's
> pack, credit that pack's author clearly, so their work isn't mistakenly attributed to this
> tool or to the emulator fork.

---

## What you need
- **Python 3** — https://www.python.org/downloads/ (tick "Add Python to PATH" on install)
- **texconv.exe** — https://github.com/microsoft/DirectXTex/releases (the texture converter)
- Both 3DS and Wii U version of MH3U
- Your **Wii U** texture extraction and your **3DS** romfs dump
- Python libraries: `pip install -r requirements.txt`

> **Python version:** use **Python 3.12** for the full pipeline. `cityhash` (needed only by
> `hash_bridge.py`) has no prebuilt wheel for the newest Python releases and will try to compile,
> failing with *"Microsoft Visual C++ 14.0 or greater is required"*.

---

## How to use

You build the mapping yourself from your own dumps. This requires extracting textures from
**both** the Wii U and 3DS versions of the game you own.

**Setup — extracting the game files**

1. Grab a Wii U game extraction tool such as **Uwizard** and extract the files. The extracted
   folder should include the `.arc` files.

2. Grab **quickbms** from its website, find `dmc4.bms`, and put it in the same folder as
   `quickbms.exe`.

3. Make a `.bat` file in the quickbms folder with the line below (replace the paths with your
   own) and run it:
   ```
   quickbms.exe -d -o -. -F "{}.arc" dmc4.bms "C:\[your .arcs folder]" "C:\[Your Extract folder]"
   ```

4. After it finishes you should have a set of extracted `.arc` folders.

5. On your 3DS emulator, dump the romfs for MH3U.

**Converting**

1. Build the match report (Wii U texture → 3DS texture):
   ```
   py hash_bridge.py "C:\[Your extracted Wii U .arcs folder]" "C:\[Your MH3U dumped romfs folder]" "C:\[Your 3DS pack folder]"
   ```
   This writes `bridge_report.csv`.

2. Convert:
   ```
   py cemu_names.py bridge_report.csv "C:\[Your extracted Wii U .arcs folder]" "C:\[Your 3DS pack folder]" "C:\[Your output folder]" ^
      --texconv "C:\[Your texconv.exe path]"
   ```

3. Copy the contents of your output folder into Cemu's `load\textures\` folder.

To keep things simple, I'd recommend keeping all the scripts and folders in one place.

---

## Extra tools

**`tex_to_name.py`** — prints the replacement filename for a Wii U `.tex`, so you can name a
hand-made replacement correctly:
```
py tex_to_name.py "C:\[path to a .tex file or folder]"
```

**`check_mip_darkening.py`** — audits a converted folder for textures whose mipmaps came out too
dark (which shows as black patches at a distance):
```
py check_mip_darkening.py "C:\[Your output folder]"
```

---

## Viewing raw textures (optional)

To convert raw `.tex` files into PNG, use Noesis with the included
`fmt_MonsterHunter_TEX.py` plugin:

1. Download and install Noesis.
2. Paste `fmt_MonsterHunter_TEX.py` into Noesis's `plugins/python` folder.
3. You can now view and export `.tex` files in Noesis.
4. To batch convert every `.tex` to PNG, make a `.bat` file with:
   ```
   @echo off
   set NOESIS=C:\[Your Noesis.exe]

   for /r "C:\[Your extracted Wii U .arcs folder]" %%f in (*.tex) do "%NOESIS%" ?cmode "%%f" "%%~dpnf.png"
   pause
   ```

---

## Options
- `--no-flip` — source textures are vertically flipped by default. If textures appear
  upside-down in game, re-run with `--no-flip`.
- Formats: BC1/BC2 and RGBA8 are handled. If the tool reports "unknown fmt," open an issue with
  the format number and it can be added.

## How it works (short version)
Cemu-Fork-Tanzia matches custom textures by a content hash of the original game texture. These
scripts reproduce that hash offline from the Wii U texture data, bridge each Wii U texture to its
3DS counterpart, and transcode the art into the format and naming the fork expects. A full
technical write-up is in the fork's repository.

## Credits
- **Cemu** by the Cemu project (MPL-2.0).

## License
MIT (these scripts only). No texture art is included or covered by this license.
