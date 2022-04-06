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
                auto font =
                text.font.now; font.size = (coord.now.size.y*7+1)/12;
                text.font = font;
                text.coord = r;

                pix::text::style style;
                style.color = text.color;
                style.font = font;
                pix::text::token t(text.text, style);
                int dx = t.lpadding;
                int dy = t.ascent - t.ascent_;
                text.shift = xy(-dx/2,0);//-dy/2);
                text.alignment = xy{
                    pix::center,
                    pix::center};
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

        void clear ()
        {
            nodes.clear();
            edges.clear();
        }

        void place (int i, xyxy r)
        {
            if (i >= nodes.size()) return;
            auto& node = nodes[i];
            auto& edge = edges[i];
            if (not node) return;

            int d = gui::metrics::text::height*12/7;
            int c = (r.x1 + r.x2)/2;

            node->coord = xywh(c-d/2, r.y1+d/2, d, d);
            edge->y1 = r.y1 - d/2;
            edge->y2 = r.y1 + d;
            edge->x2 = c;
            edge->x1 =
                i == 0 ? c:
                i%2 == 0 ?
                r.x1: r.x2;

            place (i*2+1, xyxy(r.x1, r.y1+d+d/2, c, r.y2));
            place (i*2+2, xyxy(c, r.y1+d+d/2, r.x2, r.y2));
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                int d = gui::metrics::text::height*12/7;
                xyxy r = coord.now.local();
                r.x1 += d/2; r.x2 -= d/2;
                place(0, r);
            }
        }
    };

    struct DynamicBST:
    widget<DynamicBST>
    {
        binary_property<str> kind;
        std::deque<std::optional<line>> edges;
        std::deque<std::optional<text::view>> texts;
        std::deque<std::optional<node>> nodes;
        widgetarium<node> queue; deque<str> queux;
        property<int> side = gui::metrics::text::height*12/7;
        property<double> speed = 1.0;
        property<bool> pause = true;
        property<time> timer;
        node maverick;
        int target=0;

        void clear ()
        {
            nodes.clear();
            edges.clear();
            texts.clear();
            maverick.text.text = "";
            target = 0;
        }

        void queue_place ()
        {
            int n = 0;
            int x = 0;
            int d = side.now;
            time ms {int(100/speed.now)};

            while (x + d < queue.coord.now.w)
            {
                auto& q = queue(n++);
                q.coord.go(xywh(x,0,d,d),
                q.coord.now == xywh{} ? time{} : ms);
                x += d;

                if (str(q.text.text) == "")
                {
                    if (n == 1
                    or (n >= 2 and
                    queue(n-2).text.text != "" and
                    queue(n-2).alpha == 255))
                    {
                        str x;
                        if (not queux.empty()) {
                            x = queux.front();
                            queux.pop_front(); }
                        else
                            x = std::to_string(
                            aux::random(0,99));

                        q.text.text = x;
                        q.inner.color = rgba::white;
                        q.outer.color = rgba::red;
                        q.outex.color = rgba::red;
                        q.outey.color = rgba::red;
                        q.hide();
                        q.show(ms);
                    }
                    else q.hide();
                }
            }
            while (
            queue.size() > n) {
            queux.push_front(
            queue.back().text.text);
            queue.truncate(); }
        }

        void place (int i = 0, xyxy r = xywh())
        {
            if (i >= nodes.size()) return;
            auto& node = nodes[i];
            auto& edge = edges[i];
            auto& text = texts[i];
            if (not node) return;

            time ms {int(100/speed.now)};

            // if (i > 0
            // and r.x2-r.x1 < 3
            // and side.now > gui::metrics::text::height/2)
            //     side.go(side-1,
            //         10*ms);

            int d = side.now;
            if (i == 0) {
                r = coord.now.local();
                r.x1 += d/2;
                r.x2 -= d/2;
            }
            int c = (r.x1 + r.x2)/2;

            auto t = node->coord.now == xywh{} ? time{} : ms;
            int dtx = -d/2;
            int dty = d*3/2;

            node->coord.go(xywh(c-d/2, d+r.y1+d/2, d, d), t);
            text->coord.go(xywh(c+dtx, d+r.y1+dty, d, d), t);
            text->alignment = xy{pix::center, pix::center};
            text->font = pix::font{"", (d*7+1)/12};
            edge->y1 = d + r.y1 - d/2;
            edge->y2 = d + r.y1 + d;
            edge->x2 = c;
            edge->x1 =
                i == 0 ? c:
                i%2 == 0 ?
                r.x1: r.x2;

            place (i*2+1, xyxy(r.x1, r.y1+d+d/2, c, r.y2));
            place (i*2+2, xyxy(c, r.y1+d+d/2, r.x2, r.y2));

            if (target == i)
                maverick.coord.go(
                    node->coord.now, ms);
        }

        void on_change (void* what) override
        {
            if (timer.now == time())
                timer.go(time::infinity,
                         time::infinity);

            time ms {int(100/speed.now)};

            if (what == &coord
            or  what == &side)
            {
                xyxy r = coord.now.local();
                r.x1 = (r.x1+r.x2)/2 - side.now/2;
                queue.coord = r;
                queue_place();
                place();
            }

            if (what == &kind) clear();
            
            if (what == &timer)
            {
                if (maverick.text.text == "")
                {
                    if (not queue.empty() and not pause.now)
                    {
                        maverick.text.text = str(
                        queue.front().text.text);
                        maverick.coord =
                        queue.front().coord +
                        queue.coord.now.origin;
                        maverick.inner.color = rgba::white;
                        maverick.outer.color = rgba::red;
                        maverick.outex.color = rgba::red;
                        maverick.outey.color = rgba::red;
                        maverick.show();
                        target = 0;

                        for (int i=0; i<queue.size()-1; i++)
                        {
                            queue(i+0).text.text = str(
                            queue(i+1).text.text);
                            queue(i+0).coord =
                            queue(i+1).coord.now;
                        }
                        queue.back().text.text = "";
                    }
                    else maverick.hide();
                }
                else
                {
                    if (target >= nodes.size()) {
                        nodes.resize(target+1);
                        edges.resize(target+1);
                        texts.resize(target+1);
                    }
                    auto& node = nodes[target];
                    auto& edge = edges[target];
                    auto& text = texts[target];

                    if (not node)
                    {
                        node.emplace(); children += &*node; node->parent = this;
                        edge.emplace(); children += &*edge; edge->parent = this;
                        text.emplace(); children += &*text; text->parent = this;
                        node->text.text = str(maverick.text.text);
                        node->text.color = rgba::navy;
                        node->inner.color = rgba::white;
                        node->outer.color = rgba::red;
                        node->outex.color = rgba::red;
                        node->outey.color = rgba::red;
                        edge->color = rgba::white;
                        text->color = rgba::green;
                        text->text = kind.now == "AVL" ? "0" : "";
                        node->hide();
                        edge->hide();
                        text->hide();
                    }
                    else
                    if (maverick.coord.now.origin == 
                        node->coord.now.origin)
                    {
                        int x = std::stoi(str(maverick.text.text));
                        int y = std::stoi(str(node->text.text));

                        if (x < y)
                        {
                            target = target*2+1;
                            if (kind.now == "AVL") {
                                int w = std::stoi(str(text->text));
                                w--; str plus = w > 0 ? "+" : "";
                                text->text = plus + std::to_string(w);
                                text->color = -1 <= w and w <= 1 ?
                                rgba::green : rgba::red; }
                        }
                        else
                        if (x > y)
                        {
                            target = target*2+2;
                            if (kind.now == "AVL") {
                                int w = std::stoi(str(text->text));
                                w++; str plus = w > 0 ? "+" : "";
                                text->text = plus + std::to_string(w);
                                text->color = -1 <= w and w <= 1 ?
                                rgba::green : rgba::red; }
                        }
                        else
                        {
                            maverick.text.text = "";
                            maverick.hide();
                            rgba color = 
                            node->inner.color.now;
                            node->inner.color = rgba::yellow;
                            node->inner.color.go(color, 300ms);
                            node->show();
                            edge->show();
                            text->show();
                        }
                    }
                }
                queue_place();
                place();
            }
        }
    };
}
