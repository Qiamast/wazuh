/* Copyright (C) 2015-2021, Wazuh Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#include "testUtils.hpp"
#include <gtest/gtest.h>

#include "assetBuilderDecoder.hpp"

#include "combinatorBuilderChain.hpp"
#include "combinatorBuilderBroadcast.hpp"
#include "opBuilderCondition.hpp"
#include "opBuilderHelperFilter.hpp"
#include "stageBuilderCheck.hpp"
#include "stageParse.hpp"

#include "opBuilderMap.hpp"
#include "opBuilderMapReference.hpp"
#include "opBuilderMapValue.hpp"
#include "stageBuilderNormalize.hpp"

using namespace base;
namespace bld = builder::internals::builders;

using FakeTrFn = std::function<void(std::string)>;
static FakeTrFn tr = [](std::string msg) {
};

TEST(AssetBuilderDecoder, BuildsAllNonRegistered)
{
    Document doc {R"({
        "name": "test",
        "check": [
            {"field": "value"}
        ],
        "normalize":
        [
            {
                "map":
                {
                    "mapped.field": "$field"
                }
            }
        ]
    })"};

    ASSERT_THROW(bld::assetBuilderDecoder(doc),
                 std::_Nested_exception<std::runtime_error>);
}

TEST(AssetBuilderDecoder, Builds)
{
    BuilderVariant c = bld::opBuilderMapValue;
    Registry::registerBuilder("map.value", c);
    c = bld::opBuilderMapReference;
    Registry::registerBuilder("map.reference", c);
    c = bld::opBuilderMap;
    Registry::registerBuilder("map", c);
    c = bld::combinatorBuilderChain;
    Registry::registerBuilder("combinator.chain", c);
    c = bld::stageBuilderNormalize;
    Registry::registerBuilder("normalize", c);

    c = bld::opBuilderHelperExists;
    Registry::registerBuilder("helper.exists", c);
    c = bld::opBuilderHelperNotExists;
    Registry::registerBuilder("helper.not_exists", c);
    c = bld::middleBuilderCondition;
    Registry::registerBuilder("middle.condition", c);
    c = bld::opBuilderCondition;
    Registry::registerBuilder("condition", c);
    c = bld::stageBuilderCheck;
    Registry::registerBuilder("check", c);
    Document doc {R"({
        "name": "test",
        "check": [
            {"field": "value"}
        ],
        "normalize":
        [
            {
                "map":
                {
                    "mapped.field": "$field"
                }
            }
        ]
    })"};

    bld::assetBuilderDecoder(doc);
}

TEST(AssetBuilderDecoder, BuildsOperates)
{
    BuilderVariant c = bld::stageBuilderParse;
    Registry::registerBuilder("parse", c);
    Registry::registerBuilder("combinator.broadcast", bld::combinatorBuilderBroadcast);

    Document doc {R"({
        "name": "test",
        "check": [
            {"field": "value"}
        ],
        "parse":
        {
            "logql":[
                {
                    "field": "<user.name>"
                }
            ]
        },
        "normalize":
        [
            {
                "map":
                {
                    "mapped.name": "$user.name"
                }
            }
        ]
    })"};
    auto dec = bld::assetBuilderDecoder(doc);

    Observable input = observable<>::create<Event>(
        [=](auto s)
        {
            // TODO: fix json interface to not throw exception
            s.on_next(createSharedEvent(R"({
                "field1": "value",
                "field2": 2,
                "field3": "value",
                "field4": true,
                "field5": "+exists"
            })"));
            s.on_next(createSharedEvent(R"(
                {"field":"value"}
            )"));
            s.on_next(createSharedEvent(R"({
                "field":"value",
                "field1": "value",
                "field2": 2,
                "field3": "value",
                "field4": true,
                "field5": "+exists",
                "field6": "+exists"
            })"));
            s.on_next(createSharedEvent(R"(
                {"otherfield":1}
            )"));
            s.on_completed();
        });

    Observable output = dec.connect(input);

    vector<Event> expected;
    output.subscribe([&](Event e) { expected.push_back(e); });
    ASSERT_EQ(expected.size(), 2);
    for (auto e : expected)
    {
        ASSERT_STREQ(e->getEvent()->get("/mapped/name").GetString(), "value");
    }
}
