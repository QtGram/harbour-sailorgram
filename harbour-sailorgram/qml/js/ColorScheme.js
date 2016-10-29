.pragma library

function reverseColor(color, light) {
    if(light === undefined)
        light = 3.0;

    return Qt.lighter(Qt.rgba(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, 1.0), light);
}
