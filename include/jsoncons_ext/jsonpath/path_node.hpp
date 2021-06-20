﻿// Copyright 2021 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_JSONPATH_PATH_NODE_HPP
#define JSONCONS_JSONPATH_PATH_NODE_HPP

#include <string>
#include <vector>
#include <functional>
#include <jsoncons/config/jsoncons_config.hpp>
#include <jsoncons/detail/write_number.hpp>
#include <jsoncons_ext/jsonpath/jsonpath_error.hpp>

namespace jsoncons { 
namespace jsonpath {
namespace detail { 

    template <class CharT>
    class normalized_path; 

    template <class CharT>
    class path_node 
    {
        friend class normalized_path<CharT>;
    public:
        using char_type = CharT;
        using string_type = std::basic_string<CharT>;
    private:
        enum path_node_kind {root,index,identifier};

        const path_node* parent_;
        path_node_kind node_kind_;
        string_type identifier_;
        std::size_t index_;
    public:
        path_node(char_type c)
            : parent_(nullptr), node_kind_(path_node_kind::root), index_(0)
        {
            identifier_.push_back(c);
        }

        path_node(const path_node* parent, const string_type& identifier)
            : parent_(parent), node_kind_(path_node_kind::identifier), identifier_(identifier), index_(0)
        {
        }

        path_node(const path_node* parent, std::size_t index)
            : parent_(parent), node_kind_(path_node_kind::index), index_(index)
        {
        }

        void swap(path_node& node)
        {
            std::swap(parent_, node.parent_);
            std::swap(node_kind_, node.node_kind_);
            std::swap(identifier_, node.identifier_);
            std::swap(index_, node.index_);
        }

        std::size_t hash() const
        {
            std::size_t h = node_kind_ == path_node_kind::index ? std::hash<std::size_t>{}(index_) : std::hash<string_type>{}(identifier_);
            if (parent_ != nullptr)
            {
                h = h + 17 * parent_>hash();
            }

            return h;
        }

        string_type to_string() const
        {
            string_type buffer;
            to_string(buffer);
            return buffer;
        }

        void to_string(string_type& buffer) const
        {
            if (parent_ != nullptr)
            {
                parent_->to_string(buffer);
            }
            switch (node_kind_)
            {
                case path_node_kind::root:
                    buffer.append(identifier_);
                    break;
                case path_node_kind::identifier:
                    buffer.push_back('[');
                    buffer.push_back('\'');
                    buffer.append(identifier_);
                    buffer.push_back('\'');
                    buffer.push_back(']');
                    break;
                case path_node_kind::index:
                    buffer.push_back('[');
                    jsoncons::detail::from_integer(index_, buffer);
                    buffer.push_back(']');
                    break;
            }
        }

