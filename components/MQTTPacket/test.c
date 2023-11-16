#include "MQTTPacket.h"

int main() {
    unsigned char dup;
    unsigned short packetid;
    int maxcount = 1;
    int count;
    MQTTString topicFilters[1];
    int requestedQoSs[1];
    /* Malformed Packet */
    unsigned char buf[] = {
        0x80, 0x3a, 0xff, 0x01, 0x00, 0x00, 0x01, 0x00, 0xff, 0xff, 0xff, 0xff,
        0x10, 0xe8, 0x0c, 0xff, 0xff, 0xff, 0xff, 0x00, 0x10, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xe3, 0xff, 0xf5, 0xff, 0xe3, 0x0c, 0xff, 0xff, 0xff
    };

    int rc = MQTTDeserialize_subscribe(&dup, &packetid, maxcount, &count, topicFilters,
                              requestedQoSs, buf, sizeof(buf)); 
    printf("rc %d from deserialize\n", rc);
}
