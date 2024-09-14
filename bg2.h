
//{{BLOCK(bg2)

//======================================================================
//
//	bg2, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 39 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 1248 + 2048 = 3808
//
//	Time-stamp: 2023-04-22, 23:20:46
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BG2_H
#define GRIT_BG2_H

#define bg2TilesLen 1248
extern const unsigned short bg2Tiles[624];

#define bg2MapLen 2048
extern const unsigned short bg2Map[1024];

#define bg2PalLen 512
extern const unsigned short bg2Pal[256];

#endif // GRIT_BG2_H

//}}BLOCK(bg2)
