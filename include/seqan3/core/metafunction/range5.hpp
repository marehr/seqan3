//!\cond
template <typename t>
    requires requires (t)
    {
        typename value_type_t<remove_cvref_t<t>>;
        typename value_type_t<value_type_t<remove_cvref_t<t>>>;
    }
constexpr size_t dimension_v<t> = dimension_v<value_type_t<remove_cvref_t<t>>> + 1;
//!\endcond
