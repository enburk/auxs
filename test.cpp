#pragma once
#include "test.h"

struct App:
widget<App>
{
    Test test;

    void on_change (void* what) override
    {
        if (what == &coord and
            coord.was.size !=
            coord.now.size)
        {
            test.coord = coord.now.local();
        }
    }
};

sys::app<App> app("auxs");

#include "windows_fonts.h"
#include "windows_images.h"
#include "windows_system.h"
#include "windows_windows.h"


