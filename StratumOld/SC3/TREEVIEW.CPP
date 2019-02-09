//----------------------------------------------------------------------------
// TreeView - (C) Copyright 1996 by Virtual World
//----------------------------------------------------------------------------
#include "stream.h"
#include "treeview.h"
#include "treeview.rh"

#define long_to_point(lParam) *(POINT*)((void*)(&lParam))
#include "messages.h"
#define STATE_MOUSEDOWN 1
#define STATE_DRAG      2

HBITMAP view_bitmap=0;

extern HINSTANCE hInstance;
static HWND __notify=NULL;
void _DrawIcon(HDC hdc,int x,int y,int i){
    HDC mdc=CreateCompatibleDC(hdc);
    SelectObject(mdc,view_bitmap);
    StretchBlt(hdc,x,y,ITEM_WIDTH,ITEM_HEIGHT,mdc,0,ITEM_HEIGHT*i,16,16,SRCCOPY);
    DeleteDC(mdc);
};


TreeItem::TreeItem(TreeItemData dt,int lev)
{
    NextLeft = NextRight = 0;
    if (!dt.IsImage) Image = BOOK;else Image=dt.IsImage;
    Text = NewStr(dt.text);
    Data = dt.data;
    datatype=dt.datatype;
    Level = lev;
    State = !dt.IsLeaf;
    IsActive = 0;
    Parent=0;
}

void TreeItem::Update(TreeItemData dt)
{
    if (!dt.IsImage) Image = BOOK;
    if (Text)delete Text;Text=NULL;
    Text = NewStr(dt.text);
    Data = dt.data;
    State = !dt.IsLeaf;
}

TreeItem::~TreeItem()
{
    if (NextLeft) delete NextLeft;
    if (NextRight) delete NextRight;
    if (Text)delete Text;Text=NULL;
}

void TreeItem::AddInLeft(TreeItemData dt)
{
    NextLeft = new TreeItem(dt,Level+1);
    NextLeft->Parent=this;
}

void TreeItem::AddInRightEnd(TreeItemData dt)
{
    TreeItem* ptr = this;
    while (ptr->NextRight) ptr = ptr->NextRight;
    ptr->NextRight = new TreeItem(dt,Level);
    ptr->NextRight->Parent=Parent;
}

void TreeItem::DestroyChild()
{
    if (NextLeft){
        delete NextLeft;}
    NextLeft = 0;
}

void TreeItem::ExcludeItem()
{
    // if (NextLeft) delete NextLeft;
    TreeItem* ptr = Parent->NextLeft;
    // NextLeft = 0;
    if (ptr == this) { Parent->NextLeft = NextRight;NextRight=NULL; return; }

    while (ptr->NextRight != this) ptr = ptr->NextRight;

    ptr->NextRight = NextRight;
    NextRight=NULL;
}

int TreeItem::Regen(int num)
{
    Number = num;
    if (NextLeft) num = NextLeft->Regen(num+1);
    if (NextRight) num = NextRight->Regen(num+1);

    if (Number == 0)  // this root object
    {
        switch (State)
        {
            case open : {if (Image==BOOK) Image = BOOK_2; StateImage = MINUS; break; }
            case close : {if (Image==BOOK_2) Image = BOOK; StateImage = PLUS; break; }
            case leaf : StateImage = MINUS; break;
        }
        return num;
    }

    if (NextRight)
    {
        switch (State)
        {
            case open : {if (Image==BOOK) Image = BOOK_2; StateImage = MINUS_LR; break; }
            case close : {if (Image==BOOK_2) Image = BOOK; StateImage = PLUS_LR; break; }
            case leaf : StateImage = LINE_LR; break;
        }
    }
    else
    {
        switch (State)
        {
            case open : {if (Image==BOOK) Image = BOOK_2; StateImage = MINUS_L; break; }
            case close : {if (Image==BOOK_2) Image = BOOK; StateImage = PLUS_L; break; }
            case leaf : StateImage = LINE_L; break;
        }
    }

    return num;
}
BOOL TreeItem::IsContain(TreeItem*data){
    if(data==this)return TRUE;
    BOOL ptr = 0;
    if (NextLeft) ptr = NextLeft->IsContain(data);
    if (ptr) return TRUE;
    if (NextRight) ptr = NextRight->IsContain(data);
    if (ptr) return TRUE;
    return FALSE;

};
TreeItem* TreeItem::Search(DWORD data)
{
    if (Data == data) return this;
    TreeItem* ptr = 0;
    if (NextLeft) ptr = NextLeft->Search(data);
    if (ptr) return ptr;
    if (NextRight) ptr = NextRight->Search(data);
    if (ptr) return ptr;
    return 0;
}
TreeItem* TreeItem::SearchEx(DWORD data,int&i){
    i++;
    if (Data == data) return this;
    TreeItem* ptr = 0;
    if (NextLeft) ptr = NextLeft->SearchEx(data,i);
    if (ptr) return ptr;
    if (NextRight) ptr = NextRight->SearchEx(data,i);
    if (ptr) return ptr;
    return 0;
};
TreeItem* TreeItem::GetItem(int num)
{
    if (Number == num) return this;
    TreeItem* ptr = 0;
    if (NextLeft) ptr = NextLeft->GetItem(num);
    if (ptr) return ptr;
    if (NextRight) ptr = NextRight->GetItem(num);
    if (ptr) return ptr;
    return 0;
}

