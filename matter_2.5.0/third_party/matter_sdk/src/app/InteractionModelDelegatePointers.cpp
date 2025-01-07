/*
 *
 *    Copyright (c) 2024 Project CHIP Authors
 *    All rights reserved.
 *
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
#include "InteractionModelDelegatePointers.h"

#if CHIP_CONFIG_STATIC_GLOBAL_INTERACTION_MODEL_ENGINE

// TODO: It would be nice to not need to couple the pointers class
//       to the global interaction model engine
#include "InteractionModelEngine.h"

namespace chip {

template <>
app::TimedHandlerDelegate * GlobalInstanceProvider<app::TimedHandlerDelegate>::InstancePointer()
{
    return app::InteractionModelEngine::GetInstance();
}

template <>
app::WriteHandlerDelegate * GlobalInstanceProvider<app::WriteHandlerDelegate>::InstancePointer()
{
    return app::InteractionModelEngine::GetInstance();
}

} // namespace chip

#endif
