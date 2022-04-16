#pragma once
#include "sfx_trees.h"
namespace sfx::trees::binary
{
    struct bst:
    widget<bst>
    {
        queue queue;
        property<int> side = gui::metrics::text::height*12/7;
        property<double> speed = 0.1;//1.0;
        property<bool> pause = true;
        property<time> timer;
        node* root = nullptr;
        node maverick;
        node balancer;
        bool moving;

        ~bst() { clear(); }

        void clear ()
        {
            delete root;
            root = nullptr;
            maverick.value.text = "";
            balancer.label.text = "";
        }

        void place (node* x, xyxy r)
        {
            if (!x) return;
            int d = side.now;
            int c = (r.x1 + r.x2)/2;

            time ms {int(100/speed.now)};
            time t = x->x == 0 and x->y == 0 ? time{} : ms;

            x->x.go(c, t);
            x->y.go(r.yl + d, t);
            x->r.go(d/2, t);

            if (x->moving()) moving = true; //else
            {
                int lh = x->left  ? x->left ->height : -1;
                int rh = x->right ? x->right->height : -1;
                x->height = max(lh, rh) + 1;
                x->balance = rh - lh;
            }

            if (x->up) {
                x->edge.x1 = x->x;
                x->edge.y1 = x->y;
                x->edge.x2 = x->up->x;
                x->edge.y2 = x->up->y;
            }
            else {
                x->edge.x1 = 0;
                x->edge.y1 = 0;
                x->edge.x2 = 0;
                x->edge.y2 = 0;
            }

            place (x->left , xyxy(r.x1, r.y1+d+d/2, c, r.y2));
            place (x->right, xyxy(c, r.y1+d+d/2, r.x2, r.y2));

            if (maverick.up == x) {
                maverick.x.go(x->x, ms);
                maverick.y.go(x->y, ms);
                maverick.r.go(x->r, ms);
            }

            if (balancer.up == x) {
                balancer.x.go(x->x, ms);
                balancer.y.go(x->y, ms);
                balancer.r.go(x->r, ms);
            }

            if (maverick.moving()) moving = true;
            if (balancer.moving()) moving = true;
        }
        void place ()
        {
            moving = false;
            xyxy r = coord.now.local();
            r.x1 += side/2;
            r.x2 -= side/2;
            r.y1 += side;
            place(root, r);
            if (not moving)
                tick();
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

            if (what == &speed) queue.speed = speed.now;
            
            if (what == &timer and alpha.now == 255)
            {
                if (maverick.value.text == ""
                and balancer.label.text == "")
                {
                    if (not queue.empty() and not pause.now)
                    {
                        maverick.mimic(
                        queue.nodes.front());
                        queue.pop();
                        maverick.x = maverick.x + queue.coord.now.x;
                        maverick.y = maverick.y + queue.coord.now.y;
                        maverick.show();

                        if (!root)
                        insert(root, nullptr);
                        maverick.up = root;
                    }
                    else maverick.hide();
                    place();
                    return;
                }

                node* target = maverick.up;
                if (maverick.value.text != ""
                and maverick.x == target->x
                and maverick.y == target->y)
                {
                    int i = value(maverick);
                    int j = value(target);

                    if (i < j)
                    {
                        if (!target->left) insert(
                             target->left,
                             target);

                        maverick.up = target->left;
                    }
                    else
                    if (i > j)
                    {
                        if (!target->right) insert(
                             target->right,
                             target);

                        maverick.up = target->right;
                    }
                    else
                    {
                        maverick.value.text = "";
                        maverick.hide();
                        rgba color = 
                        target->inner.color.now;
                        target->inner.color = rgba::yellow;
                        target->inner.color.go(color, 300ms);
                        target->show();
                        target->edge.show();

                        if (target->label.text == "just inserted") {
                            target->label.text = "";
                            new_leaf(target);
                        }
                    }
                }

                place();
            }
        }

        static int value (node& node) { return std::stoi(str(node.value.text)); }
        static int label (node& node) { return std::stoi(str(node.label.text)); }
        static int value (node* node) { return value(*node); }
        static int label (node* node) { return label(*node); }

        virtual void insert (node*& x, node* up)
        {
            x = new node;
            children += x;
            children += &x->edge;
            x->parent = this;
            x->edge.parent = this;
            x->edge.color = rgba::white;
            x->value.text = maverick.value.text;
            x->label.text = "just inserted";
            x->up = up;
            x->hide();
            x->edge.hide();
        }

        virtual void new_leaf (node* leaf) {}

        virtual void tick () {}
    };
}