int TreeItem::GetTreeSize()
{
    if (NextRight) return NextRight->GetTreeSize();
    else
    {
        if (NextLeft) return NextLeft->GetTreeSize();
        else return Number+1;
    }
}

int TreeItem::GetNumberLeft()
{
    if (NextLeft) return NextLeft->Number;
    else return Number;
}

int TreeItem::GetNumberRight()
{
    if (NextRight) return NextRight->GetNumberRight();
    else return Number;
}
int maxnumpaint;
void TreeItem::Paint(int X_ofs,int Y_ofs,HDC dc,RECT& rect)
{
    maxnumpaint=0;
    int UpEdge = (rect.top - Y_ofs)/ITEM_HEIGHT-1;
    int DownEdge = (rect.bottom - Y_ofs)/ITEM_HEIGHT+1;
    Paint(X_ofs,Y_ofs,dc,UpEdge,DownEdge);

    if(maxnumpaint<DownEdge){
        int ypos=Y_ofs + (maxnumpaint+1)*ITEM_HEIGHT;
        PatBlt(dc,0,ypos,2000,rect.bottom-ypos,WHITENESS);
    }

}

void TreeItem::PaintText(int X_ofs,int Y_ofs,HDC hdc)
{
    if (Text) // paint Text
    {
        SelectObject(hdc,HFont);
        if (IsActive)
        {SetBkMode(hdc,OPAQUE);
            SetBkColor(hdc,GetSysColor(COLOR_ACTIVECAPTION	));
            SetTextColor(hdc,GetSysColor(COLOR_CAPTIONTEXT	));
        }
        int xpos=X_ofs + (Level+2)*ITEM_WIDTH + (ITEM_WIDTH>>2);
        int ypos= Y_ofs + Number*ITEM_HEIGHT + (ITEM_HEIGHT>>4);

        char s[128];
        lstrcpy(s," ");
        lstrcat(s,Text);
        lstrcat(s," ");
        int textlen=lstrlen(s);
        ::TextOut(hdc,xpos,ypos,s,textlen);
        if (IsActive){
            SetBkMode(hdc,TRANSPARENT);
            SetBkColor(hdc,RGB(255,255,255));
            RECT r;r.left=xpos; r.top=ypos;
#ifdef WIN32
            SIZE p;
            GetTextExtentPoint(hdc,s,textlen,&p);
            r.right=r.left+p.cx;r.bottom=r.top+p.cy;
#else
            DWORD dw=GetTextExtent(hdc,s,textlen);
            POINT p=long_to_point(dw);
            r.right=r.left+p.x;r.bottom=r.top+p.y;
#endif 

            SetTextColor(hdc,0);
            DrawFocusRect(hdc,&r);
        }
    }
}

