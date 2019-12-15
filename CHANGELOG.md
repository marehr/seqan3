# Changelog {#about_changelog}

[TOC]

This changelog contains a top-level entry for each release with sections on new features, API changes and notable
bug-fixes (not all bug-fixes will be listed).
See the documentation on [API stability](https://docs.seqan.de/seqan/3-master-user/about_api.html) to learn about
when API changes are allowed.

<!--
The following API changes should be documented as such:
  * a previously experimental interface now being marked as stable
  * an interface being removed
  * syntactical changes to an interface (e.g. renaming or reordering of files, functions, parameters)
  * semantical changes to an interface (e.g. a function's result is now always one larger) [DANGEROUS!]

If possible, provide tooling that performs the changes, e.g. a shell-script.
-->

# 3.0.1

## New features

### CMake

* `find_package(sEqAn3)` is now case-insensitive and always populates `SEQAN3_*` variables in all upper-case.

#### Argument parser
* Simplified reading file extensions from formatted files in the input/output file validators.
* The seqan3::value_list_validator is now constructible from a range or a parameter pack.
* Enable subcommand argument parsing ([How-to](https://docs.seqan.de/seqan/3-master-user/subcommand_arg_parse.html)).

#### Core
* Added traits for "metaprogramming" with `seqan3::type_list` and type packs.

#### Input/Output

* Asynchronous input (background file reading) supported via seqan3::view::async_input_buffer.
* Reading field::CIGAR into a vector over seqan3::cigar is supported via seqan3::alignment_file_input.
* Writing field::CIGAR into a vector over seqan3::cigar is supported via seqan3::alignment_file_output.

## API changes

* **Customising for third party types has changes slightly:**
  You are only affected if you added types to `seqan3::custom::`.
  Please see [About Customisation](http://docs.seqan.de/seqan/3-master-user/about_customisation.html).

### CMake

* [find_package](https://cmake.org/cmake/help/latest/command/find_package.html#version-selection) now accepts minimum
  versions (e.g. `find_package(SEQAN3 3.1.0)` requires at least seqan3 with a version of `>= 3.1.0` and `< 4.0.0`).
* `SEQAN3_VERSION_STRING` was renamed to `SEQAN3_VERSION` (`find_package(SEQAN3)`).

#### Argument parser

* The seqan3::value_list_validator is not constructible from a std::initialiser_list any more
  (e.g. `seqan3::value_list_validator{{1,2,3}}` does **not** work, use `seqan3::value_list_validator{1,2,3}` instead).
* **Changed class signature of input/output file validators:**
  Most user code will be unaffected; to fix possible compiler errors you need to add an empty template parameter list to
  the respective instances (e.g. change `input_file_validator` to `input_file_validator<>`).

#### Core

* **The `type_list` header has moved:**
  If you included `<seqan3/core/type_list.hpp>` you need to change the path to `<seqan3/core/type_list/type_list.hpp>`.

#### Range

* **The `seqan3::concatenated_sequences::data()` function has been deprecated:**
  Use `seqan3::concatenated_sequences::raw_data()` instead.

#### Search

* **Changed class signature of (bi_)fm_index:**
  All code that relies on automatic template deduction will be unaffected. In case you specified the template parameters
  of a `seqan3::fm_index` or `seqan3::bi_fm_index` you will need to add the alphabet type as first parameter and pass a
  `seqan3::text_layout` instead of a `bool` to indicate the text layout (single, collection).
  For example, `fm_index<false> index{text}` where `text` is of type `dna4_vector` needs to be changed to
  `fm_index<dna4, text_layout::single> index{text}`.

* **The `construct()` method of the (bi_)fm_index is now private:**
  Use the constructor `seqan3::fm_index::fm_index(text_t && text)` or `seqan3::bi_fm_index::bi_fm_index(text_t && text)`
  instead.

## Notable Bug-fixes

* Copying and moving the `seqan3::fm_index` and `seqan3::bi_fm_index` now work properly.
* Searching in the `seqan3::fm_index` and `seqan3::bi_fm_index` constructed from a text collection containing a single
  text now return correct result.

# 3.0.0 ("Escala")

Initial release of SeqAn3.
