https://stackoverflow.com/questions/210504/enumerate-windows-like-alt-tab-does

To detect UWP apps check 'ApplicationFrameWindow' window classname then evaluate it with DwmGetWindowAttribute(LHWindow, DWMWA_CLOAKED, @cloaked, sizeof(Cardinal)) if cloacked (integer) returns 0 it means the UWP application is not hidden

DWMWA_CLOAKED,                      // [get] Gets the cloaked state of the window

// Start at the root owner
HWND hwndWalk = GetAncestor(hwnd, GA_ROOTOWNER);

BOOL IsAltTabWindow(HWND hwnd)
{
 // Start at the root owner
 HWND hwndWalk = GetAncestor(hwnd, GA_ROOTOWNER);

 // See if we are the last active visible popup
 HWND hwndTry;
 while ((hwndTry = GetLastActivePopup(hwndWalk)) != hwndTry) {
  if (IsWindowVisible(hwndTry)) break;
  hwndWalk = hwndTry;
 }
 return hwndWalk == hwnd;
}



 WindowInformation wi = new WindowInformation(window);

            if (wi.className == "Shell_TrayWnd" ||                          //Windows taskbar
                wi.className == "DV2ControlHost" ||                         //Windows startmenu, if open
                (wi.className == "Button" && wi.windowText == "Start") ||   //Windows startmenu-button.
                wi.className == "MsgrIMEWindowClass" ||                     //Live messenger's notifybox i think
                wi.className == "SysShadow" ||                              //Live messenger's shadow-hack
                wi.className.StartsWith("WMP9MediaBarFlyout"))              //WMP's "now playing" taskbar-toolbar
                return false;

            return true;
        }

private static IntPtr GetLastVisibleActivePopUpOfWindow(IntPtr window)
    {
        IntPtr lastPopUp = Win32.GetLastActivePopup(window);
        if (Win32.IsWindowVisible(lastPopUp))
            return lastPopUp;
        else if (lastPopUp == window)
            return IntPtr.Zero;
        else
            return GetLastVisibleActivePopUpOfWindow(lastPopUp);
    }

AltTab//只会显示exe的窗口,子窗口不显示

GetLastActivePopup
IsWindowVisible
GetAncestor
DwmGetWindowAttribute