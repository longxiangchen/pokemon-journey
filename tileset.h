
//{{BLOCK(tileset)

//======================================================================
//
//	tileset, 256x512@4, 
//	+ palette 256 entries, not compressed
//	+ 402 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x64 
//	Total size: 512 + 12864 + 4096 = 17472
//
//	Time-stamp: 2023-04-29, 15:13:35
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TILESET_H
#define GRIT_TILESET_H

#define tilesetTilesLen 12864
extern const unsigned short tilesetTiles[6432];

#define tilesetMapLen 4096
extern const unsigned short tilesetMap[2048];

#define tilesetPalLen 512
extern const unsigned short tilesetPal[256];

#endif // GRIT_TILESET_H

//}}BLOCK(tileset)
