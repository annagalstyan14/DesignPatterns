#include "Factory.h"

int main() {
    Factory dessertFactory;

    std::string flavour1 = "Chocolate";
    std::string flavour2 = "Vanilla";

    std::string time1 = "10am";
    std::string time2 = "15pm";

    dessertFactory.makeDessert("Cake", flavour1, time1);

    dessertFactory.makeDessert("Ice Cream", flavour2, time2);

    return 0;
}
