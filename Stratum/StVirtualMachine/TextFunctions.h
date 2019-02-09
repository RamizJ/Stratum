#ifndef TEXTFUNCTIONS_H
#define TEXTFUNCTIONS_H

namespace StVirtualMachine {

extern void setupTextFunctions();

//Font
extern void createFont2dPx();
extern void createFont2dPt();

extern void getFontName2d();
extern void getFontSize2d();
extern void getFontStyle2d();
extern void setFontName2d();
extern void setFontSize2d();
extern void setFontStyle2d();
extern void getFontList();

//String
extern void createString2d();
extern void getString2d();
extern void setString2d();

//Text
extern void createText2d();
extern void createRasterText2d();
extern void getTextObject2d();
extern void getTextCount2d();

extern void getTextFont2d();
extern void getTextFont2d_i();

extern void getTextString2d();
extern void getTextString2d_i();

extern void getTextFgColor2d();
extern void getTextFgColor2d_i();

extern void getTextBkColor2d();
extern void getTextBkColor2d_i();

extern void setText2d();
extern void setText2d_i();

extern void setTextFont2d();
extern void setTextString2d();
extern void setTextFgColor2d();
extern void setTextBkColor2d();

extern void addText2d();
extern void insertText2d();
extern void removeText2d();

}

#endif // TEXTFUNCTIONS_H
