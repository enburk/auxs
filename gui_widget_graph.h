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

        void clear ()
        {
            nodes.clear();
            edges.clear();
            children.clear();
        }

        void place (int i, xyxy r)
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
        std::deque<std::optional<node>> nodes;
        std::deque<std::optional<text::view>> texts;
        widgetarium<node> queue; deque<str> queux;
        property<int> side = gui::metrics::text::height*12/7;
        property<double> speed = 1.0;
        property<time> timer;
        node maverick;
        int target=0;

        void clear ()
        {
            nodes.clear();
            edges.clear();
            texts.clear();
            queue.clear();
            queux.clear();
            children.clear();
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

            int d = side.now;
            if (i == 0) {
                r = coord.now.local();
                r.x1 += d/2;
                r.x2 -= d/2;
            }
            time ms {int(100/speed.now)};
            int W = r.x2 - r.x1; if (W <= 0) return;
            int H = r.y2 - r.y1; if (H <= 0) return;
            int c = (r.x1 + r.x2)/2;

            auto t = time(100);//node->coord.now == xywh{} ? time{} : ms;

            node->coord.go(xywh(c-d/2, d+r.y1+d/2, d, d), t);
            text->coord.go(xywh(c+d/2, d+r.y1+d/2, d, d), t);
            text->alignment = xy{pix::left, pix::center};
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

            if (what == &coord)
            {
                xyxy r = coord.now.local();
                r.x1 = (r.x1+r.x2)/2 - side.now/2;
                queue.coord = r;
                queue_place();
                place();
            }
            
            if (what == &timer)
            {
                if (maverick.text.text == "")
                {
                    if (not queue.empty())
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
                            queue(i+1).coord;
                        }
                        queue.back().text.text = "";
                        queue_place();
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
                        node.emplace(); children += &*node; node->parent = this; gui::widgets.insert(&*node);
                        edge.emplace(); children += &*edge; edge->parent = this; gui::widgets.insert(&*edge);
                        text.emplace(); children += &*text; text->parent = this; gui::widgets.insert(&*text);
                        node->text.text = str(maverick.text.text);
                        node->text.color = rgba::navy;
                        node->inner.color = rgba::white;
                        node->outer.color = rgba::red;
                        node->outex.color = rgba::red;
                        node->outey.color = rgba::red;
                        edge->color = rgba::white;
                        //node->hide();
                        //edge->hide();
                        text->hide();
                        place();
                    }
                    else
                    if (maverick.coord.now.origin == 
                        node->coord.now.origin)
                    {
                        int x = std::stoi(str(maverick.text.text));
                        int y = std::stoi(str(node->text.text));

                        if (x < y) target = target*2+1; else
                        if (x > y) target = target*2+2; else
                        {
                            maverick.text.text = "";
                            maverick.hide();
                            node->show();
                            edge->show();
                            text->show();
                        }
                        place();
                    }
                    queue_place();
                }
                queue_place();
                place();
            }
        }
    };
}
