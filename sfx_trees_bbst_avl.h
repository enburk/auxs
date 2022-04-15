#pragma once
#include "sfx_trees_bbst.h"
namespace sfx::trees::binary
{
    struct unbalanced: bbst
    {
        void new_leaf (node* leaf) override
        {
            leaf->label.text = "0";
            if (leaf->up)
            {
                balancer.up = leaf->up;
                balancer.mimic(maverick);
                balancer.value.hide();
                balancer.inner.hide();
                balancer.outer.hide();
                balancer.outex.hide();
                balancer.outey.hide();
                balancer.label.show();
                set(balancer,
                leaf->is_left() ?
                -1: +1);
            }
        }

        void tick () override
        {
            if (balancer.label.text != ""
            and balancer.x == balancer.up->x
            and balancer.y == balancer.up->y)
            {
                node& balanced = *(balancer.up);

                int z = label(balancer);
                int x = label(balanced);
                x += z; set(balanced, x);

                if (not balanced.up
                or (z < 0 and x-z > 0)
                or (z > 0 and x-z < 0))
                {
                    balancer.label.text = "";
                    balancer.label.hide();
                }
                else
                {
                    set(
                    balancer, 
                    balanced.is_left() ? -1: +1);
                    balancer.up =
                    balanced.up;
                }
            }
        }

        void set (node& node, int balance)
        {
            auto plus = balance > 0 ? "+" : "";
            node.label.text = plus + std::to_string(balance);
            node.label.color = -1 <= balance and balance <= 1 ?
            rgba::green : rgba::red;
        }
    };

    struct avl: unbalanced
    {
        void tick () override
        {
            if (balancer.label.text != ""
            and balancer.x == balancer.up->x
            and balancer.y == balancer.up->y)
            {
                node& balanced = *(balancer.up);

                int z = label(balancer);
                int x = label(balanced);
                x += z; set(balanced, x);

                if (-1 <= x and x <= +1)
                {
                    if (not balanced.up)
                    {
                        balancer.label.text = "";
                        balancer.label.hide();
                    }
                    else
                    {
                        set(
                        balancer, 
                        balanced.is_left() ? -1: +1);
                        balancer.up =
                        balanced.up;
                    }
                    return;
                }

                balancer.label.hide();
                balancer.label.text = "";

                if (x < -1)
                {
                    if (label(balanced.left) > 0)
                    {
                        rotate_left(balanced.left);
                        set(balancer, 0);
                        return;
                    }
                    rotate_right(&balanced);
                }
                else
                if (x > +1)
                {
                    if (label(balanced.right) < 0)
                    {
                        rotate_right(balanced.right);
                        set(balancer, 0);
                        return;
                    }
                    rotate_left(&balanced);
                }
            }
        }

        void rotate_left (node* l)
        {
            node* r = l->right;

            int lv = label(l);
            int rv = label(r);
            set(*l, lv - rv - 1);
            set(*r, rv - 1);

            //if (r->label.text == "+1") {
            //    l->label.text =  "0";
            //    r->label.text =  "0";
            //}
            //else {
            //    l->label.text = "+1";
            //    r->label.text = "-1";
            //}

            bbst::rotate_left(l);
        }

        void rotate_right (node* r)
        {
            node* l = r->left;

            int rv = label(r);
            int lv = label(l);
            set(*r, rv + lv + 1);
            set(*l, lv + 1);

            //if (l->label.text == "-1") {
            //    r->label.text =  "0";
            //    l->label.text =  "0";
            //}
            //else {
            //    r->label.text = "-1";
            //    l->label.text = "+1";
            //}

            bbst::rotate_right(r);
        }
    };
}
