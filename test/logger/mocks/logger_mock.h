#pragma once

#include "gmock/gmock.h"
#include "logger/error_type.h"
#include "logger/interfaces/i_logger.h"
#include "state/interfaces/i_command_taker.h"
#include "state/utilities.h"

#include <cstdint>
#include <ostream>
#include <string>

using namespace std;
using namespace logger;
using namespace state;

class LoggerMock : public ILogger {
  public:
	MOCK_METHOD0(LogState, void());
	MOCK_METHOD2(LogInstructionCount, void(PlayerId, int64_t));
	MOCK_METHOD3(LogError, void(PlayerId, ErrorType, string));
	MOCK_METHOD0(LogFinalGameParams, void());
	MOCK_METHOD1(WriteGame, void(std::ostream &));
};
