/*
 * Copyright 2020, the Pez Globo team and the Pufferfish project contributors
 *
 * Device.cpp
 *
 *  Created on: Nov 26, 2020
 *      Author: Rohan Purohit
 *
 * Unit tests to confirm behavior of flow sensor driver
 *
 */
#include "Pufferfish/Driver/Serial/FDO2/Commands.h"

#include "catch2/catch.hpp"

namespace PF = Pufferfish;

SCENARIO("FDO2: SFM3000 flow meter behaves properly", "[commands]") {

    GIVEN("A Command Sender") {
        constexpr size_t buffer_size = 30UL;
        PF::Driver::Serial::FDO2::CommandSender command_sender{};
        PF::Driver::Serial::FDO2::Request request;
        request.tag = PF::Driver::Serial::FDO2::CommandTypes::vers;

        PF::Util::Vector<char, buffer_size> output_buffer;

        WHEN("A request is sent") {
            auto status = command_sender.transform(request, output_buffer);

            THEN("status should be ok") {
                REQUIRE(status == PF::Driver::Serial::FDO2::CommandSender::Status::ok);
            }
        }
    }

    GIVEN("A command reciever") {
        constexpr size_t buffer_size = 102UL;
        PF::Driver::Serial::FDO2::CommandReceiver command_receiver{};

        PF::Driver::Serial::FDO2::Responses::Vers vers{};
        vers.device_id = 8;
        vers.type = 15;
        vers.num_channels = 1;
        vers.firmware_rev = 341;

        PF::Driver::Serial::FDO2::Response response;
        response.tag = PF::Driver::Serial::FDO2::CommandTypes::vers;
        response.value.vers = vers;

        PF::Util::Vector<char, buffer_size> input_buffer;
        input_buffer.copy_from(PF::Driver::Serial::FDO2::Headers::vers, 0);
        
        WHEN("A response is received") {
            auto status = command_receiver.transform(input_buffer, response);
            REQUIRE(status == PF::Driver::Serial::FDO2::CommandReceiver::Status::ok);
        }
    }
}
