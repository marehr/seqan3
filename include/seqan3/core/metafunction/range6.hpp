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
