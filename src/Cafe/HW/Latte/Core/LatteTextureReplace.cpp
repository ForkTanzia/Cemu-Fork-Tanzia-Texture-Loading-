#include "Cafe/HW/Latte/Core/LatteTextureReplace.h"
#include "config/ActiveSettings.h"
#include "Cafe/CafeSystem.h"
#include "util/helpers/helpers.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <filesystem>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;


namespace LatteTextureReplace
{
	struct DDSInfo { uint32_t gx2Format=0; int bytesPerBlock=0; int width=0, height=0, mipCount=1; uint32_t dataOffset=0; bool ok=false; };
	static uint32_t rd32(const uint8_t* p){ return p[0]|(p[1]<<8)|(p[2]<<16)|((uint32_t)p[3]<<24); }
	static DDSInfo ddsParseHeader(const uint8_t* d, size_t n)
	{
		DDSInfo r;
		if (n < 128 || rd32(d) != 0x20534444) return r;
		r.height = rd32(d+12); r.width = rd32(d+16); r.mipCount = std::max<uint32_t>(1, rd32(d+28));
		uint32_t fourcc = rd32(d+84), dxgi = 0; r.dataOffset = 128;
		auto FCC=[](const char* s){ return (uint32_t)((uint8_t)s[0]|((uint8_t)s[1]<<8)|((uint8_t)s[2]<<16)|((uint32_t)(uint8_t)s[3]<<24)); };
		if (fourcc == FCC("DX10")) { if (n < 148) return r; dxgi = rd32(d+128); r.dataOffset = 148; }
		auto set=[&](uint32_t gx2,int bpb){ r.gx2Format=gx2; r.bytesPerBlock=bpb; r.ok=true; };
		if      (fourcc==FCC("DXT1") || dxgi==70||dxgi==71||dxgi==72) set(0x031, 8);
		else if (fourcc==FCC("DXT3") || dxgi==73||dxgi==74||dxgi==75) set(0x032, 16);
		else if (fourcc==FCC("DXT5") || dxgi==76||dxgi==77||dxgi==78) set(0x033, 16);
		else if (fourcc==FCC("ATI1") || fourcc==FCC("BC4U") || dxgi==80||dxgi==81) set(0x034, 8);
		else if (fourcc==FCC("ATI2") || fourcc==FCC("BC5U") || dxgi==83||dxgi==84) set(0x035, 16);
		return r;
	}
	static uint32_t mipByteSize(int w,int h,int bpb){ return (uint32_t)((std::max(1,(w+3)/4))*(std::max(1,(h+3)/4))*bpb); }

	struct FileRef { fs::path path; int width=0, height=0; bool isDDS=false; uint32_t gx2Format=0; };
	struct HashGroup { std::unordered_map<int,FileRef> mips; std::unordered_map<int,LatteTextureReplace_Entry> decoded; };

	static std::mutex s_mutex;
	static std::unordered_map<uint64_t,HashGroup> s_index;
	static bool s_enabled=false, s_flip=false, s_skipMip=false, s_initDone=false, s_wantEnabled=false;
	static uint64_t s_title=0;

	static bool parseName(const std::string& n,uint64_t& hash,int& w,int& h,int& mip){
		unsigned long long uHash=0; unsigned uFmt=0; int uW=0,uH=0,uMip=0;
		if (sscanf(n.c_str(),"%llx_%dx%d_fmt%x_mip%d",&uHash,&uW,&uH,&uFmt,&uMip)!=5) return false;
		hash=(uint64_t)uHash; w=uW; h=uH; mip=uMip; return true;
	}
	static void loadPackConfig(const fs::path& base){
		std::ifstream f(base/"pack.json"); if(!f) return;
		std::string s((std::istreambuf_iterator<char>(f)),{});
		auto b=[&](const char* k,bool def){ auto p=s.find(k); if(p==std::string::npos)return def;
			auto t=s.find("true",p),fa=s.find("false",p),c=s.find(',',p);
			if(t!=std::string::npos&&(c==std::string::npos||t<c))return true;
			if(fa!=std::string::npos&&(c==std::string::npos||fa<c))return false; return def; };
		s_flip=b("flip_png_files",s_flip); s_skipMip=b("skip_mipmap",s_skipMip);
	}
	static void freeAll(){ for(auto&[h,g]:s_index) for(auto&[m,e]:g.decoded) if(e.data){ if(e.isCompressed) free(e.data); else stbi_image_free(e.data);} s_index.clear(); }

	// lazy (re)build when the title or the setting changes
	static void EnsureInit(){
		bool want = ActiveSettings::LoadCustomTexturesEnabled();
		uint64_t tid = CafeSystem::GetForegroundTitleId();
		if (s_initDone && tid==s_title && want==s_wantEnabled) return;
		s_initDone=true; s_title=tid; s_wantEnabled=want;
		freeAll(); s_enabled=false; s_flip=false; s_skipMip=false;
		if (!want) return;
		fs::path base=ActiveSettings::GetUserDataPath("load/textures"); // sibling of dump/textures (no titleId subfolder, matches Cemu dump)
		std::error_code ec; if(!fs::exists(base,ec)) return;
		loadPackConfig(base);
		size_t count=0;
		for(auto& e: fs::recursive_directory_iterator(base,ec)){
			if(!e.is_regular_file()) continue;
			std::string ext=e.path().extension().string(); for(auto& c:ext) c=(char)tolower(c);
			bool dds=(ext==".dds"), img=(ext==".png"||ext==".tga");
			if(!dds && !img) continue;
			uint64_t hash; int w,h,mip;
			if(!parseName(e.path().filename().string(),hash,w,h,mip)) continue;
			if(s_skipMip && mip!=0) continue;
			FileRef ref{e.path(),w,h,dds,0};
			if(dds){ uint8_t hdr[148]={0}; std::ifstream fi(e.path(),std::ios::binary); fi.read((char*)hdr,148);
				DDSInfo di=ddsParseHeader(hdr,(size_t)fi.gcount()); if(di.ok){ ref.gx2Format=di.gx2Format; ref.width=di.width; ref.height=di.height; } } // use DDS *content* dims (filename now carries the original size)
			s_index[hash].mips[mip]=ref; count++;
		}
		s_enabled = count>0;
		cemuLog_log(LogType::Force,"[TextureReplace] {} files, flip={}, skipMip={}, title {:016x}",count,s_flip,s_skipMip,tid);
	}