void TreeItem::Paint(int X_ofs,int Y_ofs,HDC hdc,int UpEdge,int DownEdge)
{
    if (NextLeft) NextLeft->Paint(X_ofs,Y_ofs,hdc,UpEdge,DownEdge);  // goto left
    if (NextRight) NextRight->Paint(X_ofs,Y_ofs,hdc,UpEdge,DownEdge); // goto right
    // paint long lines
    if (NextRight)
    {
        if ((NextRight->Number < UpEdge) || ( Number > DownEdge)) return; // отсекаем !

        if (Number > UpEdge) UpEdge = Number;
        if (NextRight->Number < DownEdge) DownEdge = NextRight->Number;

        if (((DownEdge - UpEdge) >= 1))
        {
            for (int i = (UpEdge); i < DownEdge; i++)
            {
                _DrawIcon(hdc,X_ofs + Level*ITEM_WIDTH, Y_ofs + i*ITEM_HEIGHT,LINE_R);
            }
        }
    }

    if ((Number < UpEdge) || ( Number > DownEdge)) return; // отсекаем !
    if(Number>maxnumpaint)maxnumpaint=Number;

    int ypos= Y_ofs + Number*ITEM_HEIGHT;
    PatBlt(hdc,X_ofs + (Level-1)*ITEM_WIDTH,ypos,2000,ITEM_HEIGHT,WHITENESS);

    _DrawIcon(hdc,X_ofs + Level*ITEM_WIDTH,ypos, StateImage); // paint State

    if (Image!=USER_IMAGE) // paint Image
    {
        _DrawIcon(hdc,X_ofs + (Level+1)*ITEM_WIDTH,ypos, Image);
    }else{
        TREEPAINTSTRUCT ps;
        ps.x=X_ofs + (Level+1)*ITEM_WIDTH;
        ps.y=ypos;
        ps.sizex=ITEM_WIDTH;
        ps.sizey=ITEM_HEIGHT;
        ps.hDC=hdc;
        ps.data=Data;
        ps.datatype=datatype;
        ::SendMessage(__notify,TVN_NEEDPAINT,0,(LPARAM)&ps);
    }
    PaintText(X_ofs,Y_ofs,hdc);

};

TreeItem* TreeItem::Scan(int X_ofs,int Y_ofs,int x,int y)
{
    TreeItem* te;

    if (NextLeft)
    {
        if ((te = NextLeft->Scan(X_ofs,Y_ofs,x,y)) != 0) return te;
    }
    if (NextRight)
    {
        if ((te = NextRight->Scan(X_ofs,Y_ofs,x,y)) != 0) return te;
    }
    if (State == leaf) return 0;

    if ((x >= (X_ofs + Level*ITEM_WIDTH)) && (x <= (X_ofs + (Level+1)*ITEM_WIDTH))
        && (y >= (Y_ofs + Number*ITEM_HEIGHT)) && ( y <= (Y_ofs + (Number+1)*ITEM_HEIGHT)))
    {
        if (State == open) State = close;
        else State = open;

        return this;
    } else return 0;

}

TreeItem* TreeItem::ScanActive(int X_ofs,int Y_ofs,int x,int y)
{
    TreeItem* te;

    if (NextLeft)
    {
        if ((te = NextLeft->ScanActive(X_ofs,Y_ofs,x,y)) != 0) return te;
    }
    if (NextRight)
    {
        if ((te = NextRight->ScanActive(X_ofs,Y_ofs,x,y)) != 0) return te;
    }

    if ((x >= (X_ofs + (Level+1)*ITEM_WIDTH)) &&
        (y >= (Y_ofs + Number*ITEM_HEIGHT)) && ( y <= (Y_ofs + (Number+1)*ITEM_HEIGHT)))
    {
        //	IsActive = 1;
        return this;
    }
    return 0;
}


//.............................................................

static int ref_to_tree=0;

VTreeWindow::VTreeWindow(LPCREATESTRUCT lp,HWND hw):TWindow(lp,hw)
{shift=0;
    vscroll=new TScroller(this,0,1,16,SB_VERT);
    vscroll->flags|=SF_FIXTOP;
    if(!(ref_to_tree))view_bitmap=LoadBitmap(HResource,MAKEINTRESOURCE(TREE_BITMAP));
    ref_to_tree++;
    ElementList = 0;
    ActiveItem = 0;
    _X_ofs = 0;
    Y_ofs = 0;
    state=0;
    CanDrag=0;
};

VTreeWindow::~VTreeWindow()
{
    ref_to_tree--;
    if (!ref_to_tree)DeleteObject(view_bitmap);
    if (ElementList) delete ElementList;
};


