/* ===================================================================
**
** Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: main.cpp
** DESC: Entry class for the program to start
** ================================================================ */

#include <cstdlib>        // EXIT_SUCCESS/EXIT_FAILURE
#include <exception>      // std::exception
#include "Parser.h"

using namespace sensors_stability;

int main(int argc, char* argv[])
{
    if(argc < 2) {
        std::cout << "Missing argument. Require test case file. Type -h ";
        std::cout << "or -help for more details " << std::endl;
        return(EXIT_FAILURE);
    }

    std::string argv1 = argv[1];
    if(argv1 == "-h" || argv1 == "-help") {
      std::string program_name = "see_stability";
      std::string input_file_name = " <path_to_test_case_file>/see_stability.txt";

      std::cout << "usage: adb shell " << program_name << input_file_name
                << std::endl;
      std::cout << "Example:" << "adb shell see_stability ";
      std::cout << "/data/ClientAPI/stability/accel_stream_max_rate_1mt/";
      std::cout << "see_stability.txt";

       return(EXIT_SUCCESS);
    }
    if(argv1 == "-v" || argv1 == "-version") {
        std::cout << "see_stability v1.2" << std::endl;
        return(EXIT_SUCCESS);
    }
    std::cout << "see_stability v1.2" << std::endl;
    try {
        Parser::command_line_args(argc, argv);
    }
    catch(std::exception const& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch(const char* ex) {
        std::cerr << "Exception message: " << ex << std::endl;
    }
    catch(...) { // Catch all exception handler
        std::cerr << "Caught exception of undetermined type" << std::endl;
    }
    return 0;
}
///////////////////////////////////////////////////////////////////////////////
