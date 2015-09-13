
#include <cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>

#include <string.h>
#include <stdlib.h>

#include <Hider.hpp>

#define MAX_BIT 7
#define MIN_ARGS 2
#define MAX_ARGS 3

// Should be between 1 and 3 (not sure if there are more bits available per pixel)
//#define BIT_PER_PIXEL 3

//using namespace cv;

int main(int argc, char *argv[]) {

    //Skip program name if any
    argc -= (argc > 0);
    argv += (argc > 0);

    unsigned int message_size;
    std::string image_path;
    std::string output_file;

    std::ofstream output;

    if(argc >= MIN_ARGS && argc <= MAX_ARGS) {
            
        // Get input of the estimate of the size of the message.
        message_size = (unsigned int) strtol(argv[0], NULL, 10);

        // Get the location of the image and the image name.
        // TODO reject an image that is not using .png or tiff (or some other lossless image)
        image_path = std::string(argv[1]);

        if(argc == MAX_ARGS) {
            output_file = std::string(argv[2]);
            output.open(output_file);
        }
    } 
    else {
        std::cout << "Invalid Parameters" << std::endl;
        return -1;
    }    

    cv::Mat frame;

    frame = cv::imread(image_path);

    uint8_t* data;

    Hider find;

    int result = find.find_bits(frame, message_size, data);

    std::string message;

    char* bit_msg = (char*) data;

    // Convert the message from a char* to a string
    message = std::string(bit_msg);

    if(result < 0) {
        return -1;
    }

    if(output_file.size() == 0) {
        // No output provide print it out to console
        std::cout << "Message: " << message << std::endl;
    }
    else {
        // TODO test
        // Output message to file if path is given
        output << "Message: " << message << std::endl;
        output.close();
    }

    free(data);

    return 0;
}