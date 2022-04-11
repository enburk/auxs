#pragma once
#include "sfx_trees.h"
namespace sfx::trees::binary
{
    struct bbst:
    widget<bbst>
    {
        queue queue;
        binary_property<str> kind;
        std::unique_ptr<node> root;
        std::unique_ptr<node>* target = &root;
        property<int> side = gui::metrics::text::height*12/7;
        property<double> speed = 1.0;
        property<bool> pause = true;
        property<time> timer;
        node maverick;

        void clear ()
        {
            root.reset();
            maverick.value.text = "";
            target = &root;
        }

        void place (node* n, xyxy r)
        {
            if (!n) return;
            int d = side.now;
            int c = (r.x1 + r.x2)/2;

            time ms {int(100/speed.now)};
            time t = n->x == 0 and n->y == 0 ? time{} : ms;

            n->x.go(c, t);
            n->y.go(r.yl + d, t);
            n->r.go(d/2, t);

            if (n->parent) {
                n->edge->x1 = n->x;
                n->edge->y1 = n->y;
                n->edge->x2 = n->parent->x;
                n->edge->y2 = n->parent->y;
            }

            place (n->left .get(), xyxy(r.x1, r.y1+d+d/2, c, r.y2));
            place (n->right.get(), xyxy(c, r.y1+d+d/2, r.x2, r.y2));

            if (target->get() == n) {
                maverick.x.go(n->x, ms);
                maverick.y.go(n->y, ms);
                maverick.r.go(n->r, ms);
            }
        }
        void place ()
        {
            xyxy r = coord.now.local();
            r.x1 += side/2;
            r.x2 -= side/2;
            r.y1 += side;
            place(root.get(), r);
        }

        void on_change (void* what) override
        {
            if (what == &coord
            or  what == &side)
            {
                if (timer == time{})
                    timer.go(time::infinity,
                             time::infinity);

                xyxy r = coord.now.local();
                r.x1 = (r.x1+r.x2)/2 - side.now/2;
                r.y2 = side.now;
                queue.coord = r;
                place();
            }

            if (what == &kind) clear();
            
            if (what == &speed) queue.speed = speed.now;
            
            if (what == &timer and alpha.now == 255)
            {
                if (maverick.value.text == "")
                {
                    if (not queue.empty() and not pause.now)
                    {
                        maverick.mimic(
                        queue.nodes.front());
                        queue.pop();
                        maverick.x = maverick.x + queue.coord.now.x;
                        maverick.y = maverick.y + queue.coord.now.y;
                        maverick.show();
                        target = &root;
                        if (!root.get())
                        insert(root,
                        nullptr);
                    }
                    else maverick.hide();
                    place();
                    return;
                }

                auto& node = *target;
                
                if (node.get()
                and maverick.x == node->x
                and maverick.y == node->y)
                {
                    int i = std::stoi(str(maverick.value.text));
                    int j = std::stoi(str(node->value.text));

                    if (i < j)
                    {
                        if (!node->left.get()) insert(
                             node->left,
                             node.get());

                        target = &node->left;
                    }
                    else
                    if (i > j)
                    {
                        if (!node->right.get()) insert(
                             node->right,
                             node.get());

                        target = &node->right;
                    }
                    else
                    {
                        maverick.value.text = "";
                        maverick.hide();
                        rgba color = 
                        node->inner.color.now;
                        node->inner.color = rgba::yellow;
                        node->inner.color.go(color, 300ms);
                        node->show();
                        node->edge->show();
                    }
                }
                place();
            }
        }

        void insert (std::unique_ptr<node>& n, node* parent)
        {
            n = std::make_unique<node>();
            n->edge = std::make_unique<line>();
            n->edge->color = rgba::white;
            n->value.text = maverick.value.text;
            n->parent = parent;
            children += n.get();
            children += n->edge.get();
            n->widget::parent = this;
            n->edge->widget::parent = this;

            n->hide();
            n->edge->hide();

            if (kind.now == "AVL")
            n->label.text = "0";
        }
    };
}
