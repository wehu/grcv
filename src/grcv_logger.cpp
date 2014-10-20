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

#include <assert.h>

#include "grcv_logger.h"

namespace grcv
{

    // logger functions
    namespace logger
    {
        void print_(level l, const std::string &msg)
        {
            switch(l)
            {
                case _info  : std::cout << "[GRCV I]: "; break;
                case _debug : std::cout << "[GRCV D]: "; break;
                case _warn  : std::cout << "[GRCV W]: "; break;
                case _error : std::cerr << "[GRCV E]: "; break;
                case _fatal : std::cerr << "[GRCV F]: "; break;
            }
            if (l == _error || l == _fatal)
                std::cerr << msg << std::endl;
            else std::cout << msg << std::endl;
            if (l == _fatal) assert(0);
        }
        void info(const std::string & msg)
        {
            print_(_info, msg);
        }
        void debug(const std::string & msg)
        {
            print_(_debug, msg);
        }
        void warn(const std::string & msg)
        {
            print_(_warn, msg);
        }
        void error(const std::string & msg)
        {
            print_(_error, msg);
        }
        void fatal(const std::string & msg)
        {
            print_(_fatal, msg);
        }
        void assert_(bool c, const std::string & msg)
        {
            if (!c) fatal(msg);
        }
    }

}


