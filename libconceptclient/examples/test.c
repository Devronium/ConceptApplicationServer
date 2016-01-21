#include <stdlib.h>
#include <stdio.h>

#include "libconceptclient.h"

int callback(void *client, char *Sender, unsigned short Sender_len, int MSG_ID, char *Target, unsigned short Target_len, char *Value, unsigned long Value_len) {
    fprintf(stderr, "HERE: %s %i %s %s\n", Sender, MSG_ID, Target, Value);
    QueueMessage(client, Sender, Sender_len, MSG_ID, Target, Target_len, Value, Value_len);
}

int main() {
    void *client = CreateClient(callback, NULL, "concept://localhost:2662/test.con", 0, -1);
    if (client) {
        fprintf(stderr, "CONNECTED !\n");
        while (1)
            Iterate(client, 10000);
    }
    return 0;
}

