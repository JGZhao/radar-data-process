#include <QDebug>
#include "packet_cap.h"
#include "get_interface_friendly_name.h"

PacketCap::PacketCap()
{
    all_devs = nullptr;
    ad_handle = nullptr;
    adapters_num = 0;
    adapters_friendly_name_list.clear();
    selected_adapter_opened_flag = false;
}

PacketCap::~PacketCap()
{
    if (all_devs)
    {
        pcap_freealldevs(all_devs);
        all_devs = nullptr;
    }
}

QStringList PacketCap::getFriendlyNameListOfAdapters()
{
    // 初始化
    pcap_if_t* d = nullptr;
    adapters_friendly_name_list.clear();
    int adapters_count = 0;
    char errbuf[PCAP_ERRBUF_SIZE];
    if (all_devs)
    {
        pcap_freealldevs(all_devs);
        all_devs = nullptr;
    }

    // 获取本地网卡列表
    if (pcap_findalldevs(&all_devs, errbuf) == 1)
    {
        qDebug() << "获取网卡失败: " << errbuf;
    }
    else
    {
        const char* guid_text;
        GUID guid;
        QString friendly_name;
        // 转换网卡名称为 friendly_name
        for (d = all_devs; d != nullptr; d = d->next)
        {
            ++adapters_count;
            if (strncmp("\\Device\\NPF_", d->name, 12) == 0)
            {
                guid_text = d->name + 12;
            }
            else
            {
                guid_text = d->name;
            }
            if (parse_as_guid(guid_text, &guid))
            {
                friendly_name = get_interface_friendly_name_from_device_guid(&guid);
                adapters_friendly_name_list.append(friendly_name);
            }
        }

        if (adapters_count == 0)
        {
            qDebug() << "No adapters found!";
        }
        else
        {
            adapters_num = adapters_count;
            qDebug() << adapters_friendly_name_list;
        }

        pcap_freealldevs(all_devs);
        all_devs = nullptr;
    }
    return adapters_friendly_name_list;
}

bool PacketCap::openAdapter(int selected_adapter_index)
{
    selected_adapter_opened_flag = false;
    if (!all_devs)
    {
        char errbuf[PCAP_ERRBUF_SIZE];
        // 获取本地网卡列表
        if (pcap_findalldevs(&all_devs, errbuf) == 1)
        {
            qDebug() << "[openAdapter] 获取网卡失败: " << errbuf;
        }
    }
    pcap_if_t* d = nullptr;
    char errbuf[PCAP_ERRBUF_SIZE];

    int ii = 0;
    // 转到选中的网卡
    for (d = all_devs, ii = 0; ii < selected_adapter_index; d = d->next, ++ii)
        ;

    if ((ad_handle = pcap_open(d->name,                   // name of the device
                               65536,                     // portion of the packet to capture
                                                          // 65536 guarantees that the whole packet will be captured on all the link layers
                               PCAP_OPENFLAG_PROMISCUOUS, // promiscuous mode
                               1000,                      // read timeout
                               nullptr,                   // authentication on the remote machine
                               errbuf                     // error buffer
                               )))
    {
        qDebug() << "[openAdapter]: 打开网口成功! selected_adapter_index = " << selected_adapter_index;
        selected_adapter_opened_flag = true;
    }

    pcap_freealldevs(all_devs);
    all_devs = nullptr;
    return selected_adapter_opened_flag;
}

void PacketCap::handlePacket(unsigned char* param, const struct pcap_pkthdr* header, const unsigned char* pkt_data)
{
    struct tm ltime;
    char timestr[16];
    time_t local_tv_sec;

    (VOID)(param);
    (VOID)(pkt_data);

    /* convert the timestamp to readable format */
    local_tv_sec = header->ts.tv_sec;
    localtime_s(&ltime, &local_tv_sec);
    strftime(timestr, sizeof timestr, "%H:%M:%S", &ltime);
    qDebug() << timestr << "M:" << header->ts.tv_usec << "len:" << header->len;
}

bool PacketCap::sendPacket(unsigned char* packet, int packet_size)
{
    if (selected_adapter_opened_flag)
    {
        if (pcap_sendpacket(ad_handle, packet, packet_size) == 0)
        {
            //            qDebug() << "Pcap: 发送成功! ";
            return true;
        }
    }
    qDebug() << "Pcap: 发送失败! ";
    return false;
}
