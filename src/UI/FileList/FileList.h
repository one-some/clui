#pragma once
#include "UI/Container/Container.h"
#include "UI/TextLabel/TextLabel.h"
#include "UI/Stack/Stack.h"
#include "UI/Button/Button.h"
#include "assert.h"
#include "Claire/String.h"
#include "Claire/Directory.h"
#include "color.h"

class FileList : public VStack {
    public:
        Directory directory;

        FileList(const char* path): directory(path) {
            refresh();
        }

        void refresh() {
            this->children = {};

            printf("Listing for '%s'\n", directory.path);

            for (auto file_name : directory.list()) {
                auto child = new TextLabel(file_name.as_c());
                child->size->strategy_y = SizeStrategy::FORCE;
                child->font_size = 16;
                child->color = Colors::FG;
                this->add_child(child);
            }
        }
};