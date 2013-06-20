var INTERVAL_ID;

var DEBUG = 0;

var pos_elems = [];

function canonicalize_data(diff) {
    var newdiff = diff;

    if (newdiff.bo === '0') { newdiff.bo = 0; }

    if (newdiff.v && newdiff.v.length > 0) {
      newdiff.v = String.fromCharCode(newdiff.v[0]);
    }

    return newdiff;
}

var color_map = [
    '#000000', '#ca311c', '#60bc33', '#bebc3a',
    '#1432c8', '#c150be', '#61bdbe', '#c7c7c7',
];

var bold_color_map = [
    '#686868', '#df6f6b', '#70f467', '#fef966',
    '#6d75ea', '#ed73fc', '#73fafd', '#ffffff'
];

var cell_height = 12;
var spacing     = 1.25;
function set_font(context) {
    context.font = Math.floor(cell_height / spacing) + "pt Monaco,'Bitstream Vera Sans Mono',monospace";
    context.textBaseline = 'top';
}

function set_screen_value(screen, col, line, key, value) {
    if (!col) col = '0'; if (!line) line = '0';

    if (typeof(screen)            === 'undefined') screen            = {};
    if (typeof(screen[line])       === 'undefined') { screen[line]       = {}; }
    if (typeof(screen[line][col]) === 'undefined') screen[line][col] = {};

    screen[line][col][key] = value;
}

function get_screen_value(screen, col, line, key) {
    if (typeof(screen)            === 'undefined') return undefined;
    if (typeof(screen[line])       === 'undefined') return undefined;
    if (typeof(screen[line][col]) === 'undefined') return undefined;

    return screen[line][col][key];
}

function init_canvas(canvas, cols, lines) {
    var context = canvas.getContext('2d');

    // get the width of the letter M in our font
    set_font(context);
    var cell_width = context.measureText('M').width;
    canvas.width  = Math.floor(cell_width * cols);
    canvas.height = Math.floor(cell_height * lines * spacing);

    var border_width = 1;
    var new_width = canvas.width + (border_width-1) * 2
    $('#caption').width(new_width);
    $('#terminal').width(new_width);
    $('#caption').text('Viewing: ' + canvas.streamer_username + ' (' + cols + 'x' + lines + ')');
    // ugh, have to set the font again after adjusting the canvas geometry
    set_font(context);
}

function update_canvas(data, context, screen, cols, lines) {
    if (typeof(data) === 'object') {
        for (j = 0; j < data.length; j++) {
            var change = data[j];
            //if (change && change[1] == 0) console.log(change);
            var col  = change[0],
                line  = change[1],
                diff = canonicalize_data(change[2]);

            if (diff) {
                context.fillStyle = bold_color_map[0];
                c_update_cell_value(col, line, context, diff, screen);

                if (diff.disconnect) {
                    window.location =
                        window.location.protocol + '//' + window.location.host;
                }

                if (diff.clear) {
                    // hack to clear canvas
                    context.canvas.width = context.canvas.width;
                    set_font(context);
                }

                if (diff.resize) {
                    cols  = diff.resize[0];
                    lines = diff.resize[1];

                    // set up canvas again (with new geometry)
                    init_canvas(context.canvas, cols, lines);
                }
            }
        }
    }
}

function preserve_or_assign(key, col, line, diff, screen) {
    if (typeof(diff[key]) === 'undefined') {
        if (get_screen_value(screen, col, line, key)) {
            diff[key] = get_screen_value(screen, col, line, key);
        }
    }
    else {
        set_screen_value(screen, col, line, key, diff[key]);
    }
}

function c_update_cell_value(col, line, context, diff, screen) {

    var cell_width = context.measureText('M').width;

    var mod_height = Math.floor(cell_height * spacing);

    // FIXME track previous bg so we aren't clobbering!!
    context.fillStyle = '#000';
    context.fillRect(
        col * cell_width,
        line * mod_height,
        cell_width, mod_height
    );

    c_update_cell_bg(col, line, context, diff, screen);

    context.fillStyle = color_map[7];
    c_update_cell_fg(col, line, context, diff, screen);

    preserve_or_assign('v', col, line, diff, screen);

    context.fillText(diff.v, col * cell_width, line * mod_height);
}

function c_update_cell_bg(col, line, context, diff, screen) {

    preserve_or_assign('bg', col, line, diff, screen);

    var bg_color   = color_map[diff.bg];
    var cell_width = context.measureText('M').width;

    var mod_height = Math.floor(cell_height * spacing);


    if (bg_color) context.fillStyle = bg_color;
    context.fillRect(
        col * cell_width,
        line * mod_height,
        cell_width, mod_height
    );
}

function c_update_cell_fg(col, line, context, diff, screen) {
    //return; // XXX
    var color;

    var map;

    preserve_or_assign('bo', col, line, diff, screen);

    if (diff.bo) {
        map = bold_color_map;
    }
    else {
        map = color_map;
    }

    if (typeof(diff.fg) === 'undefined') {
        fg = get_screen_value(screen, col, line, 'fg');
        if (typeof(fg) === 'undefined') {
            color = map[7];
        }
        else {
            color = map[fg];
            color = "rgb("+(fg & 0x0000ff) + ", " + ((fg & 0x00ff00) >> 8) + ", " + ((fg & 0xff0000) >> 16) + ")";
        }
    }
    else {
        set_screen_value(screen, col, line, 'fg', diff.fg);
            color = "rgb("+(diff.fg & 0x0000ff) + ", " + ((diff.fg & 0x00ff00) >> 8) + ", " + ((diff.fg & 0xff0000) >> 16) + ")";
    }


    if (color) context.fillStyle = color;
}
