/*
 * Serial.tpp
 *
 *  Created on: May 16, 2020
 *      Author: Ethan Li
 */

#include "Pufferfish/Driver/Serial/FDO2/Commands.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <sstream>

namespace FDO2 = Pufferfish::Driver::Serial::FDO2;

// This macro is used to call the parser for a specified response type with an associated
// union field and enum value. We use a macro because it makes the code more maintainable here,
// while allowing us to ensure union tagging.
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define RESPONSE_PARSE_TAGGED(type, field, input_buffer, output_response) \
  if (std::equal(Headers::field.begin(), Headers::field.end(), (input_buffer).buffer())) {\
    type response{};\
    parse(input_buffer, response);\
    (output_response).set(response);\
    return Status::ok;\
  }
// clang-format on

// This macro is used to add a setter for a specified response type with an associated
// union field and enum value. We use a macro because it makes the code more maintainable here,
// while allowing us to ensure union tagging.
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define RESPONSE_TAGGED_SETTER(type, field) \
    template <>\
    template <>\
    void Util::TaggedUnion<FDO2::Responses::Union, FDO2::CommandTypes>::set<type>(\
        const type &new_value) {\
      tag = FDO2::CommandTypes::field;\
      value.field = new_value; } // NOLINT(cppcoreguidelines-pro-type-union-access)
// clang-format on

// This macro is used to call the writer for a specified request type with an associated
// union field and enum value. We use a macro because it makes the code more maintainable here,
// while allowing us to ensure union tagging.
// clang-format off
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define REQUEST_WRITE_TAGGED(field, input_request, output_buffer) \
  if ((input_request).tag == CommandTypes::field) {\
    write((input_request).value.field, output_buffer); return Status::ok; } // NOLINT(cppcoreguidelines-pro-type-union-access)
// clang-format on

namespace Pufferfish::Util {

RESPONSE_TAGGED_SETTER(FDO2::Responses::Vers, vers)
RESPONSE_TAGGED_SETTER(FDO2::Responses::Mraw, mraw)
RESPONSE_TAGGED_SETTER(FDO2::Responses::Logo, logo)
RESPONSE_TAGGED_SETTER(FDO2::Responses::Bcst, bcst)
RESPONSE_TAGGED_SETTER(FDO2::Responses::Erro, erro)

}  // namespace Pufferfish::Util

