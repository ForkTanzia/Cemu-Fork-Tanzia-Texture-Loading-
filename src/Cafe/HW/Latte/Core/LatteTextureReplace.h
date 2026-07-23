#pragma once
#include "Cafe/HW/Latte/ISA/LatteReg.h"
#include <cstdint>

// Runtime custom-texture replacement for Cemu (direct BCn/DDS).
//   Folder:   <UserData>/load/textures/<titleId>/
//   Match by: a full-data content hash of the guest texture (NOT Cemu's sparse
//             texDataHash2, which samples only ~296 bytes and collides between
//             different textures -- e.g. monster subspecies sharing a base texture)
//   Filename: <hash16>_<w>x<h>_fmt<XXXX>_mip<NN>.(dds|png|tga)
// DDS (BC1/BC2/BC3/BC4/BC5) uploads compressed (no RGBA8). PNG/TGA = RGBA8 fallback.

struct LatteTextureReplace_Entry
{
	uint8_t* data     = nullptr;   // BCn blocks if isCompressed, else RGBA8
	int      width    = 0;
	int      height   = 0;
	uint32_t dataSize = 0;
	bool     isCompressed = false;
	uint32_t gx2Format = 0;
};

namespace LatteTextureReplace
{
	bool IsEnabled();
	const LatteTextureReplace_Entry* GetSlice(uint64_t contentHash, int mipIndex);

	// reserved for the future auto-overwrite (ruleless) path
	struct ReplacementInfo { int width, height; bool hasFormat; uint32_t gx2Format; };
	bool GetInfo(uint64_t contentHash, ReplacementInfo& out);
	// full-data hash over the guest mip0 surface; stable and unique per distinct texture
	uint64_t HashGuest(uint32_t physImagePtr, uint32_t sizeBytes, uint32_t pixelCount, Latte::E_GX2SURFFMT fmt);
	uint64_t HashGuestRaw(uint32_t physImagePtr, uint32_t sizeBytes); // always computes (used for dump naming)
}
