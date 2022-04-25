/* Copyright (C) 2015-2021, Wazuh Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#ifndef _OP_BUILDER_CONDITION_REFERENCE_H
#define _OP_BUILDER_CONDITION_REFERENCE_H

#include "builderTypes.hpp"

namespace builder::internals::builders
{

/**
 * @brief Builds condition value operation.
 * Checks that a tuple <field: value> is present in the event.
 *
 * @param def Definition of the operation to be built
 * @return base::Lifter
 */
base::Lifter opBuilderConditionReference(const base::DocumentValue & def, types::TracerFn tr);

} // namespace builder::internals::builders

#endif // _OP_BUILDER_CONDITION_REFERENCE_H