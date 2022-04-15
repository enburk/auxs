#pragma once
#include "sfx_trees_bst.h"
namespace sfx::trees::binary
{
    struct bbst: bst
    {
        virtual void rotate_left (node* l)
        {
            node* r = l->right;
            node* m = r->left;

            if (l->is_left ()) l->up->left  = r; else
            if (l->is_right()) l->up->right = r; else
            root = r;

            r->up = l->up;
            l->up = r; r->left  = l; if (m)
            m->up = l; l->right = m;
        }

        virtual void rotate_right (node* r)
        {
            node* l = r->left;
            node* m = l->right;

            if (r->is_left ()) r->up->left  = l; else
            if (r->is_right()) r->up->right = l; else
            root = l;

            l->up = r->up;
            r->up = l; l->right = r; if (m)
            m->up = r; r->left  = m;
        }
    };
}
