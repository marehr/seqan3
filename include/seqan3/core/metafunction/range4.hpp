/*!\brief Returns the number of times you can call `seqan3::value_type_t` recursively on t [Value metafunction].
 */
template <typename t>
//!\cond
    requires requires (t) { typename value_type_t<remove_cvref_t<t>>; }
//!\endcond
constexpr size_t dimension_v = 1;
