#pragma once

#include <stdbool.h>
#include <stddef.h>

#define EDITOR_BUFFER_SIZE 4096

/**
 * @brief Holds editor state and configuration.
 */
typedef struct {
    char buffer[EDITOR_BUFFER_SIZE]; /**< Text buffer */
    size_t length;                  /**< Current length of text */
    bool hemingway_mode;            /**< When true, backspace is disabled */
} editor_t;

void editor_init(editor_t *ed);
void editor_set_hemingway(editor_t *ed, bool enable);
void editor_insert_char(editor_t *ed, char c);
const char *editor_get_buffer(editor_t *ed);
size_t editor_get_length(editor_t *ed);
void editor_clear(editor_t *ed);