	bool IsEnabled(){ std::scoped_lock lock(s_mutex); EnsureInit(); return s_enabled; }

	// Full-data content hash. Every byte of the guest mip0 surface contributes, so distinct
	// textures always get distinct hashes (unlike Cemu's 37-sample texDataHash2, which
	// collides between e.g. monster subspecies built from the same base texture).
	// Must stay bit-identical to strong_hash() in cemu_names.py.
	uint64_t HashData(const uint8* p, uint32_t size){
		uint64_t h = 0;
		const uint32_t nWords = size / 8;
		const uint64_t* w = (const uint64_t*)p;
		for (uint32_t i = 0; i < nWords; i++)
		{
			uint64_t m = (w[i] ^ ((uint64_t)i * 0x9E3779B97F4A7C15ULL)) * 0xFF51AFD7ED558CCDULL;
			m ^= m >> 29;
			h ^= m;
		}
		for (uint32_t i = nWords * 8; i < size; i++)
			h = (h ^ (uint64_t)p[i]) * 0x100000001B3ULL;
		return h;
	}

	uint64_t HashGuest(uint32_t physImagePtr, uint32_t sizeBytes, uint32_t pixelCount, Latte::E_GX2SURFFMT fmt){
		if(!s_enabled) { std::scoped_lock lock(s_mutex); EnsureInit(); if(!s_enabled) return 0; }
		const uint8* p=(const uint8*)memory_getPointerFromPhysicalOffset(physImagePtr);
		if(!p||!sizeBytes) return 0;
		return HashData(p, sizeBytes);
	}

	uint64_t HashGuestRaw(uint32_t physImagePtr, uint32_t sizeBytes){
		const uint8* p=(const uint8*)memory_getPointerFromPhysicalOffset(physImagePtr);
		if(!p||!sizeBytes) return 0;
		return HashData(p, sizeBytes);
	}

	bool GetInfo(uint64_t contentHash, ReplacementInfo& out){
		std::scoped_lock lock(s_mutex); EnsureInit();
		if(!s_enabled||contentHash==0) return false;
		auto it=s_index.find(contentHash); if(it==s_index.end()) return false;
		auto m0=it->second.mips.find(0); if(m0==it->second.mips.end()) return false;
		out.width=m0->second.width; out.height=m0->second.height;
		out.hasFormat=m0->second.isDDS; out.gx2Format=m0->second.gx2Format; return true;
	}

	const LatteTextureReplace_Entry* GetSlice(uint64_t contentHash, int mipIndex){
		std::scoped_lock lock(s_mutex); EnsureInit();
		if(!s_enabled) return nullptr;
		if(s_skipMip && mipIndex!=0) return nullptr;
		auto it=s_index.find(contentHash); if(it==s_index.end()) return nullptr;
		HashGroup& g=it->second;
		if(auto d=g.decoded.find(mipIndex); d!=g.decoded.end() && d->second.data) return &d->second; // trust only cache hits with real data; never cache misses
		// Prefer the base (mip0) file and pull this level from its internal DDS mip chain, so a
		// single mip00 DDS with a full chain serves every mip level (no per-mip copies needed).
		int internalMip = mipIndex;
		auto f=g.mips.find(0);
		if(f==g.mips.end()){ f=g.mips.find(mipIndex); internalMip=0; }
		if(f==g.mips.end()){ return nullptr; } // don't cache the miss -> allow retry
		LatteTextureReplace_Entry ent;
		if(f->second.isDDS){
			std::ifstream in(f->second.path,std::ios::binary);
			std::vector<uint8_t> buf((std::istreambuf_iterator<char>(in)),{});
			DDSInfo di=ddsParseHeader(buf.data(),buf.size());
			if(di.ok && internalMip < di.mipCount){
				uint32_t off=di.dataOffset; int w=di.width,h=di.height;
				for(int m=0;m<internalMip;m++){ off+=mipByteSize(w,h,di.bytesPerBlock); w=std::max(1,w>>1); h=std::max(1,h>>1); }
				uint32_t sz=mipByteSize(w,h,di.bytesPerBlock);
				if(off+sz<=buf.size()){ ent.data=(uint8_t*)malloc(sz); memcpy(ent.data,buf.data()+off,sz);
					ent.width=w; ent.height=h; ent.dataSize=sz; ent.isCompressed=true; ent.gx2Format=di.gx2Format; }
			}
		} else {
			stbi_set_flip_vertically_on_load(s_flip?1:0);
			int w,h,comp; uint8_t* rgba=stbi_load(f->second.path.string().c_str(),&w,&h,&comp,4);
			if(rgba){ ent.data=rgba; ent.width=w; ent.height=h; ent.dataSize=(uint32_t)w*h*4; ent.isCompressed=false; }
		}
		if(!ent.data){ cemuLog_log(LogType::Force,"[TextureReplace] load failed: {}",f->second.path.string()); return nullptr; } // don't cache failure -> allow retry
		g.decoded[mipIndex]=ent;
		return &g.decoded[mipIndex];
	}
}
