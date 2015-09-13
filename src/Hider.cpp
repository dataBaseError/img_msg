#include <Hider.hpp>

/*Hider::Hider() :bit_per_pixel(3) {

}*/

Hider::Hider(unsigned int bit_per_pixel) : bit_per_pixel(bit_per_pixel) {

}


void Hider::get_bit(uint8_t &mesg, uint8_t bits, unsigned int &bit_offset) {

    uint8_t temp = (-(bits << 0) ^ mesg) & (1 << 0);

    //std::cout << "msg " << (int)mesg << " bit " << (int)bits << " Pix " << (int)temp << std::endl;
    mesg ^= temp;

    //std::cout << "msg " << (int)mesg << std::endl;

    bit_offset++;
}

// TODO consider adding the ability to read multiple images that contain a larger message
// Would be necessary to return left over bits (if the writer uses remaining partial bits of images).
int Hider::find_bits(cv::Mat &frame, unsigned int message_size, uint8_t* &data) {
    
    //uint8_t* message_bits = (uint8_t*) malloc (message_size);
    data = (uint8_t*) malloc (message_size);
    unsigned int num_bytes = 0;
    unsigned int bit = 0;
    bool message_end = false;

    // parital bits don't really matter so we can use int division.
    if(message_size > (frame.rows * frame.cols * bit_per_pixel) / 8) {
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

            for(int c = 0; c < bit_per_pixel; c++) {

                // Blue => 0
                // Green => 1
                // Red => 2
                // Read the color bit as necessary.
                get_bit(*data, pixelPtr[i*frame.cols*cn + j*cn + c], bit);

                if(bit == 8) {

                    //std::cout << "Size = " << message_size << std::endl;
                    //std::cout << "num bytes = " << num_bytes << " Mess" << (int)*data << std::endl;
                    // Finished adding bits to the storage
                    bit = 0;
                    num_bytes++;

                    // Increment the storage byte
                    data++;
                }
                else {
                    // Move onto the next bit.
                    *data <<= 1;
                }

                if(num_bytes == message_size) {
                    // Finished reading the last byte of the message
                    data -= num_bytes;
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

    //data = () malloc (message_size);
    //data = (void*) data;

    //free(data);
    return 0;
}

/**
 * Set the rightmost bit of pixels at offset to the bit provided by bits at the bit_offset
 * @param pixels The list of pixel values to be updated
 * @param bits The array of bytes to adjust the pixel value with the current bit defined by bit_offset
 * @param bit_offset The offset of the bit to get from bits.
 */
void Hider::set_bit(uint8_t &pixels, uint8_t* bits, unsigned int &bit_offset) {

    uint8_t temp = (-(*bits >> bit_offset) ^ pixels) & (1 << 0);
    
    pixels ^= temp;
    //std::cout << "msg " << (int)pixels[offset] << " bit " << (int)(*bits >> bit_offset) << " Pix " << (int)temp << std::endl;
}

// Moved the conversion to an unsigned char to outside of the method to make it more general.
// TODO consider allowing for a longer message to be recorded in a sequence of images (video could even be possible)
// Would have to return the position of the message that the was left off at. (aka current_bit)
int Hider::hide_bits(cv::Mat &frame, uint8_t* message_bytes, std::size_t size) {

    unsigned int bit_offset = MAX_BIT;
    unsigned int cur_byte = 0;

    // parital bits don't really matter so we can use int division.
    if(size > (frame.rows * frame.cols * bit_per_pixel) / 8) {
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

            for(int c = 0; c < bit_per_pixel; c++) {

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