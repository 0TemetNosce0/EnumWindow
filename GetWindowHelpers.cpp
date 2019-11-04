#include "GetWindowHelpers.h"
#include <stdbool.h>
#include <Windows.h>
#include <Psapi.h>
#include <QDebug>

#define LOWER_HALFBYTE(x) ((x) & 0xF)
#define UPPER_HALFBYTE(x) (((x) >> 4) & 0xF)




GetWindowHelpers::GetWindowHelpers()
{

}

GetWindowHelpers::~GetWindowHelpers()
{

}

void GetWindowHelpers::deobfuscate_str(char *str, uint64_t val)
{
    uint8_t *dec_val = (uint8_t*)&val;
    int i = 0;

    while (*str != 0) {
        int pos = i / 2;
        bool bottom = (i % 2) == 0;
        uint8_t *ch = (uint8_t*)str;
        uint8_t xor = bottom ? LOWER_HALFBYTE(dec_val[pos]) : UPPER_HALFBYTE(dec_val[pos]);

        *ch ^= xor;

        if (++i == sizeof(uint64_t) * 2)
            i = 0;

        str++;
    }

}
HMODULE GetWindowHelpers::kernel32(void)
{
    static HMODULE kernel32_handle = NULL;
    if (!kernel32_handle)
        kernel32_handle = GetModuleHandleA("kernel32");
    return kernel32_handle;
}

HANDLE GetWindowHelpers::open_process(DWORD desired_access, bool inherit_handle,
                                      DWORD process_id)
{
    static HANDLE (WINAPI *open_process_proc)(DWORD, BOOL, DWORD) = NULL;
    //    using properties_delete_t = decltype(&obs_properties_destroy);
    using open_process_proc_fun = decltype(open_process_proc);
    if (!open_process_proc)
        open_process_proc = reinterpret_cast<open_process_proc_fun>(get_obfuscated_func(kernel32(),"B}caZyah`~q", 0x2D5BEBAF6DDULL));

    return open_process_proc(desired_access, inherit_handle, process_id);
}

//根据窗口句柄获取窗口exe,输出：name
bool GetWindowHelpers:: get_window_exe(QString &name, HWND window)
{
    wchar_t     wname[MAX_PATH];
    bool        success = false;
    HANDLE      process = NULL;
    int index = -1;
    DWORD       id;
    GetWindowThreadProcessId(window, &id);
    if (id == GetCurrentProcessId())
        return false;
    process = open_process(PROCESS_QUERY_LIMITED_INFORMATION, false, id);
    if (!process)
        goto fail;
    if (!GetProcessImageFileNameW(process, wname, MAX_PATH))//检索指定进程的可执行文件的名称
        goto fail;
    name = QString::fromWCharArray(wname);
    index = name.lastIndexOf("\\");
    if (index<0)
        goto fail;
    name =  name.mid(index+1);
    success = true;

fail:
    if (!success)
        name = "unknown";
    CloseHandle(process);
    return true;
}

/***************************
 * brief:获取窗口标题
 * input:hwnd
 * output:name
 * return:
 **************************/
void GetWindowHelpers::get_window_title(QString &name, HWND hwnd)
{
    wchar_t *temp;
    int len;

    len = GetWindowTextLengthW(hwnd);//标题长度
    if (!len)
        return;

    temp = (wchar_t *)malloc(sizeof(wchar_t) * (len+1));
    if (GetWindowTextW(hwnd, temp, len+1))//获取标题
        //        sprintf(name, "%ws", temp);
        name = QString::fromWCharArray(temp);
    free(temp);
}

void GetWindowHelpers::get_window_class(QString &className, HWND hwnd)
{
    wchar_t temp[256];

    temp[0] = 0;
    if (GetClassNameW(hwnd, temp, sizeof(temp) / sizeof(wchar_t)))
        //        sprintf(className, "%ws", temp);
        className =  QString::fromWCharArray(temp);

}
/***************************
 * brief:获取所有窗口：大小，窗口名
 * input:mode
 * output:list
 * return:
 **************************/
