#include "inputter.h"

namespace UNO { namespace UI {

std::string Inputter::GetAction()
{
    char inputBuffer[10];
    std::cout << "Now it's your turn." << std::endl;
    std::cout << "Input (D)raw, (S)kip or (P)lay <card_index>:" << std::endl;
    std::cin.getline(inputBuffer, 10);
    return std::string{inputBuffer};
}

char Inputter::SpecifyNextColor()
{
    char inputBuffer[10];
    std::cout << "Specify the next color (R/Y/G/B): " << std::endl;
    std::cin.getline(inputBuffer, 10);
    return *inputBuffer;
}

}}