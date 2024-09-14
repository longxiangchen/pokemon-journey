
//{{BLOCK(battleSelect)

//======================================================================
//
//	battleSelect, 256x256@4, 
//	+ palette 256 entries, not compressed
//	+ 489 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 15648 + 2048 = 18208
//
//	Time-stamp: 2023-04-22, 18:02:58
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BATTLESELECT_H
#define GRIT_BATTLESELECT_H

#define battleSelectTilesLen 15648
extern const unsigned short battleSelectTiles[7824];

#define battleSelectMapLen 2048
extern const unsigned short battleSelectMap[1024];

#define battleSelectPalLen 512
extern const unsigned short battleSelectPal[256];

#endif // GRIT_BATTLESELECT_H

//}}BLOCK(battleSelect)
