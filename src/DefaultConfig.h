#ifndef DEFAULTCONFIG_H
#define DEFAULTCONFIG_H

#include <string>
namespace pipfiles {
const std::string helpOutupt =  R"(
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
)";


const std::string defaultConfig = R"(
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
)";
}


#endif  // DEFAULTCONFIG_H
