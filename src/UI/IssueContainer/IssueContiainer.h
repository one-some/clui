#pragma once

#include "UI/Stack/Stack.h"
#include "LSPClient/LSPClient.h"
#include "UI/TextLabel/TextLabel.h"

class IssueContainer : public VStack {
public:
    IssueContainer() {
        size->strategy_x = SizeStrategy::EXPAND_TO_FILL;

        layout_style = StackLayout::STACK;
        allow_scroll = true;

        auto button = this->create_child<Button>();
        button->size->set_raw({70, 70});
        button->size->strategy_x = SizeStrategy::FORCE;
        button->size->strategy_y = SizeStrategy::FORCE;

        LSPClient::the().diagnostic_messages.subscribe([this](auto messages) {

            this->children.clear();

            for (auto& message : *messages) {
                auto label = this->create_child<TextLabel>(message.as_c());
                printf("%d | %d, %d\n", this->parent, label->size->get().x, label->size->get().y);
                label->font_size = 16;
                label->color = Colors::FG.to_ray();
            }
        });
    }
};