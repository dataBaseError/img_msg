
#include <cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <string.h>

#include <stdlib.h>

#define MAX_BIT 7
#define MAX_ARGS 2

using namespace cv;

/**
 * Set the rightmost bit of pixels at offset to the bit provided by bits at the bit_offset
 * @param pixels The list of pixel values to be updated
 * @param bits The array of bytes to adjust the pixel value with the current bit defined by bit_offset
 * @param bit_offset The offset of the bit to get from bits.
 */
void set_bit(uint8_t &pixels, uint8_t* bits, unsigned int &bit_offset) {

    uint8_t temp = (-(*bits >> bit_offset) ^ pixels) & (1 << 0);
    
    pixels ^= temp;
    //std::cout << "msg " << (int)pixels[offset] << " bit " << (int)(*bits >> bit_offset) << " Pix " << (int)temp << std::endl;

    //return pixels;
}

/**
 * Covert an array of characters to an array of uint8_t
 * @param c The input character array to be converted
 * @param size The size of the character array.
 *
 * @return The converted uint8_t array.
 */
uint8_t* ctoi(const char *c, unsigned int size) {

    uint8_t* result = (uint8_t*) malloc (size);
    unsigned int i = 0;
    while(i < size) {
        *result = (uint8_t) *c;

        //std::cout << "Char " << *c << " int = " << (int)*result << std::endl;
        c++;
        result++;
        i++;
    }
    result-=i;

    return result;
}

int main(int argc, char *argv[]) {

    //Skip program name if any
    argc -= (argc > 0);
    argv += (argc > 0);

    std::string message;
    std::string image_path;

    //TODO output directory/name

    if(argc == MAX_ARGS) {
            
        // Get input of the message.
        message = std::string(argv[0]);

        // Get the location of the image and the image name.
        image_path = std::string(argv[1]);
    } 
    else {
        std::cout << "Invalid Parameters" << std::endl;
        return -1;
    }

    uint8_t* mess_bits = ctoi(message.c_str(), message.size());

    unsigned int bit_offset = MAX_BIT;
    unsigned int bit = 0;
    bool message_end = false;

    Mat frame;
    Mat original;

    frame = imread(image_path);
    frame.copyTo(original);

    uint8_t* pixelPtr = (uint8_t*)frame.data;
    int cn = frame.channels();

    imshow("Original", original);

    for(int i = 0; i < frame.rows; i++)
    {
        for(int j = 0; j < frame.cols; j++)
        {
            //bgrPixel.val[0] = pixelPtr[i*frame.cols*cn + j*cn + 0]; // B
            //bgrPixel.val[1] = pixelPtr[i*frame.cols*cn + j*cn + 1]; // G
            //bgrPixel.val[2] = pixelPtr[i*frame.cols*cn + j*cn + 2]; // R

            // Blue
            set_bit(pixelPtr[i*frame.cols*cn + j*cn + 0], mess_bits, bit_offset);

            if(bit_offset == 0) {
                // Move to the next character
                //std::cout << "num bytes = " << bit << " Mess" << (int)*mess_bits << std::endl;
                bit++;
                bit_offset = MAX_BIT;

                mess_bits++;
            }
            else {
                // Move to the next bit
                bit_offset--;
            }

            if(bit == message.size()) {
                // No more bits to write out leave.
                message_end = true;
                break;
            }
        }
        if(message_end) {
            break;
        }
    }

    // Can also be a .tiff
    imwrite("image.png", frame);

    imshow("Display window", frame);
    waitKey(0);

    return 0;
}