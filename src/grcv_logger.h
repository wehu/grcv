/*
* Copyright 2014 huwei04@hotmail.com. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include <string>

namespace grcv
{

    // logger functions
    namespace logger
    {
        enum level
        {
            _info = 0,
            _debug,
            _warn,
            _error,
            _fatal
        };
        void print_(level l, const std::string &msg);
        void info(const std::string & msg);
        void debug(const std::string & msg);
        void warn(const std::string & msg);
        void error(const std::string & msg);
        void fatal(const std::string & msg);
        void assert_(bool c, const std::string & msg);
    }

}