int VTreeWindow::MinViewNumber()
{
    return (int)vscroll->GetPos();
}

int VTreeWindow::MaxViewNumber()
{
    RECT r;
    GetClientRect(HWindow,&r);
    return r.bottom/16+(int)vscroll->GetPos();
    //return (int)vscroll->GetPage() + (int)vscroll->GetPos();
}

LRESULT VTreeWindow::InsertItem(WPARAM, LPARAM lParam)
{
    TreeItemData* pdata = (TreeItemData*)lParam;
    TreeItem* ptr;
    if (ElementList == 0)
    {
        ptr = 0;
    }
    else
    {
        ptr = ElementList->Search(pdata->parent);
        if (!ptr) return 1;                 // error ! not found !!!
    }

    InsertChild(ptr,pdata);

    return 1;
}
LRESULT VTreeWindow::GetItemInfo(int /*code*/,TreeItemData*pdata){
    if (!ElementList) return 0;         // error ! Tree is Empty !!!
    TreeItem* ptr = ElementList->Search(pdata->data);
    if (!ptr) return 0;                 // error ! not found !!!
    GetDATA(pdata,ptr);
    return 1;
};

LRESULT VTreeWindow::UpdateItem(WPARAM, LPARAM lParam)
{
    TreeItemData* pdata = (TreeItemData*)lParam;
    TreeItem* ptr;

    if (!ElementList) return 1;         // error ! Tree is Empty !!!

    ptr = ElementList->Search(pdata->data);
    if (!ptr) return 0;                 // error ! not found !!!
    ptr->Update(*pdata);
    InvalidateRect(HWindow,0,0);
    return 1;
}

void  VTreeWindow::_Expand(int count,TreeItem*ptr){
    if (!ptr) return;                 // error ! not found !!!
    if(!(ptr->IsLeaf()) && ptr->IsClose()){
        ::SendMessage(NotifyWnd,TVN_GETITEMINFO,(WPARAM)HWindow,ptr->Data);
    }
    if (count==0 || count>1){

        ptr=ptr->NextLeft;
        if (count)count--;
        while (ptr){
            _Expand(count,ptr);
            ptr=ptr->NextRight;
        }

    }
};

LRESULT VTreeWindow::Expand(WPARAM level,LPARAM lParam ){
    if (!ElementList) return 1;           // error ! Tree is Empty !!!
    TreeItem* ptr;
    ptr = ElementList->Search((DWORD)lParam);
    _Expand(level,ptr);
};

LRESULT VTreeWindow::DeleteItem(WPARAM, LPARAM lParam)
{
    if (!ElementList) return 1;           // error ! Tree is Empty !!!

    TreeItem* ptr;
    ptr = ElementList->Search((DWORD)lParam);
    if (!ptr) return 0;                 // error ! not found !!!

    DestroyItem(ptr);
    return 1;
}

void VTreeWindow::InsertChild(TreeItem* item,TreeItemData* pdata)
{
    int num = 0;
    if (!item)
    {
        ElementList = new TreeItem(*pdata);
        item = ElementList;
        item->CloseOn();
    }
    else
    {
        if (item->GetNextLeft())
        {
            item->GetNextLeft()->AddInRightEnd(*pdata);
            num = item->GetNumber()+1;
        }
        else
        {
            item->AddInLeft(*pdata);
            num = item->GetNumber();
        }
        item->OpenOn();
    }


    ElementList->Regen();
    RECT r;
    GetClientRect(HWindow,&r);
    r.left=GetX_ofs();r.top=Y_ofs*ITEM_HEIGHT;

    InvalidateRect(HWindow,&r,0);
    // ReleaseDC(HWindow,dc);

    UpdateScrollBar();
}

void VTreeWindow::DestroyItem(TreeItem* item)
{
    CheckActive(item);
    item->DestroyChild();
    // int i=item->GetNumber();
    item->ExcludeItem();
    delete item;
    ElementList->Regen();
    InvalidateRect(HWindow,NULL,0);
    UpdateScrollBar();
    InvalidateRect(HWindow,NULL,0);
};
void VTreeWindow::CheckActive(TreeItem*item){
    if(ActiveItem){
        if(item==ActiveItem)ActiveItem=NULL;
        else{
            if(item->IsContain(ActiveItem))ActiveItem=0;
        }
    }

};

