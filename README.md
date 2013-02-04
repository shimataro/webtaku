# Setup Instructions

## install the required packages
`sudo apt-get install -y xvfb libqtwebkit-dev libicu-dev`

## install MS fonts and Japanese fonts
`sudo apt-get install -y ttf-mscorefonts-installer ttf-umefont`

## make
`git clone https://github.com/shimataro/wiget.git`  
`cd wiget`  
`qmake`  
`make`  

---

# Usage

## create image

### xvfb is only necessary for running wiget on a headless server (i.e. a server without a GUI or even a graphics card)

`xvfb-run --server-args="-screen 0, 1024x768x24" ./wiget --format=PNG "http://example.com/" >output.png`

### if "--output" option is specified, file format is determined by extension of output file ("--format" option is ignored)

`./wiget --output=output.png "http://example.com/"`

### if "--format" and "--output" options are omitted, file format will be "PPM"

`./wiget "http://example.com/" >output.ppm`

### you can resize and crop by ImageMagick as below

`./wiget "http://example.com/" | convert -resize 320 -crop 320x240+0+0 - thumb.png`
