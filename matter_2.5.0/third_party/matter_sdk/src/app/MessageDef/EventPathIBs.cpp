/**
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2018 Google LLC.
 *    Copyright (c) 2016-2017 Nest Labs, Inc.
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "EventPathIBs.h"

#include "MessageDefHelper.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#include <app/AppConfig.h>

namespace chip {
namespace app {
#if CHIP_CONFIG_IM_PRETTY_PRINT
CHIP_ERROR EventPathIBs::Parser::PrettyPrint() const
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    TLV::TLVReader reader;

    PRETTY_PRINT("EventPathIBs =");
    PRETTY_PRINT("[");

    // make a copy of the EventPathIBs reader
    reader.Init(mReader);

    while (CHIP_NO_ERROR == (err = reader.Next()))
    {
        VerifyOrReturnError(TLV::AnonymousTag() == reader.GetTag(), CHIP_ERROR_INVALID_TLV_TAG);
        VerifyOrReturnError(TLV::kTLVType_List == reader.GetType(), CHIP_ERROR_WRONG_TLV_TYPE);

        {
            EventPathIB::Parser path;
            ReturnErrorOnFailure(path.Init(reader));

            PRETTY_PRINT_INCDEPTH();
            ReturnErrorOnFailure(path.PrettyPrint());
            PRETTY_PRINT_DECDEPTH();
        }
    }

    PRETTY_PRINT("],");
    PRETTY_PRINT_BLANK_LINE();

    // if we have exhausted this container
    if (CHIP_END_OF_TLV == err)
    {
        err = CHIP_NO_ERROR;
    }
    ReturnErrorOnFailure(err);
    return reader.ExitContainer(mOuterContainerType);
}
#endif // CHIP_CONFIG_IM_PRETTY_PRINT

EventPathIB::Builder & EventPathIBs::Builder::CreatePath()
{
    if (mError == CHIP_NO_ERROR)
    {
        mError = mEventPath.Init(mpWriter);
    }
    return mEventPath;
}

CHIP_ERROR EventPathIBs::Builder::EndOfEventPaths()
{
    EndOfContainer();
    return GetError();
}
}; // namespace app
}; // namespace chip