void VTreeWindow::DestroyChild(TreeItem* item)
{
    CheckActive(item);
    item->DestroyChild();
    item->CloseOn();

    ElementList->Regen();

    HDC dc = GetDC(HWindow);
    PatBlt(dc,0,Y_ofs + (item->GetNumber()+1)*ITEM_HEIGHT,2800,2600,WHITENESS);
    SetTextColor(dc,0);
    ElementList->Paint(GetX_ofs(),Y_ofs,dc,item->GetNumber(),MaxViewNumber()+1);
    ReleaseDC(HWindow,dc);
    UpdateScrollBar();

}

void VTreeWindow::UpdateScrollBar()
{
    if (ElementList)
    {
        int num = ElementList->GetTreeSize();
        vscroll->SetRange(0,num);
    }
}

void VTreeWindow::ScrollTo(long* /*x*/,long*y){
    if (y)
    {
        Y_ofs =-(*y * ITEM_HEIGHT);
    }
};

LRESULT VTreeWindow::KeyDown(WPARAM wVkey,LPARAM /*dwKeyData*/)
{
    switch (wVkey)
    {
        case VK_ADD:
        {
            if (!ActiveItem) return 1;
            if (ActiveItem->IsLeaf()) return 1;
            if (ActiveItem->IsOpen()) return 1;
            ::SendMessage(NotifyWnd,TVN_GETITEMINFO,(WPARAM)HWindow,ActiveItem->Data);
            break;
        }
        case VK_SUBTRACT:
        {
            if (!ActiveItem) return 1;
            if (ActiveItem->IsLeaf()) return 1;
            if (ActiveItem->IsClose()) return 1;
            DestroyChild(ActiveItem);
            break;
        }
        case VK_UP :
        {
            ChangeActiveItem(-1);
            break;
        }
        case VK_DOWN :
        {
            ChangeActiveItem(+1);
            break;
        }
        case VK_PRIOR :
        {
            ChangeActiveItem(-vscroll->GetPage());
            break;
        }
        case VK_NEXT :
        {
            ChangeActiveItem(+vscroll->GetPage());
            break;
        }
        case VK_RETURN :
        {vn_Edit();
            break;
        }
        case VK_INSERT :
        {vn_Insert();
            break;
        }
            /*
  case VK_DELETE :
  {vn_Delete();
      break;
  }
*/
        default:return 0;
    }

    return 1;
}

LRESULT VTreeWindow::UpdateClassWindows(UPDATESTRUCT*us)
{
    if (!ElementList) return 1;         // error ! Tree is Empty !!!
    TreeItem* ptr;
    switch (us->mode)
    {
        case UW_DELETE:
        {
            ptr = ElementList->Search((DWORD)us->data);
            if (ptr ) DestroyItem(ptr);
        } break;
        case UW_CREATE:
        {
            ptr = ElementList->Search((DWORD)us->parent);
            if (ptr && ptr->IsOpen())
            {
                ::SendMessage(NotifyWnd,TW_UPDATECLASSWINDOWS,(WPARAM)HWindow,(LPARAM)us);
            }
        }break;
        case UW_MODIFY:
        {
            ptr = ElementList->Search((DWORD)us->data);
            if (ptr)
            {
                ::SendMessage(NotifyWnd,TW_UPDATECLASSWINDOWS,(WPARAM)HWindow,(LPARAM)us);
            }
        }break;
    }
    return 0;
};
void VTreeWindow::GetDATA(TreeItemData*pdata,TreeItem*ptr){
    if(pdata){
        pdata->data=ptr->Data;
        pdata->text=ptr->Text;
        pdata->datatype=ptr->datatype;
        pdata->IsImage=ptr->Image;
        if (ptr->Parent)pdata->parent=ptr->Parent->Data;else
            pdata->parent=0;
    }
};

BOOL VTreeWindow::SetActive(DWORD d){
    //   int i=-1;
    TreeItem* ptr = ElementList->Search(d);
    if(ptr){
        ChangeActiveItem(ptr);
        /*
    int _Y_ofs=Y_ofs;
    int top=Y_ofs/ITEM_HEIGHT;
    RECT r;
    GetClientRect(HWindow,&r);
    if(i<top){
     Y_ofs=i*ITEM_HEIGHT;
    }
    if(i>(top+(r.right/ITEM_HEIGHT))){
     Y_ofs=(i-(r.right/ITEM_HEIGHT))*ITEM_HEIGHT;
    }
    if(Y_ofs!=_Y_ofs)vscroll->SetPos(Y_ofs/ITEM_HEIGHT);
    InvalidateRect(HWindow,0,0);
    ActiveItem=ptr;
    */
        return TRUE;
    }
    return FALSE;
};

