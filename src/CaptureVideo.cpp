//
// Created by swirta on 02.05.2021.
//
#include "../include/CaptureVideo.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void Capture::run() {
    camera.open(0);
    if(!camera.isOpened())
        return;

    camera.read(rawFrame);
    if(rawFrame.empty()){
        cerr << "Frame empty" << endl;
        return;
    }

}