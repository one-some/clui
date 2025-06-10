#pragma once

#include "UI/Stack/Stack.h"
#include "LSPClient/LSPClient.h"
#include "UI/TextLabel/TextLabel.h"

class IssueContainer : public VStack {
public:
    IssueContainer() {
        layout_style = StackLayout::STACK;

        auto button = this->create_child<Button>();
        button->size->set_raw({70, 70});
        button->size->strategy_x = FORCE;
        button->size->strategy_y = FORCE;

        LSPClient::the().diagnostic_messages.subscribe([this](auto messages) {
            this->children.clear();

            for (auto& message : *messages) {
                auto label = this->create_child<TextLabel>(message.as_c());
                label->font_size = 16;
                label->color = Colors::FG.to_ray();
            }
        });
    }
};