/// Copyright 2021 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_JSONPATH_JSONPATH_ERROR_HPP
#define JSONCONS_JSONPATH_JSONPATH_ERROR_HPP

#include <jsoncons/json_exception.hpp>
#include <system_error>

namespace jsoncons { namespace jsonpath_new {

    enum class jsonpath_errc 
    {
        success = 0,
        expected_root,
        expected_current_node,
        expected_right_bracket,
        expected_key,
        expected_separator,
        expected_forward_slash,
        expected_slice_start,
        expected_slice_end,
        expected_slice_step,
        expected_left_bracket_token,
        expected_minus_or_digit_or_colon_or_comma_or_right_bracket,
        expected_digit_or_colon_or_comma_or_right_bracket,
        expected_minus_or_digit_or_comma_or_right_bracket,
        expected_digit_or_comma_or_right_bracket,
        unexpected_operator,
        invalid_function_name,
        invalid_argument,
        function_name_not_found,
        parse_error_in_filter,
        argument_parse_error,
        unidentified_error,
        unexpected_end_of_input,
        expected_colon_dot_left_bracket_comma_or_right_bracket,
        argument_to_unflatten_invalid,
        invalid_flattened_key,
        step_cannot_be_zero,
        invalid_number,
        illegal_escaped_character,
        invalid_codepoint,
        unknown_function,
        invalid_type,
        unbalanced_parenthesis,
        syntax_error,
        expected_comparator,
        expected_or,
        expected_and
    };

    class jsonpath_error_category_impl
       : public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "jsoncons/jsonpath_new";
        }
        std::string message(int ev) const override
        {
            switch (static_cast<jsonpath_errc>(ev))
            {
                case jsonpath_errc::expected_root:
                    return "Expected $";
                case jsonpath_errc::expected_current_node:
                    return "Expected @";
                case jsonpath_errc::expected_right_bracket:
                    return "Expected ]";
                case jsonpath_errc::expected_key:
                    return "Expected a name or index following the dot";
                case jsonpath_errc::expected_slice_start:
                    return "Expected slice start";
                case jsonpath_errc::expected_slice_end:
                    return "Expected slice end";
                case jsonpath_errc::expected_slice_step:
                    return "Expected slice step";
                case jsonpath_errc::expected_separator:
                    return "Expected dot or left bracket separator";
                case jsonpath_errc::expected_forward_slash:
                    return "Invalid path filter, expected '/'";
                case jsonpath_errc::expected_left_bracket_token:
                    return "Expected ?,',\",0-9,*";
                case jsonpath_errc::expected_minus_or_digit_or_colon_or_comma_or_right_bracket:
                    return "Expected - or 0-9 or : or , or ]";
                case jsonpath_errc::expected_minus_or_digit_or_comma_or_right_bracket:
                    return "Expected - or 0-9 or , or ]";
                case jsonpath_errc::expected_digit_or_comma_or_right_bracket:
                    return "Expected - or 0-9 or , or ]";
                case jsonpath_errc::expected_digit_or_colon_or_comma_or_right_bracket:
                    return "Expected 0-9 or : or , or ]";
                case jsonpath_errc::invalid_function_name:
                    return "Invalid function name";
                case jsonpath_errc::invalid_argument:
                    return "Invalid argument type";
                case jsonpath_errc::function_name_not_found:
                    return "Function name not found";
                case jsonpath_errc::parse_error_in_filter:
                    return "Could not parse JSON expression in a JSONPath filter";
                case jsonpath_errc::argument_parse_error:
                    return "Could not parse JSON expression passed to JSONPath function";
                case jsonpath_errc::unidentified_error:
                    return "Unidentified error";
                case jsonpath_errc::unexpected_end_of_input:
                    return "Unexpected end of jsonpath_new input";
                case jsonpath_errc::expected_colon_dot_left_bracket_comma_or_right_bracket:
                    return "Expected ':', '.', '[', ',', or ']'";
                case jsonpath_errc::argument_to_unflatten_invalid:
                    return "Argument to unflatten must be an object";
                case jsonpath_errc::invalid_flattened_key:
                    return "Flattened key is invalid";
                case jsonpath_errc::step_cannot_be_zero:
                    return "Slice step cannot be zero";
                case jsonpath_errc::invalid_number:
                    return "Invalid number";
                case jsonpath_errc::illegal_escaped_character:
                    return "Illegal escaped character";
                case jsonpath_errc::invalid_codepoint:
                    return "Invalid codepoint";
                case jsonpath_errc::unknown_function:
                    return "Unknown function";
                case jsonpath_errc::invalid_type:
                    return "Invalid type";
                case jsonpath_errc::unbalanced_parenthesis:
                    return "Unbalanced parenthesis";
                case jsonpath_errc::syntax_error:
                    return "Syntax error";
                case jsonpath_errc::expected_comparator:
                    return "Expected comparator";
                case jsonpath_errc::expected_or:
                    return "Expected operator '||'";
                case jsonpath_errc::expected_and:
                    return "Expected operator '&&'";
                default:
                    return "Unknown jsonpath parser error";
            }
        }
    };

    inline
    const std::error_category& jsonpath_error_category()
    {
      static jsonpath_error_category_impl instance;
      return instance;
    }

    inline 
    std::error_code make_error_code(jsonpath_errc result)
    {
        return std::error_code(static_cast<int>(result),jsonpath_error_category());
    }

} // jsonpath_new
} // jsoncons

namespace std {
    template<>
    struct is_error_code_enum<jsoncons::jsonpath_new::jsonpath_errc> : public true_type
    {
    };
}

namespace jsoncons { namespace jsonpath_new {

    class jsonpath_error : public std::system_error, public virtual json_exception
    {
        std::size_t line_number_;
        std::size_t column_number_;
        mutable std::string what_;
    public:
        jsonpath_error(std::error_code ec)
            : std::system_error(ec), line_number_(0), column_number_(0)
        {
        }
        jsonpath_error(std::error_code ec, const std::string& what_arg)
            : std::system_error(ec, what_arg), line_number_(0), column_number_(0)
        {
        }
        jsonpath_error(std::error_code ec, std::size_t position)
            : std::system_error(ec), line_number_(0), column_number_(position)
        {
        }
        jsonpath_error(std::error_code ec, std::size_t line, std::size_t column)
            : std::system_error(ec), line_number_(line), column_number_(column)
        {
        }
        jsonpath_error(const jsonpath_error& other) = default;

        jsonpath_error(jsonpath_error&& other) = default;

        const char* what() const noexcept override
        {
            if (what_.empty())
            {
                JSONCONS_TRY
                {
                    what_.append(std::system_error::what());
                    if (line_number_ != 0 && column_number_ != 0)
                    {
                        what_.append(" at line ");
                        what_.append(std::to_string(line_number_));
                        what_.append(" and column ");
                        what_.append(std::to_string(column_number_));
                    }
                    else if (column_number_ != 0)
                    {
                        what_.append(" at position ");
                        what_.append(std::to_string(column_number_));
                    }
                    return what_.c_str();
                }
                JSONCONS_CATCH(...)
                {
                    return std::system_error::what();
                }
            }
            else
            {
                return what_.c_str();
            }
        }

        std::size_t line() const noexcept
        {
            return line_number_;
        }

        std::size_t column() const noexcept
        {
            return column_number_;
        }
    };

}}

#endif
