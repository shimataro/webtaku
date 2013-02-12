# Setup Instructions

## install the packages

    # essential packages
    sudo apt-get -y install libqtwebkit-dev libicu-dev

    # fonts
    sudo apt-get -y install ttf-mscorefonts-installer ttf-umefont

    # if following message is displayed (especially Ubuntu 11.10 or later):
    # (wiget:XXXXX): Gtk-WARNING **: Unable to locate theme engine in module_path: "pixmap"
    sudo apt-get -y install gtk2-engines-pixbuf

    # virtual X frame buffer (when X server is not running)
    sudo apt-get -y install xvfb

## build & install

    git clone https://github.com/shimataro/wiget.git
    cd wiget
    qmake CONFIG+=release
    make
    sudo make install

(will be installed in /usr/local/bin and /usr/local/src)

---

# Usage

## basic usage

### dump image data into stdout, and default file format is PPM

    wiget "http://example.com/" >output.ppm

### if scheme is omitted, "http://" will be complemented

    wiget "example.com" >output.ppm

### "--format" to specify file format (BMP|JPG|PNG|PPM|XBM|XPM)

    wiget --format=JPG "http://example.com/" >output.jpg

### "--output" to specify output file
(file format will be determined by extension, and "--format" option will be ignored)

    wiget --output=output.png "http://example.com/"

### when X server is not running, use virtual frame buffer
(use "--output" option because xvfb-run redirects stderr to stdout)

    xvfb-run --server-args="-screen 0, 1024x768x24" wiget --output=output.png "http://example.com/"

### "--min-size" to specify minimum viewport size (default is 1024x768)

    wiget --min-size=1200x900 "http://example.com/" >output.ppm
    wiget --min-size=1200x "http://example.com/" >output.ppm
    wiget --min-size=x900 "http://example.com/" >output.ppm

### "--scaled-size" to resize image as below

    # this will create 300x225 image
    wiget --scaled-size=300x300 "http://example.com/" >output.ppm

    # use upper case "X" to resize by long-side (this will create 400x300 image)
    wiget --scaled-size=300X300 "http://example.com/" >output.ppm

    # omit height to resize by width (300x225)
    wiget --scaled-size=300x "http://example.com/" >output.ppm

    # omit width to resize by height (400x300)
    wiget --scaled-size=x300 "http://example.com/" >output.ppm

### "--crop" to crop image by minimum viewport size

    wiget --crop "http://example.com/" >output.ppm

### "--silent" to output no message

    wiget --silent "http://example.com/" >output.ppm

### "--user-agent" to specify user agent

    # iPhone5 (iOS6)
    wiget --user-agent="Mozilla/5.0 (iPhone; CPU iPhone OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A403 Safari/8536.25" --min-size=1136x640 "http://example.com/" >output.ppm

    # iPad (iOS6)
    wiget --user-agent="Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A403 Safari/8536.25" --min-size=1024x768 "http://example.com/" >output.ppm

## advanced usage

### to create 320x240 thumbnail from 1200x900 viewport

    wiget --min-size=1200x900 --crop --scaled-size=320x240 --output=output.png "http://example.com/"

### for more complex conversion, use ImageMagick as below

    wiget --min-size=1200x900 "http://example.com/" | convert -rotate +90 -resize 320x320! - output.ppm
