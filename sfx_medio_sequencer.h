#pragma once
#include "sfx_media.h"
namespace sfx::media
{
    template
    <class Player>
    struct sequencer:
    widget<sequencer<Player>>
    {
        using widget<sequencer<Player>>::skin;
        using widget<sequencer<Player>>::coord;
        using widget<sequencer<Player>>::notify;

        widgetarium<Player> players;
        property<byte> volume = 255;
        property<bool> mute = false;
        property<time> timer;
        time instantly = 50ms;
        time smoothly = 500ms;
        time swiftly  = 100ms;
        bool playing  = false;
        bool playall  = false;
        bool repeat   = false;
        int  current  = 0;
        int  clicked  = 0;
        str  error;

        void play ()
        {
            if (players.empty()) return;
            if (players[current].status == state::finished)
                players[current].play();

            playall = false;
            playing = true;
            timer.go(
            time::infinity,
            time::infinity);
        }
        void stop ()
        {
            if (
            players.empty()) return;
            players[current].stop();
            playing = false;
            timer.go(
            time{},
            time{});
        }
        void Play ()
        {
            play();
            playall = true;
        }
        void Stop ()
        {
            stop();
            if (current == 0) return;
            if (players.empty()) return;
            players[current].hide(smoothly); current = 0;
            players[current].show(smoothly);
        }

        void currprev () { int n = players.size(); current = (current-1+n) % n; }
        void currnext () { int n = players.size(); current = (current+1  ) % n; }

        void prev ()
        {
            if (
            players.empty()) return;
            players[current].stop();
            players[current].hide(instantly); currprev();
            players[current].show(instantly); if (playing)
            players[current].play();
        }
        void next ()
        {
            if (
            players.empty()) return;
            players[current].stop();
            players[current].hide(instantly); currnext();
            players[current].show(instantly); if (playing)
            players[current].play();
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                for (auto&
                player: players)
                player .coord = coord.now.local();
                players.coord = coord.now.local();
            }

            if (what == &players)
            {
                clicked = players.
                notifier->clicked;
                notify();
            }

            if (what == &timer and timer.to != time{})
            {
                if (not players.empty())
                switch(players[current].status) {
                case state::failed:
                    if (
                    error != "")
                    error += "<br>";
                    error +=
                    players[current].error;
                    players[current].status = state::finished;
                    players[current].show(swiftly);
                    break;
                case state::ready:
                case state::paused:
                    players[current].play();
                    players[current].show(swiftly);
                    break;
                case state::finished:
                    if (players.size() < 2
                        or not playall) {
                        stop(); break; }
                    players[current].stop();
                    players[current].hide(smoothly); currnext();
                    players[current].show(smoothly); if (repeat or current > 0)
                    players[current].play();
                    else Stop();
                    break;
                default:
                    break;
                }
            }

            if (what == &volume)
            {
                for (auto&
                player: players)
                player.volume =
                volume;
            }
            if (what == &mute)
            {
                for (auto&
                player: players)
                player.mute =
                mute;
            }
        }
    };
}
