#pragma once

namespace seqan3
{

/*!\brief Recursively determines the `value_type` on containers and/or iterators [Type metafunction].
 */
template <typename t>
//!\cond
    requires std::is_same_v<t, void>;
//!\endcond
struct innermost_value_type
{
    //!\brief The return type (recursion not shown).
    using type = value_type_t<remove_cvref_t<t>>;
};

//!\cond
template <typename t>
    requires requires (t)
    {
        typename value_type_t<remove_cvref_t<t>>;
        typename value_type_t<remove_cvref_t<value_type_t<remove_cvref_t<t>>>>;
    }
struct innermost_value_type<t>
{
    using type = typename innermost_value_type<value_type_t<remove_cvref_t<t>>>::type;
};
//!\endcond

/*!\brief Returns the number of times you can call `seqan3::value_type_t` recursively on t [Value metafunction].
 */
template <typename t>
//!\cond
    requires requires (t) { typename value_type_t<remove_cvref_t<t>>; }
//!\endcond
constexpr size_t dimension_v = 1;

//!\cond
template <typename t>
    requires requires (t)
    {
        typename value_type_t<remove_cvref_t<t>>;
        typename value_type_t<value_type_t<remove_cvref_t<t>>>;
    }
constexpr size_t dimension_v<t> = dimension_v<value_type_t<remove_cvref_t<t>>> + 1;
//!\endcond

/*!\interface seqan3::compatible_concept <>
 */
//!\cond
template <typename t1, typename t2>
concept compatible_concept = requires (t1, t2)
{
    requires (dimension_v<t1> == dimension_v<t2>);

    requires std::is_same_v<innermost_value_type_t<t1>, innermost_value_type_t<t2>>;
};
//!\endcond

//!\}

} // namespace seqan3
