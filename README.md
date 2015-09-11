# img_msg
Hide a message in an image

## Setup

1. Install OpenCV

2. Install Cmake

3. Install gcc++ or Clang++

## Hide a message in an image

1. Find an image to use

2. Run the writer

        ./bin/img_msg_writer "Message" /path/to/image.png [ /path/to/output.png ]


** Note that this expects a message in quotes (to allow for spaces). The image can be of any format however a lossless is recommended. The output image must be a lossless format (e.g. png or tiff). Finally in order to ensure a message hidden within a file cannot be easily detected an desirable image to use would be one that is not publicly available or the original image is not available to compare against the image with the hidden message.

## Recovering a message

1. Use the image that was generated from the writer

2. Run the reader

        ./bin/img_msg_reader msg_size /path/to/image.png [ /path/to/output.txt ]

** Note that this expects the estimated or exact size of the image, the path to the lossless image with a hidden message and optionally an output file to place the raw text message (this data could even be data if desired). The message size argument does not have to be exact, if it is larger than the actual message's size the message will show with random data (typically but not always, garbage data).