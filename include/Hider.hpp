
#ifndef HIDER_HPP_
#define HIDER_HPP_

#include <cv.hpp>
#include <opencv2/core/core.hpp>

#include <string.h>

#include <stdlib.h>

#define MAX_BIT 7

class Hider
{

    unsigned int bit_per_pixel;

public:

    //Hider();

    Hider(unsigned int bit_per_pixel=3);

    int hide_bits(cv::Mat &frame, uint8_t* message_bytes, std::size_t size);

    int find_bits(cv::Mat &frame, unsigned int message_size, uint8_t* &data);

private:

    void get_bit(uint8_t &mesg, uint8_t bits, unsigned int &bit_offset);

    void set_bit(uint8_t &pixels, uint8_t* bits, unsigned int &bit_offset);

};


#endif /* HIDER_HPP_ */