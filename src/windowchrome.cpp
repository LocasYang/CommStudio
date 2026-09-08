#include "windowchrome.h"

#include <QWindow>

#ifdef Q_OS_WIN
#include <dwmapi.h>
#include <windows.h>
#endif

WindowChrome::WindowChrome(QObject *parent)
    : QObject(parent)
{
}

bool WindowChrome::apply(QObject *windowObject,
                         bool dark,
                         const QColor &captionColor,
                         const QColor &textColor,
                         const QColor &borderColor)
{
    auto *window = qobject_cast<QWindow *>(windowObject);
    if (!window)
        return false;

#ifdef Q_OS_WIN
    const HWND handle = reinterpret_cast<HWND>(window->winId());
    if (!handle)
        return false;

    // Values 20/19 cover the current and legacy Windows 10 dark-caption flags.
    constexpr DWORD useImmersiveDarkMode = 20;
    constexpr DWORD useImmersiveDarkModeLegacy = 19;
    constexpr DWORD borderColorAttribute = 34;
    constexpr DWORD captionColorAttribute = 35;
    constexpr DWORD textColorAttribute = 36;

    const BOOL darkValue = dark ? TRUE : FALSE;
    HRESULT result = DwmSetWindowAttribute(
        handle,
        static_cast<DWMWINDOWATTRIBUTE>(useImmersiveDarkMode),
        &darkValue,
        sizeof(darkValue));
    if (FAILED(result)) {
        result = DwmSetWindowAttribute(
            handle,
            static_cast<DWMWINDOWATTRIBUTE>(useImmersiveDarkModeLegacy),
            &darkValue,
            sizeof(darkValue));
    }

    const COLORREF caption = RGB(captionColor.red(), captionColor.green(), captionColor.blue());
    const COLORREF text = RGB(textColor.red(), textColor.green(), textColor.blue());
    const COLORREF border = RGB(borderColor.red(), borderColor.green(), borderColor.blue());

    DwmSetWindowAttribute(
        handle,
        static_cast<DWMWINDOWATTRIBUTE>(captionColorAttribute),
        &caption,
        sizeof(caption));
    DwmSetWindowAttribute(
        handle,
        static_cast<DWMWINDOWATTRIBUTE>(textColorAttribute),
        &text,
        sizeof(text));
    DwmSetWindowAttribute(
        handle,
        static_cast<DWMWINDOWATTRIBUTE>(borderColorAttribute),
        &border,
        sizeof(border));

    SetWindowPos(
        handle,
        nullptr,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    RedrawWindow(handle, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW);
    DwmFlush();
    return SUCCEEDED(result);
#else
    Q_UNUSED(dark)
    Q_UNUSED(captionColor)
    Q_UNUSED(textColor)
    Q_UNUSED(borderColor)
    return false;
#endif
}
