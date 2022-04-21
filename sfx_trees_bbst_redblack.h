#pragma once
#include "sfx_trees_bbst.h"
namespace sfx::trees::binary
{
    struct redblack: bbst
    {
        redblack () { title.text = "red-black tree"; }

        void new_leaf (node* leaf) override
        {
            leaf->value.color = rgba::white;
            leaf->outer.color = rgba::white;
            leaf->outex.color = rgba::white;
            leaf->outey.color = rgba::white;
            leaf->inner.color = rgba::red;
            balancer.label.text = "work";
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
                x->inner.color = rgba::black;
                balancer.label.text = "";
                return; }
            if (p->inner.color == rgba::black) {
                balancer.label.text = "";
                return; }

            // both x and parent are red
            node* g = p->up; // grandfather
            if (not g) {
                p->inner.color = rgba::black;
                balancer.label.text = "";
                return; }

            node* u = p->is_left() ? // uncle
                g->right : g->left;

            if (u and
                u->inner.color == rgba::red) {
                u->inner.color = rgba::black;
                p->inner.color = rgba::black;
                g->inner.color = rgba::red;
                balancer.up = g;
                return; }

            if (x->is_left()
            and p->is_left())
            {
                balancer.label.text = "";
                p->inner.color = rgba::black;
                g->inner.color = rgba::red;
                rotate_right(g);
            }
            else
            if (x->is_right()
            and p->is_right())
            {
                balancer.label.text = "";
                p->inner.color = rgba::black;
                g->inner.color = rgba::red;
                rotate_left(g);
            }
            else
            if (x->is_left()
            and p->is_right())
            {
                balancer.up = p;
                rotate_right(p);
            }
            else
            if (x->is_right()
            and p->is_left())
            {
                balancer.up = p;
                rotate_left(p);
            }
        }
    };
}