LRESULT VTreeWindow::WndProc(UINT message,WPARAM wParam,LPARAM lParam){
    switch(message){
        case WM_GETDLGCODE:return DLGC_WANTARROWS;
        case TW_SETFOCUS:
            //SetControlBarRange(RANGE_TREE);
            return 0;
        case TV_SETSHIFT:if(shift!=wParam){
                shift=wParam;
                InvalidateRect(HWindow,0,0);
                return shift;
            }
        case TVN_GETITEMINFO: return GetItemInfo(wParam,(TreeItemData*)lParam);
        case TV_SETACTIVEITEM:{
            return SetActive(lParam);
        }return 0;
        case TVN_GETACTIVEITEM:if(ActiveItem){
                TreeItemData*pdata=(TreeItemData*)lParam;
                GetDATA(pdata,ActiveItem);
                return ActiveItem->Data;
            }return 0;
        case TVN_INSERTITEM : return InsertItem(wParam,lParam);
        case TVN_DELETEITEM : return DeleteItem(wParam,lParam);
        case TVN_UPDATEITEM : return UpdateItem(wParam,lParam);
        case TVN_GETPARENT  : return (LRESULT)GetParent((HWND)lParam);
        case TV_EXPAND:return Expand(wParam,lParam);
        case TW_UPDATECLASSWINDOWS:return UpdateClassWindows((UPDATESTRUCT*)lParam);
        case WM_KEYDOWN : if(KeyDown(wParam,lParam))return 0;break;

        case WM_PAINT:
        {
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            SetTextColor(hdc,0);
            //	 if (ps.rcPaint.top == -1)GetWindowRect(HWindow,&ps.rcPaint);

            if (ElementList){
                __notify=NotifyWnd;
                ElementList->Paint(GetX_ofs(),Y_ofs,hdc,ps.rcPaint);
            }
            EndPaint(HWindow,&ps);
        }return 0;
    }
    return TWindow::WndProc(message,wParam,lParam);
};
void VTreeWindow::EvRButtonDown(WORD,int xPos,int yPos){
    TreeItem* te;
    if (ElementList){
        if ((te = ElementList->ScanActive(GetX_ofs(),Y_ofs,xPos,yPos)) != 0)
        {
            ChangeActiveItem(te);
            lastmousepos.x=xPos;
            lastmousepos.y=yPos;
            ClientToScreen(HWindow,&lastmousepos);
            ::SendMessage(NotifyWnd,TVN_ITEMMENU,(WPARAM)HWindow,te->Data);
            return;
        }
    }
};
void VTreeWindow::EvLButtonDblClk(WORD modKeys,int xPos,int yPos){
    EvLButtonDown(modKeys,xPos,yPos);
    state=0;
    if (!ElementList) return;
    if (ActiveItem){vn_Edit();}
};

void VTreeWindow::EvLButtonDown(WORD,int xPos,int yPos)

{
    TreeItem* te;

    if (ElementList)
    {
        if ((te = ElementList->ScanActive(GetX_ofs(),Y_ofs,xPos,yPos)) != 0)
        {
            ChangeActiveItem(te);
            if(CanDrag)	state=STATE_MOUSEDOWN;
            return;
        }

        if ((te = ElementList->Scan(GetX_ofs(),Y_ofs,xPos,yPos)) != 0)
        {
            switch (te->State)
            {
                case 0 : break;
                case 1 :
                {
                    DestroyChild(te);
                    break;
                }
                case 2 :
                {
                    ::SendMessage(NotifyWnd,TVN_GETITEMINFO,(WPARAM)HWindow,te->Data);
                    break;
                }
            }
        }
    }

}

