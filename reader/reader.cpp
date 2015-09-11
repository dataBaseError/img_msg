
#include <cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>

#include <string.h>
#include <stdlib.h>

#define MAX_BIT 7
#define MIN_ARGS 2
#define MAX_ARGS 3

// Should be between 1 and 3 (not sure if there are more bits available per pixel)
#define BIT_PER_PIXEL 3

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

// TODO consider adding the ability to read multiple images that contain a larger message
// Would be necessary to return left over bits (if the writer uses remaining partial bits of images).
int find_bits(Mat &frame, unsigned int message_size, void* &data) {
    
    uint8_t* message_bits = (uint8_t*) malloc (message_size);
    unsigned int num_bytes = 0;
    unsigned int bit = 0;
    bool message_end = false;

    // parital bits don't really matter so we can use int division.
    if(message_size > (frame.rows * frame.cols * BIT_PER_PIXEL) / 8) {
        std::cout << "Message estimate to long, image would not fit a message that large." << std::endl;
        return -1;
    }

    uint8_t* pixelPtr = (uint8_t*)frame.data;
    int cn = frame.channels();

    for(int i = 0; i < frame.rows; i++)
    {
        for(int j = 0; j < frame.cols; j++)
        {
            //bgrPixel.val[0] = pixelPtr[i*frame.cols*cn + j*cn + 0]; // B
            //bgrPixel.val[1] = pixelPtr[i*frame.cols*cn + j*cn + 1]; // G
            //bgrPixel.val[2] = pixelPtr[i*frame.cols*cn + j*cn + 2]; // R

            for(int c = 0; c < BIT_PER_PIXEL; c++) {

                // Blue => 0
                // Green => 1
                // Red => 2
                // Read the color bit as necessary.
                set_bit(*message_bits, pixelPtr[i*frame.cols*cn + j*cn + c], bit);

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
        if(message_end) {
            break;
        }
    }

    data = malloc (message_size);
    data = (void*) message_bits;

    //free(message_bits);
    return 0;
}

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

    Mat frame;

    frame = imread(image_path);

    void * data;

    int result = find_bits(frame, message_size, data);

    std::string message;

    char* bit_msg = itoc((uint8_t*)data, message_size);

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