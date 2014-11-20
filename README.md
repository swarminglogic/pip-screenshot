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
filename is specified, it will default to: 'pip-screenshot-config.info'.

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

## Helper Scripts

A few utility scripts found in `scripts/` folder:

* `pip-timelapse.sh`: Calls `pip-screenshot` at regular intervals.

  Each frame is numbered and stored in output-folder/frames


* `pip-rendervideo.sh` Converts all frames from `pip-timelapse.sh`
  to a temporary `PPM` format, then uses `ffmpeg` to create an `mp4`
  video from these frames.


* `pip-mkdir.sh`: Creates numerated and date-stamped.

  For example: `./pip-mkdir.sh foo` creates folder `001_2014-11-20_foo`

  Calling it again, creates folder `001_2014-11-20_foo`

  The current number is stored in `.current`


## Timelapse workflow

Together, the above scripts allow the following timelapse creation workflow:

 1. `[Only the first time]`. Create an initial config file

    ```
    pip-screenshot --createconfig
    ```

    And customize it however you want.


 1. Create timelapse output folder `001_2014-11-20_foowork` using

    ```
    ./pip-mkdir.sh foowork
    ```

 1. Start time-lapse recording that captures a frame every 3 seconds:

    ```
    nice ./pip-timelapse.sh 3 001_2014-11-20_foowork
    ```

    Pause it with `ctrl-z`, and unpause with `fg`. Stop with `ctrl-c`

 1. Create a timelapse video from captured frames with:

    ```
    nice ./pip-rendervideo.sh 001_2014-11-20_foowork
    ```

    The video is stored as `001_2014-11-20_foowork/001_2014-11-20_foowork.mp4`

 1. Clean up frames to save space.

    ```
    rm -rf 001_2014-11-20_foowork/frames
    ```

---

`Tip:` You might want to symlink or copy these scripts to somewhere in your
`PATH` variable. For example:
```
     ln -s $(pwd)/scripts/pip-timelapse.sh ~/local/bin/timelapse
```
