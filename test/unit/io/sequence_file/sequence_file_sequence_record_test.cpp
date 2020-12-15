// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/alphabet/quality/phred42.hpp>
#include <seqan3/core/detail/debug_stream_alphabet.hpp>
#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/test/expect_range_eq.hpp>

using seqan3::operator""_dna4;

// ----------------------------------------------------------------------------
// record
// ----------------------------------------------------------------------------

struct record : public ::testing::Test
{
    using types        = seqan3::type_list<std::string, seqan3::dna4_vector>;
    using types_as_ids = seqan3::fields<seqan3::field::id, seqan3::field::seq>;
    using record_type  = seqan3::sequence_record<types, types_as_ids>;
};

TEST_F(record, definition_tuple_traits)
{
    EXPECT_TRUE((std::is_same_v<typename record_type::base_type,
                                std::tuple<std::string, seqan3::dna4_vector>>));

    EXPECT_TRUE((std::is_same_v<std::tuple_element_t<0, record_type>,
                                std::string>));
    EXPECT_TRUE((std::is_same_v<std::tuple_element_t<1, record_type>,
                                seqan3::dna4_vector>));
    EXPECT_EQ(std::tuple_size_v<record_type>, 2ul);

    EXPECT_TRUE(seqan3::tuple_like<record_type>);
}

TEST_F(record, construction)
{
    [[maybe_unused]] record_type r{"MY ID", "ACGT"_dna4};
}

TEST_F(record, get_by_index)
{
    record_type r{"MY ID", "ACGT"_dna4};

    EXPECT_EQ(std::get<0>(r), "MY ID");
    EXPECT_RANGE_EQ(std::get<1>(r), "ACGT"_dna4);
}

TEST_F(record, get_by_type)
{
    record_type r{"MY ID", "ACGT"_dna4};

    EXPECT_EQ(std::get<std::string>(r), "MY ID");
    EXPECT_RANGE_EQ(std::get<seqan3::dna4_vector>(r), "ACGT"_dna4);
}

TEST_F(record, get_by_field)
{
    record_type r{"MY ID", "ACGT"_dna4};

    EXPECT_EQ(seqan3::get<seqan3::field::id>(r), "MY ID");
    EXPECT_RANGE_EQ(seqan3::get<seqan3::field::seq>(r), "ACGT"_dna4);
}

TEST_F(record, get_by_member)
{
    record_type r{"MY ID", "ACGT"_dna4};

    EXPECT_EQ(r.id(), "MY ID");
    EXPECT_RANGE_EQ(r.sequence(), "ACGT"_dna4);
}

TEST_F(record, get_types)
{
    record_type r{"MY ID", "ACGT"_dna4};

    static_assert((std::same_as<decltype(seqan3::get<seqan3::field::id>(r)),
                                std::string &>));
    static_assert((std::same_as<decltype(seqan3::get<seqan3::field::seq>(r)),
                                seqan3::dna4_vector &>));

    static_assert((std::same_as<decltype(seqan3::get<seqan3::field::id>(std::as_const(r))),
                                std::string const &>));
    static_assert((std::same_as<decltype(seqan3::get<seqan3::field::seq>(std::as_const(r))),
                                seqan3::dna4_vector const &>));

    static_assert((std::same_as<decltype(seqan3::get<seqan3::field::id>(std::move(r))),
                                std::string &&>));
    static_assert((std::same_as<decltype(seqan3::get<seqan3::field::seq>(std::move(r))),
                                seqan3::dna4_vector &&>));

    static_assert((std::same_as<decltype(seqan3::get<seqan3::field::id>(std::move(std::as_const(r)))),
                                std::string const &&>));
    static_assert((std::same_as<decltype(seqan3::get<seqan3::field::seq>(std::move(std::as_const(r)))),
                                seqan3::dna4_vector const &&>));
}

TEST_F(record, member_types)
{
    record_type r{"MY ID", "ACGT"_dna4};

    static_assert((std::same_as<decltype(r.id()), std::string &>));
    static_assert((std::same_as<decltype(r.sequence()), seqan3::dna4_vector &>));

    static_assert((std::same_as<decltype(std::as_const(r).id()), std::string const &>));
    static_assert((std::same_as<decltype(std::as_const(r).sequence()), seqan3::dna4_vector const &>));

    static_assert((std::same_as<decltype(std::move(r).id()), std::string &&>));
    static_assert((std::same_as<decltype(std::move(r).sequence()), seqan3::dna4_vector &&>));

    static_assert((std::same_as<decltype(std::move(std::as_const(r)).id()), std::string const &&>));
    static_assert((std::same_as<decltype(std::move(std::as_const(r)).sequence()), seqan3::dna4_vector const &&>));
}
