#ifndef PACKET_CAP_H
#define PACKET_CAP_H

#include <QStringList>
#include "pcap.h"

class PacketCap
{
public:
    explicit PacketCap();
    ~PacketCap();

private:
    pcap_if_t* all_devs;
    int adapters_num;
    QStringList adapters_friendly_name_list;

public:
    pcap_t* ad_handle;
    bool selected_adapter_opened_flag;

    QStringList getFriendlyNameListOfAdapters();
    bool openAdapter(int selected_adapter_index);
    void handlePacket(unsigned char* param, const struct pcap_pkthdr* header, const unsigned char* pkt_data);
    bool sendPacket(unsigned char* packet, int packet_size);
};

#endif // PACKET_CAP_H
