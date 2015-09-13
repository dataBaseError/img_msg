
#include <cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <string.h>

#include <stdlib.h>
#include <Hider.hpp>

#define MIN_ARGS 2
#define MAX_ARGS 3

int main(int argc, char *argv[]) {

    //Skip program name if any
    argc -= (argc > 0);
    argv += (argc > 0);

    std::string message;
    std::string image_path;
    std::string output_dir = "./image.png";

    if(argc >= MIN_ARGS && argc <= MAX_ARGS) {
            
        // Get input of the message.
        message = std::string(argv[0]);

        // Get the location of the image and the image name.
        image_path = std::string(argv[1]);

        if(argc == MAX_ARGS) {
            // TODO check if this image/path is valid.
            // TODO reject image name that is not .png or tiff.
            output_dir = std::string(argv[2]);
        }
    } 
    else {
        std::cout << "Invalid Parameters" << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::Mat original;

    frame = cv::imread(image_path);

    frame.copyTo(original);   

    cv::imshow("Original", original);

    Hider hider(3);

    // Convert the message to a unsigned char array.
    // This 'serializes' the data to allow for it to be encoded.
    uint8_t * msg_chr = (uint8_t *) message.c_str();

    // Using message.size() instead of sizeof(msg_chr) prevents \0 from being written into the image.
    int result = hider.hide_bits(frame, msg_chr, message.size());

    if(result < 0) {
        return -1;
    }

    // Can also be a .tiff
    cv::imwrite(output_dir, frame);

    cv::imshow("Display window", frame);
    cv::waitKey(0);

    return 0;
}