#include <BBG/TimerQuery.h>
#include "../../Logger.h"
namespace BBG
{
    TimerQuery::TimerQuery() noexcept
    {
        glGenQueries(handles_.size(), handles_.data());
        glQueryCounter(handles_[0], GL_TIMESTAMP);
    }

    TimerQuery::~TimerQuery() noexcept
    {
        glDeleteQueries(handles_.size(), handles_.data());
    }

    float TimerQuery::GetElapsedTimeInMicroSec() const
    {
        glQueryCounter(handles_[1], GL_TIMESTAMP);

        uint64_t startTime, endTime;
        glGetQueryObjectui64v(handles_[0], GL_QUERY_RESULT, &startTime);
        glGetQueryObjectui64v(handles_[1], GL_QUERY_RESULT, &endTime);


        return (endTime - startTime) / 1000.0f;
    }
}

