/*!\brief Recursively determines the `value_type` on containers and/or iterators [Type metafunction].
 */
template <typename t>
//!\cond
    requires std::is_same_v<t, void>
//!\endcond
struct innermost_value_type
{
    //!\brief The return type (recursion not shown).
    using type = value_type_t<remove_cvref_t<t>>;
};
