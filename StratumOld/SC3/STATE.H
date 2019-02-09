#define sfbase 0
#define sfObase       sfbase
#define sfOdown1      500
#define sfOTrack      501   // таскание имиджей
#define sfrectselect  502
#define sfrectselect1 503
#define sfrectbase    504
#define sfrectdown    505
#define sfrectdrag    506  // таскание кучи имиджей


#define sfRECTstart   502
#define sfRECTend     506

#define sfimgaesize1  510
#define sfimgaesize2  511

#define sfcalcorder   540
#define sfnames       541

#define sfLbase       700
#define sfLselect     701
#define sfLnextpoint  702
#define sfLedit       703
#define sfLdown1      704
#define sfLTrack      705
#define sfL_add       706
#define sfL_delete    707
#define sfLDrag       708 // Таскание точки substate 1 - нормальное 0 - добавление
#define sfLClipBase   709
#define sfLClipdown1  710
#define sfLClipTrack  711

#define LINK_MIN      701
#define LINK_MAX      715
// Editor state

#define sflbuttondown2d 2
#define sfctrllbtndown2d 3
#define sfdrag 4
#define sfpaste 5

#define sfeditline 10
#define sfeditline_btndn 11
#define sfeditline_add 12
#define sfeditline_drag 13
#define sfeditline_delete 14

#define sfeditline_start 10
#define sfeditline_end   14

#define sfnewpolyline_1 15
//#define sfnewpolyline1  16
//#define sfnewpolyline2  17

#define sfnewbitmap  100
#define sfnewdoublebitmap 101
#define sfnewtext   102
#define sfnewline   103
#define sfnewline1  104
#define sfnewrect   105
#define sfnewrect1   106
#define sfnewpolyline 107
#define sfnewpolyline1 108
#define sfnewpolyline2 109
#define sfnewgroup     110
#define sfnewrgngroup  111
#define sfaddtogroup   112
#define sfnewcircle    113
#define sfnewcircle1   114
#define sfdragcursor    115
#define sfnewroundrect  116
#define sfnewroundrect1 117
#define sfnewprojection  118
#define sfnewprojection1 119
#define sfHELP           120
#define sfzoom           121
#define sfnewavi         122
#define sfnewgrouprect1  123
#define sfnewgrouprect   124

#define sfnewWinedit     125
#define sfnewWincombo    126
#define sfnewWincheck    127
#define sfnewWinradio    128
#define sfnewWinbutton   129
#define sfnewWinlist     130
#define sfWinwrect       131
#define sfWinwrect1      132

#define sfnewarc         133
#define sfnewarc1        134

#define sfwindrag        135
#define sfwindrag1       136
#define sftransprent     137
#define sftransprent1    138


#define sfbase3d         1000
#define sflbuttondown3d  1001
#define sfctrllbtndown3d 1002
#define sfdrag3d         1003

#define sfnewgroup3d     1004
#define sfaddtogroup3d   1005
#define sfcamera3d       1006

#define sfscnbtndown3d   1007
#define sfscndrag3d      1008
#define sfeditpoint3d    1009
#define sfeditpoint3d1   1010 // нажали
                              // size3d - максим. расстояние
                              // org3d  - base point in local coordinates
                              // tmpdata count INT16[count] POINT3D[count]
#define sfeditpoint3d2   1011


#define s3projection 0
#define s3object 1
#define s3editpoint  2
#define s3editline   3
#define s3edittriangle 4
#define s3editedge 5
#define s3editdepth 6
#define s3editaxis 7
#define s3newcircle 8
#define s3newrect 9

#define AM3D_TEXTURE    1
#define AM3D_VERTEXES   2
#define AM3D_PRIMITIVES 3

