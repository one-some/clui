#pragma once
#include "UI/Container/Container.h"
#include "UI/TextLabel/TextLabel.h"
#include "UI/Stack/Stack.h"
#include "UI/Button/Button.h"
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

            printf("Listing for '%s'\n", directory.path.as_c());

            for (auto dir_child : directory.list()) {
                auto button = create_child<Button>();
                auto label = button->create_child<TextLabel>(dir_child.name.as_c());

                label->color = dir_child.type == DirectoryChildType::TYPE_DIRECTORY ? Colors::FG.to_ray() : RayLib::RED;

                label->font_size = 16;
                button->size->strategy_y = SizeStrategy::FORCE;
                button->size->set_raw(label->text_bounds());

                // TODO: WOW THIS NEEDS TO BE AWESOMER
                String path;
                if (directory.path == "./") {
                    printf("It is: %s\n", dir_child.name.as_c());
                    path = dir_child.name;
                } else {
                    printf("It ain't: %s\n", dir_child.name.as_c());
                    path = directory.path;
                    path.append("/");
                    path.append(dir_child.name.as_c());
                }

                button->callback_on_click = [dir_child, path]{
                    if (dir_child.type != DirectoryChildType::TYPE_FILE) return;
                    EditorActions::open_file_in_new_tab(path.as_c());
                };
            }
        }
};