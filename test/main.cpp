#include <gtest/gtest.h>
#include "../src/common/common.h"

int main(int argc, char **argv) {
    UNO::Common::Common::mPlayerNum = 3;
    UNO::Common::Common::mTimeoutPerTurn = 15;
    UNO::Common::Common::mHandCardsNumPerRow = 8;
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}