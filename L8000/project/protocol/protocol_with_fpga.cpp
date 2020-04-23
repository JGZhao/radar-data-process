#include "global_settings.h"
#include "global_functions.h"
#include "protocol_with_fpga.h"

void generateHead(ToFpgaPacket *to_FPGA_packet)
{
    if (to_FPGA_packet)
    {
        to_FPGA_packet->src_addr[0] = 0x5a;
        to_FPGA_packet->src_addr[1] = 0x5a;
        to_FPGA_packet->src_addr[2] = 0x00;
        to_FPGA_packet->src_addr[3] = 0x02;
        to_FPGA_packet->src_addr[4] = 0x06;
        to_FPGA_packet->src_addr[5] = 0x06;
        to_FPGA_packet->dest_addr[0] = 0xff;
        to_FPGA_packet->dest_addr[1] = 0xff;
        to_FPGA_packet->dest_addr[2] = 0xff;
        to_FPGA_packet->dest_addr[3] = 0xff;
        to_FPGA_packet->dest_addr[4] = 0xff;
        to_FPGA_packet->dest_addr[5] = 0xff;
        to_FPGA_packet->packet_type[0] = 0xff;
        to_FPGA_packet->packet_type[1] = 0xff;
        to_FPGA_packet->head1 = 0x5a;
        to_FPGA_packet->head2 = 0x69;
    }
}

void initializeToFpgaPacket(ToFpgaPacket *to_FPGA_packet)
{
    memset(to_FPGA_packet, 0x00, sizeof(ToFpgaPacket));
    generateHead(to_FPGA_packet);
}
