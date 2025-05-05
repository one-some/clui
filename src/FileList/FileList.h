#pragma once
#include "UI/Container/Container.h"
#include "UI/Stack/Stack.h"
#include "UI/Button/Button.h"
#include "assert.h"
#include "Claire/String.h"

class FileList : public VStack {
    public:
        String directory = ".";

        FileList() {
        }
};