#include "metalUtil.h"

#include <QuartzCore/QuartzCore.h>
#include <AppKit/AppKit.h>

void setLayer(void* window, void* layer)
{
    NSWindow *nsWindow = reinterpret_cast<NSWindow *>(window);
    nsWindow.contentView.layer = reinterpret_cast<CAMetalLayer *>(layer);
    nsWindow.contentView.wantsLayer = YES;
}
