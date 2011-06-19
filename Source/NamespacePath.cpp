// LikeMagic C++ Binding Library
// Copyright 2008-2011 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)


#include "LikeMagic/NamespacePath.hpp"
#include "LikeMagic/Marshaling/NamespaceTypeInfo.hpp"

using namespace LikeMagic;
using namespace LikeMagic::Utility;
using namespace LikeMagic::Marshaling;

namespace LikeMagic {

NamespacePath::NamespacePath()
{
}

NamespacePath::NamespacePath(NamespacePath const& other)
    : path(other.path)
{
}

TypeIndex NamespacePath::get_type() const
{
    // TODO: maybe using a user-readable string is not the most reliable way to make a unique namespace string.
    // Currently this should not be a problem, but might want to investigate having two versions of to_string.
    return NamespaceTypeInfo::create_index(to_string());
}

NamespacePath::NamespacePath(std::vector<std::string> parent_path, std::string name)
    : path(parent_path)
{
    path.push_back(name);
}

NamespacePath::NamespacePath(std::vector<std::string> parent_path, int remove_amt)
    : path(parent_path)
{
    if (!path.empty())
        path.erase(path.end()+remove_amt, path.end());
}

NamespacePath NamespacePath::global()
{
    return NamespacePath();
}

NamespacePath const NamespacePath::subspace(std::string name) const
{
    return NamespacePath(this->path, name);
}

NamespacePath const NamespacePath::get_parent() const
{
    return NamespacePath(this->path, -1);
}

bool NamespacePath::is_root() const
{
    return path.empty();
}

std::string NamespacePath::to_string() const
{
    return (is_root()? "" : get_parent().to_string() + "::") + get_name();
}

std::string NamespacePath::get_name() const
{
    return is_root()? "namespace" : path.back();
}




}
