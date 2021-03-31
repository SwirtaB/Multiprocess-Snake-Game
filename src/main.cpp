#include <opencv4/opencv2/opencv.hpp>
#include <iostream>

int main(){
    cv::Mat frame;
    //VIDEOCAPTURE INITIALIZE
    cv::VideoCapture capture;
    capture.open(0);    //DEFAULT API INIT

    if(!capture.isOpened()){
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    std::cout << "Press any key to terminate" << std::endl;
    while(true){
        //waits for a new farme from camera and store it into 'frame'
        capture.read(frame);
        if(frame.empty()){
            std::cerr << "ERROR! Bland frame grabbed\n";
            break;
        }
        //shows camera feed and waits with timeout
        cv::imshow("Live", frame);
        if(cv::waitKey(1) >= 0)
            break;
    }

    return 0;
}