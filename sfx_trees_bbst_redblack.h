#pragma once
#include "sfx_trees.h"
namespace sfx::trees::binary
{
    struct bbst:
    widget<bbst>
    {
        queue queue;
        binary_property<str> kind;
        property<int> side = gui::metrics::text::height*12/7;
        property<double> speed = 0.1;//1.0;
        property<bool> pause = true;
        property<time> timer;
        node maverick;
        node balancer;
        node* root;

        void clear ()
        {
            delete root;
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

            if (x->up) {
                x->edge.x1 = x->x;
                x->edge.y1 = x->y;
                x->edge.x2 = x->up->x;
                x->edge.y2 = x->up->y;
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
        }
        void place ()
        {
            xyxy r = coord.now.local();
            r.x1 += side/2;
            r.x2 -= side/2;
            r.y1 += side;
            place(root, r);
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

                if (node*& target =
                    maverick.up; target
                and maverick.value.text != ""
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

                        target = target->left;
                    }
                    else
                    if (i > j)
                    {
                        if (!target->right) insert(
                             target->right,
                             target);

                        target = target->right;
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
                        leaf(target);
                    }
                }

                tick();
                place();
            }
        }

        static int value (node& node) { return std::stoi(str(node.value.text)); }
        static int label (node& node) { return std::stoi(str(node.label.text)); }
        static int value (node* node) { return value(*node); }
        static int label (node* node) { return label(*node); }

        void insert (node*& x, node* up)
        {
            x = new node;
            children += x;
            children += &x->edge;
            x->parent = this;
            x->edge.parent = this;
            x->edge.color = rgba::white;
            x->value.text = maverick.value.text;
            x->up = up;
            x->hide();
            x->edge.hide();

            if (kind.now == "AVL")
            x->label.text = "0";
        }

        void leaf (node* leaf)
        {
            if (kind.now == "AVL" && leaf->up)
            {
                balancer.up = leaf->up;
                balancer.mimic(maverick);
                balancer.value.hide();
                balancer.inner.hide();
                balancer.outer.hide();
                balancer.outex.hide();
                balancer.outey.hide();
                balancer.label.show();
                balancer.label.text =
                leaf->up->left  == leaf ? "-1":
                leaf->up->right == leaf ? "+1": "";
                balancer.label.color = rgba::green;
            }
        }

        void tick ()
        {
            if (kind.now == "AVL"
            and balancer.label.text != ""
            and balancer.x == balancer.up->x
            and balancer.y == balancer.up->y)
            {
                node& balanced = *(balancer.up);

                int z = label(balancer);
                int x = label(balanced);
                x += z;
                str plus = x > 0 ? "+" : "";
                balanced.label.text = plus + std::to_string(x);
                balanced.label.color = -1 <= x and x <= 1 ?
                rgba::green : rgba::red;

                if (x == 0)
                {
                    balancer.label.text = "";
                    balancer.label.hide();
                }
                else
                if (x < -1)
                {
                    rotate_right(&balanced);
                }
                else
                if (x > +1)
                {
                    rotate_left(&balanced);
                }
                else
                if (balanced.up)
                {
                    balancer.label.text =
                    balanced.up->left  == &balanced ? "-1":
                    balanced.up->right == &balanced ? "+1": "";
                    balancer.up =
                    balanced.up;
                }
                else
                {
                    balancer.label.text = "";
                    balancer.label.hide();
                }
            }
        }

        void rotate_left (node* l)
        {
            node* r = l->right;
            node* m = r->left;

            if (r->value.text == "-1") {
                rotate_right(r);
                return;
            }
            if (r->value.text == "+1") {
                l->value.text =  "0";
                r->value.text =  "0";
            }
            else {
                l->value.text = "+1";
                r->value.text = "-1";
            }

            if(!l->up) root  = r; else
            if (l->up->left == l)
                l->up->left  = r; else
                l->up->right = r;

            r->up = l->up;
            l->up = r; r->left  = l; if (m)
            m->up = l; l->right = m;
        }

        void rotate_right (node* r)
        {
            node* l = r->left;
            node* m = l->right;

            if (l->value.text == "-1") {
                rotate_left(l);
                return;
            }
            if (l->value.text == "+1") {
                r->value.text =  "0";
                l->value.text =  "0";
            }
            else {
                r->value.text = "+1";
                l->value.text = "-1";
            }

            if(!r->up) root  = l; else
            if (r->up->left == r)
                r->up->left  = l; else
                r->up->right = l;

            l->up = r->up;
            r->up = l; l->right = r; if (m)
            m->up = r; r->left  = m;
        }
    };
}
