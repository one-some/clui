#pragma once

#include "Claire/String.h"

class TextCoordinate {
public:
    size_t line = 0;
    size_t col = 0;
};

class EditorDiagnostic {
public:
    String message;
    String file_path;
    TextCoordinate range_start;
    TextCoordinate range_end;

    EditorDiagnostic(
        String message,
        String file_path,
        TextCoordinate range_start,
        TextCoordinate range_end
    ) : message(message), file_path(file_path), range_start(range_start), range_end(range_end) { }

    String label() const {
        return file_path + ": " + message;
    }
};
