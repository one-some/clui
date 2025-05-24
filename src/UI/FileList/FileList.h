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
            this->children.clear();

            printf("Listing for '%s'\n", directory.path);

            for (auto dir_child : directory.list()) {
                std::unique_ptr<TextLabel> child = std::make_unique<TextLabel>(dir_child.name.as_c());
                child->size->strategy_y = SizeStrategy::FORCE;
                child->font_size = 16;
                child->color = dir_child.type == DirectoryChildType::TYPE_DIRECTORY ? Colors::FG.to_ray() : RayLib::RED;
                this->add_child(std::move(child));
            }
        }
};