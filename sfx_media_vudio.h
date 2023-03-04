#pragma once
#include "sfx_media_audio.h"
#include "sfx_media_image.h"
#include "sfx_media_video.h"
namespace sfx::media::vudio
{
    struct player:
    widget<player>
    {
        std::atomic<state>
        status = state::finished;
        binary_property<xyxy> padding;
        property<bool> pause = false;
        xy resolution;
        time duration;
        time elapsed;
    };
}