void VTreeWindow::EvLButtonUp(WORD /*modKeys*/,int xPos,int yPos){
    if (state==STATE_DRAG){
        ReleaseCapture();
        SetCursor(0,IDC_ARROW);
        POINT point;point.x=xPos;point.y=yPos;
        ClientToScreen(HWindow,&point);
        HWND drop=WindowFromPoint(point);
        if (drop!=HWindow){
            DRAGDROPSTRUCT data;
            data.data    =ActiveItem->Data;
            data.datatype=ActiveItem->datatype;
            data.CursorPos=point;
            ScreenToClient(drop,&(data.CursorPos));
            SendMessage(drop,TW_DROP,(WPARAM)HWindow,(LPARAM)&data);
        }
    }
    state=0;
};

void VTreeWindow::EvMouseMove(WORD /*modKeys*/,int xPos,int yPos){
    if (state==STATE_MOUSEDOWN){
        state=STATE_DRAG;
        SetCapture(HWindow);
        // DRAGDROPSTRUCT data;
        // data.data    =ActiveItem->Data;
        // data.datatype=ActiveItem->Datatype;

        // if (!::SendMessage(NotifyWnd,TW_BEGINDRAGDROP,(WPARAM)HWindow,(LPARAM)&data))return ;
    }
    if (state==STATE_DRAG){
        POINT point;point.x=xPos;
        point.y=yPos;
        ClientToScreen(HWindow,&point);
        HWND drop=WindowFromPoint(point);
        if (drop==HWindow) SetCursor(HResource,DROP_CURSOR);else{
            DRAGDROPSTRUCT data;
            data.data    =ActiveItem->Data;
            data.datatype=ActiveItem->datatype;
            data.CursorPos.x=xPos;
            data.CursorPos.y=yPos;
            ScreenToClient(drop,&(data.CursorPos));
#ifdef WIN32
            if (((HINSTANCE)(GetWindowLong(drop,GWL_HINSTANCE))==hInstance)
    #else
            if (((HINSTANCE)(GetWindowWord(drop,GWW_HINSTANCE))==hInstance)
    #endif
                &&  SendMessage(drop,TW_ACCEPTDROP,(WPARAM)HWindow,(LPARAM)&data))
                SetCursor(HResource,DROP_CURSOR);else
                SetCursor(HResource,NODROP_CURSOR);
        }}
};

void VTreeWindow::ChangeActiveItem(int offset)
{
    if (!ElementList) return;

    int num = 0;
    int max = ElementList->GetTreeSize();

    if (!ActiveItem)
    {
        if (offset >= 0) num = 0;
        else num = max-1;
    }
    else
    {
        num = ActiveItem->GetNumber();
        num += offset;
        if (num < 0) num = 0;
        if (num >= max) num = max-1;
    }

    ChangeActiveItem(ElementList->GetItem(num));
}
void VTreeWindow::vn_Edit(){
    ::SendMessage(NotifyWnd,TVN_DBLCLK,(WPARAM)HWindow,ActiveItem->Data);
};
void VTreeWindow::vn_Insert(){
    if (ActiveItem)
        ::SendMessage(NotifyWnd,TVN_INSERTITEM,(WPARAM)HWindow,ActiveItem->Data);
};
void VTreeWindow::vn_Delete(){
    ::SendMessage(NotifyWnd,TVN_DELETEITEM,(WPARAM)HWindow,ActiveItem->Data);
};

void VTreeWindow::ChangeActiveItem(TreeItem* item)
{
    if (!item) return;
    if (item == ActiveItem) return;
    HDC dc = GetDC(HWindow);
    SetTextColor(dc,0);
    if (ActiveItem)
    {
        ActiveItem->ActiveOff();
        ActiveItem->PaintText(GetX_ofs(),Y_ofs,dc);
    }

    ActiveItem = item;

    int num = ActiveItem->GetNumber();
    if (num < MinViewNumber())
    {
        vscroll->SetPos(num);
    }
    else
    {
        if (num+1 >= MaxViewNumber()) vscroll->SetPos(num - ( vscroll->GetPage() -1));
    }
    item->ActiveOn();
    // ElementList->Paint(X_ofs,Y_ofs,dc,item->GetNumber(),item->GetNumber());
    item->PaintText(GetX_ofs(),Y_ofs,dc);
    ReleaseDC(HWindow,dc);

    ::SendMessage(NotifyWnd,TVN_SETACTIVEITEM,(WPARAM)HWindow,ActiveItem->Data);
}