namespace Pufferfish::Driver::Serial::FDO2 {

namespace Responses {

// Vers

template <>
ParseStatus parse<Vers>(const ChunkBuffer &input_buffer, Vers &response) {
  const char *end = input_buffer.buffer() + input_buffer.size() - 1;
  const char *parse_start = input_buffer.buffer() + Headers::length;
  char *parse_end = nullptr;
  if (*parse_start != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // device_id
  parse_start = parse_start + 1;
  parse_end = nullptr;
  response.device_id = std::strtoul(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // num_channels
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.num_channels = std::strtoul(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // firmware_rev
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.firmware_rev = std::strtoul(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // type
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.type = std::strtoul(parse_start, &parse_end, arg_base);
  if (parse_end != end) {
    return ParseStatus::unexpected_arg;
  }

  return ParseStatus::ok;
}

bool operator==(const Vers &left, const Vers &right) {
  return left.device_id == right.device_id && left.num_channels == right.num_channels &&
         left.firmware_rev == right.firmware_rev && left.type == right.type;
}

// Mraw

template <>
ParseStatus parse<Mraw>(const ChunkBuffer &input_buffer, Mraw &response) {
  const char *end = input_buffer.buffer() + input_buffer.size() - 1;
  const char *parse_start = input_buffer.buffer() + Headers::length;
  char *parse_end = nullptr;
  if (*parse_start != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // po2
  parse_start = parse_start + 1;
  parse_end = nullptr;
  response.po2 = std::strtol(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // temperature
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.temperature = std::strtol(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // status
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.status = std::strtoul(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // phase_shift
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.phase_shift = std::strtol(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // signal_intensity
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.signal_intensity = std::strtol(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // ambient_light
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.ambient_light = std::strtol(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // ambient_pressure
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.ambient_pressure = std::strtol(parse_start, &parse_end, arg_base);
  if (parse_end >= end) {
    return ParseStatus::missing_arg;
  }

  if (*parse_end != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // relative_humidity
  parse_start = parse_end + 1;
  parse_end = nullptr;
  response.relative_humidity = std::strtol(parse_start, &parse_end, arg_base);
  if (parse_end != end) {
    return ParseStatus::unexpected_arg;
  }

  return ParseStatus::ok;
}

// Logo

template <>
ParseStatus parse<Logo>(const ChunkBuffer &input_buffer, Logo & /*response*/) {
  const char *end = input_buffer.buffer() + input_buffer.size() - 1;
  const char *parse_start = input_buffer.buffer() + Headers::length;
  const char *parse_end = parse_start + 1;
  if (parse_end != end) {
    return ParseStatus::unexpected_arg;
  }

  return ParseStatus::ok;
}

// Bcst

template <>
ParseStatus parse<Bcst>(const ChunkBuffer &input_buffer, Bcst &response) {
  const char *end = input_buffer.buffer() + input_buffer.size() - 1;
  const char *parse_start = input_buffer.buffer() + Headers::length;
  char *parse_end = nullptr;
  if (*parse_start != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // interval
  parse_start = parse_start + 1;
  parse_end = nullptr;
  response.interval = std::strtoul(parse_start, &parse_end, arg_base);
  if (parse_end != end) {
    return ParseStatus::unexpected_arg;
  }

  return ParseStatus::ok;
}

bool operator==(const Bcst &left, const Bcst &right) {
  return left.interval == right.interval;
}

// Erro

template <>
ParseStatus parse<Erro>(const ChunkBuffer &input_buffer, Erro &response) {
  const char *end = input_buffer.buffer() + input_buffer.size() - 1;
  const char *parse_start = input_buffer.buffer() + Headers::length;
  char *parse_end = nullptr;
  if (*parse_start != arg_delimiter) {
    return ParseStatus::invalid_arg_delimiter;
  }

  // code
  parse_start = parse_start + 1;
  parse_end = nullptr;
  response.code = std::strtol(parse_start, &parse_end, arg_base);
  if (parse_end != end) {
    return ParseStatus::unexpected_arg;
  }

  return ParseStatus::ok;
}

}  // namespace Responses

namespace Requests {

template <>
void write<Vers>(const Vers & /*request*/, ChunkBuffer &output_buffer) {
  output_buffer.clear();
  output_buffer.copy_from(Headers::vers);
}

template <>
void write<Logo>(const Logo & /*request*/, ChunkBuffer &output_buffer) {
  output_buffer.clear();
  output_buffer.copy_from(Headers::logo);
}

template <>
void write<Bcst>(const Bcst &request, ChunkBuffer &output_buffer) {
  output_buffer.clear();
  output_buffer.copy_from(Headers::bcst);
  output_buffer.push_back(arg_delimiter);
  // We use a stream to avoid using snprintf, which Clang-tidy complains about due to its
  // use of varargs. Hopefully the way we're using streams here isn't causing dynamic
  // memory allocation.
  std::ostringstream stream;
  stream.rdbuf()->pubsetbuf(
      output_buffer.buffer() + output_buffer.size(),
      ChunkBuffer::max_size() - output_buffer.size());
  stream << request.interval;
  if (output_buffer.resize(output_buffer.size() + stream.tellp()) != IndexStatus::ok) {
    // TODO(lietk12): return error
    return;
  }
}

}  // namespace Requests

// CommandReceiver

CommandReceiver::Status CommandReceiver::transform(
    const Responses::ChunkBuffer &input_buffer, Response &output_response) {
  RESPONSE_PARSE_TAGGED(Responses::Vers, vers, input_buffer, output_response);
  RESPONSE_PARSE_TAGGED(Responses::Mraw, mraw, input_buffer, output_response);
  RESPONSE_PARSE_TAGGED(Responses::Logo, logo, input_buffer, output_response);
  RESPONSE_PARSE_TAGGED(Responses::Bcst, bcst, input_buffer, output_response);
  RESPONSE_PARSE_TAGGED(Responses::Erro, erro, input_buffer, output_response);

  return Status::invalid_header;
}

// CommandSender

CommandSender::Status CommandSender::transform(
    const Request &input_request, Requests::ChunkBuffer &output_buffer) {
  REQUEST_WRITE_TAGGED(vers, input_request, output_buffer);
  REQUEST_WRITE_TAGGED(logo, input_request, output_buffer);
  REQUEST_WRITE_TAGGED(bcst, input_request, output_buffer);

  return Status::invalid_command;
}

}  // namespace Pufferfish::Driver::Serial::FDO2
