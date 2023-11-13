#include <chrono>
#include <iostream>

#include "cluon-complete.hpp"
#include "messages.hpp"


int32_t main(int32_t , char **)
{
	// cluon::OD4Session od4(132,[&od4](cluon::data::Envelope &&envelope) noexcept
	// {		
	// 	if(envelope.dataType() == 1091)
	// 	{
	// 		opendlv::proxy::GroundSpeedRequest receivedMsg = cluon::extractMessage<opendlv::proxy::GroundSpeedRequest>(std::move(envelope));
	// 		float val = receivedMsg.groundSpeed();
	// 		// opendlv::proxy::GroundSpeedReading msg;
	// 		// msg.groundSpeed(val);
	// 		// od4.send(msg);
	// 		std::cout << val;

	// 	}
	// });

	cluon::OD4Session od4(132,[&od4](cluon::data::Envelope &&envelope) noexcept
	{		
		if(envelope.dataType() == 2001)
		{
			calValues receivedMsg = cluon::extractMessage<calValues>(std::move(envelope));
			std::cout<<"Received number is "<< receivedMsg.m00() << std::endl;
			std::cout<<"Received number is "<< receivedMsg.m01() << std::endl;
			std::cout<<"Received number is "<< receivedMsg.m02() << std::endl;
			std::cout<<"Received number is "<< receivedMsg.m10() << std::endl;
			std::cout<<"Received number is "<< receivedMsg.m11() << std::endl;
			std::cout<<"Received number is "<< receivedMsg.m12() << std::endl;
			std::cout<<"Received number is "<< receivedMsg.m20() << std::endl;
			std::cout<<"Received number is "<< receivedMsg.m21() << std::endl;
			std::cout<<"Received number is "<< receivedMsg.m22() << std::endl;
		}
	});

	while(true){}


	return 0;
}
