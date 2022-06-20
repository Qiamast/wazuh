#include "stageBuilderNormalize.hpp"

#include <algorithm>
#include <any>

#include "baseTypes.hpp"
#include "builder/expression.hpp"
#include "builder/registry.hpp"
#include "builder/syntax.hpp"
#include "json.hpp"

namespace builder::internals::builders
{

Expression stageNormalizeBuilder(std::any definition)
{
    json::Json jsonDefinition;

    try
    {
        jsonDefinition = std::any_cast<json::Json>(definition);
    }
    catch (std::exception& e)
    {
        throw std::runtime_error(
            "[builders::stageNormalizeBuilder(json)] Received unexpected argument type");
    }

    if (!jsonDefinition.isArray())
    {
        throw std::runtime_error(
            fmt::format("[builders::stageNormalizeBuilder(json)] Invalid json definition "
                        "type: expected [array] but got [{}]",
                        jsonDefinition.typeName()));
    }

    auto blocks = jsonDefinition.getArray();
    std::vector<Expression> blockExpressions;
    std::transform(
        blocks.begin(),
        blocks.end(),
        std::back_inserter(blockExpressions),
        [](auto block)
        {
            if (!block.isObject())
            {
                throw std::runtime_error(
                    fmt::format("[builders::stageNormalizeBuilder(json)] "
                                "Invalid array item type: expected [object] but got "
                                "[{}]",
                                block.typeName()));
            }
            auto blockObj = block.getObject();
            std::vector<Expression> subBlocksExpressions;

            std::transform(blockObj.begin(),
                           blockObj.end(),
                           std::back_inserter(subBlocksExpressions),
                           [](auto& tuple)
                           {
                               auto& [key, value] = tuple;
                               try
                               {
                                   return Registry::getBuilder("stage." + key)(value);
                               }
                               catch (const std::exception& e)
                               {
                                   std::throw_with_nested(std::runtime_error(fmt::format(
                                       "[builders::stageNormalizeBuilder(json)] "
                                       "Exception building stage block [{}]",
                                       key)));
                               }
                           });
            auto expression = And::create("subblock", subBlocksExpressions);
            return expression;
        });
    auto expression = Chain::create("stage.normalize", blockExpressions);
    return expression;
}

} // namespace builder::internals::builders
