#pragma once
#include "sfx_trees_bbst.h"
namespace sfx::trees::binary
{
    struct splay: bbst
    {
        splay () { title.text = "splay tree"; }

        void new_leaf (node* leaf) override
        {
            leaf->inner.color = rgba::yellow;
            balancer.label.text = "next";
            balancer.up = leaf;
            balancer.hide();
        }

        void hit (node* leaf) override
        {
            leaf->inner.color = rgba::yellow;
            balancer.label.text = "next";
            balancer.up = leaf;
            balancer.hide();
        }

        void tick () override
        {
            if (balancer.label.text == ""
            or  balancer.x != balancer.up->x
            or  balancer.y != balancer.up->y)
                return;

            node* x = balancer.up;
            node* p = x->up;
            if (not p) {
                x->inner.color.go(rgba::white, 300ms);
                balancer.label.text = "";
                return; }

            auto& next = balancer.label.text;

            node* g = p->up;
            if (not g // zig
            or next == "zig"
            or next == "zag")
            {
                if (x->is_left())
                rotate_right(p); else
                rotate_left(p);
                next = "next";
            }
            else // zig-zig
            if (x->is_left()
            and p->is_left())
            {
                rotate_right(g);
                next = "zig";
            }
            else // zig-zig
            if (x->is_right()
            and p->is_right())
            {
                rotate_left(g);
                next = "zig";
            }
            else // zig-zag
            if (x->is_left()
            and p->is_right())
            {
                rotate_right(p);
                next = "zag";
            }
            else // zig-zag
            if (x->is_right()
            and p->is_left())
            {
                rotate_left(p);
                next = "zag";
            }
        }
    };
}
