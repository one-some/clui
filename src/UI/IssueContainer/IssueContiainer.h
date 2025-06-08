#include "UI/Stack/Stack.h"
#include "LSPClient/LSPClient.h"

class IssueContainer : public VStack {
public:
    IssueContainer() {
        printf("Test:\n");
        LSPClient::the().diagnostic_messages.subscribe([](auto messages) {
            printf("HELLLO FROM A CONTAINER...\n");

            for (auto& message : *messages) {
                printf("Haha: %s\n", message.as_c());
            }
        });
    }
};