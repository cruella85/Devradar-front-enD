const std = @import("../../std.zig");
const builtin = @import("builtin");
const assert = std.debug.assert;

const windows = std.os.windows;
const GetLastError = windows.kernel32.GetLastError;
const SetLastError = windows.kernel32.SetLastError;
const unexpectedError = windows.unexpectedError;
const HWND = windows.HWND;
const UINT = windows.UINT;
const HDC = windows.HDC;
const LONG = windows.LONG;
const LONG_PTR = windows.LONG_PTR;
const WINAPI = windows.WINAPI;
const RECT = windows.RECT;
const DWORD = windows.DWORD;
const BOOL = windows.BOOL;
const TRUE = windows.TRUE;
const HMENU = windows.HMENU;
const HINSTANCE = windows.HINSTANCE;
const LPVOID = windows.LPVOID;
const ATOM = windows.ATOM;
const WPARAM = windows.WPARAM;
const LRESULT = windows.LRESULT;
const HICON = windows.HICON;
const LPARAM = windows.LPARAM;
const POINT = windows.POINT;
const HCURSOR = windows.HCURSOR;
const HBRUSH = windows.HBRUSH;

fn selectSymbol(comptime function_static: anytype, function_dynamic: *const @TypeOf(function_static), comptime os: std.Target.Os.WindowsVersion) *const @TypeOf(function_static) {
    comptime {
        const sym_ok = builtin.os.isAtLeast(.windows, os);
        if (sym_ok == true) return function_static;
        if (sym_ok == null) return function_dynamic;
        if (sym_ok == false) @compileError("Target OS range does not support function, at least " ++ @tagName(os) ++ " is required");
    }
}

// === Messages ===

pub const WNDPROC = *const fn (hwnd: HWND, uMsg: UINT, wParam: WPARAM, lParam: LPARAM) callconv(WINAPI) LRESULT;

pub const MSG = extern struct {
    hWnd: ?HWND,
    message: UINT,
    wParam: WPARAM,
    lParam: LPARAM,
    time: DWORD,
    pt: POINT,
    lPrivate: DWORD,
};

