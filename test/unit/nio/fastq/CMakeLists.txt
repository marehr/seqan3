seqan3_test(fastq_file_input_test.cpp)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/hello.fastq" "${CMAKE_CURRENT_BINARY_DIR}/hello.fastq" COPYONLY)
