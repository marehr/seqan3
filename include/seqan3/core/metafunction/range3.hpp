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
