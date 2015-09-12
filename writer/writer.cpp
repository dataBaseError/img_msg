
#include <cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <string.h>

#include <stdlib.h>

#define MAX_BIT 7
#define MIN_ARGS 2
#define MAX_ARGS 3

// Should be between 1 and 3 (not sure if there are more bits available per pixel)
#define BIT_PER_PIXEL 3

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
}

// Moved the conversion to an unsigned char to outside of the method to make it more general.
// TODO consider allowing for a longer message to be recorded in a sequence of images (video could even be possible)
// Would have to return the position of the message that the was left off at. (aka current_bit)
int hide_bits(Mat &frame, uint8_t* message_bytes, std::size_t size) {

    unsigned int bit_offset = MAX_BIT;
    unsigned int cur_byte = 0;

    // parital bits don't really matter so we can use int division.
    if(size > (frame.rows * frame.cols * BIT_PER_PIXEL) / 8) {
        std::cout << "Message to long, image will not fit the message." << std::endl;
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

                // Change the color bit as necessary.
                set_bit(pixelPtr[i*frame.cols*cn + j*cn + c], message_bytes, bit_offset);

                if(bit_offset == 0) {
                    // Move to the next character
                    //std::cout << "num bytes = " << cur_byte << " Mess" << (int)*message_bytes << std::endl;
                    cur_byte++;
                    bit_offset = MAX_BIT;

                    message_bytes++;
                }
                else {
                    // Move to the next bit
                    bit_offset--;
                }

                if(cur_byte == size) {
                    // No more bits to write out leave.
                    return 0;
                }
            }
        }
    }
    return 0;   
}

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

    Mat frame;
    Mat original;

    frame = imread(image_path);

    frame.copyTo(original);   

    imshow("Original", original);

    // Convert the message to a unsigned char array.
    // This 'serializes' the data to allow for it to be encoded.
    uint8_t * msg_chr = (uint8_t *) message.c_str();

    // Using message.size() instead of sizeof(msg_chr) prevents \0 from being written into the image.
    int result = hide_bits(frame, msg_chr, message.size());

    if(result < 0) {
        return -1;
    }

    // Can also be a .tiff
    imwrite(output_dir, frame);

    imshow("Display window", frame);
    waitKey(0);

    return 0;
}