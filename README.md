pip-screenshot
==============

Utility for creating a screenshot of an X11 display region, and
overlaying a different region as a picture-in-picture.

## Usage
```
./pip-screenshot --createconfig [CONFIG.info]
./pip-screenshot OUTFILE.svgz  [CONFIG.info]

SVGZ format is highly recommended, but optional. It was found to
use half of CPU time as PNG encoding and roughly the same disk space.

Create a default config file with --createconfig flags. If no
filename is specified, it will default to: 'pip-timelapse-config.info'.

If CONFIG.info file is provided and doesn't exist, it fails.
```

## Compiling
The project can be built with `SCons` and requires `boost` and `Magick++`.

Environment variables `BOOST_DIR` and `IMAGEMAGICK_PATH` can be used to help out.

Compile the project by calling `scons` from the root.


## Config File (default)

A default config file can be created with `pip-screenshot --createconfig`.

The default content:
```
; Configuration file for pip-screenshot utility

monitor {
    ;; This defines monitor sizes and positions
    ;;
    ;; For example assume have two monitors with the following:
    ;; Dimensions for the left display:  1680 x 1050
    ;; Dimensions for the right display: 1920 x 1080
    ;; The left display is also offset by 30 pixels (down)
    ;;
    ;; We want to use the right monitor as the principal one,
    ;; and overlay the left monitor as a PIP
    ;; This gives the following configuration:
    ;;
    principal {
        w 1920
        h 1080
        x 1680
        y 0
    }

    ;; This is the monitor that ends up as a PIP
    secondary {
        w 1680
        h 1050
        x 0
        y 30
    }
}

pip {
    border {
        color "#881111aa"
        width 2
    }

    shadow {
        color "black"
        opacity 90 ; 0-100
        sigma 2.5
        size {
            w 100
            h 100
        }
        offset {
            x 2
            y 2
        }
    }

    ; Max size constraints
    size {
        w 340
        h 340
    }
    ; Position relative to bottom right
    pos {
        x 30
        y 20
    }
}

```