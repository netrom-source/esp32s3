#include "editor.h"
#include <string.h>

/**
 * @brief Initialize editor state
 */
void editor_init(editor_t *ed) {
    memset(ed->buffer, 0, sizeof(ed->buffer));
    ed->length = 0;
    ed->cursor = 0;
    ed->hemingway_mode = false;
    ed->dirty = false;
}

void editor_set_hemingway(editor_t *ed, bool enable) {
    ed->hemingway_mode = enable;
}

/**
 * @brief Insert a character into the editor buffer.
 *
 * Handles backspace unless Hemingway mode is enabled.
 */
void editor_insert_char(editor_t *ed, char c) {
    if ((c == '\b' || c == 0x7F)) {
        if (!ed->hemingway_mode && ed->cursor > 0) {
            memmove(&ed->buffer[ed->cursor-1], &ed->buffer[ed->cursor], ed->length - ed->cursor +1);
            ed->cursor--; ed->length--; ed->dirty = true;
        }
        return;
    }

    if (ed->length < EDITOR_BUFFER_SIZE - 1) {
        memmove(&ed->buffer[ed->cursor+1], &ed->buffer[ed->cursor], ed->length - ed->cursor +1);
        ed->buffer[ed->cursor] = c;
        ed->cursor++; ed->length++; ed->dirty = true;
    }
}

const char *editor_get_buffer(editor_t *ed) {
    return ed->buffer;
}

size_t editor_get_length(editor_t *ed) {
    return ed->length;
}

void editor_clear(editor_t *ed) {
    ed->length = 0;
    ed->cursor = 0;
    ed->buffer[0] = '\0';
    ed->dirty = false;
}

void editor_move_cursor(editor_t *ed, int delta) {
    if (delta < 0) {
        size_t d = (size_t)(-delta);
        if (d > ed->cursor) d = ed->cursor;
        ed->cursor -= d;
    } else if (delta > 0) {
        size_t d = (size_t)delta;
        if (ed->cursor + d > ed->length) d = ed->length - ed->cursor;
        ed->cursor += d;
    }
}

void editor_get_cursor(editor_t *ed, size_t *line, size_t *col) {
    size_t l = 1, c = 1;
    for (size_t i = 0; i < ed->cursor; ++i) {
        if (ed->buffer[i] == '\n') { l++; c = 1; }
        else c++;
    }
    if (line) *line = l;
    if (col) *col = c;
}

bool editor_is_dirty(editor_t *ed) {
    return ed->dirty;
}

void editor_mark_saved(editor_t *ed) {
    ed->dirty = false;
}
