#pragma once
#include "UI/Container/Container.h"
#include "UI/TextLabel/TextLabel.h"
#include "UI/Stack/Stack.h"
#include "UI/Button/Button.h"
#include "Claire/String.h"
#include "Claire/Directory.h"
#include "Claire/Path.h"
#include "color.h"

class DirectoryLabelButton : public Button {
    public:
        bool open = false;
};

class FileList : public VStack {
public:
    constexpr static const float OPACITY = 0.5;
    Directory directory;

    FileList(const char* path): directory(path) {
        layout_style = StackLayout::STACK;
        refresh();
    }

    void sprawl(Directory dir, Container* container, int depth) {
        printf("Listing for '%s'\n", dir.path.as_c());

        std::vector<DirectoryChild> directory_children = dir.list();
        std::sort(directory_children.begin(), directory_children.end(), DirectoryChild::compare);

        for (auto& dir_child : directory_children) {
            auto cont = container->create_child<VStack>();
            cont->layout_style = StackLayout::STACK;
            cont->size->strategy_y = SizeStrategy::FLIMSY;

            auto button = cont->create_child<DirectoryLabelButton>();
            auto label = button->create_child<TextLabel>(dir_child.name.as_c());

            if (dir_child.type == DirectoryChildType::TYPE_DIRECTORY) {
                label->text = "| " + label->text;
            }

            // label->color = dir_child.type == DirectoryChildType::TYPE_DIRECTORY ? Colors::FG.to_ray() : RayLib::RED;
            label->color = RayLib::Fade(Colors::FG.to_ray(), OPACITY);

            label->font_size = 16;
            button->size->strategy_y = SizeStrategy::FORCE;
            button->size->set_raw(label->text_bounds());

            cont->size->set_raw(label->text_bounds());

            label->position->set_x(16 * depth);

            String new_path = Path::join(dir.path, dir_child.name);
            button->callback_on_click = [this, label, button, new_path, dir_child, cont, depth]{
                if (dir_child.type == DirectoryChildType::TYPE_FILE) {
                    EditorActions::open_file_in_new_tab(new_path);
                } else if (dir_child.type == DirectoryChildType::TYPE_DIRECTORY) {
                    if (!button->open) {
                        sprawl({ new_path }, cont, depth + 1);
                    } else {
                        // Yucky
                        for (auto& child : cont->visible_children()) {
                            if (child == button) continue;
                            cont->remove_child(child);
                        }
                    }

                    button->open = !button->open;

                    label->text = (button->open ? "\\ " : "| ") + dir_child.name;
                    label->color = RayLib::Fade(button->open ? Colors::ERROR.to_ray() : Colors::FG.to_ray(), OPACITY);
                }
            };
        }
    }

    void refresh() {
        children.clear();
        sprawl(directory, this, 0);
    }
};