        friend bool operator==(const path_node& lhs, const path_node& rhs) 
        {
            if (&lhs == &rhs)
            {
               return true;
            }

            bool are_equal;
            switch (lhs.node_kind_)
            {
                case path_node_kind::root:
                    are_equal = rhs.node_kind_ == path_node_kind::root && lhs.identifier_ == rhs.identifier_;
                    break;
                case path_node_kind::identifier:
                    are_equal = rhs.node_kind_ == path_node_kind::identifier && lhs.identifier_ == rhs.identifier_;
                    break;
                case path_node_kind::index:
                    are_equal = rhs.node_kind_ == path_node_kind::index && lhs.index_ == rhs.index_;
                    break;
                default:
                    are_equal = false;
                    break;
            }
            if (are_equal)
            {
                if (!(lhs.parent_ == nullptr && rhs.parent_ == nullptr))
                {
                    return *lhs.parent_ == *rhs.parent_;
                }
                else if (lhs.parent_ == nullptr && rhs.parent_ == nullptr)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }

        friend bool operator!=(const path_node& lhs, const path_node& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator<(const path_node& lhs,const path_node& rhs)
        {
            if (&lhs == &rhs)
            {
               return false;
            }

            std::vector<const path_node*> v1;
            {
                const path_node* p = std::addressof(lhs);
                do
                {
                    v1.push_back(p);
                    p = p->parent_;
                }
                while (p != nullptr);
            }

            std::vector<const path_node*> v2;
            {
                const path_node* p = std::addressof(rhs);
                do
                {
                    v2.push_back(p);
                    p = p->parent_;
                }
                while (p != nullptr);
            }

            auto it1 = v1.rbegin();
            auto it2 = v2.rbegin();
            while (it1 != v1.rend() && it2 != v2.rend())
            {
                int diff = (*it1)->compare_node(*(*it2));
                if (diff != 0)
                {
                    return diff < 0;
                }
                ++it1;
                ++it2;
            }
            return v1.size() < v2.size();
        }
    public:
        int compare_node(const path_node& other) const
        {
            bool diff = 0;
            if (node_kind_ != other.node_kind_)
            {
                diff = node_kind_ - other.node_kind_;
            }
            else
            {
                switch (node_kind_)
                {
                    case path_node_kind::root:
                        diff = identifier_.compare(other.identifier_);
                        break;
                    case path_node_kind::index:
                        diff = index_ - other.index_;
                        break;
                    case path_node_kind::identifier:
                        diff = identifier_.compare(other.identifier_);
                        break;
                }
            }
            return diff;
        }

        void node_to_string(string_type& buffer) const
        {
            switch (node_kind_)
            {
                case path_node_kind::root:
                    buffer.append(identifier_);
                    break;
                case path_node_kind::index:
                    buffer.push_back('[');
                    jsoncons::detail::from_integer(index_, buffer);
                    buffer.push_back(']');
                    break;
                case path_node_kind::identifier:
                    buffer.push_back('[');
                    buffer.push_back('\'');
                    buffer.append(identifier_);
                    buffer.push_back('\'');
                    buffer.push_back(']');
                    break;
            }
        }
    };

    template <class CharT>
    class normalized_path
    {
    public:
        using char_type = CharT;
        using string_type = std::basic_string<CharT>;
        using path_node_type = path_node<CharT>;
    private:
        std::vector<const path_node_type*> nodes_;
    public:
        normalized_path(const path_node_type& node)
        {
            {
                const path_node_type* p = std::addressof(node);
                do
                {
                    nodes_.push_back(p);
                    p = p->parent_;
                }
                while (p != nullptr);
            }
        }

        const path_node_type& tail() const
        {
            return *nodes_.front();
        }

        string_type to_string() const
        {
            string_type buffer;

            auto it = nodes_.rbegin();
            while (it != nodes_.rend())
            {
                (*it)->node_to_string(buffer);
                ++it;
            }

            return buffer;
        }

        friend bool operator==(const normalized_path& lhs, const normalized_path& rhs) 
        {
            if (&lhs == &rhs)
            {
               return true;
            }
            if (lhs.nodes_.size() != rhs.nodes_.size())
            {
                return false;
            }
            auto it1 = lhs.nodes_.begin();
            auto it2 = rhs.nodes_.begin();
            while (it1 != lhs.nodes_.end())
            {
                int diff = (*it1)->compare_node(*(*it2));
                if (diff != 0)
                {
                    return false;
                }
                ++it1;
                ++it2;
            }
            return true;
        }

        friend bool operator!=(const normalized_path& lhs, const normalized_path& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator<(const normalized_path& lhs, const normalized_path& rhs) 
        {
            if (&lhs == &rhs)
            {
               return false;
            }

            auto it1 = lhs.nodes_.rbegin();
            auto it2 = rhs.nodes_.rbegin();
            while (it1 != lhs.nodes_.rend() && it2 != rhs.nodes_.rend())
            {
                int diff = (*it1)->compare_node(*(*it2));
                if (diff != 0)
                {
                    return diff < 0;
                }
                ++it1;
                ++it2;
            }
            return lhs.nodes_.size() < rhs.nodes_.size();
        }
    };


} // namespace detail
} // namespace jsonpath
} // namespace jsoncons

#endif
