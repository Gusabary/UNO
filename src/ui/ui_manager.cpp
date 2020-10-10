#include "ui_manager.h"

namespace UNO { namespace UI {

void UIManager::Render()
{
    std::cout << "\t mPlayerStats: [" << std::endl;
    for (const auto &stat : mPlayerStats) {
        std::cout << "  " << stat << std::endl;
    }
    std::cout << "\t ]" << std::endl;
}

}}