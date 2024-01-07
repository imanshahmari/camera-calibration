#include <chrono>
#include <iostream>
#include <vector>


#include "cluon-complete.hpp"
#include "messages.hpp"


int32_t main(int32_t , char **)
{

    cluon::OD4Session od4(132, [&od4](cluon::data::Envelope &&envelope) noexcept
    {
        if (envelope.dataType() == 2001)
        {
            calValues receivedMsg = cluon::extractMessage<calValues>(std::move(envelope));

            const std::string &rStr = receivedMsg.r();
            const std::string &tStr = receivedMsg.t();

            std::vector<double> receivedRotation(
                reinterpret_cast<const double*>(rStr.data()),             
                reinterpret_cast<const double*>(rStr.data() + rStr.size())
            );

            std::vector<double> receivedTranslation(
                reinterpret_cast<const double*>(tStr.data()),             
                reinterpret_cast<const double*>(tStr.data() + tStr.size())
            );

            std::cout << "Received Rotation Matrix:\n";
            for (const auto &value : receivedRotation) {
                std::cout << value << " ";
            }
            std::cout << "\n";

            std::cout << "Received Translation Vector:\n";
            for (const auto &value : receivedTranslation) {
                std::cout << value << " ";
            }
            std::cout << "\n";
        }
    });

	while(true){}


	return 0;
}
