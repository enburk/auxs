#pragma once
#include "gui_widget_geometry.h"
#include "gui_widget_text_editline.h"
namespace gui::graphs
{
    struct node:
    widget<node>
    {
        oval inner;
        oval outer;
        oval outex;
        oval outey;
        text::view text;
        text::view kind;
        property<double> x = 0.0;
        property<double> y = 0.0;
        property<double> r = 0.0;
        property<double> kinda = pi*3/4;

        node ()
        {
            text .color.now = rgba::navy;
            kind .color.now = rgba::green;
            inner.color.now = rgba::white;
            outer.color.now = rgba::red;
            outex.color.now = rgba::red;
            outey.color.now = rgba::red;
        }
        void mimic (node const& n)
        {
            text.text = str(n.text.text);
            kind.text = str(n.kind.text);
            x = n.x;
            y = n.y;
            r = n.r;
        }
        void on_change (void* what) override
        {
            if (what == &x or
                what == &y or
                what == &r or
                what == &kinda)
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
                auto font =
                text.font.now;
                font.size = (d*7+1)/12;
                text.font = font;
                kind.font = font;

                text.coord = xywh(d,d,d,d);
                auto p = aux::vector<2>{d,0}.rotated(kinda);
                int xx = int(std::round(p.x));
                int yy = int(std::round(p.y));
                kind.coord = xyxy(
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
        property<double> speed = 1.0;
        property<time> timer;
        deque<str> extra;

        void pop ()
        {
            for (int i=0; i<nodes.size()-1; i++)
            nodes(i).mimic(nodes(i+1));
            nodes.back().text.text = "";
        }
        void push (str s) { extra += s; }
        bool empty () { return nodes.empty(); }
        void clear ()
        {
            nodes.clear();
            extra.clear();
        }

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

                    if (str(q.text.text) != "")
                        continue;

                    if (n == 1
                    or (n >= 2 and
                    nodes(n-2).text.text != "" and
                    nodes(n-2).alpha == 255))
                    {
                        str x;
                        if (not extra.empty()) {
                            x = extra.front();
                            extra.pop_front(); }
                        else
                            x = std::to_string(
                            aux::random(0,99));

                        q.text.text = x;
                        q.hide();
                        q.show(ms);
                    }
                    else q.hide();
                }
                while (
                nodes.size() > n) {
                extra.push_front(
                nodes.back().text.text);
                nodes.truncate(); }
            }
        }
    };

    struct bst:
    widget<bst>
    {
        struct node : gui::graphs::node
        {
            node* parent = nullptr;
            std::unique_ptr<node> left;
            std::unique_ptr<node> right;
            line edge;
        };
        std::unique_ptr<node> root;
        property<double> speed = 1.0;
        property<time> timer;

        void clear () { root.reset(); }

        void proceed (time t, int key, node* n = nullptr)
        {

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
        std::deque<std::optional<node>> nodes;
        queue queue;
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
            maverick.text.text = "";
            target = 0;
        }

        void place (int i = 0, xyxy r = xywh())
        {
            if (i >= nodes.size()) return;
            auto& node = nodes[i];
            auto& edge = edges[i];
            if (not node) return;

            int d = side.now;
            if (i == 0) {
                r = coord.now.local();
                r.x1 += d/2;
                r.x2 -= d/2;
            }
            int c = (r.x1 + r.x2)/2;

            time ms {int(100/speed.now)};
            auto t = node->coord.now == xywh{} ? time{} : ms;

            node->coord.go(xywh(c-d/2, d+r.y1+d/2, d, d), t);
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
            if (timer.now == time{})
                timer.go(time::infinity,
                         time::infinity);

            time ms {int(100/speed.now)};

            if (what == &coord
            or  what == &side)
            {
                xyxy r = coord.now.local();
                r.x1 = (r.x1+r.x2)/2 - side.now/2;
                r.y2 = side.now;
                queue.coord = r;
                place();
            }

            if (what == &kind) clear();
            
            if (what == &speed) queue.speed = speed.now;
            
            if (what == &timer)
            {
                if (maverick.text.text == "")
                {
                    if (not queue.empty() and not pause.now)
                    {
                        maverick.text.text = str(
                        queue.nodes.front().text.text);
                        maverick.coord =
                        queue.nodes.front().coord.now +
                        queue.coord.now.origin;
                        maverick.show();
                        target = 0;
                        queue.pop();
                    }
                    else maverick.hide();
                }
                else
                {
                    if (target >= nodes.size()) {
                        nodes.resize(target+1);
                        edges.resize(target+1);
                    }
                    auto& node = nodes[target];
                    auto& edge = edges[target];

                    if (not node)
                    {
                        node.emplace(); children += &*node; node->parent = this;
                        edge.emplace(); children += &*edge; edge->parent = this;
                        node->text.text = str(maverick.text.text);
                        edge->color = rgba::white;
                        node->kind.text = kind.now == "AVL" ? "0" : "";
                        node->hide();
                        edge->hide();
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
                                int w = std::stoi(str(node->kind.text));
                                w--; str plus = w > 0 ? "+" : "";
                                node->kind.text = plus + std::to_string(w);
                                node->kind.color = -1 <= w and w <= 1 ?
                                rgba::green : rgba::red; }
                        }
                        else
                        if (x > y)
                        {
                            target = target*2+2;
                            if (kind.now == "AVL") {
                                int w = std::stoi(str(node->kind.text));
                                w++; str plus = w > 0 ? "+" : "";
                                node->kind.text = plus + std::to_string(w);
                                node->kind.color = -1 <= w and w <= 1 ?
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
                        }
                    }
                }
                place();
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
