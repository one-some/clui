#pragma once
#include "UI/Container/Container.h"
#include "UI/TextLabel/TextLabel.h"
#include "UI/Stack/Stack.h"
#include "UI/Button/Button.h"
#include "Claire/String.h"
#include "Claire/Directory.h"
#include "Claire/Path.h"
#include "color.h"

class FileList : public VStack {
    public:
        Directory directory;

        FileList(const char* path): directory(path) {
            refresh();
        }

        void sprawl(Directory dir, Container* container, int depth) {
            printf("Listing for '%s'\n", dir.path.as_c());

            std::vector<DirectoryChild> directory_children = dir.list();
            std::sort(directory_children.begin(), directory_children.end(), DirectoryChild::compare);

            for (auto& dir_child : directory_children) {
                auto cont = container->create_child<VStack>();
                auto button = cont->create_child<Button>();
                auto label = button->create_child<TextLabel>(dir_child.name.as_c());

                label->color = dir_child.type == DirectoryChildType::TYPE_DIRECTORY ? Colors::FG.to_ray() : RayLib::RED;

                label->font_size = 16;
                button->size->strategy_y = SizeStrategy::FORCE;
                button->size->set_raw(label->text_bounds());

                cont->size->strategy_y = SizeStrategy::FLIMSY;
                cont->size->set_raw(label->text_bounds());

                label->position->set_x(16 * depth);

                button->callback_on_click = [this, dir, dir_child, cont, depth]{
                    String new_path = Path::join(dir.path, dir_child.name);

                    if (dir_child.type == DirectoryChildType::TYPE_FILE) {
                        EditorActions::open_file_in_new_tab(new_path.as_c());
                    } else if (dir_child.type == DirectoryChildType::TYPE_DIRECTORY) {
                        sprawl({ new_path }, cont, depth + 1);
                        // cont->size->set_y(cont->children.size() * 16);
                    }
                };
            }
        }

        void refresh() {
            children.clear();
            sprawl(directory, this, 0);
        }
};