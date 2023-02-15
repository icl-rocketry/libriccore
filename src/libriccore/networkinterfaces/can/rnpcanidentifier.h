#pragma once

#include <cstdint>

struct RnpCanIdentifier
{
    //Can Identifier members, the first 3 members make up the can packet unique identifier while the last member is the segmentation id.
    /**
     * @brief Source Address of packet
     * 
     */
    uint8_t source;
    /**
     * @brief Destination Address of packet
     * 
     */
    uint8_t destination;
    /**
     * @brief  Can physial layer packet id NOT the rnp packet id. only 5 bits!
     * 
     * 
     */
    uint8_t can_packet_id; 
    /**
     * @brief Segment ID, 0 indicates the start of a packet. If counter overflows, index starts at 1 not zero
     * 
     */
    uint8_t seg_id;
    
    static constexpr size_t size = 29; // 29bit identifier

    RnpCanIdentifier(const RnpHeader &header,uint8_t can_packet_id) : source(header.source),
                                                destination(header.destination),
                                                can_packet_id(can_packet_id),
                                                seg_id(0)
                                                {};

    RnpCanIdentifier(uint32_t can_id) : source(can_id & 0xFF),
                                        destination((can_id >> 8) & 0xFF),
                                        can_packet_id((can_id >> 16) & 0x1F),
                                        seg_id((can_id >> 21) & 0xFF)
                                        {};

    /**
     * @brief Returns the 29 bit can identifier. Masks the can_packet_id to the first 5 bits!
     * 
     * @return uint32_t 
     */
    uint32_t getIdentifier() const {return source | (destination << 8) | ((can_packet_id & 0x1F) << 16) | (seg_id << 21);};
    /**
     * @brief Extracts the unique packet identifer from the can identifier i.e ignores the segmentation part of the id.
     * 
     * @param can_id 
     * @return uint32_t 
     */
    static uint32_t getCanPacketUID(uint32_t can_id){return can_id & 0x1FFFFF;};
};