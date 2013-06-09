#include <vterm.h>
#include <string.h>
#include <stdio.h>

static int put_glyph(VTermGlyphInfo *info, VTermPos pos, void *user)
{
    printf("Row: %d\nCol: %d\n", pos.row, pos.col);
    printf("%c\n\n", info->chars[0]);
    return 1;
}

static VTermStateCallbacks state_cbs = {
  .putglyph    = &put_glyph,
};

int jason()
{
    VTerm *vt = vterm_new(80, 24);

    VTermScreen *vts = vterm_obtain_screen(vt);
    vterm_screen_reset(vts, 1);

    VTermState *state = vterm_obtain_state(vt);
    vterm_state_set_callbacks(state, &state_cbs, NULL);

    char str[] = "HEL\bLO!";
    vterm_push_bytes(vt, str, strlen(str));

    VTermPos pos = {
        .row = 0,
        .col = 0
    };

    VTermScreenCell cell;
    vterm_screen_get_cell(vts, pos, &cell);
    return (int)cell.chars[0];
}
