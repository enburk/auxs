#pragma once
#include "gui_widget_geometry.h"
#include "gui_widget_text_editline.h"
namespace sfx
{
    using gui::widget;
    using gui::widgetarium;
    using gui::property;
    using gui::unary_property;
    using gui::binary_property;
    using text = gui::text::view;
    using gui::line;
    using gui::oval;
    using gui::time;
    using namespace std::literals::chrono_literals;

    struct node:
    widget<node>
    {
        oval inner;
        oval outer;
        oval outex;
        oval outey;
        text value;
        text label;
        property<double> x = 0.0;
        property<double> y = 0.0;
        property<double> r = 0.0;
        property<double> a = pi*3/2;

        node ()
        {
            value.color = rgba::black;
            label.color = rgba::green;
            inner.color = rgba::white;
            outer.color = rgba::red;
            outex.color = rgba::red;
            outey.color = rgba::red;

            value.alignment = xy{
            pix::center_of_mass,
            pix::center_of_mass};
        }
        void mimic (node const& n)
        {
            value.text  = n.value.text;
            label.text  = n.label.text;
            value.color = n.value.color;
            label.color = n.label.color;
            inner.color = n.inner.color;
            outer.color = n.outer.color;
            outex.color = n.outex.color;
            outey.color = n.outey.color;
            x = n.x;
            y = n.y;
            r = n.r;
        }
        void on_change (void* what) override
        {
            if (what == &x or
                what == &y or
                what == &r or
                what == &a)
            {
                int d = 2*int(ceil(r));
                int X = int(std::round(x.now));
                int Y = int(std::round(y.now));
                inner.x = d*3/2; inner.rx = r-0.5;
                inner.y = d*3/2; inner.ry = r-0.5;
                outer.x = d*3/2; outer.rx = r; outer.rx2 = r;
                outer.y = d*3/2; outer.ry = r; outer.ry2 = r;
                outex.x = d*3/2; outex.rx = r-1; outex.rx2 = r-1;
                outex.y = d*3/2; outex.ry = r-1; outex.ry2 = r-1;
                outey.x = d*3/2; outey.rx = r-2; outey.rx2 = r-2;
                outey.y = d*3/2; outey.ry = r-2; outey.ry2 = r-2;
                int l = str(value.text).size();
                auto font = value.font.now;
                font.size = l <= 2 ? (d*9+1)/10 : (d*7+1)/11;
                value.font = font;
                label.font = font;

                value.coord = xywh(d,d,d,d);
                auto p = aux::vector<2>{d,0}.rotated(-a);
                int xx = int(std::round(p.x));
                int yy = int(std::round(p.y));
                label.coord = xyxy(
                xx-d/2, yy-d/2,
                xx+d/2, yy+d/2) +
                xy{d*3/2, d*3/2};
                coord = xywh(
                    X-d*3/2,
                    Y-d*3/2,
                    3*d,
                    3*d);
            }
        }
    };

    struct queue:
    widget<queue>
    {
        widgetarium<node> nodes;
        property<int> lower = 0;
        property<int> upper = 99;
        binary_property<str> distribution;
        property<double> speed = 1.0;
        property<time> timer;
        deque<str> extra;

        void pop ()
        {
            for (int i=0; i<nodes.size()-1; i++)
            nodes(i).mimic(nodes(i+1));
            nodes.back().value.text = "";
        }
        void push (str s) { extra += s; }
        bool empty () { return nodes.empty(); }
        void clear () { nodes.clear(); extra.clear(); }

        void on_change (void* what) override
        {
            if (what == &coord
            or  what == &timer)
            {
                int n = 0;
                int x = 0;
                int d = coord.now.h;
                if (d == 0) return;

                if (timer.now == time{})
                    timer.go(time::infinity,
                             time::infinity);

                time ms {int(100/speed.now)};

                nodes.coord = coord.now.local();

                while (x + d < coord.now.w)
                {
                    auto& q = nodes(n++);
                    q.r = d/2;
                    q.y = d/2;
                    q.x.go(x+d/2, q.x==0 ? time{} : ms);
                    x += d;

                    if (str(q.value.text) != "")
                        continue;

                    if (n == 1
                    or (n >= 2 and
                    nodes(n-2).value.text != "" and
                    nodes(n-2).alpha == 255))
                    {
                        str x;
                        if (not extra.empty()) {
                            x = extra.front();
                            extra.pop_front(); }
                        else
                            x = std::to_string(
                            distribution.now == "normal"?
                            aux::normal(lower.now, upper.now):
                            aux::random(lower.now, upper.now));

                        q.value.text = x;
                        q.hide();
                        q.show(ms);
                    }
                    else q.hide();
                }
                while (
                nodes.size() > n) {
                extra.push_front(
                nodes.back().value.text);
                nodes.truncate(); }
            }

            if (what == &lower
            or  what == &upper
            or  what == &distribution)
            {
                clear();
            }
        }
    };

    struct graph:
    widget<graph>
    {
        widgetarium<node> nodes;
        widgetarium<line> edges;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                xywh r = coord.now.local();
                nodes.coord = r;
                edges.coord = r;
            }
        }
    };
}
