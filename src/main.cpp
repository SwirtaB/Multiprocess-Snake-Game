#include "../include/ImageProcessing.hpp"
#include "../include/CaptureVideo.hpp"

using namespace cv;
using namespace std;

int main(){
    Capture processA = Capture();
    ImageProcessing processB = ImageProcessing();
    while(true){
        //processA.run();
        processB.run();
        break;
    }

    return 0;
}