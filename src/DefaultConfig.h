#ifndef DEFAULTCONFIG_H
#define DEFAULTCONFIG_H

#include <string>
namespace piptimelapse {
const std::string defaultConfig = R"(
; Configuration file for pip-timelapse utility

general {
    delay 3.0 ; time in seconds between each capture
}

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
        offset
        {
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
