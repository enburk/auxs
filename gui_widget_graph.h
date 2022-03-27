#pragma once
#include "gui_widget_geometry.h"
#include "gui_widget_text_editline.h"
namespace gui
{
    struct node:
    widget<node>
    {
        oval inner;
        oval outer;
        oval outex;
        oval outey;
        text::view text;

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                xywh r = coord.now.local();
                auto rx = r.w/2.0;
                auto ry = r.h/2.0;
                inner.x = rx; inner.rx = rx-0.5;
                inner.y = ry; inner.ry = ry-0.5;
                outer.x = rx; outer.rx = rx; outer.rx2 = rx;
                outer.y = ry; outer.ry = ry; outer.ry2 = ry;
                outex.x = rx; outex.rx = rx-1; outex.rx2 = rx-1;
                outex.y = ry; outex.ry = ry-1; outex.ry2 = ry-1;
                outey.x = rx; outey.rx = rx-2; outey.rx2 = rx-2;
                outey.y = ry; outey.ry = ry-2; outey.ry2 = ry-2;
                text.coord = r;
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

    struct BST:
    widget<BST>
    {
        std::deque<std::optional<line>> edges;
        std::deque<std::optional<node>> nodes;

        void add (int n, int i = 0)
        {
            if (i >= nodes.size()) {
                nodes.resize(i+1);
                edges.resize(i+1);
            }
            auto& node = nodes[i];
            auto& edge = edges[i];

            if (not node)
            {
                edge.emplace(); children += &edge.value();
                node.emplace(); children += &node.value();
                node->text.text = std::to_string(n);
                node->text.color = rgba::navy;
                node->inner.color = rgba::white;
                node->outer.color = rgba::red;
                node->outex.color = rgba::red;
                node->outey.color = rgba::red;
                edge->color = rgba::white;
            }
            else
            {
                int value = std::stoi(str(node->text.text));

                if (n < value) add(n, i*2+1); else
                if (n > value) add(n, i*2+2); else
                {}
            }
            on_change(&coord);
        }

        void place (int i, xyxy r, int align)
        {
            if (i >= nodes.size()) return;
            auto& node = nodes[i];
            auto& edge = edges[i];
            if (not node) return;

            int W = r.x2 - r.x1; if (W <= 0) return;
            int H = r.y2 - r.y1; if (H <= 0) return;
            int d = gui::metrics::text::height*12/7;
            int c = (r.x1 + r.x2)/2;

            node->coord = xywh(c-d/2, r.y1+d/2, d, d);
            edge->y1 = r.y1 - d/2;
            edge->y2 = r.y1 + d;
            edge->x2 = c;
            edge->x1 =
            align == pix::left ? r.x2:
            align == pix::right? r.x1: c;

            place (i*2+1, xyxy(r.x1, r.y1+d+d/2, c, r.y2), pix::left);
            place (i*2+2, xyxy(c, r.y1+d+d/2, r.x2, r.y2), pix::right);
        }

        void on_change (void* what) override
        {
            if (what == &coord and
                coord.was.size !=
                coord.now.size)
            {
                place(0, coord.now.local(),
                    pix::center);
            }
        }
    };
}
