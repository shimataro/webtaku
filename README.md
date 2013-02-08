# Setup Instructions

## install the required packages
`sudo apt-get install -y xvfb libqtwebkit-dev libicu-dev`

## install MS fonts and Japanese fonts
`sudo apt-get install -y ttf-mscorefonts-installer ttf-umefont`

## build & install
`git clone https://github.com/shimataro/wiget.git`  
`cd wiget`  
`qmake`  
`make`  
`sudo make install`  
(will be installed in /usr/local/bin and /usr/local/src)

---

# Usage

## create image

### xvfb is only necessary for running wiget on a headless server (i.e. a server without a GUI or even a graphics card)

`xvfb-run --server-args="-screen 0, 1024x768x24" wiget --output=output.png "http://example.com/"`

### if "--output" option is omitted, image data will be dumped into stdout

`wiget --format=JPG "http://example.com/" >output.jpg`

### if "--format" and "--output" options are omitted, file format will be "PPM"

`wiget "http://example.com/" >output.ppm`

### you can specify minimum viewport size (default is 1024x768)

`wiget --min-size=1200x900 "http://example.com/" >output.ppm`  
`wiget --min-size=1200x "http://example.com/" >output.ppm`  
`wiget --min-size=x900 "http://example.com/" >output.ppm`  

### you can resize image as below

`wiget --scaled-size=320x240 "http://example.com/" >output.ppm`  
`wiget --scaled-size=320x "http://example.com/" >output.ppm`  
`wiget --scaled-size=x240 "http://example.com/" >output.ppm`  

### you can crop image by minimum viewport size

`wiget --crop "http://example.com/" >output.ppm`

### you can resize and crop as below

`wiget --min-size=1200x900 --crop --scaled-size=320x240 "http://example.com/" >output.ppm`

### for more complex conversion, use ImageMagick as below

`wiget --min-size=1200x900 "http://example.com/" | convert -rotate +90 -resize 320x320! - output.ppm`

### you can use "silent mode" if you feel noisy about message

`wiget --silent "http://example.com/" >output.ppm`

### iPad screenshot
`wiget http://example.com/ --user-agent="Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A403 Safari/8536.25" --min-width=1024 --min-height=768 >output.ppm`

