#include "editor.h"
#include <string.h>

/**
 * @brief Initialize editor state
 */
void editor_init(editor_t *ed) {
    memset(ed->buffer, 0, sizeof(ed->buffer));
    ed->length = 0;
    ed->hemingway_mode = false;
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
        // Handle backspace
        if (!ed->hemingway_mode && ed->length > 0) {
            ed->length--;
            ed->buffer[ed->length] = '\0';
        }
        return;
    }

    if (ed->length < EDITOR_BUFFER_SIZE - 1) {
        ed->buffer[ed->length++] = c;
        ed->buffer[ed->length] = '\0';
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
    ed->buffer[0] = '\0';
}
