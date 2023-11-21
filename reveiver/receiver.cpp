#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
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

            std::cout << "Camera Id:" << receivedMsg.cameraId() << std::endl;

            cv::Mat receivedMatrix = (cv::Mat_<double>(3, 3) << receivedMsg.m00(), receivedMsg.m01(), receivedMsg.m02(),
                                      receivedMsg.m10(), receivedMsg.m11(), receivedMsg.m12(),
                                      receivedMsg.m20(), receivedMsg.m21(), receivedMsg.m22());

            std::cout << "Received Camera Matrix:\n" << receivedMatrix << "\n";

            cv::Mat receivedVector = (cv::Mat_<double>(1, 5) << receivedMsg.d0(), receivedMsg.d1(), receivedMsg.d2(), receivedMsg.d3(), receivedMsg.d4());

            std::cout << "Received Distortion Vector:\n" << receivedVector << "\n";
        }
    });

	while(true){}


	return 0;
}
