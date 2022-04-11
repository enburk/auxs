#pragma once
#include "sfx.h"
namespace sfx::trees::binary
{
    struct node : sfx::node
    {
        node* parent = nullptr;
        std::unique_ptr<node> left;
        std::unique_ptr<node> right;
        std::unique_ptr<line> edge;

        void pre_order
        (auto const& f)
        {
            f(this);
            if (left)  left ->pre_order(f);
            if (right) right->pre_order(f);
        }
        void in_order
        (auto const& f)
        {
            if (left)  left ->in_order(f); f(this);
            if (right) right->in_order(f);
        }
        void post_order
        (auto const& f)
        {
            if (left)  left ->post_order(f);
            if (right) right->post_order(f);
            f(this);
        }
        auto pre_order
        () -> generator<node*>
        {
            co_yield this;
            if (left)  for (node* x: left ->pre_order()) co_yield x;
            if (right) for (node* x: right->pre_order()) co_yield x;
        }
        auto in_order
        () -> generator<node*>
        {
            if (left)  for (node* x: left ->in_order()) co_yield x;  co_yield this;
            if (right) for (node* x: right->in_order()) co_yield x;
        }
        auto post_order
        () -> generator<node*>
        {
            if (left)  for (node* x: left ->post_order()) co_yield x;
            if (right) for (node* x: right->post_order()) co_yield x;
            co_yield this;
        }
    };
}
