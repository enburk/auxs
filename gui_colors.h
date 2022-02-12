#pragma once
#include <map>
#include <variant>
#include "pix_color.h"
#include "pix_text.h"
namespace gui
{
    using namespace data;
    using namespace pix;

    namespace metrics
    {
        namespace text { inline int height; };
        namespace line { inline int width;  };
    };

    struct schema
    {
        pix::font font;
        std::pair<RGBA,RGBA>
        ultralight, light, soft, normal, middle, heavy, dark,
        active, hovered, touched, focused, disabled,
        highlight, selection, error;
    };
    inline std::map<str, schema> skins;

    inline std::map<str, std::pair<RGBA, RGBA>[10]> palettes;

    inline array<str> palettes_names;

    const inline array<const char*> palettes_data = {
    // https://material.io/design/color/#tools-for-picking-colors
    //                 0        1        2        3        4        5        6        7        8        9
    "red",         "FFEBEE","FFCDD2","EF9A9A","E57373","EF5350","F44336","E53935","D32F2F","C62828","B71C1C",
                   "000000","000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF", 
    "pink",        "FCE4EC","F8BBD0","F48FB1","F06292","EC407A","E91E63","D81B60","C2185B","AD1457","880E4F",
                   "000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "purple",      "F3E5F5","E1BEE7","CE93D8","BA68C8","AB47BC","9C27B0","8E24AA","7B1FA2","6A1B9A","4A148C",
                   "000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "deep purple", "EDE7F6","D1C4E9","B39DDB","9575CD","7E57C2","673AB7","5E35B1","512DA8","4527A0","311B92",
                   "000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "indigo",      "E8EAF6","C5CAE9","9FA8DA","7986CB","5C6BC0","3F51B5","3949AB","303F9F","283593","1A237E",
                   "000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "blue",        "E3F2FD","BBDEFB","90CAF9","64B5F6","42A5F5","2196F3","1E88E5","1976D2","1565C0","0D47A1",
                   "000000","000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF", 
    "light blue",  "E1F5FE","B3E5FC","81D4FA","4FC3F7","29B6F6","03A9F4","039BE5","0288D1","0277BD","01579B",
                   "000000","000000","000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF", 
    "cyan",        "E0F7FA","B2EBF2","80DEEA","4DD0E1","26C6DA","00BCD4","00ACC1","0097A7","00838F","006064",
                   "000000","000000","000000","000000","000000","000000","000000","000000","000000","FFFFFF", 
    "teal",        "E0F2F1","B2DFDB","80CBC4","4DB6AC","26A69A","009688","00897B","00796B","00695C","004D40",
                   "000000","000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF", 
    "green",       "E8F5E9","C8E6C9","A5D6A7","81C784","66BB6A","4CAF50","43A047","388E3C","2E7D32","1B5E20",
                   "000000","000000","000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF", 
    "light green", "F1F8E9","DCEDC8","C5E1A5","AED581","9CCC65","8BC34A","7CB342","689F38","558B2F","33691E",
                   "000000","000000","000000","000000","000000","000000","000000","000000","000000","FFFFFF", 
    "lime",        "F9FBE7","F0F4C3","E6EE9C","DCE775","D4E157","CDDC39","C0CA33","AFB42B","9E9D24","827717",
                   "000000","000000","000000","000000","000000","000000","000000","000000","000000","000000", 
    "yellow",      "FFFDE7","FFF9C4","FFF59D","FFF176","FFEE58","FFEB3B","FDD835","FBC02D","F9A825","F57F17",
                   "000000","000000","000000","000000","000000","000000","000000","000000","000000","000000", 
    "amber",       "FFF8E1","FFECB3","FFE082","FFD54F","FFCA28","FFC107","FFB300","FFA000","FF8F00","FF6F00",
                   "000000","000000","000000","000000","000000","000000","000000","000000","000000","000000", 
    "orange",      "FFF3E0","FFE0B2","FFCC80","FFB74D","FFA726","FF9800","FB8C00","F57C00","EF6C00","E65100",
                   "000000","000000","000000","000000","000000","000000","000000","000000","000000","000000", 
    "deep orange", "FBE9E7","FFCCBC","FFAB91","FF8A65","FF7043","FF5722","F4511E","E64A19","D84315","BF360C",
                   "000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "brown",       "EFEBE9","D7CCC8","BCAAA4","A1887F","8D6E63","795548","6D4C41","5D4037","4E342E","3E2723",
                   "000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "gray",        "FAFAFA","F5F5F5","EEEEEE","E0E0E0","BDBDBD","9E9E9E","757575","616161","424242","212121", 
                   "000000","000000","000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    "blue gray",   "ECEFF1","CFD8DC","B0BEC5","90A4AE","78909C","607D8B","546E7A","455A64","37474F","263238",
                   "000000","000000","000000","000000","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF","FFFFFF", 
    };

    void init ()
    {
        metrics::text::height = pix::metrics(pix::font{}).height;
        metrics::line::width = max (1, metrics::text::height/16);

        str name;
        for (auto [index, s] : enumerate(palettes_data))
        {
            int i = index % 21;
            RGBA* color = nullptr;
            if (i == 0) name = s; else
            if (i < 11) color = &palettes[name][i- 1].first; else
            if (i < 21) color = &palettes[name][i-11].second;
            if (color) { *color = pix::ARGB(std::strtoul(s, nullptr, 16));
                color->a = 255; }
        }
        for (auto [index, s] : enumerate(palettes_data))
        {
            int i = index % 21;
            RGBA* color = nullptr;
            if (i == 0) name = s + str("+"); else
            if (i < 11) color = &palettes[name][9 - (i- 1)].first; else
            if (i < 21) color = &palettes[name][9 - (i-11)].second;
            if (color) { *color = pix::ARGB(std::strtoul(s, nullptr, 16));
                color->a = 255; }
        }
        for (auto [index, s] : enumerate(palettes_data))
        {
            int i = index % 21;
            if (i == 0) {
                palettes_names += s;
                palettes_names += s + str("+");
            }
        }

        // for (int i=0; i<10; i++)
        //     palettes[""][i] = std::pair{
        //         RGBA::random(), RGBA::random()};

        for (auto [name, palette] : palettes) {
            auto & skin = skins[name];

            skin.ultralight = palette[0];
            skin.light      = palette[1];
            skin.soft       = palette[2];
            skin.normal     = palette[3];
            skin.middle     = palette[4];
            skin.heavy      = palette[5];
            skin.dark       = palette[9];

            skin.hovered    = palette[6];
            skin.active     = palette[7];
            skin.touched    = palette[8];
            skin.focused    = std::pair{RGBA::amber, RGBA::black};
            skin.disabled   = std::pair{palette[3].first, palette[5].first};

            skin.highlight  = palette[3];
            skin.selection  = palette[4];
            skin.error      = std::pair{RGBA::error, RGBA::white};
        }

        skins[""] = skins["gray"];
    }
    void done () {}
}