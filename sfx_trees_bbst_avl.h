#pragma once
#include "sfx_trees_bbst.h"
namespace sfx::trees::binary
{
    struct unbalanced: bst
    {
        unbalanced () { title.text = "unbalanced BST"; }

        void new_leaf (node* leaf) override
        {
            leaf->label.text = "0";
            if (not leaf->up)
                return;

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

        void tick () override
        {
            if (balancer.label.text == ""
            or  balancer.x != balancer.up->x
            or  balancer.y != balancer.up->y)
                return;

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

        void set (node& node, int balance)
        {
            auto plus = balance > 0 ? "+" : "";
            node.label.text = plus + std::to_string(balance);
            node.label.color = -1 <= balance and balance <= 1 ?
            rgba::green : rgba::red;
        }
    };

    struct avl: bbst
    {
        avl () { title.text = "AVL tree"; }

        void new_leaf (node* leaf) override
        {
            leaf->label.text = "0";
            if (not leaf->up)
                return;

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

        void tick () override
        {
            if (balancer.label.text == ""
            or  balancer.x != balancer.up->x
            or  balancer.y != balancer.up->y)
                return;

            node& balanced = *(balancer.up);
            int balance = balanced.balance;
            set(balanced, balance);

            if (-1 == balance or balance == +1)
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

            if (balance < -1)
            {
                set(balancer, 0);
                if (balanced.left->balance > 0)
                    rotate_left(balanced.left);
                else rotate_right(&balanced);
            }
            else
            if (balance > +1)
            {
                set(balancer, 0);
                if (balanced.right->balance < 0)
                    rotate_right(balanced.right);
                else rotate_left(&balanced);
            }
            else update_balance(root);
        }

        void set (node& node, int balance)
        {
            auto plus = balance > 0 ? "+" : "";
            node.label.text = plus + std::to_string(balance);
            node.label.color = -1 <= balance and balance <= 1 ?
            rgba::green : rgba::red;
        }
        void set (node* node, int balance) { set(*node, balance); }

        void update_balance(node* x)
        {
            if (!x) return;
            set(*x, x->balance);
            update_balance(x->left);
            update_balance(x->right);
        }
    };
}
