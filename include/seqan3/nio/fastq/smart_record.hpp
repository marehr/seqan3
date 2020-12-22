#pragma once


template <typename id_field_t, typename sequence_field_t, typename quality_sequence_field_t>
class sequence_record : public id_field_t, public sequence_field_t, public quality_sequence_field_t
{
public:
    template <typename sequence_alphabet_t>
    using sequence_alphabet = sequence_record<id_field_t,
                                              sequence_field<sequence_alphabet_t>,
                                              sequence_quality_field_t>;

    template <typename quality_alphabet_t>
    using quality_alphabet = sequence_record<id_field_t,
                                             sequence_quality_field_t,
                                             quality_sequence_field<quality_alphabet_t>>;

    template <typename ...select_fields_t>
    using select_fields = selected_sequence_record<select_fields_t...>;
};

int main()
{
    sequence_file_input fin{"path"};

    std::ranges::for_each(fin, [] (sequence_record const & record)
    {
        seqan3::debug_stream << record.id() << " " << record.seq() << "\n";
    });

    using custom_sequence_record_t = typename sequence_record::sequence_alphabet<seqan3::dna4>::
                                                               quality_alphabet<seqan3::phred42>;

    std::ranges::for_each(fin, [] (custom_sequence_record_t const & record)
    {
        seqan3::debug_stream << record.id() << " " << record.seq() << "\n";
    });

    // Now also a different type: -> compiler error is expressive: type is not convertible to selected_sequence_record...
    // Also not necessary!
    using selected_sequence_record_t = typename sequence_record::select<id_field, sequence_field>;
    std::ranges::for_each(fin, [] (selected_sequence_record_t const & record)
    {
        seqan3::debug_stream << record.id() << " " << record.seq() << "\n";
    });
}
