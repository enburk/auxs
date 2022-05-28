#pragma once
#include "sfx.h"
namespace sfx::trees::binary
{
    struct node : sfx::node
    {
        node* up    = nullptr;
        node* left  = nullptr;
        node* right = nullptr;
        line  edge;

        int height  = 0;
        int balance = 0;

        ~node()
        {
            delete left;
            delete right;
        }

        bool is_left  () { return up and up->left  == this; }
        bool is_right () { return up and up->right == this; }

        bool moving () { return 
            (x.now != x.to) or
            (y.now != y.to); }

        generator<node*> pre_order ()
        {
            co_yield this;
            if (left)  for (node* x: left ->pre_order()) co_yield x;
            if (right) for (node* x: right->pre_order()) co_yield x;
        }
        generator<node*> in_order ()
        {
            if (left)  for (node* x: left ->in_order()) co_yield x;  co_yield this;
            if (right) for (node* x: right->in_order()) co_yield x;
        }
        generator<node*> post_order ()
        {
            if (left)  for (node* x: left ->post_order()) co_yield x;
            if (right) for (node* x: right->post_order()) co_yield x;
            co_yield this;
        }
    };

    struct static_bst:
    widget<static_bst>
    {
        node* root;

        ~static_bst() { clear(); }

        void clear () { delete root; root = nullptr; } // f*ck stack overflow

        void insert (int key, node*& x, node* up)
        {
            if (!x)
            {
                x = new node;
                children.insert(
                children.end(), x);
                children.insert(
                children.begin(),
                    &x->edge);

                x->parent = this;
                x->edge.parent = this;
                x->edge.color = rgba::white;
                x->value.text = std::to_string(key);
                x->up = up;
                return;
            }

            int value = std::stoi(str(x->value.text));
            if (key < value) insert(key, x->left,  x); else
            if (key > value) insert(key, x->right, x); else
            {}
        }
        void insert (int key) { insert(key, root, nullptr); }

        void place (node* x, xyxy r)
        {
            if (!x) return;
            int d = gui::metrics::text::height*13/10;
            int c = (r.x1 + r.x2)/2;

            x->x = c;
            x->y = r.yl + d;
            x->r = d/2;

            if (x->up) {
                x->edge.x1 = x->x;
                x->edge.y1 = x->y;
                x->edge.x2 = x->up->x;
                x->edge.y2 = x->up->y;
            }

            place (x->left , xyxy(r.x1, r.y1+d+d/2, c, r.y2));
            place (x->right, xyxy(c, r.y1+d+d/2, r.x2, r.y2));
        }
        void place ()
        {
            int d = gui::metrics::text::height*13/10;
            xyxy r = coord.now.local();
            r.x1 += d/2; r.x2 -= d/2;
            place(root, r);
        }

        void on_change (void* what) override
        {
            if (what == &coord)
                place();
        }

        generator<node*> post_order ()
        {
            if (root) 
            for (node* x: root->post_order())
                co_yield x;
        }
    };
}
