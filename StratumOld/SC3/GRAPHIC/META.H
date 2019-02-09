/*
Copyright (c) 1995  Virtual World
Module Name:
	 meta.h
Author:
	 Alexander Shelemekhov
*/
#ifndef __META_SIGNATURES___
#define __META_SIGNATURES___

#define mfstartofgroup		  1
#define mfendofgroup			  2
#define mfmerge              3
#define mfsetscale2d         4
// Tools2d
#define mfdeleteunusedtools  10
#define mfdeletetool         11
#define mfcreatetool         12
#define mfloadtools          13
// pen
#define mfcreatepen          20
#define mfsetpenstyle        21
#define mfsetpenwidth        22
#define mfsetpencolor        23
#define mfsetpenrop2         24
// brush
#define mfcreatebrush        30
#define mfsetbrushstyle      31
#define mfsetbrushhatch      32
#define mfsetbrushdibindex   33
#define mfsetbrushcolor      34
#define mfsetbrushrop2       35
// dib
#define mfcreatedib          40
#define mfsetdib             41
#define mfrcreatedib         42
#define mfrsetdib            43

// doubledib
#define mfcreatedoubledib    45
#define mfsetdoubledib       46
#define mfrcreatedoubledib   47
#define mfrsetdoubledib      48

// font
#define mfcreatefont         50
#define mfsetlogfont         51

// string
#define mfcreatestring       60
#define mfsetlogstring       61
// text
#define mfcreatetext         62
#define mfsetlogtext         63
// group2d
#define mfcreategroup2d      100
#define mfdeletegroup2d      101
#define mfdeletegroupitem2d  102
#define mfsetgroupitems2d    103
#define mfsetgroupitem2d     104
#define mfaddgroupitem2d     105
#define mfloadgroupitem2d    106

// RGN group
#define mfcreatergngroup2d   110
#define mfsetrgngroupitems   111
#define mfsetrgngroupitem    112
#define mfaddrgngroupitem    113

#define mfcreate3dspace      200
#define mfloadspace3d		  201
// Objects
#define mfloadobject__       500

// Objects2d
// all 2d objects
#define mfdeleteobject2d     1000
#define mfsetobjectorg2d     1001
#define mfsetobjectsize2d    1002
#define mfrotateobject2d     1003
#define mfcoordinates        1004
#define mfloadobject2d       1005

//#define mfsetshowobject      1010
#define mfsetbkbrush         1011
#define mfsetlayercolor      1012
#define mfsetcurrentobj2d    1013
#define mfsetattribute2d     1014
#define mfsetobject2dptr     1015


// polyline
#define mfcreatepolyline     1100
#define mfaddvectorpoint     1101
#define mfdelvectorpoint     1102
#define mfsetvectorpoints    1103
#define mfsetvectorpoint     1104
#define mfsetrgncreatemode   1105
#define mfsetpenobject       1106
#define mfsetbrushobject     1107

// all bitmasps (double and single)
#define mfsetbitmapsrcrect   1110
// bitmap
#define mfcreatebitmap       1120
#define mfsetdibobject       1121
// double
#define mfcreatedoublebitmap 1130
#define mfsetdoubledibobject 1131
// text
#define mfcreatetextraster   1140
#define mfsettextobject      1141
#define mfsettextdelta      1142
// 3d projection
#define mfcreate3dprojection 1150
#define mfset3dspace2d       1151
#define mfset3dviewparam     1152

// Z order
#define mfZtotop             1200
#define mfZtobottom          1201
#define mfZswap              1202
#define mfZset               1203

// 3d

#define mfdeletespace3d      2001

#define mfaddpoint3d         2010
#define mfdelpoint3d         2011
#define mfdeleteunusedpoint3d 2012
#define mfsetpoint3d         2013
#define mfsetpoints3d        2014
#define mfloadpoint3d         2015
#define mfloadpoints3d        2016
#define mfloadunusedpoints3d 2017

#define mfaddline3d          2020
#define mfdelline3d          2021
#define mfsetline3d          2022
#define mfsetlines3d         2023
#define mfloadline3d         2024
#define mfloadlines3d        2025

#define mfaddtriangle3d      2030
#define mfdeltriangle3d      2031
#define mfsettriangle3d      2032
#define mfsettriangles3d     2033
#define mfloadtriangle3d     2034
#define mfloadtriangles3d    2035

// group3d
#define mfcreategroup3d      2040
#define mfdeletegroup3d      2041
#define mfdeletegroupitem3d  2042
#define mfsetgroupitems3d    2043
#define mfsetgroupitem3d     2044
#define mfaddgroupitem3d     2045
#define mfloadgroupitem3d    2045

#define mfcreateobject3d     2061
#define mfdeleteobject3d     2062
#define mfsetcurrentobject3d 2063
#define mfsetobjectsize3d    2064
#define mfrotateobject3d     2065
#define mfsetobjectorg3d     2066
#define mfdeleteemptyobjects3d 2067
#define mfloadobject3d      2068
#define mfloadcoordinates3d 2069
#define mfpasteobject3d     2070
#define mfsetattribute3d    2071
#define mfsetcolor3d        2072
#define mfloadcolor3d       2073
#endif //__META_SIGNATURES___
