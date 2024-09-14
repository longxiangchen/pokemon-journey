
//{{BLOCK(mapTwo)

//======================================================================
//
//	mapTwo, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 15 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 480 + 2048 = 3040
//
//	Time-stamp: 2023-04-29, 22:12:52
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MAPTWO_H
#define GRIT_MAPTWO_H

#define mapTwoTilesLen 480
extern const unsigned short mapTwoTiles[240];

#define mapTwoMapLen 2048
extern const unsigned short mapTwoMap[1024];

#define mapTwoPalLen 512
extern const unsigned short mapTwoPal[256];

#endif // GRIT_MAPTWO_H

//}}BLOCK(mapTwo)
