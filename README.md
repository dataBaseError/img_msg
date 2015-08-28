# img_msg
Hide a message in an image

## Setup

1. Install OpenCV

2. Install Cmake

3. Install gcc++ or Clang++

## Hide a message in an image

1. Find an image to use

2. Run the writer

        ./bin/img_msg_writer "Message" /path/to/image.png

## Recovering a message

1. Use the image that was generated from the writer

2. Run the reader

        ./bin/img_msg_reader msg_size /path/to/image.png