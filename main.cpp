// Testing the preprocessor

#include <iostream>
#include "includes/preprocessor.hpp"

int main()
{
    using namespace zeta;
    prep::Preprocessor p("test.zasm");
    p.process();
}