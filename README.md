# Setup Instructions

## install the required packages
`sudo apt-get install -y libqtwebkit-dev libicu-dev`

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

## basic usage

### dump image data into stdout, and default file format is PPM
`wiget "http://example.com/" >output.ppm`  

### "--format" to specify file format
(BMP|JPG|PNG|PPM|XBM|XPM)  
`wiget --format=JPG "http://example.com/" >output.jpg`  

### "--output" to specify output file
(file format will be determined by extension, and "--format" option will be ignored)  
`wiget --output=output.png "http://example.com/"`  

### "--min-size" to specify minimum viewport size (default is 1024x768)
`wiget --min-size=1200x800 "http://example.com/" >output.ppm`  
`wiget --min-size=1200x "http://example.com/" >output.ppm`  
`wiget --min-size=x800 "http://example.com/" >output.ppm`  

### "--scaled-size" to resize image as below
`wiget --scaled-size=320x240 "http://example.com/" >output.ppm`  
`wiget --scaled-size=320x "http://example.com/" >output.ppm`  
`wiget --scaled-size=x240 "http://example.com/" >output.ppm`  

### upper case "X" to resize by long-side
`wiget --scaled-size=320X240 "http://example.com/" >output.ppm`  

### "--crop" to crop image by minimum viewport size
`wiget --crop "http://example.com/" >output.ppm`  

### "--silent" to output no message
`wiget --silent "http://example.com/" >output.ppm`  

### "--user-agent" to specify user agent
(example: iPad screenshot)  
`wiget http://example.com/ --user-agent="Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A403 Safari/8536.25" --min-width=1024 --min-height=768 >output.ppm`  

## with external tools

### when X server is not running, use virtual frame buffer
(use "--output" option because xvfb-run redirects stderr to stdout)  
`sudo apt-get install -y xvfb`  
`xvfb-run --server-args="-screen 0, 1024x768x24" wiget --output=output.png "http://example.com/"`  

### for more complex conversion, use ImageMagick as below
`wiget --min-size=1200x900 "http://example.com/" | convert -rotate +90 -resize 320x320! - output.ppm`  
