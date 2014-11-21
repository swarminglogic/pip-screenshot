pip-screenshot
==============

Utility for creating a screenshot of an X11 display region, and
overlaying a different region as a picture-in-picture.


## Example

PiP on the lower right is the content of the secondary montior:

 [![Test](/example-thumb.png)](/example.png?raw=true)


## Documentation
```
 Usage:
 ./pip-screenshot --help
 ./pip-screenshot --createconfig [CONFIG.info]
 ./pip-screenshot OUTFILE.FMT [CONFIG.info]

 Use --createconfig flag to create a default config file. If no
 filename is specified, it will default to: 'pip-screenshot-config.info'.
 If CONFIG.info file is provided and doesn't exist, it fails.

 FMT extension determines which image format is used for the output.
 Whether or not this works depends on your ImageMagick implementation,
 To see the list of available formats: `convert -list formats`.

 Which format is best suited depends on your priority: CPU usage,
 file size, and/or image quality. Uncompressed formats require
 very little processing, but take up a large amount of space.

 Lossless formats are usually preferable for screenshots, as desktop
 graphics contain a lot of high contrasting areas. Suggested lossless
 formats are:

 |--------+------+------+------+------+----------+-----------|
 | Format | Avg. | Enc. | Dec. | Rel. | Rel. enc | Rel. dec. |
 |        | Size | time | time | size |  speedup |   speedup |
 |--------+------+------+------+------+----------+-----------|
 | ppm    | 6076 | 0.76 | 0.76 | 14.0 |      7.5 |       2.3 |
 | sgi    | 1276 | 0.97 | 0.94 |  2.9 |      5.8 |       1.9 |
 | pcx    | 1350 | 1.38 | 1.97 |  3.1 |      4.1 |       0.9 |
 | pict   | 1249 | 1.50 | 2.40 |  2.9 |      3.8 |       0.7 |
 | svgz   |  544 | 2.40 | 1.15 |  1.3 |      2.4 |       1.5 |
 | png    |  435 | 5.64 | 1.77 |  1.0 |      1.0 |       1.0 |
 |--------+------+------+------+------+----------+-----------|

 The table above shows average frame sizes (in KiB), decoding and
 encoding times (in s). Not surprisingly, the better the file-size
 compression, the more cpu-expensive the process is. Relative
 factors are computed against PNG.

 Of these, ffmpeg supports PPM, SGI and PNG.

 The SGI format stands out significantly, in that it is quick to
 encode/decode, as well as give a good filesize reduction.

 If CPU usage is most important, and disk usage doesn't matter,
 use PPM (or any other uncompressed format, e.g BMP)

 If CPU usage doesn't matter, and filesize really does, use PNG.

 Mainted at: https://github.com/swarminglogic/pip-screenshot
 Author:     Roald Fernandez (github@swarminglogic.com)
 Version:    0.1.0 (2014-11-20)
 License:    MIT-license (see LICENSE file)

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
        sigma 5.0

        ; Shadow position [-1.0 .. 1.0] (clamped)
        ;    -1.0 -> shadow in top / left
        ;     1.0 -> shadow in bottom / right
        ;     0.0 -> shadow centered
        pos {
            x 0.5
            y 0.5
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


* `pip-rendervideo.sh` Uses `ffmpeg` to create an `mp4`
   video from the frames created by `pip-timelapse.sh`.


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
    nice ./pip-timelapse.sh 001_2014-11-20_foowork 3
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
