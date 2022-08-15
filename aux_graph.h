#pragma once
#include <unordered_map>
#include "aux_array.h"
#include "aux_aux.h"
namespace aux
{
    template<class x> struct graph
    {
        array<x> vertices;
        std::unordered_multimap <int,int> edges;

        void add (x const& source, x const& target)
        {
            auto& v = vertices;
            auto i1 = v.find_or_emplace(source) - v.begin();
            auto i2 = v.find_or_emplace(target) - v.begin();
            edges.emplace(int(i1), int(i2));
        }

        auto dfs() { return DFS(*this); }

        struct DFS
        {
            graph const& g;
            array<array<int>> cycles;
            array<int> visiting;
            array<int> finished;
            std::unordered_map
            <int,int> parent;

            DFS (graph const& g) : g(g)
            {
                for (int i=0; i<g.vertices.size(); i++)
                    if (not parent.contains(i)) {
                        parent[i] = -1;
                        visit(i); }

                std::ranges::reverse(finished); // topological order
            }

            void visit (int s)
            {
                visiting.push_back(s);

                auto [f,l] = g.edges.equal_range(s);
                for (auto [_,v]: std::ranges::subrange(f,l))
                {
                    if (visiting.contains(v)) {
                        cycles += array<int>{v};
                        for (int i: std::ranges::views::reverse(visiting)) {
                        cycles.back() += i; if (i == s) break; }
                        break; }

                    if (not parent.contains(v)) {
                        parent[v] = s;
                        visit(v); }
                }

                finished.push_back(s);
                visiting.pop_back();
            }
        };

    };
}