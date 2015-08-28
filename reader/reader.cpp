

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
 * Retrieve the right most bit from the current bits value and store it in mesg.
 * @param mesg Set the rightmost bit to that of bits current value.
 * @param bits The value to retrieve the rightmost bit from.
 * @param bit_offset The offset used to keep track of the which bit the rightmost bit will be moved to.
 */
void set_bit(uint8_t &mesg, uint8_t bits, unsigned int &bit_offset) {

    uint8_t temp = (-(bits << 0) ^ mesg) & (1 << 0);

    //std::cout << "msg " << (int)mesg << " bit " << (int)bits << " Pix " << (int)temp << std::endl;
    mesg ^= temp;

    //std::cout << "msg " << (int)mesg << std::endl;

    bit_offset++;
}

/**
 * Convert an array of uint8_t  to a character array
 * @param in The initial uint8_t* to be converted to a character array.
 * @param size The size of the array.
 *
 * @return the coverted array of characters.
 */
char* itoc(uint8_t *in, unsigned int size) {

    char* result = (char*) malloc (size+1);
    unsigned int i = 0;
    while(i < size) {
        *result = (char) *in;

        //std::cout << "int " << (int)*in << " char = " << *result << std::endl;
        in++;
        result++;
        i++;
    }
    *result = '\0';

    result-=i;

    return result;
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

        //std::cout << "Char " << *c << " int = " << *result << std::endl;
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

    unsigned int message_size;
    std::string image_path;

    //TODO output directory/name for text

    if(argc == MAX_ARGS) {
            
        // Get input of the estimate of the size of the message.
        message_size = (unsigned int) strtol(argv[0], NULL, 10);

        // Get the location of the image and the image name.
        image_path = std::string(argv[1]);
    } 
    else {
        std::cout << "Invalid Parameters" << std::endl;
        return -1;
    }
    
    uint8_t* message_bits = (uint8_t*) malloc (message_size);
    unsigned int num_bytes = 0;
    unsigned int bit = 0;

    bool message_end = false;  

    Mat frame;

    frame = imread(image_path);

    uint8_t* pixelPtr = (uint8_t*)frame.data;
    int cn = frame.channels();

    for(int i = 0; i < frame.rows; i++)
    {
        for(int j = 0; j < frame.cols; j++)
        {
            //bgrPixel.val[0] = pixelPtr[i*frame.cols*cn + j*cn + 0]; // B
            //bgrPixel.val[1] = pixelPtr[i*frame.cols*cn + j*cn + 1]; // G
            //bgrPixel.val[2] = pixelPtr[i*frame.cols*cn + j*cn + 2]; // R
            
            // Change the Blue rgb value if necessary
            set_bit(*message_bits, pixelPtr[i*frame.cols*cn + j*cn + 0], bit);

            if(bit == 8) {

                //std::cout << "Size = " << message_size << std::endl;
                //std::cout << "num bytes = " << num_bytes << " Mess" << (int)*message_bits << std::endl;
                // Finished adding bits to the storage
                bit = 0;
                num_bytes++;

                // Increment the storage byte
                message_bits++;
            }
            else {
                // Move onto the next bit.
                *message_bits <<= 1;
            }

            if(num_bytes == message_size) {
                // Finished reading the last byte of the message
                message_bits -= num_bytes;
                message_end = true;
                break;
            }
        }
        if(message_end) {
            break;
        }
    }

    // Convert the message from a uint8_t to a string
    std::string val = std::string(itoc(message_bits, message_size));

    std::cout << "Message " << val << std::endl;

    return 0;
}