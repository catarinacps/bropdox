#pragma once

#include "util/FileData.hpp"
#include "util/Messages.hpp"

#include <memory>

/******************************************************************************
 * Helper converter functions
 */

namespace bdu {

std::unique_ptr<handshake_t>        convert_to_handshake(byte_t* data);
std::unique_ptr<ack_t>              convert_to_ack(byte_t* data);
std::unique_ptr<syn_t>              convert_to_syn(byte_t* data);
std::unique_ptr<packet_t>           convert_to_packet(byte_t* data);
std::unique_ptr<file_data_t>        convert_to_file_data(byte_t* data);
}