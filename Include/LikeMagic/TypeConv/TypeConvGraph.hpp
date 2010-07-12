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
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;

#include "LikeMagic/TypeConv/AbstractTypeConverter.hpp"
#include "LikeMagic/SFMO/AbstractExpression.hpp"

#include "LikeMagic/Utility/AbstractTypeInfo.hpp"
#include "LikeMagic/Utility/KeyWrapper.hpp"

namespace LikeMagic { namespace TypeConv {

using LikeMagic::Utility::AbstractTypeInfo;
using LikeMagic::Utility::KeyWrapper;
using namespace boost::graph;

struct FindType;

class TypeConvGraph
{
private:
    // To give it acess to our private typedefs.
    friend struct FindType;

    typedef AbstractTypeConverter const* p_conv_t;

    struct edge_info
    {
        p_conv_t conv;
    };

    struct vertex_info
    {
        KeyWrapper<AbstractTypeInfo> type;
    };

    typedef adjacency_list<vecS, vecS, directedS, vertex_info, edge_info> graph_t;
    typedef graph_traits<graph_t>::vertex_descriptor vertex_t;
    typedef graph_traits<graph_t>::edge_descriptor edge_t;
    typedef std::map<KeyWrapper<AbstractTypeInfo>, vertex_t> vertex_map_t;

    graph_t graph;
    vertex_map_t vertex_map;

    // Mutable is for when the object is logically const, but technically a member needs
    // to sometimes change in a way that's not visible from outside the class.
    // Caching is a perfect example of this.
    mutable std::map<std::pair<KeyWrapper<AbstractTypeInfo>, KeyWrapper<AbstractTypeInfo>>, std::vector<AbstractTypeConverter const*>> conv_cache;

    bool has_type(AbstractTypeInfo const& type) const;

    ExprPtr build_conv_chain(ExprPtr from_expr, std::vector<AbstractTypeConverter const*> const& chain) const;
    std::vector<AbstractTypeConverter const*> search_for_conv(AbstractTypeInfo const& from, AbstractTypeInfo const& to) const;

    // Don't allow TypeConvGraph to be copied accidently.
    TypeConvGraph(TypeConvGraph const&)=delete;
    TypeConvGraph& operator =(TypeConvGraph const&)=delete;

public:
    TypeConvGraph();
    vertex_t  add_type(AbstractTypeInfo const& type);
    void add_conv(AbstractTypeInfo const& from, AbstractTypeInfo const& to, AbstractTypeConverter const* conv);
    ExprPtr wrap_expr(ExprPtr from_expr, AbstractTypeInfo const& from, AbstractTypeInfo const& to) const;
    bool has_conv(AbstractTypeInfo const& from_type, AbstractTypeInfo const& to_type) const;
    void print_graph() const;
};

}}