void GetWindowHelpers::FillWindowList(QList<WindowInfo> &p, GetWindowHelpers::window_search_mode mode)
{
    HWND parent;
    bool use_findwindowex = false;

    HWND window = first_window(mode, &parent, &use_findwindowex);
    QString exe;
    while (window) {
        if (!get_window_exe(exe, window)||is_microsoft_internal_window_exe(exe)) {

        }else{
            RECT rctB = {0};
            RECT rectIsZoomed = {0};
            if(GetWindowRect(window,&rctB)){
                if (::IsZoomed(window)) {//最大化窗口的处理
                    //ClientToScreen();
                    ::GetClientRect(window,&rectIsZoomed);
                    rctB.left  = rectIsZoomed.left;
                     rctB.right  = rectIsZoomed.right;
                      rctB.top  = rectIsZoomed.top;
                }

                QRect rect(rctB.left,rctB.top,rctB.right-rctB.left,rctB.bottom-rctB.top);
                WindowInfo tmp;
                tmp.windowRect = rect;

                QString className ;
                QString title ;
                QString encoded;

                get_window_title(title, window);
                get_window_class(className, window);
                encode_dstr(title);
                encode_dstr(className);
                encode_dstr(exe);

                encoded.append(title);
                encoded.append(":");
                encoded.append(className);
                encoded.append(":");
                encoded.append(exe);

                tmp.windowName = encoded;
                p.append(tmp);
            } else {
                printf("ScreenToClient hwnd=%p -> fail(%ld)\n", window, GetLastError());
            }

        }
        window = next_window(window, mode, &parent, use_findwindowex);//下一个窗口
    }
}
#include <Dwmapi.h>

void GetWindowHelpers::FillWindowList(QList<QRect> &p, GetWindowHelpers::window_search_mode mode)
{
    HWND parent;
    bool use_findwindowex = false;

    HWND window = first_window(mode, &parent, &use_findwindowex);
    QString exe;
    while (window) {
        if (!get_window_exe(exe, window)||is_microsoft_internal_window_exe(exe)) {

        }else{
            RECT rctB = {0};
            RECT rectIsZoomed = {0};
            if(GetWindowRect(window,&rctB)){
                if (::IsZoomed(window)) {//最大化窗口的处理
                    //ClientToScreen();
                    ::GetClientRect(window,&rectIsZoomed);
                    rctB.left  = rectIsZoomed.left;
                     rctB.right  = rectIsZoomed.right;
                      rctB.top  = rectIsZoomed.top;
                }
                  HRESULT hr = S_OK;
                  BOOL enabled = true;

                hr = DwmGetWindowAttribute(window, DWMWA_NCRENDERING_ENABLED, &enabled, sizeof(BOOL));

                if (SUCCEEDED(hr))
                {
                    // Use the retrieved information.
                }
                QRect rect(rctB.left,rctB.top,rctB.right-rctB.left,rctB.bottom-rctB.top);
                p.append(rect);
            } else {
                printf("ScreenToClient hwnd=%p -> fail(%ld)\n", window, GetLastError());
            }

        }
        window = next_window(window, mode, &parent, use_findwindowex);//下一个窗口
    }
}
HWND GetWindowHelpers::first_window(enum window_search_mode mode, HWND *parent,
                                    bool *use_findwindowex)
{
    HWND window = FindWindowEx(GetDesktopWindow(), NULL, NULL, NULL);//GetDesktopWindow()的第一个子窗口

    if (!window) {//没有
        *use_findwindowex = false;
        window = GetWindow(GetDesktopWindow(), GW_CHILD);//该函数返回与指定窗口有特定关系（如Z序或所有者）的窗口句柄
    } else {
        *use_findwindowex = true;
    }

    *parent = NULL;

    if (!check_window_valid(window, mode)) {//不是有效窗口
        window = next_window(window, mode, parent, *use_findwindowex);

        if (!window && *use_findwindowex) {
            *use_findwindowex = false;

            window = GetWindow(GetDesktopWindow(), GW_CHILD);
            if (!check_window_valid(window, mode))
                window = next_window(window, mode, parent,
                                     *use_findwindowex);
        }
    }

    if (is_uwp_window(window)) {//uwp
        HWND child = get_uwp_actual_window(window);
        if (child) {
            *parent = window;
            return child;
        }
    }

    return window;
}
void *GetWindowHelpers::get_obfuscated_func(HMODULE module, const char *str, uint64_t val)
{
    char new_name[128];
    strcpy(new_name, str);
    deobfuscate_str(new_name, val);
    return GetProcAddress(module, new_name);
}

