#ifndef MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_APPLICATIONMAIN_HPP
#define MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_APPLICATIONMAIN_HPP

#ifdef M_EXECUTABLE
#   define M_APPLICATION_MAIN_DEFINITION extern
#else
#   define M_APPLICATION_MAIN_DEFINITION
#endif

namespace Merrie {

    /**
     * Entry point of a Merrie application
     */
    M_APPLICATION_MAIN_DEFINITION int ApplicationMain(int argc, char* argv[]);
}

#ifdef M_EXECUTABLE
int main(int argc, char* argv[]) {
    return Merrie::ApplicationMain(argc, argv);
}
#endif

#endif //MERRIE_COMMONS_HEADERS_INCLUDES_COMMONS_APPLICATIONMAIN_HPP
