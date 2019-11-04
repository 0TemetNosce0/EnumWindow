#pragma once
#include <stdint.h>
#include <Windows.h>
#include <Qlist>
#include <QStringList>

#include "windowinfo.h"
//TODO 窗口移除桌面问题。桌面选择鼠标在工具条不变问题
class GetWindowHelpers
{
public:
    enum window_priority {
        WINDOW_PRIORITY_CLASS,
        WINDOW_PRIORITY_TITLE,
        WINDOW_PRIORITY_EXE,
    };

    enum window_search_mode {
        INCLUDE_MINIMIZED,
        EXCLUDE_MINIMIZED
    };
public:
    explicit GetWindowHelpers();
    ~GetWindowHelpers();

    void FillWindowList(QList<WindowInfo> &p, window_search_mode mode);
    void FillWindowList(QList<QRect> &p, GetWindowHelpers::window_search_mode mode);
private:
    bool get_window_exe(QString &name, HWND window);
    void get_window_title(QString &name, HWND hwnd);
    void get_window_class(QString &className, HWND hwnd);
    /* this is a workaround to A/Vs going crazy whenever certain functions (such as
     * OpenProcess) are used */
    void *get_obfuscated_func(HMODULE module, const char *str, uint64_t val);
    void deobfuscate_str(char *str, uint64_t val);
    HMODULE kernel32(void);
    HANDLE open_process(DWORD desired_access, bool inherit_handle,
                        DWORD process_id);

    HWND first_window(window_search_mode mode, HWND *parent,
                      bool *use_findwindowex);

    bool check_window_valid(HWND window, enum window_search_mode mode);
    bool is_uwp_window(HWND hwnd);
    HWND get_uwp_actual_window(HWND parent);
    HWND next_window(HWND window, enum window_search_mode mode,
                     HWND *parent, bool use_findwindowex);
    bool is_microsoft_internal_window_exe(const QString exe);
//    void add_window(QList<HWND> &p, HWND hwnd);

    //窗口捕获，编码，冒号要换成#3A
    inline void encode_dstr(QString &str)
    {
        str.replace("#", "#22");
        str.replace(":", "#3A");
    }

    void build_window_strings(const char *str,
                              char **className,
                              char **title,
                              char **exe);

    /* not capturable or internal windows */
    //不捕获win内部窗口
    const char *internal_microsoft_exes[7] = {
        "applicationframehost",
        "shellexperiencehost",
        "winstore.app",
        "searchui",
        "SystemSettings",//win系统设置
        //    "explorer",//explorer.exe是Windows程序管理器或者文件资源管理器，它用于管理Windows图形壳，包括桌面和文件管理
        //    "Music.UI",//groove音乐
        "LockApp",//锁屏界面
        NULL
    };
};