//窗口是否有效
bool GetWindowHelpers::check_window_valid(HWND window, enum window_search_mode mode)
{
    DWORD styles, ex_styles;
    RECT  rect;

    if (!IsWindowVisible(window) ||
            (mode == EXCLUDE_MINIMIZED && IsIconic(window)))
        return false;

    GetClientRect(window, &rect);
    styles    = (DWORD)GetWindowLongPtr(window, GWL_STYLE);
    ex_styles = (DWORD)GetWindowLongPtr(window, GWL_EXSTYLE);

    if (ex_styles & WS_EX_TOOLWINDOW)
        return false;
    if (styles & WS_CHILD)
        return false;
    if (mode == EXCLUDE_MINIMIZED && (rect.bottom == 0 || rect.right == 0))
        return false;

    return true;
}
//是否是uwp应用
bool GetWindowHelpers::is_uwp_window(HWND hwnd)
{
    wchar_t name[256];

    name[0] = 0;
    if (!GetClassNameW(hwnd, name, sizeof(name) / sizeof(wchar_t)))
        return false;

    return wcscmp(name, L"ApplicationFrameWindow") == 0;//ApplicationFrameWindow  uwp应用
}

//获取uwp实际的窗口
HWND GetWindowHelpers::get_uwp_actual_window(HWND parent)
{
    DWORD parent_id = 0;
    HWND child;

    GetWindowThreadProcessId(parent, &parent_id);
    child = FindWindowEx(parent, NULL, NULL, NULL);

    while (child) {
        DWORD child_id = 0;
        GetWindowThreadProcessId(child, &child_id);

        if (child_id != parent_id)
            return child;

        child = FindWindowEx(parent, child, NULL, NULL);
    }

    return NULL;
}

//搜索下一个窗口
HWND GetWindowHelpers::next_window(HWND window, enum window_search_mode mode,
                                   HWND *parent, bool use_findwindowex)
{
    if (*parent) {
        window = *parent;
        *parent = NULL;
    }

    while (true) {
        if (use_findwindowex)
            window = FindWindowEx(GetDesktopWindow(), window, NULL,
                                  NULL);
        else
            window = GetNextWindow(window, GW_HWNDNEXT);//下一个窗口

        if (!window || check_window_valid(window, mode))
            break;
    }

    if (is_uwp_window(window)) {
        HWND child = get_uwp_actual_window(window);
        if (child) {
            *parent = window;
            return child;
        }
    }

    return window;
}


//是否是win内部程序
bool GetWindowHelpers::is_microsoft_internal_window_exe(const QString exe)
{
    char cur_exe[MAX_PATH];

    if (exe.isEmpty())
        return false;

    for (const char **vals = internal_microsoft_exes; *vals; vals++) {
        strcpy(cur_exe, *vals);
        strcat(cur_exe, ".exe");
        //        if (strcmpi(cur_exe, exe) == 0)
        //            return true;
        //        QString s = QString::fromLocal8Bit(cur_exe);
        if(exe.compare(QString::fromLocal8Bit(cur_exe),Qt::CaseInsensitive)==0){
            return true;
        }
    }

    return false;
}