// Compiled by the WINE team @ https://wiki.winehq.org/List_Of_Windows_Messages
pub const WM_NULL = 0x0000;
pub const WM_CREATE = 0x0001;
pub const WM_DESTROY = 0x0002;
pub const WM_MOVE = 0x0003;
pub const WM_SIZE = 0x0005;
pub const WM_ACTIVATE = 0x0006;
pub const WM_SETFOCUS = 0x0007;
pub const WM_KILLFOCUS = 0x0008;
pub const WM_ENABLE = 0x000A;
pub const WM_SETREDRAW = 0x000B;
pub const WM_SETTEXT = 0x000C;
pub const WM_GETTEXT = 0x000D;
pub const WM_GETTEXTLENGTH = 0x000E;
pub const WM_PAINT = 0x000F;
pub const WM_CLOSE = 0x0010;
pub const WM_QUERYENDSESSION = 0x0011;
pub const WM_QUIT = 0x0012;
pub const WM_QUERYOPEN = 0x0013;
pub const WM_ERASEBKGND = 0x0014;
pub const WM_SYSCOLORCHANGE = 0x0015;
pub const WM_ENDSESSION = 0x0016;
pub const WM_SHOWWINDOW = 0x0018;
pub const WM_CTLCOLOR = 0x0019;
pub const WM_WININICHANGE = 0x001A;
pub const WM_DEVMODECHANGE = 0x001B;
pub const WM_ACTIVATEAPP = 0x001C;
pub const WM_FONTCHANGE = 0x001D;
pub const WM_TIMECHANGE = 0x001E;
pub const WM_CANCELMODE = 0x001F;
pub const WM_SETCURSOR = 0x0020;
pub const WM_MOUSEACTIVATE = 0x0021;
pub const WM_CHILDACTIVATE = 0x0022;
pub const WM_QUEUESYNC = 0x0023;
pub const WM_GETMINMAXINFO = 0x0024;
pub const WM_PAINTICON = 0x0026;
pub const WM_ICONERASEBKGND = 0x0027;
pub const WM_NEXTDLGCTL = 0x0028;
pub const WM_SPOOLERSTATUS = 0x002A;
pub const WM_DRAWITEM = 0x002B;
pub const WM_MEASUREITEM = 0x002C;
pub const WM_DELETEITEM = 0x002D;
pub const WM_VKEYTOITEM = 0x002E;
pub const WM_CHARTOITEM = 0x002F;
pub const WM_SETFONT = 0x0030;
pub const WM_GETFONT = 0x0031;
pub const WM_SETHOTKEY = 0x0032;
pub const WM_GETHOTKEY = 0x0033;
pub const WM_QUERYDRAGICON = 0x0037;
pub const WM_COMPAREITEM = 0x0039;
pub const WM_GETOBJECT = 0x003D;
pub const WM_COMPACTING = 0x0041;
pub const WM_COMMNOTIFY = 0x0044;
pub const WM_WINDOWPOSCHANGING = 0x0046;
pub const WM_WINDOWPOSCHANGED = 0x0047;
pub const WM_POWER = 0x0048;
pub const WM_COPYGLOBALDATA = 0x0049;
pub const WM_COPYDATA = 0x004A;
pub const WM_CANCELJOURNAL = 0x004B;
pub const WM_NOTIFY = 0x004E;
pub const WM_INPUTLANGCHANGEREQUEST = 0x0050;
pub const WM_INPUTLANGCHANGE = 0x0051;
pub const WM_TCARD = 0x0052;
pub const WM_HELP = 0x0053;
pub const WM_USERCHANGED = 0x0054;
pub const WM_NOTIFYFORMAT = 0x0055;
pub const WM_CONTEXTMENU = 0x007B;
pub const WM_STYLECHANGING = 0x007C;
pub const WM_STYLECHANGED = 0x007D;
pub const WM_DISPLAYCHANGE = 0x007E;
pub const WM_GETICON = 0x007F;
pub const WM_SETICON = 0x0080;
pub const WM_NCCREATE = 0x0081;
pub const WM_NCDESTROY = 0x0082;
pub const WM_NCCALCSIZE = 0x0083;
pub const WM_NCHITTEST = 0x0084;
pub const WM_NCPAINT = 0x0085;
pub const WM_NCACTIVATE = 0x0086;
pub const WM_GETDLGCODE = 0x0087;
pub const WM_SYNCPAINT = 0x0088;
pub const WM_NCMOUSEMOVE = 0x00A0;
pub const WM_NCLBUTTONDOWN = 0x00A1;
pub const WM_NCLBUTTONUP = 0x00A2;
pub const WM_NCLBUTTONDBLCLK = 0x00A3;
pub const WM_NCRBUTTONDOWN = 0x00A4;
pub const WM_NCRBUTTONUP = 0x00A5;
pub const WM_NCRBUTTONDBLCLK = 0x00A6;
pub const WM_NCMBUTTONDOWN = 0x00A7;
pub const WM_NCMBUTTONUP = 0x00A8;
pub const WM_NCMBUTTONDBLCLK = 0x00A9;
pub const WM_NCXBUTTONDOWN = 0x00AB;
pub const WM_NCXBUTTONUP = 0x00AC;
pub const WM_NCXBUTTONDBLCLK = 0x00AD;
pub const EM_GETSEL = 0x00B0;
pub const EM_SETSEL = 0x00B1;
pub const EM_GETRECT = 0x00B2;
pub const EM_SETRECT = 0x00B3;
pub const EM_SETRECTNP = 0x00B4;
pub const EM_SCROLL = 0x00B5;
pub const EM_LINESCROLL = 0x00B6;
pub const EM_SCROLLCARET = 0x00B7;
pub const EM_GETMODIFY = 0x00B8;
pub const EM_SETMODIFY = 0x00B9;
pub const EM_GETLINECOUNT = 0x00BA;
pub const EM_LINEINDEX = 0x00BB;
pub const EM_SETHANDLE = 0x00BC;
pub const EM_GETHANDLE = 0x00BD;
pub const EM_GETTHUMB = 0x00BE;
pub const EM_LINELENGTH = 0x00C1;
pub const EM_REPLACESEL = 0x00C2;
pub const EM_SETFONT = 0x00C3;
pub const EM_GETLINE = 0x00C4;
pub const EM_LIMITTEXT = 0x00C5;
pub const EM_SETLIMITTEXT = 0x00C5;
pub const EM_CANUNDO = 0x00C6;
pub const EM_UNDO = 0x00C7;
pub const EM_FMTLINES = 0x00C8;
pub const EM_LINEFROMCHAR = 0x00C9;
pub const EM_SETWORDBREAK = 0x00CA;
pub const EM_SETTABSTOPS = 0x00CB;
pub const EM_SETPASSWORDCHA