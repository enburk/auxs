#pragma once
#include "sfx_trees.h"
namespace sfx::trees::binary
{
    struct bst:
    widget<bst>
    {
        std::unique_ptr<node> root;

        void clear () { root.reset(); } // f*ck stack overflow

        void insert (int key, std::unique_ptr<node>& n, node* parent)
        {
            if (!n.get()) {
            n = std::make_unique<node>();
            n->edge = std::make_unique<line>();
            n->edge->color = rgba::white;
            n->value.text = std::to_string(key);
            n->parent = parent;
            children += n.get();
            children += n->edge.get();
            n->widget::parent = this;
            n->edge->widget::parent = this;
            return; }

            int value = std::stoi(str(n->value.text));
            if (key < value) insert(key, n->left,  n.get()); else
            if (key > value) insert(key, n->right, n.get()); else
            {}
        }
        void insert (int key) { insert(key, root, nullptr); }

        void place (node* n, xyxy r)
        {
            if (!n) return;
            int d = gui::metrics::text::height*12/7;
            int c = (r.x1 + r.x2)/2;

            n->x = c;
            n->y = r.yl + d;
            n->r = d/2;

            if (n->parent) {
                n->edge->x1 = n->x;
                n->edge->y1 = n->y;
                n->edge->x2 = n->parent->x;
                n->edge->y2 = n->parent->y;
            }

            place (n->left .get(), xyxy(r.x1, r.y1+d+d/2, c, r.y2));
            place (n->right.get(), xyxy(c, r.y1+d+d/2, r.x2, r.y2));
        }
        void place ()
        {
            int d = gui::metrics::text::height*12/7;
            xyxy r = coord.now.local();
            r.x1 += d/2; r.x2 -= d/2;
            place(root.get(), r);
        }

        void on_change (void* what) override
        {
            if (what == &coord)
            {
                place();
            }
        }

        generator<node*> nodes ()
        {
            if (root) 
            for (node* x: root->post_order())
                co_yield x;
        }
    };
}
