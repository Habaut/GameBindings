// LikeMagic C++ Binding Library
// Copyright 2008-2010 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)


#pragma once

#include <utility> // std::pair
#include <map>

#include "boost/graph/graph_traits.hpp"
#include "boost/graph/adjacency_list.hpp"

#include "boost/unordered_map.hpp"

#include "LikeMagic/TypeConv/AbstractTypeConverter.hpp"
#include "LikeMagic/SFMO/AbstractExpression.hpp"

#include "LikeMagic/Utility/AbstractTypeInfo.hpp"
#include "LikeMagic/Utility/KeyWrapper.hpp"

namespace LikeMagic { namespace TypeConv {

using namespace boost::graph;
using LikeMagic::Utility::TypeInfoKey;
using LikeMagic::Utility::TypeInfoPtr;
using LikeMagic::Utility::TypeInfoList;

struct FindType;

class TypeConvGraph
{
private:
    // To give it acess to our private typedefs.
    friend struct FindType;

    typedef boost::intrusive_ptr<AbstractTypeConverter const> p_conv_t;
    typedef boost::shared_ptr<std::vector<p_conv_t>> p_chain_t;

    struct edge_info
    {
        p_conv_t conv;
    };

    struct vertex_info
    {
        TypeInfoKey type;
    };

    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, vertex_info, edge_info> graph_t;
    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_t;
    typedef boost::graph_traits<graph_t>::edge_descriptor edge_t;
    typedef std::map<TypeInfoKey, vertex_t> vertex_map_t;

    graph_t graph;
    vertex_map_t vertex_map;

    // Mutable is for when the object is logically const, but technically a member needs
    // to sometimes change in a way that's not visible from outside the class.
    // Caching is a perfect example of this.
    mutable boost::unordered_map<std::pair<TypeInfoKey, TypeInfoKey>, p_chain_t> conv_cache;

    bool has_type(TypeInfoKey type) const;

    ExprPtr build_conv_chain(ExprPtr from_expr, p_chain_t chain) const;
    p_chain_t search_for_conv(TypeInfoKey from, TypeInfoKey to) const;

    // Don't allow TypeConvGraph to be copied accidently.
    TypeConvGraph(TypeConvGraph const&)=delete;
    TypeConvGraph& operator =(TypeConvGraph const&)=delete;

    vertex_t const no_vertex;

public:
    TypeConvGraph();
    ~TypeConvGraph();
    vertex_t  add_type(TypeInfoPtr type);
    void add_conv(TypeInfoPtr from, TypeInfoPtr to, p_conv_t conv);
    ExprPtr wrap_expr(ExprPtr from_expr, TypeInfoPtr from, TypeInfoPtr to) const;
    bool has_conv(TypeInfoPtr from_type, TypeInfoPtr to_type) const;
    void print_graph() const;
};

}}
